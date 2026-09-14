# asc_atomic_inc

<!-- md-trans-meta sourceCommit=2bc2b38d5648dc61f632186b01ba7f36c2295b13 translatedAt=2026-09-05T12:22:09.778Z pushedAt=2026-09-08T03:45:05.340Z -->

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

Performs an atomic increment operation on the counter pointed to by **address** in Global Memory. If the value at **address** is greater than or equal to the specified value **val**, **address** is set to 0; otherwise, the value at **address** is incremented by 1.

## Prototype

```cpp
__aicore__ inline uint32_t asc_atomic_inc(__gm__ uint32_t *address, uint32_t val)

__aicore__ inline uint64_t asc_atomic_inc(__gm__ uint64_t *address, uint64_t val)
```

## Parameters

| Parameter | Input/Output | Description |
| --- | --- | --- |
| address | Output | Address in Global Memory. |
| val | Input | Source operand. |

## Return Value

Returns the original data at **address** before the computation.

## Pipeline Type

PIPE_S

## Constraints

- When the compiler automatic synchronization feature is enabled, the compiler can automatically insert synchronization between PIPE_MTE2/PIPE_MTE3 and PIPE_S. However, **asc_atomic_inc** is a scalar computation. When reading from or writing to GM, if there is a data dependency with the transfer units (MTE2/MTE3), the compiler cannot automatically insert synchronization, and developers need to manually insert synchronization based on the actual situation.
- Scalar atomic operations bypass the DCache. Call the [asc_dcci](../cache_ctrl/asc_dcci.md) API to ensure consistency between GM and the DCache.

## Example

```cpp
uint32_t counter = 0;      // Initial value.
asc_dcci_entire_all();            // Manual synchronization.
uint32_t old = asc_atomic_inc(&counter, 9);  // counter = 1, old = 0
```
