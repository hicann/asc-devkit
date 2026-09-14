# \_\_cvta\_local\_to\_generic

<!-- md-trans-meta sourceCommit=c17b32aa8d725bcb43135ab25add5a26d62ad5c0 translatedAt=2026-08-28T07:07:54.047Z pushedAt=2026-08-28T08:52:04.028Z -->

## Applicable Products

- Ascend 950PR/Ascend 950DT: Supported
- Atlas A3 training products/Atlas A3 inference products: Not supported
- Atlas A2 training products/Atlas A2 inference products: Not supported
- Atlas 200I/500 A2 inference products: Not supported
- Atlas inference products AI Core: Not supported
- Atlas inference products Vector Core: Not supported
- Atlas training products: Not supported

## Description

Converts the address value in the stack space into the corresponding pointer and returns it.

## Function Prototype

```
void* __cvta_local_to_generic(size_t rawbits)
```

## Parameters

**Table 1**  Parameters

| Parameter | Input/Output | Description |
| --- | --- | --- |
| **rawbits** | Input | Source operand. |

## Return Value

Returns a pointer to the input stack space. This API does not verify whether the input is a valid stack space address. Examples of exception scenarios are described as follows:

| Input Scenario | Return Value |
| --- | --- |
| `rawbits` is `0` | The returned pointer is determined as 1 by `__isLocal`, but the pointer is not necessarily a stack space address that can be safely accessed. |
| `rawbits` is all 1s | The returned pointer is determined as 1 by `__isUbuf`, indicating that the classification result of an invalid `rawbits` cannot be relied upon. |

## Constraints

This API is not supported in mixed SIMD and SIMT programming scenarios.

## Required Header File

To use this API, include the **simt_api/device_functions.h** header file.

```
#include "simt_api/device_functions.h"
```

## Example

SIMT programming scenario:

```
__global__ __launch_bounds__(1024) void kernel___cvta_local_to_generic(uint32_t* dst, uint32_t* src)
{
    int idx = threadIdx.x + blockIdx.x * blockDim.x;
    uint32_t data_ptr[]={10,11};
    size_t addr = __cvta_generic_to_local(data_ptr);
    void* ptr = __cvta_local_to_generic(addr);
    dst[idx] = *(uint32_t*)ptr;
}
```

