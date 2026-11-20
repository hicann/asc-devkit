# asc\_atomic\_sub

<!-- md-trans-meta sourceCommit=22e1e8c65e425d4c0fb07294311f2eac049ab0b3 translatedAt=2026-10-23T12:10:43.294Z pushedAt=2026-10-26T06:23:22.594Z nl=2 -->

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
<!-- @ref: asc-devkit/res/docs/zh/api/SIMT-API/atomic_operations/asc_atomic_sub_res.md#id1 -->

## Description

The header file paths are: `"simt_api/device_atomic_functions.h"` (for APIs other than the half, half2, bfloat16_t, and bfloat16x2_t types), `"simt_api/asc_fp16.h"` (for the half and half2 type APIs), and `"simt_api/asc_bf16.h"` (for the bfloat16_t and bfloat16x2_t type APIs).

Performs an atomic subtraction operation on the data in Unified Buffer (UB) or Global Memory with the specified data, that is, subtracts the specified data from the data in these memory regions.

## Prototype

```cpp
inline int32_t asc_atomic_sub(int32_t* address, int32_t val)
```

```cpp
inline uint32_t asc_atomic_sub(uint32_t* address, uint32_t val)
```

```cpp
inline float asc_atomic_sub(float* address, float val)
```

```cpp
inline int64_t asc_atomic_sub(int64_t* address, int64_t val)
```

```cpp
inline uint64_t asc_atomic_sub(uint64_t* address, uint64_t val)
```

```cpp
inline half asc_atomic_sub(half* address, half val)
```

```cpp
inline half2 asc_atomic_sub(half2* address, half2 val)
```

```cpp
inline bfloat16_t asc_atomic_sub(bfloat16_t* address, bfloat16_t val)
```

```cpp
inline bfloat16x2_t asc_atomic_sub(bfloat16x2_t* address, bfloat16x2_t val)
```

## Parameters

**Table 1**  Parameters

| Name | Input/Output | Description |
| --- | --- | --- |
| address | Output | Address in UB or Global Memory. |
| val | Input | Source operand. |

The memory ranges supported by different data types are described as follows:

**Table 2**  Memory ranges supported by different data types

| Parameter Data Type | Supported Memory Space |
| --- | --- |
| int32_t,uint32_t,float,half,bfloat16_t,half2,bfloat16x2_t | UB,Global Memory |
| int64_t,uint64_t | Global Memory |

## Return Value

The original data at the memory address before the API is executed.

Note that due to underlying hardware constraints, the return values of the half and bfloat16_t types are inaccurate, and using the return values of these types is prohibited. The half2 and bfloat16x2_t types are not subject to this restriction.

## Constraints

-   The atomic operation guarantees the atomicity of the read-modify-write process on the same address, but does not guarantee the execution order among multiple threads. For scenarios involving floating-point subtraction or those sensitive to the order of return values, the result may vary with thread scheduling.
-   The performance of this API is affected by the following factors. For the related principles, see [Atomic Operation Mechanism](atomic_operations_intro.md#atomic-operation-mechanism).
    -   Memory space: The access path of UB is shorter than that of Global Memory, and UB usually has lower access overhead. When the data type used supports UB (that is, int32\_t, uint32\_t, float, half2, bfloat16x2\_t), it is recommended to perform the atomic operation in UB first.
    -   Return value: This API has no corresponding performance optimization instruction. For all data types, whether the return value of this API is used in the program has little impact on API performance.
    -   Address distribution: Global Memory atomic operations are processed through L2 Cache. L2 Cache uses a 512B Cache Line as the cache management unit, and each Cache Line contains four 128B sectors. Global Memory atomic operations use a 128B Sector as the processing granularity. When destination addresses are concentrated in the same Sector, the processing efficiency is lower; when destination addresses are distributed across more Sectors, the processing efficiency is higher. Therefore, when the service allows, it is recommended to distribute the destination addresses of atomic operations across more Sectors.

## Examples

The example scenario is as follows: multiple threads process resource request amounts and use the `asc_atomic_sub` API to deduct the consumed amount from the shared remaining quota. This use case assumes that the total requested amount does not exceed the initial quota to avoid unsigned underflow. The input and output parameters are described as follows:

| Name | Description |
| --- | --- |
| `requests` | Each element represents the quota to be deducted for one resource request. |
| `remaining` | The remaining quota in Global Memory, initialized before kernel launch. |
| `n` | Number of requests. |

The core code implementation is as follows:

-   SIMT programming scenario:

    ```cpp
    #include "simt_api/device_atomic_functions.h"

    __global__ __launch_bounds__(256) void consume_quota(uint32_t *remaining,
                                                         uint32_t *requests,
                                                         uint32_t n)
    {
        uint32_t idx = blockIdx.x * blockDim.x + threadIdx.x;
        if (idx >= n) {
            return;
        }

        asc_atomic_sub(remaining, requests[idx]);
    }
    ```

-   SIMD and SIMT mixed programming scenario:

    In SIMD and SIMT mixed programming scenarios, address space qualifiers must be used explicitly to indicate the address space: `__gm__` indicates the Global Memory memory space, and `__ubuf__` indicates the UB memory space.

    ```cpp
    #include "simt_api/device_atomic_functions.h"

    __simt_vf__ __launch_bounds__(1024) inline void consume_quota(__gm__ uint32_t *remaining,
                                                                  __gm__ uint32_t *requests,
                                                                  uint32_t n)
    {
        uint32_t idx = blockIdx.x * blockDim.x + threadIdx.x;
        if (idx >= n) {
            return;
        }

        asc_atomic_sub(remaining, requests[idx]);
    }
    ```

The output result example is as follows:

```
remaining before: 100
requests: 4, 8, 3
remaining after: 85 // Indicates that the shared quota was atomically deducted by 15 across three threads.
```
