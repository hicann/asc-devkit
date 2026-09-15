# SetAtomicType<a name="ZH-CN_TOPIC_0000001835083633"></a>

<!-- md-trans-meta sourceCommit=e3c8c70b3f5d27f3770063ae4b535bc75d904e3e translatedAt=2026-08-27T14:14:50.357Z -->

## Applicable Products<a name="section1550532418810"></a>

<!-- npu="950" id13 -->
- Ascend 950PR/Ascend 950DT: Supported
<!-- end id13 -->
<!-- npu="A3" id14 -->
- Atlas A3 training products/Atlas A3 inference products: Supported
<!-- end id14 -->
<!-- npu="910b" id15 -->
- Atlas A2 training products/Atlas A2 inference products: Supported
<!-- end id15 -->
<!-- npu="310b" id16 -->
- Atlas 200I/500 A2 inference products: Supported
<!-- end id16 -->
<!-- npu="310p" id17 -->
- Atlas inference products AI Core: Supported
<!-- end id17 -->
<!-- npu="310p" id18 -->
- Atlas inference products Vector Core: Not supported
<!-- end id18 -->
<!-- npu="910" id19 -->
- Atlas training products: Not supported
<!-- end id19 -->

## Description<a name="section618mcpsimp"></a>

The header file path is `"basic_api/kernel_operator_set_atomic_intf.h"`.

Different data types for atomic operations are set by configuring the template parameter.

## Prototype<a name="section620mcpsimp"></a>

```cpp
template <typename T>
__aicore__ inline void SetAtomicType()
```

## Parameters<a name="section622mcpsimp"></a>

**Table 1**  Template parameter description

| Parameter | Description |
|--------|------|
| **T** | Sets different data types for atomic operations. |

## Data Types

<!-- npu="950" id1 -->
- Ascend 950PR/Ascend 950DT: supported data type is int8_t, int16_t, half, bfloat16_t, int32_t, float.
<!-- end id1 -->

<!-- npu="A3" id2 -->
- Atlas A3 training products/Atlas A3 inference products: supported data type is int8_t, int16_t, half, bfloat16_t, int32_t, float.
<!-- end id2 -->

<!-- npu="910b" id3 -->
- Atlas A2 training products/Atlas A2 inference products: supported data type is int8_t, int16_t, half, bfloat16_t, int32_t, float.
<!-- end id3 -->

<!-- npu="310b" id4 -->
- Atlas 200I/500 A2 inference products: supported data type is int16_t, half, int32_t, float.
<!-- end id4 -->

<!-- npu="310p" id5 -->
- Atlas inference products AI Core: supported data type is int16_t, half, float.
<!-- end id5 -->

## Return Value<a name="section640mcpsimp"></a>

None

## Constraints<a name="section633mcpsimp"></a>

- [SetAtomicAdd](SetAtomicAdd.md), [SetAtomicMax](SetAtomicMax_ISASI.md), and [SetAtomicMin](SetAtomicMin_ISASI.md) already integrate the same functionality as SetAtomicType. When calling the preceding APIs, it is recommended that you explicitly specify the data type of the atomic operation by setting the template parameter, without the need to additionally call the SetAtomicType API.
- After use, it is recommended that you use DisableDmaAtomic to clear the atomic operation state to avoid affecting the functionality of subsequent related instructions.

## Example<a name="section177231425115410"></a>

According to the constraints, when using [SetAtomicAdd](SetAtomicAdd.md), [SetAtomicMax](SetAtomicMax_ISASI.md), or [SetAtomicMin](SetAtomicMin_ISASI.md), the data type of the atomic operation is already specified through the template parameter, so there is no need to additionally call the `SetAtomicType` API. Therefore, no example is provided for this API.
