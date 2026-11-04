# DataCopy (GM -> UB Multi-dimensional Data Movement NDDMA)<a name="ZH-CN_TOPIC_0000002544407954"></a>

<!-- md-trans-meta sourceCommit=754fb95859dead826727a591e674261c6a829a08 translatedAt=2026-10-26T09:38:35.740Z pushedAt=2026-10-28T03:14:39.494Z nl=2 -->

## Applicable Products<a name="section1550532418810"></a>

<!-- npu="950" id9 -->
- Ascend 950PR&950DT products: supported
<!-- end id9 -->
<!-- npu="A3" id10 -->
- Atlas A3 products: not supported
<!-- end id10 -->
<!-- npu="910b" id11 -->
- Atlas A2 products: not supported
<!-- end id11 -->
<!-- npu="310b" id12 -->
- Atlas 200I/500 A2 inference products: not supported
<!-- end id12 -->
<!-- npu="310p" id13 -->
- Atlas inference products AI Core: not supported
<!-- end id13 -->
<!-- npu="310p" id14 -->
- Atlas inference products Vector Core: not supported
<!-- end id14 -->
<!-- npu="910" id15 -->
- Atlas training products: not supported
<!-- end id15 -->
<!-- @ref: asc-devkit/res/docs/zh/api/SIMD-API/basic_api/memory_vector_compute/data_move/DataCopy_GMToUB_NDDMA_res.md#id1 -->

## Description<a name="section474617392321"></a>

The header file path is `"basic_api/kernel_operator_data_copy_intf.h"`.

A multi-dimensional data movement API. Compared with the basic data movement API, it allows more flexible configuration of the dimension information to be moved in and the corresponding Stride.

The specific supported data paths are (represented by [logical position TPosition](../../aux_data_structures/TPosition.md)):

- Global Memory -> UB
    - GM -> VECIN

## Prototype<a name="section1954364615315"></a>

- Global Memory -> UB, supporting multi-dimensional data movement

    ```cpp
    template <typename T, uint8_t dim, const NdDmaConfig& config = kDefaultNdDmaConfig>
    __aicore__ inline void DataCopy(const LocalTensor<T>& dst, const GlobalTensor<T>& src, const NdDmaParams<T, dim>& params)
    ```

- NDDMA Cache refresh. Before using the DataCopy API to move data, use the NdDmaDci API to refresh the NDDMA Cache to ensure it is up to date.

    ```cpp
    __aicore__ inline void NdDmaDci()
    ```

## Parameters<a name="section622mcpsimp"></a>

**Table 1** Template parameter description

