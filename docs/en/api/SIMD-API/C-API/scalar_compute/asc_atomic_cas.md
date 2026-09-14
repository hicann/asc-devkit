# asc_atomic_cas

<!-- md-trans-meta sourceCommit=2bc2b38d5648dc61f632186b01ba7f36c2295b13 translatedAt=2026-09-05T12:19:37.482Z pushedAt=2026-09-08T03:45:05.333Z -->

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

Performs an atomic compare-and-swap operation on the element pointed to by **address** in Global Memory. If the value at **address** equals the specified value **compare**, **address** is assigned the specified value **val**; otherwise, the value at **address** remains unchanged.

## Prototype

```cpp
__aicore__ inline float asc_atomic_cas(__gm__ float *address, float compare, float val)

__aicore__ inline int32_t asc_atomic_cas(__gm__ int32_t *address, int32_t compare, int32_t val)

__aicore__ inline uint32_t asc_atomic_cas(__gm__ uint32_t *address, uint32_t compare, uint32_t val)

__aicore__ inline int64_t asc_atomic_cas(__gm__ int64_t *address, int64_t compare, int64_t val)

__aicore__ inline uint64_t asc_atomic_cas(__gm__ uint64_t *address, uint64_t compare, uint64_t val)
```

## Parameters

| Parameter | Input/Output | Description |
| --- | --- | --- |
| address | Output | Address in Global Memory. |
| compare | Input | Source operand, the value to be compared. |
| val | Input | Source operand, the value to be assigned. |

## Return Value

Returns the original data at **address** before the operation.

## Pipeline Type

PIPE_S

## Constraints

- With the compiler's automatic synchronization feature enabled, the compiler can automatically insert synchronization between PIPE_MTE2/PIPE_MTE3 and PIPE_S. However, **asc_atomic_cas** is a scalar operation. When reading from or writing to GM, if there is a data dependency with the data movement units (MTE2/MTE3), the compiler cannot automatically insert synchronization, and developers must manually insert synchronization as needed.
- Scalar atomic operations bypass the DCache. Call the [asc_dcci](../cache_ctrl/asc_dcci.md) API to ensure consistency between GM and the DCache.

## Example

```cpp
int32_t dst = 10;          // Initial value.
asc_dcci_entire_all();            // Manually synchronize.
int32_t old = asc_atomic_cas(&dst, 10, 20);  // dst = 20, old = 10
```
