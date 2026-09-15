# Key Features<a name="ZH-CN_TOPIC_0000002555621184"></a>

<!-- md-trans-meta sourceCommit=e3c8c70b3f5d27f3770063ae4b535bc75d904e3e translatedAt=2026-08-27T14:04:22.274Z -->

Deterministic computation refers to a computation process that always produces completely consistent output results under the same input conditions, regardless of how the number of executions or the execution environment changes. Deterministic computation provides a guarantee for system stability and experimental verifiability.

## Deterministic Computation Overview<a name="ZH-CN_TOPIC_0000002555780816"></a>

To introduce the problem of non-deterministic computation in atomic operation scenarios, we construct the following common deterministic computation scenario: first, initialize the GM through a single group of floating-point data movement; then, start the atomic accumulation operation; finally, accumulate multiple groups of floating-point data on the GM through multiple data movements. The specific pseudocode is as follows:

```text
1. Move data0 to GM;    // Data movement, overwriting the original random value in the GM, with the expected GM data being data0.
2. SetAtomicAdd();         // Enable atomic accumulation. Subsequent movements from UB/L0C Buffer/L1 Buffer to the GM all perform atomic accumulation.
3. Move data1 to GM;    // Data movement with an accompanying atomic operation, with the expected GM data being data0 + data1.
4. Move data2 to GM;    // Data movement with an accompanying atomic operation, with the expected GM data being data0 + data1 + data2.
5. Move data3 to GM;    // Data movement with an accompanying atomic operation, with the expected GM data being data0 + data1 + data2 + data3.
```

As shown in Figure 1 below, the developer's expected result is that the order in which instructions are issued strictly corresponds to the actual instruction execution order. No matter how many times this code segment is executed, the final GM data is always data0 + data1 + data2 + data3, with completely consistent results, thereby achieving deterministic computation.

**Figure 1**  Deterministic computation scenario, GM data change process<a name="zh-cn_topic_0000002552941426_fig71821376818"></a>  
![](../../../figures/deterministic_computation_gm_data_change_process.png "Deterministic computation scenario, GM data change process")

However, in reality, if the developer does not intervene, the execution order of these instructions may change each time the program runs, ultimately causing the GM data to be inconsistent with the expected result. Two possible instruction execution orders and their corresponding execution flows are listed below.

## Non-deterministic Computation, Result 1<a name="zh-cn_topic_0000002552941426_section027603463315"></a>

**Figure 2**  Non-deterministic computation scenario 1, GM data change process<a name="zh-cn_topic_0000002552941426_fig273814664819"></a>  
![](../../../figures/non_deterministic_computation_1_gm_data_change_process.png "Non-deterministic computation scenario 1, GM data change process")

As shown in Figure 2, the instruction execution flow in this scenario is as follows:

1. In the initial state, the GM data is a random value.
2. Move data0 to GM, and the GM data is initialized to data0.
3. Execute SetAtomicAdd to enable atomic accumulation for subsequent move instructions. The GM data is data0.
4. The three move instructions with accompanying atomic operations are out of order, and the actual execution order is "move out data2 → move out data3 → move out data1". The final GM data is data0 + data2 + data3 + data1.

**Cause 1 of non-deterministic computation:**

The move instructions with accompanying atomic operations are out of order. Because floating-point addition does not satisfy the associative law, that is, \(a+b\)+c!=a+\(b+c\), the final GM data data0 + data2 + data3 + data1 deviates from the expected data0 + data1 + data2 + data3.

The following three preconditions must be met for the out-of-order move instructions with accompanying atomic operations to cause a deviation in the final result:

- The atomic operation type is atomic accumulation (maximum and minimum operations satisfy the associative law).
- The atomic operation data type is a floating-point number (integer addition satisfies the associative law).
- The number of move instructions with an accompanying atomic operation reaches three or more (floating-point addition satisfies the commutative law but not the associative law).

## Non-deterministic Computation, Result 2<a name="zh-cn_topic_0000002552941426_section1547411390818"></a>

**Figure 3** Non-deterministic computation scenario 2, GM data change process<a name="zh-cn_topic_0000002552941426_fig1829915169912"></a>  
![](../../../figures/non_deterministic_computation_2_gm_data_change_process.png "Non-deterministic computation scenario 2, GM data change process")