| Parameter | Description |
| :--- | :--- |
| T | Data type of the source operand or destination operand. |
| dim | Dimension of the data to be moved. The data type is uint8_t, and the supported dimensions are [1, 5]. |
| config | Movement configuration option, of the NdDmaConfig type, defined as follows. For specific parameter descriptions, refer to [Table 3](#table_nddma_3).<br>struct NdDmaConfig {<br>    static constexpr uint16_t unsetPad = 0xffff;<br>    bool isNearestValueMode = false;<br>    uint16_t loopLpSize = unsetPad; // Left padding size of all dimensions, must be less than 256.<br>    uint16_t loopRpSize = unsetPad; // Right padding size of all dimensions, must be less than 256.<br>    bool ascOptimize = false;       // used for Ascend C optimization on special scenario.<br>}; |

**Table 2**  Parameters

| Parameter | Input/Output | Description |
| :--- | :---: | :--- |
| dst | Output | Destination operand, of the [LocalTensor](../../data_structures/LocalTensor/LocalTensor_intro.md) type. |
| src | Input | Source operand, of the [GlobalTensor](../../data_structures/GlobalTensor/GlobalTensor_intro.md) type. |
| params | Input | Movement parameter, of the NdDmaParams type, defined as follows. For specific parameter descriptions, refer to [Table 4](#table_nddma_4).<br>template &lt;typename T, uint8_t dim&gt;<br>struct NdDmaParams  {<br>    NdDmaLoopInfo&lt;dim&gt; loopInfo;<br>    T constantValue;  // If left/right padding exists and NearestValueMode is disabled, this value is used as the padding value.<br>};<br>NdDmaLoopInfo type, defined as follows. For specific parameter descriptions, refer to [Table 5](#table_nddma_5).<br>template &lt;uint8_t dim&gt;<br>struct NdDmaLoopInfo  {<br>    uint64_t loopSrcStride[dim] = {0}; // src stride info per loop.<br>    uint32_t loopDstStride[dim] = {0}; // dst stride info per loop.<br>    uint32_t loopSize[dim] = {0}; // Loop size per loop.<br>    uint8_t loopLpSize[dim] = {0}; // Left padding size per loop.<br>    uint8_t loopRpSize[dim] = {0}; // Right padding size per loop.<br>};<br>// Note: The valid range of dim is [1, 5]. |

**Table 3**  NdDmaConfig structure parameter definition<a name="table_nddma_3"></a>

| Parameter | Description |
| :--- | :--- |
| unsetPad | Indicates that PaddingSize is not uniformly set in NdDmaConfig. It is fixed to 0xFFFF and is a constant of the uint16_t data type. When loopLpSize or loopRpSize takes this value, the corresponding PaddingSize is set per dimension by the parameter of the same name in NdDmaLoopInfo. |
| isNearestValueMode | Indicates the padding value filling mode. The type is bool.<br>true: Enables the nearest value filling mode, in which the left and right padding values are filled with the leftmost or rightmost value of the current dimension. Refer to [Figure 1](#fig10722115123919).<br>false: Enables the constant filling mode, in which all padding values are filled with the fixed value NdDmaParams::constantValue.<br>When the data type is b64, the value of isNearestValueMode should be false. |
| loopLpSize | Left padding size uniformly used by all dimensions, in element count. The value range is [0, 255] or unsetPad, and the default value is unsetPad. When set to [0, 255], all dimensions use this value and NdDmaLoopInfo::loopLpSize does not take effect. When set to unsetPad, NdDmaLoopInfo::loopLpSize is used per dimension. |
| loopRpSize | Right padding size uniformly used by all dimensions, in element count. The value range is [0, 255] or unsetPad, and the default value is unsetPad. When set to [0, 255], all dimensions use this value and NdDmaLoopInfo::loopRpSize does not take effect. When set to unsetPad, NdDmaLoopInfo::loopRpSize is used per dimension. |
| ascOptimize | Reserved parameter. Currently not supported. |

**Table 4**  NdDmaParams structure parameter definition<a name="table_nddma_4"></a>

| Parameter | Description |
| :--- | :--- |
| loopInfo | Information about the data to be moved in each dimension. The type is NdDmaLoopInfo&lt;dim&gt;.<br>The element count of the array-type parameters in the NdDmaLoopInfo structure is determined by the template parameter dim. The value range of dim is [1, 5], and the valid index range of the array is [0, dim - 1]. For details about specific parameters, refer to [Table 5](#table_nddma_5). |
| constantValue | A value of data type T. When padding exists on the left and right of a dimension and NearestValueMode is disabled, this value is used as the padding value.<br>When the data type is b64, the value of constantValue must be 0. |

**Table 5**  NdDmaLoopInfo structure parameter definition<a name="table_nddma_5"></a>

| Parameter | Description |
| :--- | :--- |
| loopSrcStride | Indicates the interval between a source operand element and the next element within each dimension.<br>Unit: element count. The data type is uint64_t, and loopSrcStride must be in [0, 2<sup>40</sup>). |
| loopDstStride | Indicates the interval between a destination operand element and the next element within each dimension.<br>Unit: element count. The data type is uint32_t, and loopDstStride must be in [0, 2<sup>20</sup>). |
| loopSize | Indicates the number of elements processed within each dimension (excluding padding elements).<br>Unit: element count. The data type is uint32_t, and loopSize must be in [1, 2<sup>20</sup>). |
| loopLpSize | Indicates the number of elements to be padded on the left within each dimension.<br>Unit: element count. The data type is uint8_t, and loopLpSize must not exceed the value range of the data type: [0, 255]. |
| loopRpSize | Indicates the number of elements to be padded on the right within each dimension.<br>Unit: element count. The data type is uint8_t, and loopRpSize must not exceed the value range of the data type: [0, 255]. |

The following uses 2D examples to introduce several typical usage scenarios.

**Figure 1**  2D Padding scenario<a name="fig10722115123919"></a>  
![](../../../../figures/2d_padding_scenario.png "2D Padding scenario")

**Figure 2**  2D Transpose scenario<a name="fig104451143104510"></a>  
![](../../../../figures/2d_transpose_scenario.png "2D Transpose scenario")

**Figure 3**  2D BroadCast scenario<a name="fig4654326132913"></a>  
![](../../../../figures/2d_broadcast_scenario.png "2D BroadCast scenario")

**Figure 4**  2D Slice scenario<a name="fig13061435132915"></a>  
![](../../../../figures/2d_slice_scenario.png "2D Slice scenario")

## Data Types

The supported data types of the source operand and the destination operand must be consistent.

<!-- npu="950" id1 -->
For the Ascend 950PR&950DT products, the supported data types are: bool, int8_t, uint8_t, int16_t, uint16_t, half, bfloat16_t, int32_t, uint32_t, float, complex32, int64_t, uint64_t, double, complex64.
<!-- end id1 -->

## Return Value<a name="section7607175220218"></a>

None

## Constraints<a name="section633mcpsimp"></a>

- The address range width of all data that can be obtained by one instruction cannot exceed 40 bits (1 TB), that is:

    The size of each loop of the source operand is: \(loopLpSize + loopSize + loopRpSize -1 \) \* loopSrcSize, and the size of each loop of the destination operand is: \(loopLpSize + loopSize + loopRpSize -1 \) \* loopDstSize. The total size of all loops cannot exceed 2 to the 40th power bits.

- When dstStride of each loop level is an ascending sequence, the address spaces of different loops cannot interleave or overlap. Taking a 2D Padding scenario as an example, the minimum stride value of the second dimension of loopSrcStride and loopDstStride is 3, and data 3 cannot fall into the loop of dimension 1.

    ![](../../../../figures/nddma_padding_2d.png)

- This API transfers data through NDDMA. The corresponding NDDMA cache size is 32 KB. Before using the DataCopy API to transfer data, use the NdDmaDci API to refresh the cache. Otherwise, in a multi-core scenario, reading and writing the same Global Memory address may cause some cores to read incorrect data.

## Examples<a name="section122101199486"></a>

- Scenario 1: 2D padding with constant filling

    ```cpp
    // xGmShape: [16, 32]. Copy 8 columns and 2 rows of data, with left padding 15, top padding 13, right padding 17, and bottom padding 3. xLocalShape: [32, 64].
    AscendC::NdDmaLoopInfo<2> loopInfo{{1, 32}, {1, 64}, {32, 16}, {15, 13}, {17, 3}};
    AscendC::NdDmaParams<T, 2> params{loopInfo, 0};  // The padding value is 0.
    AscendC::NdDmaDci();  // Refresh the cache.
    static constexpr AscendC::NdDmaConfig dmaConfig;  // Use the default parameters. This argument can also be omitted.
    AscendC::DataCopy<T, 2, dmaConfig>(xLocal, xGm, params);
    ```

- Scenario 2: 2D padding with nearest-value filling

    ```cpp
    // xGmShape: [28, 15]. Copy 3 columns and 4 rows of data, with left padding 11, top padding 3, right padding 6, and bottom padding 1. xLocalShape: [32, 32].
    AscendC::NdDmaLoopInfo<2> loopInfo{{1, 15}, {1, 32}, {15, 28}, {11, 3}, {6, 1}};
    AscendC::NdDmaParams<T, 2> params{loopInfo, 0};
    AscendC::NdDmaDci();  // Refresh the cache.
    // isNearestValueMode = true: the left and right padding values are filled with the leftmost or rightmost value of the current dimension.
    static constexpr AscendC::NdDmaConfig dmaConfig = {true};
    AscendC::DataCopy<T, 2, dmaConfig>(xLocal, xGm, params);
    ```

- Scenario 3: 2D Transpose scenario

    ```cpp
    // xGmShape: [16, 64]. Move 8 columns and 4 rows of data. xLocalShape: [64, 16].
    AscendC::NdDmaLoopInfo<2> loopInfo{{1, 64}, {16, 1}, {64, 16}, {0, 0}, {0, 0}};
    AscendC::NdDmaParams<T, 2> params{loopInfo, 0};
    AscendC::NdDmaDci();  // Refresh the cache.
    static constexpr AscendC::NdDmaConfig dmaConfig;  // Use default parameters. This can also be omitted.
    AscendC::DataCopy<T, 2, dmaConfig>(xLocal, xGm, params);
    ```

- Scenario 4: 2D BroadCast scenario

    ```cpp
    // xGmShape: [1, 16]. Move 16 columns and 1 row of data and broadcast it. xLocalShape: [3, 16].
    AscendC::NdDmaLoopInfo<2> loopInfo{{1, 0}, {1, 16}, {16, 3}, {0, 0}, {0, 0}};
    AscendC::NdDmaParams<T, 2> params{loopInfo, 0};
    AscendC::NdDmaDci();  // Refresh the cache.
    static constexpr AscendC::NdDmaConfig dmaConfig;  // Use default parameters. This can also be omitted.
    AscendC::DataCopy<T, 2, dmaConfig>(xLocal, xGm, params);
    ```

- Scenario 5: 2D Slice scenario

    ```cpp
    // xGmShape: [32, 64]. Crop a 16*10 matrix into a 5*7 matrix. xLocalShape: [16, 16].
    AscendC::NdDmaLoopInfo<2> loopInfo{{1, 64}, {1, 16}, {16, 16}, {0, 0}, {0, 0}};
    AscendC::NdDmaParams<T, 2> params{loopInfo, 0};
    AscendC::NdDmaDci();  // Flush the cache.
    static constexpr AscendC::NdDmaConfig dmaConfig;  // Use the default parameters. This can also be omitted.
    AscendC::DataCopy<T, 2, dmaConfig>(xLocal, xGm, params);
    ```

For the complete sample, see [Multidimensional Data Movement Sample](../../../../../../../examples/01_simd_cpp_api/03_basic_api/00_data_movement/data_copy_gm2ub_nddma).
