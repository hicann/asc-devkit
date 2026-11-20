# Key Features Description<a name="ZH-CN_TOPIC_0000002555621184"></a>

<!-- md-trans-meta sourceCommit=69e8932e723e077228981554026ee19b14b10fa7 translatedAt=2026-11-02T02:15:03.622Z pushedAt=2026-11-18T01:25:06.588Z nl=2 -->

Deterministic computation is a computation process that always produces exactly the same output result under the same input conditions, regardless of the number of executions or changes in the execution environment. Deterministic computation provides a guarantee for system stability and experimental verifiability.

## Deterministic Computation Overview<a name="ZH-CN_TOPIC_0000002555780816"></a>

To introduce the problem of non-deterministic computation in atomic operation scenarios, we construct the following common deterministic computation scenario: first, initialize GM through a single group of floating-point data movement; then, start the atomic accumulation operation; finally, accumulate multiple groups of floating-point data on GM through multiple data movements. The specific pseudocode is as follows:

```text
1. Move data0 to GM;    // Data movement, overwriting the original random value in GM. The expected GM data is data0.
2. SetAtomicAdd();         // Enable atomic accumulation. Subsequent moves from Unified Buffer (UB)/L0C Buffer/L1 Buffer to GM all perform atomic accumulation.
3. Move data1 to GM;    // Data movement with an in-line atomic operation. The expected GM data is data0 + data1.
4. Move data2 to GM;    // Data movement with an in-line atomic operation. The expected GM data is data0 + data1 + data2.
5. Move data3 to GM;    // Data movement with an in-line atomic operation. The expected GM data is data0 + data1 + data2 + data3.
```

As shown in Figure 1, the developer's expected result is that the instruction issue order strictly corresponds to the actual instruction execution order. No matter how many times this code segment is executed, the final GM data is always data0 + data1 + data2 + data3, with completely consistent results, achieving deterministic computation.

**Figure 1**  Deterministic computation scenario, GM data change process<a name="zh-cn_topic_0000002552941426_fig71821376818"></a>

![](../../../figures/deterministic_computation_gm_data_change_process.png "Deterministic computation scenario, GM data change process")

In practice, however, without developer intervention, the execution order of these instructions may change each time the program runs, ultimately causing the GM data to differ from the expected result. The following lists two possible instruction execution orders and their corresponding execution flows.

## Non-deterministic Computation, Result 1<a name="zh-cn_topic_0000002552941426_section027603463315"></a>

**Figure 2**  Non-deterministic computation scenario 1, GM data change process<a name="zh-cn_topic_0000002552941426_fig273814664819"></a>

![](../../../figures/non_deterministic_computation_1_gm_data_change_process.png "Non-deterministic computation scenario 1, GM data change process")

As shown in Figure 2, the instruction execution flow in this scenario is as follows:

1. In the initial state, the GM data is a random value.
2. Move data0 to GM, and the GM data is initialized to data0.
3. Execute SetAtomicAdd to enable atomic accumulation for subsequent move instructions. The GM data is data0.
4. The three move instructions with in-line atomic operations are executed out of order. The actual execution order is "move out data2 → move out data3 → move out data1", and the final GM data is data0 + data2 + data3 + data1.

**Cause 1 of non-deterministic computation:**

The move instructions with in-line atomic operations are executed out of order. Because floating-point addition does not satisfy the associative law, that is, \(a+b\)+c!=a+\(b+c\), the final GM data data0 + data2 + data3 + data1 deviates from the expected data0 + data1 + data2 + data3.

The following three preconditions cause the final result to deviate when move instructions with in-line atomic operations are executed out of order:

- The atomic operation type is atomic accumulation (maximum and minimum operations satisfy the associativity law).
- The atomic operation data type is floating-point (integer addition satisfies the associativity law).
- There are three or more move instructions with in-line atomic operations (floating-point addition satisfies the commutativity law but not the associativity law).

## Non-deterministic Computation, Result 2<a name="zh-cn_topic_0000002552941426_section1547411390818"></a>

**Figure 3**  Non-deterministic computation scenario 2, GM data change process<a name="zh-cn_topic_0000002552941426_fig1829915169912"></a>

![](../../../figures/non_deterministic_computation_2_gm_data_change_process.png "Non-deterministic computation scenario 2, GM data change process")

As shown in Figure 3, the instruction execution flow in this scenario is as follows:

