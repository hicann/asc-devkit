# DataCopy (GMToL1 High-Dimensional Segmented Data Copy)<a id="ZH-CN_TOPIC_0000002538231128"></a>

<!-- md-trans-meta sourceCommit=e9bec8b34ada6c37c20391d212da5ebf088e2e8b translatedAt=2026-08-27T16:01:00.112Z -->

## Applicable Products<a id="zh-cn_topic_0000002566658905_section796754519912"></a>

<!-- npu="950" id1 -->
- Ascend 950PR/Ascend 950DT: Supported
<!-- end id1 -->
<!-- npu="A3" id2 -->
- Atlas A3 training products/Atlas A3 inference products: Supported
<!-- end id2 -->
<!-- npu="910b" id3 -->
- Atlas A2 training products/Atlas A2 inference products: Supported
<!-- end id3 -->
<!-- npu="310b" id4 -->
- Atlas 200I/500 A2 inference products: Not supported
<!-- end id4 -->
<!-- npu="310p" id5 -->
- Atlas inference products AI Core: Supported
<!-- end id5 -->
<!-- npu="310p" id6 -->
- Atlas inference products Vector Core: Not supported
<!-- end id6 -->
<!-- npu="910" id7 -->
- Atlas training products: Supported
<!-- end id7 -->
## Description<a id="zh-cn_topic_0000002566658905_section106841136114319"></a>

The header file path is `"basic_api/kernel_operator_data_copy_intf.h"`.

This API mainly copies a cube from **Global Memory** to the **L1 Buffer** (with **TPosition** set to **A1/B1**), keeping the format and content unchanged during the copy.

High-dimensional slicing refers to the ability to implement non-continuous copy by configuring copy parameters such as the number of data blocks, the length of a single data block, and address offsets.

## Prototype<a id="zh-cn_topic_0000002566658905_section82039854412"></a>

The API supports both non-continuous copy and continuous copy:

```cpp
template <typename T>
__aicore__ inline void DataCopy(const LocalTensor<T>& dst, const GlobalTensor<T>& src, const DataCopyParams& repeatParams)
```

Note: For continuous copy scenarios, [DataCopy (GMToL1 continuous data copy)](DataCopy_GMToL1_continuous.md) is recommended.

## Parameters<a id="zh-cn_topic_0000002566658905_section16128134420472"></a>

**Table 1** Template parameter description

