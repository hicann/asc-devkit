# DataCopy (UBToUB High-Dimensional Splitting Data Movement)<a name="ZH-CN_TOPIC_0000002382908021"></a>

<!-- md-trans-meta sourceCommit=754fb95859dead826727a591e674261c6a829a08 translatedAt=2026-10-26T09:45:23.702Z pushedAt=2026-10-28T03:14:39.498Z nl=2 -->

## Applicable Products<a name="section1550532418810"></a>

<!-- npu="950" id1 -->
- Ascend 950PR&950DT products: supported
<!-- end id1 -->
<!-- npu="A3" id2 -->
- Atlas A3 products: supported
<!-- end id2 -->
<!-- npu="910b" id3 -->
- Atlas A2 products: supported
<!-- end id3 -->
<!-- npu="310b" id4 -->
- Atlas 200I/500 A2 inference products: not supported
<!-- end id4 -->
<!-- npu="310p" id5 -->
- Atlas inference products AI Core: supported
<!-- end id5 -->
<!-- npu="310p" id6 -->
- Atlas inference products Vector Core: not supported
<!-- end id6 -->
<!-- npu="910" id7 -->
- Atlas training products: supported
<!-- end id7 -->
<!-- @ref: asc-devkit/res/docs/zh/api/SIMD-API/basic_api/memory_vector_compute/data_move/DataCopy_UBToUB_highdim_split_res.md#id1 -->

## Description<a name="section474617392321"></a>

The header file path is `"basic_api/kernel_operator_data_copy_intf.h"`.

Supports high-dimensional splitting data movement within the Unified Buffer (UB). The data retains its original format and content during transmission.

The specific supported data paths are (represented by [logical position TPosition](../../aux_data_structures/TPosition.md)):

- UB -> UB
    - VECIN -> VECCALC
    - VECCALC -> VECOUT

High-dimensional splitting data movement can be configured with movement parameters such as the number of data blocks, the length of a single data block, and the address offset, and supports both non-contiguous and contiguous data movement.

## Prototype<a name="section1954364615315"></a>

```cpp
template <typename T>
__aicore__ inline void DataCopy(const LocalTensor<T>& dst, const LocalTensor<T>& src, const DataCopyParams& repeatParams)
```

## Parameters<a name="section622mcpsimp"></a>

**Table 1** Template parameter description

