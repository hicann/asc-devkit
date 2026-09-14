# GM-to-UB Multidimensional Data Transfer with NDDMA (DataCopy)<a name="ZH-CN_TOPIC_0000002544407954"></a>

<!-- md-trans-meta sourceCommit=e5451c7feba11f9e259f4fbc19c4bf82f8c0496c translatedAt=2026-09-09T13:27:32.687Z -->

## Applicable Products<a name="section1550532418810"></a>

<!-- npu="950" id9 -->
- Ascend 950PR/Ascend 950DT: Supported
<!-- end id9 -->
<!-- npu="A3" id10 -->
- Atlas A3 training products/Atlas A3 inference products: Not supported
<!-- end id10 -->
<!-- npu="910b" id11 -->
- Atlas A2 training products/Atlas A2 inference products: Not supported
<!-- end id11 -->
<!-- npu="310b" id12 -->
- Atlas 200I/500 A2 inference products: Not supported
<!-- end id12 -->
<!-- npu="310p" id13 -->
- Atlas inference products AI Core: Not supported
<!-- end id13 -->
<!-- npu="310p" id14 -->
- Atlas inference products Vector Core: Not supported
<!-- end id14 -->
<!-- npu="910" id15 -->
- Atlas training products: Not supported
<!-- end id15 -->

## Description<a name="section474617392321"></a>

Header file path: `"basic_api/kernel_operator_data_copy_intf.h"`.

The multidimensional data transfer API, compared with the basic data transfer API, allows more flexible configuration of the dimension information to be transferred and the corresponding Stride.

The specific supported data paths are (represented by [logical position TPosition](../../aux_data_structures/TPosition.md)):

- Global Memory -> Unified Buffer
    - GM -> VECIN

## Prototype<a name="section1954364615315"></a>

- Global Memory -> Unified Buffer, supporting multidimensional data transfer

    ```cpp
    template <typename T, uint8_t dim, const NdDmaConfig& config = kDefaultNdDmaConfig>
    __aicore__ inline void DataCopy(const LocalTensor<T>& dst, const GlobalTensor<T>& src, const NdDmaParams<T, dim>& params)
    ```

- NDDMA Cache refresh. Before using the DataCopy API to transfer data, use the NdDmaDci API to refresh the NdDMA Cache to ensure it is in the latest state.

    ```cpp
    __aicore__ inline void NdDmaDci()
    ```

## Parameters<a name="section622mcpsimp"></a>

**Table 1** Template parameters

