# Heterogeneous System and Programming Model

<!-- md-trans-meta sourceCommit=d53e1f2b44a41ea05bcf35b79cb06ef444c034d2 translatedAt=2026-10-23T11:12:44.590Z pushedAt=2026-11-03T07:05:13.259Z nl=2 -->

> As a quick-start section for operator programming, this document starts with the fundamentals of the Ascend heterogeneous system and gradually breaks down the core elements of the programming model, helping you quickly build an overall understanding of Ascend operator programming and lay a foundation for subsequent hands-on development.

---

## What Is a Heterogeneous System?

The core of a heterogeneous system based on Ascend processors is **the collaborative work of two types of functionally complementary processors**, which balances general-purpose computing and high-density parallel computing to achieve optimal overall performance.

| Role | Composition | Responsibility |
|------|------|------|
| **Host** | CPU + host memory | General-purpose computing, resource management, task scheduling, and overall coordination |
| **Device** | Ascend NPU + device memory | High-density parallel computing (deep learning inference/training, image processing, etc.) |

> [!NOTE] Note
> **Plain understanding**: The host is like a project manager, responsible for planning, allocation, and coordination; the device is like a team of professional engineers, focused on efficiently completing intensive and repetitive computation tasks. The two work in a division of labor to improve overall efficiency.

---

## Components of an Ascend Application

An application based on the Ascend heterogeneous system necessarily consists of two parts of code, which have clear division of responsibilities and run collaboratively:

| Code Type | Running Location | Programming Language | Core Responsibility |
|----------|----------|----------|----------|
| **Host code** | CPU (host side) | Standard C/C++ | Manages devices, transfers data, launches NPU tasks, and synchronizes status. |
| **Device code** | NPU (device side) | **Ascend C** | Executes specific parallel computation tasks, called [kernel functions](../../programming_guide/programming_model/ai_core_simd_programming/kernel_function.md) |

> [!NOTE] Note
> **Convenient tip**: Host code and device code can be written in the same `.asc` file, and the BiSheng compiler automatically identifies and compiles them separately, simplifying the development process.

---

## Host-Device Collaboration Process

The host side collaborates with the device by calling the **CANN Runtime API**. The typical process is as follows:

1. **Allocate memory**: Request the space required for input/output in device memory.
2. **Data copy-in**: Copy the input data from host memory to device memory (data must enter the device side before it can be accessed by the NPU).
3. **Start the NPU computation task**: Call the [kernel function](../../programming_guide/programming_model/ai_core_simd_programming/kernel_function.md) pre-written on the device side, and the NPU starts parallel computation.
4. **Synchronization and wait**: The host side waits for the NPU to finish execution to ensure that the data computation is complete (avoid reading the result before completion).
5. **Data copy-out**: Copy the computation result from device memory back to host memory for subsequent processing.

> [!NOTE] Note
> **Key point**: The host and the NPU execute asynchronously. The synchronization in step 4 is essential; otherwise, incomplete data may be read.

---

## AI Core: Core Computing Unit of the NPU

The NPU is the computing core on the device side, while the **AI Core** is the "smallest computing unit" inside the NPU. An NPU chip typically integrates multiple AI Cores, which can process different data blocks in parallel, greatly improving the overall computing throughput.

The internal structure of each AI Core is highly optimized and specifically adapted to parallel computing requirements. Its core components include:

- **Scalar unit**: Handles control flow (such as branching and loops) and address computation. It is similar to a traditional CPU core and serves as the "control core" of the AI Core.
- **Vector unit**: Executes vector operations and is the primary carrier of the SIMD and SIMT parallel models. Different Ascend chip architectures differ in their support for these two modes.
- **Cubeunit**: Specially optimized for cube multiplication-addition operations and compatible only with the SIMD mode. It serves as the "performance acceleration core" for operators such as convolution and fully connected layers in deep learning.
- **Local memory**: Used to cache the data required for computation, effectively reducing the access latency to the global device memory and improving computing efficiency.

<!-- npu="950" id1 -->

> [!NOTE] Note
> **Architecture evolution**: Before the Ascend 950PR&950DT products architecture, the AI Core **supported only SIMD**. Starting from the Ascend 950PR&950DT products architecture, the vector unit **supports both SIMD and SIMT**, providing more flexible options for different operator tasks.

<!-- end id1 -->

---

## SIMD and SIMT Parallel Models

**SIMD** (Single Instruction Multiple Data) and **SIMT** (Single Instruction Multiple Threads) are the two core parallel execution models supported by Ascend AI Core, and are also key to operator programming. They differ significantly in core logic and applicable scenarios, as compared below:

