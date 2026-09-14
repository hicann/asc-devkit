# \_\_cvta\_generic\_to\_ubuf

<!-- md-trans-meta sourceCommit=c17b32aa8d725bcb43135ab25add5a26d62ad5c0 translatedAt=2026-08-28T07:07:59.332Z pushedAt=2026-08-28T08:52:04.036Z -->

## Applicable Products

- Ascend 950PR/Ascend 950DT: Supported
- Atlas A3 training products/Atlas A3 inference products: Not supported
- Atlas A2 training products/Atlas A2 inference products: Not supported
- Atlas 200I/500 A2 inference product: Not supported
- Atlas inference products AI Core: Not supported
- Atlas inference products Vector Core: Not supported
- Atlas training products: Not supported

## Description

Converts the input pointer into the address value of the Unified Buffer memory space it points to and returns it.

## Prototype

```
size_t __cvta_generic_to_ubuf(const void* ptr)
```

## Parameters

**Table 1**  Parameters

| Parameter | Input/Output | Description |
| --- | --- | --- |
| **ptr** | Input | Source operand. |

## Return Value

Returns the address value of the Unified Buffer memory space pointed to by the input pointer.
This API does not verify whether the input address is a valid address that can be safely accessed. Only when `ptr` actually points to the Unified Buffer memory space is the return value a valid Unified Buffer address value. Special scenarios are described as follows:

| Input Scenario | Return Value |
| --- | --- |
| `ptr` is a Global Memory pointer | Undefined behavior; the return value is not a valid Unified Buffer address. |
| `ptr` is `nullptr` | Returns `0x00000000fff80000`. |
| `ptr` is a stack pointer | A compilation error is reported. |

## Constraints

**SIMD** and **SIMT** mixed programming scenarios do not support the use of this API.

## Required Header Files

To use this API, include the **simt\_api/device\_functions.h** header file.

```
#include "simt_api/device_functions.h"
```

## Example

SIMT programming scenario:

```
__global__ __launch_bounds__(1024) void kernel__cvta_generic_to_ubuf(uint32_t* dst, uint32_t* src)
{
    __ubuf__ uint32_t ptr[1024];
    int idx = threadIdx.x + blockIdx.x * blockDim.x;
    dst[idx] = __cvta_generic_to_ubuf(ptr + threadIdx.x);
}
```

