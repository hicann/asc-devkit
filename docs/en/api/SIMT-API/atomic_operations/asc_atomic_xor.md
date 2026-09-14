# asc\_atomic\_xor

<!-- md-trans-meta sourceCommit=c17b32aa8d725bcb43135ab25add5a26d62ad5c0 translatedAt=2026-09-07T09:38:04.730Z pushedAt=2026-09-08T00:54:26.308Z -->

## Applicable Products

- Ascend 950PR/Ascend 950DT: Supported
- Atlas A3 training products/Atlas A3 inference products: Not supported
- Atlas A2 training products/Atlas A2 inference products: Not supported
- Atlas 200I/500 A2 inference product: Not supported
- Atlas inference products AI Core: Not supported
- Atlas inference products Vector Core: Not supported
- Atlas training products: Not supported

## Description

Performs an atomic XOR (^) operation on the value at **address** in Unified Buffer or Global Memory with the specified value **val**. That is, the result of XORing (^) the value at **address** with **val** is assigned to Unified Buffer or Global Memory.

## Function Prototype

```
inline int32_t asc_atomic_xor(int32_t *address, int32_t val)
```

```
inline uint32_t asc_atomic_xor(uint32_t *address, uint32_t val)
```

```
inline int64_t asc_atomic_xor(int64_t *address, int64_t val)
```

```
inline uint64_t asc_atomic_xor(uint64_t *address, uint64_t val)
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
| int32_t, uint32_t | Unified Buffer, Global Memory |
| int64_t, uint64_t | Global Memory |

## Return Value

Returns the initial data in Unified Buffer or Global Memory.

## Constraints

The atomic operation guarantees the atomicity of the read-modify-write process on the same address, but does not guarantee the execution order among multiple threads. In scenarios where the return value of the API is used to determine the order of threads, the result may vary with thread scheduling.

## Header Files to Include

To use this API, include the "simt\_api/device\_atomic\_functions.h" header file.

```
#include "simt_api/device_atomic_functions.h"
```

## Measured Verification

Measured environment: Ascend 950PR, CANN 9.0.0, `bisheng --enable-simt --npu-arch=dav-3510`.

| Coverage Item | Initial Value | Operation | Expected Result | Measured Result |
| --- | --- | --- | --- | --- |
| `uint32_t` Global Memory | `0` | 128 threads each execute `asc_atomic_xor(address, 1U)` | `0` | Passed |

This use case performs XOR an even number of times, so the final value returns to 0. It is used to verify that the atomic XOR produces consistent results when multiple threads compete to write to the same address.

## Example

The example scenario is as follows: multiple threads check whether an event is hit, and when hit, they use the `asc_atomic_xor` interface to flip a shared parity flag. A final value of 1 indicates an odd number of hits, and 0 indicates an even number of hits. The input parameters are described as follows:

| Name | Description |
| --- | --- |
| `hit` | Each element indicates whether a thread hits the event, where 1 means hit and 0 means not hit. |
| `parity` | The parity flag in Global Memory, cleared before the kernel starts. |
| `n` | The number of input elements. |

The core code implementation is as follows:

-   SIMT programming scenario:

    ```cpp
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

-   SIMD and SIMT hybrid programming scenario:

    In SIMD and SIMT hybrid programming scenarios, address space qualifiers must be explicitly used to indicate the address space: \_\_gm\_\_ indicates the Global Memory space, and \_\_ubuf\_\_ indicates the Unified Buffer space.

    ```cpp
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

An example of the output result is as follows:

```
hit: 1, 0, 1, 1
parity: 1 // There are 3 hits in total, and after an odd number of flips the result is 1.
```
