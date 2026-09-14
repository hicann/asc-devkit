# SIMT Language Extension Layer C API<a name="ZH-CN_TOPIC_0000002509743873"></a>

<!-- md-trans-meta sourceCommit=e3c8c70b3f5d27f3770063ae4b535bc75d904e3e translatedAt=2026-08-26T12:28:19.799Z pushedAt=2026-09-06T07:38:57.971Z -->

SIMT programming is implemented based on the hardware capabilities of AI Core. Currently, the C API categories supported by the SIMT language extension layer are as follows:

-   [Synchronization and memory fences](../../../api/SIMT-API/sync_and_memory_fence/sync_and_memory_fence.md): Provides synchronization and memory fence APIs to resolve potential data races between threads and thread synchronization issues.
-   [Atomic operations](../../../api/SIMT-API/atomic_operations/atomic_operations_149.md): Provides a series of APIs for performing atomic operations on data in the Unified Buffer or Global Memory with specified data.
-   [Warp functions](../../../api/SIMT-API/Warp_functions/warp_functions.md): Provides a series of APIs for processing data of 32 threads within a single warp.
-   [Mathematical functions](../../../api/SIMT-API/math_functions/math_functions.md): Provides a set of function APIs for mathematical operations.
-   [Memory access functions](../../../api/SIMT-API/memory_access_functions/memory_access_functions.md): Provides Load/Store functions with Cache Hints enabled.
-   [Address space predicate functions](../../../api/SIMT-API/address_space_predicate_functions/address_space_predicate_functions.md): Provides a set of function APIs for determining the address space to which an input pointer belongs.
-   [Address space conversion functions](../../../api/SIMT-API/address_space_conversion_functions/address_space_conversion_functions.md): Provides a set of function APIs for converting between address values and pointers in different address spaces.
-   [Cooperative groups](../../../api/SIMT-API/cooperative_groups/cooperative_groups.md): Provides APIs for finer-grained cooperation between threads.
-   [Debugging APIs](../../../api/Utils-API/tuning_interface/tuning_interface.md): Related APIs used in SIMT debugging scenarios.
