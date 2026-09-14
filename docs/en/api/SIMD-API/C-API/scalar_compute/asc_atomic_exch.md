# asc_atomic_exch

<!-- md-trans-meta sourceCommit=2bc2b38d5648dc61f632186b01ba7f36c2295b13 translatedAt=2026-09-05T12:20:36.245Z pushedAt=2026-09-08T03:45:05.338Z -->

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

Performs an atomic assignment operation on the element pointed to by **address** in Global Memory, that is, sets the element value to **val**.

## Prototype

```cpp
__aicore__ inline float asc_atomic_exch(__gm__ float *address, float val)

__aicore__ inline int32_t asc_atomic_exch(__gm__ int32_t *address, int32_t val)

__aicore__ inline uint32_t asc_atomic_exch(__gm__ uint32_t *address, uint32_t val)

__aicore__ inline int64_t asc_atomic_exch(__gm__ int64_t *address, int64_t val)

__aicore__ inline uint64_t asc_atomic_exch(__gm__ uint64_t *address, uint64_t val)
```

## Parameters

| Parameter | Input/Output | Description |
| --- | --- | --- |
| address | Output | Address in Global Memory. |
| val | Input | Source operand. |

## Return Value

Returns the original data at the **address** before the computation.

## Pipeline Type

PIPE_S

## Constraints

- With the compiler auto-synchronization feature enabled, the compiler can automatically insert synchronization between PIPE_MTE2/PIPE_MTE3 and PIPE_S. However, **asc_atomic_exch** is a scalar computation. When reading from or writing to GM, if there is a data dependency with the transfer units (MTE2/MTE3), the compiler cannot automatically insert synchronization, and developers need to manually insert synchronization based on the actual situation.
- Scalar atomic operations bypass DCache. Call the [asc_dcci](../cache_ctrl/asc_dcci.md) API to ensure consistency between GM and DCache.

## Example

```cpp
int32_t dst = 10;          // Initial value.
asc_dcci_entire_all();            // Manually synchronize.
int32_t old = asc_atomic_exch(&dst, 20);  // dst = 20, old = 10
```
