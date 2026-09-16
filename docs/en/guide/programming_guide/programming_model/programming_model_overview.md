# Programming Model Overview<a name="ZH-CN_TOPIC_0000002554090609"></a>

<!-- md-trans-meta sourceCommit=198cbfdf29f72f78c8de735d88a5244dc258f2e7 translatedAt=2026-08-20T10:58:12.646Z pushedAt=2026-08-26T03:07:19.217Z -->

As described in [Heterogeneous System](./heterogeneous_system.md), an application based on Ascend processors typically consists of two parts: **host code** and **device code**. The host code runs on the CPU and is responsible for device resource management, data movement between host memory and device memory, and task scheduling. The device code runs on the Neural-network Processing Unit (NPU) and specifically executes the actual computation tasks. This programming guide focuses on how to write device code based on the **Ascend C programming language** and how to schedule and execute the device code through host code.

## AI Core Hardware Basics

The core compute unit on the device side is the **AI Core**, which serves as the primary compute carrier of Ascend AI processors. A single Ascend NPU chip typically integrates multiple AI Cores, and these cores can work in parallel to greatly improve the overall compute throughput of the device. Each AI Core has a modular internal architecture with clearly defined responsibilities. Its core components are as follows:

- **Scalar unit**: Handles control flow (such as branching and loops) and address computation. It functions similarly to a traditional CPU core and serves as the "control center" of the AI Core.

- **Vector unit**: Undertakes core vector operations and is the primary hardware carrier of the two parallel execution models: SIMD and SIMT. Different Ascend hardware architectures differ in their adaptability to these two parallel modes.

- **Cube unit**: Provides deep hardware optimization for cube multiply-accumulate operations and supports only the SIMD execution mode. It is an ultra-fast acceleration unit for core operators such as convolution and fully connected layers in deep learning.

- **Local storage**: high-speed storage resources built into the AI Core, used to cache the data required for real-time computation. It effectively avoids the high-latency access problem of global device memory and significantly improves overall computation efficiency.

It is important to distinguish the architectural differences: AI Cores before the Ascend 950PR/Ascend 950DT architecture support only the **SIMD** execution model. Starting from the Ascend 950PR/Ascend 950DT architecture, the vector unit of the AI Core is compatible with both SIMD and SIMT parallel models, allowing flexible selection of parallel schemes for different computation tasks and adaptation to more complex business scenarios.

> 💡 **Why understand the internal structure of the AI Core?**  
> Understanding the AI Core hardware architecture is the core prerequisite for developing high-performance operators. For example, you must proactively move data to local storage to fully leverage the high-bandwidth compute advantages of SIMD; you must understand the hardware characteristics of the vector unit to proactively vectorize computation logic, maximize hardware peak performance, and avoid wasting computing power.

## Parallel Execution Models: SIMD and SIMT

In the field of high-performance parallel programming, **SIMD** and **SIMT** are two mainstream core parallel execution models. They define the core mechanism by which instructions drive multiple compute units to work collaboratively, and are key technologies for improving program data throughput and optimizing compute performance. They are also the core learning content of Ascend C programming.

### SIMD

**Core concept**: Single Instruction Multiple Data (SIMD) is a **data parallelism** model. Its core logic is that a single instruction performs exactly the same operation on multiple data elements within the same clock cycle, enabling batch parallel processing of data.

**Core features**:

- Single-instruction driven: All parallel compute units execute the same instruction synchronously, with completely identical operations.

- Data homogeneity: The data types involved in computation must be uniform and of the same length, ensuring that the instruction can process them in batches.

- Synchronous execution: Operations on all data are completed within the same instruction cycle, with no independent scheduling logic and a fully unified execution rhythm.

**Applicable scenarios**: mainly suited to computation tasks that are data-intensive, have regular operations, and involve no branching or very little branching. Typical scenarios include:

- Image pixel processing (such as grayscale conversion, filtering, and pixel scaling);

