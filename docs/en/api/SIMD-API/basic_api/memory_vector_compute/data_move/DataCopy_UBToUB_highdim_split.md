# UB-to-UB High-Dimensional Split Data Transfer (DataCopy)<a name="ZH-CN_TOPIC_0000002382908021"></a>

<!-- md-trans-meta sourceCommit=f6490d1e030d257ee55f3f1600bffece5a8bd3f7 translatedAt=2026-09-09T13:38:53.337Z -->

## Applicable Products<a name="section1550532418810"></a>

<!-- npu="950" id17 -->
- Ascend 950PR/Ascend 950DT: Supported
<!-- end id17 -->
<!-- npu="A3" id18 -->
- Atlas A3 training products/Atlas A3 inference products: Supported
<!-- end id18 -->
<!-- npu="910b" id19 -->
- Atlas A2 training products/Atlas A2 inference products: Supported
<!-- end id19 -->
<!-- npu="310b" id20 -->
- Atlas 200I/500 A2 inference products: Not supported
<!-- end id20 -->
<!-- npu="310p" id21 -->
- Atlas inference products AI Core: Supported
<!-- end id21 -->
<!-- npu="310p" id22 -->
- Atlas inference products Vector Core: Not supported
<!-- end id22 -->
<!-- npu="910" id23 -->
- Atlas training products: Supported
<!-- end id23 -->

## Description<a name="section474617392321"></a>

Header file path: `"basic_api/kernel_operator_data_copy_intf.h"`.

Supports high-dimensional split data transfer between Unified Buffers. The data remains in its original format and content during transmission.

The specific supported data paths are as follows (represented by [logical position TPosition](../../aux_data_structures/TPosition.md)):

- Unified Buffer -> Unified Buffer
    - VECIN -> VECCALC
    - VECCALC -> VECOUT

High-dimensional split data transfer can be implemented by configuring transfer parameters such as the number of data blocks, the length of a single data block, and the address offset. It supports both non-continuous and continuous data transfer.

## Prototype<a name="section1954364615315"></a>

```cpp
template <typename T>
__aicore__ inline void DataCopy(const LocalTensor<T>& dst, const LocalTensor<T>& src, const DataCopyParams& repeatParams)
```

## Parameters<a name="section622mcpsimp"></a>

**Table 1** Template parameters

