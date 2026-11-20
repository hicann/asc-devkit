# DataCopy (GMToL1 Continuous Copy)<a id="ZH-CN_TOPIC_0000002569070903"></a>

<!-- md-trans-meta sourceCommit=0f9a59471afc8678a19cd607420c13ec5e8e0290 translatedAt=2026-11-02T02:54:33.298Z pushedAt=2026-11-18T01:25:06.683Z nl=2 -->

## Applicable Products<a id="zh-cn_topic_0000002535739034_section796754519912"></a>

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
<!-- npu="x90" id8 -->
- Kirin X90 processor products: Supported
<!-- end id8 -->
<!-- npu="9030" id9 -->
- Kirin 9030 processor products: Supported
<!-- end id9 -->
<!-- @ref: asc-devkit/res/docs/zh/api/SIMD-API/basic_api/cube_compute_ISASI/cube_compute_load/DataCopy_GMToL1_continuous_res.md#id1 -->

## Description<a id="zh-cn_topic_0000002535739034_section106841136114319"></a>

The header file path is `"basic_api/kernel_operator_data_copy_intf.h"`.

This API moves the matrix from Global Memory to L1 Buffer continuously (TPosition is A1/B1), and the format and content remain unchanged during data movement.

## Prototype<a id="zh-cn_topic_0000002535739034_section82039854412"></a>

```cpp
template <typename T>
__aicore__ inline void DataCopy(const LocalTensor<T>& dst, const GlobalTensor<T>& src, const uint32_t count)
```

## Parameters<a id="zh-cn_topic_0000002535739034_section16128134420472"></a>

**Table 1** Template parameters

| Parameter | Description |
| ---------- | ---------- |
| T | Data type of the source operand or destination operand. For supported data types, see [data type](#zh-cn_topic_0000002535739034_section4219135304818). |

**Table 2** Parameters

| Parameter | Input/Output | Meaning |
| ---------- | ---------- | ---------- |
| dst | Output | Destination operand, of the **LocalTensor** type, stored in the L1 Buffer (**TPosition** is A1/B1). The destination address must be 32-byte aligned. |
| src | Input | Source operand, of the **GlobalTensor** type, stored in Global Memory. The source address must be 1-byte aligned. |
| count | Input | Number of elements to be copied.<br>**Note: count * sizeof(T) must be 32-byte aligned. If it is not aligned, the copy amount is rounded down to 32-byte alignment.** |

Taking the half data type as an example, the shape of the source operand is 1 * 128. When count = 128, [Figure 1](#zh-cn_topic_0000002535739034_fig79455329161) continuously copies 128 elements from the source operand to the destination operand.

**Figure 1** Continuous copy diagram<a id="zh-cn_topic_0000002535739034_fig79455329161"></a>

![](../../../../figures/continuous_data_copy_diagram.png)

## Data Type<a id="zh-cn_topic_0000002535739034_section4219135304818"></a>

The source matrix and destination matrix support the same data types.

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

<!-- npu="x90" id15 -->
For Kirin X90 processor products, the supported data types are: int8_t, uint8_t, int16_t, uint16_t, int32_t, uint32_t, int64_t, uint64_t, half, float, double.
<!-- end id15 -->

<!-- npu="9030" id16 -->
For Kirin 9030 processor products, the supported data types are: int8_t, uint8_t, int16_t, uint16_t, int32_t, uint32_t, int64_t, uint64_t, half, float, double.
<!-- end id16 -->

<!-- @ref: asc-devkit/res/docs/zh/api/SIMD-API/basic_api/cube_compute_ISASI/cube_compute_load/DataCopy_GMToL1_continuous_res.md#id2 -->

## Return Value

None

## Constraints<a id="zh-cn_topic_0000002535739034_section2045914466492"></a>

- The source address in Global Memory must be 1-byte aligned, and the destination address in L1 Buffer must be 32-byte aligned.
- When calling the continuous copy API, count * sizeof(T) must be 32-byte aligned. If it is not aligned, the copy amount is rounded down to 32-byte alignment.
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

    In the cross-card communication operator development scenario, the **DataCopy** class APIs support cross-card data copy only over the HCCS physical link, and do not support other paths. During development, pay attention to the physical path involved in inter-card communication. You can run the npu-smi info -t topo command to query the HCCS physical link.
<!-- end id17 -->
<!-- npu="950" id20 -->
- On NPU architecture version 3510, the internal implementation of this API occupies the inter-core synchronization flagId. If it is used together with CrossCoreSetFlag/CrossCoreWaitFlag, avoid flagId conflicts. For details, see [Inter-Core Synchronization flagId Usage](../../sync_control/inter_core_sync/key_features.md#inter_core_sync_flagid_usage).
<!-- end id20 -->

## Example<a id="zh-cn_topic_0000002535739034_section088124295117"></a>

Take the scenario shown in [Figure 1 Continuous copy diagram](#zh-cn_topic_0000002535739034_fig79455329161) as an example:

```cpp
constexpr uint32_t copyCount = 128;
// Source operand: 128 half-type data elements stored contiguously in GM.
AscendC::GlobalTensor<half> srcGm;
srcGm.SetGlobalBuffer((__gm__ half *)src, copyCount);
// Destination operand: L1 Buffer.
AscendC::LocalTensor<half> dstLocal(AscendC::TPosition::A1, 0, copyCount);
// count = 128, indicating that 128 half-type data elements are copied, and the actual 256B of copied data satisfies the 32B alignment constraint.
AscendC::DataCopy(dstLocal, srcGm, copyCount);
```
