# asc\_atomic\_xor

<!-- md-trans-meta sourceCommit=22e1e8c65e425d4c0fb07294311f2eac049ab0b3 translatedAt=2026-10-23T12:13:08.491Z pushedAt=2026-10-26T06:23:22.596Z nl=2 -->

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
<!-- @ref: asc-devkit/res/docs/zh/api/SIMT-API/atomic_operations/asc_atomic_xor_res.md#id1 -->

## Description

The header file path is `"simt_api/device_atomic_functions.h"`.

Performs an atomic XOR (^) operation on the value at **address** in Unified Buffer (UB) or Global Memory with the specified value **val**. That is, the result of XORing (^) the value at **address** with **val** is assigned to UB or Global Memory.

## Prototype

```cpp
inline int32_t asc_atomic_xor(int32_t *address, int32_t val)
```

```cpp
inline uint32_t asc_atomic_xor(uint32_t *address, uint32_t val)
```

```cpp
inline int64_t asc_atomic_xor(int64_t *address, int64_t val)
```

```cpp
inline uint64_t asc_atomic_xor(uint64_t *address, uint64_t val)
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
| int32_t,uint32_t | UB,Global Memory |
| int64_t,uint64_t | Global Memory |

## Return Value

Initial data in UB or Global Memory.

## Constraints

-   Atomic operations guarantee the atomicity of the read-modify-write process on the same address, but do not guarantee the execution order among multiple threads. For scenarios where the return value of the API is used to determine the order of threads, the result may vary with thread scheduling.
-   The performance of this API is affected by the following factors. For the related principles, see [Atomic Operation Mechanism](atomic_operations_intro.md#atomic-operation-mechanism).
    -   Memory space: The access path of UB is shorter than that of Global Memory, and usually has lower access overhead. When the data type used supports UB (that is, int32\_t, uint32\_t), it is recommended to perform atomic operations in UB first.
    -   Return value: This API has no corresponding performance optimization instruction. For all data types, whether the return value of this API is used in the program has little impact on API performance.
    -   Address distribution: Global Memory atomic operations are processed through L2 Cache. L2 Cache uses a 512B Cache Line as the cache management unit, and each Cache Line contains four 128B sectors. Global Memory atomic operations use a 128B Sector as the processing granularity. When destination addresses are concentrated in the same Sector, the processing efficiency is lower; when destination addresses are distributed across more Sectors, the processing efficiency is higher. Therefore, when the service allows, it is recommended to distribute the destination addresses of atomic operations across more Sectors.

## Measured Verification

Measured environment: Ascend 950PR, CANN 9.0.0, `bisheng --enable-simt --npu-arch=dav-3510`.

| Coverage Item | Initial Value | Operation | Expected Result | Measured Result |
| --- | --- | --- | --- | --- |
| `uint32_t` Global Memory | `0` | 128 threads each execute `asc_atomic_xor(address, 1U)` | `0` | Passed |

This use case performs XOR an even number of times, so the final value returns to 0. It is used to verify that atomic XOR produces consistent results when multiple threads compete to write to the same address.

## Examples

The example scenario is as follows: multiple threads check whether an event is hit, and when it is hit, they use the `asc_atomic_xor` API to flip a shared parity flag. A final value of 1 indicates an odd number of hits, and 0 indicates an even number of hits. The input parameters are described as follows:

| Name | Description |
| --- | --- |
| `hit` | Each element indicates whether a thread hits the event, where 1 means hit and 0 means miss. |
| `parity` | The parity flag in Global Memory, cleared to zero before kernel launch. |
| `n` | The number of input elements. |

The core code implementation is as follows:

-   SIMT programming scenario:

    ```cpp
    #include "simt_api/device_atomic_functions.h"

    __global__ __launch_bounds__(256) void compute_hit_parity(uint32_t *parity,
                                                             uint32_t *hit,
                                                             uint32_t n)
    {
        uint32_t idx = blockIdx.x * blockDim.x + threadIdx.x;
        if (idx >= n) {
            return;
        }

        if (hit[idx] != 0U) {
            asc_atomic_xor(parity, 1U);
        }
    }
    ```

-   SIMD and SIMT mixed programming scenario:

    In SIMD and SIMT mixed programming scenarios, address space qualifiers must be used explicitly to indicate the address space: `__gm__` indicates the Global Memory memory space, and `__ubuf__` indicates the UB memory space.

    ```cpp
    #include "simt_api/device_atomic_functions.h"

    __simt_vf__ __launch_bounds__(1024) inline void compute_hit_parity(__gm__ uint32_t *parity,
                                                                      __gm__ uint32_t *hit,
                                                                      uint32_t n)
    {
        uint32_t idx = blockIdx.x * blockDim.x + threadIdx.x;
        if (idx >= n) {
            return;
        }

        if (hit[idx] != 0U) {
            asc_atomic_xor(parity, 1U);
        }
    }
    ```

The output result is as follows:

```
hit: 1, 0, 1, 1
parity: 1 // Three hits in total; after an odd number of flips, the result is 1.
```
