# GetStoreAtomicConfig\(ISASI\)<a name="ZH-CN_TOPIC_0000001836926165"></a>

<!-- md-trans-meta sourceCommit=22e1e8c65e425d4c0fb07294311f2eac049ab0b3 translatedAt=2026-11-02T02:12:53.058Z pushedAt=2026-11-18T01:25:06.585Z nl=2 -->

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
<!-- @ref: asc-devkit/res/docs/zh/api/SIMD-API/basic_api/atomic_operations/GetStoreAtomicConfig_ISASI_res.md#id1 -->

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
| atomicType | Output | Atomic operation enable bit.<br>0: No atomic operation.<br>1: Enables atomic operation, with the data type for the atomic operation being float.<br>2: Enables atomic operation, with the data type for the atomic operation being half.<br>3: Enables atomic operation, with the data type for the atomic operation being int16_t.<br>4: Enables atomic operation, with the data type for the atomic operation being int32_t.<br>5: Enables atomic operation, with the data type for the atomic operation being int8_t.<br>6: Enables atomic operation, with the data type for the atomic operation being bfloat16_t. |
| atomicOp | Output | Atomic operation type.<br>0: Sum operation. |

## Return Value<a name="section640mcpsimp"></a>

None

## Constraints<a name="section633mcpsimp"></a>

This API must be used together with [SetStoreAtomicConfig\(ISASI\)](SetStoreAtomicConfig_ISASI.md) to obtain the values of the atomic operation enable bit and the atomic operation type.

## Example<a name="section837496171220"></a>

```cpp
AscendC::SetStoreAtomicConfig<AscendC::AtomicDtype::ATOMIC_F16, AscendC::AtomicOp::ATOMIC_SUM>();
uint16_t type = 0;       // Atomic operation enable bit.
uint16_t op = 0;         // Atomic operation type.
AscendC::GetStoreAtomicConfig(type, op);
```
