# Overview

<!-- md-trans-meta sourceCommit=1398d4baddf82d4b412922bb4a32a05abf2ada8a translatedAt=2026-11-16T01:40:03.577424+00:00 pushedAt=2026-11-16T02:10:57.859Z nl=2 -->

## Synchronization Control Overview

The synchronization control C APIs provide intra-core pipeline synchronization, as well as notification and wait APIs between cores and sub-cores.

### Synchronization Types and Usage Scenarios

In Ascend C programming, synchronization is used to constrain the execution order of different execution units inside and outside the AI Core, ensuring that subsequent instructions continue only after the dependent data, state, or synchronization signals are ready. Because data movement, computation, different kernel functions, or different AI Cores can run in parallel, without necessary synchronization, subsequent execution units may read data that has not been fully written, or execute early before preceding tasks complete, leading to incorrect computation results or unexpected execution behavior.

AI Core synchronization is divided into two categories: [intra-core synchronization](#intra-core-synchronization-principles) and [inter-core synchronization](#inter-core-synchronization-principles).

The following content shows the usage scenarios of the two types of synchronization.

- Intra-core synchronization is used to constrain the execution order of different hardware pipelines within the same core or instructions within the same hardware pipeline.

    For example, in the AI Vector, after the MTE2 unit moves data into on-chip storage through `PIPE_MTE2`, the Vector unit can read the data through `PIPE_V` for computation. In this case, synchronization from `PIPE_MTE2` to `PIPE_V` needs to be inserted so that `PIPE_V` waits for the preceding move-in by `PIPE_MTE2` to complete.
- Inter-core synchronization is used to constrain the execution order between different cores. The objects participating in synchronization can be multiple AI Cores, or the AI Cube and AI Vector within the same AI Core.
    <!-- npu="950" id1 -->
    As shown in Figure 1, for [NPU architecture version 3510](../../../../guide/programming_guide/language_extension/simd_builtin_keywords.md#npu-arch), after the AI Vector in the same AI Core moves data from the Unified Buffer to the L1 Buffer through `PIPE_MTE3`, the AI Cube side can move the data from the L1 Buffer into the L0A Buffer and L0B Buffer through `PIPE_MTE1` and continue Cube computation. In this case, the AI Cube needs to wait for the notification that the AI Vector has completed the data movement.

    The difference between intra-core synchronization and inter-core synchronization is that intra-core synchronization resolves data dependencies between hardware pipelines within the same core, focusing on the order of pipelines such as `PIPE_MTE2` and `PIPE_V`; inter-core synchronization resolves progress dependencies between different cores or different logical cores, focusing on the order between the AI Cube, AI Vector, or multiple AI Cores.

    **Figure 1**  [NPU architecture version 3510](../../../../guide/programming_guide/language_extension/simd_builtin_keywords.md#npu-arch) architecture diagram

    ![](../../../figures/npu_3510_hw_arch_sync.png)
    <!-- end id1 -->

### Intra-Core Synchronization Principles

#### Why Intra-Core Synchronization Is Needed

Execution units inside the AI Core (such as the MTE2 DMA unit and the Vector computation unit) run asynchronously in parallel and may have data dependencies when reading and writing the same storage resource. To ensure data consistency and computation correctness, synchronization control is required to coordinate operation timing.

<!-- npu="950" id2 -->
For [NPU architecture version 3510](../../../../guide/programming_guide/language_extension/simd_builtin_keywords.md#npu-arch), the hardware architecture is shown in Figure 1. The highlighted part shows the compute unit and DMA unit that execute in parallel.

<!-- end id2 -->

<!-- npu="A3,910b" id3 -->

For [NPU architecture version 2201](../../../../guide/programming_guide/language_extension/simd_builtin_keywords.md#npu-arch), the hardware architecture diagram is as follows. The highlighted part shows the compute unit and DMA unit that execute in parallel.

**Figure 2**  NPU architecture version 2201 architecture diagram

![](../../../figures/atlas_a2_a3_architecture.png)

<!-- end id3 -->

The following example describes a common Vector computation data flow:

1. First, the DMA execution unit moves data from GM to Local Memory.
2. Perform computation.
3. Then, the DMA execution unit moves the computation result from Local Memory to GM.

**Figure 3** Vector computation data flow diagram

![](../../../figures/vector_compute_data_flow.png)

The four execution units Scalar, Vector, DMA (MTE2), and DMA (MTE3) execute in parallel. If they access the same Local Memory, a synchronization mechanism is required to control their access timing: ensure that data is moved into Local Memory before computation, and moved out after computation is complete.

**Figure 4** Intra-core parallel pipeline execution timing diagram

![](../../../figures/intra_core_parallel_pipeline_execution_timing.png)

#### Hardware Pipeline Types

The parallel instruction pipeline types inside the AI Core and their descriptions are as follows:

> [!NOTE]
> For different hardware architectures, the specific pipelines included in each hardware pipeline type may differ. For details, see [Hardware Implementation](../../../../guide/programming_guide/advanced_programming/hardware_implementation/hardware_implementation.md).

**Table 1** Instruction pipeline types and related descriptions

| Pipeline Type | Meaning |
| --- | --- |
| `PIPE_S` | Scalar pipeline, used when scalar memory access statements or scalar computation APIs access global memory or on-chip storage addresses |
| `PIPE_V` | Vector computation pipeline and, on some hardware architectures, the L0C Buffer->Unified Buffer data movement pipeline |
| `PIPE_M` | Cube computation pipeline |
| `PIPE_MTE1` | L1 Buffer->L0A Buffer and L1 Buffer->L0B Buffer data movement pipeline |
| `PIPE_MTE2` | GM->L1 Buffer and GM->Unified Buffer data movement pipeline |
| `PIPE_MTE3` | Unified Buffer->GM data movement pipeline |
| `PIPE_FIX` | Data movement pipelines such as L0C Buffer->GM and L0C Buffer->L1 Buffer |

In addition, `PIPE_ALL` indicates all pipelines and is used in scenarios where it is necessary to wait for all preceding instructions of all pipelines.

The values of the `pipe_t` enum type in the synchronization APIs are as follows: <a id="hardware-pipeline-type"></a>

```c
typedef enum {
    PIPE_S = 0,
    PIPE_V = 1,
    PIPE_M = 2,
    PIPE_MTE1 = 3,
    PIPE_MTE2 = 4,
    PIPE_MTE3 = 5,
    PIPE_ALL = 6,
    PIPE_FIX = 10
} pipe_t;
```

#### Intra-core Synchronization Combinations

<!-- npu="A3,910b" id4 -->
Taking [NPU architecture version 2201](../../../../guide/programming_guide/language_extension/simd_builtin_keywords.md#npu-arch) as an example, all legal intra-core synchronization combinations under this hardware architecture are shown in [Table 2](#aic_intra_core_sync_combinations) and [Table 3](#aiv_intra_core_sync_combinations). "Not applicable" indicates that this synchronization combination does not exist at the hardware level, and "No application scenario" indicates that this synchronization combination exists but is not required in actual development scenarios.

<a id="aic_intra_core_sync_combinations"></a>

**Table 2** All legal intra-core synchronization combinations in AIC

| Source pipeline/Destination pipeline | `PIPE_S` | `PIPE_M` | `PIPE_MTE1` | `PIPE_MTE2` | `PIPE_MTE3` | `PIPE_FIX` |
| --- | --- | --- | --- | --- | --- | --- |
| `PIPE_S` | Not applicable | Not applicable | Not applicable | Not applicable | Not applicable | Not applicable |
| `PIPE_M` | Not applicable | `asc_sync_pipe(PIPE_M)` | `asc_sync_notify(PIPE_M, PIPE_MTE1, EVENT_ID0)`<br>`asc_sync_wait(PIPE_M, PIPE_MTE1, EVENT_ID0)` | `asc_sync_notify(PIPE_M, PIPE_MTE2, EVENT_ID0)`<br>`asc_sync_wait(PIPE_M, PIPE_MTE2, EVENT_ID0)` | Not applicable | `asc_sync_notify(PIPE_M, PIPE_FIX, EVENT_ID0)`<br>`asc_sync_wait(PIPE_M, PIPE_FIX, EVENT_ID0)` |
| `PIPE_MTE1` | Not applicable | `asc_sync_notify(PIPE_MTE1, PIPE_M, EVENT_ID0)`<br>`asc_sync_wait(PIPE_MTE1, PIPE_M, EVENT_ID0)` | `asc_sync_pipe(PIPE_MTE1)` | `asc_sync_notify(PIPE_MTE1, PIPE_MTE2, EVENT_ID0)`<br>`asc_sync_wait(PIPE_MTE1, PIPE_MTE2, EVENT_ID0)` | `asc_sync_notify(PIPE_MTE1, PIPE_MTE3, EVENT_ID0)`<br>`asc_sync_wait(PIPE_MTE1, PIPE_MTE3, EVENT_ID0)` | `asc_sync_notify(PIPE_MTE1, PIPE_FIX, EVENT_ID0)`<br>`asc_sync_wait(PIPE_MTE1, PIPE_FIX, EVENT_ID0)` |
| `PIPE_MTE2` | Not applicable | `asc_sync_notify(PIPE_MTE2, PIPE_M, EVENT_ID0)`<br>`asc_sync_wait(PIPE_MTE2, PIPE_M, EVENT_ID0)` | `asc_sync_notify(PIPE_MTE2, PIPE_MTE1, EVENT_ID0)`<br>`asc_sync_wait(PIPE_MTE2, PIPE_MTE1, EVENT_ID0)` | `asc_sync_pipe(PIPE_MTE2)` | `asc_sync_notify(PIPE_MTE2, PIPE_MTE3, EVENT_ID0)`<br>`asc_sync_wait(PIPE_MTE2, PIPE_MTE3, EVENT_ID0)` | No application scenario |
| `PIPE_MTE3` | Not applicable | Not applicable | `asc_sync_notify(PIPE_MTE3, PIPE_MTE1, EVENT_ID0)`<br>`asc_sync_wait(PIPE_MTE3, PIPE_MTE1, EVENT_ID0)` | `asc_sync_notify(PIPE_MTE3, PIPE_MTE2, EVENT_ID0)`<br>`asc_sync_wait(PIPE_MTE3, PIPE_MTE2, EVENT_ID0)` | `asc_sync_pipe(PIPE_MTE3)` | No application scenario |
| `PIPE_FIX` | Not applicable | `asc_sync_notify(PIPE_FIX, PIPE_M, EVENT_ID0)`<br>`asc_sync_wait(PIPE_FIX, PIPE_M, EVENT_ID0)` | `asc_sync_notify(PIPE_FIX, PIPE_MTE1, EVENT_ID0)`<br>`asc_sync_wait(PIPE_FIX, PIPE_MTE1, EVENT_ID0)` | No application scenario | No application scenario | `asc_sync_pipe(PIPE_FIX)` |

<a id="aiv_intra_core_sync_combinations"></a>

**Table 3** All legal intra-core synchronization combinations in AIV

| Source pipeline/Destination pipeline | `PIPE_S` | `PIPE_V` | `PIPE_MTE2` | `PIPE_MTE3` |
| --- | --- | --- | --- | --- |
| `PIPE_S` | Not applicable | `asc_sync_notify(PIPE_S, PIPE_V, EVENT_ID0)`<br>`asc_sync_wait(PIPE_S, PIPE_V, EVENT_ID0)` | `asc_sync_notify(PIPE_S, PIPE_MTE2, EVENT_ID0)`<br>`asc_sync_wait(PIPE_S, PIPE_MTE2, EVENT_ID0)` | `asc_sync_notify(PIPE_S, PIPE_MTE3, EVENT_ID0)`<br>`asc_sync_wait(PIPE_S, PIPE_MTE3, EVENT_ID0)` |
| `PIPE_V` | `asc_sync_notify(PIPE_V, PIPE_S, EVENT_ID0)`<br>`asc_sync_wait(PIPE_V, PIPE_S, EVENT_ID0)` | `asc_sync_pipe(PIPE_V)` | `asc_sync_notify(PIPE_V, PIPE_MTE2, EVENT_ID0)`<br>`asc_sync_wait(PIPE_V, PIPE_MTE2, EVENT_ID0)` | `asc_sync_notify(PIPE_V, PIPE_MTE3, EVENT_ID0)`<br>`asc_sync_wait(PIPE_V, PIPE_MTE3, EVENT_ID0)` |
| `PIPE_MTE2` | `asc_sync_notify(PIPE_MTE2, PIPE_S, EVENT_ID0)`<br>`asc_sync_wait(PIPE_MTE2, PIPE_S, EVENT_ID0)` | `asc_sync_notify(PIPE_MTE2, PIPE_V, EVENT_ID0)`<br>`asc_sync_wait(PIPE_MTE2, PIPE_V, EVENT_ID0)` | `asc_sync_pipe(PIPE_MTE2)` | `asc_sync_notify(PIPE_MTE2, PIPE_MTE3, EVENT_ID0)`<br>`asc_sync_wait(PIPE_MTE2, PIPE_MTE3, EVENT_ID0)` |
| `PIPE_MTE3` | `asc_sync_notify(PIPE_MTE3, PIPE_S, EVENT_ID0)`<br>`asc_sync_wait(PIPE_MTE3, PIPE_S, EVENT_ID0)` | `asc_sync_notify(PIPE_MTE3, PIPE_V, EVENT_ID0)`<br>`asc_sync_wait(PIPE_MTE3, PIPE_V, EVENT_ID0)` | `asc_sync_notify(PIPE_MTE3, PIPE_MTE2, EVENT_ID0)`<br>`asc_sync_wait(PIPE_MTE3, PIPE_MTE2, EVENT_ID0)` | `asc_sync_pipe(PIPE_MTE3)` |

<!-- end id4 -->

#### When Developers Need to Manually Insert Synchronization

In C API programming, all synchronization must be managed manually by developers.

### Inter-core Synchronization Principles

Inter-core synchronization is typically used when a core can obtain the data required for its computation only after all other related cores have completed their computations.

Taking Figure 5 as an example, the AI Cube needs to depend on the `asc_reduce` computation result of the AI Vector. Since the overall vector is large, it must be split into multiple parts, with each AI Vector completing part of the computation. Each AI Vector writes its partial computation result to global memory through atomic accumulation. The AI Cube must read the final result after all AI Vectors have completed accumulation, so inter-core synchronization is required to ensure the timing correctness when the AI Cube reads the `asc_reduce` result.

**Figure 5**  Example of an inter-core synchronization service scenario (AI Cube to AI Vector ratio of 1:2)

![](../../../figures/inter_core_sync_scenario_example.png)

The AI Cube/AI Vector multi-core structure is shown in Figure 6. AI Cube/AI Vector are divided by group. A group contains 1 block and N subblocks (N≥1), where block indicates the "primary core" and each subblock indicates a "secondary core".

Operators can be divided into three types by computation characteristics: Cube operators (Cube computation), Vector operators (vector computation), and Mix operators (containing both Cube and vector computation). As shown in Table 4, the operator type determines the required inter-core synchronization method and optional group configuration mode. For Mix operators, the block/subblock relationship between AI Cube and AI Vector is specified through function execution space qualifiers. Cube operators and Vector operators are executed by a single type of core and do not involve block/subblock division. The configuration where AI Vector is the block and AI Cube is the subblock (that is, Vector Core as primary and Cube Core as secondary) is currently not supported. Table 6 summarizes the inter-core synchronization APIs and their supported synchronization scenarios.

**Figure 6**  Relationship between block and subblock (the gray part indicates a group, that is, 1 block and N subblocks)<a id="fig_block_subblock_relationship"></a>

![](../../../figures/block_subblock_relationship_3510.png)

**Table 4** group configuration

| Operator Type | Function Execution Space Qualifier | Kernel Type | block/subblock | 1: N |
| --- | --- | --- | --- | --- |
| Cube operator | `__cube__` | `KERNEL_TYPE_AIC_ONLY` | AIC is a block, with no subblock. | Not involved. |
| Vector operator | `__vector__` | `KERNEL_TYPE_AIV_ONLY` | AIV is a block, with no subblock. | Not involved. |
| Mix operator | `__mix__(1, 0)` | `KERNEL_TYPE_MIX_AIC_1_0` | AIC is a block, with no subblock. | Not involved. |
| Mix operator | `__mix__(0, 1)` | `KERNEL_TYPE_MIX_AIV_1_0` | AIV is a block, with no subblock. | Not involved. |
| Mix operator | `__mix__(1, 1)` | `KERNEL_TYPE_MIX_AIC_1_1` | AIC is a block, and AIV is a subblock. | 1: 1 |
| Mix operator | `__mix__(1, 2)` | `KERNEL_TYPE_MIX_AIC_1_2` | AIC is a block, and AIV is a subblock. | 1: 2 |

#### Inter-core Synchronization Control Modes

Figure 7 shows four synchronization control modes, and the function description of each mode is as follows. This figure is based on the scenario where the kernel function is modified with `__mix__(1, 2)` and the logical core count `numBlocks=4`.

- Mode 0: Synchronization control between AI Core cores. For the all-AIC core scenario, all AIC cores are synchronized, and instructions after `asc_sync_inter_wait` execute only after all AIC cores have executed `asc_sync_inter_arrive`. For the all-AIV core scenario, all AIV cores are synchronized, and instructions after `asc_sync_inter_wait` execute only after all AIV cores have executed `asc_sync_inter_arrive`.
- Mode 1: Synchronization control between AIV cores within an AI Core. Instructions after `asc_sync_subblock_wait` execute only after both AIV cores have executed `asc_sync_subblock_arrive`.
- Mode 2: Synchronization control between AIC and AIV within an AI Core. Only after the AIC core executes `asc_sync_block_arrive` can instructions after `asc_sync_block_wait` on the two AIV cores continue to execute. Only after both AIV cores execute `asc_sync_block_arrive` can instructions after `asc_sync_block_wait` on the AIC core execute.
- Mode 4: Synchronization control between AIC and a single AIV within an AI Core. Only after a single AIV core executes `asc_sync_intra_arrive` can instructions after `asc_sync_intra_wait` on the AIC core continue to execute. Only after the AIC core executes `asc_sync_intra_arrive` can instructions after `asc_sync_intra_wait` on the single AIV core execute. AIV0 and AIV1 can independently trigger the AIC wait.

**Figure 7**  Schematic diagram of four inter-core synchronization control modes<a id="fig_sync_control_mode"></a>

![](../../../figures/3510_sync_control_mode_diagram.png)

## API List

### Intra-Core Synchronization

The classification, applicable scenarios, and differences of intra-core synchronization APIs are shown in Table 5.

**Table 5** Intra-core synchronization APIs

| API Name | API Description | Finest-Granularity Common Header File |
| --- | --- | --- |
| [asc_sync_notify](intra_core_sync/asc_sync_notify.md)/[asc_sync_wait](intra_core_sync/asc_sync_wait.md) | **Multi-pipeline synchronization**: Synchronization between different types of pipeline instructions with data dependencies within the same core.<br>Developers need to explicitly specify a pair of source pipeline and destination pipeline. `asc_sync_notify` sets a flag after all read and write operations of preceding instructions in the source pipeline are complete; `asc_sync_wait` blocks the destination pipeline until the flag is detected, then unblocks and clears the flag. | `c_api/sync/sync.h` |
| [asc_lock](intra_core_sync/asc_lock.md)/[asc_unlock](intra_core_sync/asc_unlock.md) | **Multi-pipeline synchronization**: Synchronization control between different pipelines within the same core.<br>Locks and releases the specified pipeline through `mutex_id`. `asc_lock` acquires the Mutex based on `mutex_id`; if the Mutex is already locked, it blocks the execution of subsequent instructions on the specified pipeline; `asc_unlock` releases the corresponding Mutex after the preceding instructions of the specified pipeline are complete. Compared with `asc_sync_notify`/`asc_sync_wait`, this combination has stronger cohesion and is decoupled from other pipelines, which can simplify reverse synchronization logic. | `c_api/sync/sync.h` |
| [asc_sync_mte2](intra_core_sync/asc_sync_mte2.md), [asc_sync_mte3](intra_core_sync/asc_sync_mte3.md), [asc_sync_vec](intra_core_sync/asc_sync_vec.md) | **Multi-pipeline synchronization**: Blocks multiple destination pipelines simultaneously until the preceding instructions of the specified source pipeline are complete.<br>**Ease-of-use APIs** that internally encapsulate multiple pairs of `asc_sync_notify` and `asc_sync_wait`.<br>`asc_sync_mte2` is equivalent to performing two groups of synchronization: `PIPE_MTE2`->`PIPE_MTE3` and `PIPE_MTE2`->`PIPE_V`.<br>`asc_sync_mte3` is equivalent to performing two groups of synchronization: `PIPE_MTE3`->`PIPE_MTE2` and `PIPE_MTE3`->`PIPE_V`.<br>`asc_sync_vec` is equivalent to performing three groups of synchronization: `PIPE_V`->`PIPE_MTE2`, `PIPE_V`->`PIPE_MTE3`, and `PIPE_V`->`PIPE_S`.<br>The above three APIs can only be called in AI Vector. | `c_api/sync/sync.h` |
| [asc_sync_pipe](intra_core_sync/asc_sync_pipe.md) | **Single-pipeline synchronization**: Synchronization between instructions of the same pipeline type with data dependencies within the same core.<br>Although instructions in the same pipeline execute sequentially, the previous instruction may not have finished executing when the next instruction starts. `asc_sync_pipe` ensures that all data reads and writes in preceding instructions are complete before subsequent instructions begin execution. This API does not support `PIPE_S` single-pipeline synchronization. | `c_api/sync/sync.h` |
| [asc_sync](intra_core_sync/asc_sync.md) | **All-pipeline synchronization**: Blocks subsequent instructions of all hardware pipelines after the call point until all preceding instructions of all hardware pipelines before the call point are complete.<br>Functionally equivalent to `asc_sync_pipe(PIPE_ALL)`, suitable for scenarios where a single pipeline cannot be constrained alone. | `c_api/sync/sync.h` |
| [asc_sync_data_barrier](intra_core_sync/asc_sync_data_barrier.md) | **Memory access barrier**: Waits until all previously issued but not yet completed memory access instructions have finished executing.<br>Specifies the memory range affected by the barrier through the `arg` parameter, ensuring that memory access instructions before and after the barrier complete in the expected order. | `c_api/sync/sync.h` |

### Inter-Core Synchronization

Inter-core synchronization APIs and their supported synchronization scenarios are shown in Table 6.

**Table 6** Inter-core synchronization APIs

| API Name | API Description | Finest-Granularity Common Header File |
| --- | --- | --- |
| [asc_sync_inter_arrive](inter_core_sync/asc_sync_inter_arrive.md)/[asc_sync_inter_wait](inter_core_sync/asc_sync_inter_wait.md) | **Synchronization scope**: Inter-group synchronization, that is, synchronization between all blocks (all AI Cubes or all AI Vectors) in different groups.<br>**Corresponding mode**: Mode 0 (synchronously waits for all AI Cubes or all AI Vectors to finish execution)<br>`asc_sync_inter_arrive` is the producer, responsible for sending notifications; `asc_sync_inter_wait` is the consumer, responsible for blocking and waiting for the notification to arrive. | `c_api/sync/sync.h` |
| [asc_sync_subblock_arrive](inter_core_sync/asc_sync_subblock_arrive.md)/[asc_sync_subblock_wait](inter_core_sync/asc_sync_subblock_wait.md) | **Synchronization scope**: Synchronization between subblocks within a group, that is, synchronization between different subblocks (AI Vectors) in the same group.<br>**Corresponding mode**: Mode 1 (synchronization between all AI Vectors within a single AI Core)<br>`asc_sync_subblock_arrive` is the producer, responsible for sending notifications; `asc_sync_subblock_wait` is the consumer, responsible for blocking and waiting for the notification to arrive. | `c_api/sync/sync.h` |
| [asc_sync_block_arrive](inter_core_sync/asc_sync_block_arrive.md)/[asc_sync_block_wait](inter_core_sync/asc_sync_block_wait.md) | **Synchronization scope**: Synchronization between the block (AI Cube) and all subblocks (AI Vectors) within the same group.<br>**Corresponding mode**: Mode 2 (synchronization between the AI Cube and all AI Vectors within a single AI Core)<br>`asc_sync_block_arrive` is the producer, responsible for sending notifications; `asc_sync_block_wait` is the consumer, responsible for blocking and waiting for the notification to arrive. | `c_api/sync/sync.h` |
| [asc_sync_intra_arrive](inter_core_sync/asc_sync_intra_arrive.md)/[asc_sync_intra_wait](inter_core_sync/asc_sync_intra_wait.md) | **Synchronization scope**: Synchronization between the block (AI Cube) and a single subblock (AI Vector) within the same group.<br>**Corresponding mode**: Mode 4 (synchronization between the AI Cube core and a single AI Vector within a single AI Core)<br>`asc_sync_intra_arrive` is the producer, responsible for sending notifications; `asc_sync_intra_wait` is the consumer, responsible for blocking and waiting for the notification to arrive. | `c_api/sync/sync.h` |
