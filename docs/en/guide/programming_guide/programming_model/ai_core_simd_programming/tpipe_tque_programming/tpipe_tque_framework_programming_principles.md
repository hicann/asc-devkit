# TPipe-TQue Programming Principles

<!-- md-trans-meta sourceCommit=f6490d1e030d257ee55f3f1600bffece5a8bd3f7 translatedAt=2026-08-20T11:30:17.000Z pushedAt=2026-08-26T03:06:45.083Z -->

## Design Philosophy Origin: From Classic Queue Pipeline to the Ascend C TPipe-TQue Programming Model

The core architecture of the Ascend C programming model deeply inherits the classic design philosophy of the **queue pipeline** in traditional C/C++ concurrent programming. In general-purpose concurrent programming systems, the core value of the pipeline pattern lies in task decoupling and process structuring: a complex compute task is split into multiple independent, parallel-executable processing stages, with data transfer and message synchronization between stages accomplished through thread-safe queues.

This architecture of **stage decoupling and queue interconnection** transforms originally unordered asynchronous concurrency control into a logically clear, sequentially understandable data flow, fundamentally eliminating the mental burden imposed by explicit locks and complex synchronization, and reducing the cost of designing, debugging, and maintaining parallel programs.

For the heterogeneous parallel compute scenarios of AI Core, Ascend C adapts and innovates upon the classic queue pipeline concept, building a dedicated **TPipe-TQue parallel programming model**. This model consists of three core elements: parallel-executable processing stages, global resource management, and cross-stage queue-based synchronization communication. At the same time, Ascend C encapsulates the underlying hardware synchronization instructions and on-chip memory lifetime management into two standardized API groups, `EnQue`/`DeQue` and `AllocTensor`/`FreeTensor`, forming a lightweight, highly controllable, and standardized heterogeneous parallel programming paradigm.

## Core Definitions and Programming Principles of the TPipe-TQue Framework

**TPipe** and **TQue** are the two fundamental components that support the parallel execution of multiple stages in a pipeline. They have distinct responsibilities and work closely together:

- **[TPipe](../../../../../api/SIMD-API/basic_api/resource_management/TPipe/TPipe.md)** serves as the resource manager, uniformly managing system memory resources and events used for synchronization.

- **[TQue](../../../../../api/SIMD-API/basic_api/resource_management/TQue/TQue.md)** serves as the queue, responsible for synchronization and communication between stage tasks.

Under the TPipe-TQue model, the complete operator computation process is divided into several independent, parallel-schedulable stages, each of which follows a standardized four-step programming paradigm:

1. **Memory acquisition**: Actively allocate on-chip temporary memory through `AllocTensor`, or obtain ready data memory from the upstream TQue through `DeQue`.

2. **Computation execution**: Perform core computation, data movement, or format conversion on the acquired valid memory.

3. **Data transfer and synchronization**: Push the result data into the downstream TQue through `EnQue`, and simultaneously trigger a synchronization signal to wake up the waiting downstream tasks.

4. **Memory release**: For memory that is no longer in use, call `FreeTensor` to actively release it and send an overwritable signal to the hardware, so that subsequent tasks can reuse it and improve on-chip memory utilization.

This paradigm abstracts the complex asynchronous parallel scheduling and hardware synchronization into memory management and queue operations familiar to developers. Taking vector computation as an example, these APIs are converted by the compiler into underlying instruction sequences, forming a complete instruction pipeline, as shown in the following figure:

**Figure 1**  Vector programming paradigm instruction queue example<a name="fig27113275917"></a>  
![](../../../../figures/vector_programming_paradigm_instruction_queue_example.png "Vector programming paradigm instruction queue example")

Instruction generation, dispatch, and execution follow a unified asynchronous pipeline:

1.  **Instruction fetch and decode**: The scalar processing unit reads the operator instruction sequence and decodes it.

2.  **Instruction dispatch**: The scalar unit dispatches the decoded instructions to the instruction queue of the corresponding execution unit.

3.  **Parallel execution**: Each execution unit independently and concurrently fetches instructions from its own queue and executes them.

