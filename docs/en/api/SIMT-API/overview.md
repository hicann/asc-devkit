# Overview

<!-- md-trans-meta sourceCommit=84d4771c758afa4058a229c863ddc58cf59e3dd3 translatedAt=2026-09-08T01:18:41.349Z -->

## API Categories

The SIMT API is a parallel computing programming interface for AI processors that enables efficient data-parallel computation. The SIMT API supports two programming models: SIMT programming, and hybrid SIMD-SIMT programming. Users should first read [SIMT Programming Introduction](SIMT_programming_intro/simt_programming_intro.md) and [SIMD and SIMT Hybrid Programming Introduction](SIMD_SIMT_hybrid_programming_intro/simd_simt_hybrid_programming_introduction.md) to understand the programming fundamentals. The subsequent chapters describe the API interfaces in detail.

**Table 1**  SIMT API category list

| Category | Function |
| --- | --- |
| [Synchronization and Memory Fence](sync_and_memory_fence/sync_and_memory_fence.md) | Memory management and synchronization interfaces that address potential data races among threads in different cores and thread synchronization issues. |
| [Atomic Operations](atomic_operations/atomic_operations_149.md) | A set of interfaces that perform atomic operations on data in the Unified Buffer or Global Memory with specified data. |
| [Warp Functions](Warp_functions/warp_functions.md) | A set of API interfaces for processing the data of 32 threads within a single warp. |
| [Math Functions](math_functions/math_functions.md) | A collection of functions for performing mathematical operations, along with conversion functions for different precisions and data types. |
| [Address Space Predicate Functions](address_space_predicate_functions/address_space_predicate_functions.md) | Determine whether an input pointer is an address in a specified address space. |
| [Address Space Conversion Functions](address_space_conversion_functions/address_space_conversion_functions.md) | Interfaces that convert an address value in a specified address space into a pointer, or convert an input pointer into an address value in the corresponding memory space. |
| [Memory Access Functions](memory_access_functions/memory_access_functions.md) | Interfaces related to data loading and data caching. |
| [Cooperative Groups](cooperative_groups/cooperative_groups.md) | Provides a standard and secure mechanism for more efficient parallel collaboration among threads. |
| [Debugging Interfaces](../Utils-API/tuning_interface/tuning_interface.md) | Interfaces used in SIMT VF debugging scenarios. |

## Header Files and Library Files Required for Calling Interfaces

After installing the CANN software package and configuring the Ascend C compilation environment, you can reference the SIMT API header files when compiling SIMT source files. The public SIMT API header files are located in the `${INSTALL_DIR}/asc/include/simt_api/` directory, and they are referenced in code using `#include "simt_api/xxx.h"`. Replace `${INSTALL_DIR}` with the file storage path after CANN software installation. For example, when installed as the root user, the default path is `/usr/local/Ascend/cann`.

**Table 2** Header file list

| Header File | Purpose |
| --- | --- |
| `simt_api/asc_simt.h` | Aggregated header file of SIMT basic APIs, suitable for general SIMT programming scenarios. |
| `simt_api/common_functions.h` | SIMT general capability header file, providing basic definitions such as SIMT VF calls. |
| `simt_api/device_functions.h` | Device function header file, covering interfaces such as address space predicates, address space conversions, memory access functions, and Lane-ID functions, and including the synchronization, atomic, and Warp function header files. |
| `simt_api/device_sync_functions.h` | Header file of synchronization and memory barrier interfaces. |
| `simt_api/device_atomic_functions.h` | Header file of atomic operation interfaces. |
| `simt_api/device_warp_functions.h` | Header file of Warp-level voting, data exchange, and reduction interfaces. |
| `simt_api/math_functions.h` | Header file of mathematical functions and numerical conversion functions, mainly covering data type interfaces other than `half`, `half2`, `bfloat16_t`, `bfloat16x2_t`, and FP8-related types. |
| `simt_api/vector_functions.h` | Header file of short vector constructor functions. |
| `simt_api/cooperative_groups.h` | Header file of collaboration group interfaces. |
| `simt_api/asc_fp16.h` | Header file of SIMT interfaces for the `half` and `half2` types. |
| `simt_api/asc_bf16.h` | Header file of SIMT interfaces for the `bfloat16_t` and `bfloat16x2_t` types. |
| `simt_api/asc_fp8.h` | Header file of SIMT interfaces for the `hifloat8x2_t`, `float8_e4m3x2_t`, and `float8_e5m2x2_t` types. |

When using the SIMT API, include the corresponding header file based on the interface actually called.

**Table 3** Header files to include for each SIMT API category

| Category | Header Files to Include |
| --- | --- |
| [Synchronization and memory fence](sync_and_memory_fence/sync_and_memory_fence.md) | `#include "simt_api/device_sync_functions.h"` |
| [Atomic Operation](atomic_operations/atomic_operations_149.md) | `#include "simt_api/device_atomic_functions.h"` |
| [Warp Function](Warp_functions/warp_functions.md) | `#include "simt_api/device_warp_functions.h"` |
| [Mathematical Function](math_functions/math_functions.md) | Standard library interfaces of mathematical functions: `#include "simt_api/math_functions.h" `<br><br>Intrinsic interfaces of mathematical functions: `#include "simt_api/device_functions.h"` |
| [Address Space Predicate Function](address_space_predicate_functions/address_space_predicate_functions.md)<br><br>[Address Space Conversion Function](address_space_conversion_functions/address_space_conversion_functions.md)<br><br>[Memory Access Function](memory_access_functions/memory_access_functions.md) | `#include "simt_api/device_functions.h"` |
| [Collaborative group](cooperative_groups/cooperative_groups.md) | `#include "simt_api/cooperative_groups.h"` |

> [!NOTE]Note
>
> Table 3 lists the minimum dependent header files by interface category. If `simt_api/asc_simt.h` is already included, you do not need to separately include `device_sync_functions.h`, `device_atomic_functions.h`, `device_warp_functions.h`, `math_functions.h`, `device_functions.h`, and `vector_functions.h`.

When using interfaces related to data types such as `half`, `half2`, `bfloat16_t`, `bfloat16x2_t`, `hifloat8x2_t`, `float8_e4m3x2_t`, and `float8_e5m2x2_t`, you also need to include the corresponding extension header files as listed in Table 4.

**Table 4** Header Files to Include When Using Interfaces of Different Data Types

| Data Type | Header Files to Include |
| --- | --- |
| Types other than `half`, `half2`, `bfloat16_t`, `bfloat16x2_t`, `hifloat8x2_t`, `float8_e4m3x2_t`, and `float8_e5m2x2_t` | `#include "simt_api/asc_simt.h"` |
| `half`, `half2` | `#include "simt_api/asc_fp16.h"` |
| `bfloat16_t`, `bfloat16x2_t` | `#include "simt_api/asc_bf16.h"` |
| `hifloat8x2_t`, `float8_e4m3x2_t`, `float8_e5m2x2_t` | `#include "simt_api/asc_fp8.h"` |

The SIMT API is a Kernel-side basic interface, and no additional library files need to be linked when using it.