1. In the initial state, the GM data is: random value;
2. Execute SetAtomicAdd to enable atomic accumulation for subsequent move instructions. The GM data is: random value;
3. Execute two move instructions with in-line atomic operations in sequence, in the order of "move out data1 → move out data2". The GM data is: random value + data1 + data2;
4. Move data0 to GM. The accumulated result on GM is overwritten by data0. The GM data is: data0;
5. Finally, move data3. The final GM data is: data0 + data3.

**Cause 2 of non-deterministic computation:**

Out-of-order execution between a normal move instruction before atomic accumulation is enabled and a move instruction with atomic operations enabled causes data that has already undergone atomic operations on the GM to be incorrectly overwritten by data0, resulting in a non-deterministic computation result.

This type of out-of-order execution that causes result deviation does not require any precondition. You no longer need to distinguish between atomic operation types or atomic operation data types, nor do you need to consider whether the number of move instructions with in-line atomic operations reaches three or more.

## Deterministic Computation Implementation Solution<a name="ZH-CN_TOPIC_0000002586300751"></a>

Based on the two root causes of non-deterministic computation, the following describes the solution for implementing deterministic computation from these two aspects. The core idea is to insert appropriate synchronization between instructions so that the related instructions execute in the expected deterministic order each time the program runs, ultimately ensuring that the program outputs the same result on every execution. Specifically, it includes the following two aspects:

- Insert synchronization between the move instruction before enabling atomic accumulation and the instruction that enables the atomic operation.

    As shown in the following pseudocode, inserting synchronization between instructions 1 and 2 ensures that the initial value of GM meets expectations before the atomic operation starts.

- Synchronization between multiple move instructions after enabling the atomic accumulation operation.

    Inserting synchronization between instructions 3 and 4, and between 4 and 5, ensures that the order of floating-point accumulation meets expectations.

    >[!CAUTION]Note
    >You do not need to insert synchronization between the instruction that enables the atomic operation and the subsequent move instructions.

```text
// The entire atomic accumulation is executed within the same core, and the execution order of the five instructions is controlled as "1→2→3→4→5".
1. Move data0 to GM;    // Data movement overwrites the original random value in GM, expecting the GM data to be data0.
Intra-core synchronization
2. SetAtomicAdd();         // Enable atomic accumulation. All subsequent moves from UB/L0C Buffer/L1 Buffer to GM perform atomic accumulation.
// No synchronization is required between instructions 2 and 3.
3. Move data1 to GM;    // Data movement after enabling atomic accumulation. The expected GM data is data0 + data1.
intra-core synchronization
4. Move data2 to GM;    // Data movement after enabling atomic accumulation. The expected GM data is data0 + data1 + data2.
intra-core synchronization
5. Move data3 to GM;    // Data movement after enabling atomic accumulation. The expected GM data is data0 + data1 + data2 + data3.
```

As shown in the following pseudocode, when the preceding instructions are executed on different cores, replace the intra-core synchronization above with inter-core synchronization.

```text
// The entire atomic accumulation is executed on four different cores, with the execution order controlled as "core 0 → core 1 → core 2 → core 3".
if (GetBlockIdx() == 0) {
   Move data0 to GM;
   inter-core synchronization
} else if (GetBlockIdx() == 1) {
   inter-core synchronization
   SetAtomicAdd();
   Move data1 to GM;
   inter-core synchronization
} else if (GetBlockIdx() == 2) {
   inter-core synchronization
   SetAtomicAdd();
   Move data2 to GM;
   inter-core synchronization
} else if (GetBlockIdx() == 3) {
   inter-core synchronization
   SetAtomicAdd();
   Move data3 to GM;
}
```

The following describes how to implement intra-core synchronization based on hardware synchronization instructions, and how to implement inter-core synchronization based on a software synchronization solution.

## Intra-core Synchronization<a name="zh-cn_topic_0000002583421469_section6896184932119"></a>