- Audio signal analysis (such as noise reduction and signal preprocessing);

- Core deep learning operations such as cube multiplication and convolution;

- Element-wise math functions (such as vector addition/subtraction, multiplication/division, and exponential/logarithmic operations).

**Four-step SIMD [kernel function](./ai_core_simd_programming/kernel_function.md) programming**: SIMD programming follows the Single Program Multiple Data (SPMD) model, where each AI Core runs the same kernel function but processes different data blocks. The specific steps are as follows:

1. **Tiling design**: Evenly partition the global ultra-large data, allocate balanced independent data tiles to each AI Core, precisely adapt to the SPMD multi-core parallel architecture, avoid single-core compute bottlenecks, and achieve full-domain load balancing.

2. **Data copy-in**: Call SIMD-specific APIs to move the data required for computation from device memory (usually HBM) to the local cache of the AI Core, reducing global memory access latency.

3. **Data computation**: Call vector instructions to process multiple homogeneous data elements at a time. Note that data movement and computation are usually executed asynchronously, so synchronization instructions must be inserted to ensure that the data is ready before computation, guaranteeing accurate results.

4. **Data copy-out**: Call SIMD-specific APIs to move the computation results from the local cache back to device memory for use by subsequent tasks.

### SIMT

**Core concept**: Single Instruction Multiple Threads (SIMT) is a **thread-parallel** model. Its core logic is that a single instruction simultaneously drives multiple independent threads, with each thread processing only one data element. Instructions are issued synchronously, but threads can execute independently based on code branches (such as if-else), offering higher flexibility.

**Core features**:

- Single instruction controlling multiple threads: A single instruction synchronously schedules a group of threads (such as a warp of 32 threads) and uniformly initiates instruction execution.

- Thread independence: Each thread has its own program counter, registers, and execution state, can process one data element independently, and supports complex branching logic.

- Automatic hardware scheduling and masking: Thread scheduling, switching, and masking of inactive threads are all handled automatically by the hardware, requiring no manual management by the programmer and reducing development complexity.

**Applicable scenarios**: Mainly suited to irregular data access, branch-intensive, and sparse computation scenarios. Typical scenarios include:

- Sparse operators in deep learning (such as sparse convolution and sparse cube operations);

- Element-wise operations with complex if-else branching;

- Algorithms with dynamic data dependencies (such as parallel prefix sum and sorting networks).

**Four-step SIMT [kernel function](./ai_core_simt_programming/kernel_function.md) programming**: SIMT programming also follows the SPMD model, that is, the same program runs on each thread, and each thread processes different data elements. The specific steps are as follows:

1. **Tiling design**: Split the overall task into multiple independent threads so that thread indices map with data indices in one-to-one manner, ensuring that each thread processes a unique data element and avoiding data duplication or omission.

2. **Data copy-in**: Directly access device memory through pointers, without the need to call dedicated APIs as in SIMD. The hardware automatically loads the required data from device memory into the thread registers, simplifying the development process.

3. **Data computation**: The programming style is similar to CPU scalar code, supporting complex control logic such as branching and loops. Data movement and computation generally do not require explicit synchronization. If multi-thread collaboration is involved (for example, using shared memory), synchronization instructions must be inserted.

4. **Data copy-out**: Directly write the computation results back to device memory through pointers, without the need to call dedicated APIs. This is more similar to CPU programming logic and lowers the learning curve.

> 📌 **Note:** On the host side, you can use the `<<<>>>` syntactic sugar to call and run the kernel function.

## AI Core Programming Model<a name="section_ai_core_programming_model"></a>

