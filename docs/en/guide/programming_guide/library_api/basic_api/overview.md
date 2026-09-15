# Overview<a name="ZH-CN_TOPIC_0000002518297265"></a>

<!-- md-trans-meta sourceCommit=e3c8c70b3f5d27f3770063ae4b535bc75d904e3e translatedAt=2026-08-26T12:21:35.800Z pushedAt=2026-09-06T03:00:13.870Z -->

[Basic APIs](../../../../api/SIMD-API/basic_api/basic_api.md) abstract hardware capabilities, expose chip capabilities, and ensure completeness and compatibility. APIs marked as Instruction Set Architecture Special Interface (ISASI) cannot guarantee compatibility across hardware versions.

**Based on their functions, the APIs are classified into the following categories**:

-   [Scalar computation APIs](../../../../api/SIMD-API/basic_api/scalar_compute/scalar_computation.md), which call the Scalar compute unit to perform computation.
-   [Memory vector computation APIs](../../../../api/SIMD-API/basic_api/memory_vector_compute/memory_vector_computation.md), which call the Vector compute unit to perform computation.
-   [Cube computation APIs](../../../../api/SIMD-API/basic_api/cube_compute_ISASI/cube_compute_ISASI.md), which call the Cube compute unit to perform computation.
-   [Data movement APIs](../../../../api/SIMD-API/basic_api/memory_vector_compute/data_move/data_movement.md): Computation APIs perform computation based on Local Memory data. Therefore, data must first be moved from Global Memory to Local Memory, then processed by computation APIs, and finally moved from Local Memory back to Global Memory. The APIs that perform the movement process are called data movement APIs, such as the **DataCopy** API.
-   [Resource management APIs](../../../../api/SIMD-API/basic_api/resource_management/resource_management.md), which are used to allocate and manage memory, such as the **AllocTensor** and **FreeTensor** APIs.
-   Synchronization control APIs, which implement communication and synchronization between tasks, such as the **EnQue** and **DeQue** APIs. Dependencies may exist between different API instructions. As described in [Abstract Hardware Architecture](../../programming_model/ai_core_simd_programming/abstract_hardware_architecture.md), different instructions are executed asynchronously and in parallel. To ensure that instructions in different instruction queues are executed in the correct logical order, synchronization instructions must be sent to different components. The synchronization control APIs complete this process of sending synchronization instructions internally. Developers do not need to focus on the internal implementation logic and can complete the task by using simple APIs.

**Based on how they operate on data, the APIs are classified into the following categories**:

-   Contiguous computation APIs: support computation on the first *n* data elements of a tensor. They compute the first *n* consecutive data elements of the source operand and write the results consecutively to the destination operand, solving the contiguous computation problem of one-dimensional tensors.

    ```
    Add(dst, src1, src2, n);
    ```

-   High-dimensional sharding APIs: support Repeat and Stride. These flexible computation APIs provide programming capabilities fully equivalent to those of built-in APIs, fully leverage hardware advantages, and support operations on parameters such as **DataBlock Stride**, **Repeat Stride**, and **Mask** of each operand.

The following figure uses vector addition as an example to show the characteristics of **contiguous computation APIs and high-dimensional sharding APIs**.

**Figure 1** Characteristics of several computation modes of computation APIs<a name="zh-cn_topic_0000001762058545_fig6847134062319"></a>  
![](../../../figures/characteristics_of_several_computation_modes_of_computation_apis.png "Characteristics of several computation modes of computation APIs")
