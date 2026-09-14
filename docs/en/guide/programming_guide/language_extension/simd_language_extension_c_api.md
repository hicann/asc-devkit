# SIMD Language Extension Layer C API<a name="ZH-CN_TOPIC_0000002477823928"></a>

<!-- md-trans-meta sourceCommit=2bc2b38d5648dc61f632186b01ba7f36c2295b13 translatedAt=2026-08-26T12:26:01.997Z pushedAt=2026-09-06T07:17:09.219Z -->

The C API exposes the complete programming capabilities of the chip, supports memory allocation in the form of arrays, and is generally pointer-based, delivering a C programming experience consistent with industry standards.

Include the **asc\_simd.h** file to call the corresponding C APIs. Unless otherwise specified, including this header file meets the API call requirements. If there are special instructions in the [API documentation](../../../api/SIMD-API/C-API/C-API.md), follow the specific instructions in the API.

```
#include "c_api/asc_simd.h" 
```

The C API is mainly divided into the following categories:

-   Vector computation, which implements the function of calling the vector compute unit to perform computation.
-   Data movement: computation APIs perform computation based on Local Memory data. Therefore, data must first be moved from Global Memory to Local Memory, then computation APIs are used to complete the computation, and finally the data is moved from Local Memory back to Global Memory. The APIs that perform the data movement process are called data movement APIs.
-   Synchronization control: completes communication and synchronization between tasks, such as the **asc\_sync\_notify**/**asc\_sync\_wait** APIs. Dependencies may exist between different API instructions. As described in [Abstract Hardware Architecture](../programming_model/ai_core_simd_programming/abstract_hardware_architecture.md), different instructions are executed asynchronously in parallel. To ensure that instructions in different instruction queues are executed in the correct logical order, synchronization instructions must be sent to different components. The synchronization control APIs internally complete the process of sending these synchronization instructions.
-   System variables: access and obtain built-in system variables to assist computation APIs.

Computation APIs can be classified into the following categories:

-   Computation of the first *n* data items: this type of API uses a "tightly packed" data reading method during computation, that is, starting from the initial position, it sequentially and continuously obtains the required data. For example, if *N* data items need to be processed, the data is obtained starting from position 0 of the source operand and continuing sequentially to position *N*-1.
-   High-dimensional sharding computation: This type of API "skips some data" according to the configured rules. It is suitable for scenarios that require interval sampling and offers high flexibility, but additional parameters need to be configured.
-   Synchronous computation: This type of API automatically inserts synchronization operations internally, making it easier to use.
