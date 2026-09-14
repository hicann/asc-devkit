# asc\_atomic\_add

<!-- md-trans-meta sourceCommit=c17b32aa8d725bcb43135ab25add5a26d62ad5c0 translatedAt=2026-09-07T09:30:54.120Z pushedAt=2026-09-08T00:54:26.289Z -->

## Applicable Products

- Ascend 950PR/Ascend 950DT: Supported
- Atlas A3 training products/Atlas A3 inference products: Not supported
- Atlas A2 training products/Atlas A2 inference products: Not supported
- Atlas 200I/500 A2 inference product: Not supported
- Atlas inference products AI Core: Not supported
- Atlas inference products Vector Core: Not supported
- Atlas training products: Not supported

## Description

Performs an atomic add operation on the data in Unified Buffer or Global Memory with the specified data, that is, accumulates the specified data into the data in these memory regions.

## Function Prototype

```
inline int32_t asc_atomic_add(int32_t *address, int32_t val)
```

```
inline uint32_t asc_atomic_add(uint32_t *address, uint32_t val)
```

```
inline float asc_atomic_add(float *address, float val)
```

```
inline int64_t asc_atomic_add(int64_t *address, int64_t val)
```

```
inline uint64_t asc_atomic_add(uint64_t *address, uint64_t val)
```

```
inline half asc_atomic_add(half *address, half val)
```

```
inline bfloat16_t asc_atomic_add(bfloat16_t *address, bfloat16_t val)
```

```
inline half2 asc_atomic_add(half2 *address, half2 val)
```

```
inline bfloat16x2_t asc_atomic_add(bfloat16x2_t *address, bfloat16x2_t val)
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
| int32_t, uint32_t, float, half, bfloat16_t, half2, bfloat16x2_t | Unified Buffer, Global Memory |
| int64_t, uint64_t | Global Memory |

## Return Value

Returns the initial data in Unified Buffer or Global Memory.

Note that due to underlying hardware constraints, the return values of the half and bfloat16\_t types are inaccurate. Avoid directly using the return values of these types.

## Constraints

The atomic operation guarantees atomicity of the read-modify-write process on the same address, but does not guarantee the execution order among multiple threads. For scenarios sensitive to the order of floating-point accumulation, the result may vary with thread scheduling.

## Header Files to Include

To use interfaces of types other than half, half2, bfloat16\_t, and bfloat16x2\_t, include the "simt\_api/device\_atomic\_functions.h" header file. To use interfaces of the half and half2 types, include the "simt\_api/asc\_fp16.h" header file. To use interfaces of the bfloat16\_t and bfloat16x2\_t types, include the "simt\_api/asc\_bf16.h" header file.

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

See [Byte Order Frequency Histogram Example](https://gitcode.com/cann/asc-devkit/tree/9.1.0/examples/03_simt_api/02_features/01_api_features/02_atomic_operation/histogram), which demonstrates in detail how to use the `asc_atomic_add` interface to efficiently count the occurrence frequency of each byte value in an input byte sequence.

Simple example scenario: multiple threads scan a status array, where a non-zero status indicates an abnormal record. The `asc_atomic_add` interface is used to count the number of abnormal statuses. The input parameters are described as follows:

| Name | Description |
| --- | --- |
| `status` | Each element represents a status record, where 0 indicates normal and non-zero indicates abnormal. |
| `error_count` | Abnormal counter in Global Memory, cleared to zero before kernel launch. |
| `n` | Number of input elements. |

The core code implementation is as follows:

-   SIMT programming scenario:

    ```cpp
    __global__ __launch_bounds__(256) void count_error_status(uint32_t *error_count,
                                                         uint32_t *status,
                                                         uint32_t n)
    {
        uint32_t idx = blockIdx.x * blockDim.x + threadIdx.x;
        if (idx >= n) {
            return;
        }

        if (status[idx] != 0U) {
            asc_atomic_add(error_count, 1U);
        }
    }
    ```

-   SIMD and SIMT hybrid programming scenario:

    In SIMD and SIMT hybrid programming scenarios, address space qualifiers must be explicitly used to indicate the address space: \_\_gm\_\_ indicates the Global Memory space, and \_\_ubuf\_\_ indicates the Unified Buffer space.

    ```cpp
    __simt_vf__ __launch_bounds__(1024) inline void count_error_status(__gm__ uint32_t *error_count,
                                                         __gm__ uint32_t *status,
                                                         uint32_t n)
    {
        uint32_t idx = blockIdx.x * blockDim.x + threadIdx.x;
        if (idx >= n) {
            return;
        }

        if (status[idx] != 0U) {
            asc_atomic_add(error_count, 1U);
        }
    }
    ```

An example of the output result is as follows:

```cpp
status: 0, 2, 0, 1, 3
error_count: 3 // Indicates that 3 data items in status are non-zero.
```