| Parameter | Description |
| ---------- | ---------- |
| **T** | Data type of the source operand or destination operand. For supported data types, see [Data Type](#zh-cn_topic_0000002566658905_section4219135304818). |

**Table 2** Parameter description

| Parameter | Input/Output | Description |
| ---------- | ---------- | ---------- |
| **dst** | Output | Destination operand, type is **LocalTensor**, stored in L1 Buffer (**TPosition** is A1/B1). The destination address requires 32-byte alignment. |
| **src** | Input | Source operand, type is **GlobalTensor**, stored in Global Memory. The source address requires 1-byte alignment. |
| **repeatParams** | Input | Copy parameter, type is **DataCopyParams**. This parameter configures the number of data blocks, length, address gap, and other information for the copy, and supports both non-continuous and continuous copy.<br>For details about **DataCopyParams**, see [Table 3](#zh-cn_topic_0000002566658905_table144203616291). |

**Table 3** DataCopyParams structure parameter definition<a id="zh-cn_topic_0000002566658905_table144203616291"></a>

| Parameter | Description |
| ---------- | ---------- |
| **blockCount** | Number of data blocks to copy. Type is **uint16_t**. Value range: blockCount∈[0, 4095]. Default value: 1.<br>**Note: blockCount = 0 means no copy is performed, and this API is considered a NOP (no-op).** |
| **blockLen** | Length of each data block to copy. Type is **uint16_t**. Value range: blockLen∈[0, 65535]. Unit: DataBlock (32 bytes).<br>**Note: blockLen = 0 means no copy is performed, and this API is considered a NOP (no-op).** |
| **srcGap** | Gap between adjacent data blocks of the source operand (that is, the difference between the **end address** of the previous data block and the **start address** of the next data block). Type is **uint16_t**. Value range: srcGap∈[0, 65535]. Unit: DataBlock (32 bytes).<br>&nbsp;&nbsp;&bull; When blockCount = 1, srcGap is meaningless and can be set to 0. |
| **dstGap** | Gap between adjacent data blocks of the destination operand (that is, the difference between the **end address** of the previous data block and the **start address** of the next data block). Type is **uint16_t**. Value range: dstGap∈[0, 65535]. Unit: DataBlock (32 bytes).<br>&nbsp;&nbsp;&bull; When blockCount = 1, dstGap is meaningless and can be set to 0. |

## Data Type<a id="zh-cn_topic_0000002566658905_section4219135304818"></a>

The data types supported by the source cube and the destination cube must be consistent.

<!-- npu="950" id10 -->
For Ascend 950PR/Ascend 950DT, the supported data types are: b8, b16, b32, b64.
<!-- end id10 -->

<!-- npu="A3" id11 -->
For Atlas A3 training products/Atlas A3 inference products, the supported data types are: b8, b16, b32, b64.
<!-- end id11 -->

<!-- npu="910b" id12 -->
For Atlas A2 training products/Atlas A2 inference products, the supported data types are: b8, b16, b32, b64.
<!-- end id12 -->

<!-- npu="310p" id13 -->
For Atlas inference products AI Core, the supported data types are: int8_t, uint8_t, int16_t, uint16_t, int32_t, uint32_t, int64_t, uint64_t, half, float, double.
<!-- end id13 -->

<!-- npu="910" id14 -->
For Atlas training products, the supported data types are: int8_t, uint8_t, int16_t, uint16_t, int32_t, uint32_t, int64_t, uint64_t, half, float, double.
<!-- end id14 -->

## Return Value

None

## Constraints<a id="zh-cn_topic_0000002566658905_section2045914466492"></a>

- The source address in Global Memory must be 1-byte aligned, and the destination address in L1 Buffer must be 32-byte aligned.
- When either **blockCount** or **blockLen** in the **DataCopyParams** structure is 0, the instruction is not executed, and this API is considered a NOP (no-op).
- The values of the **DataCopyParams** structure parameters must fall within their value ranges:

    **Table 4** Value ranges of the DataCopyParams structure parameters

    | Parameter | Value Range |
    | ---------- | ---------- |
    | blockCount | [0, 4095] |
    | blockLen | [0, 65535] |
    | srcGap | [0, 65535] |
    | dstGap | [0, 65535] |

- If multiple DataCopy instructions need to be executed and their destination addresses overlap, call [PipeBarrier\(ISASI\)](../../sync_control/intra_core_sync/PipeBarrier_ISASI.md) to insert a synchronization instruction, ensuring that the multiple DataCopy instructions are serialized and preventing abnormal data. As shown in the left diagram below, when two DataCopy instructions are executed and their destination GM addresses overlap, call PipeBarrier<PIPE\_MTE3\>\(\) between the two copy instructions to synchronize the MTE3 outbound pipeline. As shown in the right diagram below, when the destination Unified Buffer addresses overlap, call PipeBarrier<PIPE\_MTE2\>\(\) between the two copy instructions to synchronize the MTE2 inbound pipeline.

    ![](../../../../figures/Figure 4.png)

<!-- npu="A3,910b" id17 -->
- For the following product models:

    <!-- npu="A3" id18 -->
    Atlas A3 training products/Atlas A3 inference products;
    <!-- end id18 -->

    <!-- npu="910b" id19 -->
    Atlas A2 training products/Atlas A2 inference products;
    <!-- end id19 -->

    In cross-card communication operator development scenarios, the **DataCopy** class APIs support cross-card data copy only over the HCCS physical link and do not support other paths. During development, developers need to pay attention to the physical paths involved in inter-card communication. The HCCS physical link can be queried using the **npu-smi info -t topo** command.
<!-- end id17 -->

## Key Features

### Continuous Copy<a id="zh-cn_topic_0000002566658905_section3150944151518"></a>

For continuous copy scenarios, [DataCopy (GMToL1 continuous data copy)](DataCopy_GMToL1_continuous.md) is recommended. The following example only demonstrates that the high-dimensional split data copy API supports continuous copy.

Taking the half data type as an example, the shape of the source operand is 1 * 128.

[Figure 1](#zh-cn_topic_0000002566658905_fig769164701717) continuously copies 128 elements from the source operand to the destination operand. The **DataCopyParams** structure parameters are configured as follows:

- **blockCount** = 1, which copies one data block, indicating continuous copy.
- **blockLen** = 8, indicating that the length of one data block is 8 DataBlocks.
- **srcGap** = 0, indicating that the gap between adjacent data blocks of the source operand is 0.
- **dstGap** = 0, indicating that the gap between adjacent data blocks of the destination operand is 0.

**Figure 1** Continuous copy diagram<a id="zh-cn_topic_0000002566658905_fig769164701717"></a>

![](../../../../figures/datacopy_gm2l1_continuous_copy_highdim.png)

### Non-continuous Copy<a id="zh-cn_topic_0000002566658905_section1084342081618"></a>

Taking the half data type as an example, the shape of the source operand is 1 * 112.

[Figure 2](#zh-cn_topic_0000002566658905_fig12371910189) copies non-contiguous data from the source operand to the destination operand. The **DataCopyParams** structure parameters are configured as follows:

- **blockCount** = 2, copying two data blocks.
- **blockLen** = 3, the length of one data block is 3 DataBlocks.
- **srcGap** = 1, the gap between adjacent data blocks in the source operand is 1.
- **dstGap** = 2, the gap between adjacent data blocks in the destination operand is 2.

**Figure 2** Non-continuous copy diagram<a id="zh-cn_topic_0000002566658905_fig12371910189"></a>

![](../../../../figures/datacopy_gm2l1_noncontinuous_copy_highdim.png)

## Example<a id="zh-cn_topic_0000002566658905_section088124295117"></a>

For continuous copy, take the scenario shown in [Figure 1 Continuous copy diagram](#zh-cn_topic_0000002566658905_fig769164701717) as an example:

```cpp
constexpr uint32_t copyCount = 128;
// Source Operand: 128 half values are stored contiguously in GM.
AscendC::GlobalTensor<half> srcGm;
srcGm.SetGlobalBuffer((__gm__ half *)src, copyCount);
// Destination Operation: L1 Buffer.
AscendC::LocalTensor<half> dstLocal(AscendC::TPosition::A1, 0, copyCount);

AscendC::DataCopyParams repeatParams;
repeatParams.blockCount = 1; // Copy one data block, indicating continuous copy.
repeatParams.blockLen = 8;   // Each data block is 8 DataBlocks long, that is, 256B, equal to 128 half values.
repeatParams.srcGap = 0;     // When blockCount = 1, the source gap is meaningless and is set to 0.
repeatParams.dstGap = 0;     // When blockCount = 1, the destination gap is meaningless and is set to 0.

AscendC::DataCopy(dstLocal, srcGm, repeatParams);
```

For non-continuous copy, take the scenario shown in [Figure 2 Non-continuous copy diagram](#zh-cn_topic_0000002566658905_fig12371910189) as an example:

```cpp
constexpr uint32_t srcCount = 112;
constexpr uint32_t dstCount = 128;
// Source Operand: 112 half values are stored in GM, arranged as "48 valid elements + 16 skipped elements + 48 valid elements".
AscendC::GlobalTensor<half> srcGm;
srcGm.SetGlobalBuffer((__gm__ half *)src, srcCount);

// Destination operand: L1 Buffer, with an interval of 32 half reserved between two data blocks.
AscendC::LocalTensor<half> dstLocal(AscendC::TPosition::A1, 0, dstCount);

AscendC::DataCopyParams repeatParams;
repeatParams.blockCount = 2; // Copy 2 data blocks.
repeatParams.blockLen = 3;   // Each data block is 3 DataBlocks long, that is, 96B, equal to 48 half.
repeatParams.srcGap = 1;     // Skip 1 DataBlock, that is, 16 half, between two data blocks on the source side.
repeatParams.dstGap = 2;     // Leave an interval of 2 DataBlocks, that is, 32 half, between two data blocks on the destination side.

AscendC::DataCopy(dstLocal, srcGm, repeatParams);
```