As shown in Figure 3, the instruction execution flow in this scenario is as follows:

1. In the initial state, the GM data is a random value.
2. Execute SetAtomicAdd to enable atomic accumulation for subsequent move instructions. The GM data is a random value.
3. Execute two move instructions with atomic operations carried along the path in sequence, in the order of "move out data1 → move out data2". The GM data is: random value + data1 + data2.
4. Move data0 to GM. The accumulated result on GM is overwritten by data0. The GM data is: data0.
5. Finally, execute the move of data3. The final data on GM is: data0 + data3.

**Cause 2 of non-deterministic computation:**

Out-of-order execution between a normal move instruction before atomic accumulation is enabled and a move instruction with atomic operations enabled causes the data on GM that has completed the atomic operation to be incorrectly overwritten by data0, thereby producing a non-deterministic computation result.

Such out-of-order execution that causes result deviation requires no preconditions. Developers no longer need to distinguish between atomic operation types or atomic operation data types, nor do they need to consider whether the number of move instructions with piggyback atomic operations reaches three or more.

## Deterministic Computation Implementation<a name="ZH-CN_TOPIC_0000002586300751"></a>

Based on the two root causes of non-deterministic computation, the following describes the implementation of deterministic computation from these two aspects. The core idea is to insert appropriate synchronization between instructions so that the related instructions execute in the expected deterministic order each time the program runs, ultimately ensuring that the program produces the same result on every execution. Specifically, this includes the following two aspects:

- Insert synchronization between the move instruction before atomic accumulation is enabled and the instruction that enables the atomic operation.

    As shown in the following pseudocode, inserting synchronization between instructions 1 and 2 ensures that the initial value of GM meets expectations before the atomic operation starts.

- Synchronization between multiple move instructions after atomic accumulation is enabled.

    Inserting synchronization between instructions 3 and 4, and between 4 and 5, ensures that the order of floating-point accumulation meets expectations.

    >[!CAUTION]Note
    >Developers do not need to insert synchronization between the instruction that enables the atomic operation and the subsequent move instructions.

```text
// The entire atomic accumulation is executed within the same core, controlling the execution order of the five instructions as "1→2→3→4→5".
1. Move data0 to GM;    // Data movement, overwriting the original random value in GM, with the expected GM data being data0.
Intra-core sync
2. SetAtomicAdd();         // Enable atomic accumulation. Subsequent moves from UB/L0C Buffer/L1 Buffer to GM all perform atomic accumulation.
// No synchronization is required between instructions 2 and 3.
3. Move data1 to GM;    // Data movement after atomic accumulation is enabled, expecting the GM data to be data0 + data1.
Intra-core sync
4. Move data2 to GM;    // Data movement after atomic accumulation is enabled, expecting the GM data to be data0 + data1 + data2.
Intra-core sync
5. Move data3 to GM;    // Data movement after atomic accumulation is enabled, expecting the GM data to be data0 + data1 + data2 + data3.
```

As shown in the following pseudocode, when the preceding instructions are executed on different cores, the intra-core synchronization described above must be replaced with inter-core synchronization.

```text
// The entire atomic accumulation is executed on four different cores, and the execution order of the four cores is controlled as "core 0 → core 1 → core 2 → core 3". 3".
if (GetBlockIdx() == 0) {
   Move data0 to GM;
   Intra-core synchronization
} else if (GetBlockIdx() == 1) {
   Intra-core synchronization    
   SetAtomicAdd();         
   Move data1 to GM;
   Intra-core synchronization   
} else if (GetBlockIdx() == 2) {
   Intra-core synchronization
   SetAtomicAdd();         
   Move data2 to GM;  
   Intra-core synchronization 
} else if (GetBlockIdx() == 3) {
   Intra-core synchronization
   SetAtomicAdd();         
   Move data3 to GM;   
} 
```

The following describes how to implement intra-core synchronization based on hardware synchronization instructions, and how to implement inter-core synchronization based on a software synchronization scheme.

