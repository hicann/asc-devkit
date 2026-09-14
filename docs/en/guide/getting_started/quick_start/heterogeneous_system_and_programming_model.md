# Heterogeneous System and Programming Model

<!-- md-trans-meta sourceCommit=5ac36e94e68ec9e51e8b66b637216dd7c30b9b13 translatedAt=2026-08-26T12:51:15.233Z -->

> As a quick-start chapter on operator programming, this document starts with the fundamentals of the Ascend heterogeneous system and gradually breaks down the core elements of the programming model, helping you quickly build an overall understanding of Ascend operator programming and lay a foundation for subsequent hands-on development.



## What Is a Heterogeneous System?

A heterogeneous system based on Ascend processors is centered on **two types of functionally complementary processors working together**, balancing general-purpose computing and high-density parallel computing to achieve optimal overall performance.

| Role | Composition | Responsibility |
|------|------|------|
| **Host** | CPU + Host Memory | General-purpose computing, resource management, task scheduling, and overall coordination |
| **Device** | Ascend NPU + Device Memory | High-density parallel computing (deep learning inference/training, image processing, etc.) |

> 💡 **Plainly speaking**: The host is like a project manager, responsible for planning, allocation, and coordination; the device is like a team of specialized engineers, focused on efficiently completing intensive, repetitive computational work. The two work in collaboration to improve overall efficiency.



## Components of an Ascend Application

An application based on the Ascend heterogeneous system necessarily consists of two parts of code, which have clear division of responsibilities and run collaboratively:

| Code Type | Running Location | Programming Language | Core Responsibility |
|----------|----------|----------|----------|
| **Host code** | CPU (Host side) | Standard C/C++ | Manages Device devices, copies data, starts NPU tasks, and synchronizes status |
| **Device code** | NPU (Device side) | **Ascend C** | Executes specific parallel computation tasks, called [kernel functions](../../programming_guide/programming_model/ai_core_simd_programming/kernel_function.md) |

> ✅ **Tips**: Host code and device code can be written in the same `.asc` file, and the BiSheng Compiler automatically identifies and compiles them separately, simplifying the development process.



## Host-Device Collaboration Process

The host side collaborates with the device by calling the **CANN Runtime API**. The typical process is as follows:

1. **Allocate memory**: Request the space required for input/output in Device Memory.
2. **Data copy-in**: Copy the input data from Host Memory to Device Memory (data must enter the device side before it can be accessed by the NPU).
3. **Start the NPU computation task**: Call the [kernel function](../../programming_guide/programming_model/ai_core_simd_programming/kernel_function.md) pre-written on the device side, and the NPU starts parallel computation.
4. **Synchronize and wait**: The host side waits for the NPU to finish execution to ensure that the data computation is complete (avoiding reading the result before completion).
5. **Data copy-out**: Copy the computation result from Device Memory back to Host Memory for subsequent processing.

> 📌 **Key point**: The host and the NPU execute asynchronously. The synchronization in step 4 is essential; otherwise, incomplete data may be read.



## AI Core: Core Compute Unit of the NPU

The NPU is the computing core on the device side, while the **AI Core** is the "smallest computing unit" inside the NPU. An NPU chip typically integrates multiple AI Cores, which can process different data blocks in parallel, greatly improving the overall computing throughput.

The internal structure of each AI Core is highly optimized and specifically adapted to parallel computing requirements. Its core components include:

- **Scalar unit**: Handles control flow (such as branching and loops) and address computation. It is similar to a traditional CPU core and serves as the "control core" of the AI Core.
- **Vector unit**: Executes vector operations and is the primary carrier of the SIMD and SIMT parallel models. Different Ascend chip architectures provide varying degrees of support for these two modes.
- **Matrix unit**: Specially optimized for matrix multiply-accumulate operations. It is compatible only with the SIMD mode and serves as the "performance acceleration core" for operators such as convolution and fully connected layers in deep learning.
- **Local memory**: Used to cache the data required for computation, effectively reducing the access latency to the global Device Memory and improving computing efficiency.

> 🧬 **Architecture evolution**: Before the Ascend 950PR/Ascend 950DT architecture, the AI Core **supported only SIMD**. Starting from the Ascend 950PR/Ascend 950DT architecture, the vector unit **supports both SIMD and SIMT**, providing more flexible options for different operator tasks.



## SIMD and SIMT Parallel Models

**SIMD** (Single Instruction Multiple Data) and **SIMT** (Single Instruction Multiple Threads) are the two core parallel execution models supported by Ascend AI Core, and are also key to operator programming. The two models differ significantly in core logic and applicable scenarios, as compared below:

