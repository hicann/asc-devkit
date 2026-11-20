---
okftype: reference.concept
description: "Starting from deterministic computation, this document analyzes two root causes of non-deterministic atomic accumulation results: multiple move instructions with inline atomic operations are executed out of order, causing the floating-point accumulation order to deviate from expectations, while normal move instructions before and after enabling atomic operations are reordered and overwrite the completed accumulation results. It also provides intra-core synchronization and inter-core software synchronization schemes based on GM semaphores, pseudocode, and a pipeline type table for the related instructions."
---
# Key Feature Description

<!-- md-trans-meta sourceCommit=54f1d966cfcc22290ddc2c6312bbe4c6ac2a9232 translatedAt=2026-11-16T09:15:48.281Z pushedAt=2026-11-18T08:15:34.163Z nl=2 -->

Deterministic computation is a computation process that always produces exactly the same output under the same input conditions, regardless of the number of executions or the operating environment. Deterministic computation ensures system stability and experimental verifiability.

The APIs mentioned in the following descriptions are explained as follows:

- For atomic accumulation of floating-point values, this document uses `asc_set_atomic_add_float()` as an example. For more atomic operation types, see [asc_set_atomic_add](datamove_atomic/asc_set_atomic_add.md).
- For atomic maximum and atomic minimum, this document uses `asc_set_atomic_max_float()` and `asc_set_atomic_min_float()` as examples. For more atomic operation types, see [asc_set_atomic_max](datamove_atomic/asc_set_atomic_max.md) and [asc_set_atomic_min](datamove_atomic/asc_set_atomic_min.md).
- For the APIs that move data to GM in AIV and AIC, this document uses `asc_copy_ub2gm()` and `asc_copy_l0c2gm()` as examples, respectively. For more move APIs, see [Vector Data Movement](../vector_datamove/vector_datamove.md) and [Cube Data Movement](../cube_datamove/cube_datamove.md).

## Deterministic Computation Overview

To introduce the problem of nondeterministic computation in atomic operation scenarios, we construct the following common deterministic computation scenario: first, initialize GM through a single group of floating-point data movement; then, start the atomic accumulation operation; finally, accumulate multiple groups of floating-point data on GM through multiple data movements. The specific pseudocode is as follows:

```text
1. Move data0 to GM;    // Data movement, overwrites the original random value in GM, expecting the GM data to be data0.
2. asc_set_atomic_add_float();         // Enable atomic add. Subsequent moves from Unified Buffer (UB)/L0C Buffer/L1 Buffer to GM all perform atomic add.
3. Move data1 to GM;    // Data movement with an atomic operation, expecting the GM data to be data0 + data1.
4. Move data2 to GM;    // Data movement with an atomic operation, expecting the GM data to be data0 + data1 + data2.
5. Move data3 to GM;    // Data movement with an atomic operation, expecting the GM data to be data0 + data1 + data2 + data3.
```

As shown in Figure 1, your expected result is that the instruction issue order strictly corresponds to the actual instruction execution order. No matter how many times this code segment is executed, the final GM data is always data0 + data1 + data2 + data3, with completely consistent results, achieving deterministic computation.

**Figure 1**  Deterministic computation scenario, GM data change process
![](../../../figures/deterministic_computation_gm_data_change_process.png "Deterministic computation scenario, GM data change process")

However, in reality, if you do not intervene, the execution order of these instructions may change each time the program runs, ultimately causing the GM data to be inconsistent with the expected result. The following lists two possible instruction execution orders and their corresponding execution flows.

## Nondeterministic Computation, Result 1

**Figure 2** Nondeterministic computation scenario 1, GM data change process
![](../../../figures/non_deterministic_computation_1_gm_data_change_process.png "Non-deterministic computation scenario 1, GM data change process")

As shown in Figure 2, the instruction execution flow in this scenario is as follows:

1. In the initial state, the GM data is a random value.
2. Move data0 to the GM. The GM data is initialized to data0.
3. Execute `asc_set_atomic_add_float()` to enable atomic accumulation for subsequent move instructions. The GM data is data0.
4. The three move instructions with inline atomic operations are out of order, and the actual execution order is "move out data2 → move out data3 → move out data1". The final GM data is data0 + data2 + data3 + data1.

