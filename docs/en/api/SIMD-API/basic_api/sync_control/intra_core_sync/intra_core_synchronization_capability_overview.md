# Intra-core Synchronization Overview<a name="ZH-CN_TOPIC_0000002555621170"></a>

<!-- md-trans-meta sourceCommit=f6490d1e030d257ee55f3f1600bffece5a8bd3f7 translatedAt=2026-08-27T13:57:33.445Z -->

## Why Intra-Core Synchronization Is Needed<a name="zh-cn_topic_0000002542725361_section199075142046"></a>

The execution units inside the AI Core (such as the MTE2 move engine and the Vector compute unit) run asynchronously and in parallel. When they read from and write to the same storage resource, data dependencies may exist. To ensure data consistency and computation correctness, synchronization control is required to coordinate the operation timing.

<!-- npu="910b,A3" id1 -->

For [NPU architecture 2201](https://gitcode.com/cann/asc-devkit/blob/9.1.0/docs/en/guide/programming_guide/language_extension/SIMD-BuiltIn_keyword.md#table65291052154114), the hardware architecture diagram is as follows. The highlighted parts show the compute units and move engines that execute in parallel.

**Figure 1**  Architecture diagram<a name="zh-cn_topic_0000002542725361_fig11732135516285"></a>  
![](../../../../figures/atlas_a2_a3_architecture.png "Atlas_A2_A3_hardware_architecture_diagram")

<!-- end id1 -->

<!-- npu="950" id2 -->

For [NPU architecture 3510](https://gitcode.com/cann/asc-devkit/blob/9.1.0/docs/en/guide/programming_guide/language_extension/SIMD-BuiltIn_keyword.md#table65291052154114), the hardware architecture diagram is as follows. The highlighted parts show the compute units and move engines that execute in parallel.

**Figure 2**  Architecture diagram<a name="zh-cn_topic_0000002542725361_fig31512113433"></a>  
![](../../../../figures/ascend_950pr_950dt_architecture.png "Ascend_950PR_950DT_hardware_architecture_diagram")

<!-- end id2 -->

The following example describes a common Vector compute data flow:

1. First, the DMA execution unit moves data from Global Memory into Local Memory.
2. Computation is performed.
3. Then, the DMA execution unit moves the computation results from Local Memory out to Global Memory.

![](../../../../figures/vector_compute_data_flow.png "Vector_compute_data_flow_diagram")

The four execution units Scalar, Vector, DMA (MTE2), and DMA (MTE3) execute in parallel. If they access the same Local Memory, a synchronization mechanism is required to control their access timing: ensure that data is moved into Local Memory before computation, and moved out after computation is complete.

![](../../../../figures/intra_core_parallel_pipeline_execution_timing.png "intra_core_parallel_pipeline_execution_timing")

## Hardware Pipeline Types<a name="zh-cn_topic_0000002542725361_section1272612276459"></a>

The parallel instruction pipeline types inside the AI Core and their descriptions are as follows:

> [!NOTE]
> Different hardware architectures may have different specific pipelines under each hardware pipeline type. For details, see [Hardware Implementation](https://gitcode.com/cann/asc-devkit/blob/9.1.0/docs/en/guide/programming_guide/Advanced programming/Hardware implementation/Hardware implementation.md).

**Table 1** Instruction pipeline types and related descriptions

| Pipeline type | Description |
| --- | --- |
| PIPE_S | Scalar pipeline, used when calling the GetValue function of GlobalTensor/LocalTensor |
| PIPE_V | Vector computation pipeline, and the L0C Buffer-&gt;UB data movement pipeline on some hardware architectures |
| PIPE_M | Cube computation pipeline |
| PIPE_MTE1 | L1 Buffer -&gt;L0A Buffer and L1 Buffer-&gt;L0B Buffer data movement pipeline |
| PIPE_MTE2 | GM-&gt;L1 Buffer and GM-&gt;UB data movement pipeline |
| PIPE_MTE3 | UB-&gt;GM data movement pipeline |
| PIPE_FIX | L0C Buffer-&gt;GM and L0C Buffer -&gt;L1 data movement pipeline |

## Intra-core Synchronization Classification<a name="zh-cn_topic_0000002542725361_section2167161594419"></a>

The synchronization control over the intra-core parallel pipelines described above falls into two categories:

- Multi-pipeline synchronization: synchronization between instructions of different pipeline types that have data dependencies within the same core.
    The difference between the [TQueSync](TQueSync Template Parameters.md) class APIs and the synchronization control APIs provided in [SetFlag/WaitFlag\(ISASI\)](SetFlag_WaitFlag_ISASI.md) is that the APIs in [SetFlag/WaitFlag\(ISASI\)](SetFlag_WaitFlag_ISASI.md) are marked as the ISASI category and cannot guarantee cross-hardware-version compatibility, whereas the TQueSync class APIs can guarantee cross-hardware-version compatibility.
    Synchronization control between different pipelines is performed through the SetFlag/WaitFlag APIs. In the SetFlag/WaitFlag instructions, a pair of instruction pipelines (a source pipeline and a destination pipeline) can be specified to define their execution order, representing a "lock" mechanism between the two instruction pipelines. Its working principle is as follows:
    - SetFlag: After all read and write operations of the preceding instructions in the source pipeline are completed, the current instruction starts executing and sets the corresponding flag in the hardware to 1.
    - **WaitFlag**: When the destination pipeline executes this instruction, if the corresponding flag bit in the hardware is found to be 0, the subsequent instructions of the destination pipeline are blocked; if the corresponding flag bit in the hardware is found to be 1, the corresponding flag bit in the hardware is set to 0, and the subsequent instructions of the destination pipeline start executing.

     <!-- npu="950" id3 -->
     Ascend 950PR/Ascend 950DT additionally support synchronization control between different pipelines through the [Lock](Lock.md)/[Unlock](Unlock.md) APIs. Lock a specified pipeline (blocking subsequent instructions) through **Lock**, and then release the pipeline through **Unlock** to complete the synchronization dependency between pipelines.
     - **Lock**: Obtains the Mutex based on the MutexID. If the Mutex is already locked, the subsequent instruction queue of the specified pipeline is blocked until the Mutex corresponding to the MutexID in the preceding instructions is unlocked by **Unlock**.
     - **Unlock**: After the preceding instructions of the current pipeline exit, releases the corresponding Mutex based on the MutexID.
     <!-- end id3 -->
- Single-pipeline synchronization: Synchronization between instructions of the same pipeline type that have data dependencies within the same core.
    - Use the [PipeBarrier](PipeBarrier_ISASI.md) API to control synchronization between the same pipelines. Although instructions in the same pipeline are executed sequentially, this does not mean that the preceding instruction has finished executing when the subsequent instruction starts. The PipeBarrier instruction ensures that all data reads and writes of the preceding instructions are complete before the subsequent instructions begin execution. Note that this API does not support synchronization of the PIPE\_S single pipeline.

<!-- npu="910b,A3" id4 -->

Taking [NPU architecture 2201](https://gitcode.com/cann/asc-devkit/blob/9.1.0/docs/en/guide/programming_guide/language_extension/SIMD-BuiltIn_keyword.md#table65291052154114) as an example, all legal intra-core synchronization combinations under this hardware architecture are shown in [Table 2](#zh-cn_topic_0000002542725361_table13753342164113) and [Table 3](#zh-cn_topic_0000002542725361_table1555712177426). Among them, "Not involved" indicates that this synchronization combination does not exist at the hardware level, and "No application scenario" indicates that this synchronization combination exists but is not required in actual development scenarios.

**Table 2**  All legal intra-core synchronization combinations in AIC<a name="zh-cn_topic_0000002542725361_table13753342164113"></a>

| Source pipeline | PIPE_S | PIPE_M | PIPE_MTE1 | PIPE_MTE2 | PIPE_MTE3 | PIPE_FIX |
| --- | --- | --- | --- | --- | --- | --- |
| PIPE_S | Not involved | Not involved | Not involved | Not involved | Not involved | Not involved |
| PIPE_M | Not involved | PipeBarrier&lt;PIPE_M&gt;() | SetFlag&lt;M_MTE1&gt;(0)<br>WaitFlag&lt;M_MTE1&gt;(0) | SetFlag&lt;M_MTE2&gt;(0)<br>WaitFlag&lt;M_MTE2&gt;(0) | Not involved | SetFlag&lt;M_FIX&gt;(0)<br>WaitFlag&lt;M_FIX&gt;(0) |
| PIPE_MTE1 | Not involved | SetFlag&lt;MTE1_M&gt;(0)<br>WaitFlag&lt;MTE1_M&gt;(0) | PipeBarrier&lt;PIPE_MTE1&gt;() | SetFlag&lt;MTE1_MTE2&gt;(0)<br>WaitFlag&lt;MTE1_MTE2&gt;(0) | SetFlag&lt;MTE1_MTE3&gt;(0)<br>WaitFlag&lt;MTE1_MTE3&gt;(0) | SetFlag&lt;MTE1_FIX&gt;(0)<br>WaitFlag&lt;MTE1_FIX&gt;(0) |
| PIPE_MTE2 | Not involved | SetFlag&lt;MTE2_M&gt;(0)<br>WaitFlag&lt;MTE2_M&gt;(0) | SetFlag&lt;MTE2_MTE1&gt;(0)<br>WaitFlag&lt;MTE2_MTE1&gt;(0) | PipeBarrier&lt;PIPE_MTE2&gt;() | SetFlag&lt;MTE2_MTE3&gt;(0)<br>WaitFlag&lt;MTE2_MTE3&gt;(0) | No application scenario |
| PIPE_MTE3 | Not involved | Not involved | SetFlag&lt;MTE3_MTE1&gt;(0)<br>WaitFlag&lt;MTE3_MTE1&gt;(0) | SetFlag&lt;MTE3_MTE2&gt;(0)<br>WaitFlag&lt;MTE3_MTE2&gt;(0) | PipeBarrier&lt;PIPE_MTE3&gt;() | No application scenario |
| PIPE_FIX | Not involved | SetFlag&lt;FIX_M&gt;(0)<br>WaitFlag&lt;FIX_M&gt;(0) | SetFlag&lt;FIX_MTE1&gt;(0)<br>WaitFlag&lt;FIX_MTE1&gt;(0) | No application scenario | No application scenario | PipeBarrier&lt;PIPE_FIX&gt;() |

**Table 3**  All legal intra-core synchronization combinations in AIV<a name="zh-cn_topic_0000002542725361_table1555712177426"></a>

| Source pipeline | PIPE_S | PIPE_V | PIPE_MTE2 | PIPE_MTE3 |
| --- | --- | --- | --- | --- |
| PIPE_S | Not involved | SetFlag&lt;S_V&gt;(0)<br>WaitFlag&lt;S_V&gt;(0) | SetFlag&lt;S_MTE2&gt;(0)<br>WaitFlag&lt;S_MTE2&gt;(0) | SetFlag&lt;S_MTE3&gt;(0)<br>WaitFlag&lt;S_MTE3&gt;(0) |
| PIPE_V | SetFlag&lt;V_S&gt;(0)<br>WaitFlag&lt;V_S&gt;(0) | PipeBarrier&lt;PIPE_V&gt;() | SetFlag&lt;V_MTE2&gt;(0)<br>WaitFlag&lt;V_MTE2&gt;(0) | SetFlag&lt;V_MTE3&gt;(0)<br>WaitFlag&lt;V_MTE3&gt;(0) |
| PIPE_MTE2 | SetFlag&lt;MTE2_S&gt;(0)<br>WaitFlag&lt;MTE2_S&gt;(0) | SetFlag&lt;MTE2_V&gt;(0)<br>WaitFlag&lt;MTE2_V&gt;(0) | PipeBarrier&lt;PIPE_MTE2&gt;() | SetFlag&lt;MTE2_MTE3&gt;(0)<br>WaitFlag&lt;MTE2_MTE3&gt;(0) |
| PIPE_MTE3 | SetFlag&lt;MTE3_S&gt;(0)<br>WaitFlag&lt;MTE3_S&gt;(0) | SetFlag&lt;MTE3_V&gt;(0)<br>WaitFlag&lt;MTE3_V&gt;(0) | SetFlag&lt;MTE3_MTE2&gt;(0)<br>WaitFlag&lt;MTE3_MTE2&gt;(0) | PipeBarrier&lt;PIPE_MTE3&gt;() |

<!-- end id4 -->

## When Do Developers Need to Manually Insert Synchronization

Both the TPipe-TQue framework programming paradigm and the Bisheng Compiler support automatic synchronization, which reduces the workload of manually inserting synchronization by developers. However, in some scenarios, developers still need to complete it manually. For details, see [Automatic Synchronization](key_feature_description.md).
