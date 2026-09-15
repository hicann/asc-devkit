# DataCopy (L1ToGM High-Dimensional Segmented Data Copy)<a name="ZH-CN_TOPIC_0000002594407261"></a>

<!-- md-trans-meta sourceCommit=e9bec8b34ada6c37c20391d212da5ebf088e2e8b translatedAt=2026-08-27T15:39:43.335Z -->

## Applicable Products<a name="zh-cn_topic_0000002540558032_section796754519912"></a>

<!-- npu="950" id1 -->
- Ascend 950PR/Ascend 950DT: Not Supported
<!-- end id1 -->
<!-- npu="A3" id2 -->
- Atlas A3 training products/Atlas A3 inference products: Supported
<!-- end id2 -->
<!-- npu="910b" id3 -->
- Atlas A2 training products/Atlas A2 inference products: Supported
<!-- end id3 -->
<!-- npu="310b" id4 -->
- Atlas 200I/500 A2 inference products: Not Supported
<!-- end id4 -->
<!-- npu="310p" id5 -->
- Atlas inference products AI Core: Not Supported
<!-- end id5 -->
<!-- npu="310p" id6 -->
- Atlas inference products Vector Core: Not Supported
<!-- end id6 -->
<!-- npu="910" id7 -->
- Atlas training products: Not Supported
<!-- end id7 -->
## Description<a name="zh-cn_topic_0000002540558032_section106841136114319"></a>

The header file path is `"basic_api/kernel_operator_data_copy_intf.h"`.

This API mainly copies a cube from **L1 Buffer** to **Global Memory**, with the format and content remaining unchanged during the copy.

High-dimensional segmentation refers to the ability to implement non-continuous copy by configuring copy parameters such as the number of data blocks, the length of a single data block, and address offsets.

## Prototype<a name="zh-cn_topic_0000002540558032_section82039854412"></a>

The API supports both non-continuous copy and continuous copy:

```cpp
template <typename T>
__aicore__ inline void DataCopy(const GlobalTensor<T>& dst, const LocalTensor<T>& src, const DataCopyParams& repeatParams)
```

For continuous copy scenarios, [DataCopy (L1ToGM continuous data copy)](DataCopy_L1ToGM_continuous.md) is recommended.

## Parameters<a name="zh-cn_topic_0000002540558032_section16128134420472"></a>

**Table 1** Template parameter description

