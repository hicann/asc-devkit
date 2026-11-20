# Programming Model Overview<a name="ZH-CN_TOPIC_0000002554090609"></a>

<!-- md-trans-meta sourceCommit=5c07153f668df7dd8585d7a94d76b4114ca4b1ac translatedAt=2026-11-09T11:10:09.971Z pushedAt=2026-11-19T06:32:19.988Z nl=2 -->

As described in [Heterogeneous System](./heterogeneous_system.md), an application based on Ascend processors is generally divided into two parts: **Host code** and **Device code**. The Host code runs on the CPU and is responsible for device resource management, data transfer between Host Memory and Device Memory, and task scheduling. The Device code runs on the NPU (Neural-network Processing Unit) and specifically executes the actual computation tasks. This programming guide focuses on how to write Device code based on the **Ascend C programming language** and how to schedule and execute Device code through Host code.

## Parallel Execution Model: SPMD

**SPMD** (Single Program, Multiple Data) is a program-level parallel model: multiple independent compute units execute the same program code simultaneously, but each processes a different data slice.

**SIMD** (Single Instruction, Multiple Data) and **SIMT** (Single Instruction, Multiple Threads) both belong to the instruction-driven parallel execution paradigm: SIMD uses a single instruction to drive multiple compute units to process different data simultaneously, offering high hardware efficiency but limited branching capability; SIMT uses a single instruction to drive multiple threads to execute in parallel, where threads can branch independently, providing greater flexibility.

In Ascend C programming, SPMD combines with SIMD/SIMT in three typical ways:

- **SPMD+SIMD**: The AI Core is abstracted as a logical core (Block). All Blocks run the same code, programming is performed from the Block perspective, and SIMD instructions are called inside the Block.
- **SPMD+SIMT**: The hardware is abstracted into a Grid → Thread Block → Thread architecture. Programming is performed from the Thread perspective, focusing on the data that each Thread needs to process.
- **SPMD+SIMD/SIMT hybrid**: The kernel function essentially follows SPMD+SIMD, and internally switches between SIMD and SIMT at the granularity of vector functions (Vector Function, VF) — a SIMT VF is abstracted as a SIMT Thread Block, within which programming is Thread-based; a SIMD VF directly calls SIMD instructions internally.

For simplicity, SIMD and SIMT are used below to refer to the corresponding programming models respectively. The two define the core mechanism by which instructions drive multiple compute units to work collaboratively, and are key technologies for improving data throughput and optimizing compute performance. They are also the core content of Ascend C programming, and will be introduced separately in the following sections.

### SIMD (Single Instruction, Multiple Data)

**Core concept**: SIMD (Single Instruction, Multiple Data) is a **data parallelism** model. Its core logic is that a single instruction performs exactly the same operation on multiple data elements within the same clock cycle, enabling batch parallel processing of data.

**Core features**:
- Single-instruction driven: All parallel compute units execute the same instruction synchronously, with completely identical operations.
- Data homogeneity: The data types involved in computation must be uniform and of the same length, ensuring that the instruction can process them in batches.
- Synchronous execution: Operations on all data are completed within the same instruction cycle, with no independent scheduling logic and a fully unified execution rhythm.

**Applicable scenarios**: mainly suited to computation tasks that are data-intensive, have regular operations, and involve no branching or very little branching. Typical scenarios include:
- Image pixel processing (such as grayscale conversion, filtering, and pixel scaling);
- Audio signal analysis (such as noise reduction and signal preprocessing);
- Core deep learning operations such as cube multiplication and convolution;
- Element-wise math functions (such as vector addition/subtraction, multiplication/division, and exponential/logarithmic operations).

**Four-step programming of the SIMD [kernel function](./ai_core_simd_programming/kernel_function.md)**: In the SIMD programming model, each AI Core runs the same kernel function and processes different data blocks. The specific steps are as follows:
1. **Tiling design**: Evenly partition the global ultra-large data, allocate balanced independent data tiles to each AI Core, avoid single-core compute bottlenecks, and achieve full-domain load balancing.
2. **Data copy-in**: Call SIMD-specific APIs to move the data required for computation from device memory (usually HBM) to the local cache of the AI Core, reducing global memory access latency.
3. **Data computation**: Call vector instructions to process multiple homogeneous data elements at a time. Note that data movement and computation are usually executed asynchronously, so synchronization instructions must be inserted to ensure that the data is ready before computation, guaranteeing accurate results.
4. **Data copy-out**: Call SIMD-specific APIs to move the computation results from the local cache back to device memory for use by subsequent tasks.

