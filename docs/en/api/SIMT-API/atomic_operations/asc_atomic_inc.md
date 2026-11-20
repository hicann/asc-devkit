# asc\_atomic\_inc

<!-- md-trans-meta sourceCommit=22e1e8c65e425d4c0fb07294311f2eac049ab0b3 translatedAt=2026-10-23T12:05:23.515Z pushedAt=2026-10-26T06:23:22.587Z nl=2 -->

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
<!-- @ref: asc-devkit/res/docs/zh/api/SIMT-API/atomic_operations/asc_atomic_inc_res.md#id1 -->

## Description

The header file path is `"simt_api/device_atomic_functions.h"`.

Performs an atomic increment-by-1 operation on the value at **address** in Unified Buffer (UB) or Global Memory. If the value at **address** is greater than or equal to the specified value **val**, **address** is set to 0; otherwise, the value at **address** is incremented by 1.

## Prototype

```cpp
inline uint32_t asc_atomic_inc(uint32_t *address, uint32_t val)
```

```cpp
inline uint64_t asc_atomic_inc(uint64_t *address, uint64_t val)
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
| uint32_t | UB,Global Memory |
| uint64_t | Global Memory |

## Return Value

Initial data in UB or Global Memory.

## Constraints

-   The atomic operation guarantees the atomicity of the read-modify-write process on the same address, but does not guarantee the execution order among multiple threads. In scenarios where the return value is used to allocate sequence numbers or slots, the sequence number corresponding to the return value is unique, but the order in which specific threads are assigned may vary with thread scheduling.
-   The performance of this API is affected by the following factors. For the related principles, see [Atomic Operation Mechanism](atomic_operations_intro.md#atomic-operation-mechanism).
    -   Memory space: The access path of UB is shorter than that of Global Memory, and usually has lower access overhead. When the data type used supports UB (that is, uint32\_t), it is recommended to perform the atomic operation in UB first.
    -   Return value: This API has no corresponding performance optimization instruction. For all data types, whether the return value of this API is used in the program has little impact on API performance.
    -   Address distribution: Global Memory atomic operations are processed through L2 Cache. L2 Cache uses a 512B Cache Line as the cache management unit, and each Cache Line contains four 128B sectors. Global Memory atomic operations use a 128B Sector as the processing granularity. When destination addresses are concentrated in the same Sector, the processing efficiency is lower; when destination addresses are distributed across more Sectors, the processing efficiency is higher. Therefore, when the service allows, it is recommended to distribute the destination addresses of atomic operations across more Sectors.

## Examples

The example scenario is as follows: multiple threads write tasks to a fixed-capacity ring buffer, and the `asc_atomic_inc` API is used to obtain a slot number that increments and automatically wraps around. The return value is the old count before the update, which can be used as the slot obtained by the current thread. The input parameters are described as follows:

| Name | Description |
| --- | --- |
| `ticket` | Ring counter in Global Memory, initialized before kernel launch. |
| `slots` | Stores the slot number obtained by each thread. |
| `capacity` | Capacity of the ring queue. |
| `n` | Number of threads that need to allocate slots. |

The core code implementation is as follows:

-   SIMT programming scenario:

    ```cpp
    #include "simt_api/device_atomic_functions.h"

    __global__ __launch_bounds__(256) void allocate_ring_slot(uint32_t *ticket,
                                                             uint32_t *slots,
                                                             uint32_t capacity,
                                                             uint32_t n)
    {
        uint32_t idx = blockIdx.x * blockDim.x + threadIdx.x;
        if (idx >= n) {
            return;
        }

        uint32_t old_ticket = asc_atomic_inc(ticket, capacity - 1U);
        slots[idx] = old_ticket;
    }
    ```

-   SIMD and SIMT mixed programming scenario:

    In SIMD and SIMT mixed programming scenarios, address space qualifiers must be used explicitly to indicate the address space: `__gm__` indicates the Global Memory memory space, and `__ubuf__` indicates the UB memory space.

    ```cpp
    #include "simt_api/device_atomic_functions.h"

    __simt_vf__ __launch_bounds__(1024) inline void allocate_ring_slot(__gm__ uint32_t *ticket,
                                                                      __gm__ uint32_t *slots,
                                                                      uint32_t capacity,
                                                                      uint32_t n)
    {
        uint32_t idx = blockIdx.x * blockDim.x + threadIdx.x;
        if (idx >= n) {
            return;
        }

        uint32_t old_ticket = asc_atomic_inc(ticket, capacity - 1U);
        slots[idx] = old_ticket;
    }
    ```

The output result example is as follows:

```
ticket before: 0
capacity: 4
n: 6
slots: 0, 1, 2, 3, 0, 1 // The order is determined by the actual atomic execution order.
ticket after: 2
```