| Parameter | Description |
| :--- | :--- |
| T | Data type of the operand. For supported data types, see [Data Type](#section4219135304818). |

**Table 2** Parameters

| Parameter | Input/Output | Description |
| :--- | :---: | :--- |
| dst | Output | Destination operand, of the [LocalTensor](../../data_structures/LocalTensor/localtensor_introduction.md) type, stored in Unified Buffer. The destination address must be 32-byte aligned. |
| src | Input | Source operand, of the LocalTensor type, stored in Unified Buffer. The source address must be 32-byte aligned. |
| repeatParams | Input | Transfer parameters, of the DataCopyParams type. For details, see Table 3. This parameter configures the size, number, and interval of the data blocks to be transferred, and supports both non-continuous and continuous transfer.<br>For the specific definition, see \$\{INSTALL\_DIR\}/include/ascendc/basic\_api/interface/kernel\_struct\_data\_copy.h, where \$\{INSTALL\_DIR\} is the file storage path after CANN software installation. |

**Table 3** DataCopyParams structure parameters

| Parameter | Description |
| --- | --- |
| blockCount | Number of data blocks to be transferred. Data type: uint16_t. Value range: blockCount∈[0, 4095]. Default value: 1. |
| blockLen | Length of each data block to be transferred. Data type: uint16_t. Value range: blockLen∈[0, 65535]. Unit: DataBlock (32B). |
| srcGap | Interval between adjacent data blocks of the source operand (that is, the difference between the **end address** of the previous data block and the **start address** of the next data block). Data type: uint16_t. Value range: srcGap∈[0, 65535]. Unit: DataBlock (32B).<br>&bull; When blockCount = 1, srcGap is meaningless and can be set to 0. |
| dstGap | Interval between adjacent data blocks of the destination operand (that is, the difference between the **end address** of the previous data block and the **start address** of the next data block). Data type: uint16_t. Value range: dstGap∈[0, 65535]. Unit: DataBlock (32B).<br>&bull; When blockCount = 1, dstGap is meaningless and can be set to 0. |

The following samples demonstrate the usage of continuous transfer and non-continuous transfer through two examples.

For continuous transfer scenarios, it is recommended to use [UB-to-UB continuous data transfer \(DataCopy\)](DataCopy_UBToUB_continuous.md#ZH-CN_TOPIC_0000002382908021). The following example only demonstrates that the high-dimensional split data transfer API supports continuous transfer.

- Example 1: Continuous transfer

    Take the half data type as an example. The shape of the source operand is 1 \* 128.

    Using Figure 1 as an example, the 128 elements in the source operand are **continuously** transferred to the destination operand. The DataCopyParams structure parameters are configured as follows:

    - blockCount = 1, indicating that one Data Block is transferred, which means continuous transfer.
    - blockLen = 8, indicating that the length of one Data Block is 8 Data Blocks.
    - srcGap = 0, indicating that the gap between adjacent Data Blocks in the source operand is 0.
    - dstGap = 0, indicating that the gap between adjacent Data Blocks in the destination operand is 0.

    **Figure 1**  Continuous transfer diagram<a name="zh-cn_topic_0000002535088926_fig4621152141711"></a>  
    ![](../../../../figures/continuous_data_copy_diagram.png "Continuous transfer diagram")

- Example 2: Non-continuous transfer

    Using the half data type as an example, the shape of the source operand is 1 \* 112.

    Using Figure 2 as an example, the data in the source operand is **non-continuously** transferred to the destination operand. The DataCopyParams structure parameters are configured as follows:

    - blockCount = 2, indicating that two Data Blocks are transferred.
    - blockLen = 3, the length of one data block is 3 Data Blocks.
    - srcGap = 1, the interval between adjacent data blocks of the source operand is 1.
    - dstGap = 2, the interval between adjacent data blocks of the destination operand is 2.

    **Figure 2**  Non-continuous transfer diagram<a name="zh-cn_topic_0000002535088926_fig422819202304"></a>  
    ![](../../../../figures/discontinuous_data_copy_diagram.png "Non-continuous transfer diagram")

## Data Type<a name="section4219135304818"></a>

<!-- npu="950" id1 -->
- Ascend 950PR/Ascend 950DT, supported data types are: bool, int8_t, uint8_t, hifloat8_t, fp8_e8m0_t, fp8_e5m2_t, fp8_e4m3fn_t, int16_t, uint16_t, half, bfloat16_t, int32_t, uint32_t, float, complex32, int64_t, uint64_t, double, complex64.
<!-- end id1 -->

<!-- npu="A3" id2 -->
- Atlas A3 Training Series products/Atlas A3 Inference Series products, supported data types are: int8_t, uint8_t, int16_t, uint16_t, half, bfloat16_t, int32_t, uint32_t, float, int64_t, uint64_t, double.
<!-- end id2 -->

<!-- npu="910b" id3 -->
- Atlas A2 Training Series products/Atlas A2 Inference Series products, supported data types are: int8_t, uint8_t, int16_t, uint16_t, half, bfloat16_t, int32_t, uint32_t, float, int64_t, uint64_t, double.
<!-- end id3 -->

<!-- npu="310p" id4 -->
- Atlas Inference Series AI Core supports the following data types: int8_t, uint8_t, int16_t, uint16_t, half, int32_t, uint32_t, float, int64_t, uint64_t, double.
<!-- end id4 -->

<!-- npu="910" id5 -->
- Atlas training products support the following data types: int8_t, uint8_t, int16_t, uint16_t, half, int32_t, uint32_t, float, int64_t, uint64_t, double.
<!-- end id5 -->

## Return Value<a name="section44801012174220"></a>

None

## Constraints<a name="section633mcpsimp"></a>

- The address in the Unified Buffer must be 32-byte aligned.
<!-- npu="A3,910b" id9 -->
- When either blockCount or blockLen in the DataCopyParams structure is 0, this API is treated as a NOP (no operation). This description applies to the following models:
  <!-- npu="A3" id6 -->
  - Atlas A3 training products/Atlas A3 inference products
  <!-- end id6 -->
  <!-- npu="910b" id7 -->
  - Atlas A2 training products/Atlas A2 inference products
  <!-- end id7 -->
<!-- end id9 -->
- The values of the DataCopyParams structure parameters must be within the value ranges:

    **Table 4**  Value ranges of DataCopyParams structure parameters

    | Parameter | Value Range |
    | --- | --- |
    | blockCount | [0, 4095] |
    | blockLen | [0, 65535] |
    | srcGap | [0, 65535] |
    | dstGap | [0, 65535] |

<!-- npu="910b,A3" id8 -->
- For the following product models:
    - Atlas A2 training products/Atlas A2 inference products
    - Atlas A3 training products/Atlas A3 inference products

    In cross-card communication operator development scenarios, the DataCopy APIs support cross-card data transfer only over the HCCS physical link, and do not support other paths. During development, you need to pay attention to the physical paths involved in inter-card communication. You can run the `npu-smi info -t topo` command to query the HCCS physical link.
<!-- end id8 -->

## Example<a name="section122101199486"></a>

- Scenario 1: Continuous transfer - continuously transfer 128 half elements from the source data to the destination operand (corresponding to [Figure 1](#zh-cn_topic_0000002535088926_fig4621152141711))

    ```cpp
    // srcLocal and dstLocal are LocalTensors of the half type.
    // Use the transfer API that takes the count parameter to complete the continuous transfer.
    AscendC::DataCopy(dstLocal, srcLocal, 128);
    
    // Use the transfer API that takes the DataCopyParams parameter to achieve the same continuous transfer effect.
    // blockCount = 1: the number of continuous data blocks is 1.
    // blockLen = 8: the length of one data block is 8 DataBlocks (128 half elements / (32B/2B) = 8).
    // srcGap = 0, dstGap = 0: no gap, continuous transfer.
    DataCopyParams params;
    params.blockCount = 1;
    params.blockLen = 8;
    params.srcGap = 0;
    params.dstGap = 0;
    AscendC::DataCopy(dstLocal, srcLocal, params);
    ```

- Scenario 2: Non-continuous transfer - transfer 2 data blocks from the source data, each data block containing 3 DataBlocks, with a source gap of 1 DataBlock and a destination gap of 2 DataBlocks (corresponding to [Figure 2](#zh-cn_topic_0000002535088926_fig422819202304))

    ```cpp
    // srcLocal and dstLocal are LocalTensors of the half type.
    // The source data shape is [1, 112], with 112 half elements (7 Data Blocks).
    // The destination receives 2 non-contiguously arranged Data Blocks, for a total of 6 Data Blocks (96 half elements).
    // blockCount = 2: transfer 2 Data Blocks
    // blockLen = 3: each Data Block has a length of 3 Data Blocks
    // srcGap = 1: adjacent source Data Blocks are spaced 1 Data Block apart
    // dstGap = 2: adjacent destination Data Blocks are spaced 2 Data Blocks apart
    DataCopyParams params;
    params.blockCount = 2;
    params.blockLen = 3;
    params.srcGap = 1;
    params.dstGap = 2;
    AscendC::DataCopy(dstLocal, srcLocal, params);
    ```