### SIMT

**Core concept**: SIMT is a **thread parallelism** model. Its core logic is that a single instruction drives multiple independent threads simultaneously, with each thread processing only one data element. Instructions are issued synchronously, but threads can execute independently according to code branches (such as if-else), offering higher flexibility.

**Core features**:
- Single instruction controlling multiple threads: A single instruction synchronously schedules a group of threads (such as a warp of 32 threads) and uniformly initiates and executes them.
- Thread independence: Each thread has its own program counter, registers, and execution state, can process one data element independently, and supports complex branching logic.
- Automatic hardware scheduling and masking: Thread scheduling, switching, and masking of inactive threads are all handled automatically by the hardware, requiring no manual management by the programmer and reducing development complexity.

**Applicable scenarios**: Mainly suited to tasks such as irregular data access, branch-intensive, and sparse computation. Typical scenarios include:
- Sparse operators in deep learning (such as sparse convolution and sparse cube operations);
- Element-wise operations with complex if-else branching;
- Algorithms with dynamic data dependencies (such as parallel prefix sum and sorting networks).

**Four-step SIMT [kernel function](./ai_core_simt_programming/kernel_function.md) programming**: In the SIMT programming mode, the same program runs on each thread, and each thread processes different data elements. The specific steps are as follows:
1. **Tiling design**: Split the overall task into multiple independent threads so that thread indices map with data indices in one-to-one manner, ensuring that each thread processes a unique data element and avoiding data duplication or omission.
2. **Data copy-in**: Directly access device memory through pointers, without the need to call dedicated APIs as in SIMD. The hardware automatically loads the required data from device memory into the thread registers, simplifying the development process.
3. **Data computation**: The programming style is similar to CPU scalar code, supporting complex control logic such as branching and loops. Data movement and computation generally do not require explicit synchronization. If multi-thread collaboration is involved (for example, using shared memory), synchronization instructions must be inserted.
4. **Data copy-out**: Directly write the computation results back to device memory through pointers, without the need to call dedicated APIs. This is more similar to CPU programming logic and lowers the learning curve.

> 📌 **Note:** On the host side, you can use the `<<<>>>` syntactic sugar to call and run the kernel function.

## AI Core Hardware Basics

The two parallel execution models, SIMD and SIMT, both have clear physical hardware counterparts in Ascend AI processors. The core compute unit on the device side is the **AI Core**, which is the primary computing power carrier of Ascend AI processors. A single Ascend NPU chip usually integrates multiple AI Cores, and these cores can work in parallel and collaborate with each other, greatly improving the overall computing throughput of the device. Each AI Core has a clearly modularized internal architecture, and its core components are as follows:

- **Scalar unit**: Handles control flow (such as branching and loops) and address computation. It functions similarly to a traditional CPU core and serves as the "control center" of the AI Core.

- **Vector unit**: Undertakes vector operations and is the primary hardware carrier of the two parallel execution models: SIMD and SIMT. Different Ascend hardware architectures differ in their adaptability to these two parallel modes.

- **Cube unit**: Provides deep hardware optimization for cube multiply-accumulate operations and supports only the SIMD execution mode. It is an efficient acceleration unit for core operators such as convolution and fully connected layers in deep learning.

- **Local storage**: high-speed storage resources built into the AI Core, used to cache the data required for real-time computation. It effectively avoids the high-latency access problem of global device memory and significantly improves overall computation efficiency.

<!-- npu="950" id1 -->
It is important to distinguish the architectural differences: In architectures before the Ascend 950PR&950DT products, the AI Core supports only the **SIMD** execution model. Starting from the Ascend 950PR&950DT product architecture, the vector unit of the AI Core is compatible with both SIMD and SIMT parallel models, allowing flexible selection of parallel schemes based on computation tasks and adaptation to more complex business scenarios.
<!-- end id1 -->

> 💡 **Why understand the internal structure of the AI Core?**  
> Understanding the AI Core hardware architecture is the prerequisite for developing high-performance operators: only by knowing that data must be actively copied into local storage can you fully leverage the high-bandwidth computing power of SIMD; only by mastering the hardware characteristics of the vector unit can you proactively vectorize the computation logic, maximize the peak hardware performance, and avoid wasting computing power.

## AI Core Programming Model<a name="section_ai_core_programming_model"></a>

