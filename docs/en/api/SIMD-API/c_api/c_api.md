---
okftype: reference.concept
description: "C API master directory organized by the finest-grained common header files, listing API names, functional descriptions, and their owning header files line by line, covering basic definitions, utility APIs, Cube computation and its movement, Memory and Reg vector computation, vector data movement, scalar computation, atomic operations, cache control, synchronization control, and special register access, and separately listing deprecated APIs; except for experimental APIs, including c_api/asc_simd.h is sufficient to call them."
---
# API List

<!-- md-trans-meta sourceCommit=54f1d966cfcc22290ddc2c6312bbe4c6ac2a9232 translatedAt=2026-11-16T01:40:02.225345+00:00 pushedAt=2026-11-16T02:10:55.291Z nl=2 -->

C APIs are divided into the following categories by function. Click a category name to view its overview and API list. Except for experimental APIs, you can include `c_api/asc_simd.h` for overall use.

For concepts, hardware paths, and selection guidance of data movement APIs, see [Data Movement Guide](data_move_guide/data_move_guide.md).

**Table 1** C API categories

| Category | Description |
| --- | --- |
| [Basic Definitions](defs/overview.md) | Provides unions, enums, register types, macros, and constants used by C APIs. |
| [Utility APIs](utils/overview.md) | Provides APIs for system initialization, no-operation, and access to system resources and variables. |
| [Cube Computation](cube_compute/overview.md) | Provides matrix multiplication-accumulation computation and its auxiliary configuration APIs. |
| [Cube Computation Movement](cube_datamove/overview.md) | Provides APIs for moving matrix data in, moving computation results out, and auxiliary configuration required during the movement process. |
| [Memory Vector Computation](vector_compute/overview.md) | Provides vector computation APIs based on Unified Buffer (UB) data, covering arithmetic, logical, reduction, broadcast, comparison, layout transformation, sorting, aggregation, type conversion, and mask operations. |
| [Vector Data Movement](vector_datamove/overview.md) | Provides data movement APIs between Global Memory (GM), UB, and L1 Buffer, as well as auxiliary configuration APIs such as fill and multi-dimensional loop. |
| [Reg Vector Computation](reg_compute/overview.md) | Provides register-level vector computation APIs, as well as data movement APIs between UB and registers, supporting autonomous control of data movement and computation processes. |
| [Scalar Computation](scalar_compute/overview.md) | Provides scalar bit operations, type conversion, and APIs for moving scalar data in and out. |
| [Atomic Operations](atomic/overview.md) | Provides configuration APIs for atomic operations accompanying data movement, and scalar atomic operation APIs that perform atomic computation on a single element in GM. |
| [Cache Control](cache_ctrl/overview.md) | Provides APIs for prefetching, invalidation, status query of data cache and instruction cache, and configuration of scalar memory access cache modes. |
| [Synchronization Control](sync/overview.md) | Provides intra-core pipeline synchronization, as well as notification and wait APIs between cores and sub-cores. |
| [Experimental APIs](experimental/overview.md) | Provides register computation, data movement, and instruction dispatch queue status query APIs in dual-return-value form. To use them, define the `CANN_ASC_USE_EXPERIMENTAL` macro and explicitly include the corresponding header file. |
| [Special Register Access](spr/overview.md) | Provides query and configuration APIs for control registers, saturation control state, and compression state. |
| [Deprecated APIs](deprecated_interface/overview.md) | Summarizes deprecated C APIs and overloads, providing alternative APIs and migration instructions. |
