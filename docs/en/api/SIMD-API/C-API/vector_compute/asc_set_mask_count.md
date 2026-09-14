# asc_set_mask_count

<!-- md-trans-meta sourceCommit=0ece502ce5e30f12c91b14fc87b28b1b051cb796 translatedAt=2026-09-05T15:16:52.929Z pushedAt=2026-09-08T03:45:05.580Z -->

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

Sets the mask mode to Counter mode. In this mode, developers do not need to be aware of the number of iterations or handle unaligned tail blocks. They can directly pass in the number of data elements to be computed, and the actual number of iterations is automatically inferred by the Vector computing unit. This mode is automatically set inside the APIs for computing the first n data elements, so it does not need to be set manually.

## Prototype

```cpp
__aicore__ inline void asc_set_mask_count()
```

## Parameters

None

## Return Value

None

## Pipeline Type

PIPE_S

## Constraints

This API must be used together with [asc_set_vector_mask](asc_set_vector_mask.md).

## Example

```cpp
asc_set_mask_count();
asc_set_vector_mask(0, static_cast<uint64_t>(64)); // Set the first 64 elements to participate in the computation.
... // Computation operation.
asc_set_mask_norm();    // Restore Normal mode. This step can be set as needed.
```