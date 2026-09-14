# asc_ffz

<!-- md-trans-meta sourceCommit=0ece502ce5e30f12c91b14fc87b28b1b051cb796 translatedAt=2026-09-05T12:30:55.855Z pushedAt=2026-09-08T03:45:05.352Z -->

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

Obtains the position of the first 0, starting from the least significant bit, in the binary representation of a uint64_t number. Returns -1 if no 0 is found.

## Prototype

```c++
__aicore__ inline int64_t asc_ffz(uint64_t value)
```

## Parameters

|Parameter|Input/Output|Description|
| :------ | :--- | :------------ |
|value   |Input   |Input data, of the uint64_t data type.|

## Return Value

Returns the position of the first 0 in value.

## Pipeline Type

PIPE_S

## Constraints

None

## Example

```c++
uint64_t value = 33;
// count_first_zero = 1
int64_t count_first_zero = asc_ffz(value);
```