| Model | Full Name | Core Idea | Analogy | Application Scenario |
|------|------|----------|----------|----------|
| **SIMD** | Single Instruction Multiple Data | One instruction operates on multiple homogeneous data elements simultaneously | One chef stir-fries multiple identical dishes at the same time | Regular, high-density computation (cube multiplication, convolution, element-wise operations) |
| **SIMT** | Single Instruction Multiple Threads | One instruction drives multiple independent threads | One chef directs multiple assistants, each cutting their own ingredients | Irregular access, branch-intensive, sparse computation (such as sparse convolution) |

### Basic Programming Steps of a SIMD Kernel Function (Following the SPMD Model)

> **SPMD model of SIMD (Single Program, Multiple Data)**: Abstract each AI Core into a block, and use the built-in variable [block_idx](../../programming_guide/language_extension/simd_builtin_keywords.md) as the block index. Each block executes the same operator kernel function code, and the data processing range of each block is divided based on **block_idx** to implement multi-core load balancing and parallel scheduling.

1. **Tiling**: Divide the data into uniform blocks, with each AI Core responsible for one block to implement load balancing.
2. **Data copy-in**: You need to **explicitly call the data movement API** to move data from device memory to local memory.
3. **Data computation**: Call the **API corresponding to the SIMD instruction** to complete the computation. Note that data movement and computation are asynchronous, so a synchronization instruction must be inserted.
4. **Data copy-out**: You need to **explicitly call the data movement API** to write the results in local memory back to device memory.

### Basic Programming Steps of a SIMT Kernel Function (Following the SPMD Model)

> **SPMD model (Single Program, Multiple Data) of SIMT**: Each thread runs the same program, but each thread processes different data elements.

1. **Tiling**: Establish a one-to-one mapping between thread indexes and data indexes.
2. **Data copy-in**: Access device memory directly through pointers, and the hardware automatically loads data into registers. **No explicit data movement API call is required**.
3. **Data computation**: Write **scalar code** (supporting branches and loops); insert synchronization instructions when multiple threads collaborate.
4. **Data copy-out**: Write results directly back to device memory through pointers. **No explicit data movement API call is required**.

> [!NOTE] Note
> **Tip**: On the host side, you can call and run the kernel function through the `<<<>>>` syntax sugar or the CANN Runtime API.

---

## Programming Model Selection Guide

To improve the programming flexibility of vector computation in scenarios such as complex control flow and scattered memory access, the new homogeneous architecture that deeply integrates SIMD and SIMT adopts the following design: the cube computation unit continues the SIMD design, while the vector computation unit introduces SIMT capabilities on top of SIMD. This design establishes a new homogeneous programming model with **SIMD as the primary and SIMT as the secondary** approach. Under this model, the SIMD portions of cube computation and vector computation are allocated more than 90% of the computing power, delivering high performance and high computing power utilization for intensive computation. SIMT in vector computation serves as a flexibility supplement, specifically addressing irregular scenarios such as complex control flow and scattered memory access, thereby improving the efficiency of algorithm development and optimization in such scenarios.

| Programming Model | Supported Scope | Application Scenario | Chip Support |
|----------|----------|----------|----------|
| **SIMD (primary)** | Vector, cube, and fusion computation | Regular, high-density tasks (convolution, cube multiplication, element-wise transformation) | All Ascend products |
| **SIMT (secondary)** | Vector computation only | Scattered access, complex branching, sparse computation | <!-- npu="950" id2 -->Ascend 950PR & 950DT products only<!-- end id2 --> |
| **SIMD+SIMT hybrid** | Vector, cube, and fusion computation | Use SIMT to handle irregular logic and SIMD for efficient batch computation | <!-- npu="950" id3 -->Ascend 950PR & 950DT products only<!-- end id3 --> |

> [!NOTE] Note
> **Quick selection suggestions**:
> - When developing conventional high-performance operators (such as cube multiplication and element-wise operators), use the SIMD programming model. This model supports all Ascend products and offers a high energy efficiency ratio.
> - For operators involving scattered data or complex branching logic, use the SIMT programming model.

---

## Summary

The core logic of Ascend heterogeneous computing can be summarized as: **Host (CPU) scheduling + Device (NPU) execution**

- **Host** side: coordinates resources and schedules tasks.
- **Device** side: the core is the **AI Core**, which supports a new homogeneous programming model with **SIMD as the primary approach and SIMT as the secondary approach**.

**Key to operator programming**: select an appropriate programming model based on the **regularity, memory access pattern, and branch density** of the data, and fully leverage the hardware performance of the AI Core to efficiently develop high-performance Ascend C operators.
