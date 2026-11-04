# Memory Consistency

<!-- md-trans-meta sourceCommit=5e55c38a4e567465079c4ca7387d07ab9e6347c5 translatedAt=2026-10-23T14:28:11.853Z pushedAt=2026-11-03T07:05:13.433Z nl=2 -->

This document describes memory consistency issues in NPU programming and explains how to establish predictable read/write ordering between different execution units, different AI Cores, different streams, and between the host side and the device side. This document focuses on "operation ordering and visibility". For issues such as cache copy writeback and invalidation, see [Cache Coherence](cache_coherence.md).

The related examples involve memories such as Global Memory (GM), Unified Buffer (UB), and Data Cache (DCache).

## Basic Concepts of Memory Consistency

### What Is Memory Consistency

Memory consistency concerns whether the read/write order of multiple accessors to the same memory region matches the program's expectation. A typical memory consistency example is: the producer writes data first and then writes a flag; after the consumer observes the flag, can it necessarily read the data written by the producer:

```text
Initial state:
GM[data] = 0
GM[flag] = 0

Time point    Producer                                Consumer
======    ====================================    ====================================
T0        STORE data = 1
T1        STORE flag = 1
                                                T2  LOAD flag -> 1
                                                T3  LOAD data -> ?
```

Users expect the memory order to be consistent with the code order. Before `flag` is assigned, `data` has already been assigned. It is expected that after the consumer sees `flag = 1` at `T2`, it can definitely read `data = 1` at `T3`. However, on the NPU, the producer and consumer may run on different pipelines, different AI Cores, different Streams, or even between the host side and the device side. Different execution domains are usually out-of-order and parallel, and a global order is not automatically established. Users need to insert synchronization, MEMBAR, or communication mechanisms according to the access scope to manage memory consistency themselves.

### Differences Between Cache Coherence and Memory Consistency

Cache coherence and memory consistency are two different types of issues:

| Type | Focus | Typical Issue | Common Handling Method |
| --- | --- | --- | --- |
| Cache coherence | Whether the value in the cache copy is consistent with that in the next-level storage. | The DCache retains an old value, or a dirty copy has not been written back to the GM. | DCCI |
| Memory consistency | Whether the ordering relationship between multiple read/write operations meets expectations. | After the consumer observes `flag = 1`, whether it can definitely read the `data` written by the producer. | Synchronization, DSB, etc. |

Cache coherence is about "whether the copy is consistent with the latest value", while memory consistency is about "whether the actual execution order of instructions is consistent with the code order". In actual programming, the two types of issues often occur together. For example, after the producer writes data, it publishes a synchronization flag through `flag`, and the consumer reads the data after waiting for `flag`:

```text
Initial state:
GM[data] = 0
GM[flag] = 0
Consumer DCache[data] = 0

Time point    Producer                              Consumer
======    ====================================    ====================================
T0        Scalar.STORE data = 1
T1        DCCI data
T2        Scalar.DSB
          // Ensure that data is written back to the GM.

T3        STORE flag = 1
          // Publish the flag. The flag itself is a synchronization notification.

                                                T4  WAIT flag == 1
                                                    // Wait for flag to become 1.

                                                T5  DCCI data
                                                T6  Scalar.DSB
                                                    // Handle the stale DCache copy on the consumer side.

                                                T7  Scalar.LOAD data -> 1
```

In the preceding example, `flag` is the synchronization flag. The producer writes `flag = 1` to publish a notification, and the consumer waits for `flag == 1` to receive the notification. DCCI is used to maintain the cache copy corresponding to `data`, which is cache coherence handling. `Scalar.DSB` is used to wait for the completion of writeback or invalidation operations. `STORE flag` and `WAIT flag` are used to establish the execution ordering relationship between the producer and the consumer, which is memory consistency handling.

- If only synchronization is performed without handling the DCache copy, a stale value may still be read.
- If only DCCI is performed without establishing the execution order, the consumer may also read prematurely.

If either cache coherence or memory consistency is not handled properly, the data finally read may not meet expectations.

## NPU Memory Consistency Model

### NPU Memory Order

#### NPU Hardware Execution Hierarchy and Consistency Boundary

The NPU hardware architecture has multiple levels, which are characterized by asynchronous parallelism:
- On the host side, tasks are asynchronously dispatched to the device side through Streams.
- On the device side, a Kernel can be executed in parallel by multiple AI Cores.
- Within each AI Core, there are multiple pipelines that can execute asynchronously in parallel.

These hardware levels determine that memory consistency maintenance in NPU programming usually needs to be handled hierarchically by access scope:

- Within the same AI Core, when different pipelines access the same address or transfer data through UB/GM, intra-core synchronization is required.
- Between different AI Cores, when data is shared or flags are transferred through GM, inter-core synchronization is required.
- Between the host side and the device side, since Kernel execution, asynchronous copy, and Stream task dispatch are all asynchronous, Host-Device synchronization is required.

#### Inter-Pipeline Consistency

The AI Core contains the Scalar pipeline, MTE pipeline, Fixpipe pipeline<!-- npu="950" id1 -->, and SIMT pipeline<!-- end id1 -->, which are responsible for different types of computation or memory access tasks and can access GM through their respective data paths.

Different pipelines can execute concurrently by default, which means that the code order does not necessarily equal the memory access order observed by different pipelines. If no necessary synchronization relationship is established, read and write operations on different pipelines may be interleaved according to hardware scheduling and resource states, leading to memory consistency issues in scenarios such as RAW, WAW, and WAR.

Therefore, when there is a data dependency across pipelines, you cannot rely solely on the code order and need to establish an ordering relationship in software. If cross-pipeline access also involves the Scalar<!-- npu="950" id2 --> or SIMT<!-- end id2 --> pipeline accessing GM through the Cache, you also need to consider [Cache Coherence](cache_coherence.md) issues such as stale copies and dirty copies in the DCache<!-- npu="950" id3 --> or SIMT DCache<!-- end id3 -->.

#### Intra-Pipeline Consistency

##### Consistency Guarantees Within a Single Pipeline

Instructions in the same pipeline typically enter the pipeline in code order and execute in program order, which is the basis for analyzing intra-pipeline ordering.

However, this does not mean that the memory access of a preceding instruction has completed when a subsequent instruction begins execution. If the preceding and subsequent instructions access the same address or overlapping addresses, single-pipeline synchronization may still be required.

##### Scalar Pipeline

Scalar is responsible for scalar control, address calculation, scalar computation, Scalar memory access, and issuing instructions to other pipelines. GM accesses within the Scalar pipeline must be distinguished by GM address, access path, and the Cache Line granularity of the DCache:

