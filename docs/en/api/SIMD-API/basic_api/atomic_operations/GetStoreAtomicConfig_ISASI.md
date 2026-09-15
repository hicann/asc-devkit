# GetStoreAtomicConfig\(ISASI\)<a name="ZH-CN_TOPIC_0000001836926165"></a>

<!-- md-trans-meta sourceCommit=e3c8c70b3f5d27f3770063ae4b535bc75d904e3e translatedAt=2026-08-27T14:11:26.655Z -->

## Applicable Products<a name="section1550532418810"></a>

<!-- npu="950" id8 -->
- Ascend 950PR/Ascend 950DT: Supported
<!-- end id8 -->
<!-- npu="A3" id9 -->
- Atlas A3 training products/Atlas A3 inference products: Supported
<!-- end id9 -->
<!-- npu="910b" id10 -->
- Atlas A2 training products/Atlas A2 inference products: Supported
<!-- end id10 -->
<!-- npu="310b" id11 -->
- Atlas 200I/500 A2 inference products: Supported
<!-- end id11 -->
<!-- npu="310p" id12 -->
- Atlas inference products AI Core: Not supported
<!-- end id12 -->
<!-- npu="310p" id13 -->
- Atlas inference products Vector Core: Not supported
<!-- end id13 -->
<!-- npu="910" id14 -->
- Atlas training products: Not supported
<!-- end id14 -->

## Description<a name="section618mcpsimp"></a>

The header file path is `"basic_api/kernel_operator_common_intf.h"`.

Obtains the values of the atomic operation enable bit and the atomic operation type. For details, see [Table 1](SetStoreAtomicConfig_ISASI.md#table622mcpsimp).

## Prototype<a name="section620mcpsimp"></a>

```cpp
__aicore__ inline void GetStoreAtomicConfig(uint16_t& atomicType, uint16_t& atomicOp)
```

## Parameters<a name="section622mcpsimp"></a>

**Table 1**  Parameters

| Parameter | Input/Output | Description |
|--------|-----------|------|
| atomicType | Output | Atomic operation enable bit.<br>**0**: No atomic operation.<br>**1**: Enables the atomic operation, and the data type for the atomic operation is **float**.<br>**2**: Enables the atomic operation, and the data type for the atomic operation is **half**.<br>**3**: Enables the atomic operation, and the data type for the atomic operation is **int16_t**.<br>**4**: Enables the atomic operation, and the data type for the atomic operation is **int32_t**.<br>**5**: Enables the atomic operation, and the data type for the atomic operation is **int8_t**.<br>**6**: Enables the atomic operation, and the data type for the atomic operation is **bfloat16_t**. |
| atomicOp | Output | Atomic operation type.<br>**0**: Sum operation. |

## Return Value<a name="section640mcpsimp"></a>

None

## Constraints<a name="section633mcpsimp"></a>

This API must be used together with [SetStoreAtomicConfig(ISASI)](SetStoreAtomicConfig_ISASI.md) to obtain the values of the atomic operation enable bit and the atomic operation type.

## Example<a name="section837496171220"></a>

```cpp
AscendC::SetStoreAtomicConfig<AscendC::AtomicDtype::ATOMIC_F16, AscendC::AtomicOp::ATOMIC_SUM>();
uint16_t type = 0;       // Atomic operation enable bit.
uint16_t op = 0;         // Atomic operation type.
AscendC::GetStoreAtomicConfig(type, op);
```