| Parameter | Description |
| :--- | :--- |
| T | Data type of the operand. For supported data types, see [Data Types](#section4219135304818). |

**Table 2**  Parameters

| Parameter | Input/Output | Description |
| :--- | :---: | :--- |
| dst | Output | Destination operand, of type [LocalTensor](../../data_structures/LocalTensor/LocalTensor_intro.md). It is stored in the UB, and the destination address must be 32-byte aligned. |
| src | Input | Source operand, of type LocalTensor. It is stored in the UB, and the source address must be 32-byte aligned. |
| repeatParams | Input | Copy parameters, of type DataCopyParams. For details, see Table 3. This parameter configures the size, number, and gap of the data blocks to be copied, and supports both non-contiguous and contiguous copying.<br>For the definition, see `${INSTALL_DIR}/asc/include/basic_api/kernel_struct_data_copy.h`. Replace `${INSTALL_DIR}` with the file storage path after the CANN software is installed. |

**Table 3** Parameter definition of the DataCopyParams structure

| Parameter | Description |
| --- | --- |
| blockCount | Number of data blocks to be copied. Data type: uint16_t. Value range: blockCount ∈ [0, 4095]. Default value: 1. |
| blockLen | Length of each data block to be copied. Data type: uint16_t. Value range: blockLen ∈ [0, 65535]. Unit: DataBlock (32B). |
| srcGap | Gap between adjacent data blocks of the source operand (that is, the difference between the **end address** of the previous data block and the **start address** of the next data block). Data type: uint16_t. Value range: srcGap ∈ [0, 65535]. Unit: DataBlock (32B).<br>&bull; When blockCount = 1, srcGap is meaningless and can be set to 0. |
| dstGap | Gap between adjacent data blocks of the destination operand (that is, the difference between the **end address** of the previous data block and the **start address** of the next data block). Data type: uint16_t. Value range: dstGap ∈ [0, 65535]. Unit: DataBlock (32B).<br>&bull; When blockCount = 1, dstGap is meaningless and can be set to 0. |

The following sample demonstrates the usage of contiguous copying and non-contiguous copying through two examples.

For continuous copy scenarios, [DataCopy\(UBToUB continuous data copy \)](DataCopy_UBToUB_continuous.md#ZH-CN_TOPIC_0000002382908021) is recommended. The following example only demonstrates that the high-dimensional split data copy API supports continuous copy.

- Example 1: Continuous copy

    Taking the half data type as an example, the shape of the source operand is 1 \* 128.

    Using Figure 1 as an example, **continuously** copy 128 elements from the source operand to the destination operand. The DataCopyParams structure parameters are configured as follows:

    - blockCount = 1, copying 1 data block, indicating continuous copying.
    - blockLen = 8, one data block has a length of 8 DataBlocks.
    - srcGap = 0, with an inter-block gap of 0 between adjacent data blocks in the source operand.
    - dstGap = 0, with an inter-block gap of 0 between adjacent data blocks in the destination operand.

    **Figure 1**  Continuous copy diagram<a name="zh-cn_topic_0000002535088926_fig4621152141711"></a>  
    ![](../../../../figures/continuous_data_copy_diagram.png "Continuous copy diagram")

- Example 2: Non-continuous copy

    Take the half data type as an example. The shape of the source operand is 1 \* 112.

    Using Figure 2 as an example, **non-continuously** copy data from the source operand to the destination operand. The DataCopyParams structure parameters are configured as follows:

    - blockCount = 2, copying 2 data blocks.
    - blockLen = 3, the length of one data block is 3 Data Blocks.
    - srcGap = 1, the gap between adjacent data blocks in the source operand is 1.
    - dstGap = 2, the gap between adjacent data blocks in the destination operand is 2.

    **Figure 2**  Non-continuous copy diagram<a name="zh-cn_topic_0000002535088926_fig422819202304"></a>  
    ![](../../../../figures/discontinuous_data_copy_diagram.png "Non-continuous copy diagram")

## Data Type<a name="section4219135304818"></a>

<!-- npu="950" id10 -->

- Ascend 950PR&950DT products: supported data types: bool, int8_t, uint8_t, hifloat8_t, fp8_e8m0_t, fp8_e5m2_t, fp8_e4m3fn_t, int16_t, uint16_t, half, bfloat16_t, int32_t, uint32_t, float, complex32, int64_t, uint64_t, double, complex64.

<!-- end id10 -->

<!-- npu="A3" id11 -->

- For the Atlas A3 products, the supported data types are: int8_t, uint8_t, int16_t, uint16_t, half, bfloat16_t, int32_t, uint32_t, float, int64_t, uint64_t, double.

<!-- end id11 -->

<!-- npu="910b" id12 -->

- For the Atlas A2 products, the supported data types are: int8_t, uint8_t, int16_t, uint16_t, half, bfloat16_t, int32_t, uint32_t, float, int64_t, uint64_t, double.

<!-- end id12 -->

<!-- npu="310p" id13 -->

- Atlas inference products AI Core: supported data types: int8_t, uint8_t, int16_t, uint16_t, half, int32_t, uint32_t, float, int64_t, uint64_t, double.

<!-- end id13 -->

<!-- npu="910" id14 -->

- Atlas training products: supported data types: int8_t, uint8_t, int16_t, uint16_t, half, int32_t, uint32_t, float, int64_t, uint64_t, double.

<!-- end id14 -->

## Return Value<a name="section44801012174220"></a>

None

## Constraints<a name="section633mcpsimp"></a>

- The address in the UB must be 32-byte aligned.
<!-- npu="A3,910b,950" id18 -->
- When either blockCount or blockLen in the DataCopyParams structure parameter is 0, this API is treated as a NOP (no operation). This description applies to the following models:
  <!-- npu="A3" id15 -->
  - Atlas A3 products
  <!-- end id15 -->
  <!-- npu="910b" id16 -->
  - Atlas A2 products
  <!-- end id16 -->
  <!-- npu="950" id17 -->
  - Ascend 950PR&950DT products
  <!-- end id17 -->
<!-- end id18 -->
- The values of the **DataCopyParams** structure parameters must be within the value range:

    **Table 4** Value range of the DataCopyParams structure parameters

    | Parameter | Value Range |
    | --- | --- |
    | blockCount | [0, 4095] |
    | blockLen | [0, 65535] |
    | srcGap | [0, 65535] |
    | dstGap | [0, 65535] |

<!-- npu="910b,A3" id19 -->

- For the following product models:
    - Atlas A2 products
    - Atlas A3 products

    In the cross-card communication operator development scenario, the DataCopy class APIs support cross-card data copy only over the HCCS physical link, and do not support other paths. During development, pay attention to the physical path involved in inter-card communication. You can run the `npu-smi info -t topo` command to query the HCCS physical link.

<!-- end id19 -->

## Examples<a name="section122101199486"></a>

- Scenario 1: Continuous copy — copy 128 half elements from the source data continuously to the destination operand (corresponding to [Figure 1](#zh-cn_topic_0000002535088926_fig4621152141711))

    ```cpp
    // srcLocal and dstLocal are LocalTensors of the half type.
    // Use the copy API that takes the count parameter to perform continuous copy.
    AscendC::DataCopy(dstLocal, srcLocal, 128);
    
    // Use the copy API that takes the DataCopyParams parameter to achieve the same continuous copy effect.
    // blockCount = 1: The number of continuous data blocks is 1.
    // blockLen = 8: The length of one data block is 8 DataBlocks (128 half elements ÷ (32B/2B) = 8).
    // srcGap = 0, dstGap = 0: No gap, continuous copy.
    AscendC::DataCopyParams params;
    params.blockCount = 1;
    params.blockLen = 8;
    params.srcGap = 0;
    params.dstGap = 0;
    AscendC::DataCopy(dstLocal, srcLocal, params);
    ```

- Scenario 2: Non-continuous copy — copy 2 data blocks from the source data, each data block containing 3 DataBlocks, with a source gap of 1 DataBlock and a destination gap of 2 DataBlocks (corresponding to [Figure 2](#zh-cn_topic_0000002535088926_fig422819202304))

    ```cpp
    // srcLocal and dstLocal are LocalTensors of the half type.
    // The source data shape is [1, 112], with 112 half elements (7 DataBlocks) in total.
    // The destination data receives 2 non-contiguously arranged data blocks, with 6 DataBlocks (96 half elements) in total.
    // blockCount = 2: Copy 2 data blocks.
    // blockLen = 3: Each data block is 3 DataBlocks long.
    // srcGap = 1: The gap between adjacent source data blocks is 1 DataBlock.
    // dstGap = 2: The gap between adjacent destination data blocks is 2 DataBlocks.
    AscendC::DataCopyParams params;
    params.blockCount = 2;
    params.blockLen = 3;
    params.srcGap = 1;
    params.dstGap = 2;
    AscendC::DataCopy(dstLocal, srcLocal, params);
    ```
