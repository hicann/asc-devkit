# asc_zero_bits_cnt

<!-- md-trans-meta sourceCommit=0ece502ce5e30f12c91b14fc87b28b1b051cb796 translatedAt=2026-09-05T12:36:23.278Z pushedAt=2026-09-08T03:45:05.363Z -->

## Applicable Products

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
- Atlas inference products AI Core: Not supported
<!-- end id5 -->
<!-- npu="310p" id6 -->
- Atlas inference products Vector Core: Not supported
<!-- end id6 -->
<!-- npu="910" id7 -->
- Atlas training products: Not supported
<!-- end id7 -->

## Description

Obtains the number of 0s in the binary representation of a `uint64_t` value.

## Prototype

```c++
__aicore__ inline int64_t asc_zero_bits_cnt(uint64_t value)
```

## Parameters

| Parameter | Input/Output | Description |
| :------ | :--- | :------------ |
| value | Input | The binary digit to be counted. |

## Return Value

Number of 0s in **value**.

## Pipeline Type

PIPE_S

## Constraints

None

## Example

```c++
uint64_t value = 33;
int64_t count_zero = asc_zero_bits_cnt(value);
```