Under this asynchronous parallel architecture, the concurrent execution of multiple units and multiple stages introduces complex data dependencies, which is precisely the core difficulty of heterogeneous programming. Ascend C addresses this problem by using two pairs of APIs to precisely insert `Set`/`Wait` hardware synchronization instructions at the underlying level, at an extremely low performance cost.

## Data Dependency and Synchronization Mechanism: Collaborative Work of the Dual APIs

The core data dependencies in asynchronous parallel compute can be summarized into two types: **write-after-read (WAR)** and **read-after-write (RAW)**. Ascend C provides two paired API sets accordingly, forming a complete parallel synchronization loop.

### Producer-Consumer Synchronization: EnQue/DeQue Resolve WAR Conflicts

WAR is a typical upstream-downstream dependency: the upstream producer must complete the write before the downstream consumer can begin reading; otherwise, incomplete or incorrect data will be read. `EnQue`/`DeQue` are used to enforce this strict timing constraint.

- **EnQue (producer signal)**: As the closing operation of the upstream task, after being called, it automatically issues the `Set` hardware synchronization instruction at the underlying layer, marking that the write in the current stage is complete and proactively waking up the blocked downstream task.

- **DeQue (consumer wait)**: As the starting operation of the downstream task, after being called, it issues the `Wait` synchronization instruction at the underlying layer. The current task enters a blocked state until it receives the upstream `Set` signal, ensuring that the required data is fully ready before reading proceeds.

![](../../../../figures/figure_1_ai_core_internal_parallel_compute_architecture_abstract_diagram_28.png)

Through the precise coordination of signal blocking and wake-up, this mechanism strictly guarantees the **write-before-read** execution sequence, eliminating read-while-write conflicts at the source and ensuring the integrity and correctness of cross-stage data transfer.

### Memory Lifetime Management: AllocTensor/FreeTensor Resolving RAW Conflicts

RAW conflicts arise from memory reuse: if a block of on-chip memory is overwritten by new data while readers are still accessing it, the unread data is lost and computation errors occur. `AllocTensor`/`FreeTensor` resolve this contradiction by managing the memory lifetime.

- **AllocTensor (allocation and waiting)**: When a task requests a block of memory, the underlying layer issues a `Wait` synchronization instruction, which keeps waiting until all read operations on that memory region are fully completed and the region enters an overwritable state. Only then is the memory allocated and the task unblocked.

- **FreeTensor (release and notification)**: When a task finishes using the memory and calls `FreeTensor`, the underlying layer issues a `Set` synchronization instruction to notify the hardware that the memory no longer has read dependencies and can be safely released or overwritten for reuse by subsequent tasks.

![](../../../../figures/figure_1_ai_core_internal_parallel_compute_architecture_abstract_diagram_29.png)

This mechanism precisely manages the read/write state and lifetime of each block of on-chip memory, ensuring that overwriting occurs only after all read operations are complete. It eliminates RAW conflicts at the underlying level, enabling efficient and safe reuse of on-chip memory.

![](../../../../figures/Iterate_flase_2.png)

As shown above, the complex synchronization control required by asynchronous parallel programs is transformed in Ascend C into resource operations familiar to developers, such as `EnQue`/`DeQue` and `AllocTensor`/`FreeTensor`. This encapsulation preserves hardware-level performance while greatly reducing the difficulty of programming and understanding.

## Summary

The core challenge of heterogeneous asynchronous parallel programming lies in the precise orchestration of underlying hardware synchronization signals, strict timing constraints among multiple tasks, and secure memory management. The TPipe-TQue programming model adopts the classic queue-pipeline concept as its framework and, through the clear division of labor in which **TPipe manages resources and TQue manages communication**, encapsulates the obscure `Set`/`Wait` synchronization mechanism into intuitive queue operations and memory management APIs. This design fully unleashes the heterogeneous parallel compute power of the AI Core and guarantees ultimate operator performance while significantly lowering the barriers to development and learning, providing a concise, reliable, and standardized programming paradigm for high-performance operator development in the Ascend C domain.