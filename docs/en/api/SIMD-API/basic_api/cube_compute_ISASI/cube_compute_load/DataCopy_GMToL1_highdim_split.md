# DataCopy (GMToL1 High-Dimensional Splitting Data Copy)<a id="ZH-CN_TOPIC_0000002538231128"></a>

<!-- md-trans-meta sourceCommit=754fb95859dead826727a591e674261c6a829a08 translatedAt=2026-09-23T11:28:38.536Z pushedAt=2026-10-26T07:41:44.065Z nl=2 -->

## Applicable Products<a id="zh-cn_topic_0000002566658905_section796754519912"></a>

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
<!-- @ref: asc-devkit/res/docs/zh/api/SIMD-API/basic_api/cube_compute_ISASI/cube_compute_load/DataCopy_GMToL1_highdim_split_res.md#id1 -->

## Description<a id="zh-cn_topic_0000002566658905_section106841136114319"></a>

The header file path is `"basic_api/kernel_operator_data_copy_intf.h"`.

This API mainly copies a matrix from Global Memory to the L1 Buffer (with **TPosition** set to **A1/B1**), keeping the format and content unchanged during the copy.

High-dimensional splitting means that non-contiguous copy can be implemented by configuring copy parameters such as the number of data blocks, the length of a single data block, and the address offset.

## Prototype<a id="zh-cn_topic_0000002566658905_section82039854412"></a>

The API supports both non-contiguous copy and continuous copy:

```cpp
template <typename T>
__aicore__ inline void DataCopy(const LocalTensor<T>& dst, const GlobalTensor<T>& src, const DataCopyParams& repeatParams)
```

Note: For continuous copy scenarios, [DataCopy (GMToL1 contiguous data copy)](DataCopy_GMToL1_continuous.md) is recommended.

## Parameters<a id="zh-cn_topic_0000002566658905_section16128134420472"></a>

**Table 1** Template parameter description

