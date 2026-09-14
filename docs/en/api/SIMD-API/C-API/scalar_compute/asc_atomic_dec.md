# asc_atomic_dec

<!-- md-trans-meta sourceCommit=2bc2b38d5648dc61f632186b01ba7f36c2295b13 translatedAt=2026-09-05T12:19:37.556Z pushedAt=2026-09-08T03:45:05.334Z -->

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

Performs an atomic decrement operation on the counter pointed to by **address** in Global Memory. If the value at **address** equals 0 or is greater than the specified value **val**, **val** is assigned to **address**; otherwise, the value at **address** is decremented by 1.

## Prototype

```cpp
__aicore__ inline uint32_t asc_atomic_dec(__gm__ uint32_t *address, uint32_t val)

__aicore__ inline uint64_t asc_atomic_dec(__gm__ uint64_t *address, uint64_t val)
```

## Parameters

| Parameter | Input/Output | Description |
| --- | --- | --- |
| address | Output | Address in Global Memory. |
| val | Input | Source operand. |

## Return Value

The original data at the address before the calculation.

## Pipeline Type

PIPE_S

## Constraints

- When the compiler automatic synchronization feature is enabled, the compiler can automatically insert synchronization between PIPE_MTE2/PIPE_MTE3 and PIPE_S. However, **asc_atomic_dec** is a scalar computation. When reading from or writing to GM, if there is a data dependency with the transfer unit (MTE2/MTE3), the compiler cannot automatically insert synchronization, and developers need to manually insert synchronization as appropriate.
- Scalar atomic operations bypass DCache. Call [asc_dcci](../cache_ctrl/asc_dcci.md) to ensure consistency between GM and DCache.

## Example

```cpp
uint32_t counter = 5;      // Initial value.
asc_dcci_entire_all();            // Manual synchronization.
uint32_t old = asc_atomic_dec(&counter, 9);  // counter = 4, old = 5
```