**Cause 1 of nondeterministic computation:**

The move instructions with inline atomic operations are out of order. Because floating-point addition does not satisfy the associative law, that is, \(a+b\)+c!=a+\(b+c\), the final GM data data0 + data2 + data3 + data1 deviates from the expected data0 + data1 + data2 + data3.

The following three conditions must be met for the out-of-order move instructions with inline atomic operations to cause a deviation in the final result:

- The atomic operation type is atomic accumulation (maximum and minimum operations satisfy associativity).
- The atomic operation data type is floating-point (integer addition satisfies associativity).
- There are three or more move instructions with inline atomic operations (floating-point addition satisfies commutativity but not associativity).

## Nondeterministic Computation, Result 2

**Figure 3** Nondeterministic computation scenario 2, GM data change process
![](../../../figures/non_deterministic_computation_2_gm_data_change_process.png "Non-deterministic computation scenario 2, GM data change process")

As shown in Figure 3, the instruction execution flow in this scenario is as follows:

1. In the initial state, the GM data is a random value.
2. Execute `asc_set_atomic_add_float()` to enable atomic accumulation for subsequent move instructions. The GM data is: random value;
3. Execute two move instructions with inline atomic operations in sequence, in the execution order of "move out data1 → move out data2". The GM data is: random value + data1 + data2;
4. Move data0 to GM. The accumulated result on GM is overwritten by data0. The GM data is: data0;
5. Finally, execute the move of data3. The final data on GM is: data0 + data3.

**Cause 2 of nondeterministic computation:**

Out-of-order execution between a normal move instruction issued before atomic accumulation is enabled and a move instruction with atomic operations enabled causes the data on GM on which the atomic operation has been completed to be incorrectly overwritten by data0, resulting in a nondeterministic computation result.

This type of out-of-order execution that causes result deviation requires no preconditions. You no longer need to distinguish between atomic operation types or atomic operation data types, nor do you need to consider whether the number of move instructions with inline atomic operations reaches three or more.

## Deterministic Computation Implementation

Based on the two root causes of nondeterministic computation, the following describes the implementation of deterministic computation from these two aspects. The core idea is to insert appropriate synchronization between instructions so that the related instructions execute in the expected deterministic order each time the program runs, ultimately ensuring that the program produces the same output on every execution. Specifically, this includes the following two aspects:

- Insert synchronization between the move instruction before enabling atomic accumulation and the instruction that enables the atomic operation

    As shown in the following pseudocode, inserting synchronization between instructions 1 and 2 ensures that the initial value of GM meets expectations before the atomic operation starts.

- Synchronization between multiple move instructions after the atomic accumulation operation is enabled

    Inserting synchronization between instructions 3 and 4, and between 4 and 5, ensures that the order of floating-point accumulation meets expectations.

    >[!CAUTION]Note
    >The synchronization between the instruction that enables the atomic operation and the subsequent move instructions is guaranteed by hardware.

```text
// The entire atomic add is executed within the same core, controlling the execution order of the 5 instructions as "1->2->3->4->5".
1. Move data0 to GM;    // Data movement, overwrites the original random value in GM, expecting the GM data to be data0.
Intra-core synchronization
2. asc_set_atomic_add_float();         // Enable atomic add. Subsequent moves from UB/L0C Buffer/L1 Buffer to GM all perform atomic add.
// No synchronization is required between instructions 2 and 3.
3. Move data1 to GM;    // Data movement after enabling atomic add; the expected GM data is data0 + data1.
Intra-core synchronization
4. Move data2 to GM;    // Data movement after enabling atomic add; the expected GM data is data0 + data1 + data2.
Intra-core synchronization
5. Move data3 to GM;    // Data movement after enabling atomic add; the expected GM data is data0 + data1 + data2 + data3.
```

As shown in the following pseudocode, in a pure Vector operator, when the above instructions are executed on different AIV cores, the intra-core synchronization above must be replaced with inter-core synchronization.

