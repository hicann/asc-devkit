# \_\_isLocal

<!-- md-trans-meta sourceCommit=c17b32aa8d725bcb43135ab25add5a26d62ad5c0 translatedAt=2026-08-28T07:03:20.971Z pushedAt=2026-08-28T08:53:06.840Z -->

## Applicable Products

- Ascend 950PR/Ascend 950DT: Supported
- Atlas A3 training products/Atlas A3 inference products: Not supported
- Atlas A2 training products/Atlas A2 inference products: Not supported
- Atlas 200I/500 A2 inference products: Not supported
- Atlas inference products AI Core: Not supported
- Atlas inference products Vector Core: Not supported
- Atlas training products: Not supported

## Description

Determines whether the input pointer points to an address in the stack space.

## Prototype

```
unsigned int __isLocal(const void* ptr)
```

## Parameters

**Table 1**  Parameters

| Parameter | Input/Output | Description |
| --- | --- | --- |
| **ptr** | Input | Source operand. |

## Return Value

Returns 1 if the input pointer points to an address in the stack space; otherwise, returns 0.  
This interface classifies the pointer based on the address space information of the input pointer and does not verify whether the pointer is a valid address that can be safely accessed. A return value of 1 from `__isLocal` only indicates that the pointer is classified as a stack space address, and does not mean that the address can definitely be safely accessed. Special scenarios are described as follows:  
| Input Scenario | Return Value |
| --- | --- |
| `ptr` is a valid Global Memory pointer | 0 |
| `ptr` is a valid Unified Buffer pointer | 0 |
| `ptr` is `nullptr` | 0 |
| `ptr` is a forged low-order address, such as `(void*)0x1` | 0 |
| `ptr` is a forged all-ones address | 0 |
| `ptr` is returned by `__cvta_local_to_generic(0)` or `__cvta_local_to_generic(1)` | 1 |
| `ptr` is returned by `__cvta_local_to_generic(all ones)` | 0 |

Therefore, `__isLocal(ptr) == 1` alone cannot be used to determine whether `ptr` is a stack space address that can be safely accessed.

## Constraints

This API is not supported in mixed SIMD and SIMT programming scenarios.

## Header File to Include

To use this API, include the **simt_api/device_functions.h** header file.

```
#include "simt_api/device_functions.h"
```

## Example

SIMT programming scenario:

```
__global__ __launch_bounds__(1024) void kernel__isLocal(uint32_t* dst, uint32_t* src)
{
    int idx = threadIdx.x + blockIdx.x * blockDim.x;
    uint32_t ptr[10];
    if(__isLocal(ptr) == 1) {
       dst[idx] = 1;
    } else {
      dst[idx] = 0;
    }
}
```