To improve the programming flexibility of vector computation in scenarios such as complex control flow and scattered memory access, the new homogeneous architecture deeply integrates SIMD and SIMT: the cube unit continues the SIMD design, while the vector unit introduces SIMT capabilities on top of SIMD, thereby establishing a new homogeneous programming model with **SIMD as the primary mode and SIMT as the secondary mode**. Under this model, the SIMD portion of cube computation and vector computation is allocated more than 90% of the computing power, delivering high performance and high computing power utilization for intensive computation; the SIMT portion of vector computation serves as a flexibility supplement, specifically handling irregular scenarios such as complex control flow and scattered memory access, and improving the efficiency of algorithm development and optimization in such scenarios.

### SIMD Programming (Mainstream Paradigm, Supported by All Series)

- **Capability scope**: Supports vector computation, cube computation, and fused vector-cube computation, covering most core deep learning scenarios.
- **Applicable scenarios**: Regular, high-density data-parallel tasks such as convolution, cube multiplication, and element-wise transformation. It is the mainstream choice for Ascend NPU development.
- **Advantages**: High energy efficiency and low instruction execution overhead, fully leveraging hardware performance and approaching the peak compute capability of the AI Core.
- **Learning path**: For details, see [AI Core SIMD Programming](./ai_core_simd_programming/overview.md). For the operator development process, see [SIMD Operator Implementation](../../operator_practice/simd_operator_impl/simd_operator_impl.md).

<!-- npu="950" id2 -->

### SIMT Programming (Supplementary)

- **Capability scope**: Supports only vector computation, without cube computation or fused vector-cube computation, resulting in a relatively limited functional scope.
- **Applicable scenarios**: Vector operators with discrete data access and complex branching control, and suitable for developers familiar with the SIMT model to quickly get started with Ascend C.
- **Limitations**: <!-- npu="950" id3 -->Currently, only the Ascend 950PR&950DT products are supported.<!-- end id3 -->
  <!-- @ref: asc-devkit/res/docs/zh/guide/programming_guide/programming_model/programming_model_overview_res.md#id1 -->
- **Learning path**: For details, see [AI Core SIMT Programming](./ai_core_simt_programming/overview.md). For operator development, see [SIMT Operator Implementation](../../operator_practice/simt_operator_impl/simt_operator_impl.md).
<!-- end id2 -->

### SIMD and SIMT Hybrid Programming

The kernel function of SIMD and SIMT hybrid programming is still based on the SIMD programming model at the underlying level. Developers can flexibly combine the two types of programming logic within the kernel function: first use SIMT logic to process irregular computations such as sparse indexes and complex branching, and output regularized data blocks; then use SIMD high-throughput vector/cube computation to process the regular data, balancing code flexibility with high hardware performance to adapt to complex hybrid computation scenarios. For the detailed implementation method, see [AI Core SIMD and SIMT Hybrid Programming](../advanced_programming/advanced_ai_core_programming_model/simd_simt_hybrid_programming/overview.md).

<!-- @ref: asc-devkit/res/docs/zh/guide/programming_guide/programming_model/programming_model_overview_res.md#id2 -->

## AI Core Programming Summary

Ascend C adopts the **SIMD+SIMT dual-mode parallelism** design, which retains the advantages of SIMD in high energy efficiency and high throughput while using the SIMT model to supplement the computation capability for irregular scenarios such as discrete data and complex branching, achieving efficient adaptation to full-scenario computation tasks. In actual development, developers can flexibly choose a single programming model or a hybrid programming solution based on the algorithm's **memory access pattern** (continuous and regular/discrete and random), **branch density** (low branching/high branching), and **parallelism granularity**. In addition, the host side provides capabilities such as memory management and task scheduling through the CANN Runtime API, working together with the device-side code to achieve efficient heterogeneous computation.

> 📌 **Next step**: If you mainly develop regular high-performance operators, it is recommended that you directly read [AI Core SIMD Programming](ai_core_simd_programming/overview.md). If you need to process sparse data or complex branching logic, you can start with [AI Core SIMT Programming](ai_core_simt_programming/overview.md).

## AI CPU Programming Model

The AI CPU is an auxiliary processor (based on the ARM architecture) on the device, mainly used to execute tasks that cannot be efficiently processed by the AI Core, such as logic with complex control flow and strong data dependencies. Its programming model follows general-purpose CPU programming specifications and can be developed using standard C/C++ syntax. For the detailed development guide, see [AI CPU Programming](./ai_cpu_programming.md).
