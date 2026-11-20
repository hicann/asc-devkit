# Intra-core Synchronization Overview<a name="ZH-CN_TOPIC_0000002555621170"></a>

<!-- md-trans-meta sourceCommit=c14730e9a76fabd38b673e588b8c2e8f65f06fc8 translatedAt=2026-11-05T05:20:12.167Z pushedAt=2026-11-18T01:25:31.165Z nl=2 -->

## Why Is Intra-Core Synchronization Needed<a name="zh-cn_topic_0000002542725361_section199075142046"></a>

The execution units inside the AI Core (such as the **MTE2** copy unit and the **Vector** compute unit) run asynchronously and in parallel, and data dependencies may exist when they read from and write to the same storage resources. To ensure data consistency and computation correctness, synchronization control is required to coordinate the operation timing.

<!-- npu="910b,A3" id1 -->

For [NPU architecture 2201](../../../../../guide/programming_guide/language_extension/simd_builtin_keywords.md#npu-arch), the hardware architecture diagram is as follows. The highlighted parts show the compute units and copy units that execute in parallel.

**Figure 1** Architecture diagram<a name="zh-cn_topic_0000002542725361_fig11732135516285"></a>  
![](../../../../figures/atlas_a2_a3_architecture.png "Atlas A2 A3 hardware architecture diagram")

<!-- end id1 -->

<!-- npu="950" id2 -->

For [NPU architecture 3510](../../../../../guide/programming_guide/language_extension/simd_builtin_keywords.md#npu-arch), the hardware architecture diagram is as follows. The highlighted parts show the compute units and copy units that execute in parallel.

**Figure 2** Architecture diagram<a name="zh-cn_topic_0000002542725361_fig31512113433"></a>  
![](../../../../figures/npu_3510_hw_arch_sync.png "Ascend 950PR 950DT hardware architecture diagram")

<!-- end id2 -->

The following example describes a common **Vector** compute data flow:

1. First, the **DMA** execution unit moves data from Global Memory (GM) into Local Memory.
2. Perform computation.
3. Then, the **DMA** execution unit moves the computation results from Local Memory out to GM.

![](../../../../figures/vector_compute_data_flow.png "Vector compute data flow diagram")

The four execution units Scalar, Vector, DMA (MTE2), and DMA (MTE3) execute in parallel. If they access the same Local Memory, a synchronization mechanism is required to control their access timing: ensure that data is copied into Local Memory before computation, and copied out after computation completes.

![](../../../../figures/intra_core_parallel_pipeline_execution_timing.png "intra_core_parallel_pipeline_execution_timing")

## Hardware Pipeline Type<a name="zh-cn_topic_0000002542725361_section1272612276459"></a>

The instruction pipeline types that run in parallel inside the AI Core and their descriptions are as follows:

> [!NOTE]Note 
>Different hardware architectures may have different specific pipelines included in each hardware pipeline type. For details, see [Hardware Implementation](../../../../../guide/programming_guide/advanced_programming/hardware_implementation/hardware_implementation.md).

**Table 1**  Instruction pipeline types and related descriptions

| Pipeline type | Meaning |
| --- | --- |
| PIPE_S | Scalar pipeline, used when calling the GetValue function of GlobalTensor/LocalTensor |
| PIPE_V | Vector computation pipeline, and the L0C Buffer-&gt;UB data copy pipeline under some hardware architectures |
| PIPE_M | Matrix computation pipeline |
| PIPE_MTE1 | L1 Buffer -&gt;L0A Buffer and L1 Buffer-&gt;L0B Buffer data copy pipeline |
| PIPE_MTE2 | GM-&gt;L1 Buffer and GM-&gt;UB data copy pipeline |
| PIPE_MTE3 | UB-&gt;GM and L1 Buffer-&gt;GM data copy pipeline |
| PIPE_FIX | L0C Buffer-&gt;GM and L0C Buffer -&gt;L1 data copy pipeline |

## Intra-core Synchronization Classification<a name="zh-cn_topic_0000002542725361_section2167161594419"></a>

The synchronization control for the intra-core parallel pipelines described above can be selected according to Table 2.

**Table 2**  Classification of intra-core synchronization APIs

| Synchronization Type | API | Applicable Scenario | Difference |
| --- | --- | --- | --- |
| Multi-pipeline synchronization | [TQueSync](TQueSync.md) | Synchronization between instructions of different pipeline types that have data dependencies within the same core. | A cross-hardware-version compatible API. The source pipeline and destination pipeline are specified through template parameters, and synchronization between a pair of pipelines is then completed through the member functions `SetFlag`/`WaitFlag`. |
| Multi-pipeline synchronization | [SetFlag/WaitFlag\(ISASI\)](SetFlag_WaitFlag_ISASI.md) | Synchronization between instructions of different pipeline types that have data dependencies within the same core. | You need to explicitly specify a pair of source and destination pipelines. `SetFlag` sets the flag after all read and write operations of the preceding instructions on the source pipeline are complete; `WaitFlag` blocks the destination pipeline until the flag is detected, then unblocks it and clears the flag. This API is an ISASI-type API and does not guarantee cross-hardware-version compatibility. |
| Multi-pipeline synchronization | [Lock](Lock.md)/[Unlock](Unlock.md) | Synchronization control between different pipelines within the same core. | Locks and releases a specified pipeline through MutexID. `Lock` acquires the Mutex based on MutexID. If the Mutex is already locked, it blocks the execution of subsequent instructions on the specified pipeline; `Unlock` releases the corresponding Mutex after the preceding instructions on the specified pipeline are complete. Compared with `SetFlag`/`WaitFlag`, this combination is more cohesive and decoupled from other pipelines, which simplifies reverse synchronization logic. For supported cases, see the API documentation. |
| Single-pipeline synchronization | [PipeBarrier](PipeBarrier_ISASI.md) | Synchronization between instructions of the same pipeline type that have data dependencies within the same core. | Although instructions in the same pipeline are executed sequentially, the preceding instruction may not have finished executing when the next instruction starts. `PipeBarrier` ensures that all data reads and writes in the preceding instructions are complete before the subsequent instructions start executing. This API does not support `PIPE_S` single-pipeline synchronization; to block all pipelines, use `PipeBarrier<PIPE_ALL>()`. |
| Memory access barrier | [DataSyncBarrier](DataSyncBarrier_ISASI.md) | Used when you need to wait for all previously issued but not yet completed memory access instructions to finish executing. | Used to resolve Scalar memory dependencies. The memory location to wait for is specified through template parameters, ensuring that memory access instructions before and after the barrier complete in the expected order. |

<!-- npu="910b,A3" id4 -->

Take [NPU architecture 2201](../../../../../guide/programming_guide/language_extension/simd_builtin_keywords.md#npu-arch) as an example. All legal intra-core synchronization combinations under this hardware architecture are shown in [Table 3](#zh-cn_topic_0000002542725361_table13753342164113) and [Table 4](#zh-cn_topic_0000002542725361_table1555712177426). Among them, "Not involved" indicates that this synchronization combination does not exist at the hardware level, and "No application scenario" indicates that this synchronization combination exists but is not required in actual development scenarios.

**Table 3**  All legal intra-core synchronization combinations in AIC<a name="zh-cn_topic_0000002542725361_table13753342164113"></a>

| Source pipeline | PIPE_S | PIPE_M | PIPE_MTE1 | PIPE_MTE2 | PIPE_MTE3 | PIPE_FIX |
| --- | --- | --- | --- | --- | --- | --- |
| PIPE_S | Not involved | Not involved | Not involved | Not involved | Not involved | Not involved |
| PIPE_M | Not involved | PipeBarrier&lt;PIPE_M&gt;() | SetFlag&lt;M_MTE1&gt;(0)<br>WaitFlag&lt;M_MTE1&gt;(0) | SetFlag&lt;M_MTE2&gt;(0)<br>WaitFlag&lt;M_MTE2&gt;(0) | Not involved | SetFlag&lt;M_FIX&gt;(0)<br>WaitFlag&lt;M_FIX&gt;(0) |
| PIPE_MTE1 | Not involved | SetFlag&lt;MTE1_M&gt;(0)<br>WaitFlag&lt;MTE1_M&gt;(0) | PipeBarrier&lt;PIPE_MTE1&gt;() | SetFlag&lt;MTE1_MTE2&gt;(0)<br>WaitFlag&lt;MTE1_MTE2&gt;(0) | SetFlag&lt;MTE1_MTE3&gt;(0)<br>WaitFlag&lt;MTE1_MTE3&gt;(0) | SetFlag&lt;MTE1_FIX&gt;(0)<br>WaitFlag&lt;MTE1_FIX&gt;(0) |
| PIPE_MTE2 | Not involved | SetFlag&lt;MTE2_M&gt;(0)<br>WaitFlag&lt;MTE2_M&gt;(0) | SetFlag&lt;MTE2_MTE1&gt;(0)<br>WaitFlag&lt;MTE2_MTE1&gt;(0) | PipeBarrier&lt;PIPE_MTE2&gt;() | SetFlag&lt;MTE2_MTE3&gt;(0)<br>WaitFlag&lt;MTE2_MTE3&gt;(0) | No application scenario |
| PIPE_MTE3 | Not involved | Not involved | SetFlag&lt;MTE3_MTE1&gt;(0)<br>WaitFlag&lt;MTE3_MTE1&gt;(0) | SetFlag&lt;MTE3_MTE2&gt;(0)<br>WaitFlag&lt;MTE3_MTE2&gt;(0) | PipeBarrier&lt;PIPE_MTE3&gt;() | No application scenario |
| PIPE_FIX | Not involved | SetFlag&lt;FIX_M&gt;(0)<br>WaitFlag&lt;FIX_M&gt;(0) | SetFlag&lt;FIX_MTE1&gt;(0)<br>WaitFlag&lt;FIX_MTE1&gt;(0) | No application scenario | No application scenario | PipeBarrier&lt;PIPE_FIX&gt;() |

**Table 4**  All legal intra-core synchronization combinations in AIV<a name="zh-cn_topic_0000002542725361_table1555712177426"></a>

| Source pipeline | PIPE_S | PIPE_V | PIPE_MTE2 | PIPE_MTE3 |
| --- | --- | --- | --- | --- |
| PIPE_S | Not involved | SetFlag&lt;S_V&gt;(0)<br>WaitFlag&lt;S_V&gt;(0) | SetFlag&lt;S_MTE2&gt;(0)<br>WaitFlag&lt;S_MTE2&gt;(0) | SetFlag&lt;S_MTE3&gt;(0)<br>WaitFlag&lt;S_MTE3&gt;(0) |
| PIPE_V | SetFlag&lt;V_S&gt;(0)<br>WaitFlag&lt;V_S&gt;(0) | PipeBarrier&lt;PIPE_V&gt;() | SetFlag&lt;V_MTE2&gt;(0)<br>WaitFlag&lt;V_MTE2&gt;(0) | SetFlag&lt;V_MTE3&gt;(0)<br>WaitFlag&lt;V_MTE3&gt;(0) |
| PIPE_MTE2 | SetFlag&lt;MTE2_S&gt;(0)<br>WaitFlag&lt;MTE2_S&gt;(0) | SetFlag&lt;MTE2_V&gt;(0)<br>WaitFlag&lt;MTE2_V&gt;(0) | PipeBarrier&lt;PIPE_MTE2&gt;() | SetFlag&lt;MTE2_MTE3&gt;(0)<br>WaitFlag&lt;MTE2_MTE3&gt;(0) |
| PIPE_MTE3 | SetFlag&lt;MTE3_S&gt;(0)<br>WaitFlag&lt;MTE3_S&gt;(0) | SetFlag&lt;MTE3_V&gt;(0)<br>WaitFlag&lt;MTE3_V&gt;(0) | SetFlag&lt;MTE3_MTE2&gt;(0)<br>WaitFlag&lt;MTE3_MTE2&gt;(0) | PipeBarrier&lt;PIPE_MTE3&gt;() |

<!-- end id4 -->

## When Do Developers Need to Manually Insert Synchronization

Both the TPipe-TQue framework programming paradigm and the Bisheng Compiler support automatic synchronization, which reduces the workload of manually inserting synchronization for developers. However, in some scenarios, developers still need to complete synchronization manually. For details, see [Automatic Synchronization](key_features.md).
