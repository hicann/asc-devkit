# DataCopy (GMToL1 Continuous Copy)<a id="ZH-CN_TOPIC_0000002569070903"></a>

<!-- md-trans-meta sourceCommit=e9bec8b34ada6c37c20391d212da5ebf088e2e8b translatedAt=2026-08-27T15:57:37.434Z -->

## Applicable Products<a id="zh-cn_topic_0000002535739034_section796754519912"></a>

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
## Description<a id="zh-cn_topic_0000002535739034_section106841136114319"></a>

The header file path is `"basic_api/kernel_operator_data_copy_intf.h"`.

This API continuously copies a cube from Global Memory to the L1 Buffer (with TPosition set to A1/B1), keeping the format and content unchanged during the copy.

## Prototype<a id="zh-cn_topic_0000002535739034_section82039854412"></a>

```cpp
template <typename T>
__aicore__ inline void DataCopy(const LocalTensor<T>& dst, const GlobalTensor<T>& src, const uint32_t count)
```

## Parameters<a id="zh-cn_topic_0000002535739034_section16128134420472"></a>

**Table 1** Template parameter description

| Parameter | Description |
| ---------- | ---------- |
| T | Data type of the source operand or destination operand. For supported data types, see [Data Types](#zh-cn_topic_0000002535739034_section4219135304818). |

**Table 2** Parameter description

| Parameter | Input/Output | Description |
| ---------- | ---------- | ---------- |
| dst | Output | Destination operand, of the **LocalTensor** type, stored in the L1 Buffer (**TPosition** is A1/B1). The destination address must be 32-byte aligned. |
| src | Input | Source operand, of the **GlobalTensor** type, stored in Global Memory. The source address must be 1-byte aligned. |
| count | Input | Number of elements to be copied. count * sizeof(T) must be 32-byte aligned. If it is not aligned, the copy amount is rounded down to 32-byte alignment. |

Taking the half data type as an example, the shape of the source operand is 1 * 128. When count = 128, [Figure 1](#zh-cn_topic_0000002535739034_fig79455329161) continuously copies 128 elements from the source operand to the destination operand.

**Figure 1** Continuous copy diagram<a id="zh-cn_topic_0000002535739034_fig79455329161"></a>

![](../../../../figures/datacopy_gm2l1_continuous_copy.png)

## data type<a id="zh-cn_topic_0000002535739034_section4219135304818"></a>

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

## Constraints<a id="zh-cn_topic_0000002535739034_section2045914466492"></a>

- The source address in Global Memory must be 1-byte aligned, and the destination address in L1 Buffer must be 32-byte aligned.
- When calling the continuous copy API, `count * sizeof(T)` must be 32-byte aligned. If it is not aligned, the copy amount is rounded down to 32-byte alignment.
- If multiple DataCopy instructions need to be executed and their destination addresses overlap, call [PipeBarrier\(ISASI\)](../../sync_control/intra_core_sync/PipeBarrier_ISASI.md) to insert a synchronization instruction, ensuring that multiple DataCopy instructions are serialized to prevent abnormal data. As shown in the left diagram below, when two DataCopy instructions are executed and their destination GM addresses overlap, call `PipeBarrier<PIPE\_MTE3\>\(\)` between the two copy instructions to add synchronization for the MTE3 outbound pipeline. As shown in the right diagram below, when the destination Unified Buffer addresses overlap, call `PipeBarrier<PIPE\_MTE2\>\(\)` between the two copy instructions to add synchronization for the MTE2 inbound pipeline.

    ![](../../../../figures/drawing_4.png)

<!-- npu="A3,910b" id17 -->
- For the following product models:

    <!-- npu="A3" id18 -->
    Atlas A3 training products/Atlas A3 inference products;
    <!-- end id18 -->

    <!-- npu="910b" id19 -->
    Atlas A2 training products/Atlas A2 inference products;
    <!-- end id19 -->

    In cross-card communication operator development scenarios, DataCopy APIs support cross-card data copy only over the HCCS physical link, and do not support other paths. During development, developers need to pay attention to the physical paths involved in inter-card communication. The HCCS physical link can be queried using the `npu-smi info -t topo` command.
<!-- end id17 -->

## Example<a id="zh-cn_topic_0000002535739034_section088124295117"></a>

Take the scenario shown in [Figure 1 Continuous copy diagram](#zh-cn_topic_0000002535739034_fig79455329161) as an example:

```cpp
constexpr uint32_t copyCount = 128;
// Source operand: 128 half elements stored contiguously in GM.
AscendC::GlobalTensor<half> srcGm;
srcGm.SetGlobalBuffer((__gm__ half *)src, copyCount);
// Destination operand: L1 Buffer.
AscendC::LocalTensor<half> dstLocal(AscendC::TPosition::A1, 0, copyCount);
// count = 128, indicating that 128 half elements are copied, with an actual copied byte count of 256B that satisfies the 32B byte alignment constraint.
AscendC::DataCopy(dstLocal, srcGm, copyCount);
```
