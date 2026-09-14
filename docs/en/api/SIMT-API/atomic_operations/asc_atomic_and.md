# asc\_atomic\_and

<!-- md-trans-meta sourceCommit=c17b32aa8d725bcb43135ab25add5a26d62ad5c0 translatedAt=2026-09-07T09:30:59.429Z pushedAt=2026-09-08T00:54:26.294Z -->

## Applicable Products

- Ascend 950PR/Ascend 950DT: Supported
- Atlas A3 training products/Atlas A3 inference products: Not supported
- Atlas A2 training products/Atlas A2 inference products: Not supported
- Atlas 200I/500 A2 inference product: Not supported
- Atlas inference products AI Core: Not supported
- Atlas inference products Vector Core: Not supported
- Atlas training products: Not supported

## Description

Performs an atomic AND (&) operation between the value at **address** in Unified Buffer or Global Memory and the specified value **val**, that is, assigns the result of the value at **address** AND (&) **val** to Unified Buffer or Global Memory.

## Function Prototype

```
inline int32_t asc_atomic_and(int32_t *address, int32_t val)
```

```
inline uint32_t asc_atomic_and(uint32_t *address, uint32_t val)
```

```
inline int64_t asc_atomic_and(int64_t *address, int64_t val)
```

```
inline uint64_t asc_atomic_and(uint64_t *address, uint64_t val)
```

## Parameters

**Table 1**  Parameters

| Parameter | Input/Output | Description |
| --- | --- | --- |
| address | Output | Address in Unified Buffer or Global Memory. |
| val | Input | Source operand. |

The memory ranges supported by different data types are described as follows:

**Table 2**  Memory ranges supported by different data types

| parameter data type | Supported memory space |
| --- | --- |
| int32_t, uint32_t | Unified Buffer, Global Memory |
| int64_t, uint64_t | Global Memory |

## Return Value

Returns the initial data in Unified Buffer or Global Memory.

## Constraints

The atomic operation guarantees the atomicity of the read-modify-write process on the same address, but does not guarantee the execution order among multiple threads. For scenarios where the return value of the API is used to determine the order of threads, the result may vary with thread scheduling.

## Header Files to Include

To use this API, include the "simt\_api/device\_atomic\_functions.h" header file.

```
#include "simt_api/device_atomic_functions.h"
```

## Example

The example scenario is as follows: multiple threads clear certain bits in a shared status word based on their respective detection results, and the `asc_atomic_and` API is used to ensure that different threads clearing different bits do not overwrite each other's modifications. The input parameters are described as follows:

| Name | Description |
| --- | --- |
| `flags` | Shared status bits in Global Memory. |
| `clear_bits` | Each element indicates the bit to be cleared, which is converted to an AND mask inside the kernel. |
| `n` | Number of masks. |

The core code implementation is as follows:

-   SIMT programming scenario:

    ```cpp
    __global__ __launch_bounds__(256) void clear_status_bits(uint32_t *flags,
                                                            uint32_t *clear_bits,
                                                            uint32_t n)
    {
        uint32_t idx = blockIdx.x * blockDim.x + threadIdx.x;
        if (idx >= n) {
            return;
        }

        uint32_t mask = ~clear_bits[idx];
        asc_atomic_and(flags, mask);
    }
    ```

-   SIMD and SIMT hybrid programming scenario:

    In SIMD and SIMT hybrid programming scenarios, explicitly use address space qualifiers to indicate the address space: \_\_gm\_\_ indicates the Global Memory space, and \_\_ubuf\_\_ indicates the Unified Buffer space.

    ```cpp
    __simt_vf__ __launch_bounds__(1024) inline void clear_status_bits(__gm__ uint32_t *flags,
                                                                     __gm__ uint32_t *clear_bits,
                                                                     uint32_t n)
    {
        uint32_t idx = blockIdx.x * blockDim.x + threadIdx.x;
        if (idx >= n) {
            return;
        }

        uint32_t mask = ~clear_bits[idx];
        asc_atomic_and(flags, mask);
    }
    ```

An example of the output result is as follows:

```cpp
flags before: 0xF
clear_bits: 0x2, 0x4
flags after: 0x9 // Indicates that bit1 and bit2 are cleared concurrently.
```
