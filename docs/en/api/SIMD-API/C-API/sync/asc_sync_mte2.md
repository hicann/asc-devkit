# asc_sync_mte2

<!-- md-trans-meta sourceCommit=0ece502ce5e30f12c91b14fc87b28b1b051cb796 translatedAt=2026-09-05T12:54:29.331Z pushedAt=2026-09-08T03:45:05.394Z -->

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

Performs a synchronization operation on the **PIPE_MTE2** pipeline.

## Prototype

```cpp
__aicore__ inline void asc_sync_mte2(int id)
```

## Parameters

| Parameter | Input/Output | Description |
| :--- | :--- | :--- |
| id | Input | Synchronization ID. |

## Return Value

None

## Pipeline Type

PIPE_S

## Constraints

None

## Example

```cpp
// In this example, total_length indicates the total length of the data involved in the computation. src0_gm, src1_gm, and dst_gm are externally input source operands and the destination operand of the float type, pointing to GM memory space.
constexpr uint32_t total_length = 128;
__ubuf__ float src0[total_length];
__ubuf__ float src1[total_length];
__ubuf__ float dst[total_length];

asc_copy_gm2ub(src0, src0_gm, total_length * sizeof(float));
asc_copy_gm2ub(src1, src1_gm, total_length * sizeof(float));

// Synchronization operation: The subsequent operation can start only after the data transfer operation (GM to UB, PIPE_MTE2 pipeline) is complete.
asc_sync_mte2(0);

asc_add(dst, src1, src0, total_length);

// Synchronization operation: The subsequent operation can start only after the preceding operation is complete.
asc_sync();

asc_copy_ub2gm(dst_gm, dst, total_length * sizeof(float));
```
