# asc_get_rsvd_count

<!-- md-trans-meta sourceCommit=0ece502ce5e30f12c91b14fc87b28b1b051cb796 translatedAt=2026-09-05T14:14:54.538Z pushedAt=2026-09-08T03:45:05.494Z -->

## Applicable Products

<!-- npu="950" id1 -->
- Ascend 950PR/Ascend 950DT: Not supported
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

This API is used to obtain the number of remaining elements after the [asc_reduce](./asc_reduce.md) operation is executed.

## Prototype

```cpp
  __aicore__ inline int64_t asc_get_rsvd_count()
```

## Parameters

None

## Return Value

Returns the number of remaining elements after the [asc_reduce](./asc_reduce.md) operation is executed.

## Pipeline Type

PIPE_S

## Constraints

- It must be used together with the [asc_reduce](./asc_reduce.md) operation.
- Use a synchronization operation to ensure that [asc_reduce](./asc_reduce.md) has completed before calling this API to obtain the result.

## Example

```cpp
// Initialize dst, src0, src1, and total_length (the data length involved in the computation).
int total_length = 256;
__ubuf__ uint32_t src0[total_length];
__ubuf__ uint32_t src1[total_length];
__ubuf__ uint32_t dst[total_length];
asc_reduce(dst, src0, src1, total_length);
asc_sync_notify(PIPE_V, PIPE_S, 0); // Set the wait and synchronization signals.
asc_sync_wait(PIPE_V, PIPE_S, 0);
int64_t result = asc_get_rsvd_count();  // Obtain the result.
```
