# Overview<a name="ZH-CN_TOPIC_0000002518297265"></a>

<!-- md-trans-meta sourceCommit=e2a2709368f65375f9b4d904990c7884199bd22e translatedAt=2026-10-23T15:23:35.549Z pushedAt=2026-11-03T07:05:13.476Z nl=2 -->

[Basic APIs](../../../../api/SIMD-API/basic_api/basic_api.md) abstract hardware capabilities, expose chip capabilities, and ensure completeness and compatibility. APIs marked as ISASI (Instruction Set Architecture Special Interface) cannot guarantee compatibility across hardware versions.

**Based on their functions, the APIs are classified as follows**:

-   [Scalar compute APIs](../../../../api/SIMD-API/basic_api/scalar_compute/scalar_compute.md), which call the Scalar compute unit to perform computation.
-   [Memory vector compute APIs](../../../../api/SIMD-API/basic_api/memory_vector_compute/memory_vector_compute.md), which call the Vector compute unit to perform computation.
-   [Cubecompute APIs](../../../../api/SIMD-API/basic_api/cube_compute_ISASI/cube_compute_ISASI.md), which call the Cube compute unit to perform computation.
-   [Data movement APIs](../../../../api/SIMD-API/basic_api/memory_vector_compute/data_move/data_move.md). Compute APIs perform computation based on Local Memory data. Therefore, data must first be moved from Global Memory to Local Memory, then processed by compute APIs, and finally moved from Local Memory back to Global Memory. The APIs that perform the movement process are called data movement APIs, such as the DataCopy API.
-   [Resource management APIs](../../../../api/SIMD-API/basic_api/resource_management/resource_management.md), which are used to allocate and manage memory, such as the AllocTensor and FreeTensor APIs.
-   Synchronization control APIs, which implement communication and synchronization between tasks, such as the EnQue and DeQue APIs. Dependencies may exist between different API instructions. As described in [Abstract Hardware Architecture](../../programming_model/ai_core_simd_programming/abstract_hardware_architecture.md), different instructions are executed asynchronously and in parallel. To ensure that instructions in different instruction queues are executed in the correct logical order, synchronization instructions must be sent to different components. The synchronization control APIs complete this process of sending synchronization instructions internally. Developers do not need to focus on the internal implementation logic and can complete the task by using simple API interfaces.

**Based on the data operation methods, the APIs are classified as follows**:

-   [Continuous Compute API](./interface_classification/continuous_compute_api.md): Supports computation on the first n data elements of a Tensor. It performs computation on n consecutive data elements of the source operand and writes the results consecutively to the destination operand, solving the continuous computation problem of one-dimensional tensors.

    ```
    Add(dst, src1, src2, n);
    ```

-   [High-Dimensional Split API](./interface_classification/high_dim_split_api.md): Supports Repeat and Stride. A flexible computation API that provides programming capabilities fully equivalent to the BuiltIn API, fully leverages hardware advantages, and supports operations on parameters such as DataBlock Stride, Repeat Stride, and Mask for each operand.

The following figure uses vector addition as an example to show the characteristics of the **Continuous Compute API and High-Dimensional Split API**.

**Figure 1**  Characteristics of several computation methods of the compute API<a name="zh-cn_topic_0000001762058545_fig6847134062319"></a>  
![](../../../figures/calc_api_feat.png "Characteristics of several computation methods of the compute API")