| Parameter | Description |
| ---------- | ---------- |
| T | Data type of the source operand or destination operand. For supported data types, see [Data Types](#zh-cn_topic_0000002540558032_section4219135304818). |

**Table 2** Parameter description

| Parameter | Input/Output | Description |
| ---------- | ---------- | ---------- |
| dst | Output | Destination operand, of type **GlobalTensor**, stored in **Global Memory**, with an address requiring 1-byte alignment. |
| src | Input | Source operand, of type **LocalTensor**, stored in **L1 Buffer**, with an address requiring 32-byte alignment. |
| repeatParams | Input | Copy parameters, of type **DataCopyParams**. This parameter configures the number of data blocks to copy, the length, the address gap, and other information, and supports both non-continuous and continuous copy.<br>For details about the **DataCopyParams** parameters, see [Table 3](#zh-cn_topic_0000002540558032_table144203616291). |

**Table 3** DataCopyParams structure parameter definition<a id="zh-cn_topic_0000002540558032_table144203616291"></a>

| Parameter | Description |
| ---------- | ---------- |
| blockCount | Number of data blocks to copy. Type: **uint16_t**. Value range: blockCount ∈ [0, 4095]. Default value: 1.<br>**Note: blockCount = 0 indicates that no copy is performed, and this API is treated as a NOP (no operation)**. |
| blockLen | Length of each data block to copy. Type: **uint16_t**. Value range: blockLen ∈ [0, 65535]. Unit: **DataBlock** (32 bytes).<br>**Note: blockLen = 0 indicates that no copy is performed, and this API is treated as a NOP (no operation)**. |
| srcGap | Gap between adjacent data blocks of the source operand (that is, the difference between the **end address** of the preceding data block and the **start address** of the following data block). Type: **uint16_t**. Value range: srcGap ∈ [0, 65535]. Unit: **DataBlock** (32 bytes).<br>&bull; When blockCount = 1, srcGap is meaningless and can be set to 0. |
| dstGap | Gap between adjacent data blocks of the destination operand (that is, the difference between the **end address** of the preceding data block and the **start address** of the following data block). Type: **uint16_t**. Value range: dstGap ∈ [0, 65535]. Unit: **DataBlock** (32 bytes).<br>&bull; When blockCount = 1, dstGap is meaningless and can be set to 0. |

The following figure illustrates how the **DataCopyParams** structure parameters are used. In the example, two continuous data blocks are copied, each containing eight **DataBlock** units. There is no gap between adjacent data blocks of the source operand, while the destination operand has a gap of one **DataBlock** between the end of the preceding data block and the start of the following data block.

![](../../../../figures/repeat-times.png)

## Data Type<a id="zh-cn_topic_0000002540558032_section4219135304818"></a>

The source cube and the destination cube support the same data types.

<!-- npu="A3" id10 -->
For Atlas A3 training products/Atlas A3 inference products, the supported data types are: int8_t, uint8_t, int16_t, uint16_t, half, bfloat16_t, int32_t, uint32_t, float, int64_t, uint64_t, and double.
<!-- end id10 -->

<!-- npu="910b" id11 -->
For Atlas A2 training products/Atlas A2 inference products, the supported data types are: int8_t, uint8_t, int16_t, uint16_t, half, bfloat16_t, int32_t, uint32_t, float, int64_t, uint64_t, and double.
<!-- end id11 -->

## Return Value

None

## Constraints<a name="zh-cn_topic_0000002540558032_section2045914466492"></a>

- The source address in the L1 Buffer must be 32-byte aligned, and the destination address in Global Memory must be 1-byte aligned.
- When either **blockCount** or **blockLen** in the **DataCopyParams** structure is 0, the instruction is not executed.
<!-- npu="A3,910b" id14 -->
- For the following product models:
    <!-- npu="A3" id15 -->
    Atlas A3 training products/Atlas A3 inference products
    <!-- end id15 -->
    <!-- npu="910b" id16 -->
    Atlas A2 training products/Atlas A2 inference products
    <!-- end id16 -->
    In cross-card communication operator development scenarios, the **DataCopy** APIs support cross-card data copy only over the HCCS physical link, and do not support other paths. During development, developers need to pay attention to the physical paths involved in inter-card communication. The HCCS physical link can be queried using the `npu-smi info -t topo` command.
<!-- end id14 -->
- The values of the **DataCopyParams** structure parameters must be within their value ranges:

    **Table 4** Value ranges of the DataCopyParams structure parameters

    | Parameter | Value Range |
    | ---------- | ---------- |
    | blockCount | [1, 4095] |
    | blockLen | [1, 65535] |
    | srcGap | [0, 65535] |
    | dstGap | [0, 65535] |

## Key Features<a name="zh-cn_topic_0000002540558032_section1084342081618"></a>

**Continuous Copy**

For continuous copy scenarios, [DataCopy (L1ToGM Continuous Data Copy)](DataCopy_L1ToGM_continuous.md#ZH-CN_TOPIC_0000002563687928) is recommended. The following example only demonstrates that the high-dimensional segmented data copy API supports continuous copy.

Taking the half data type as an example, the shape of the source operand is 1 \* 128.

[Figure 1](#zh-cn_topic_0000002540558032_fig187115784817) continuously copies 128 elements from the source operand to the destination operand. The **DataCopyParams** structure parameters are configured as follows:

- **blockCount** = 1, which copies one data block, indicating continuous copy.
- **blockLen** = 8, where the length of one data block is 8 **DataBlock**s.
- **srcGap** = 0, where the gap between adjacent data blocks in the source operand is 0.
- **dstGap** = 0, where the gap between adjacent data blocks in the destination operand is 0.

**Figure 1** Continuous copy diagram<a id="zh-cn_topic_0000002540558032_fig187115784817"></a>  

![](../../../../figures/L12GM_HighDim_Continuous_Transporting.png)

**Non-continuous Copy**

Taking the half data type as an example, the shape of the source operand is 1 \* 112.

[Figure 2](#zh-cn_topic_0000002540558032_fig123821924175312) copies non-contiguous data from the source operand to the destination operand. The **DataCopyParams** structure parameters are configured as follows:

- **blockCount** = 2, which copies 2 data blocks.
- **blockLen** = 3, where the length of one data block is 3 DataBlocks.
- **srcGap** = 1, where the gap between adjacent data blocks in the source operand is 1.
- **dstGap** = 2, where the gap between adjacent data blocks in the destination operand is 2.

**Figure 2** Non-continuous copy diagram<a id="zh-cn_topic_0000002540558032_fig123821924175312"></a>  

![](../../../../figures/L12GM_HighDim_Discontinuous_Transporting.png)

## Example<a name="zh-cn_topic_0000002540558032_section088124295117"></a>

```cpp
// dstLocal is a LocalTensor of the half type, and dstGlobal is a GlobalTensor of the half type.
// Use the copy API that takes DataCopyParams parameters, which supports both continuous and non-continuous copy.
DataCopyParams intriParams;
intriParams.blockCount = 1; // The number of continuous data blocks is 1.
intriParams.blockLen = 512 * sizeof(half) / 32; // Length of the continuous data block, in DataBlock units. The length here is 512 half elements.
intriParams.srcGap = 0; // The source operand performs continuous copy.
intriParams.dstGap = 0; // The destination operand is arranged continuously.
AscendC::DataCopy(dstGlobal, dstLocal, intriParams);
```
