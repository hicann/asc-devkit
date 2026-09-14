# asc_set_atomic_none

<!-- md-trans-meta sourceCommit=0ece502ce5e30f12c91b14fc87b28b1b051cb796 translatedAt=2026-09-05T12:41:07.216Z pushedAt=2026-09-08T03:45:05.370Z -->

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

Clears the atomic operation state. It is generally used together with [asc_set_atomic_add](asc_set_atomic_add.md), [asc_set_atomic_max](asc_set_atomic_max.md), and [asc_set_atomic_min](asc_set_atomic_min.md) to disable the atomic operation after it completes, preventing it from affecting subsequent functions.

## Prototype

```c++
__aicore__ inline void asc_set_atomic_none()
```

## Parameters

None
## Return Value

None

## Pipeline Type

PIPE_S

## Constraints

None

## Example

```c++
//total_length indicates the total length of the data to be moved. dst is the externally input int16_t GM memory.
constexpr uint32_t total_length = 256;
__ubuf__ int16_t src0[total_length];
__ubuf__ int16_t src1[total_length];
asc_set_atomic_add_int16();
asc_copy_ub2gm_sync(dst, src0, total_length * sizeof(int16_t));
asc_copy_ub2gm_sync(dst, src1, total_length * sizeof(int16_t));
asc_set_atomic_none();
```

Result example:

```
Input data src0: [1, 1, 1, ..., 1]  // int16_t type
Input data src1: [2, 2, 2, ..., 2]  // int16_t type
Output data dst: [3, 3, 3, ..., 3]   // int16_t type
```