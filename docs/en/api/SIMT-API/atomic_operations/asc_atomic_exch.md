# asc\_atomic\_exch

<!-- md-trans-meta sourceCommit=c17b32aa8d725bcb43135ab25add5a26d62ad5c0 translatedAt=2026-09-07T09:33:21.129Z pushedAt=2026-09-08T00:54:26.297Z -->

## Applicable Products

- Ascend 950PR/Ascend 950DT: Supported
- Atlas A3 training products/Atlas A3 inference products: Not supported
- Atlas A2 training products/Atlas A2 inference products: Not supported
- Atlas 200I/500 A2 inference product: Not supported
- Atlas inference products AI Core: Not supported
- Atlas inference products Vector Core: Not supported
- Atlas training products: Not supported

## Description

Performs an atomic assignment operation on a Unified Buffer or Global Memory address, that is, assigns the specified data to the Unified Buffer or Global Memory address.

## Function Prototype

```
inline float asc_atomic_exch(float *address, float val)
```

```
inline int32_t asc_atomic_exch(int32_t *address, int32_t val)
```

```
inline uint32_t asc_atomic_exch(uint32_t *address, uint32_t val)
```

```
inline int64_t asc_atomic_exch(int64_t *address, int64_t val)
```

```
inline uint64_t asc_atomic_exch(uint64_t *address, uint64_t val)
```

```
inline half2 asc_atomic_exch(half2 *address, half2 val)
```

```
inline bfloat16x2_t asc_atomic_exch(bfloat16x2_t *address, bfloat16x2_t val)
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

Atomic operations guarantee the atomicity of the read-modify-write process on the same address, but do not guarantee the execution order among multiple threads. When multiple threads write different values to the same address, the value ultimately retained depends on the serialization order of the atomic operations, which may vary with thread scheduling.

## Header Files to Include

To use interfaces of data types other than half2 and bfloat16x2\_t, include the "simt\_api/device\_atomic\_functions.h" header file. To use interfaces of the half2 data type, include the "simt\_api/asc\_fp16.h" header file. To use interfaces of the bfloat16x2\_t data type, include the "simt\_api/asc\_bf16.h" header file.

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

The example scenario is as follows: multiple threads scan fault flags, and the threads that detect a fault use the `asc_atomic_exch` interface to set the shared status to the fault state and record the old status before replacement. The input parameters are described as follows:

| Name | Description |
| --- | --- |
| `fault_flags` | Each element represents a detection result, where 0 indicates no fault and a non-zero value indicates a fault. |
| `status` | Shared state in Global Memory, where 0 indicates normal and 1 indicates a fault. |
| `old_status` | Stores the old state read by each faulty thread before the exchange. |
| `n` | Number of input elements. |

The core code implementation is as follows:

-   SIMT programming scenario:

    ```cpp
    __global__ __launch_bounds__(256) void publish_fault_status(uint32_t *status,
                                                               uint32_t *old_status,
                                                               uint32_t *fault_flags,
                                                               uint32_t n)
    {
        uint32_t idx = blockIdx.x * blockDim.x + threadIdx.x;
        if (idx >= n) {
            return;
        }

        if (fault_flags[idx] != 0U) {
            old_status[idx] = asc_atomic_exch(status, 1U);
        }
    }
    ```

-   SIMD and SIMT hybrid programming scenario:

    In SIMD and SIMT hybrid programming scenarios, address space qualifiers must be explicitly used to indicate the address space: \_\_gm\_\_ indicates the Global Memory space, and \_\_ubuf\_\_ indicates the Unified Buffer space.

    ```cpp
    __simt_vf__ __launch_bounds__(1024) inline void publish_fault_status(__gm__ uint32_t *status,
                                                                        __gm__ uint32_t *old_status,
                                                                        __gm__ uint32_t *fault_flags,
                                                                        uint32_t n)
    {
        uint32_t idx = blockIdx.x * blockDim.x + threadIdx.x;
        if (idx >= n) {
            return;
        }

        if (fault_flags[idx] != 0U) {
            old_status[idx] = asc_atomic_exch(status, 1U);
        }
    }
    ```

An example of the output result is as follows:

```cpp
fault_flags: 0, 1, 0, 1
status before: 0
status after: 1 // Indicates that at least one thread has detected a fault and published the fault status.
```

