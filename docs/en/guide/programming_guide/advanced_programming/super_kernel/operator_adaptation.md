# Operator Adaptation Description

<!-- md-trans-meta sourceCommit=f83e857605e3844db8b9de0442007cc115278a47 translatedAt=2026-10-23T14:43:20.939Z pushedAt=2026-11-03T07:05:13.436Z nl=2 -->

To support fusion into SuperKernel, a custom operator does not differ significantly from a regular operator in terms of the development process, but it must comply with the following series of **specific constraints**. These constraints are independent of the specific enabling method (the npugraph_ex backend or GE graph mode). Most of the constraints are related to the relationship between the number of startup cores (block num) of the operator and the number of startup cores of SuperKernel, and developers need to pay attention to them at the operator design stage.

> [!NOTE] Note
> The number of startup cores of SuperKernel is the maximum number of startup cores among all its sub-kernel functions (Kernels). For example, if SuperKernel contains operator a (with a number of startup cores of 4) and operator b (with a number of startup cores of 2), the number of startup cores of SuperKernel is 4.

## All-Core Synchronization Constraints

If a custom operator performs all-core synchronization, note that the number of startup cores of the sub-kernel function must be the same as that of the SuperKernel, which usually requires starting all cores of the current hardware. If the number of startup cores of the sub-kernel function is smaller than that of the SuperKernel, all-core synchronization waits for all cores to complete, causing the process to hang and time out.

## MIX 1:1 Operator Core Ratio Constraint

If the kernel function type of a custom operator is set to `KERNEL_TYPE_MIX_AIC_1_1`, because SuperKernel adjusts its startup ratio based on information such as the number of startup cores, pay special attention to ensure that the operator can also adapt to SuperKernel's 1:2 startup ratio, so that the hard synchronization operations between AIC and AIV are executed correctly. For example:

- If the operator internally uses the hard synchronization APIs between AIC and AIV (**CrossCoreSetFlag** and **CrossCoreWaitFlag**), do not specify only certain AIV cores to call the hard synchronization APIs. Instead, ensure that all AIV cores call the hard synchronization APIs to prevent deadlock and timeout caused by a mismatch in the number of hard synchronizations.
- When using the advanced Matmul API, the operator logic should ensure that only one AIV0 core calls the Matmul API, to prevent the AIV1 core from being unable to receive messages after two AIV cores are started, which would cause deadlock and timeout.

## Scalar Read/Write and Cache Coherence

When developing custom operators, developers must ensure that all scalar read/write operations on GM correctly insert the [DataCacheCleanAndInvalid](../../../../api/SIMD-API/basic_api/cache_control/DataCacheCleanAndInvalid.md) instruction as required.

- In the single-operator compilation scenario, the BiSheng compiler automatically adds the **DataCacheCleanAndInvalid** instruction at the end of the operator to flush the entire DCache (data cache).
- In SuperKernel, sub-kernel functions (Kernels) are treated as regular functions, and the compiler does not automatically insert this instruction to ensure data cache coherency. Developers need to guarantee it themselves to avoid errors caused by changes in the fault tolerance mechanism.

For performance considerations, the cache flush mechanism in the SuperKernel scenario is as follows:

- If developers call the [`GetValue`](../../../../api/SIMD-API/basic_api/data_structures/GlobalTensor/GetValue.md) and [`SetValue`](../../../../api/SIMD-API/basic_api/data_structures/GlobalTensor/SetValue.md) APIs of GlobalTensor to perform scalar read/write on GM, in the GE graph insertion and npugraph_ex backend aclnn call scenarios, the SuperKernel compilation automatically inserts the DataCacheCleanAndInvalid instruction inside these two APIs to flush a single cache line, ensuring a certain level of data cache coherency. DataCacheCleanAndInvalid is not inserted before or after the sub-kernel function call.
- If developers use the `()` operator API of GlobalTensor to obtain values (a **DataCacheCleanAndInvalid** instruction is inserted during reads to flush a single cache line and ensure data cache coherency), but directly rewrite the value at the corresponding position of GlobalTensor through this API, the **DataCacheCleanAndInvalid** instruction is not automatically inserted, and developers need to ensure data cache coherency themselves. For example:

    ```c++
    AscendC::GlobalTensor<float> xGm;
    xGm.SetGlobalBuffer((__gm__ float *)(addr), length); // addr is the GM address, and length is the corresponding GM length
    xGm(0) = (float)(1.0); // When obtaining a value, SuperKernel can automatically flush the cache line. However, when writing a value, it is equivalent to directly assigning a value to a regular variable, and SuperKernel cannot insert DataCacheCleanAndInvalid. Therefore, you need to ensure data cache coherency.
    ```

