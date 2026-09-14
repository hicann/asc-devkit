# asc_clz

<!-- md-trans-meta sourceCommit=0ece502ce5e30f12c91b14fc87b28b1b051cb796 translatedAt=2026-09-05T12:28:55.706Z pushedAt=2026-09-08T03:45:05.350Z -->

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

Computes the number of leading zeros in the binary representation of a uint64_t integer, that is, the number of consecutive zeros from the most significant bit to the first occurrence of a binary 1.

## Prototype

```c++
__aicore__ inline int64_t asc_clz(uint64_t value_in)
```

## Parameters

|Parameter|Input/Output|Description|
|------------|------------|-----------|
| value_in     | Input     | Number to be counted.|

## Return Value

Returns the number of leading zeros in value_in.

## Pipeline Type

PIPE_S

## Constraints

None

## Example

```c++
uint64_t value_in = 0x0fffffffffffffff;
int64_t ans = asc_clz(value_in); //Returns ans = 4.
```
