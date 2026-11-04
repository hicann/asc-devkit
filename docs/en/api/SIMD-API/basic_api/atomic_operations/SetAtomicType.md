# SetAtomicType<a name="ZH-CN_TOPIC_0000001835083633"></a>

<!-- md-trans-meta sourceCommit=754fb95859dead826727a591e674261c6a829a08 translatedAt=2026-09-23T10:36:56.583Z pushedAt=2026-10-26T07:41:43.963Z nl=2 -->

## Applicable Products<a name="section1550532418810"></a>

<!-- npu="950" id13 -->
- Ascend 950PR&950DT products: supported
<!-- end id13 -->
<!-- npu="A3" id14 -->
- Atlas A3 products: supported
<!-- end id14 -->
<!-- npu="910b" id15 -->
- Atlas A2 products: supported
<!-- end id15 -->
<!-- npu="310b" id16 -->
- Atlas 200I/500 A2 inference product: supported
<!-- end id16 -->
<!-- npu="310p" id17 -->
- Atlas inference products AI Core: supported
<!-- end id17 -->
<!-- npu="310p" id18 -->
- Atlas inference products Vector Core: not supported
<!-- end id18 -->
<!-- npu="910" id19 -->
- Atlas training products: not supported
<!-- end id19 -->
<!-- @ref: asc-devkit/res/docs/zh/api/SIMD-API/basic_api/atomic_operations/SetAtomicType_res.md#id1 -->

## Description<a name="section618mcpsimp"></a>

The header file path is `"basic_api/kernel_operator_set_atomic_intf.h"`.

Sets different data types for atomic operations by configuring template parameters.

## Function Prototype<a name="section620mcpsimp"></a>

```cpp
template <typename T>
__aicore__ inline void SetAtomicType()
```

## Parameters<a name="section622mcpsimp"></a>

**Table 1**  Template parameter description

| Parameter | Description |
|--------|------|
| **T** | Sets different data types for the atomic operation. |

## Data Types

<!-- npu="950" id1 -->
- For the Ascend 950PR&950DT products, the supported data types are int8_t, int16_t, half, bfloat16_t, int32_t, and float.
<!-- end id1 -->

<!-- npu="A3" id2 -->
- Atlas A3 products supports data types int8_t, int16_t, half, bfloat16_t, int32_t, and float.
<!-- end id2 -->

<!-- npu="910b" id3 -->
- Atlas A2 products, the supported data types are int8_t, int16_t, half, bfloat16_t, int32_t, and float.
<!-- end id3 -->

<!-- npu="310b" id4 -->
- Atlas 200I/500 A2 inference products, the supported data types are int16_t, half, int32_t, and float.
<!-- end id4 -->

<!-- npu="310p" id5 -->
- Atlas inference products AI Core supports data types int16_t, half, and float.
<!-- end id5 -->

## Return Value<a name="section640mcpsimp"></a>

None

## Constraints<a name="section633mcpsimp"></a>

- [SetAtomicAdd](SetAtomicAdd.md), [SetAtomicMax](SetAtomicMax_ISASI.md), and [SetAtomicMin](SetAtomicMin_ISASI.md) already integrate the same functionality as SetAtomicType. When calling these APIs, it is recommended to explicitly specify the data type of the atomic operation by setting the template parameter, without the need to additionally call the SetAtomicType API.
- After use, it is recommended to use **DisableDmaAtomic** to clear the state of the atomic operation to avoid affecting the functionality of subsequent related instructions.

## Example<a name="section177231425115410"></a>

According to the constraints, when using [SetAtomicAdd](SetAtomicAdd.md), [SetAtomicMax](SetAtomicMax_ISASI.md), and [SetAtomicMin](SetAtomicMin_ISASI.md), the data type of the atomic operation is already specified through the template parameter, so there is no need to call the `SetAtomicType` API additionally. Therefore, this API does not provide an example.
