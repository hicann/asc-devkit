# Enabling Double Buffer<a name="ZH-CN_TOPIC_0000001893038945"></a>

<!-- md-trans-meta sourceCommit=fcfc9d3c5c94ebc7869767a984d4f85589421dc0 translatedAt=2026-11-09T10:37:40.489Z pushedAt=2026-11-19T06:32:19.798Z nl=2 -->

[Priority] Medium

[Description] The instruction queues executed on the AI Core mainly include the following types: the [Vector](../../../technical_appendix/concepts_and_terms/glossary.md#p203923211517) instruction queue V, the [Cube](../../../technical_appendix/concepts_and_terms/glossary.md#p1038918214519) instruction queue M, the [Scalar](../../../technical_appendix/concepts_and_terms/glossary.md#p1739112219516) instruction queue S, and the data movement instruction queues [Memory Transfer Engine 1 (MTE1)](../../../technical_appendix/concepts_and_terms/glossary.md#p339019220518)/[Memory Transfer Engine 2 (MTE2)](../../../technical_appendix/concepts_and_terms/glossary.md#p53901325511)/[Memory Transfer Engine 3 (MTE3)](../../../technical_appendix/concepts_and_terms/glossary.md#p183901322511). The mutual independence and parallel execution capability of different instruction queues are the cornerstone of the Double Buffer optimization mechanism.

Taking vector-only computation as an example, the CopyIn and CopyOut processes before and after vector computation use the data movement instruction queues (MTE2/MTE3), and the Compute process uses the vector instruction queue (V). Different instruction queues can be executed in parallel, which means that the CopyIn and CopyOut processes can run in parallel with the Compute process. As shown in the following figure, consider a complete data movement and computation process. The CopyIn process moves data from [Global Memory (GM)](../../../technical_appendix/concepts_and_terms/glossary.md#p1439018225112) to [Local Memory (LM)](../../../technical_appendix/concepts_and_terms/glossary.md#p193901420518). After the Vector compute unit completes the compute operation, the computation results are moved back to GM through the CopyOut process.

**Figure 1** Data movement and vector computation process<a name="fig_data_vector_compute"></a>  
![](../../../figures/data_vec_54.png "Data movement and Vector computation process")

**Figure 2** Pipeline chart without Double Buffer enabled<a name="fig_disable_double_buffer_pipeline"></a>  
![](../../../figures/no_doublebuf_pipe.png "Pipeline chart without Double Buffer enabled")

In this process, data movement and vector computation are executed serially, and the Vector compute unit inevitably suffers from resource idle problems. Assuming that the CopyIn, Compute, and CopyOut phases each take the same time $t$, the utilization of the vector is only one third. The waiting time is too long, and the vector utilization is severely insufficient.

To reduce the vector waiting time, Double Buffer prepares two groups of buffers, Ping and Pong, for the data to be processed. As shown in the following table, at t0, MTE2 moves the input of data block 0 into Ping. At t1, MTE2 moves the input of data block 1 into Pong, while the vector uses Ping to compute data block 0. At t2, MTE2 moves the input of data block 2 into Ping, the vector uses Pong to compute data block 1, and MTE3 simultaneously moves the result of data block 0 out of Ping. In this way, data moving in, vector computation, and data moving out can be executed in parallel across data blocks, alleviating the idle problem of the Vector compute unit.

**Table 1** Three time points of Ping/Pong Double Buffer  
| Execution Phase | t0 | t1 | t2 |
| --- | --- | --- | --- |
| MTE2 copy-in | Input of data block 0 to the Ping input area | Input of data block 1 to the Pong input area | Input of data block 2 to the Ping input area |
| Vector computation | None | Compute data block 0 (Ping) | Compute data block 1 (Pong) |
| MTE3 copy-out | None | None | Result of data block 0 moved out of the Ping output area |

At t2, the input area and output area in Ping are independent of each other, so moving in data block 2 and moving out data block 0 can be executed simultaneously.

In general, Double Buffer is based on the independence and parallelism of the MTE instruction queue and the vector instruction queue. It hides most of the data movement time by executing data movement and vector computation in parallel, reduces the waiting time of vector instructions, and ultimately improves the utilization efficiency of the Vector unit. The overall pipeline after enabling Double Buffer is shown in the following figure.

**Figure 3** Pipeline chart with Double Buffer enabled<a name="fig_enable_double_buffer_pipeline"></a>  
![](../../../figures/dbuf_pipe.png "Pipeline chart with Double Buffer enabled")

## Usage

In most cases, Double Buffer can effectively improve the utilization of the vector and reduce the operator execution time. However, the fact that the Double Buffer mechanism alleviates the vector idle problem does not mean that it always brings significant overall performance improvement. For example:

- When the data movement time is short while the vector computation time is long, the performance gain brought by the Double Buffer mechanism is relatively small because data movement accounts for a low proportion of the entire computation process.
- When the original data is small and the vector can complete the computation of all data at one time, pipeline overlap across data blocks cannot be formed, and the additional buffers and synchronization processing may be counterproductive.

Using Double Buffer also requires the following conditions:

- The single-core data can be divided into at least two data blocks so that adjacent data blocks can alternately use two groups of buffers.
- The [Unified Buffer (UB)](../../../technical_appendix/concepts_and_terms/glossary.md#p17392122195118) can accommodate two groups of input and output buffers as well as other temporary data required by the operator.
- The write, read, and reuse order of the same buffer can be correctly managed through the synchronization mechanism of the corresponding programming method.
- The buffer addresses and data movement lengths meet the alignment requirements of the target product and the movement API, and the tail block is processed according to the actual data length.

Therefore, the use of Double Buffer requires comprehensive consideration of factors such as vector computing power, data volume, UB capacity, the number of data blocks, and the ratio of data movement time to computation time.

## Implementation in Different Programming Modes

This document describes the `TPipe/TQue`-based programming mode and the pointer-based C programming mode. The static tensor programming mode can also use Double Buffer, but it requires manually dividing the Ping/Pong buffers and managing synchronization events. For its implementation and constraints, see [Pipeline Optimization for Static Tensor Programming](../../../programming_guide/programming_model/ai_core_simd_programming/cpp_tensor_programming/static_tensor_programming.md#section121239188376). This document does not elaborate on it separately.

### Programming Based on TPipe/TQue

When `TPipe/TQue` is used to manage resources, call `InitBuffer` in the `Init` function of the operator class to initialize the queues and set the number of memory blocks to control whether Double Buffer is enabled. When the `num` parameter is set to **1**, each queue is allocated one memory block and Double Buffer is not enabled. When it is set to **2**, each queue is allocated two memory blocks and Double Buffer is enabled.

[Negative example]

When the `num` parameter of `InitBuffer` is set to **1**, both the input and output queues are allocated only one memory block, and data copy-in, computation, and data copy-out cannot overlap across data blocks.

```cpp
__aicore__ inline void Init(__gm__ uint8_t* src0Gm, __gm__ uint8_t* src1Gm, __gm__ uint8_t* dstGm)
{
    src0Global.SetGlobalBuffer((__gm__ half*)src0Gm);
    src1Global.SetGlobalBuffer((__gm__ half*)src1Gm);
    dstGlobal.SetGlobalBuffer((__gm__ half*)dstGm);
    // Double Buffer is not enabled. The occupied physical space is 1 * sizeSrc0 * sizeof(half).
    // After the three InitBuffer calls are executed, the total space is:
    // 1 * (sizeSrc0 * sizeof(half) + sizeSrc1 * sizeof(half) + sizeDst0 * sizeof(half)).
    pipe.InitBuffer(inQueueSrc0, 1, sizeSrc0 * sizeof(half));
    pipe.InitBuffer(inQueueSrc1, 1, sizeSrc1 * sizeof(half));
    pipe.InitBuffer(outQueueDst, 1, sizeDst0 * sizeof(half));
}

__aicore__ inline void Process()
{
    // round * 2 loops are required to process all the data.
    for (uint32_t index = 0; index < round * 2; ++index) {
        CopyIn(index);
        Compute();
        CopyOut(index);
    }
}
```

[Positive example]

When initializing the input and output queues in the `Init` function, set the `num` parameter of `InitBuffer` to **2**. The `Process` function calls `CopyIn`, `Compute`, and `CopyOut` in the order of the data blocks. The queues manage buffer reuse and pipeline synchronization based on the enqueue, dequeue, and release order of the memory blocks.

```cpp
__aicore__ inline void Init(__gm__ uint8_t* src0Gm, __gm__ uint8_t* src1Gm, __gm__ uint8_t* dstGm)
{
    src0Global.SetGlobalBuffer((__gm__ half*)src0Gm);
    src1Global.SetGlobalBuffer((__gm__ half*)src1Gm);
    dstGlobal.SetGlobalBuffer((__gm__ half*)dstGm);
    // The value 2 in InitBuffer indicates that Double Buffer is enabled. The occupied physical space is 2 * sizeSrc0 * sizeof(half).
    // After the three InitBuffer calls are executed, the total space is:
    // 2 * (sizeSrc0 * sizeof(half) + sizeSrc1 * sizeof(half) + sizeDst0 * sizeof(half)).
    pipe.InitBuffer(inQueueSrc0, 2, sizeSrc0 * sizeof(half));
    pipe.InitBuffer(inQueueSrc1, 2, sizeSrc1 * sizeof(half));
    pipe.InitBuffer(outQueueDst, 2, sizeDst0 * sizeof(half));
}

__aicore__ inline void Process()
{
    // The prerequisite for enabling Double Buffer is that the number of loop iterations is greater than or equal to 2.
    for (uint32_t index = 0; index < round; ++index) {
        CopyIn(index);
        Compute();
        CopyOut(index);
    }
}
```

When using this method, note the following:

- `InitBuffer` should be called in the `Init` function of the operator class. Its three parameters represent the queue, the number of memory blocks, and the size of a single memory block in bytes, respectively.
- After Double Buffer is enabled, the UB space occupied by the input and output queues doubles. You need to recompute the length of a single data block and the number of loop iterations.
- The number of loop iterations in `Process` depends on the tiling result. The `round` above is used only to show the code positions before and after enabling Double Buffer, and cannot be used as a fixed computation method for all operators.
- If the tail block does not meet the Double Buffer splitting conditions of the main block, process the tail block separately.

For the complete tiling, queue initialization, and main/tail block processing methods, see [Double Buffer Scenario](../../simd_operator_impl/vector_programming/double_buffer_scenario.md).

### C Language Programming Based on Pointers

When the pointer-based C language programming method is used, Double Buffer is not a single switch. You need to manually allocate two groups of UB buffers, Ping and Pong, in the kernel, alternately select a buffer based on the data block sequence number, and use the [asc_lock](../../../../api/SIMD-API/c_api/sync/intra_core_sync/asc_lock.md) and [asc_unlock](../../../../api/SIMD-API/c_api/sync/intra_core_sync/asc_unlock.md) APIs to constrain the read, write, and reuse of the same buffer.

The following code omits inter-core splitting, movement parameter configuration, and host-side logic, retaining only the kernel where Double Buffer resides and its key logic. For the complete single-buffer baseline and Double Buffer implementation, see the [fused vector computation Double Buffer sample](../../../../../../examples/02_simd_c_api/02_features/01_reg_vector_compute/00_add_double_buffer/README.md). `BLOCK_LENGTH` indicates the maximum number of elements in a single data block, and `currentLength` indicates the actual number of elements in the current data block.

```cpp
__global__ __vector__ void add_double_buffer(__gm__ half* xGm, __gm__ half* yGm, __gm__ half* zGm,
    uint32_t totalElements)
{
    asc_init();

    // Each group contains three buffers: x, y, and z. The two groups are Ping and Pong.
    __ubuf__ half ubBuffer[BLOCK_LENGTH * 6];
    __ubuf__ half* xPing = ubBuffer;
    __ubuf__ half* yPing = xPing + BLOCK_LENGTH;
    __ubuf__ half* zPing = yPing + BLOCK_LENGTH;
    __ubuf__ half* xPong = zPing + BLOCK_LENGTH;
    __ubuf__ half* yPong = xPong + BLOCK_LENGTH;
    __ubuf__ half* zPong = yPong + BLOCK_LENGTH;

    uint32_t blockCount = (totalElements + BLOCK_LENGTH - 1) / BLOCK_LENGTH;
    for (uint32_t blockIdx = 0; blockIdx < blockCount; ++blockIdx) {
        uint32_t offset = blockIdx * BLOCK_LENGTH;
        uint32_t currentLength = (totalElements - offset) > BLOCK_LENGTH ?
            BLOCK_LENGTH : (totalElements - offset);

        // Even blocks use Ping, and odd blocks use Pong.
        uint8_t inputMutexId = static_cast<uint8_t>(blockIdx & 1);
        uint8_t outputMutexId = static_cast<uint8_t>(inputMutexId + 2);
        __ubuf__ half* xLocal = (blockIdx & 1) == 0 ? xPing : xPong;
        __ubuf__ half* yLocal = (blockIdx & 1) == 0 ? yPing : yPong;
        __ubuf__ half* zLocal = (blockIdx & 1) == 0 ? zPing : zPong;

        // MTE2 moves in the current block input. The input mutex protects x/y in the same group from being written again.
        asc_lock(PIPE_MTE2, inputMutexId);
        asc_copy_gm2ub_align(xLocal, xGm + offset, currentLength * sizeof(half));
        asc_copy_gm2ub_align(yLocal, yGm + offset, currentLength * sizeof(half));
        asc_unlock(PIPE_MTE2, inputMutexId);

        // Vector waits for the current x/y to be ready and for z in the same group to be writable.
        asc_lock(PIPE_V, inputMutexId);
        asc_lock(PIPE_V, outputMutexId);
        asc_add(zLocal, xLocal, yLocal, currentLength);
        asc_unlock(PIPE_V, inputMutexId);
        asc_unlock(PIPE_V, outputMutexId);

        // MTE3 waits for the current z computation to complete before moving it out.
        asc_lock(PIPE_MTE3, outputMutexId);
        asc_copy_ub2gm_align(zGm + offset, zLocal, currentLength * sizeof(half));
        asc_unlock(PIPE_MTE3, outputMutexId);
    }

    // Drain the submitted but incomplete asynchronous operations before the kernel exits.
    asc_sync_pipe(PIPE_ALL);
}
```

When `blockIdx` is 0, 1, and 2, the code selects Ping, Pong, and Ping, respectively. The loop submits the data movement, computation, and data movement-out operations in code order. Different pipelines execute asynchronously after their synchronization dependencies are satisfied, so that adjacent data blocks overlap.

Each group of input and output buffers must satisfy the following synchronization relationships:

| UB Area | Producer and Consumer | Required Order | Synchronization Method |
| --- | --- | --- | --- |
| Ping/Pong `x/y` | MTE2 write -> Vector read | Computation can start only after the input of the same group is moved in; before the next write to the same group's input, the previous read must have finished. | Use the input `mutex_id` of the group. |
| Ping/Pong `z` | Vector write -> MTE3 read | The result can be moved out only after computation is complete. | Use the output `mutex_id` of the group. |
| Ping/Pong `z` | MTE3 read -> next vector write | The output buffer of the same group can be reused only after the result of the previous data block is moved out. | The next vector waits when acquiring the same output `mutex_id`. |

When MTE2 moves new data into Ping's `x/y`, MTE3 can simultaneously move out the result of the previous data block from Ping's `z`, because `x/y/z` are mutually independent sub-buffers in the same group. Before writing to the same group's `x/y` again, you must wait for the previous vector read to finish; before writing to the same group's `z` again, you must wait for the previous MTE3 move-out to finish.

When using this method, note the following:

- The size, offset, and alignment of the Ping/Pong buffers must be statically determined based on the available UB capacity.
- The tail block must be moved and computed according to the actual length through `currentLength`.
- Note the product support status of the `asc_lock/asc_unlock` APIs. If Double Buffer is enabled on other products, use APIs such as `asc_sync_notify/asc_sync_wait` to establish equivalent data dependencies based on the supported synchronization APIs. Do not directly copy the code in this section.
- Before the kernel exits, wait for the submitted asynchronous operations to complete. The example uses `asc_sync_pipe(PIPE_ALL)` to drain the pipeline.
