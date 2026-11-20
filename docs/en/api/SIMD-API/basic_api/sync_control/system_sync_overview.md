# System Synchronization Overview<a name="ZH-CN_TOPIC_0000002586323695"></a>

<!-- md-trans-meta sourceCommit=c14730e9a76fabd38b673e588b8c2e8f65f06fc8 translatedAt=2026-11-05T05:29:33.757Z pushedAt=2026-11-18T01:25:31.181Z nl=2 -->

In Ascend C programming, synchronization is used to constrain the execution order of different execution units inside and outside the AI Core, ensuring that subsequent instructions continue only after the data, state, or synchronization signals they depend on are ready. Because data transfer, computation, different kernel functions (Kernel), or different AI Cores can run in parallel, without the necessary synchronization, a subsequent execution unit may read data that has not been fully written, or execute ahead of time before a preceding task completes, resulting in incorrect computation results or unexpected execution behavior.

AI Core synchronization is divided into three categories: intra-core synchronization, inter-core synchronization, and inter-task synchronization.

The following describes the usage scenarios of the three synchronization categories.

- Intra-core synchronization is used to constrain the execution order of instructions across different hardware pipelines or within the same hardware pipeline on the same core.

    For example, in the AIV, after the MTE2 unit moves data into on-chip storage through `PIPE_MTE2`, the Vector unit can read the data through `PIPE_V` for computation. In this case, synchronization from `PIPE_MTE2` to `PIPE_V` must be inserted so that `PIPE_V` waits for the preceding data transfer of `PIPE_MTE2` to complete.