## Intra-core Synchronization<a name="zh-cn_topic_0000002583421469_section6896184932119"></a>

The pipeline types of move instructions and instructions with atomic operations enabled are listed in the following table. When these instructions are executed within the same core, developers can insert [single-pipeline synchronization](../sync_control/intra_core_sync/PipeBarrier_ISASI.md) or [multi-pipeline synchronization](../sync_control/intra_core_sync/SetFlag_WaitFlag_ISASI.md) as needed. For details, see the [branch with scenarioNum=1 in the sample](#zh-cn_topic_0000002583421469_example1).

**Table 1**  Pipeline types of instructions related to deterministic computation for atomic operations

| Instruction Name | Pipeline Type |
| --- | --- |
| DataCopy | PIPE_MTE3 |
| Fixpipe | PIPE_FIX |
| SetAtomicAdd/SetAtomicMax/SetAtomicMin | PIPE_S |

## Inter-core Synchronization<a name="zh-cn_topic_0000002583421469_section331316523218"></a>

Because no hardware synchronization API is currently provided for controlling the execution order between different cores, inter-core synchronization in deterministic computation scenarios must be implemented through software emulation. For the three scenarios of pure Vector operators, pure Cube operators, and Mix operators (which contain both Vector and Cube computations), the software synchronization schemes adopted differ, as shown in the following table.

**Table 2**  Software implementation schemes for inter-core synchronization

| Operator Type | Software Synchronization Scheme | Description |
| --- | --- | --- |
| Pure Vector operator | Scheme 1: By combining multiple pairs of [IBSet](../sync_control/inter_core_sync/IBSet.md) and [IBWait](../sync_control/inter_core_sync/IBWait.md) APIs, synchronization among multiple AIVs can be achieved. For details, see [the branch with scenarioNum=2 in the sample](#zh-cn_topic_0000002583421469_example2). | Scheme 1 supports specifying a subset of AIVs to participate in synchronization and can control the execution order of each AIV. |
| Pure Vector operator | Scheme 2: By using the three APIs [InitDetermineComputeWorkspace](../sync_control/inter_core_sync/InitDetermineComputeWorkspace.md), [NotifyNextBlock](../sync_control/inter_core_sync/NotifyNextBlock.md), and [WaitPreBlock](../sync_control/inter_core_sync/WaitPreBlock.md) together, all AIV cores are ensured to execute in ascending order of blockIdx. | Scheme 2 requires all AIVs to participate in synchronization, and the execution order is fixed as ascending blockIdx. |
| Pure Cube operator | Inter-core synchronization is implemented through semaphores in GM. First establish synchronization between a pair of AIC cores, and then extend it to synchronization among multiple AIC cores. For details, see [the branch with scenarioNum=3 in the sample](#zh-cn_topic_0000002583421469_example3). | When accessing GM through the Scalar unit, the data consistency issue among multiple cores must be considered. |
| Mix operator | Inter-core synchronization is implemented through semaphores in GM. First establish synchronization between a pair of cores, and then extend it to synchronization among multiple cores. | When accessing GM through the Scalar unit, the data consistency issue among multiple cores must be considered. Here, a "core" can be either an AIV or an AIC. |

The following figure shows how inter-core synchronization is performed between two cores through semaphores in GM:

- After the previous core completes data movement or starts an atomic operation, it writes the value 1 to the semaphore in the GM shared between cores through the Scalar unit, indicating that its task is complete. Intra-core synchronization also needs to be inserted in the previous core:
    - When there are multiple move instructions in the previous core, intra-core synchronization 1 needs to be inserted between them.
    - Before the Scalar unit writes data to GM, it must be ensured that all preceding move instructions have completed execution. Therefore, intra-core synchronization 2 also needs to be inserted between them.

- Before executing the move task, the current core continuously reads the value of the semaphore through the Scalar unit. If the semaphore is not equal to 1, the current core enters a blocking wait state; when the semaphore is detected to be equal to 1, the current core is unblocked and starts executing its own data movement or atomic operation. To ensure that the current core does not execute the move instruction before the semaphore equals 1, intra-core synchronization 3 needs to be inserted before the move instruction.

**Figure 4**  Software synchronization scheme flowchart between a pair of cores<a name="zh-cn_topic_0000002583421469_fig1491417244811"></a>  
![](../../../figures/software_synchronization_between_two_cores_flowchart.png "Software synchronization flowchart between two cores")

The Scalar unit accesses the semaphore on GM in two ways:

- Access through DCache

    Use the **GetValue** and **SetValue** member functions of **GlobalTensor** for the operation. In this case, developers need to manually call the [DataCacheCleanAndInvalid](../cache_control/DataCacheCleanAndInvalid.md) API to ensure data consistency across multiple cores.

- Access without going through DCache

    Use [WriteGmByPassDCache](../scalar_compute/WriteGmByPassDCache_ISASI.md) and [ReadGmByPassDCache](../scalar_compute/ReadGmByPassDCache_ISASI.md). This approach guarantees data consistency across multiple cores without additional operations.

The performance difference between the two approaches is as follows: bypassing DCache results in lower performance, but if the amount of GM data read and written is small, the DCache-bypassing approach can be considered.

As shown in Figure 4, the inter-core synchronization scheme also needs to be used in conjunction with intra-core synchronization. The roles of the three intra-core synchronizations are described as follows:

- Intra-core synchronization 1 (optional): When multiple data movement instructions exist within a core, this synchronization ensures that each move operation is executed strictly in order.
- Intra-core synchronization 2 (mandatory): Only after all tasks of the previous core are completed is the Scalar unit allowed to write 1 to the global memory semaphore.
- Intra-core synchronization 3 (mandatory): Only after the Scalar unit detects that the semaphore has been updated to 1 does the current core start executing subsequent tasks.

## Example<a name="zh-cn_topic_0000002583421469_section027603463315"></a>

- **<a name="zh-cn_topic_0000002583421469_example1"></a>Intra-core synchronization deterministic computation (corresponding to scenarioNum=1): All instructions are executed in the same AIV.**

    ```cpp
    // Initialize GM with src0Local before the atomic accumulation.
    AscendC::DataCopy(dstGlobal, src0Local, DATA_LENGTH);
    // Ensure that GM initialization is complete before the atomic accumulation.
    AscendC::PipeBarrier<PIPE_MTE3>();

    // Enable atomic accumulation.
    // The expected accumulation execution order is: src2Local + src3Local + src1Local.
    AscendC::SetAtomicAdd<float>();
    AscendC::DataCopy(dstGlobal, src2Local, DATA_LENGTH);
    // Call PipeBarrier<PIPE_MTE3>() between the two move instructions to ensure the correct movement order of src2Local and src3Local.
    AscendC::PipeBarrier<PIPE_MTE3>();
    AscendC::DataCopy(dstGlobal, src3Local, DATA_LENGTH);
    AscendC::PipeBarrier<PIPE_MTE3>();
    AscendC::DataCopy(dstGlobal, src1Local, DATA_LENGTH);

    AscendC::DisableDmaAtomic();
    // GM result: data0 + data2 + data3 + data1.
    ```

- **<a name="zh-cn_topic_0000002583421469_example2"></a>Pure Vector operator inter-core synchronization (corresponding to scenarioNum=2): Four data movements are executed in four different AIVs.**

    ```cpp
    // The expected execution order of the four cores is core 0 --> core 2 --> core 3 --> core 1.e 1.
    // Core 0 is responsible for initializing GM with src0Local before atomic accumulation; cores 2, 3, and 1 are responsible for performing atomic accumulation in order.

    if (blockIdx == 0) {
        AscendC::LocalTensor<float> src0Local = inQueueSrc0.AllocTensor<float>();
        AscendC::DataCopy(src0Local, src0Global, DATA_LENGTH);
        AscendC::SetFlag<AscendC::HardEvent::MTE2_MTE3>(0);
        AscendC::WaitFlag<AscendC::HardEvent::MTE2_MTE3>(0);

        // Initialize GM with src0Local before atomic accumulation.
        AscendC::DataCopy(dstGlobal, src0Local, DATA_LENGTH);

        // Core 0 notifies core 2 that the initialization of src0Local on core 0 is complete.
        auto sync_buf = vecIn.AllocTensor<int32_t>();
        AscendC::IBSet(src4Global, sync_buf, 0, 0); // The last two parameters: int32_t blockIdx, int32_t eventID.
        inQueueSrc0.FreeTensor(src0Local);
        vecIn.FreeTensor(sync_buf);
        AscendC::printf("Core index %d\n", blockIdx);
    } else if (blockIdx == 1) {
        AscendC::LocalTensor<float> src1Local = inQueueSrc1.AllocTensor<float>();
        // Core 1 waits for core 3 until core 3 has completed the accumulation.
        auto sync_buf = vecIn.AllocTensor<int32_t>();
        AscendC::IBWait(src4Global, sync_buf, 3, 0); // The last two parameters: int32_t blockIdx, int32_t eventID.

        AscendC::DataCopy(src1Local, src1Global, DATA_LENGTH);
        AscendC::SetFlag<AscendC::HardEvent::MTE2_MTE3>(0);
        AscendC::WaitFlag<AscendC::HardEvent::MTE2_MTE3>(0);

        AscendC::DisableDmaAtomic();
        AscendC::SetAtomicAdd<float>();
        AscendC::DataCopy(dstGlobal, src1Local, DATA_LENGTH);
        AscendC::DisableDmaAtomic();
        vecIn.FreeTensor(sync_buf);
        inQueueSrc1.FreeTensor(src1Local);
        AscendC::printf("Core index %d\n", blockIdx);
    } else if (blockIdx == 2) {
        AscendC::LocalTensor<float> src2Local = inQueueSrc2.AllocTensor<float>();
        // Core 2 waits for core 0 until core 0 has completed GM initialization.
        auto sync_buf = vecIn.AllocTensor<int32_t>();
        AscendC::IBWait(src4Global, sync_buf, 0, 0); // The last two parameters: int32_t blockIdx, int32_t eventID.

        AscendC::DataCopy(src2Local, src2Global, DATA_LENGTH);
        AscendC::SetFlag<AscendC::HardEvent::MTE2_MTE3>(0);
        AscendC::WaitFlag<AscendC::HardEvent::MTE2_MTE3>(0);

        AscendC::DisableDmaAtomic();
        AscendC::SetAtomicAdd<float>();
        AscendC::DataCopy(dstGlobal, src2Local, DATA_LENGTH);
        AscendC::DisableDmaAtomic();

        // Core 2 notifies core 3 that core 2's data has been accumulated.
        AscendC::IBSet(src4Global, sync_buf, 2, 0);
        vecIn.FreeTensor(sync_buf);
        inQueueSrc2.FreeTensor(src2Local);
        AscendC::printf("Core index %d\n", blockIdx);
    } else if (blockIdx == 3) {
        // Core 3 waits for core 2 until core 2's data has been accumulated.
        auto sync_buf = vecIn.AllocTensor<int32_t>();
        AscendC::IBWait(src4Global, sync_buf, 2, 0); // The last two parameters: int32_t blockIdx, int32_t eventID.

        AscendC::LocalTensor<float> src3Local = inQueueSrc3.AllocTensor<float>();
        AscendC::DataCopy(src3Local, src3Global, DATA_LENGTH);
        //  AscendC::DumpTensor(src3Local,0,8);
        AscendC::SetFlag<AscendC::HardEvent::MTE2_MTE3>(0);
        AscendC::WaitFlag<AscendC::HardEvent::MTE2_MTE3>(0);

        AscendC::DisableDmaAtomic();
        AscendC::SetAtomicAdd<float>();
        AscendC::DataCopy(dstGlobal, src3Local, DATA_LENGTH);
        AscendC::DisableDmaAtomic();

        // Core 3 notifies core 1 that core 3's data has been accumulated.
        AscendC::IBSet(src4Global, sync_buf, 3, 0);
        vecIn.FreeTensor(sync_buf);
        inQueueSrc3.FreeTensor(src3Local);
        AscendC::printf("Core index %d\n", blockIdx);
    }
    // GM result: data0 + data2 + data3 + data1.
    ```

- **<a name="zh-cn_topic_0000002583421469_example3"></a>Pure Cube inter-core synchronization (corresponding to scenarioNum=3): Four moves are executed in four different AICs respectively**

    ```cpp
    // The expected execution order of the four cores is core 0 --> core 2 --> core 3 --> core 1.
    // Core 0 initializes GM with src0Local before the atomic accumulation; cores 2, 3, and 1 perform the atomic accumulation in order.
    if (blockIdx == 0) {
        AscendC::DataCopy(src0Local, src0Global, DATA_LENGTH);
        AscendC::SetFlag<AscendC::HardEvent::MTE2_MTE3>(0);
        AscendC::WaitFlag<AscendC::HardEvent::MTE2_MTE3>(0);

        // Initialize GM with src0Local before the atomic accumulation.
        AscendC::DataCopy(dstGlobal, src0Local, DATA_LENGTH);

        // Core 0 notifies core 2 that the src0Local initialization of core 0 is complete.
        // Wait for the current core to complete its task before notifying the next core.
        AscendC::SetFlag<AscendC::HardEvent::MTE3_S>(0);
        AscendC::WaitFlag<AscendC::HardEvent::MTE3_S>(0);
        AscendC::WriteGmByPassDCache<int32_t>(addr + blockIdx * 32, 1);
    } else if (blockIdx == 1) {
        int32_t preblockIdx = 3;

        // Core 1 waits for core 3 until the data of core 3 has been accumulated.
        while (true) {
            int32_t value = AscendC::ReadGmByPassDCache<int32_t>(addr + preblockIdx * 32);
            if (value == 1) {
                AscendC::WriteGmByPassDCache<int32_t>(addr + preblockIdx * 32, 0);
                break;
            }
        }
        // Wait for the notification from the previous core before starting the task of the current core.
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
            int32_t value = AscendC::ReadGmByPassDCache<int32_t>(addr + preblockIdx * 32);
            if (value == 1) {
                AscendC::WriteGmByPassDCache<int32_t>(addr + preblockIdx * 32, 0);
                break;
            }
        }
        // Wait for the notification from the previous core before starting the task of the current core.
        AscendC::SetFlag<AscendC::HardEvent::S_MTE2>(0);
        AscendC::WaitFlag<AscendC::HardEvent::S_MTE2>(0);

        AscendC::DataCopy(src2Local, src2Global, DATA_LENGTH);
        AscendC::SetFlag<AscendC::HardEvent::MTE2_MTE3>(0);
        AscendC::WaitFlag<AscendC::HardEvent::MTE2_MTE3>(0);

        AscendC::DisableDmaAtomic();
        AscendC::SetAtomicAdd<float>();
        AscendC::DataCopy(dstGlobal, src2Local, DATA_LENGTH);
        AscendC::DisableDmaAtomic();

        // Wait for the current core to complete its task before notifying the next core.
        AscendC::SetFlag<AscendC::HardEvent::MTE3_S>(0);
        AscendC::WaitFlag<AscendC::HardEvent::MTE3_S>(0);
        AscendC::WriteGmByPassDCache<int32_t>(addr + blockIdx * 32, 1);
    } else if (blockIdx == 3) {
        int32_t preblockIdx = 2;

        // Core 3 waits for core 2 until core 2 has finished accumulating the data.
        while (true) {
            int32_t value = AscendC::ReadGmByPassDCache<int32_t>(addr + preblockIdx * 32);
            if (value == 1) {
                AscendC::WriteGmByPassDCache<int32_t>(addr + preblockIdx * 32, 0);
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

        // Wait for the current core's task to complete before notifying the next core.
        AscendC::SetFlag<AscendC::HardEvent::MTE3_S>(0);
        AscendC::WaitFlag<AscendC::HardEvent::MTE3_S>(0);
        AscendC::WriteGmByPassDCache<int32_t>(addr + blockIdx * 32, 1);
    }
    // GM result: data0 + data2 + data3 + data1.
    ```

For the complete sample, see [Sample of deterministic computation using data movement with piggyback atomic operations](https://gitcode.com/cann/asc-devkit/tree/9.1.0/examples/01_simd_cpp_api/03_basic_api/06_atomic/set_atomic_deterministic_computation).
