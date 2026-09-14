# \_\_cvta\_generic\_to\_global

<!-- md-trans-meta sourceCommit=c17b32aa8d725bcb43135ab25add5a26d62ad5c0 translatedAt=2026-08-28T07:07:57.283Z pushedAt=2026-08-28T08:52:04.033Z -->

## Applicable Products

- Ascend 950PR/Ascend 950DT: Supported
- Atlas A3 training products/Atlas A3 inference products: Not supported
- Atlas A2 training products/Atlas A2 inference products: Not supported
- Atlas 200I/500 A2 inference products: Not supported
- Atlas inference products AI Core: Not supported
- Atlas inference products Vector Core: Not supported
- Atlas training products: Not supported

## Description

Converts the input pointer to the address value of the Global Memory memory space it points to and returns it.

## Prototype

```
size_t __cvta_generic_to_global(const void* ptr)
```

## Parameters

**Table 1**  Parameters

| Parameter | Input/Output | Description |
| --- | --- | --- |
| ptr | Input | Source operand. |

## Return Value

Returns the address value of the input pointer pointing to the Global Memory memory space.  
This interface does not verify whether the input address is a valid address that can be safely accessed. Only when `ptr` actually points to the Global Memory memory space is the return value a valid Global Memory address value. Special scenarios are described as follows:  
| Input Scenario | Return Value |
| --- | --- |
| `ptr` is `nullptr` | Returns `0`. |
| `ptr` is a Unified Buffer or stack space pointer | Reports an error at the compilation stage. |

## Constraints

The use of this API is not supported in mixed SIMD and SIMT programming scenarios.

## Header Files to Include

To use this API, include the **simt_api/device_functions.h** header file.

```
#include "simt_api/device_functions.h"
```

## Example

SIMT programming scenario:

```
__global__ __launch_bounds__(1024) void kernel__cvta_generic_to_global(uint32_t* dst, uint32_t* src)
{
    int idx = threadIdx.x + blockIdx.x * blockDim.x;
    dst[idx] = __cvta_generic_to_global(src + idx);
}
```