For operators that frequently call `GetValue` and `SetValue`, it is recommended to use `dcci_before_kernel_start` and `dcci_after_kernel_end` to disable the cache flush instructions automatically inserted in `GetValue`/`SetValue` within the specified operator, and instead insert a full DCache flush before and after the operator call to avoid performance degradation. For details about the options, see the option description sections in the respective PyTorch graph mode SuperKernel documents: for GE graph insertion, see [SuperKernel Scope Calibration in Graph](https://www.hiascend.com/document/detail/en/Pytorch/2620/devguide/TorchAir/docs/en/ascend_ir/features/advanced/super_kernel_scope.md); for the npugraph_ex backend, see the `super_kernel_optimize_options` parameter description in [SuperKernel Function](https://www.hiascend.com/document/detail/en/Pytorch/2620/devguide/TorchAir/docs/en/npugraph_ex/advanced/superkernel.md).

The cache refresh mechanism schematic diagram is shown in the following figure:

![](../../../figures/sk_dcci.png)

## Constraints on the Core Count Query API

In the GE graph insertion and npugraph_ex backend aclnn call scenarios, when the [GetBlockNum](../../../../api/SIMD-API/basic_api/tool_interface/system_resources_and_variables/GetBlockNum.md) API is called in a sub-kernel function to obtain the number of cores, the obtained number of cores remains unchanged regardless of whether SuperKernel is fused, and is not affected by the number of startup cores of SuperKernel. Therefore, when using this API, developers do not need to pay special attention to the number of startup cores of SuperKernel, and the usage is the same as when developing regular operators.

In the SuperKernel scenario, the following Ascend C APIs are adapted to SuperKernel during operator compilation. Sub-operators must be programmed strictly following the APIs provided by Ascend C, so that they do not need to be aware of whether SuperKernel is enabled. For example, when obtaining the number of cores and the index, **do not call underlying variables and related APIs such as `block_idx` and `block_num`**. Instead, use the Ascend C APIs listed in the following table:

| API | Function Description |
| --- | --- |
| [AscendC::GetBlockIdx()](../../../../api/SIMD-API/basic_api/tool_interface/system_resources_and_variables/GetBlockIdx.md) | Obtains the index of the current core. |
| [AscendC::GetBlockNum()](../../../../api/SIMD-API/basic_api/tool_interface/system_resources_and_variables/GetBlockNum.md) | Obtains the number of cores configured for the current task. |

<!-- npu="A3,910b" id1 -->
## TPipe Destruction Constraints

For Atlas A2 products and Atlas A3 products, in the GE graph insertion and npugraph_ex backend aclnn call scenarios, the `AscendC::PipeBarrier<PIPE_ALL>()` instruction inside the sub-operator TPipe object destructor ([TPipe::Destroy](../../../../api/SIMD-API/basic_api/resource_management/TPipe/Destroy.md)) is removed. If the operator uses multiple TPipe objects internally or manually calls the Destroy function, the developer must ensure the synchronization of the pipeline between TPipe objects.

Example:

- **Scenario 1: synchronization upon TPipe object destruction**

    When multiple TPipe objects exist in an operator and destruction is triggered through scope (braces or operator completion), manually insert `PipeBarrier<PIPE_ALL>()` after the first TPipe is destroyed to ensure pipeline synchronization.

    ```c++
    // Scenario 1: Two TPipe objects are destructed through scope. Insert PipeAll after the first destructor.
    {
        AscendC::TPipe pipe1;
        AscendC::TQue<AscendC::TPosition::VECOUT, 2> que1;
        uint8_t num1 = 2;
        uint32_t len1 = 128;
        pipe1.InitBuffer(que1, num1, len1);
        // ... Use pipe1 for operator computation ...
    } // pipe1 is destructed, but PipeBarrier in Destroy has been removed

    // Manually insert PipeAll to ensure that pipe2 is used only after the pipe1 pipeline completes
    AscendC::PipeBarrier<AscendC::PIPE_ALL>();

    {
        AscendC::TPipe pipe2;
        AscendC::TQue<AscendC::TPosition::VECOUT, 2> que2;
        uint8_t num2 = 2;
        uint32_t len2 = 128;
        pipe2.InitBuffer(que2, num2, len2);
        // ... Use pipe2 for operator computation ...
    } // pipe2 is destructed
    ```

- **Scenario 2: synchronization when Destroy is manually called**

    When multiple TPipe objects exist in an operator and the `Destroy` API is manually called, insert `PipeBarrier<PIPE_ALL>()` manually after the `Destroy` call of the first TPipe to ensure pipeline synchronization.

    ```c++
    // Scenario 2: Two TPipe objects manually call Destroy. Insert PipeAll after the first Destroy.
    AscendC::TPipe pipe1;
    AscendC::TQue<AscendC::TPosition::VECOUT, 2> que1;
    uint8_t num1 = 2;
    uint32_t len1 = 128;
    pipe1.InitBuffer(que1, num1, len1);
    // ... Use pipe1 for operator computation ...
    pipe1.Destroy(); // PipeBarrier in Destroy has been removed

    // You must manually insert PipeAll to ensure that pipe1 completes its pipeline before pipe2 is used.
    AscendC::PipeBarrier<AscendC::PIPE_ALL>();

    AscendC::TPipe pipe2;
    AscendC::TQue<AscendC::TPosition::VECOUT, 2> que2;
    uint8_t num2 = 2;
    uint32_t len2 = 128;
    pipe2.InitBuffer(que2, num2, len2);
    // ... Use pipe2 for operator computation ...
    pipe2.Destroy();
    ```
<!-- end id1 -->


## Performance Optimization Suggestions

- **Inter-task synchronization**

    When programming on the kernel function side, developers can call the two inter-task APIs [SetNextTaskStart](../../../../api/SIMD-API/basic_api/sync_control/inter_task_sync/SetNextTaskStart.md) and [WaitPreTaskEnd](../../../../api/SIMD-API/basic_api/sync_control/inter_task_sync/WaitPreTaskEnd.md) to further improve performance.

    - Instructions after calling `SetNextTaskStart` can achieve parallelism with subsequent other sub-kernel functions, improving overall performance. As shown in [Figure 1](#fig37581010773), SuperKernel calls sub-kernel functions (Kernel) in sequence. To ensure that data between sub-kernel functions (Kernel) does not interfere with each other, inter-operator synchronization is inserted between sub-kernel functions (Kernel) for ordering. After sub-kernel function<sub>N-1</sub> calls this API, the subsequent instructions achieve parallelism with the following sub-kernel function<sub>N</sub>.

    **Figure 1**  Schematic diagram of achieving parallelism through SetNextTaskStart<a name="fig37581010773"></a>  
    ![](../../../figures/set_next.png "Schematic diagram of achieving parallelism through SetNextTaskStart")

    - Instructions before calling `WaitPreTaskEnd` can achieve parallelism with preceding other sub-kernel functions (Kernel), improving overall performance. As shown in [Figure 2](#fig99271836191110), SuperKernel calls sub-kernel functions (Kernel) in sequence. To ensure that data between sub-kernel functions (Kernel) does not interfere with each other, inter-operator synchronization is inserted between sub-kernel functions (Kernel) for ordering. The instructions before sub-kernel function<sub>N+1</sub> calls this API can achieve parallelism with the preceding sub-kernel function<sub>N</sub>.

    **Figure 2**  Schematic diagram of achieving parallelism through WaitPreTaskEnd<a name="fig99271836191110"></a>  
    ![](../../../figures/wait_pre.png "Schematic diagram of achieving parallelism through WaitPreTaskEnd")

- **Binary reuse optimization**

    In the GE graph insertion and Tiling sinking scenario, you can use the `--op_relocatable_kernel_binary` compilation option to enable binary reuse optimization and improve compilation performance. For details, see [Operator Project Compilation](../aclnn_operator_development/compilation_and_deployment/basic_process.md).

## Additional Adaptation for Operators Directly Called by Kernel Functions

For Ascend C operators developed using the `<<<>>>` method, fusion into SuperKernel is currently supported only on the npugraph_ex backend. In addition to the general constraints described above, such operators also need to add a SuperKernel entry function on the operator kernel entry side. For details, see [Additional Adaptation for Operators Directly Called by Kernel Functions](kernel_direct_call_adaptation.md).
