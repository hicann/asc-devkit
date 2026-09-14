# \_\_isGlobal

<!-- md-trans-meta sourceCommit=c17b32aa8d725bcb43135ab25add5a26d62ad5c0 translatedAt=2026-08-28T07:03:24.972Z pushedAt=2026-08-28T08:53:06.841Z -->

## Applicable Products

- Ascend 950PR/Ascend 950DT: Supported
- Atlas A3 training products/Atlas A3 inference products: Not supported
- Atlas A2 training products/Atlas A2 inference products: Not supported
- Atlas 200I/500 A2 inference product: Not supported
- Atlas inference products AI Core: Not supported
- Atlas inference products Vector Core: Not supported
- Atlas training products: Not supported

## Description

Determines whether the input pointer points to an address in Global Memory.

## Prototype

```
unsigned int __isGlobal(const void* ptr)
```

## Parameters

**Table 1** Parameters

| Parameter | Input/Output | Description |
| --- | --- | --- |
| ptr | Input | Source operand. |

## Return Value

Returns 1 if the input pointer points to an address in the Global Memory space; otherwise, returns 0.  
This interface classifies the pointer based on its address space information and does not verify whether the pointer is a valid address that can be safely accessed. A return value of 1 from `__isGlobal` only indicates that the pointer is classified as a Global Memory address, and does not mean that the address can necessarily be safely accessed. Special scenarios are described as follows:  
| input scenario | Return Value |
| --- | --- |
| `ptr` is a valid Unified Buffer pointer | 0 |
| `ptr` is a valid stack space pointer | 0 |
| `ptr` is `nullptr` | 0 |
| `ptr` is `(void*)0x1` | 0 |
| `ptr` is an all-ones address | 1 |
| `ptr` is returned by `__cvta_global_to_generic(0)` or `__cvta_global_to_generic(1)` | 0 |
| `ptr` is returned by `__cvta_global_to_generic(all-ones)` | 1 |

Therefore, `__isGlobal(ptr) == 1` alone cannot be used to determine whether `ptr` is a safely accessible Global Memory address.

## Constraints

This interface is not supported in mixed SIMD and SIMT programming scenarios.

## Header Files to Include

To use this API, include the **simt_api/device_functions.h** header file.

```
#include "simt_api/device_functions.h"
```

## Example

SIMT programming scenario:

```
__global__ __launch_bounds__(1024) void kernel_isGlobal(uint32_t* dst, uint32_t* src)
{
    int idx = threadIdx.x + blockIdx.x * blockDim.x;
    dst[idx] = __isGlobal(src + idx);
}
```

