# DataCopy (L1ToGM High-Dimensional Split Data Copy)<a name="ZH-CN_TOPIC_0000002594407261"></a>

<!-- md-trans-meta sourceCommit=8fa2928a9ba79777fe5da4f7c9987e59a88f273e translatedAt=2026-11-02T03:47:48.752Z pushedAt=2026-11-18T01:25:06.736Z nl=2 -->

## Applicable Products<a name="zh-cn_topic_0000002540558032_section796754519912"></a>

<!-- npu="950" id1 -->
- Ascend 950PR&950DT products: not supported
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
- Atlas inference products AI Core: not supported
<!-- end id5 -->
<!-- npu="310p" id6 -->
- Atlas inference products Vector Core: not supported
<!-- end id6 -->
<!-- npu="910" id7 -->
- Atlas training products: not supported
<!-- end id7 -->
<!-- npu="x90" id8 -->
- Kirin X90 processor products: Supported
<!-- end id8 -->
<!-- npu="9030" id9 -->
- Kirin 9030 processor products: Supported
<!-- end id9 -->
<!-- @ref: asc-devkit/res/docs/zh/api/SIMD-API/basic_api/cube_compute_ISASI/cube_compute_store/DataCopy_L1ToGM_highdim_split_res.md#id1 -->

## Description<a name="zh-cn_topic_0000002540558032_section106841136114319"></a>

The header file path is `"basic_api/kernel_operator_data_copy_intf.h"`.

This API mainly moves the matrix from the L1 Buffer to Global Memory. The format and content remain unchanged during data movement.

High-dimensional slicing means that non-continuous movement can be implemented by configuring movement parameters such as the number of data blocks, the length of a single data block, and the address offset.

## Prototype<a name="zh-cn_topic_0000002540558032_section82039854412"></a>

The API supports both non-contiguous copy and continuous copy:

```cpp
template <typename T>
__aicore__ inline void DataCopy(const GlobalTensor<T>& dst, const LocalTensor<T>& src, const DataCopyParams& repeatParams)
```

For continuous copy scenarios, [DataCopy (L1ToGM Continuous Copy)](DataCopy_L1ToGM_continuous.md) is recommended.

## Parameters<a name="zh-cn_topic_0000002540558032_section16128134420472"></a>

**Table 1** Template parameters

