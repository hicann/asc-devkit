# asc_sflbits

<!-- md-trans-meta sourceCommit=0ece502ce5e30f12c91b14fc87b28b1b051cb796 translatedAt=2026-09-05T12:34:14.697Z pushedAt=2026-09-08T03:45:05.359Z -->

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

Computes the number of consecutive bits, starting from the most significant value bit, that are identical to the sign bit in the binary representation of an int64_t number.

- Example: int64_t value = 0x0f00000000000000;

  The sign bit is 0, and starting from the most significant value bit (excluding the sign bit), there are 3 consecutive bits identical to the sign bit, so 3 is returned.

## Prototype

```cpp
__aicore__ inline int64_t asc_sflbits(int64_t value)
```

## Parameters

| Parameter  | Input/Output | Description |
| :----- | :------- | :------- |
| value | Input | Input data. |

## Return Value

Returns the number of consecutive bits, starting from the most significant value bit, that are identical to the sign bit. The data type is int64_t.

## Pipeline Type

PIPE_S

## Constraints

When the input is -1 (all bits are 1) or 0 (all bits are 0), -1 is returned.

## Example

```cpp
int64_t val = 0x000f00ff0ff0ffff;
int64_t res = 0;
res = asc_sflbits(val);    // Returns 11.
```