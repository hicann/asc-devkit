# asc_set_store_atomic_config_v1

<!-- md-trans-meta sourceCommit=0ece502ce5e30f12c91b14fc87b28b1b051cb796 translatedAt=2026-09-05T12:42:40.309Z pushedAt=2026-09-08T03:45:05.371Z -->

## Applicable Products

<!-- npu="950" id1 -->
- Ascend 950PR/Ascend 950DT: Not supported
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
- Atlas inference products AI Core: Not supported
<!-- end id5 -->
<!-- npu="310p" id6 -->
- Atlas inference products Vector Core: Not supported
<!-- end id6 -->
<!-- npu="910" id7 -->
- Atlas training products: Not supported
<!-- end id7 -->

## Description

Sets the atomic operation enable bit and the atomic operation type value.

## Prototype

```cpp
__aicore__ inline void asc_set_store_atomic_config_v1(uint16_t type, uint16_t op)
```

## Parameters

| Parameter  | Input/Output | Description |
| :----- | :------- | :------- |
| type | Input | Atomic operation enable bit.<br>**0**: No atomic operation.<br>**1**: Enable atomic, and the data type of the operand for the atomic operation is float.<br>**2**: Enable atomic, and the data type of the operand for the atomic operation is half.<br>**3**: Enable atomic, and the data type of the operand for the atomic operation is int16_t.<br>**4**: Enable atomic, and the data type of the operand for the atomic operation is int32_t.<br>**5**: Enable atomic, and the data type of the operand for the atomic operation is int8_t.<br>**6**: Enable atomic, and the data type of the operand for the atomic operation is bfloat16_t.<br>Other values have no specific meaning. |
| op | Input | Atomic operation type.<br>**0**: Sum operation.<br>Other values have no specific meaning. |

## Return Value

None

## Pipeline Type

PIPE_S

## Constraints

None

## Example

```cpp
uint16_t type = 1;
uint16_t op = 0;
asc_set_store_atomic_config_v1(type, op);
```