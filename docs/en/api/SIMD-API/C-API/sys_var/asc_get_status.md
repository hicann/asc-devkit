# asc_get_status

<!-- md-trans-meta sourceCommit=0ece502ce5e30f12c91b14fc87b28b1b051cb796 translatedAt=2026-09-05T13:14:42.871Z pushedAt=2026-09-08T03:45:05.425Z -->

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

Obtains the status information.

## Prototype

```cpp
__aicore__ inline int64_t asc_get_status()
```

## Parameters

None

## Return Value

Status information. The meaning of each bit is as follows:
| Bit range    | Meaning |
| ----------- |:----|
| 5 | Floating-point operation overflow. Overflow in int16_t and int32_t arithmetic operations of SIMD instructions is also reported to this bit. |
| 6 | Floating-point operation underflow (when the result is smaller than the minimum value representable by a denormalized number, the result is 0). |
| 7 | When converting any floating-point number to an unsigned integer, the input is negative. |
| 8 | Overflow occurs during data movement from L0C to UB (float->half, int32_t->half). |
| 9 | Underflow occurs during data movement from L0C to UB (float->half). |
| 10 | CUBE accumulation operation overflow (may be float, half, or int32_t). |
| 11 | CUBE accumulation operation underflow (may be float or half). |
| 13 | Scalar instruction input is NaN/INF. |
| 14 | Vector instruction input is NaN/INF. |
| 15 | CUBE instruction input is NaN/INF. |
| 61 | Data movement instruction input is NaN/INF. |
| Other bits | Reserved bits. |

## Pipeline Type

PIPE_S

## Constraints

None

## Example

```cpp
int64_t status = asc_get_status();
printf("status is %x", status);// Use %x to print it as a hexadecimal number.
```