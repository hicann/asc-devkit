# \_\_cvta\_generic\_to\_local

<!-- md-trans-meta sourceCommit=c17b32aa8d725bcb43135ab25add5a26d62ad5c0 translatedAt=2026-08-28T07:07:56.557Z pushedAt=2026-08-28T08:52:04.031Z -->

## Applicable Products

- Ascend 950PR/Ascend 950DT: Supported
- Atlas A3 training products/Atlas A3 inference products: Not supported
- Atlas A2 training products/Atlas A2 inference products: Not supported
- Atlas 200I/500 A2 inference products: Not supported
- Atlas inference products AI Core: Not supported
- Atlas inference products Vector Core: Not supported
- Atlas training products: Not supported

## Description

Converts the input pointer to the address value of the stack space it points to and returns it.

## Prototype

```
size_t __cvta_generic_to_local(const void* ptr)
```

## Parameters

**Table 1**  Parameters

| Parameter | Input/Output | Description |
| --- | --- | --- |
| **ptr** | Input | Source operand. |

## Return Value

Returns the address value of the stack space pointed to by the input pointer.  
This interface does not verify whether the input address is a valid address that can be safely accessed. Only when `ptr` actually points to the stack space is the return value a valid stack space address. Special scenarios are described as follows:  
| Input Scenario | Return Value |
| --- | --- |
| `ptr` is a Global Memory pointer | Undefined behavior; the return value is not a valid stack space address. |
| `ptr` is `nullptr` | Returns `0x00000000fff00000`. |
| `ptr` is a Unified Buffer pointer | A compilation error is reported. |

## Constraints

This API is not supported in mixed SIMD and SIMT programming scenarios.

## Required Header Files

To use this interface, include the **simt_api/device_functions.h** header file.

```
#include "simt_api/device_functions.h"
```

## Example

SIMT programming scenario:

```
__global__ __launch_bounds__(1024) void kernel__cvta_generic_to_local(uint32_t* dst, uint32_t* src)
{
    uint32_t ptr[1024];
    int idx = threadIdx.x + blockIdx.x * blockDim.x;
    dst[idx] = __cvta_generic_to_local(ptr + idx);
}
```

