# Overview<a name="ZH-CN_TOPIC_0000002518297265"></a>

<!-- md-trans-meta sourceCommit=49eda737d7d7268a9ddb02fcaba4732176bfc076 translatedAt=2026-11-09T11:04:22.143Z pushedAt=2026-11-19T06:32:19.959Z nl=2 -->

[Basic API](../../../../api/SIMD-API/basic_api/basic_api.md) abstracts hardware capabilities, exposes chip capabilities, and ensures completeness and compatibility. APIs marked as ISASI (Instruction Set Architecture Special Interface) do not guarantee compatibility across hardware versions.

**Based on their functions, the APIs are classified into the following categories**:

-   [Scalar Compute API](../../../../api/SIMD-API/basic_api/scalar_compute/scalar_compute.md), which calls the Scalar compute unit to perform computation.
-   [Memory Vector Compute API](../../../../api/SIMD-API/basic_api/memory_vector_compute/memory_vector_compute.md), which calls the Vector compute unit to perform computation.
-   [Cube Compute API](../../../../api/SIMD-API/basic_api/cube_compute_ISASI/cube_compute_ISASI.md), which calls the Cube compute unit to perform computation.
-   [Data Movement API](../../../../api/SIMD-API/basic_api/memory_vector_compute/data_move/data_move.md). Compute APIs perform computation based on Local Memory data. Therefore, data must first be moved from Global Memory to Local Memory, then processed by compute APIs, and finally moved out of Local Memory to Global Memory. The APIs that perform the data movement process are called Data Movement APIs, such as the DataCopy API.
-   [Resource Management API](../../../../api/SIMD-API/basic_api/resource_management/resource_management.md), which is used to allocate and manage memory, such as the AllocTensor and FreeTensor APIs.
-   Synchronization control API, which implements communication and synchronization between tasks, such as the **EnQue** and **DeQue** APIs. Dependencies may exist between different API instructions. As described in [Abstract Hardware Architecture](../../programming_model/ai_core_simd_programming/abstract_hardware_architecture.md), different instructions are executed asynchronously and in parallel. To ensure that instructions in different instruction queues are executed in the correct logical order, synchronization instructions must be sent to different components. The synchronization control API completes the process of sending these synchronization instructions internally. Developers do not need to focus on the internal implementation logic and can complete the task by using simple APIs.

**Based on the data operation methods, the APIs are classified into the following categories**:

-   [Continuous Compute API](./interface_classification/continuous_compute_api.md): supports computation on the first n data elements of a tensor. It performs computation on *n* consecutive data elements of the source operand and writes the results consecutively to the destination operand, addressing the continuous computation problem of one-dimensional tensors.

    ```
    Add(dst, src1, src2, n);
    ```

-   [High-Dimensional Sharding API](./interface_classification/high_dim_split_api.md): supports repeat and stride. It is a flexible compute API that provides programming capabilities fully equivalent to those of the built-in API, fully leveraging hardware advantages. It supports operations on parameters such as DataBlock Stride, Repeat Stride, and Mask for each operand.

The following figure uses vector addition as an example to illustrate the characteristics of the **continuous compute API and high-dimensional sharding API**.

**Figure 1**  Characteristics of several compute modes of the compute API<a name="zh-cn_topic_0000001762058545_fig6847134062319"></a>  
<img src="../../../figures/calc_api_feat.png" alt="Characteristics of several compute modes of the compute API" width="920px" height="602px">