| Parameter | Description |
| :--- | :--- |
| T | Data type of the source operand or destination operand. |
| dim | Dimension of the data to be transferred. The data type is uint8_t, and the supported range is [1, 5]. |
| config | Transfer configuration option, of the NdDmaConfig type, defined as follows. For details about specific parameters, see [Table 3](#table_nddma_3).<br>struct NdDmaConfig {<br>    static constexpr uint16_t unsetPad = 0xffff;<br>    bool isNearestValueMode = false;<br>    uint16_t loopLpSize = unsetPad; // Left padding size of all dimensions, must be less than 256.<br>    uint16_t loopRpSize = unsetPad; // Right padding size of all dimensions, must be less than 256.<br>    bool ascOptimize = false;       // used for Ascend C optimization on special scenario.<br>};<br>|

**Table 2** Parameters

| Parameter | Input/Output | Description |
| :--- | :---: | :--- |
| dst | Output | Destination operand, of the [LocalTensor](../../data_structures/LocalTensor/localtensor_introduction.md) type. |
| src | Input | Source operand, of the [GlobalTensor](../../data_structures/GlobalTensor/globaltensor_introduction.md) type. |
| params | Input | Transfer parameter, of the NdDmaParams type, defined as follows. For details about specific parameters, see [Table 4](#table_nddma_4).<br>template &lt;typename T, uint8_t dim&gt;<br>struct NdDmaParams  {<br>    NdDmaLoopInfo&lt;dim&gt; loopInfo;<br>    T constantValue;  // If left and right padding exist and NearestValueMode is disabled, this value is used as the padding value.<br>};<br>The NdDmaLoopInfo type is defined as follows. For details about specific parameters, see [Table 5](#table_nddma_5).<br>template &lt;uint8_t dim&gt;<br>struct NdDmaLoopInfo  {<br>    uint64_t loopSrcStride[dim] = {0}; // src stride info per loop.<br>    uint32_t loopDstStride[dim] = {0}; // dst stride info per loop.<br>    uint32_t loopSize[dim] = {0}; // Loop size per loop.<br>    uint8_t loopLpSize[dim] = {0}; // Left padding size per loop.<br>    uint8_t loopRpSize[dim] = {0}; // Right padding size per loop.<br>};<br>// Note: The valid range of dim is [1, 5].<br>|

**Table 3** NdDmaConfig structure parameters<a name="table_nddma_3"></a>

| Parameter | Description |
| :--- | :--- |
| unsetPad | Indicates that PaddingSize is not set. The value is fixed at 0xFFFF. |
| isNearestValueMode | Indicates the padding value filling mode. The type is bool.<br>True: Enables the nearest value filling mode, in which the left and right padding values are filled with the leftmost or rightmost value of the current dimension. For details, see [Figure 1](#fig10722115123919).<br>False: Enables the constant filling mode, in which all padding values are filled with the fixed value NdDmaParams::constantValue.<br>When the data type is b64, the value of isNearestValueMode must be False. |
| loopLpSize | Indicates the PaddingSize within each dimension. When this value is not unsetPad, it indicates that the left PaddingSize in all loops is this value, and NdDmaLoopInfo::loopLpSize does not take effect. The default value is unsetPad. Developers can set it to the default value or a value in [0, 255]. |
| loopRpSize | Indicates the PaddingSize within each dimension. When this value is not unsetPad, it indicates that the right PaddingSize in all loops is this value, and NdDmaLoopInfo::loopRpSize does not take effect. The default value is unsetPad. Developers can set it to the default value or a value in [0, 255]. |
| ascOptimize | Reserved parameter, not supported. |

**Table 4** NdDmaParams structure parameters<a name="table_nddma_4"></a>

| Parameter | Description |
| :--- | :--- |
| loopInfo | Information for transfer in each dimension, of the NdDmaLoopInfo&lt;dim&gt; type.<br>For array-type parameters in the NdDmaLoopInfo structure, the array index corresponds to the actual dimension information, with indexes 0 to 4 corresponding to dimensions 1 to 5. For details about specific parameters, see [Table 5](#table_nddma_5). |
| constantValue | A value of the T data type. When left and right padding exist in a dimension and NearestValueMode is disabled, this value is used as the padding value.<br>When the data type is b64, the value of constantValue must be 0. |

**Table 5**  NdDmaLoopInfo structure parameter definition<a name="table_nddma_5"></a>

| Parameter | Description |
| :--- | :--- |
| loopSrcStride | Indicates the interval between a source operand element and the next element within each dimension.<br>The unit is element count. The data type is uint64_t, and loopSrcStride must be within [0, 2<sup>40</sup>). |
| loopDstStride | Indicates the interval between a destination operand element and the next element within each dimension.<br>The unit is element count. The data type is uint32_t, and loopDstStride must be within [0, 2<sup>20</sup>). |
| loopSize | Indicates the number of elements processed within each dimension (excluding Padding elements).<br>The unit is element count. The data type is uint32_t, and loopSize must be within [0, 2<sup>20</sup>). |
| loopLpSize | Indicates the number of elements to be padded on the left within each dimension.<br>The unit is element count. The data type is uint8_t, and loopLpSize must not exceed the value range of this data type: [0, 255]. |
| loopRpSize | Indicates the number of elements to be padded on the right within each dimension.<br>The unit is element count. The data type is uint8_t, and loopRpSize must not exceed the value range of this data type: [0, 255]. |

The following uses a 2D example to introduce several typical usage scenarios.

**Figure 1**  2D Padding scenario<a name="fig10722115123919"></a>  
![](../../../../figures/2d_padding_scenario.png "2D-Padding scenario")

**Figure 2**  2D Transpose scenario<a name="fig104451143104510"></a>  
![](../../../../figures/2d_transpose_scenario.png "2D-Transpose scenario")

**Figure 3**  2D BroadCast scenario<a name="fig4654326132913"></a>  
![](../../../../figures/2d_broadcast_scenario.png "2D-BroadCast scenario")

**Figure 4**  2D Slice scenario<a name="fig13061435132915"></a>  
![](../../../../figures/2d_slice_scenario.png "2D-Slice scenario")

## Data Type

The data types supported by the source operand and the destination operand must be consistent.

<!-- npu="950" id1 -->
The data types supported by Ascend 950PR/Ascend 950DT are: b8, b16, b32, and b64.
<!-- end id1 -->

## Return Value<a name="section7607175220218"></a>

None

## Constraints<a name="section633mcpsimp"></a>

- The address range width of all data that can be obtained by a single instruction must not exceed 40 bits (1 TB), that is:

    The size of each loop of the source operand is: \(loopLpSize + loopSize + loopRpSize -1 \) \* loopSrcSize, and the size of each loop of the destination operand is: \(loopLpSize + loopSize + loopRpSize -1 \) \* loopDstSize. The sum of the sizes of all loops must not exceed 2^40 bits.

- When dstStride of each loop is an ascending sequence, the address spaces of different loops must not interleave or overlap. Taking a 2D Padding scenario as an example, the minimum stride value of the second dimension of loopSrcStride and loopDstStride is 3, and data 3 must not fall within the loop of dimension 1.

    ![](../../../../figures/nddma_padding_2d.png)

- This API transfers data through NDDMA, and the corresponding NDDMA Cache size is 32 KB. Before using the DataCopy API to transfer data, use the NdDmaDci API to refresh the cache. Otherwise, in a multi-core scenario, reading and writing the same Global Memory address may cause some cores to read incorrect data.

## Calling Example<a name="section122101199486"></a>

- Scenario 1: 2D Padding scenario, constant padding mode

    ```cpp
    // xGmShape: [16, 32], copy 8 columns and 2 rows of data, left padding 15, upper padding 13, right padding 17, lower padding 3, xLocalShape: [32, 64].
    AscendC::NdDmaLoopInfo<2> loopInfo{{1, 32}, {1, 64}, {32, 16}, {15, 13}, {17, 3}};
    AscendC::NdDmaParams<T, 2> params{loopInfo, 0};  // The padding value is 0.
    AscendC::NdDmaDci();  // Refresh the cache.
    static constexpr AscendC::NdDmaConfig dmaConfig;  // Use default parameters. This can also be omitted.
    AscendC::DataCopy<T, 2, dmaConfig>(xLocal, xGm, params);
    ```

- Scenario 2: 2D Padding scenario, nearest value padding mode

    ```cpp
    // xGmShape: [28, 15], copy 3 columns and 4 rows of data, left padding 11, upper padding 3, right padding 6, lower padding 1, xLocalShape: [32, 32].
    AscendC::NdDmaLoopInfo<2> loopInfo{{1, 15}, {1, 32}, {15, 28}, {11, 3}, {6, 1}};
    AscendC::NdDmaParams<T, 2> params{loopInfo, 0};
    AscendC::NdDmaDci();  // Refresh the cache.
    // isNearestValueMode = true: the left and right padding values are filled with the leftmost or rightmost value of the current dimension.
    static constexpr AscendC::NdDmaConfig dmaConfig = {true};
    AscendC::DataCopy<T, 2, dmaConfig>(xLocal, xGm, params);
    ```

- Scenario 3: 2D Transpose

    ```cpp
    // xGmShape: [16, 64], move 8 columns and 4 rows of data, xLocalShape: [64, 16].
    AscendC::NdDmaLoopInfo<2> loopInfo{{1, 64}, {16, 1}, {64, 16}, {0, 0}, {0, 0}};
    AscendC::NdDmaParams<T, 2> params{loopInfo, 0};
    AscendC::NdDmaDci();  // Refresh the cache
    static constexpr AscendC::NdDmaConfig dmaConfig;  // Use default parameters; this can also be omitted.
    AscendC::DataCopy<T, 2, dmaConfig>(xLocal, xGm, params);
    ```

- Scenario 4: 2D BroadCast

    ```cpp
    // xGmShape: [1, 16], move 16 columns and 1 row of data and broadcast it, xLocalShape: [3, 16].
    AscendC::NdDmaLoopInfo<2> loopInfo{{1, 0}, {1, 16}, {16, 3}, {0, 0}, {0, 0}};
    AscendC::NdDmaParams<T, 2> params{loopInfo, 0};
    AscendC::NdDmaDci();  // Refresh the cache
    static constexpr AscendC::NdDmaConfig dmaConfig;  // Use default parameters; this can also be omitted.
    AscendC::DataCopy<T, 2, dmaConfig>(xLocal, xGm, params);
    ```

- Scenario 5: 2D Slice

    ```cpp
    // xGmShape: [32, 64], truncate the 16*10 matrix into a 5*7 matrix, xLocalShape: [16, 16].
    AscendC::NdDmaLoopInfo<2> loopInfo{{1, 64}, {1, 16}, {16, 16}, {0, 0}, {0, 0}};
    AscendC::NdDmaParams<T, 2> params{loopInfo, 0};
    AscendC::NdDmaDci();  // Refresh the cache.
    static constexpr AscendC::NdDmaConfig dmaConfig;  // Use the default parameters. This parameter can also be omitted.
    AscendC::DataCopy<T, 2, dmaConfig>(xLocal, xGm, params);
    ```

For the complete sample, see [multidimensional data transfer sample](https://gitcode.com/cann/asc-devkit/tree/9.1.0/examples/01_simd_cpp_api/03_basic_api/00_data_movement/data_copy_gm2ub_nddma).
