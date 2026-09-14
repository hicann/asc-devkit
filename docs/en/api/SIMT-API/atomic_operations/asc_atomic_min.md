# asc\_atomic\_min

<!-- md-trans-meta sourceCommit=c17b32aa8d725bcb43135ab25add5a26d62ad5c0 translatedAt=2026-09-07T09:35:17.953Z pushedAt=2026-09-08T00:54:26.304Z -->

## Applicable Products

- Ascend 950PR/Ascend 950DT: Supported
- Atlas A3 training products/Atlas A3 inference products: Not supported
- Atlas A2 training products/Atlas A2 inference products: Not supported
- Atlas 200I/500 A2 inference product: Not supported
- Atlas inference products AI Core: Not supported
- Atlas inference products Vector Core: Not supported
- Atlas training products: Not supported

## Description

Performs an atomic minimum operation on Unified Buffer or Global Memory data, that is, assigns the minimum value between the data in Unified Buffer or Global Memory and the specified data to the Unified Buffer or Global Memory address.

## Function Prototype

```
inline int32_t asc_atomic_min(int32_t *address, int32_t val)
```

```
inline uint32_t asc_atomic_min(uint32_t *address, uint32_t val)
```

```
inline float asc_atomic_min(float *address, float val)
```

```
inline int64_t asc_atomic_min(int64_t *address, int64_t val)
```

```
inline uint64_t asc_atomic_min(uint64_t *address, uint64_t val)
```

```
inline half asc_atomic_min(half *address, half val)
```

```
inline bfloat16_t asc_atomic_min(bfloat16_t *address, bfloat16_t val)
```

```
inline half2 asc_atomic_min(half2 *address, half2 val)
```

```
inline bfloat16x2_t asc_atomic_min(bfloat16x2_t *address, bfloat16x2_t val)
```

## Parameters

**Table 1**  Parameters

| Parameter | Input/Output | Description |
| --- | --- | --- |
| address | Output | Address in Unified Buffer or Global Memory. |
| val | Input | Source operand. |

The memory ranges supported by different data types are described as follows:

**Table 2** Memory ranges supported by different data types

| Parameter Data Type | Supported Memory Space |
| --- | --- |
| int32_t, uint32_t, float, half, bfloat16_t, half2, bfloat16x2_t | Unified Buffer, Global Memory |
| int64_t, uint64_t | Global Memory |

## Return Value

Returns the initial data in Unified Buffer or Global Memory.

Note that due to underlying hardware constraints, the return values of the half and bfloat16_t types are inaccurate. Avoid directly using the return values of these types.

## Constraints

The atomic operation guarantees the atomicity of the read-modify-write process on the same address, but does not guarantee the execution order among multiple threads. For scenarios where the return value is used to determine the order of threads, the result may vary with thread scheduling.

## Header Files to Include

To use interfaces of types other than half, half2, bfloat16_t, and bfloat16x2_t, include the **simt\_api/device\_atomic\_functions.h** header file. To use interfaces of the half and half2 types, include the **simt\_api/asc\_fp16.h** header file. To use interfaces of the bfloat16\_t and bfloat16x2\_t types, include the **simt\_api/asc\_bf16.h** header file.

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

The example scenario is as follows: multiple threads scan a latency array and use the `asc_atomic_min` interface to write the global minimum latency to the same result address. The input parameters are described as follows:

| Name | Description |
| --- | --- |
| `latency` | Each element represents the latency value of a request. |
| `min_latency` | The minimum value result in Global Memory, initialized to a sufficiently large value before kernel launch. |
| `n` | The number of latency samples. |

The core code implementation is as follows:

-   SIMT programming scenario:

    ```cpp
    __global__ __launch_bounds__(256) void find_min_latency(uint32_t *min_latency,
                                                           uint32_t *latency,
                                                           uint32_t n)
    {
        uint32_t idx = blockIdx.x * blockDim.x + threadIdx.x;
        if (idx >= n) {
            return;
        }

        asc_atomic_min(min_latency, latency[idx]);
    }
    ```

-   SIMD and SIMT hybrid programming scenario:

    In SIMD and SIMT hybrid programming scenarios, address space qualifiers must be explicitly used to indicate the address space: \_\_gm\_\_ indicates the Global Memory space, and \_\_ubuf\_\_ indicates the Unified Buffer space.

    ```cpp
    __simt_vf__ __launch_bounds__(1024) inline void find_min_latency(__gm__ uint32_t *min_latency,
                                                                    __gm__ uint32_t *latency,
                                                                    uint32_t n)
    {
        uint32_t idx = blockIdx.x * blockDim.x + threadIdx.x;
        if (idx >= n) {
            return;
        }

        asc_atomic_min(min_latency, latency[idx]);
    }
    ```

An example of the output result is as follows:

```
latency: 31, 20, 45
min_latency: 20 // Indicates the minimum value obtained after all threads update concurrently.
```
