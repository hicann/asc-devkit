# Overview

<!-- md-trans-meta sourceCommit=7ef8ad7bdbdb0cef0f561d391759ecb8b4e4043a translatedAt=2026-10-27T04:19:11.895Z pushedAt=2026-10-27T09:21:55.801Z nl=2 -->

## API Categories

SIMT APIs are parallel computing programming APIs for AI processors that enables efficient data-parallel computation. SIMT APIs support two programming models: SIMT programming and hybrid SIMD and SIMT programming. You can first read [SIMT Programming Introduction](SIMT_programming_intro/SIMT_programming_intro.md) and [Hybrid SIMD and SIMT Programming Introduction](SIMD_SIMT_hybrid_programming_intro/SIMD_SIMT_hybrid_programming_intro.md) to understand the programming basics. The subsequent chapters describe the APIs in detail.

**Table 1**  SIMT API category list

| Category | Function |
| --- | --- |
| [Synchronization and Memory Fence](sync_and_memory_fence/sync_and_memory_fence.md) | Memory management and synchronization APIs that resolve potential data races among threads in different cores and thread synchronization issues. |
| [Atomic Operations](atomic_operations/atomic_operations.md) | A series of APIs that perform atomic operations on data in UB or Global Memory with specified data. |
| [Warp Functions](Warp_functions/Warp_functions.md) | A series of APIs for processing data of 32 threads within a single warp. |
| [Mathematical Functions](math_functions/math_functions.md) | A collection of functions for mathematical operations and conversion functions for different precisions and data types. |
| [Address Space Predicate Functions](address_space_predicate_functions/address_space_predicate_functions.md) | Determine whether an input pointer is an address in a specified space. |
| [Address Space Conversion Functions](address_space_conversion_functions/address_space_conversion_functions.md) | APIs that convert an address value in a specified address space into a pointer, or convert an input pointer into an address value in the corresponding memory space. |
| [Memory Access Functions](memory_access_functions/memory_access_functions.md) | APIs related to data loading and data caching. |
| [Cooperative Groups](cooperative_groups/cooperative_groups.md) | Provide a standard and secure mechanism for more efficient thread parallel cooperation. |
| [Tuning APIs](../Utils-API/tuning_interface/tuning_interface.md) | APIs used in SIMT VF debugging scenarios. |

## Header Files and Library Files Required for Calling APIs

After installing the CANN software package and configuring the Ascend C compilation environment, you can reference SIMT API header files when compiling SIMT source files. The public SIMT API header files are located in the `${INSTALL_DIR}/asc/include/simt_api/` directory and are referenced in code as `#include "simt_api/xxx.h"`. Replace `${INSTALL_DIR}` with the storage path of the installed CANN software. For example, when installed by the root user, the default path is `/usr/local/Ascend/cann`.

**Table 2**  Header file list

| Header File | Purpose |
| --- | --- |
| `simt_api/asc_simt.h` | Aggregate header file of basic SIMT APIs, suitable for common SIMT programming scenarios. |
| `simt_api/common_functions.h` | Header file of common SIMT capabilities, providing basic definitions such as SIMT VF invocation. |
| `simt_api/device_functions.h` | Device function header file, covering APIs such as address space predicates, address space conversion, memory access functions, and Lane-ID functions, and including the synchronization, atomic, and Warp function header files. |
| `simt_api/device_sync_functions.h` | Header file of synchronization and memory fence APIs. |
| `simt_api/device_atomic_functions.h` | Header file of atomic operation APIs. |
| `simt_api/device_warp_functions.h` | Header file of Warp-level voting, data exchange, and reduction APIs. |
| `simt_api/math_functions.h` | Header file of mathematical functions and numeric conversion functions, mainly covering data type APIs other than `half`, `half2`, `bfloat16_t`, `bfloat16x2_t`, and FP8-related types. |
| `simt_api/vector_functions.h` | Header file of short vector constructors. |
| `simt_api/cooperative_groups.h` | Header file of cooperative group APIs. |
| `simt_api/asc_fp16.h` | Header file of SIMT APIs for the `half` and `half2` types. |
| `simt_api/asc_bf16.h` | Header file of SIMT APIs for the `bfloat16_t` and `bfloat16x2_t` types. |
| `simt_api/asc_fp8.h` | Header file of SIMT APIs for the `hifloat8x2_t`, `float8_e4m3x2_t`, and `float8_e5m2x2_t` types. |

When using SIMT APIs, include the corresponding header file based on the APIs actually called.

**Table 3**  Header files required for each SIMT API category

| Category | Required Header File |
| --- | --- |
| [Synchronization and Memory Fence](sync_and_memory_fence/sync_and_memory_fence.md) | `#include "simt_api/device_sync_functions.h"` |
| [Atomic Operations](atomic_operations/atomic_operations.md) | `#include "simt_api/device_atomic_functions.h"` |
| [Warp Functions](Warp_functions/Warp_functions.md) | `#include "simt_api/device_warp_functions.h"` |
| [Mathematical Functions](math_functions/math_functions.md) | Standard library APIs of mathematical functions: `#include "simt_api/math_functions.h" `<br><br>Intrinsic APIs of mathematical functions: `#include "simt_api/device_functions.h"` |
| [Address Space Predicate Functions](address_space_predicate_functions/address_space_predicate_functions.md)<br><br>[Address Space Conversion Functions](address_space_conversion_functions/address_space_conversion_functions.md)<br><br>[Memory Access Functions](memory_access_functions/memory_access_functions.md) | `#include "simt_api/device_functions.h"` |
| [Cooperative Groups](cooperative_groups/cooperative_groups.md) | `#include "simt_api/cooperative_groups.h"` |

> [!NOTE] Note
>
> Table 3 lists the minimum required header files by API category. If `simt_api/asc_simt.h` is already included, you do not need to separately include `device_sync_functions.h`, `device_atomic_functions.h`, `device_warp_functions.h`, `math_functions.h`, `device_functions.h`, and `vector_functions.h`.

When using APIs related to data types such as `half`, `half2`, `bfloat16_t`, `bfloat16x2_t`, `hifloat8x2_t`, `float8_e4m3x2_t`, and `float8_e5m2x2_t`, include the corresponding extension header file as listed in Table 4.

**Table 4**  Header files required for using APIs with different data types

| data type | header file |
| --- | --- |
| Types other than `half`, `half2`, `bfloat16_t`, `bfloat16x2_t`, `hifloat8x2_t`, `float8_e4m3x2_t`, and `float8_e5m2x2_t` | `#include "simt_api/asc_simt.h"` |
| `half`,`half2` | `#include "simt_api/asc_fp16.h"` |
| `bfloat16_t`,`bfloat16x2_t` | `#include "simt_api/asc_bf16.h"` |
| `hifloat8x2_t`,`float8_e4m3x2_t`,`float8_e5m2x2_t` | `#include "simt_api/asc_fp8.h"` |

The SIMT API is a basic API on the kernel side, and no additional library files need to be linked when using it.
