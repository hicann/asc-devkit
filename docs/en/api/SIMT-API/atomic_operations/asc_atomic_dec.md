# asc\_atomic\_dec

<!-- md-trans-meta sourceCommit=c17b32aa8d725bcb43135ab25add5a26d62ad5c0 translatedAt=2026-09-07T09:31:40.646Z pushedAt=2026-09-08T00:54:26.295Z -->

## Applicable Products

- Ascend 950PR/Ascend 950DT: Supported
- Atlas A3 training products/Atlas A3 inference products: Not supported
- Atlas A2 training products/Atlas A2 inference products: Not supported
- Atlas 200I/500 A2 inference product: Not supported
- Atlas inference products AI Core: Not supported
- Atlas inference products Vector Core: Not supported
- Atlas training products: Not supported

## Description

Atomically decrements the value at **address** in Unified Buffer or Global Memory by 1. If the value at **address** is equal to 0 or greater than the specified value **val**, **address** is assigned **val**; otherwise, the value at **address** is decremented by 1.

## Function Prototype

```
inline uint32_t asc_atomic_dec(uint32_t *address, uint32_t val)
```

```
inline uint64_t asc_atomic_dec(uint64_t *address, uint64_t val)
```

## Parameters

**Table 1**  Parameters

| Parameter | Input/Output | Description |
| --- | --- | --- |
| address | Output | Address in Unified Buffer or Global Memory. |
| val | Input | Source operand. |

The memory ranges supported by different data types are described as follows:

**Table 2**  Memory ranges supported by different data types

| parameter data type | Supported Memory Space |
| --- | --- |
| uint32_t | Unified Buffer, Global Memory |
| uint64_t | Global Memory |

## Return Value

Returns the initial data in Unified Buffer or Global Memory.

## Constraints

Atomic operations guarantee the atomicity of the read-modify-write process on the same address, but do not guarantee the execution order among multiple threads. For scenarios that rely on the return value to assign sequence numbers or slots, the sequence number corresponding to the return value is unique, but the order in which they are assigned to specific threads may vary with thread scheduling.

## Header Files to Include

To use this API, include the "simt\_api/device\_atomic\_functions.h" header file.

```
#include "simt_api/device_atomic_functions.h"
```

## Example

The example scenario is as follows: multiple threads allocate slots cyclically from high to low, using the `asc_atomic_dec` API to obtain the old count before the update. When the old value is 0, the counter wraps around to the specified upper bound `capacity - 1`. The input parameters are described as follows:

| Name | Description |
| --- | --- |
| `ticket` | Reverse ring counter in Global Memory, initialized before kernel launch. |
| `slots` | Stores the slot number obtained by each thread. |
| `capacity` | Capacity of the ring queue. |
| `n` | Number of threads that need to allocate slots. |

The core code implementation is as follows:

-   SIMT programming scenario:

    ```cpp
    __global__ __launch_bounds__(256) void allocate_reverse_ring_slot(uint32_t *ticket,
                                                                    uint32_t *slots,
                                                                    uint32_t capacity,
                                                                    uint32_t n)
    {
        uint32_t idx = blockIdx.x * blockDim.x + threadIdx.x;
        if (idx >= n) {
            return;
        }

        uint32_t old_ticket = asc_atomic_dec(ticket, capacity - 1U);
        slots[idx] = old_ticket;
    }
    ```

-   SIMD and SIMT hybrid programming scenario:

    In the SIMD and SIMT hybrid programming scenario, explicitly use address space qualifiers to indicate the address space: \_\_gm\_\_ indicates the Global Memory space, and \_\_ubuf\_\_ indicates the Unified Buffer space.

    ```cpp
    __simt_vf__ __launch_bounds__(1024) inline void allocate_reverse_ring_slot(__gm__ uint32_t *ticket,
                                                                             __gm__ uint32_t *slots,
                                                                             uint32_t capacity,
                                                                             uint32_t n)
    {
        uint32_t idx = blockIdx.x * blockDim.x + threadIdx.x;
        if (idx >= n) {
            return;
        }

        uint32_t old_ticket = asc_atomic_dec(ticket, capacity - 1U);
        slots[idx] = old_ticket;
    }
    ```

An example of the output is as follows:

```cpp
ticket before: 0
capacity: 4
n: 6
slots: 0, 3, 2, 1, 0, 3 // The order is determined by the actual atomic execution order.
ticket after: 2
```