To improve the programming flexibility of vector computation in scenarios such as complex control flow and scattered memory access, Ascend 950PR/Ascend 950DT adopt a new homogeneous architecture that deeply integrates SIMD and SIMT: the cube computation unit continues the SIMD design, while the vector computation unit introduces SIMT capabilities on top of SIMD. This design establishes a new homogeneous programming model with **SIMD as the primary paradigm and SIMT as the auxiliary one**. Under this model, the SIMD portions of cube computation and vector computation are allocated more than 90% of the compute power, delivering high performance and high compute utilization for dense computation. SIMT in vector computation serves as a flexibility supplement, specifically addressing irregular scenarios such as complex control flow and discrete memory access, thereby improving the efficiency of algorithm development and optimization in such scenarios.

### SIMD Programming (Mainstream Paradigm, Supported by All Series)

- **Capability scope**: Supports vector computation, cube computation, and fused vector-cube computation, covering most core deep learning scenarios.

- **Applicable scenarios**: Regular, high-density data-parallel tasks such as convolution, cube multiplication, and element-wise transformation. It is the mainstream choice for Ascend NPU development.

- **Advantages**: High energy efficiency and low instruction execution overhead, fully leveraging hardware performance and approaching the peak compute capability of the AI Core.

- **Learning path**: For details, see [AI Core SIMD Programming](./ai_core_simd_programming/overview.md). For the operator development process, see [SIMD Operator Implementation](../../operator_practice/simd_operator_impl/simd_operator_implementation.md).

### SIMT Programming (Supplementary, Ascend 950PR/Ascend 950DT Only)

- **Capability scope**: Supports only vector computation, without cube computation or fused vector-cube computation, resulting in a relatively limited functional scope.

- **Applicable scenarios**: Vector operators with discrete data access and complex branching control, and suitable for developers familiar with the SIMT model to quickly get started with Ascend C.

- **Limitations**: Currently supports only the Ascend 950PR/Ascend 950DT chip architecture.

- **Learning path**: For details, see [AI Core SIMT Programming](./ai_core_simt_programming/overview.md). For operator development, see [SIMT Operator Implementation](../../operator_practice/simt_operator_impl/simt_operator_implementation.md).

### SIMD and SIMT Hybrid Programming

The kernel function for SIMD and SIMT hybrid programming is still based on the SIMD programming model at the underlying layer. Developers can flexibly combine the two types of programming logic within a kernel function: use SIMT logic to handle irregular computation scenarios such as sparse indexing and complex branching, and output regularized data blocks; then use SIMD high-throughput vector/matrix operations to process regular data, balancing code flexibility with hardware high performance to adapt to complex hybrid computation scenarios. For detailed implementation methods, see [AI Core SIMD and SIMT Hybrid Programming](../advanced_programming/advanced_ai_core_programming_model/simd_simt_hybrid_programming/overview.md).

## AI Core Programming Summary

Ascend C adopts a **SIMD+SIMT dual-mode parallelism** core design. It retains the core advantages of SIMD, namely high energy efficiency and high throughput, while using the SIMT model to supplement the compute capability for irregular scenarios such as discrete data and complex branching, thereby achieving efficient adaptation to compute tasks across all scenarios. During actual development, developers can flexibly choose a single programming model or a hybrid programming scheme based on the algorithm's **memory access pattern** (contiguous and regular/discrete and random), **branch density** (low branching/high branching), and **parallelism granularity**. In addition, the host side provides capabilities such as memory management and task scheduling through the CANN Runtime API, working in coordination with device-side code to achieve efficient heterogeneous compute.

> 📌 **Next step**: If you mainly develop regular high-performance operators, it is recommended that you directly read [AI Core SIMD Programming](ai_core_simd_programming/overview.md). If you need to process sparse data or complex branching logic, you can start with [AI Core SIMT Programming](ai_core_simt_programming/overview.md).

## AI CPU Programming Model

The AI CPU is an auxiliary processor (based on the ARM architecture) on the device, mainly used to execute tasks that cannot be efficiently processed by the AI Core, such as logic with complex control flow and strong data dependencies. Its programming model follows general-purpose CPU programming specifications and can be developed using standard C/C++ syntax. For the detailed development guide, see [AI CPU Programming](./ai_cpu_programming.md).