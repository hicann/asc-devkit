# asc_sync_pipe

<!-- md-trans-meta sourceCommit=0ece502ce5e30f12c91b14fc87b28b1b051cb796 translatedAt=2026-09-05T12:58:45.208Z pushedAt=2026-09-08T03:45:05.400Z -->

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

Completes synchronization control within the same pipeline, used to constrain the execution order within a single pipeline. It ensures that all data read and write operations of preceding instructions are fully completed before subsequent instructions can execute.

## Prototype

```cpp
__aicore__ inline void asc_sync_pipe(pipe_t pipe)
```

## Parameters

| Parameter | Input/Output | Description |
| :--- | :--- | :--- |
| pipe | Input | Pipeline category to be blocked. **PIPE_S** is not supported. If you do not care about the pipeline category and want to block all pipelines, you can pass **PIPE_ALL**. |

## Return Value

None

## Pipeline Type

PIPE_S

## Constraints

- Synchronization between Scalar pipelines is automatically guaranteed by the hardware. The **asc_sync_pipe** API does not support synchronization of the single **PIPE_S** pipeline.
- **asc_sync_pipe(PIPE_ALL)** waits for all previously submitted APIs in all pipelines to complete, which impacts performance. If blocking only a single pipeline can solve the problem, avoid calling **asc_sync_pipe(PIPE_ALL)** arbitrarily.
- For **PIPE_MTE2**/**PIPE_MTE3**, developers need to insert synchronization when the transfer addresses overlap.

## Example

```cpp
// src0_gm and src1_gm are externally input float pointers to the GM memory space, with 128 and 32 valid data elements respectively. The 32 consecutive data elements in src0_gm need to be replaced with the data in src1_gm.
__ubuf__ float src0_ub[128];            // Receive the 128 data elements in src0_gm.
__ubuf__ float* src1_ub = src0_ub + 64; // Receive the 32 data elements from src1_gm. They are located within the valid space of src0_ub.

asc_copy_gm2ub(src0_ub, src0_gm, 128 * sizeof(float));

// Insert synchronization to ensure that the overlapping region uses the data from src1_gm as the authoritative source.
asc_sync_pipe(PIPE_MTE2);

asc_copy_gm2ub(src1_ub, src1_gm, 32 * sizeof(float));

/*
 * **PIPE_ALL** can block all pipelines to ensure that the GM -> UB transfer operation is completed.
 * Note that **asc_sync_pipe(PIPE_ALL)** is used here only to illustrate its purpose. In actual application scenarios, flexibly use APIs such as **asc_sync_mte2** and **asc_sync_notify**/**asc_sync_wait** for precise control to improve performance.
 */
asc_sync_pipe(PIPE_ALL);

// Complete the data replacement and copy the data out to src0_gm.
asc_copy_ub2gm(src0_gm, src0_ub, 128 * sizeof(float));
```
