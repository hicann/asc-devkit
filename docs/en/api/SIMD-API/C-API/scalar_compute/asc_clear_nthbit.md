# asc_clear_nthbit

<!-- md-trans-meta sourceCommit=0ece502ce5e30f12c91b14fc87b28b1b051cb796 translatedAt=2026-09-05T12:28:22.894Z pushedAt=2026-09-08T03:45:05.348Z -->

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

A bit manipulation function that sets the **idx**-th bit of a uint64_t integer **bits** to 0.

## Prototype

```cpp
__aicore__ inline uint64_t asc_clear_nthbit(uint64_t bits, int64_t idx)
```

## Parameters

|Parameter|Input/Output|Description|
|------------|------------|-----------|
| bits    | Input     | The value to be modified.   |
| idx     | Input     | Bit index, indicating the position of the bit to be set to 0.|

## Return Value

Returns the modified uint64_t integer.

## Pipeline Type

PIPE_S

## Constraints

None

## Example

```cpp
uint64_t bits = 0x7;
int64_t idx = 0x2;
uint64_t res = asc_clear_nthbit(bits, idx);
```