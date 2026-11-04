# SIMT Language Extension Layer C API<a name="ZH-CN_TOPIC_0000002509743873"></a>

<!-- md-trans-meta sourceCommit=7ef8ad7bdbdb0cef0f561d391759ecb8b4e4043a translatedAt=2026-10-23T15:16:14.932Z pushedAt=2026-11-03T07:05:13.469Z nl=2 -->

SIMT programming is implemented based on the hardware capabilities of AI Core. Currently, the C API categories supported by the SIMT language extension layer are as follows:

-   [Synchronization and Memory Fence](../../../api/SIMT-API/sync_and_memory_fence/sync_and_memory_fence.md): Provides synchronization and memory fence interfaces to resolve potential data races between threads and thread synchronization issues.
-   [Atomic Operations](../../../api/SIMT-API/atomic_operations/atomic_operations.md): Provides a series of API interfaces for performing atomic operations on data in Unified Buffer (UB) or Global Memory with specified data.
-   [Warp Functions](../../../api/SIMT-API/Warp_functions/Warp_functions.md): Provides a series of API interfaces for processing data of 32 threads within a single warp.
-   [Math Functions](../../../api/SIMT-API/math_functions/math_functions.md): Provides a collection of function interfaces for processing mathematical operations.
-   [Memory Access Functions](../../../api/SIMT-API/memory_access_functions/memory_access_functions.md): Provides Load/Store functions with Cache Hints enabled.
-   [Address Space Predicate Functions](../../../api/SIMT-API/address_space_predicate_functions/address_space_predicate_functions.md): Provides a collection of function interfaces for determining the address space to which an input pointer belongs.
-   [Address Space Conversion Functions](../../../api/SIMT-API/address_space_conversion_functions/address_space_conversion_functions.md): Provides a collection of function interfaces for converting between address values and pointers in different address spaces.
-   [Cooperative Groups](../../../api/SIMT-API/cooperative_groups/cooperative_groups.md): Provides API interfaces for finer-grained inter-thread cooperation.
-   [Tuning Interface](../../../api/Utils-API/tuning_interface/tuning_interface.md): Related interfaces used in SIMT debugging scenarios.
