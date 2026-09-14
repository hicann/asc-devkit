# asc\_atomic\_cas

<!-- md-trans-meta sourceCommit=c17b32aa8d725bcb43135ab25add5a26d62ad5c0 translatedAt=2026-09-07T09:30:52.570Z pushedAt=2026-09-08T00:54:26.291Z -->

## Applicable Products

- Ascend 950PR/Ascend 950DT: Supported
- Atlas A3 training products/Atlas A3 inference products: Not supported
- Atlas A2 training products/Atlas A2 inference products: Not supported
- Atlas 200I/500 A2 inference product: Not supported
- Atlas inference products AI Core: Not supported
- Atlas inference products Vector Core: Not supported
- Atlas training products: Not supported

## Description

Performs an atomic compare-and-swap operation on the value at **address** in Unified Buffer or Global Memory. If the value at **address** equals the specified value **compare**, **address** is assigned the specified value **val**; otherwise, the value at **address** remains unchanged.

## Function Prototype

```
inline float asc_atomic_cas(float *address, float compare, float val)
```

```
inline int32_t asc_atomic_cas(int32_t *address, int32_t compare, int32_t val)
```

```
inline uint32_t asc_atomic_cas(uint32_t *address, uint32_t compare, uint32_t val)
```

```
inline int64_t asc_atomic_cas(int64_t *address, int64_t compare, int64_t val)
```

```
inline uint64_t asc_atomic_cas(uint64_t *address, uint64_t compare, uint64_t val)
```

```
inline half2 asc_atomic_cas(half2 *address, half2 compare, half2 val)
```

```
inline bfloat16x2_t asc_atomic_cas(bfloat16x2_t *address, bfloat16x2_t compare, bfloat16x2_t val)
```

## Parameters

**Table 1**  Parameters

| Parameter | Input/Output | Description |
| --- | --- | --- |
| address | Output | Address in Unified Buffer or Global Memory. |
| compare | Input | Source operand, the value to be compared. |
| val | Input | Source operand, the value to be assigned. |

The memory ranges supported by different data types are described as follows:

**Table 2** Memory ranges supported by different data types

| Parameter data type | Supported memory space |
| --- | --- |
| int32_t, uint32_t, float, half2, bfloat16x2_t | Unified Buffer, Global Memory |
| int64_t, uint64_t | Global Memory |

## Return Value

Returns the initial data in Unified Buffer or Global Memory.

## Constraints

Atomic operations guarantee the atomicity of the read-modify-write process on the same address, but do not guarantee the execution order among multiple threads. When using `asc_atomic_cas` to implement preemption logic, at most one thread is guaranteed to preempt successfully, but it is not guaranteed which thread preempts successfully.

## Header Files to Include

To use interfaces of types other than half2 and bfloat16x2\_t, include the "simt\_api/device\_atomic\_functions.h" header file. To use the half2 type interface, include the "simt\_api/asc\_fp16.h" header file. To use the bfloat16x2\_t type interface, include the "simt\_api/asc\_bf16.h" header file.

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

For the complete sample, see [InsertHashTable operator sample](https://gitcode.com/cann/asc-devkit/tree/9.1.0/examples/03_simt_api/02_features/01_api_features/00_memory_access/insert_hash_table/README.md).

The simple example scenario is as follows: multiple threads attempt to claim the same task, and the initial owner ID of the task is 0. The `asc_atomic_cas` interface is used to ensure that only one thread claims the task successfully, while other threads fail to claim it after reading a non-zero value. The input and output parameters are described as follows:

| Name | Description |
| --- | --- |
| `worker_ids` | Each element represents a worker ID. |
| `owner` | The task owner in Global Memory, where 0 indicates that the task is unoccupied. |
| `claim_result` | Stores whether each thread claims the task successfully. |
| `n` | The number of threads participating in the claim. |

The core code implementation is as follows:

-   SIMT programming scenario:

    ```cpp
    __global__ __launch_bounds__(256) void claim_task(uint32_t *owner,
                                                      uint32_t *claim_result,
                                                      uint32_t *worker_ids,
                                                      uint32_t n)
    {
        uint32_t idx = blockIdx.x * blockDim.x + threadIdx.x;
        if (idx >= n) {
            return;
        }

        uint32_t old_owner = asc_atomic_cas(owner, 0U, worker_ids[idx]);
        claim_result[idx] = (old_owner == 0U) ? 1U : 0U;
    }
    ```

-   SIMD and SIMT hybrid programming scenario:

    In the SIMD and SIMT mixed programming scenario, address space qualifiers must be explicitly used to indicate the address space: \_\_gm\_\_ indicates the Global Memory space, and \_\_ubuf\_\_ indicates the Unified Buffer space.

    ```cpp
    __simt_vf__ __launch_bounds__(1024) inline void claim_task(__gm__ uint32_t *owner,
                                                               __gm__ uint32_t *claim_result,
                                                               __gm__ uint32_t *worker_ids,
                                                               uint32_t n)
    {
        uint32_t idx = blockIdx.x * blockDim.x + threadIdx.x;
        if (idx >= n) {
            return;
        }

        uint32_t old_owner = asc_atomic_cas(owner, 0U, worker_ids[idx]);
        claim_result[idx] = (old_owner == 0U) ? 1U : 0U;
    }
    ```

An example of the output result is as follows:

```cpp
worker_ids: 101, 102, 103
owner before: 0
owner after: One of 101, 102, and 103 // Indicate that only one thread claims the task successfully.
claim_result: Only one element is 1.
```