- Inter-core synchronization is used to constrain the execution progress between different cores. The objects involved in synchronization can be multiple AI Cores, or the AIC and AIV within the same AI Core.
    <!-- npu="950" id1 -->
    As shown in Figure 1, for the [NPU architecture 3510](../../../../guide/programming_guide/language_extension/simd_builtin_keywords.md#npu-arch), after the AIV within the same AI Core moves data from the UB to the L1 Buffer through `PIPE_MTE3`, the AIC-side `PIPE_MTE1` can move the data from the L1 Buffer into the L0A Buffer and L0B Buffer and continue matrix computation. In this case, the AIC must wait for the AIV's data transfer completion notification.

    The difference between intra-core synchronization and inter-core synchronization is as follows: intra-core synchronization resolves data dependencies between hardware pipelines within the same core, focusing on the order of pipelines such as `PIPE_MTE2` and `PIPE_V`; inter-core synchronization resolves progress dependencies between different cores or different logical cores, focusing on the order between the AIC, AIV, or multiple AI Cores.

    **Figure 1**  Architecture diagram of [NPU architecture 3510](../../../../guide/programming_guide/language_extension/simd_builtin_keywords.md#npu-arch)
    ![](../../../figures/npu_3510_hw_arch_sync.png)
    <!-- end id1 -->

- Inter-task synchronization is used for scheduling and parallel control between multiple sub-kernel functions (Kernel) in the SuperKernel scenario. SuperKernel fuses multiple sub-kernel functions (Kernel) into a single super kernel function (Kernel) for execution. Through inter-task synchronization APIs, some instructions of adjacent sub-kernel functions (Kernel) can run in parallel, reducing the Task scheduling wait time. For more principles and adaptation instructions, see [SuperKernel](../../../../guide/programming_guide/advanced_programming/super_kernel/principles.md).

The APIs corresponding to each synchronization category are as follows.

**Table 1**  Intra-core synchronization APIs

| API name | Function Description | Synchronization Category |
| --- | --- | --- |
| [SetFlag/WaitFlag\(ISASI\)](intra_core_sync/SetFlag_WaitFlag_ISASI.md) | Multi-pipeline synchronization: synchronization instructions between different pipelines within the same core. SetFlag sends the notification, and WaitFlag blocks until SetFlag sends the notification. | Hardware Synchronization |
| [PipeBarrier](intra_core_sync/PipeBarrier_ISASI.md) | Single-pipeline synchronization: synchronization instructions between the same pipeline within the same core (PIPE_S single-pipeline synchronization is not supported). | Hardware Synchronization |
| [DataSyncBarrier](intra_core_sync/DataSyncBarrier_ISASI.md) | Blocks the execution of subsequent instructions until all previous memory access instructions (the memory locations to wait for can be controlled through parameters) finish executing. | Hardware Synchronization |
| [Lock/Unlock](intra_core_sync/Mutex_ISASI.md) | Used for synchronization between asynchronous pipeline instructions within a core. Its function is similar to the lock mechanism in a traditional CPU. It locks a specified pipeline and then releases the pipeline to complete the synchronization dependency between pipelines. | Hardware Synchronization |

**Table 2**  Inter-core synchronization APIs

| API name | Function Description | Synchronization Category |
| --- | --- | --- |
| [CrossCoreSetFlag](inter_core_sync/CrossCoreSetFlag_ISASI.md)/[CrossCoreWaitFlag](inter_core_sync/CrossCoreWaitFlag_ISASI.md) |Synchronization control between multiple AICs or AIVs. CrossCoreSetFlag sends the notification, and CrossCoreWaitFlag blocks until CrossCoreSetFlag sends the notification.<br>Supports the following synchronization scenarios:<br>&bull;Synchronously wait for all AICs or all AIVs to finish executing.<br>&bull;Synchronization between all AIVs within a single AI Core.<br>&bull;Synchronization between the AIC and all AIVs within a single AI Core. | Hardware Synchronization |
| [IBSet](inter_core_sync/IBSet.md)/[IBWait](inter_core_sync/IBWait.md) | IBSet is called in the waited core to notify the waiting core by writing the value 1 to a certain address block in global memory; IBWait is called in the waiting core to continuously read the value of the corresponding address block in global memory until its value becomes 1. | Software Synchronization |
| [SyncAll](inter_core_sync/SyncAll.md) | Used for synchronization control between multiple cores. Supports the following synchronization scenarios:<br>&bull;Full-core synchronization of pure Vector operators. For details, see [Mode 0 of CrossCoreSetFlag](inter_core_sync/key_features.md).<br>&bull;Full-core synchronization of fused operators, which implements three inter-core synchronization capabilities at once (see [Key Feature Description of CrossCoreSetFlag](inter_core_sync/key_features.md)): first, complete the synchronization between the AIC and all AIVs within a single AI Core (AIC waits for AIVs); then complete the synchronization of all AICs between multiple AI Cores; finally, complete the synchronization between all AIVs and the AIC within a single AI Core (AIVs wait for the AIC). | Hardware Synchronization |
| [SyncAll](inter_core_sync/SyncAll.md) | Supports only multi-core synchronization of pure Vector operators (the number of cores participating in synchronization can be specified through an input parameter). Each AIV writes the signal value 1 to the corresponding location in global memory, and then each AIV continuously polls the signal values corresponding to all AIVs in global memory until the signal values corresponding to all AIV cores become 1. | Software Synchronization |
| [InitDetermineComputeWorkspace](inter_core_sync/InitDetermineComputeWorkspace.md)/[NotifyNextBlock](inter_core_sync/NotifyNextBlock.md)/[WaitPreBlock](inter_core_sync/WaitPreBlock.md) | &bull; InitDetermineComputeWorkspace, NotifyNextBlock, and WaitPreBlock must be used in combination. They ensure that multiple AIV cores execute strictly in ascending order of blockIdx, which is suitable for scenarios requiring deterministic computation.<br>&bull; InitDetermineComputeWorkspace initializes the value of the GM shared memory. Only after initialization is complete can NotifyNextBlock and WaitPreBlock be called.<br>&bull; NotifyNextBlock notifies other cores by writing to global memory that the current core has finished executing and that other cores can continue; WaitPreBlock determines whether the current core can continue by reading global memory. | Software Synchronization |

**Table 3**  Inter-task synchronization APIs

| API name | Function Description | Synchronization Category |
| --- | --- | --- |
| [SetNextTaskStart](./inter_task_sync/SetNextTaskStart.md) | Called in the sub-core function (Kernel) of SuperKernel. The instructions after the call can run in parallel with subsequent other sub-core functions (Kernel), improving overall performance. | Hardware Synchronization |
| [WaitPreTaskEnd](./inter_task_sync/WaitPreTaskEnd.md) | Called in the sub-core function (Kernel) of SuperKernel. The instructions before the call can run in parallel with preceding other sub-core functions (Kernel), improving overall performance. | Hardware Synchronization |
