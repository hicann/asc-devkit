# UB-to-UB Continuous Data Transfer (DataCopy)<a name="ZH-CN_TOPIC_0000002382908021"></a>

<!-- md-trans-meta sourceCommit=f6490d1e030d257ee55f3f1600bffece5a8bd3f7 translatedAt=2026-09-09T13:35:32.301Z -->

## Applicable Products<a name="section1550532418810"></a>

<!-- npu="950" id14 -->
- Ascend 950PR/Ascend 950DT: Supported
<!-- end id14 -->
<!-- npu="A3" id15 -->
- Atlas A3 training products/Atlas A3 inference products: Supported
<!-- end id15 -->
<!-- npu="910b" id16 -->
- Atlas A2 training products/Atlas A2 inference products: Supported
<!-- end id16 -->
<!-- npu="310b" id17 -->
- Atlas 200I/500 A2 inference products: Not supported
<!-- end id17 -->
<!-- npu="310p" id18 -->
- Atlas inference products AI Core: Supported
<!-- end id18 -->
<!-- npu="310p" id19 -->
- Atlas inference products Vector Core: Not supported
<!-- end id19 -->
<!-- npu="910" id20 -->
- Atlas training products: Supported
<!-- end id20 -->

## Description<a name="section474617392321"></a>

Header file path: `"basic_api/kernel_operator_data_copy_intf.h"`.

Supports continuous data copy between Unified Buffers. The data retains its original format and content during transmission.

The supported data paths are as follows (represented by [logical position TPosition](../../aux_data_structures/TPosition.md)):

- Unified Buffer -> Unified Buffer
    - VECIN -> VECCALC
    - VECCALC -> VECOUT

## Prototype<a name="section1954364615315"></a>

```cpp
template <typename T>
__aicore__ inline void DataCopy(const LocalTensor<T>& dst, const LocalTensor<T>& src, const uint32_t count)
```

## Parameters<a name="section622mcpsimp"></a>

**Table 1** Template parameters

| Parameter | Description |
| :--- | :--- |
| T | Data type of the operand. For supported data types, see [Data Type](#section4219135304818). |

**Table 2** Parameters

| Parameter | Input/Output | Description |
| :--- | :---: | :--- |
| dst | Output | Destination operand. The type is [LocalTensor](../../data_structures/LocalTensor/localtensor_introduction.md), the storage location is Unified Buffer, and the destination address requires 32-byte alignment. |
| src | Input | Source operand. The type is LocalTensor, the storage location is Unified Buffer, and the source address requires 32-byte alignment. |
| count | Input | Number of elements to be moved.<br>Note: count * sizeof(T) requires 32-byte alignment. If it is not aligned, the amount of data moved is rounded down to a multiple of 32 bytes. |

Take the half data type as an example. The shape of the source operand is 1 \* 128. When count = 128, [Figure 1](#zh-cn_topic_0000002567699425_fig13481537531) continuously moves 128 elements from the source operand to the destination operand.

**Figure 1** Continuous data copy diagram<a name="zh-cn_topic_0000002567699425_fig13481537531"></a>  
![](../../../../figures/continuous_data_copy_diagram.png "Continuous data copy diagram")

## Data Type<a name="section4219135304818"></a>

The source operand and destination operand support the same data types, as follows:

<!-- npu="950" id1 -->
- Ascend 950PR/Ascend 950DT: the supported data types are bool, int8_t, uint8_t, hifloat8_t, fp8_e8m0_t, fp8_e5m2_t, fp8_e4m3fn_t, int16_t, uint16_t, half, bfloat16_t, int32_t, uint32_t, float, complex32, int64_t, uint64_t, double, and complex64.
<!-- end id1 -->

<!-- npu="A3" id2 -->
- Atlas A3 training products/Atlas A3 inference products: the supported data types are int8_t, uint8_t, int16_t, uint16_t, half, bfloat16_t, int32_t, uint32_t, float, int64_t, uint64_t, and double.
<!-- end id2 -->

<!-- npu="910b" id3 -->
- Atlas A2 training products/Atlas A2 inference products: the supported data types are int8_t, uint8_t, int16_t, uint16_t, half, bfloat16_t, int32_t, uint32_t, float, int64_t, uint64_t, and double.
<!-- end id3 -->

<!-- npu="310p" id4 -->
- Atlas inference products AI Core: the supported data types are int8_t, uint8_t, int16_t, uint16_t, half, int32_t, uint32_t, float, int64_t, uint64_t, and double.
<!-- end id4 -->

<!-- npu="910" id5 -->
- Atlas training products: the supported data types are int8_t, uint8_t, int16_t, uint16_t, half, int32_t, uint32_t, float, int64_t, uint64_t, and double.
<!-- end id5 -->

## Return Value<a name="section44801012174220"></a>

None

## Constraints<a name="section633mcpsimp"></a>

- The address in the Unified Buffer must be 32-byte aligned.
- When calling the continuous copy API, count \* sizeof\(T\) must be 32-byte aligned. If it is not aligned, the copy amount is rounded down to 32-byte alignment.
<!-- npu="910b,A3" id6 -->
- For the following product models:
    - Atlas A2 training products/Atlas A2 inference products
    - Atlas A3 training products/Atlas A3 inference products

    In cross-card communication operator development scenarios, the DataCopy APIs support cross-card data copy only over the HCCS physical link, and do not support other paths. During development, developers need to pay attention to the physical path involved in inter-card communication. The HCCS physical link can be queried by running the `npu-smi info -t topo` command.
<!-- end id6 -->

## Example<a name="section122101199486"></a>

```cpp
// srcLocal and dstLocal are LocalTensors of the half type.
// Use the copy API that takes the count parameter to complete continuous copy.
AscendC::DataCopy(dstLocal, srcLocal, 512);
```

Result example:

```text
Input data srcLocal: [1 2 3 ... 512]
Output data dstLocal: [1 2 3 ... 512]
```
