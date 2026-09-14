# asc\_atomic\_or

<!-- md-trans-meta sourceCommit=c17b32aa8d725bcb43135ab25add5a26d62ad5c0 translatedAt=2026-09-07T09:36:30.996Z pushedAt=2026-09-08T00:54:26.306Z -->

## Applicable Products

- Ascend 950PR/Ascend 950DT: Supported
- Atlas A3 training products/Atlas A3 inference products: Not supported
- Atlas A2 training products/Atlas A2 inference products: Not supported
- Atlas 200I/500 A2 inference product: Not supported
- Atlas inference products AI Core: Not supported
- Atlas inference products Vector Core: Not supported
- Atlas training products: Not supported

## Description

Performs an atomic OR (|) operation on the value at **address** in Unified Buffer or Global Memory with the specified value **val**. That is, the result of ORing the value at **address** with **val** is assigned back to Unified Buffer or Global Memory.

## Function Prototype

```
inline int32_t asc_atomic_or(int32_t *address, int32_t val)
```

```
inline uint32_t asc_atomic_or(uint32_t *address, uint32_t val)
```

```
inline int64_t asc_atomic_or(int64_t *address, int64_t val)
```

```
inline uint64_t asc_atomic_or(uint64_t *address, uint64_t val)
```

## Parameters

**Table 1**  Parameters

| Parameter | Input/Output | Description |
| --- | --- | --- |
| address | Output | Address in Unified Buffer or Global Memory. |
| val | Input | Source operand. |

The memory ranges supported by different data types are described as follows:

**Table 2**  Memory ranges supported by different data types

| parameter data type | Supported memory space |
| --- | --- |
| int32_t, uint32_t | Unified Buffer, Global Memory |
| int64_t, uint64_t | Global Memory |

## Return Value

Returns the initial data in Unified Buffer or Global Memory.

## Constraints

The atomic operation guarantees atomicity of the read-modify-write process on the same address, but does not guarantee the execution order among multiple threads. For scenarios that rely on the return value to determine the order of threads, the result may vary with thread scheduling.

## Header Files to Include

To use this API, include the "simt\_api/device\_atomic\_functions.h" header file.

```
#include "simt_api/device_atomic_functions.h"
```

## Example

The example scenario is as follows: multiple threads each detect different features, and the `asc_atomic_or` API is used to merge these features into the same shared bitmask. The input parameters are described as follows:

| Name | Description |
| --- | --- |
| `observed_flags` | Each element represents the feature bits observed by a thread. |
| `flags` | The aggregated bitmask in Global Memory, cleared before kernel launch. |
| `n` | The number of feature sources. |

The core code implementation is as follows:

-   SIMT programming scenario:

    ```cpp
    __global__ __launch_bounds__(256) void merge_observed_flags(uint32_t *flags,
                                                               uint32_t *observed_flags,
                                                               uint32_t n)
    {
        uint32_t idx = blockIdx.x * blockDim.x + threadIdx.x;
        if (idx >= n) {
            return;
        }

        asc_atomic_or(flags, observed_flags[idx]);
    }
    ```

-   SIMD and SIMT hybrid programming scenario:

    In a mixed SIMD and SIMT programming scenario, address space qualifiers must be used explicitly to indicate the address space: \_\_gm\_\_ indicates the Global Memory space, and \_\_ubuf\_\_ indicates the Unified Buffer space.

    ```cpp
    __simt_vf__ __launch_bounds__(1024) inline void merge_observed_flags(__gm__ uint32_t *flags,
                                                                        __gm__ uint32_t *observed_flags,
                                                                        uint32_t n)
    {
        uint32_t idx = blockIdx.x * blockDim.x + threadIdx.x;
        if (idx >= n) {
            return;
        }

        asc_atomic_or(flags, observed_flags[idx]);
    }
    ```

An example of the output result is as follows:

```
observed_flags: 0x1, 0x4, 0x2
flags: 0x7 // Indicates that the feature bits observed by the three threads are merged.
```
