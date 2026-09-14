# asc_set_store_atomic_config_v2 (Deprecated)

<!-- md-trans-meta sourceCommit=0ece502ce5e30f12c91b14fc87b28b1b051cb796 translatedAt=2026-09-05T12:43:20.297Z pushedAt=2026-09-08T03:45:05.376Z -->

## Applicable Products

<!-- npu="950" id1 -->
- Ascend 950PR/Ascend 950DT: Supported
<!-- end id1 -->
<!-- npu="A3" id2 -->
- Atlas A3 training products/Atlas A3 inference products: Not supported
<!-- end id2 -->
<!-- npu="910b" id3 -->
- Atlas A2 training products/Atlas A2 inference products: Not supported
<!-- end id3 -->
<!-- npu="310b" id4 -->
- Atlas 200I/500 A2 inference products: Not supported
<!-- end id4 -->
<!-- npu="310p" id5 -->
- Atlas inference products AI Core: Not supported
<!-- end id5 -->
<!-- npu="310p" id6 -->
- Atlas inference products Vector Core: Not supported
<!-- end id6 -->
<!-- npu="910" id7 -->
- Atlas training products: Not supported
<!-- end id7 -->

## Description

**This API is deprecated. For atomic addition operations, use [asc_atomic_add](../scalar_compute/asc_atomic_add.md) instead.**

Sets the values of the atomic operation enable bit and the atomic operation type.

## Prototype

```cpp
__aicore__ inline void asc_set_store_atomic_config_v2(uint16_t type, uint16_t op)
```

## Parameters

| Parameter | Input/Output | Description |
| :----- | :------- | :------- |
| type | Input | Atomic operation enable bit.<br>1: Enable atomic operations, and the data type of the operand for performing atomic operations is int32_t.<br>4: Enable atomic operations, and the data type of the operand for performing atomic operations is half.<br>5: Enable atomic operations, and the data type of the operand for performing atomic operations is float.<br>7: Enable atomic operations, and the data type of the operand for performing atomic operations is bfloat16_t.<br>9: Enable atomic operations, and the data type of the operand for performing atomic operations is int16_t.<br>10: Enable atomic operations, and the data type of the operand for performing atomic operations is int8_t.<br>Other values have no specific meaning. |
| op | Input | Atomic operation type.<br>2: Sum operation.<br>Other values have no specific meaning. |

## Return Value

None

## Pipeline Type

PIPE_S

## Constraints

None

## Example

```cpp
uint16_t type = 1;
uint16_t op = 2;
asc_set_store_atomic_config_v2(type, op);
```