The pipeline types of move instructions and instructions that enable atomic operations are listed in the following table. When these instructions are executed within the same core, you can insert [single-pipeline synchronization](../sync_control/intra_core_sync/PipeBarrier_ISASI.md) or [multi-pipeline synchronization](../sync_control/intra_core_sync/SetFlag_WaitFlag_ISASI.md) as needed. For details, see the [branch with scenarioNum=1 in the sample](#zh-cn_topic_0000002583421469_example1).

**Table 1** Pipeline types of instructions related to deterministic computation of atomic operations

| Instruction Name | Pipeline type |
| --- | --- |
| DataCopy | PIPE_MTE3 |
| Fixpipe | PIPE_FIX |
| SetAtomicAdd/SetAtomicMax/SetAtomicMin | PIPE_S |

## Inter-core Synchronization<a name="zh-cn_topic_0000002583421469_section331316523218"></a>

Because no hardware synchronization API is currently provided for controlling the execution order between different cores, inter-core synchronization in deterministic computation scenarios must be implemented through software emulation. The software synchronization solutions differ for the three scenarios of pure Vector operators, pure Cube operators, and Mix operators (which contain both Vector and Cube computation), as shown in the following table.

**Table 2**  Software implementation solutions for inter-core synchronization

| Operator Type | Software Synchronization Solution | Description |
| --- | --- | --- |
| Pure Vector operator | Solution 1: Combine multiple pairs of [IBSet](../sync_control/inter_core_sync/IBSet.md) and [IBWait](../sync_control/inter_core_sync/IBWait.md) APIs to synchronize multiple AIVs. For details, see the branch with scenarioNum=2 in the [sample](#zh-cn_topic_0000002583421469_example2). | Solution 1 supports specifying some AIVs to participate in synchronization and controlling the execution order of each AIV. |
| Pure Vector operator | Solution 2: Use the [InitDetermineComputeWorkspace](../sync_control/inter_core_sync/InitDetermineComputeWorkspace.md), [NotifyNextBlock](../sync_control/inter_core_sync/NotifyNextBlock.md), and [WaitPreBlock](../sync_control/inter_core_sync/WaitPreBlock.md) APIs together to ensure that all AIV cores execute in ascending order of blockIdx. | Solution 2 requires all AIVs to participate in synchronization, and the execution order is fixed to ascending order of blockIdx. |
| Pure Cube operator | Use semaphores in GM to implement inter-core synchronization. First establish synchronization between a pair of AIC cores, and then extend it to synchronization among multiple AIC cores. For details, see the branch with scenarioNum=3 in the [sample](#zh-cn_topic_0000002583421469_example3). | When accessing GM through the Scalar unit, consider the data consistency issue among multiple cores. |
| Mix operator | Use semaphores in GM to implement inter-core synchronization. First establish synchronization between a pair of cores, and then extend it to synchronization among multiple cores. | When accessing GM through the Scalar unit, consider the data consistency issue among multiple cores. Here, "core" can be an AIV or an AIC. |

The following figure shows how two cores perform inter-core synchronization through semaphores in GM:

- After the previous core completes data movement or enables atomic operations, it writes the value 1 to the semaphore in the GM shared between cores through the Scalar unit, indicating that its task is complete. Intra-core synchronization also needs to be inserted in the previous core:
    - When the previous core has multiple move instructions, intra-core synchronization 1 needs to be inserted between them.
    - Before the Scalar unit writes data to GM, it must be ensured that all preceding move instructions have completed execution. Therefore, intra-core synchronization 2 also needs to be inserted between them.

- Before executing the move task, the current core continuously reads the value of the semaphore through the Scalar unit. If the semaphore is not equal to 1, the current core enters a blocking wait state. When the semaphore is detected to be equal to 1, the current core is unblocked and starts executing its own data movement or atomic operations. To ensure that the current core does not execute move instructions before the semaphore equals 1, intra-core synchronization 3 needs to be inserted before the move instructions.

**Figure 4**  Software synchronization solution flowchart between a pair of cores<a name="zh-cn_topic_0000002583421469_fig1491417244811"></a>

![](../../../figures/software_synchronization_between_two_cores_flowchart.png "Software synchronization flowchart between two cores")

The Scalar unit accesses the semaphore on GM in two ways:

- Access through DCache

    Use the GetValue and SetValue member functions of GlobalTensor for the operation. In this case, you need to manually call the [DataCacheCleanAndInvalid](../cache_control/DataCacheCleanAndInvalid.md) API to ensure data consistency across multiple cores.

- Access without going through DCache

    Use [WriteGmBypassDCache](../scalar_compute/WriteGmBypassDCache_ISASI.md) and [ReadGmBypassDCache](../scalar_compute/ReadGmBypassDCache_ISASI.md). This method ensures data consistency across multiple cores without additional operations.

The performance difference between the two solutions is as follows: bypassing DCache results in lower performance, but if the amount of GM data read and written is small, you can consider using the method that bypasses DCache.

As shown in Figure 4, the inter-core synchronization solution also needs to work with intra-core synchronization. The three intra-core synchronizations are described as follows:

- Intra-core synchronization 1 (optional): When there are multiple data move instructions within a core, this synchronization ensures that each move operation is executed strictly in order.
- Intra-core synchronization 2 (mandatory): Only after all tasks of the previous core are completed is the Scalar unit allowed to write 1 to the global memory semaphore.
- Intra-core synchronization 3 (mandatory): Wait until the Scalar unit detects that the semaphore is updated to 1, and then the current core starts executing subsequent tasks.

## Examples<a name="zh-cn_topic_0000002583421469_section027603463315"></a>

- **<a name="zh-cn_topic_0000002583421469_example1"></a>Intra-core synchronization deterministic computation (corresponding to scenarioNum=1): all instructions execute in the same AIV**

    ```cpp
    // Initialize GM with src0Local before the atomic accumulation.
    AscendC::DataCopy(dstGlobal, src0Local, DATA_LENGTH);
    // Ensure that GM initialization is complete before the atomic accumulation.
    AscendC::PipeBarrier<PIPE_MTE3>();

    // Enable atomic accumulation.
    // The expected accumulation execution order is: src2Local + src3Local + src1Local.
    AscendC::SetAtomicAdd<float>();
    AscendC::DataCopy(dstGlobal, src2Local, DATA_LENGTH);
    // Call PipeBarrier<PIPE_MTE3>() between the two move instructions to ensure the correct move order of src2Local and src3Local.
    AscendC::PipeBarrier<PIPE_MTE3>();
    AscendC::DataCopy(dstGlobal, src3Local, DATA_LENGTH);
    AscendC::PipeBarrier<PIPE_MTE3>();
    AscendC::DataCopy(dstGlobal, src1Local, DATA_LENGTH);

    AscendC::DisableDmaAtomic();
    // GM result: data0 + data2 + data3 + data1.
    ```

- **<a name="zh-cn_topic_0000002583421469_example2"></a>Pure Vector operator inter-core synchronization (corresponding to scenarioNum=2): four moves execute in four different AIVs**

    ```cpp
    // The expected execution order of the four cores is core 0 --> core 2 --> core 3 --> core 1.
    // Core 0 initializes GM with src0Local before the atomic accumulation; cores 2, 3, and 1 perform the atomic accumulation in order.

    if (blockIdx == 0) {
        AscendC::LocalTensor<float> src0Local = inQueueSrc0.AllocTensor<float>();
        AscendC::DataCopy(src0Local, src0Global, DATA_LENGTH);
        AscendC::SetFlag<AscendC::HardEvent::MTE2_MTE3>(0);
        AscendC::WaitFlag<AscendC::HardEvent::MTE2_MTE3>(0);

        // Initialize GM with src0Local before the atomic accumulation.
        AscendC::DataCopy(dstGlobal, src0Local, DATA_LENGTH);

        // Core 0 notifies core 2 that the initialization of src0Local on core 0 is complete.
        auto sync_buf = vecIn.AllocTensor<int32_t>();
        AscendC::IBSet(src4Global, sync_buf, 0, 0); // Last two parameters: int32_t blockIdx, int32_t eventID.
        inQueueSrc0.FreeTensor(src0Local);
        vecIn.FreeTensor(sync_buf);
        AscendC::printf("core index %d\n", blockIdx);
    } else if (blockIdx == 1) {
        AscendC::LocalTensor<float> src1Local = inQueueSrc1.AllocTensor<float>();
        // Core 1 waits for core 3 until core 3 has finished the accumulation.
        auto sync_buf = vecIn.AllocTensor<int32_t>();
        AscendC::IBWait(src4Global, sync_buf, 3, 0); // Last two parameters: int32_t blockIdx, int32_t eventID.

        AscendC::DataCopy(src1Local, src1Global, DATA_LENGTH);
        AscendC::SetFlag<AscendC::HardEvent::MTE2_MTE3>(0);
        AscendC::WaitFlag<AscendC::HardEvent::MTE2_MTE3>(0);

        AscendC::DisableDmaAtomic();
        AscendC::SetAtomicAdd<float>();
        AscendC::DataCopy(dstGlobal, src1Local, DATA_LENGTH);
        AscendC::DisableDmaAtomic();
        vecIn.FreeTensor(sync_buf);
        inQueueSrc1.FreeTensor(src1Local);
        AscendC::printf("core index %d\n", blockIdx);
    } else if (blockIdx == 2) {
        AscendC::LocalTensor<float> src2Local = inQueueSrc2.AllocTensor<float>();
        // Core 2 waits for core 0 until core 0 has finished initializing GM.
        auto sync_buf = vecIn.AllocTensor<int32_t>();
        AscendC::IBWait(src4Global, sync_buf, 0, 0); // Last two parameters: int32_t blockIdx, int32_t eventID.

        AscendC::DataCopy(src2Local, src2Global, DATA_LENGTH);
        AscendC::SetFlag<AscendC::HardEvent::MTE2_MTE3>(0);
        AscendC::WaitFlag<AscendC::HardEvent::MTE2_MTE3>(0);

        AscendC::DisableDmaAtomic();
        AscendC::SetAtomicAdd<float>();
        AscendC::DataCopy(dstGlobal, src2Local, DATA_LENGTH);
        AscendC::DisableDmaAtomic();

        // Core 2 notifies core 3 that core 2 data accumulation is complete.
        AscendC::IBSet(src4Global, sync_buf, 2, 0);
        vecIn.FreeTensor(sync_buf);
        inQueueSrc2.FreeTensor(src2Local);
        AscendC::printf("core index %d\n", blockIdx);
    } else if (blockIdx == 3) {
        // Core 3 waits for core 2 until core 2 data accumulation is complete.
        auto sync_buf = vecIn.AllocTensor<int32_t>();
        AscendC::IBWait(src4Global, sync_buf, 2, 0); // Last two parameters: int32_t blockIdx, int32_t eventID.

        AscendC::LocalTensor<float> src3Local = inQueueSrc3.AllocTensor<float>();
        AscendC::DataCopy(src3Local, src3Global, DATA_LENGTH);
        //  AscendC::DumpTensor(src3Local,0,8);
        AscendC::SetFlag<AscendC::HardEvent::MTE2_MTE3>(0);
        AscendC::WaitFlag<AscendC::HardEvent::MTE2_MTE3>(0);

        AscendC::DisableDmaAtomic();
        AscendC::SetAtomicAdd<float>();
        AscendC::DataCopy(dstGlobal, src3Local, DATA_LENGTH);
        AscendC::DisableDmaAtomic();

        // Core 3 notifies core 1 that core 3 data accumulation is complete.
        AscendC::IBSet(src4Global, sync_buf, 3, 0);
        vecIn.FreeTensor(sync_buf);
        inQueueSrc3.FreeTensor(src3Local);
        AscendC::printf("core index %d\n", blockIdx);
    }
    // GM result: data0 + data2 + data3 + data1.
    ```

- **<a name="zh-cn_topic_0000002583421469_example3"></a>Pure Cube inter-core synchronization (corresponding to scenarioNum=3): four moves are executed in four different AICs**

    ```cpp
    // The expected execution order of the four cores is core 0 --> core 2 --> core 3 --> core 1.
    // Core 0 initializes GM with src0Local before atomic accumulation; cores 2, 3, and 1 perform atomic accumulation in order.
    if (blockIdx == 0) {
        AscendC::DataCopy(src0Local, src0Global, DATA_LENGTH);
        AscendC::SetFlag<AscendC::HardEvent::MTE2_MTE3>(0);
        AscendC::WaitFlag<AscendC::HardEvent::MTE2_MTE3>(0);

        // Initialize GM with src0Local before atomic accumulation.
        AscendC::DataCopy(dstGlobal, src0Local, DATA_LENGTH);

        // Core 0 notifies core 2 that the initialization of src0Local on core 0 is complete.
        // Wait for the current core's task to complete before notifying the next core.
        AscendC::SetFlag<AscendC::HardEvent::MTE3_S>(0);
        AscendC::WaitFlag<AscendC::HardEvent::MTE3_S>(0);
        AscendC::WriteGmBypassDCache<int32_t>(addr + blockIdx * 32, 1);
    } else if (blockIdx == 1) {
        int32_t preblockIdx = 3;

        // Core 1 waits for core 3 until the data on core 3 has been accumulated.
        while (true) {
            int32_t value = AscendC::ReadGmBypassDCache<int32_t>(addr + preblockIdx * 32);
            if (value == 1) {
                AscendC::WriteGmBypassDCache<int32_t>(addr + preblockIdx * 32, 0);
                break;
            }
        }
        // Wait for the notification from the previous core before starting the current core's task.
        AscendC::SetFlag<AscendC::HardEvent::S_MTE2>(0);
        AscendC::WaitFlag<AscendC::HardEvent::S_MTE2>(0);

        AscendC::DataCopy(src1Local, src1Global, DATA_LENGTH);
        AscendC::SetFlag<AscendC::HardEvent::MTE2_MTE3>(0);
        AscendC::WaitFlag<AscendC::HardEvent::MTE2_MTE3>(0);

        AscendC::DisableDmaAtomic();
        AscendC::SetAtomicAdd<float>();
        AscendC::DataCopy(dstGlobal, src1Local, DATA_LENGTH);
        AscendC::DisableDmaAtomic();
    } else if (blockIdx == 2) {
        int32_t preblockIdx = 0;

        // Core 2 waits for core 0 until core 0 has completed GM initialization.
        while (true) {
            int32_t value = AscendC::ReadGmBypassDCache<int32_t>(addr + preblockIdx * 32);
            if (value == 1) {
                AscendC::WriteGmBypassDCache<int32_t>(addr + preblockIdx * 32, 0);
                break;
            }
        }
        // Wait for the notification from the previous core before starting the current core's task.
        AscendC::SetFlag<AscendC::HardEvent::S_MTE2>(0);
        AscendC::WaitFlag<AscendC::HardEvent::S_MTE2>(0);

        AscendC::DataCopy(src2Local, src2Global, DATA_LENGTH);
        AscendC::SetFlag<AscendC::HardEvent::MTE2_MTE3>(0);
        AscendC::WaitFlag<AscendC::HardEvent::MTE2_MTE3>(0);

        AscendC::DisableDmaAtomic();
        AscendC::SetAtomicAdd<float>();
        AscendC::DataCopy(dstGlobal, src2Local, DATA_LENGTH);
        AscendC::DisableDmaAtomic();

        // Wait for the current core's task to complete before notifying the next core.
        AscendC::SetFlag<AscendC::HardEvent::MTE3_S>(0);
        AscendC::WaitFlag<AscendC::HardEvent::MTE3_S>(0);
        AscendC::WriteGmBypassDCache<int32_t>(addr + blockIdx * 32, 1);
    } else if (blockIdx == 3) {
        int32_t preblockIdx = 2;

        // Core 3 waits for core 2 until the data on core 2 has been accumulated.
        while (true) {
            int32_t value = AscendC::ReadGmBypassDCache<int32_t>(addr + preblockIdx * 32);
            if (value == 1) {
                AscendC::WriteGmBypassDCache<int32_t>(addr + preblockIdx * 32, 0);
                break;
            }
        }

        // Wait for the notification from the previous core before starting the current core's task.
        AscendC::SetFlag<AscendC::HardEvent::S_MTE2>(0);
        AscendC::WaitFlag<AscendC::HardEvent::S_MTE2>(0);

        AscendC::DataCopy(src3Local, src3Global, DATA_LENGTH);
        AscendC::SetFlag<AscendC::HardEvent::MTE2_MTE3>(0);
        AscendC::WaitFlag<AscendC::HardEvent::MTE2_MTE3>(0);

        AscendC::DisableDmaAtomic();
        AscendC::SetAtomicAdd<float>();
        AscendC::DataCopy(dstGlobal, src3Local, DATA_LENGTH);
        AscendC::DisableDmaAtomic();

        // Wait for the current core to finish its task before notifying the next core.
        AscendC::SetFlag<AscendC::HardEvent::MTE3_S>(0);
        AscendC::WaitFlag<AscendC::HardEvent::MTE3_S>(0);
        AscendC::WriteGmBypassDCache<int32_t>(addr + blockIdx * 32, 1);
    }
    // GM result: data0 + data2 + data3 + data1.
    ```

For the complete sample, see [Sample of deterministic computation using in-line atomic data movement](../../../../../../examples/01_simd_cpp_api/03_basic_api/06_atomic/set_atomic_deterministic_computation).