| Parameter | Description |
| ---------- | ---------- |
| T | Data Type of the source operand or destination operand. For supported data types, see [Data Type](#zh-cn_topic_0000002540558032_section4219135304818). |

**Table 2** Parameters

| Parameter | Input/Output | Meaning |
| ---------- | ---------- | ---------- |
| dst | Output | Destination operand, of type **GlobalTensor**, stored in Global Memory, with 1-byte alignment required for the address. |
| src | Input | Source operand, of type **LocalTensor**, stored in L1 Buffer, with 32-byte alignment required for the address. |
| repeatParams | Input | Copy parameters, of type **DataCopyParams**. This parameter configures the number of data blocks, length, address gap, and other information for the copy, and supports both non-continuous and continuous copy.<br>For details about the **DataCopyParams** parameters, see [Table 3](#zh-cn_topic_0000002540558032_table144203616291). |

**Table 3** DataCopyParams structure parameter definition<a id="zh-cn_topic_0000002540558032_table144203616291"></a>

| Parameter | Meaning |
| ---------- | ---------- |
| blockCount | Number of data blocks to copy. Type: **uint16_t**. Value range: blockCount ∈ [0, 4095]. Default value: 1.<br>**Note: blockCount = 0 means no copy is performed, and the API is treated as a NOP (no operation).** |
| blockLen | Length of each data block to copy. Type: **uint16_t**. Value range: blockLen ∈ [0, 65535]. Unit: DataBlock (32 bytes).<br>**Note: blockLen = 0 means no copy is performed, and the API is treated as a NOP (no operation).** |
| srcGap | Gap between adjacent data blocks of the source operand (that is, the difference between the **end address** of the previous data block and the **start address** of the next data block). Type: **uint16_t**. Value range: srcGap ∈ [0, 65535]. Unit: DataBlock (32 bytes).<br>&bull; When blockCount = 1, srcGap is meaningless and can be set to 0. |
| dstGap | Gap between adjacent data blocks of the destination operand (that is, the difference between the **end address** of the previous data block and the **start address** of the next data block). Type: **uint16_t**. Value range: dstGap ∈ [0, 65535]. Unit: DataBlock (32 bytes).<br>&bull; When blockCount = 1, dstGap is meaningless and can be set to 0. |

The following figure shows how to use the **DataCopyParams** structure parameters. In the example, two contiguous data blocks are copied, each containing 8 DataBlocks. There is no gap between adjacent data blocks of the source operand, and the destination operand has a gap of 1 DataBlock between the tail and head of adjacent data blocks.

![](../../../../figures/repeat-times.png)

## Data Type<a id="zh-cn_topic_0000002540558032_section4219135304818"></a>

The source matrix and destination matrix support the same data types.

<!-- npu="A3" id10 -->
For Atlas A3 products, the supported data types are: int8_t, uint8_t, int16_t, uint16_t, half, bfloat16_t, int32_t, uint32_t, float, int64_t, uint64_t, double.
<!-- end id10 -->

<!-- npu="910b" id11 -->
For Atlas A2 products, the supported data types are: int8_t, uint8_t, int16_t, uint16_t, half, bfloat16_t, int32_t, uint32_t, float, int64_t, uint64_t, double.
<!-- end id11 -->

<!-- npu="x90" id12 -->
For Kirin X90 processor products, the supported data types are: int8_t, uint8_t, int16_t, uint16_t, half, int32_t, uint32_t, float, int64_t, uint64_t, double.
<!-- end id12 -->

<!-- npu="9030" id13 -->
For Kirin 9030 processor products, the supported data types are: int8_t, uint8_t, int16_t, uint16_t, half, int32_t, uint32_t, float, int64_t, uint64_t, double.
<!-- end id13 -->

## Return Value

None

## Constraints<a name="zh-cn_topic_0000002540558032_section2045914466492"></a>

- The source address in the L1 Buffer must be 32-byte aligned, and the destination address in Global Memory must be 1-byte aligned.
- When either **blockCount** or **blockLen** in the **DataCopyParams** structure parameter is 0, the instruction is not executed.
<!-- npu="A3,910b" id14 -->
- For the following product models:
    <!-- npu="A3" id15 -->
    Atlas A3 products
    <!-- end id15 -->
    <!-- npu="910b" id16 -->
    Atlas A2 products
    <!-- end id16 -->
    In the cross-card communication operator development scenario, the **DataCopy** class APIs support cross-card data copy only over the HCCS physical link, and do not support other paths. During development, pay attention to the physical path involved in inter-card communication. You can run the npu-smi info -t topo command to query the HCCS physical link.
<!-- end id14 -->
- The values of the **DataCopyParams** structure parameters must be within the value range:

    **Table 4** Value range of the DataCopyParams structure parameters

    | Parameter | Value Range |
    | ---------- | ---------- |
    | blockCount | [1, 4095] |
    | blockLen | [1, 65535] |
    | srcGap | [0, 65535] |
    | dstGap | [0, 65535] |

## Key Features<a name="zh-cn_topic_0000002540558032_section1084342081618"></a>

**Continuous Copy**

For continuous copy scenarios, [DataCopy (L1ToGM Contiguous Data Copy)](DataCopy_L1ToGM_continuous.md#ZH-CN_TOPIC_0000002563687928) is recommended. The following example only demonstrates that the high-dimensional split data copy API supports continuous copy.

Taking the half data type as an example, the shape of the source operand is 1 \* 128.

[Figure 1](#zh-cn_topic_0000002540558032_fig187115784817) continuously copies 128 elements from the source operand to the destination operand. The **DataCopyParams** structure parameters are configured as follows:

- blockCount = 1, copying 1 data block, indicating continuous copying.
- blockLen = 8, one data block has a length of 8 DataBlocks.
- srcGap = 0, with an inter-block gap of 0 between adjacent data blocks in the source operand.
- dstGap = 0, with an inter-block gap of 0 between adjacent data blocks in the destination operand.

**Figure 1** Continuous copy diagram<a id="zh-cn_topic_0000002540558032_fig187115784817"></a>  

![](../../../../figures/continuous_data_copy_diagram.png)

**Discontinuous Copy**

Take the half data type as an example. The shape of the source operand is 1 \* 112.

[Figure 2](#zh-cn_topic_0000002540558032_fig123821924175312) copies the discontinuous data in the source operand to the destination operand. The **DataCopyParams** structure parameters are configured as follows:

- blockCount = 2, copying 2 data blocks.
- blockLen = 3, the length of one data block is 3 Data Blocks.
- srcGap = 1, the gap between adjacent data blocks in the source operand is 1.
- dstGap = 2, the gap between adjacent data blocks in the destination operand is 2.

**Figure 2** Non-contiguous copy diagram<a id="zh-cn_topic_0000002540558032_fig123821924175312"></a>  

![](../../../../figures/discontinuous_data_copy_diagram.png)

## Example<a name="zh-cn_topic_0000002540558032_section088124295117"></a>

Using the scenario shown in [Figure 2](#zh-cn_topic_0000002540558032_fig123821924175312) as an example, the following shows a call example for non-contiguous copy:

```cpp
constexpr uint32_t srcElemCount = 112;
constexpr uint32_t dstElemCount = 128;

// Source operand: L1 Buffer, arranged as "48 valid half + 16 skipped half + 48 valid half".
AscendC::LocalTensor<half> srcLocal(AscendC::TPosition::A1, a1Addr, srcElemCount);

// Destination operand: 128 half reserved in GM, with a 32-half gap between two valid data blocks.
AscendC::GlobalTensor<half> dstGm;
dstGm.SetGlobalBuffer((__gm__ half *)dst, dstElemCount);

AscendC::DataCopyParams repeatParams;
// Copy two continuous transfer data blocks.
repeatParams.blockCount = 2;
// Each data block is 3 DataBlocks long.
repeatParams.blockLen = 3;
// Skip 1 DataBlock between adjacent data blocks on the source side.
repeatParams.srcGap = 1;
// Leave a gap of 2 DataBlocks between adjacent data blocks on the destination side.
repeatParams.dstGap = 2;
AscendC::DataCopy(dstGm, srcLocal, repeatParams);
```
