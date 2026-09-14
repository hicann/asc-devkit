# asc_set_atomic_max

<!-- md-trans-meta sourceCommit=0ece502ce5e30f12c91b14fc87b28b1b051cb796 translatedAt=2026-09-05T12:39:33.714Z pushedAt=2026-09-08T03:45:05.367Z -->

## Applicable Products

<!-- npu="950" id3 -->
- Ascend 950PR/Ascend 950DT: Supported
<!-- end id3 -->
<!-- npu="A3" id4 -->
- Atlas A3 training products/Atlas A3 inference products: Supported
<!-- end id4 -->
<!-- npu="910b" id5 -->
- Atlas A2 training products/Atlas A2 inference products: Supported
<!-- end id5 -->
<!-- npu="310b" id6 -->
- Atlas 200I/500 A2 inference products: Not supported
<!-- end id6 -->
<!-- npu="310p" id7 -->
- Atlas inference products AI Core: Not supported
<!-- end id7 -->
<!-- npu="310p" id8 -->
- Atlas inference products Vector Core: Not supported
<!-- end id8 -->
<!-- npu="910" id9 -->
- Atlas training products: Not supported
<!-- end id9 -->

## Description

Enables the atomic compare-and-max operation for subsequent data transfers from Unified Buffer/L0C Buffer/L1 Buffer to Global Memory. Supported data types include int8_t, int16_t, int32_t, bfloat16_t, half, and float.
<!-- npu="950" id1 -->
In particular, for Ascend 950PR/Ascend 950DT, the path from L1 Buffer to Global Memory is not supported.
<!-- end id1 -->

## Prototype

```cpp
__aicore__ inline void asc_set_atomic_max_int8()
__aicore__ inline void asc_set_atomic_max_int16()
__aicore__ inline void asc_set_atomic_max_int()
__aicore__ inline void asc_set_atomic_max_bfloat()
__aicore__ inline void asc_set_atomic_max_float16()
__aicore__ inline void asc_set_atomic_max_float()
```

## Parameters

None

## Return Value

None

## Pipeline Type

PIPE_S

## Constraints

- After use, it is recommended to disable the atomic max operation through [asc_set_atomic_none](./asc_set_atomic_none.md) to avoid affecting the functionality of subsequent related instructions.
<!-- npu="950" id2 -->
- For Ascend 950PR/Ascend 950DT, the path from L1 Buffer to Global Memory is not supported.
<!-- end id2 -->

## Example

```cpp
// total_length indicates the length of the data involved in the computation, and dst is the externally input int8_t GM memory address.
constexpr uint32_t total_length = 256;
__ubuf__ int8_t src0[total_length];
__ubuf__ int8_t src1[total_length];

asc_copy_ub2gm(dst, src0, total_length * sizeof(int8_t));
asc_sync_pipe(PIPE_MTE3);
asc_set_atomic_max_int8();
asc_copy_ub2gm(dst, src1, total_length * sizeof(int8_t));
asc_set_atomic_none();
```

Result example:

```
Input data src0: [1, 1, 1, ..., 1]  // int8_t type
Input data src1: [2, 2, 2, ..., 2]  // int8_t type
Output data dst: [2, 2, 2, ..., 2]   // int8_t type
```
