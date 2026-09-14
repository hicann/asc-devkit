# asc\_atomic\_sub

<!-- md-trans-meta sourceCommit=c17b32aa8d725bcb43135ab25add5a26d62ad5c0 translatedAt=2026-09-07T09:36:51.838Z pushedAt=2026-09-08T00:54:26.307Z -->

## Applicable Products

- Ascend 950PR/Ascend 950DT: Supported
- Atlas A3 training products/Atlas A3 inference products: Not supported
- Atlas A2 training products/Atlas A2 inference products: Not supported
- Atlas 200I/500 A2 inference product: Not supported
- Atlas inference products AI Core: Not supported
- Atlas inference products Vector Core: Not supported
- Atlas training products: Not supported

## Description

Performs an atomic subtraction operation on data in Unified Buffer or Global Memory with the specified data, that is, subtracts the specified data from the data in these memory regions.

## Function Prototype

```
inline int32_t asc_atomic_sub(int32_t *address, int32_t val)
```

```
inline uint32_t asc_atomic_sub(uint32_t *address, uint32_t val)
```

```
inline float asc_atomic_sub(float *address, float val)
```

```
inline int64_t asc_atomic_sub(int64_t *address, int64_t val)
```

```
inline uint64_t asc_atomic_sub(uint64_t *address, uint64_t val)
```

```
inline half2 asc_atomic_sub(half2 *address, half2 val)
```

```
inline bfloat16x2_t asc_atomic_sub(bfloat16x2_t *address, bfloat16x2_t val)
```

## Parameters

**Table 1**  Parameters

| Parameter | Input/Output | Description |
| --- | --- | --- |
| address | Output | Address in Unified Buffer or Global Memory. |
| val | Input | Source operand. |

The memory ranges supported by different data types are described as follows:

**Table 2**  Memory ranges supported by different data types

| Parameter Data Type | Supported Memory Space |
| --- | --- |
| int32_t, uint32_t, float, half2, bfloat16x2_t | Unified Buffer, Global Memory |
| int64_t, uint64_t | Global Memory |

## Return Value

Returns the initial data in Unified Buffer or Global Memory.

## Constraints

The atomic operation guarantees the atomicity of the read-modify-write process on the same address, but does not guarantee the execution order among multiple threads. For scenarios involving floating-point cumulative subtraction or those sensitive to the order of return values, the result may vary with thread scheduling.

## Header Files to Include

To use interfaces of types other than half2 and bfloat16x2\_t, include the "simt\_api/device\_atomic\_functions.h" header file. To use interfaces of the half2 type, include the "simt\_api/asc\_fp16.h" header file. To use interfaces of the bfloat16x2\_t type, include the "simt\_api/asc\_bf16.h" header file.

```
#include "simt_api/device_atomic_functions.h"
```

```
#include "simt_api/asc_fp16.h"
```

```
#include "simt_api/asc_bf16.h"
```

## Example

The example scenario is as follows: multiple threads process resource request amounts and use the `asc_atomic_sub` interface to deduct the consumed quantity from the shared remaining quota. This use case assumes that the total requested amount does not exceed the initial quota to avoid unsigned underflow. The input and output parameters are described as follows:

| Name | Description |
| --- | --- |
| `requests` | Each element indicates the quota to be deducted for one resource request. |
| `remaining` | Remaining quota in Global Memory, initialized before kernel launch. |
| `n` | Number of requests. |

The core code implementation is as follows:

-   SIMT programming scenario:

    ```cpp
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

-   SIMD and SIMT hybrid programming scenario:

    In SIMD and SIMT hybrid programming scenarios, explicitly use address space qualifiers to indicate the address space: \_\_gm\_\_ indicates the Global Memory space, and \_\_ubuf\_\_ indicates the Unified Buffer space.

    ```cpp
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

An example of the output result is as follows:

```
remaining before: 100
requests: 4, 8, 3
remaining after: 85 // Indicates that the shared quota has been atomically deducted by 15 across 3 threads.
```
