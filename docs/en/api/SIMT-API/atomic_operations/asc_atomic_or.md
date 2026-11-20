# asc\_atomic\_or

<!-- md-trans-meta sourceCommit=22e1e8c65e425d4c0fb07294311f2eac049ab0b3 translatedAt=2026-10-23T12:09:22.865Z pushedAt=2026-10-26T06:23:22.592Z nl=2 -->

## Applicable Products

<!-- npu="950" id1 -->
- Ascend 950PR&950DT products: Supported
<!-- end id1 -->
<!-- npu="A3" id2 -->
- Atlas A3 products: Not supported
<!-- end id2 -->
<!-- npu="910b" id3 -->
- Atlas A2 products: Not supported
<!-- end id3 -->
<!-- npu="310b" id4 -->
- Atlas 200I/500 A2 inference products: Not supported
<!-- end id4 -->
<!-- npu="310p" id5 -->
- Atlas inference products AI Core: Not supported
- Atlas inference products Vector Core: Not supported
<!-- end id5 -->
<!-- npu="910" id6 -->
- Atlas training products: Not supported
<!-- end id6 -->
<!-- @ref: asc-devkit/res/docs/zh/api/SIMT-API/atomic_operations/asc_atomic_or_res.md#id1 -->

## Description

The header file path is `"simt_api/device_atomic_functions.h"`.

Performs an atomic OR (|) operation on the value at **address** in Unified Buffer (UB) or Global Memory with the specified value **val**, that is, assigns the result of ORing the value at **address** with **val** to UB or Global Memory.

## Prototype

```cpp
inline int32_t asc_atomic_or(int32_t *address, int32_t val)
```

```cpp
inline uint32_t asc_atomic_or(uint32_t *address, uint32_t val)
```

```cpp
inline int64_t asc_atomic_or(int64_t *address, int64_t val)
```

```cpp
inline uint64_t asc_atomic_or(uint64_t *address, uint64_t val)
```

## Parameters

**Table 1**  Parameters

| Parameter | Input/Output | Description |
| --- | --- | --- |
| address | Output | Address in UB or Global Memory. |
| val | Input | Source operand. |

The memory ranges supported by different data types are described as follows:

**Table 2**  Memory ranges supported by different data types

| Parameter Data Type | Supported Memory Space |
| --- | --- |
| int32_t, uint32_t | UB,Global Memory |
| int64_t, uint64_t | Global Memory |

## Return Value

Initial data in UB or Global Memory.

## Constraints

-   Atomic operations guarantee the atomicity of the read-modify-write process on the same address, but do not guarantee the execution order among multiple threads. For scenarios where the return value of the API is used to determine the order of threads, the result may vary with thread scheduling.
-   The performance of this API is affected by the following factors. For the related principles, see [Atomic Operation Mechanism](atomic_operations_intro.md#atomic-operation-mechanism).
    -   Memory space: The access path of UB is shorter than that of Global Memory, and usually has lower access overhead. When the data type used supports UB (that is, int32\_t, uint32\_t), it is recommended to perform atomic operations in UB first.
    -   Return value: This API has no corresponding performance optimization instruction. For all data types, whether the return value of this API is used in the program has little impact on API performance.
    -   Address distribution: Global Memory atomic operations are processed through L2 Cache. L2 Cache uses a 512B Cache Line as the cache management unit, and each Cache Line contains four 128B sectors. Global Memory atomic operations use a 128B Sector as the processing granularity. When destination addresses are concentrated in the same Sector, the processing efficiency is lower; when destination addresses are distributed across more Sectors, the processing efficiency is higher. Therefore, when the service allows, it is recommended to distribute the destination addresses of atomic operations across more Sectors.

## Examples

The example scenario is as follows: multiple threads each detect different features and use the `asc_atomic_or` API to merge these features into the same shared bitmask. The input parameters are described as follows:

| Name | Description |
| --- | --- |
| `observed_flags` | Each element represents the feature bits observed by a thread. |
| `flags` | The aggregated bitmask in Global Memory, cleared to zero before kernel launch. |
| `n` | The number of feature sources. |

The core code implementation is as follows:

-   SIMT programming scenario:

    ```cpp
    #include "simt_api/device_atomic_functions.h"

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

-   SIMD and SIMT mixed programming scenario:

    In SIMD and SIMT mixed programming scenarios, address space qualifiers must be used explicitly to indicate the address space: `__gm__` indicates the Global Memory memory space, and `__ubuf__` indicates the UB memory space.

    ```cpp
    #include "simt_api/device_atomic_functions.h"

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

The following is an example of the output:

```
observed_flags: 0x1, 0x4, 0x2
flags: 0x7 // Indicates that the feature bits observed by three threads are merged.
```
