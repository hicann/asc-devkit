# SetStoreAtomicConfig\(ISASI\)<a name="ZH-CN_TOPIC_0000001790206710"></a>

<!-- md-trans-meta sourceCommit=22e1e8c65e425d4c0fb07294311f2eac049ab0b3 translatedAt=2026-11-02T02:19:39.965Z pushedAt=2026-11-18T01:25:06.595Z nl=2 -->

## Applicable Products<a name="section1550532418810"></a>

<!-- npu="950" id8 -->
- Ascend 950PR&950DT products: supported
<!-- end id8 -->
<!-- npu="A3" id9 -->
- Atlas A3 products: supported
<!-- end id9 -->
<!-- npu="910b" id10 -->
- Atlas A2 products: supported
<!-- end id10 -->
<!-- npu="310b" id11 -->
- Atlas 200I/500 A2 inference products: supported
<!-- end id11 -->
<!-- npu="310p" id12 -->
- Atlas inference products AI Core: not supported
<!-- end id12 -->
<!-- npu="310p" id13 -->
- Atlas inference products Vector Core: not supported
<!-- end id13 -->
<!-- npu="910" id14 -->
- Atlas training products: not supported
<!-- end id14 -->
<!-- @ref: asc-devkit/res/docs/zh/api/SIMD-API/basic_api/atomic_operations/SetStoreAtomicConfig_ISASI_res.md#id1 -->

## Description<a name="section618mcpsimp"></a>

The header file path is `"basic_api/kernel_operator_common_intf.h"`.

Sets the atomic operation enable bit and the atomic operation type.

## Prototype<a name="section620mcpsimp"></a>

```cpp
template <AtomicDtype type, AtomicOp op>
__aicore__ inline void SetStoreAtomicConfig()
```

## Parameters<a name="section622mcpsimp"></a>

<a name="table622mcpsimp"></a>**Table 1** Template parameter description

| Parameter | Input/Output | Description |
|--------|-----------|------|
| type | Input | Atomic operation enable bit. The AtomicDtype enum class is defined as follows:<br>enum class AtomicDtype {<br>    ATOMIC_NONE = 0,  // No atomic operation.<br>    ATOMIC_F32,       // Enable atomic operation. The data type for the atomic operation is float.<br>    ATOMIC_F16,       // Enable atomic operation. The data type for the atomic operation is half.<br>    ATOMIC_S16,       // Enable atomic operation. The data type for the atomic operation is int16_t.<br>    ATOMIC_S32,       // Enable atomic operation. The data type for the atomic operation is int32_t.<br>    ATOMIC_S8,        // Enable atomic operation. The data type for the atomic operation is int8_t.<br>    ATOMIC_BF16       // Enable atomic operation. The data type for the atomic operation is bfloat16_t.<br>}; |
| op | Input | Atomic operation type. It is valid only when the atomic operation is enabled (that is, when "type" is not "ATOMIC_NONE"). Currently, only the sum operation is supported.<br>enum class AtomicOp {<br>    ATOMIC_SUM = 0   // Sum operation.<br>}; |

## Data Types

The supported data types are int8_t, int16_t, half, bfloat16_t, int32_t, and float.

## Return Value<a name="section640mcpsimp"></a>

None

## Constraints<a name="section633mcpsimp"></a>

None

## Example<a name="section837496171220"></a>

```cpp
// Set the atomic operation to sum, with half as the supported data type.
AscendC::SetStoreAtomicConfig<AscendC::AtomicDtype::ATOMIC_F16, AscendC::AtomicOp::ATOMIC_SUM>();
```