| Parameter | Description |
| ---------- | ---------- |
| T | Data type of the source operand or destination operand. For supported data types, see [Data Types](#zh-cn_topic_0000002566658905_section4219135304818). |

**Table 2** Parameter description

| Parameter | Input/Output | Description |
| ---------- | ---------- | ---------- |
| dst | Output | Destination operand, of type **LocalTensor**, stored in the L1 Buffer (**TPosition** is **A1/B1**). The destination address must be 32-byte aligned. |
| src | Input | Source operand, of type **GlobalTensor**, stored in Global Memory. The source address must be 1-byte aligned. |
| repeatParams | Input | Copy parameters, of type **DataCopyParams**. This parameter configures the number of data blocks to copy, the length, the address gap, and other information, and supports both non-continuous and continuous copy.<br>For the **DataCopyParams** parameter description, see [Table 3](#zh-cn_topic_0000002566658905_table144203616291). |

**Table 3** DataCopyParams structure parameter definition<a id="zh-cn_topic_0000002566658905_table144203616291"></a>

| Parameter | Description |
| ---------- | ---------- |
| blockCount | Number of data blocks to copy. Type: **uint16_t**. Value range: blockCount ∈ [0, 4095]. Default value: 1.<br>**Note: blockCount = 0 means no copy is performed, and the API is treated as a NOP (no operation).** |
| blockLen | Length of each data block to copy. Type: **uint16_t**. Value range: blockLen ∈ [0, 65535]. Unit: DataBlock (32 bytes).<br>**Note: blockLen = 0 means no copy is performed, and the API is treated as a NOP (no operation).** |
| srcGap | Gap between adjacent data blocks of the source operand (that is, the difference between the **end address** of the previous data block and the **start address** of the next data block). Type: **uint16_t**. Value range: srcGap ∈ [0, 65535]. Unit: DataBlock (32 bytes).<br>&nbsp;&nbsp;&bull; When blockCount = 1, srcGap is meaningless and can be set to 0. |
| dstGap | Gap between adjacent data blocks of the destination operand (that is, the difference between the **end address** of the previous data block and the **start address** of the next data block). Type: **uint16_t**. Value range: dstGap ∈ [0, 65535]. Unit: DataBlock (32 bytes).<br>&nbsp;&nbsp;&bull; When blockCount = 1, dstGap is meaningless and can be set to 0. |

## Data Type<a id="zh-cn_topic_0000002566658905_section4219135304818"></a>

The source matrix and the destination matrix support the same data types.

<!-- npu="950" id10 -->
For Ascend 950PR&950DT products, the supported data types are: b8, b16, b32, b64.
<!-- end id10 -->

<!-- npu="A3" id11 -->
For Atlas A3 products, the supported data types are: b8, b16, b32, b64.
<!-- end id11 -->

<!-- npu="910b" id12 -->
For Atlas A2 products, the supported data types are: b8, b16, b32, b64.
<!-- end id12 -->

<!-- npu="310p" id13 -->
For the AI Core of Atlas inference products, the supported data types are: int8_t, uint8_t, int16_t, uint16_t, int32_t, uint32_t, int64_t, uint64_t, half, float, double.
<!-- end id13 -->

<!-- npu="910" id14 -->
For Atlas training products, the supported data types are: int8_t, uint8_t, int16_t, uint16_t, int32_t, uint32_t, int64_t, uint64_t, half, float, double.
<!-- end id14 -->

## Return Value

None

## Constraints<a id="zh-cn_topic_0000002566658905_section2045914466492"></a>

- The source address in Global Memory must be 1-byte aligned, and the destination address in the L1 Buffer must be 32-byte aligned.
- When either **blockCount** or **blockLen** in the **DataCopyParams** structure is 0, the instruction is not executed, and the API is treated as a NOP (no operation).
- The values of the **DataCopyParams** structure parameters must be within the value range:

    **Table 4** Value range of the DataCopyParams structure parameters

    | Parameter | Value Range |
    | ---------- | ---------- |
    | blockCount | [0, 4095] |
    | blockLen | [0, 65535] |
    | srcGap | [0, 65535] |
    | dstGap | [0, 65535] |

- If multiple **DataCopy** instructions need to be executed and their destination addresses overlap, call [PipeBarrier\(ISASI\)](../../sync_control/intra_core_sync/PipeBarrier_ISASI.md) to insert a synchronization instruction to serialize the multiple **DataCopy** instructions and prevent abnormal data. As shown in the left diagram below, when two **DataCopy** instructions are executed and their destination GM addresses overlap, call PipeBarrier<PIPE\_MTE3\>\(\) between the two copy instructions to add synchronization for the MTE3 copy-out pipeline. As shown in the right diagram below, when the destination Unified Buffer (UB) addresses overlap, call PipeBarrier<PIPE\_MTE2\>\(\) between the two copy instructions to add synchronization for the MTE2 copy-in pipeline.

    ![](../../../../figures/datacopy_constraints.png)

<!-- npu="A3,910b" id17 -->
- For the following product models:

    <!-- npu="A3" id18 -->
    Atlas A3 products;
    <!-- end id18 -->

    <!-- npu="910b" id19 -->
    Atlas A2 products;
    <!-- end id19 -->

    In the cross-card communication operator development scenario, the **DataCopy** class of APIs supports cross-card data copy over only the **HCCS** physical link, and no other paths are supported. During development, developers need to pay attention to the physical paths involved in inter-card communication. The **HCCS** physical link can be queried using the **npu-smi info -t topo** command.
<!-- end id17 -->

## Key Features

### Continuous Copy<a id="zh-cn_topic_0000002566658905_section3150944151518"></a>

For continuous copy scenarios, [DataCopy (GMToL1 contiguous data copy)](DataCopy_GMToL1_continuous.md) is recommended. The following example only demonstrates that the high-dimensional split data copy API supports continuous copy.

Taking the **half** data type as an example, the shape of the source operand is 1 * 128.

[Figure 1](#zh-cn_topic_0000002566658905_fig769164701717) continuously copies 128 elements from the source operand to the destination operand. The **DataCopyParams** structure parameters are configured as follows:

- **blockCount** = 1, which copies one data block, indicating continuous copy.
- **blockLen** = 8, where one data block is 8 DataBlocks in length.
- **srcGap** = 0, where the gap between adjacent data blocks in the source operand is 0.
- **dstGap** = 0, where the gap between adjacent data blocks in the destination operand is 0.

**Figure 1** Continuous copy diagram<a id="zh-cn_topic_0000002566658905_fig769164701717"></a>

![](../../../../figures/continuous_data_copy_diagram.png)

### Non-contiguous Copy<a id="zh-cn_topic_0000002566658905_section1084342081618"></a>

Take the half data type as an example. The shape of the source operand is 1 * 112.

[Figure 2](#zh-cn_topic_0000002566658905_fig12371910189) copies non-continuous data from the source operand to the destination operand. The **DataCopyParams** structure parameters are configured as follows:

- **blockCount** = 2, which copies 2 data blocks.
- **blockLen** = 3, which means the length of one data block is 3 **DataBlock**s.
- **srcGap** = 1, which means the gap between adjacent data blocks in the source operand is 1.
- **dstGap** = 2, which means the gap between adjacent data blocks in the destination operand is 2.

**Figure 2** Non-contiguous copy diagram<a id="zh-cn_topic_0000002566658905_fig12371910189"></a>

![](../../../../figures/discontinuous_data_copy_diagram.png)

## Example<a id="zh-cn_topic_0000002566658905_section088124295117"></a>

The continuous copy uses the scenario shown in [Figure 1 Continuous copy diagram](#zh-cn_topic_0000002566658905_fig769164701717) as an example:

```cpp
constexpr uint32_t copyCount = 128;
// Source operand: 128 half values stored contiguously in GM.
AscendC::GlobalTensor<half> srcGm;
srcGm.SetGlobalBuffer((__gm__ half *)src, copyCount);
// Destination operand: L1 Buffer.
AscendC::LocalTensor<half> dstLocal(AscendC::TPosition::A1, 0, copyCount);

AscendC::DataCopyParams repeatParams;
repeatParams.blockCount = 1; // Copy one data block, indicating continuous copy.
repeatParams.blockLen = 8;   // Each data block is 8 DataBlocks long, that is, 256B, equal to 128 half values.
repeatParams.srcGap = 0;     // When blockCount = 1, the source gap is meaningless; set it to 0.
repeatParams.dstGap = 0;     // When blockCount = 1, the destination gap is meaningless; set it to 0.

AscendC::DataCopy(dstLocal, srcGm, repeatParams);
```

The non-contiguous copy uses the scenario shown in [Figure 2 Non-contiguous copy diagram](#zh-cn_topic_0000002566658905_fig12371910189) as an example:

```cpp
constexpr uint32_t srcCount = 112;
constexpr uint32_t dstCount = 128;
// Source operand: 112 half values in GM, arranged as "48 valid elements + 16 skipped elements + 48 valid elements".
AscendC::GlobalTensor<half> srcGm;
srcGm.SetGlobalBuffer((__gm__ half *)src, srcCount);

// Destination operand: L1 Buffer, with a gap of 32 half values reserved between two data blocks.
AscendC::LocalTensor<half> dstLocal(AscendC::TPosition::A1, 0, dstCount);

AscendC::DataCopyParams repeatParams;
repeatParams.blockCount = 2; // Copy 2 data blocks.
repeatParams.blockLen = 3;   // Each data block is 3 DataBlocks long, that is, 96B, equal to 48 half values.
repeatParams.srcGap = 1;     // Skip 1 DataBlock between two data blocks on the source side, that is, 16 half values.
repeatParams.dstGap = 2;     // Leave a gap of 2 DataBlocks between two data blocks on the destination side, that is, 32 half values.

AscendC::DataCopy(dstLocal, srcGm, repeatParams);
```