| Model | Full Name | Core Idea | Plain Analogy | Applicable Scenario |
|------|------|----------|----------|----------|
| **SIMD** | Single Instruction Multiple Data | One instruction operates on multiple homogeneous data elements simultaneously | One chef stir-fries multiple identical dishes at the same time | Regular, high-density computation (matrix multiplication, convolution, element-wise operations) |
| **SIMT** | Single Instruction Multiple Threads | One instruction drives multiple independent threads | One chef directs multiple assistants, each chopping their own ingredients | Irregular access, branch-intensive, sparse computation (such as sparse convolution) |

### Basic Steps of SIMD Kernel Programming (Following the SPMD Model)

> **SPMD (Single Program, Multiple Data)**: Each AI Core is abstracted as a Block, and the built-in variable [block_idx](../../programming_guide/language_extension/SIMD-BuiltIn_keyword.md) is used as the Block index. Each Block executes the same operator kernel code, and the data processing range of each Block is divided based on block_idx to implement multi-core load balancing and parallel scheduling.

1. **Tiling**: Divide the data into uniform blocks, with each AI Core responsible for one block to achieve load balancing.
2. **Data copy-in**: You need to **explicitly call the data transfer API** to move data from Device Memory to local storage.
3. **Data computation**: Call the **API corresponding to the SIMD instruction** to complete the computation. Note that data transfer and computation are asynchronous, so synchronization instructions must be inserted.
4. **Data copy-out**: You need to **explicitly call the data transfer API** to write the results in local storage back to Device Memory.

### Basic Steps of SIMT Kernel Programming (Following the SPMD Model)

> **SPMD (Single Program, Multiple Data)**: Each thread runs the same program, but each thread processes different data elements.

1. **Tiling**: Establish a one-to-one mapping between thread indices and data indices.
2. **Data copy-in**: Access Device Memory directly through pointers. The hardware automatically loads data into registers, so **there is no need to explicitly call data copy APIs**.
3. **Computation**: Write **scalar code** (supporting branches and loops). Insert synchronization instructions when multiple threads collaborate.
4. **Data copy-out**: Write results directly back to Device Memory through pointers, so **there is no need to explicitly call data copy APIs**.

> 📌 **Tips**: On the host side, you can invoke and run the kernel function through the `<<<>>>` syntax sugar or the CANN Runtime API.



## Programming Model Selection Guide

To improve the programming flexibility of vector computation in scenarios such as complex control flow and scattered memory access, Ascend 950PR/Ascend 950DT adopt a new homogeneous architecture that deeply integrates SIMD and SIMT: the cube computation unit continues the SIMD design, while the vector computation unit introduces SIMT capabilities on top of SIMD. This design establishes a new homogeneous programming model with **SIMD as the primary paradigm and SIMT as the auxiliary one**. Under this model, the SIMD portions of cube computation and vector computation are allocated more than 90% of the compute power, delivering high performance and high compute utilization for dense computation. SIMT in vector computation serves as a flexibility supplement, specifically addressing irregular scenarios such as complex control flow and discrete memory access, thereby improving the efficiency of algorithm development and optimization in such scenarios.

| Programming Model | Supported Scope | Applicable Scenario | Chip Support |
|----------|----------|----------|----------|
| **SIMD (primary)** | Vector, matrix, and fusion computation | Regular, high-density tasks (convolution, matrix multiplication, element-wise transformation) | All Ascend series |
| **SIMT (secondary)** | Vector computation only | Discrete access, complex branching, sparse computation | Ascend 950PR/Ascend 950DT only |
| **SIMD+SIMT hybrid** | Vector, matrix, and fusion computation | Use SIMT to handle irregular logic and SIMD for efficient batch computation | Ascend 950PR/Ascend 950DT only |

> 📌 **Quick selection suggestions**:
> - When developing conventional high-performance operators (such as matrix multiplication and element-wise operators), use the SIMD programming model. This model supports all Ascend series products and offers a high energy efficiency ratio.
> - For operators involving discrete data or complex branching logic, use the SIMT programming model.



## Summary

The core logic of Ascend heterogeneous computing can be summarized as: **Host (CPU) scheduling + Device (NPU) execution**

- **Host** side: coordinates resources and schedules tasks.
- **Device** side: the core is the **AI Core**, which supports a new homogeneous programming model with **SIMD as the primary and SIMT as the secondary** approach.

**Key to operator programming**: select an appropriate programming model based on the **regularity, memory access pattern, and branch density** of the data to fully leverage the hardware performance of the AI Core, thereby efficiently developing high-performance Ascend C operators.