```text
// The entire atomic add is executed on 4 different cores, and the execution order of the 4 cores is controlled as "core0→core1→core2→core3".
if (block_idx == 0) {
   Move data0 to GM;
   inter-core synchronization
} else if (block_idx == 1) {
   Inter-core synchronization
   asc_set_atomic_add_float();
   Move data1 to GM;
   inter-core synchronization
} else if (block_idx == 2) {
   inter-core synchronization
   asc_set_atomic_add_float();
   Move data2 to GM;
   inter-core synchronization
} else if (block_idx == 3) {
   inter-core synchronization
   asc_set_atomic_add_float();
   Move data3 to GM;
}
```

The following describes how to implement intra-core synchronization based on hardware synchronization instructions and inter-core synchronization based on a software synchronization scheme.

## Intra-Core Synchronization

The pipeline types of move instructions and instructions with atomic operations enabled are listed in the following table. When these instructions are executed within the same core, insert [asc_sync_pipe](../sync/intra_core_sync/asc_sync_pipe.md) or [asc_sync_notify](../sync/intra_core_sync/asc_sync_notify.md) and [asc_sync_wait](../sync/intra_core_sync/asc_sync_wait.md) as needed.

**Table 1** Pipeline types of instructions related to deterministic computation in atomic operations

| Instruction Name | Pipeline Type |
| --- | --- |
| asc_copy_ub2gm | PIPE_MTE3 |
| asc_copy_l0c2gm | PIPE_FIX |
| asc_set_atomic_add_float()/asc_set_atomic_max_float()/asc_set_atomic_min_float() | PIPE_S |

## Inter-Core Synchronization

Because no hardware synchronization API is currently provided for controlling the execution order between different cores, inter-core synchronization in deterministic computation scenarios must be implemented through software emulation: use semaphores in GM to implement inter-core synchronization, first establish synchronization between a pair of cores (AIV or AIC), and then extend it to synchronization among multiple cores.

The following figure shows how two cores perform inter-core synchronization through a semaphore on GM:

- After the previous core completes data movement or starts an atomic operation, it writes the value 1 to the semaphore on the GM shared between cores through the Scalar unit, indicating that its task is complete. Intra-core synchronization also needs to be inserted in the previous core:
    - When there are multiple move instructions in the previous core, intra-core synchronization 1 needs to be inserted between them.
    - Before the Scalar unit writes data to GM, it must be ensured that all preceding move instructions have completed execution. Therefore, intra-core synchronization 2 also needs to be inserted between them.

- Before executing the move task, the current core continuously reads the value of the semaphore through the Scalar unit. If the semaphore is not equal to 1, the current core enters a blocking wait state; when the semaphore is detected to be equal to 1, the current core is unblocked and starts executing its own data movement or atomic operation. To ensure that the current core does not execute the move instruction before the semaphore equals 1, insert intra-core synchronization 3 before the move instruction.

**Figure 4**  Software synchronization flowchart between two cores    
![](../../../figures/software_synchronization_between_two_cores_flowchart_c_api.png "Software synchronization flowchart between two cores")

There are two ways for the Scalar unit to access the semaphore on GM:

- Access through DCache

    When you read from or write to GM through `x_gm[i]` (where `x_gm` is a `__gm__` type pointer), you must manually call the [asc_dcci](../cache_ctrl/asc_dcci.md) API to ensure data consistency across multiple cores.

- Access without going through DCache

    Use [asc_store_dev](../scalar_compute/scalar_store/asc_store_dev.md) and its corresponding DCache-bypassing read capability to directly access GM. This approach ensures data consistency across multiple cores without any additional operations.

As shown in Figure 4, the inter-core synchronization scheme also needs to be used together with intra-core synchronization. The roles of the three intra-core synchronizations are described as follows:

- Intra-core synchronization 1 (optional): When multiple data move instructions exist within a core, this synchronization ensures that each move operation is executed strictly in order.
- Intra-core synchronization 2 (required): The Scalar unit is allowed to write 1 to the global memory semaphore only after all tasks of the previous core have completed execution.
- Intra-core synchronization 3 (required): The current core starts executing subsequent tasks only after the Scalar unit detects that the semaphore has been updated to 1.
