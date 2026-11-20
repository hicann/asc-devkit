# asc\_atomic\_cas

<!-- md-trans-meta sourceCommit=22e1e8c65e425d4c0fb07294311f2eac049ab0b3 translatedAt=2026-10-23T12:02:15.469Z pushedAt=2026-10-26T06:23:22.582Z nl=2 -->

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
<!-- @ref: asc-devkit/res/docs/zh/api/SIMT-API/atomic_operations/asc_atomic_cas_res.md#id1 -->

## Description

The header file paths are: `"simt_api/device_atomic_functions.h"` (for APIs other than the half2 and bfloat16x2\_t types), `"simt_api/asc_fp16.h"` (for the half2 type APIs), and `"simt_api/asc_bf16.h"` (for the bfloat16x2\_t type APIs).

Performs an atomic compare-and-assign operation on the value at **address** in Unified Buffer (UB) or Global Memory. If the value at **address** equals the specified value **compare**, **address** is assigned the specified value **val**; otherwise, the value at **address** remains unchanged.

## Prototype

```cpp
inline float asc_atomic_cas(float *address, float compare, float val)
```

```cpp
inline int32_t asc_atomic_cas(int32_t *address, int32_t compare, int32_t val)
```

```cpp
inline uint32_t asc_atomic_cas(uint32_t *address, uint32_t compare, uint32_t val)
```

```cpp
inline int64_t asc_atomic_cas(int64_t *address, int64_t compare, int64_t val)
```

```cpp
inline uint64_t asc_atomic_cas(uint64_t *address, uint64_t compare, uint64_t val)
```

```cpp
inline half2 asc_atomic_cas(half2 *address, half2 compare, half2 val)
```

```cpp
inline bfloat16x2_t asc_atomic_cas(bfloat16x2_t *address, bfloat16x2_t compare, bfloat16x2_t val)
```

## Parameters

**Table 1**  Parameters

| Name | Input/Output | Description |
| --- | --- | --- |
| address | Output | Address in UB or Global Memory. |
| compare | Input | Source operand, the value used for comparison. |
| val | Input | Source operand, the value used for assignment. |

The memory ranges supported by different data types are described as follows:

**Table 2**  Memory ranges supported by different data types

| Parameter Data Type | Supported Memory Space |
| --- | --- |
| int32_t, uint32_t, float, half2, bfloat16x2_t | UB,Global Memory |
| int64_t, uint64_t | Global Memory |

## Return Value

Initial data in UB or Global Memory.

## Constraints

-   Atomic operations guarantee that the read-modify-write process on the same address is atomic, but do not guarantee the execution order among multiple threads. When `asc_atomic_cas` is used to implement preemption logic, at most one thread is guaranteed to preempt successfully, but it is not guaranteed which thread preempts successfully.
-   The performance of this API is affected by the following factors. For the related principles, see [Atomic Operation Mechanism](atomic_operations_intro.md#atomic-operation-mechanism).
    -   Memory space: The access path of UB is shorter than that of Global Memory, and UB usually has lower access overhead. When the data type used supports UB (that is, int32\_t, uint32\_t, float, half2, bfloat16x2\_t), it is recommended to perform the atomic operation in UB first.
    -   Return value: This API has no corresponding performance optimization instruction. For all data types, whether the return value of this API is used in the program has little impact on API performance.
    -   Address distribution: Global Memory atomic operations are processed through L2 Cache. L2 Cache uses a 512B Cache Line as the cache management unit, and each Cache Line contains four 128B sectors. Global Memory atomic operations use a 128B Sector as the processing granularity. When destination addresses are concentrated in the same Sector, the processing efficiency is lower; when destination addresses are distributed across more Sectors, the processing efficiency is higher. Therefore, when the service allows, it is recommended to distribute the destination addresses of atomic operations across more Sectors.

## Examples

For the complete sample, see [InsertHashTable operator sample](../../../../../examples/03_simt_api/02_features/01_api_features/00_memory_access/insert_hash_table/README.md).

The simple sample scenario is as follows: multiple threads attempt to claim the same task, and the initial owner ID of the task is 0. The `asc_atomic_cas` API is used so that only one thread claims the task successfully, while other threads fail after reading a non-zero value. The input and output parameters are described as follows:

| Name | Description |
| --- | --- |
| `worker_ids` | Each element represents a worker ID. |
| `owner` | The task owner in Global Memory, where 0 indicates that no one occupies it. |
| `claim_result` | Stores whether each thread claims the task successfully. |
| `n` | The number of threads participating in the claim. |

The core code implementation is as follows:

-   SIMT programming scenario:

    ```cpp
    #include "simt_api/device_atomic_functions.h"

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

-   SIMD and SIMT mixed programming scenario:

    In SIMD and SIMT mixed programming scenarios, address space qualifiers must be used explicitly to indicate the address space: `__gm__` indicates the Global Memory memory space, and `__ubuf__` indicates the UB memory space.

    ```cpp
    #include "simt_api/device_atomic_functions.h"

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

The output result example is as follows:

```
worker_ids: 101, 102, 103
owner before: 0
owner after: one of 101/102/103 // Indicates that only one thread successfully preempts.
claim_result: only one element is 1
```