- **Accessing different GM addresses**: There is no natural data dependency between different addresses. Regardless of whether the accesses go through the DCache, the hardware does not guarantee that the completion order of these accesses or their visibility order to other observers is consistent with the code order. For example, when writing `data` first and then `flag`, you cannot rely solely on the code order to infer that other pipelines, other AI Cores, or the host side will definitely observe `data` before `flag`.
- **Accessing the same GM address**: When ordinary Scalar accesses the same GM address through the DCache, the result of same-address accesses remains consistent with the program order. Although the result of same-address accesses within the pipeline remains consistent with the program order, when other pipelines or other AI Cores need to observe the write result, the Dirty copies or stale copies in the DCache must still be handled on a CacheLine basis. For details, see [Cache Coherence Between DCache and GM](./cache_coherence.md#cache-coherence-between-dcache-and-gm).
- **Mixing different access paths for the same GM address**: If accesses to the same address mix paths that go through the DCache and paths that do not, the hardware does not guarantee that the final result conforms to the code order. The software must additionally handle access completion and Cache copy issues. For example, when Scalar read/write through the DCache is mixed with Scalar atomic operations, the user needs to explicitly insert synchronization to ensure memory consistency.

##### MTE/Fixpipe Pipeline

For MTE/Fixpipe instructions, regardless of whether they access the same address, the actual completion order or externally visible order of memory accesses between instructions may differ from the code order, and the hardware does not guarantee memory consistency. If preceding and subsequent instructions involve overlapping addresses or data dependencies, you need to explicitly wait for the data read/write of the preceding instruction to complete.

<!-- npu="950" id4 -->
##### SIMT Pipeline

The SIMT pipeline is used for thread-level execution in the SIMT programming model and is suitable for scenarios involving complex control flow and irregular memory access. SIMT threads can access GM and also support thread synchronization, MEMBAR, and atomic operations.

The ordering guarantees of the SIMT pipeline need to distinguish between inter-thread and intra-thread cases:

- Inter-thread: Regardless of whether the accesses target the same address, the actual completion order or externally visible order of memory accesses between different SIMT threads may differ from the code order, and the hardware does not guarantee memory consistency between threads.
- Intra-thread: When a single SIMT thread accesses different addresses, the hardware does not guarantee that the access order of different addresses is consistent with the code order; when a single SIMT thread accesses the same address, the hardware guarantees that the execution result is consistent with the code order, regardless of whether the access passes through the SIMT DCache.
<!-- end id4 -->

### Relationship Between NPU Memory Consistency and Standard Consistency Models

From the perspective of consistency models in classic parallel architectures, the NPU provides neither global sequential consistency nor a strong memory order similar to total store ordering.

| Consistency Model in Classic Parallel Architectures | Model Characteristics | NPU Support and Characteristics on the NPU |
| --- | --- | --- |
| Sequential Consistency (SC) | The memory access results observed by all processing units appear to come from a single global unique order, and the order within each processing unit is consistent with the program order. | Not supported. By default, the NPU has no global unique read/write order, and it cannot be assumed that all pipelines, AI Cores, Streams, and the host side observe all writes in the same order. |
| Total Store Order (TSO) | Write operations are generally made externally visible in a global store order. The model is stronger than weak consistency and weaker than strict sequential consistency. | Not supported. The NPU does not guarantee that ordinary write operations become visible to all observers in a global store order. Write completion and external visibility must be handled in combination with DSB, synchronization, or MEMBAR. |
| Weak Consistency | Ordinary reads and writes can be reordered or have delayed visibility. A clear order is established only before and after special operations such as synchronization and fences. | Similar. By default, ordinary memory accesses on the NPU do not establish ordering across execution domains. Ordering of access completion and visibility must be explicitly constrained through mechanisms such as intra-core synchronization, inter-core synchronization, Stream/Event synchronization, Host-Device synchronization, and DSB. |
| Release/Acquire Consistency | The producer publishes data with Release, and the consumer obtains data with Acquire. Writes before Release become visible to the consumer after Acquire. | Similar semantics are supported. On the NPU, a Release/Acquire relationship is typically established through the combination of [asc_sync_notify](../../../../api/SIMD-API/c_api/sync/intra_core_sync/asc_sync_notify.md) and [asc_sync_wait](../../../../api/SIMD-API/c_api/sync/intra_core_sync/asc_sync_wait.md). |

The NPU is closer to a weak consistency model: ordinary memory accesses have only limited local ordering by default, and ordering across pipelines, cores, Streams, and between Host and Device must be explicitly established by the user.


## Intra-core Memory Consistency

### Intra-core Consistency Issues

This section discusses how to establish a predictable access order between different pipelines<!-- npu="950" id5 -->or SIMT threads<!-- end id5 -->within the same AI Core. Here, "intra-core" means that both the producer and the consumer are within the same AI Core.

Intra-core consistency issues typically arise from two types of scenarios:

- Data is passed between different pipelines through the same buffer. For example, after MTE2 moves GM data into the UB, the Vector pipeline reads the UB for computation.
- The same pipeline<!-- npu="950" id6 -->or SIMT thread<!-- end id6 -->accesses the same address or overlapping addresses before and after, and the subsequent access depends on the preceding access actually completing.

Therefore, when handling intra-core consistency, first determine which execution unit the preceding access and the subsequent access are located in, and then determine whether the dependency between them is RAW, WAW, or WAR. If the access path involves DCache<!-- npu="950" id7 -->or SIMT DCache<!-- end id7 -->, also refer to [Cache Coherence](cache_coherence.md) to handle cache copy issues.

| Dependency Type | Meaning | Examples |
| --- | --- | --- |
| RAW | Write first, then read. The read operation must read the data written by the preceding write. | MTE2 writes the UB first, and Vector reads the UB later. |
| WAW | Write first, then write. The final result must be based on the later write. | Two MTE2 transfers write overlapping UB addresses. |
| WAR | Read first, then write. The write operation must not overwrite data that the preceding read has not finished using. | Vector is still reading the UB, and MTE2 is about to reuse the same UB space. |


### Cross-Pipeline Memory Consistency

A cross-pipeline dependency means that the preceding access and the subsequent access are not on the same pipeline. Different pipelines can execute in parallel by default, so a preceding transfer or computation in code order is not necessarily completed before the subsequent pipeline accesses the data.

The core of cross-pipeline synchronization is to establish the ordering relationship of "the consumer pipeline accesses the data only after the producer pipeline completes." In the examples in this document, `WAIT` is used to represent an abstract synchronization point. In actual coding, you can select intra-core multi-pipeline synchronization primitives such as [asc_sync_notify](../../../../api/SIMD-API/c_api/sync/intra_core_sync/asc_sync_notify.md)/[asc_sync_wait](../../../../api/SIMD-API/c_api/sync/intra_core_sync/asc_sync_wait.md) and [asc_lock](../../../../api/SIMD-API/c_api/sync/intra_core_sync/asc_lock.md)/[asc_unlock](../../../../api/SIMD-API/c_api/sync/intra_core_sync/asc_unlock.md) based on the scenario. For API selection, see [Intra-Core Synchronization Primitives](#intra-core-synchronization-primitives).

Using MTE2 to move data from GM into UB, the Vector pipeline to read UB and perform computation, and MTE3 to move results from UB out to GM as an example, the following shows the memory consistency issues caused by cross-pipeline execution:

#### Forward Dependency

A forward dependency refers to the producer-consumer dependency within the current loop iteration. The `MTE2 -> V -> MTE3` flow is as follows:

```text
Time point    MTE2 pipeline                    V pipeline                       MTE3 pipeline
======    ========================    ========================    ========================
T0        MTE2.STORE_UB
          // Move GM data into UB

T1                                    WAIT MTE2
                                      // Wait for MTE2 to finish writing UB

T2                                    V.LOAD_UB
                                      V.STORE_UB
                                      // Read UB input and write UB result

T3                                                                WAIT V
                                                                  // Wait for Vector to finish writing UB

T4                                                                MTE3.LOAD_UB
                                                                  MTE3.STORE_GM
                                                                  // Read UB result and move it out to GM
```

The abstract flow above corresponds to two synchronization points:

- `MTE2 -> V`: After MTE2 finishes writing UB, Vector can read UB. The ordering is typically established through `asc_sync_notify(PIPE_MTE2, PIPE_V, event_id)` and `asc_sync_wait(PIPE_MTE2, PIPE_V, event_id)`.
- `V -> MTE3`: After the Vector finishes writing to the UB, MTE3 can read the UB and move the data out to the GM. The ordering is typically established through `asc_sync_notify(PIPE_V, PIPE_MTE3, event_id)` and `asc_sync_wait(PIPE_V, PIPE_MTE3, event_id)`.

If the first synchronization is omitted, the Vector may read UB data that has not yet been fully moved in; if the second synchronization is omitted, MTE3 may move results that have not yet been fully computed out to the GM.

#### Reverse Dependency

A reverse dependency occurs when a buffer is reused across multiple loop iterations. A forward dependency constrains that "a later pipeline in the current iteration waits for an earlier pipeline in the current iteration", for example, the Vector in the current iteration waits for the MTE2 in the current iteration to finish moving data in. A reverse dependency constrains that "before a subsequent loop rewrites a buffer, it must wait for the pipeline that last used the buffer in the preceding loop to finish accessing it".

Whether a single buffer or [Double Buffer](../../../technical_appendix/concepts_and_terms/performance_optimization/double_buffer.md) is used, as long as a subsequent loop rewrites a buffer, synchronization is required to ensure consistency. The difference between a single buffer and Double Buffer lies in the interval at which the same buffer is reused:

| Scenario | Buffer reuse relationship | Location requiring waiting | Description |
| --- | --- | --- | --- |
| Single buffer | Loop 1 reuses `buf` from Loop 0. | Before Loop 1 writes `buf`, wait for the pipeline that last used `buf` in Loop 0 to finish. | A reverse dependency exists between adjacent loops. |
| Double Buffer | Loop 0 uses `buffer1`, Loop 1 uses `buffer2`, and Loop 2 reuses `buffer1`. | Before Loop 2 writes `buffer1`, wait for the pipeline that last used `buffer1` in Loop 0 to finish. | The reverse dependency still exists, but the reuse interval of the same buffer becomes longer. |

If the reverse dependency is not handled, the write in a subsequent loop may overwrite data that the preceding loop is still reading or moving out, causing a memory consistency issue. The following uses a single buffer and Double Buffer respectively to illustrate this.

##### Single Buffer Example

In the single buffer scenario, adjacent loops reuse the same buffer. Before Loop 1 starts moving data into `buf`, it must wait for Loop 0's MTE3 to finish moving data out, so that Loop 0's result is correctly written back to GM.

```text
Time point    Loop 0                                  Loop 1
======    ====================================    ====================================
T0        MTE2.STORE_UB(buf)
          // Move in the input of this round

T1        V.LOAD_UB(buf)
          V.STORE_UB(buf)
          // Read the input of this round and write the result of this round

T2        MTE3.LOAD_UB(buf)
          MTE3.STORE_GM
          // Move out the result of this round

T3                                                WAIT MTE3
                                                  MTE2.STORE_UB(buf)
                                                  // Only after the previous round finishes moving out can the next round reuse buf to move in
```

The `WAIT MTE3` in the example above is the reverse synchronization point. It is not the forward dependency of this round's `MTE2 -> V -> MTE3`, but is used to ensure that Loop 0 has finished using `buf` before Loop 1 reuses `buf`.

##### Double Buffer Example

```text
Time      Loop 0                                  Loop 1                                  Loop 2
======    ====================================    ====================================    ====================================
T0        MTE2.STORE_UB(buffer1)
          // Load in Loop 0 input

T1        WAIT MTE2
          V.LOAD_UB(buffer1)
          V.STORE_UB(buffer1)                    MTE2.STORE_UB(buffer2)
          // Loop 0 compute                        // Load in Loop 1 input, in parallel with Loop 0 compute

T2        WAIT V
          MTE3.LOAD_UB(buffer1)
          MTE3.STORE_GM                          WAIT MTE2
          // Move out Loop 0 result                V.LOAD_UB(buffer2)
                                                  V.STORE_UB(buffer2)
                                                  // Loop 1 compute

T3                                                WAIT V
                                                  MTE3.LOAD_UB(buffer2)
                                                  MTE3.STORE_GM                          WAIT MTE3
                                                  // Move out Loop 1 result                    // Wait for Loop 0 to finish moving out buffer1

T4                                                                                        MTE2.STORE_UB(buffer1)
                                                                                          // Reuse buffer1 to load in Loop 2 input
```

In the preceding example, `WAIT MTE3` is the reverse synchronization point in the Double Buffer scenario. It is not used to establish the forward dependency of `MTE2 -> V -> MTE3` within Loop 2, but to ensure that Loop 0 has finished using `buffer1` before `buffer1` is written again by Loop 2.

For reverse dependencies, whether in the single Buffer or Double Buffer scenario, the principle of maintaining memory consistency remains unchanged: whoever uses the Buffer first notifies after finishing the use; whoever reuses the Buffer later waits before the reuse.

If `mutex_id` is used for synchronization, in the single Buffer scenario all loops reuse the same `mutex_id`; in the Double Buffer scenario, `buffer1` and `buffer2` use different `mutex_id` values. In this way, when a subsequent loop reuses a `Buffer`, it waits for the last user of that `Buffer` in the previous round to release the corresponding `mutex_id`. For a specific example, see [asc_lock](../../../../api/SIMD-API/c_api/sync/intra_core_sync/asc_lock.md)/[asc_unlock](../../../../api/SIMD-API/c_api/sync/intra_core_sync/asc_unlock.md).

### Memory Consistency in the Same Pipeline

In the MTE, Fixpipe, Vector, and Cube pipelines, instructions in the same pipeline can be understood as "issued in order, executed asynchronously": subsequent instructions enter the same pipeline in code order, but the data reads and writes involved in preceding instructions may not have fully completed. If preceding and subsequent instructions access the same address or overlapping addresses, RAW, WAW, or WAR issues may occur.

| Observation Dimension | Meaning | Impact on Users |
| --- | --- | --- |
| Code order/issue order | A preceding instruction enters the same pipeline before a subsequent instruction. | It only indicates the ordering relationship of submission, and does not mean that the data reads and writes of the preceding instruction have completed. |
| Data read/write completion order | The data reads and writes of a preceding instruction may still be incomplete when a subsequent instruction starts. | When a subsequent instruction accesses the same address or overlapping addresses, it may read data that is not ready or overwrite data that is still in use. |
| Software synchronization point | Explicitly wait for the preceding pipeline to complete before the subsequent dependent access. | It constrains "completion of the preceding access" to before "start of the subsequent access", ensuring the access order in RAW, WAW, and WAR scenarios. |

For example, the same MTE2 pipeline continuously moves data into overlapping address regions in the UB, and the overlapping region is expected to reflect the result of the later move. If same-pipeline synchronization is not inserted, the following issue may occur:

```text
Time point    MTE2 pipeline
======    ====================================
T0        MTE2.STORE_UB(region0)
          // First move, writes UB region region0

T1        MTE2.STORE_UB(region1)
          // Second move, region1 overlaps with region0

T2        The result of the overlapping region of region0 and region1 is uncertain
          // The data reads and writes of the previous move have not fully completed, and the next move has already started
          // The overlapping region may not reflect the result of the later move
```

To ensure that the overlapping region takes the result of the last move-in as the final result, before the later write starts, the user needs to wait for the preceding MTE2 write to UB to complete:

```text
Time    MTE2 pipeline
======    ====================================
T0        MTE2.STORE_UB(region0)
          // First move-in, write to UB region region0

T1        WAIT MTE2
          // Wait for the preceding MTE2 write to UB to complete

T2        MTE2.STORE_UB(region1)
          // Second move-in, region1 overlaps with region0
```

Here, `WAIT MTE2` is a software synchronization point inserted by the user to ensure that the later write occurs after the preceding write completes.


### Intra-core Synchronization Primitives

Intra-core synchronization primitives are used to establish a predictable execution ordering relationship within the same AI Core. Based on the synchronization object, they can be divided into single-pipeline synchronization and multi-pipeline synchronization.

#### Single-Pipeline Synchronization

Single-pipeline synchronization is used to wait for the completion of preceding operations that have already been submitted in a pipeline before allowing subsequent dependent operations to continue execution.

| API | Scenario | Brief Description |
| --- | --- | --- |
| [asc_sync_pipe](../../../../api/SIMD-API/c_api/sync/intra_core_sync/asc_sync_pipe.md) | Preceding and subsequent operations within the same pipeline access the same address, overlapping addresses, or have data dependencies. | Waits for all data reads and writes of preceding instructions in the specified pipeline to complete before subsequent instructions in that pipeline can begin execution. `PIPE_S` is not supported. You can pass `PIPE_ALL` to wait for all pipelines, but this expands the synchronization scope. |
| [asc_sync_data_barrier](../../../../api/SIMD-API/c_api/sync/intra_core_sync/asc_sync_data_barrier.md) | GM or UB accesses initiated by Scalar need to wait for completion. | Waits for preceding memory accesses on the Scalar side to complete. `Scalar.DSB` waits for GM accesses, `DSB_UB` waits for UB accesses, and `DSB_ALL` waits for both GM and UB accesses. |

When using these APIs, it is recommended to prioritize the minimum synchronization scope. For example, when you only need to wait for the completion of preceding MTE2 data transfers, use `asc_sync_pipe(PIPE_MTE2)`. Use `PIPE_ALL` or a larger synchronization scope only when the dependent pipeline cannot be accurately determined.

The Scalar pipeline is not handled through `asc_sync_pipe(PIPE_S)`. To ensure that the memory access of the previous instruction on Scalar is complete, use `asc_sync_data_barrier`.

#### Multi-Pipeline Synchronization

Multi-pipeline synchronization is used to establish a producer-to-consumer ordering relationship between different pipelines, for example, `MTE2 -> V`, `V -> MTE3`, or `S -> MTE2`.

| API | Scenario | Brief Description |
| --- | --- | --- |
| [asc_sync_notify](../../../../api/SIMD-API/c_api/sync/intra_core_sync/asc_sync_notify.md)/[asc_sync_wait](../../../../api/SIMD-API/c_api/sync/intra_core_sync/asc_sync_wait.md) | There is an explicit dependency between two pipelines. | `notify` sets a synchronization flag on the source pipeline, and `wait` waits for the flag on the target pipeline; they are paired through the source pipeline, target pipeline, and `event_id`. |
| [asc_lock](../../../../api/SIMD-API/c_api/sync/intra_core_sync/asc_lock.md)/[asc_unlock](../../../../api/SIMD-API/c_api/sync/intra_core_sync/asc_unlock.md) | Dependencies across multiple pipelines need to be managed by buffer lifecycle or `mutex_id`. | Uses the mutex mechanism to block or release a specified pipeline, suitable for complex pipeline orchestration, reverse dependencies, and double-buffer scenarios; they must be used in pairs, and nesting the same `mutex_id` must be avoided. This mechanism is newly added in [NPU architecture version 3510](../../language_extension/simd_builtin_keywords.md). |
| [asc_sync](../../../../api/SIMD-API/c_api/sync/intra_core_sync/asc_sync.md) | It is necessary to wait for all preceding operations on all pipelines to complete. | Waits for all pipeline operations to complete, which has a large scope and affects performance; if only a single or a few pipelines are depended on, prefer more precise synchronization APIs. |
| [asc_sync_mte2](../../../../api/SIMD-API/c_api/sync/intra_core_sync/asc_sync_mte2.md)/[asc_sync_mte3](../../../../api/SIMD-API/c_api/sync/intra_core_sync/asc_sync_mte3.md)/[asc_sync_vec](../../../../api/SIMD-API/c_api/sync/intra_core_sync/asc_sync_vec.md) | It is necessary to wait for preceding operations on the MTE2, MTE3, or Vector pipeline to complete. | Convenient waiting APIs for specific pipelines, used to wait for preceding operations on a specified pipeline to complete before starting subsequent related operations; these APIs do not express a precise pairing relationship between a source pipeline and a target pipeline. |

When selecting a synchronization primitive, it is recommended to first determine whether the dependency relationship falls only within the same pipeline; if so, prefer single-pipeline synchronization. If it spans pipelines:

- When the producer and consumer are on different pipelines and it is necessary to precisely express that "the target pipeline can continue only after the source pipeline completes", prefer `asc_sync_notify/asc_sync_wait`.
- On NPU architecture version 3510, you can also use `asc_lock/asc_unlock` to manage cross-pipeline dependencies by buffer lifecycle.
- If you only need to wait for preceding operations on a specific pipeline or all pipelines to complete, you can use `asc_sync_mte2`, `asc_sync_mte3`, `asc_sync_vec`, or `asc_sync`.

Synchronization can only establish an execution ordering relationship; if the access path involves DCache<!-- npu="950" id8 --> or SIMT DCache<!-- end id8 -->, you also need to refer to [Cache Coherence](cache_coherence.md) to handle cache copies.

### event_id Management

`asc_sync_notify` and `asc_sync_wait` are associated through `event_id`. Each `event_id` can be understood as a status marker for the transfer, computation, and transfer-out of a block of data or a Buffer: after the producer pipeline completes the preceding operations on a block of data, it sends a notification through that `event_id`; the consumer pipeline can continue processing the corresponding data only after waiting for the same `event_id`.

The number of `event_id` values is limited, typically ranging from 0 to 7, of which 6 and 7 are internally reserved IDs. Do not manually set them to 6 or 7. Management errors may lead to event state confusion, ID conflicts, undefined behavior, or timeout issues. When using them, follow these principles:

- A data block that is still being processed or a Buffer reuse cycle should use an independent `event_id` to represent its state, avoiding reuse of the same `event_id` by multiple unfinished data blocks.
- `asc_sync_notify` and `asc_sync_wait` must be used in pairs, and the source pipeline, target pipeline, and `event_id` must be consistent.
- Under the same source pipeline, same target pipeline, and same `event_id`, do not execute `asc_sync_notify` multiple times consecutively without a corresponding `asc_sync_wait` to consume it.
- When a synchronization ID needs to be selected dynamically, obtain a valid ID through [FetchEventID](../../../../api/SIMD-API/basic_api/resource_management/TPipe/FetchEventID.md) or [AllocEventID](../../../../api/SIMD-API/basic_api/resource_management/TPipe/AllocEventID.md). If `AllocEventID` is used to occupy an ID, call [ReleaseEventID](../../../../api/SIMD-API/basic_api/resource_management/TPipe/ReleaseEventID.md) immediately after use to release it, avoiding ID exhaustion.

### Scalar Memory Consistency

Scalar memory consistency needs to be discussed in two types of scenarios: one is the ordering between preceding and subsequent accesses within a single Scalar pipeline; the other is the visibility between Scalar and other observers, such as Scalar and other hardware units like MTE, Fixpipe<!-- npu="950" id9 -->, and SIMT<!-- end id9 --> within the same core.

#### Memory Consistency of Accesses Within a Single Scalar Pipeline

##### Case 1: Both Accesses are DCache Same-Address Accesses

If the preceding and following accesses target the same GM address and both go through the DCache, the hardware maintains the access order on that address, and the execution result is consistent with the code order. In this scenario within a single Scalar pipeline, the user does not need to insert additional synchronization to guarantee the same-address access order.

```text
Time point    Scalar pipeline
======    ====================================
T0        Scalar.STORE_GM_DCache(addr) = 1
T1        Scalar.LOAD_GM_DCache(addr) -> 1
```

Note that the guarantee above only means that when the current Scalar accesses the same address through the DCache, the result conforms to the program order. It does not mean that the cache line containing the address has been written back to GM, nor that other pipelines or other AI Cores can already observe it.

##### Case 2: Both Accesses Are DCache Accesses to Different Addresses

If the preceding and following accesses target different GM addresses, even if both go through the DCache, the hardware does not guarantee that the completion order or the externally visible order of accesses to different addresses is consistent with the code order. A typical issue is writing data first and then writing a flag:

```text
Time    Scalar pipeline
======    ====================================
T0        Scalar.STORE_GM_DCache(data) = 1
T1        Scalar.STORE_GM_DCache(flag) = 1
```

If subsequent logic requires the `data` write to complete before the `flag` is published, you need to ensure that the accesses related to `data` have completed before publishing `flag`. If the consumer needs to observe `data` through the GM, you also need to first handle the dirty copy of the write end of the Cache Line where `data` resides:

```text
Time    Scalar pipeline
======    ====================================
T0        Scalar.STORE_GM_DCache(data) = 1
T1        Scalar.DCCI(data)
T2        Scalar.DSB
          // Wait for data to be written back to the GM.

T3        Scalar.STORE_GM_DCache(flag) = 1
          // If flag also needs to be visible to other units, flag itself still needs to be processed further along the publishing path.
```

##### Case 3: Both Accesses Bypass the DCache

The path that bypasses the DCache directly accesses GM without going through the DCache, for example, [asc_store_dev](../../../../api/SIMD-API/c_api/scalar_compute/scalar_store/asc_store_dev.md), [WriteGmBypassDCache](../../../../api/SIMD-API/basic_api/scalar_compute/WriteGmBypassDCache_ISASI.md), and Scalar atomic operations. When both the preceding and subsequent accesses bypass the DCache, regardless of whether they access the same address or different addresses, if the subsequent operation depends on the completion of the preceding access, the user needs to insert a DSB to wait for the preceding GM access to complete.

In a same-address WAW scenario, if the final result is expected to be determined by the last write, you cannot rely solely on code order:

```text
Time    Scalar pipeline
======    ====================================
T0        Scalar.STORE_GM_no_cache(addr) = 1
T1        Scalar.STORE_GM_no_cache(addr) = 2
T2        The final value of GM[addr] is not guaranteed to be 2
```

The correct handling is to wait for the preceding DCache-bypassing access write to complete before the last write:

```text
Time    Scalar pipeline
======    ====================================
T0        Scalar.STORE_GM_no_cache(addr) = 1
T1        Scalar.DSB
          // Wait for the preceding GM write to complete

T2        Scalar.STORE_GM_no_cache(addr) = 2
```

In a different-address publish scenario, if `data` is expected to be externally visible before `flag`, you also need to wait for the `data` write to complete before writing `flag`:

```text
Time    Scalar pipeline
======    ====================================
T0        Scalar.STORE_GM_no_cache(data) = 1
T1        Scalar.DSB
T2        Scalar.STORE_GM_no_cache(flag) = 1
```

##### Case 4: Mixed Same-Address Access Through DCache and Non-DCache Paths

When the same GM address is accessed through both the DCache path and the non-DCache path, the hardware does not guarantee that ordering and cache copies are automatically maintained between the two paths. For example, if a preceding DCache write creates a Dirty copy, a subsequent non-DCache write directly updates GM, but when the DCache Dirty copy is later written back, it overwrites the non-DCache write:

```text
Time point    Scalar pipeline
======    ====================================
T0        Scalar.STORE_GM_DCache(addr) = 1
          // DCache[addr] = 1, Dirty; GM[addr] may still hold the old value

T1        Scalar.STORE_GM_no_cache(addr) = 2
          // GM[addr] = 2

T2        The DCache Dirty copy is written back later
          // GM[addr] is overwritten to 1
```

The correct handling is to write back the existing DCache Dirty copy in the cache line containing the same address to GM before the non-DCache write, and wait for the write-back to complete:

```text
Time point    Scalar pipeline
======    ====================================
T0        Scalar.STORE_GM_DCache(addr) = 1
T1        Scalar.DCCI(addr)
T2        Scalar.DSB
          // Wait for the DCache Dirty copy to be written back to GM

T3        Scalar.STORE_GM_no_cache(addr) = 2
```

Conversely, if a non-DCache access has already updated GM and the current core's DCache has already cached the cache line containing the same address, when the address is subsequently read through the DCache path, the stale copy in the DCache must be handled first. For the handling method, see the read-side maintenance method for DCache and GM consistency in [Cache Coherence](cache_coherence.md). If the current core's DCache already has a Dirty copy of the cache line containing the same address, DCCI and `Scalar.DSB` should be executed before the non-DCache write, following the previous process, to prevent the stale Dirty copy from overwriting the non-DCache write.


#### Memory Consistency Between Scalar and Other Hardware Units

When there is a data dependency between Scalar and other hardware units, the user needs to insert synchronization to ensure memory consistency. For example, if Scalar writes GM first and then MTE2, Vector, Fixpipe<!-- npu="950" id10 -->, or SIMT<!-- end id10 --> reads the data, you cannot rely solely on the code order to infer that the subsequent hardware units will definitely see the Scalar write.

The following uses the example of Scalar writing GM and then MTE2 reading GM:

```text
Time point    Scalar pipeline                              MTE2 pipeline
======    ====================================    ====================================
T0        Scalar.STORE_GM(data) = 1
T1        Scalar.DCCI
          // Add DCCI when the Scalar write to GM passes through DCache.
T2        Scalar.DSB
          // Wait for the Scalar write to GM to complete.

T3        sync S->MTE2
          // Notify MTE2 that it can read.

                                                T3  WAIT S->MTE2
                                                T4  MTE2.LOAD_GM(data)
```

Here, `Scalar.DSB` can be implemented through [asc_sync_data_barrier](../../../../api/SIMD-API/c_api/sync/intra_core_sync/asc_sync_data_barrier.md), which is used to wait for the preceding GM accesses on the Scalar side to complete. `sync S->MTE2` can be implemented through [asc_sync_notify](../../../../api/SIMD-API/c_api/sync/intra_core_sync/asc_sync_notify.md)/[asc_sync_wait](../../../../api/SIMD-API/c_api/sync/intra_core_sync/asc_sync_wait.md) to establish the ordering relationship from the Scalar pipeline to the MTE2 pipeline:

`DSB` and pipeline synchronization only handle the access completion order and the execution ordering relationship to ensure memory consistency, and are not responsible for writing back or invalidating DCache copies. If the Scalar write to GM passes through DCache, or if the subsequent read path may hit a stale cache copy, you also need to refer to [Cache Coherence](cache_coherence.md) to handle DCache-related issues.

<!-- npu="950" id11 -->
### SIMT Memory Consistency

SIMT memory consistency must distinguish between intra-thread and inter-thread access:

#### Same-Address Access Within a Single Thread

When the same thread of SIMT accesses the same address, regardless of whether it passes through the SIMT DCache, the hardware guarantees that the execution result is consistent with the code order. In such scenarios, the user does not need to insert additional synchronization to guarantee the access order to the same address within the same thread.

```text
Time Point    Thread 0
======    ====================================
T0        STORE addr = 1
T1        LOAD addr -> 1
T2        STORE addr = 2
T3        LOAD addr -> 2
```


#### Accessing Different Addresses Within a Single Thread

Within the same thread of SIMT, when accessing different addresses, the hardware does not guarantee that the externally visible order of accesses to different addresses is consistent with the code order. If subsequent logic requires a release order such as "write data first, then write flag" between different addresses, the user needs to use a memory fence (MEMBAR) to establish the visibility order.

Incorrect example:

```text
Time Point    Producer Thread0                           Consumer Thread1
======    ====================================    ====================================
T0        STORE data = value
T1        STORE ready = 1
                                                T2  LOAD ready -> 1
                                                T3  LOAD data -> ?
```

In the preceding example, `ready` and `data` are operations on different addresses within the same thread. When the consumer observes `ready = 1`, it does not necessarily mean that the write to `data` has become visible to the consumer as expected. The producer needs to insert a MEMBAR between writing the data and publishing the flag; the consumer still needs to wait for the flag through synchronization methods such as polling.

Positive example:

```text
Time Point    Producer Thread0                           Consumer Thread1
======    ====================================    ====================================
T0        STORE data = value
T1        MEMBAR
          // Ensure that the data write becomes externally visible before the ready flag is published

T2        STORE ready = 1
                                                T3  WAIT ready == 1
                                                T4  LOAD data -> value
```

The MEMBAR in the preceding example can be implemented using [asc_threadfence](../../../../api/SIMT-API/sync_and_memory_fence/memory_fence/asc_threadfence.md), which is suitable for scenarios where visibility to all other threads is required. If the visibility scope only needs to cover the current Thread Block, [asc_threadfence_block](../../../../api/SIMT-API/sync_and_memory_fence/memory_fence/asc_threadfence_block.md) can be used. MEMBAR only establishes the visibility order of memory operations before and after it in the current thread and does not block the thread.

If the consumer uses SIMT DCache when accessing GM, it also needs to handle stale copies in combination with the [Cache Coherence](cache_coherence.md) requirements between SIMT DCache and GM. MEMBAR is responsible for constraining the access order, but it does not automatically invalidate the cache copies on the consumer side.


#### Inter-Thread Access

Between different threads in SIMT, regardless of whether the accesses target the same address, the hardware does not automatically guarantee inter-thread memory consistency. If data written by one thread is to be read by another thread, the user needs to insert synchronization manually.

For example, Thread 0 writes shared data, and Thread 1 reads it subsequently. Without synchronization, Thread 1 may read before Thread 0 completes the write:

```text
Time Point    Thread 0                                Thread 1
======    ====================================    ====================================
T0        STORE shared_data = value
                                                T1  LOAD shared_data -> ?
```

If the producer and consumer are in the same Thread Block, you can use [asc_syncthreads](../../../../api/SIMT-API/sync_and_memory_fence/sync_interface/asc_syncthreads.md) to establish phase synchronization:

```text
Time Point    Thread 0                                Thread 1
======    ====================================    ====================================
T0        STORE shared_data = value
T1        asc_syncthreads()                       asc_syncthreads()
          // Wait for all threads in the current Thread Block to reach the synchronization point.

                                                T2  LOAD shared_data -> value
```

`asc_syncthreads` blocks until all threads in the current Thread Block reach the synchronization point, and guarantees that memory operations before the synchronization point are visible to threads within the block.

<!-- end id11 -->


## Inter-Core Memory Consistency

### Inter-Core Consistency Issues and Ordering Risks of Multi-Core Access to GM

Different AI Cores typically share data through GM. The hardware does not automatically establish a global read/write order for multiple AI Cores: when one core writes to GM and another core reads from GM, if there is no explicit synchronization, the actual completion order or externally visible order of the read/write operations may be inconsistent with what the code expects. If the access path passes through the DCache, [Cache Coherence](cache_coherence.md) issues such as dirty copies on the write side and stale copies on the read side are also superimposed.

When multiple cores access GM, common risks can be understood in terms of three dependency types: RAW, WAW, and WAR.

RAW (Read After Write) requires that a subsequent read operation read the new value written by a preceding write operation. If the consumer does not wait for the producer to publish the data, it may read the old value prematurely; if the read-side DCache already holds a stale copy, it may still read the old value even after synchronization.

```text
Time point    Core 0                                  Core 1
======    ====================================    ====================================
T0        STORE data = 1
                                                T1  LOAD data -> old value or indeterminate value
```

WAW (Write After Write) requires that the data ultimately retained in GM be that of the later write operation. If the write order of two cores to the same address or to data within the same cache line is not established, the final result may not meet the expectation that "the later write overwrites the earlier write"; if the preceding write forms a dirty copy through the DCache, the subsequent write may also be overwritten by the writeback of the preceding dirty copy.

```text
Time point    Core 0                                  Core 1
======    ====================================    ====================================
T0        STORE data = 1
                                                T1  STORE data = 2
T2        The preceding write or dirty copy becomes visible to GM later than Core 1's write
Result      GM may not contain the value that Core 1 expects to retain
```

WAR (Write After Read) requires that the preceding read operation reads the old value before the subsequent write operation. If the later-writing core publishes the new value too early, the earlier-reading core may read the new value, breaking the phase relationship of "read the old value first, then write the new value."

```text
Time    Core 0                                  Core 1
======    ====================================    ====================================
T0                                                STORE data = 1
T1        LOAD data -> new value
Result      Core 0 does not read the old value before the subsequent write
```

In these scenarios, inter-core memory consistency must be manually maintained by users.

### Inter-Core Synchronization Primitives

Inter-core synchronization primitives are used to establish arrival, waiting, or barrier relationships among multiple AI Cores. When selecting an interface, first confirm the synchronization scope: whether all participating cores need to be aligned, or whether ordering only needs to be established between certain groups, or between blocks/subblocks within the same group.

In separate mode, the AIC (Cube Core) and AIV (Vector Core) are divided by group. A group consists of 1 block (primary core) and N subblocks (secondary cores), with a ratio of 1:N. Therefore, inter-group synchronization refers to block synchronization between different groups, and intra-group synchronization refers to synchronization between the block and subblocks within the same group.

| API | Synchronization Scope | Scope Boundary |
| --- | --- | --- |
| [asc_sync_inter_arrive](../../../../api/SIMD-API/c_api/sync/inter_core_sync/asc_sync_inter_arrive.md)/[asc_sync_inter_wait](../../../../api/SIMD-API/c_api/sync/inter_core_sync/asc_sync_inter_wait.md) | Inter-group block synchronization, that is, block synchronization between different groups. | Passes synchronization signals through `flag_id`, suitable for producer-consumer synchronization between groups. |
| [asc_sync_block_arrive](../../../../api/SIMD-API/c_api/sync/inter_core_sync/asc_sync_block_arrive.md)/[asc_sync_block_wait](../../../../api/SIMD-API/c_api/sync/inter_core_sync/asc_sync_block_wait.md) | Intra-group block and subblock synchronization, that is, synchronization between the AIC and AIV within the same group. | Passes synchronization signals through `flag_id`, suitable for establishing ordering between the Cube side and Vector side within the same group. |

The `arrive`/`wait` interfaces only pass synchronization signals; `flag_id` must be used in pairs, and counter overflow and ID conflicts must be avoided. After using these interfaces, you still need to determine whether DCCI and DSB are required based on the data access path.

Taking the inter-core producer-consumer scenario as an example, the following shows the abstract process in which the producer is responsible for writing GM data and publishing a synchronization signal, and the consumer reads the GM data after waiting for the synchronization signal:

```text
Time Point    Producer Core                          Consumer Core
======    ====================================   ====================================
T0        STORE data = value
T1        DCCI data
T2        Scalar.DSB
T3        ARRIVE sync_point
                                                T4  WAIT sync_point
                                                T5  DCCI data
                                                T6  Scalar.DSB
                                                T7  LOAD data -> value
```

Among them, at T1 and T2, the Dirty copies that may be formed after the producer writes through the DCache are handled to ensure that the data has been written back to GM; at T3 and T4, the inter-core publish/acquire ordering is established; at T5 and T6, the stale DCache copies that may exist in the consumer's own core are handled. If neither the producer's nor the consumer's data access path passes through the DCache, the corresponding DCCI can be omitted according to the actual path, but synchronization primitives are still required to establish cross-core execution ordering.

The `ARRIVE sync_point` and `WAIT sync_point` in the preceding example represent abstract inter-core synchronization actions. Inter-group block synchronization can be implemented using `asc_sync_inter_arrive`/`asc_sync_inter_wait`; synchronization between the block and subblocks within the same group can be implemented using `asc_sync_block_arrive`/`asc_sync_block_wait`.

### Atomic Operations and Memory Consistency

Atomic operations are used to ensure that the read-modify-write process on the same GM address is indivisible. When multiple AI Cores update the same address simultaneously, ordinary writes may overwrite each other. With atomic operations, each update computes based on a previous value at that address and writes the new value back to the same address, avoiding data races.

For example, multiple cores perform atomic accumulation on the same address simultaneously:

```text
Initial value    GM[dst] = 0

Time point    Core 0                                  Core 1
======    ====================================    ====================================
T0        ATOMIC_ADD dst, 1
                                                T1  ATOMIC_ADD dst, 1

Result      GM[dst] = 2
```

The `ATOMIC_ADD` in the preceding example can be implemented using APIs such as [asc_atomic_add](../../../../api/SIMD-API/c_api/atomic/scalar_atomic/asc_atomic_add.md). Regardless of the order in which the atomic additions of Core 0 and Core 1 take effect, the final result is `2`. Therefore, this example only shows that atomic operations prevent concurrent updates to the same address from being lost, and does not show that atomic operations can establish the cross-core order expected by users.

When ordering is required between different atomic operations, explicit synchronization is still needed. For example, if the business requires Core 0 to perform an atomic maximum first and Core 1 to perform an atomic accumulation afterward:

```text
Initial value    GM[dst] = 0

Time point    Core 0                                  Core 1
======    ====================================    ====================================
T0        ATOMIC_MAX dst, 10
                                                T1  ATOMIC_ADD dst, 1

Possible result  ATOMIC_MAX completes first, ATOMIC_ADD completes later, GM[dst] = 11
Possible result  ATOMIC_ADD completes first, ATOMIC_MAX completes later, GM[dst] = 10
```

Neither result violates atomicity, because each atomic operation itself remains indivisible; the issue is that no execution order of "compute the maximum first, then accumulate" is established between the two cores. If the business requires the result to be fixed at `11`, inter-core synchronization must be inserted between the two stages:

```text
Initial value    GM[dst] = 0

Time point    Core 0                                  Core 1
======    ====================================    ====================================
T0        ATOMIC_MAX dst, 10
T1        ARRIVE sync_point
                                                T2  WAIT sync_point
                                                T3  ATOMIC_ADD dst, 1

Result      GM[dst] = 11
```

Therefore, the boundary between atomic operations and memory consistency can be understood as follows:

| Mechanism | Guaranteed content | Content not guaranteed |
| --- | --- | --- |
| Atomic Operations | Guarantees that the read-modify-write process on the same address is indivisible. | Does not guarantee the completion order of preceding and following ordinary reads/writes or their visibility to other cores. |
| Memory consistency maintenance | Guarantees that ordinary data is visible between the producer and consumer in the expected order. | Does not replace the protection provided by atomic operations against concurrent updates to the same address. |

If the user has ordering requirements for the program execution result, they need to insert synchronization themselves to handle consistency issues.


## Host-Device Memory Consistency

### Asynchronous Dispatch Semantics

On the host side, tasks such as kernels and asynchronous memory copies are dispatched to the device through the Runtime. For asynchronous tasks, the return of the host-side API usually only indicates that the task has been dispatched to the Stream, not that the task has completed execution on the device. The host thread continues to execute downward, while the device task waits in the corresponding Stream for scheduling and execution.

If subsequent host code depends on the result of a device task, it cannot rely solely on the host-side code order; it must explicitly wait for the device task to complete.

```text
Time Point    Host Thread                                Stream
======    ====================================    ====================================
T0        Dispatch Task A
T1        Host continues execution
T2        Host reads the result of Task A
          // Task A may not have completed at this point
                                                T3  Task A completes execution
```

Tasks in the same Stream are executed in the order in which they enter the queue; different Streams can execute in parallel by default, and no dependency is automatically established based on the host-side dispatch order. If a data dependency is required between Streams, the ordering should be established through Events or host-side explicit synchronization APIs.

### Host-Side Explicit Synchronization APIs

Host-side explicit synchronization blocks the host thread and waits for tasks within a specified scope on the device to complete. The commonly used APIs are as follows:

| API | Wait Scope | Typical Scenario |
| --- | --- | --- |
| `aclrtSynchronizeStream(stream)` | Waits for the tasks that have been dispatched to a specified stream to complete. | The host depends only on the task results of a specific stream. |
| `aclrtSynchronizeDevice()` | Waits for all stream tasks in the current context of the current device to complete. | The host indeed needs to wait for all device tasks in the current context to complete. |
| `aclrtSynchronizeEvent(event)` | Waits for a specified event to complete. | The host only needs to wait for a specific task stage to complete. |

When selecting a synchronization API, use the smallest wait scope whenever possible. If you depend only on the results of a specific stream, use `aclrtSynchronizeStream`. If you depend only on a specific task stage, you can wait for the corresponding event. Use `aclrtSynchronizeDevice` only when you indeed need to wait for all stream tasks in the current context to complete.

### Cross-Stream Task Dependency

Tasks across streams cannot directly depend on the dispatch order on the host side. Even if the host first dispatches a producer task to `stream0` and then dispatches a consumer task to `stream1`, the tasks on the two streams may still be scheduled in parallel, and the consumer task may complete before the producer task.

Example without an established dependency:

```text
Time    Host thread                            Stream 0                               Stream 1
======    ====================================    ====================================   ====================================
T0        Dispatch Producer Task to stream0
T1        Dispatch Consumer Task to stream1
                                                                                    T2  Consumer Task may execute first
                                                T3  Producer Task completes
```

If the consumer task in `stream1` depends on the producer task in `stream0`, use an Event to establish a fine-grained task-level dependency:

```text
Time    Stream 0                               Stream 1
======    ====================================   ====================================
T0        Producer Task
T1        RECORD event
                                               T2  WAIT event
                                               T3  Consumer Task
```

The `RECORD event` in the preceding example can be implemented by calling `aclrtRecordEvent(event, stream0)`, which captures the tasks that have been dispatched on `stream0`. The `WAIT event` can be implemented by calling `aclrtStreamWaitEvent(stream1, event)`, which dispatches a wait task on `stream1` so that tasks after the wait point wait for the tasks captured by the event to complete. `aclrtStreamWaitEvent` constrains only the subsequent tasks on the specified stream, making it suitable for expressing precise producer-consumer dependencies across streams.


## Common Consistency Errors and Debugging

| Symptom | Possible Root Cause | Troubleshooting Direction |
| --- | --- | --- |
| Incorrect or intermittent results | Synchronization is missing across pipelines, cores, or streams, so the consumer may read data before the producer finishes. | Trace the dependency chain backward from the data producer and consumer, and confirm whether pipeline synchronization, inter-core synchronization, or an Event has been inserted at the dependency boundary. |
| The current core reads stale, incomplete, or abnormal values | A later pipeline accesses the same data without waiting for the preceding pipeline to finish, or same-pipeline overlapping-address access lacks completion waiting. | Check whether `notify/wait` is used across pipelines, and whether `sync_pipe` is used for same-pipeline overlapping access to wait for the preceding access to complete. |
| Timeout or hang | Synchronization signals are not matched in pairs, event_id or flag_id is inconsistent, or reserved EventId 6 and 7 are used. | Check whether the source pipeline, target pipeline, event_id, or flag_id of `notify/wait` and `arrive/wait` match, and confirm that the producer corresponding to the waiting side is guaranteed to execute; avoid manually using 6 and 7. |
| Multiple cores writing the same GM address produce a smaller-than-expected result | Ordinary writes from multiple cores overwrite each other, or ordinary read-modify-write loses updates because atomic operations are not used to protect concurrent updates to the same address. | Confirm whether concurrent updates such as multi-core accumulation or extremum computation exist on the same GM address; use atomic operations when read-modify-write to the same address must be indivisible. |
| Scalar reads a stale GM value | The Dirty copy in the write-side DCache is not written back, or a stale copy exists in the read-side DCache. This is a cache coherence issue. | Check whether the write side executes DCCI and `Scalar.DSB`, and whether the read side needs to invalidate the stale copy before reading; for specific maintenance methods, see [Cache Coherence](cache_coherence.md). |
| Host does not read the ready result | Host only submits the task and does not wait for the Device task to finish executing. | Check whether Host calls `aclrtSynchronizeStream`, `aclrtSynchronizeDevice`, or `aclrtSynchronizeEvent` to wait for the corresponding task to complete before reading the result. |
| Data disorder across streams | There is no default dependency between different streams, so the consumer task may execute before the producer task. | Check whether the producer stream records an Event and whether the consumer stream waits for that Event via `aclrtStreamWaitEvent` before executing the dependent task. |

## Quick Decision Reference

To determine whether users need to maintain memory consistency, first identify which type of consistency boundary the access crosses:

| Consistency Boundary | Preferred Mechanism | Responsible Party |
| --- | --- | --- |
| Within the same pipeline | Same-pipeline synchronization, for example `asc_sync_pipe(PIPE_xxx)`; use `asc_sync_data_barrier` for Scalar access completion. | The user waits for preceding access completion at locations where same-address, overlapping-address, or dependent accesses exist. |
| Across pipelines within a core | Inter-pipeline synchronization, for example `asc_sync_notify`/`asc_sync_wait`, `asc_lock`/`asc_unlock`. | The user publishes a synchronization signal on the producer pipeline and waits on the consumer pipeline before accessing data. |
| Scalar access to GM | `asc_sync_data_barrier` combined with the necessary DCCI. | The user waits for Scalar-side GM access completion; if the access goes through DCache, dirty or stale copies must also be handled according to [Cache Coherence](cache_coherence.md). |
| Across multiple cores | Inter-core synchronization, atomic operations, and DCCI and DSB when necessary. | The user establishes ordering between the producer Core and the consumer Core; multi-core same-address read-modify-write uses atomic operations, while ordinary data transfer still requires visibility handling. |
| Across Streams | Event task-level dependency. | The user needs to handle inter-Stream synchronization through Events. |
| Host waiting for Device | Host-side explicit synchronization APIs, for example `aclrtSynchronizeStream`, `aclrtSynchronizeDevice`, `aclrtSynchronizeEvent`. | The user needs to add synchronization APIs to ensure that the host side waits for the corresponding Device task to complete before reading Device results or reusing related resources. |

## Summary

NPU memory consistency is closer to a weak consistency model. Ordinary memory accesses provide only limited local ordering by default, and the hardware does not guarantee a globally consistent read/write order across pipelines, AI Cores, streams, or between the host and device.

Therefore, users need to proactively maintain memory consistency based on data dependencies: establish synchronization between producers and consumers, wait for preceding memory accesses to complete when necessary, and coordinate with cache coherence maintenance when cache copies are involved.
