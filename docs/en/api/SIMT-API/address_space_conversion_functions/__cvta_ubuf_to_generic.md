# \_\_cvta\_ubuf\_to\_generic

<!-- md-trans-meta sourceCommit=c17b32aa8d725bcb43135ab25add5a26d62ad5c0 translatedAt=2026-08-28T07:09:42.574Z pushedAt=2026-08-28T08:52:04.038Z -->

## Applicable Products

- Ascend 950PR/Ascend 950DT: Supported
- Atlas A3 training products/Atlas A3 inference products: Not supported
- Atlas A2 training products/Atlas A2 inference products: Not supported
- Atlas 200I/500 A2 inference products: Not supported
- Atlas inference products AI Core: Not supported
- Atlas inference products Vector Core: Not supported
- Atlas training products: Not supported

## Description

Converts the address value in the Unified Buffer memory space into the corresponding pointer and returns it.

## Prototype

```
void* __cvta_ubuf_to_generic(size_t rawbits)
```

## Parameters

**Table 1**  Parameters

| Parameter | Input/Output | Description |
| --- | --- | --- |
| rawbits | Input | Source operand. |

## Return Value

Returns a pointer to the **Unified Buffer** memory space. This API does not verify whether the input is a valid **Unified Buffer** address. Examples of exception scenarios are described as follows:

| Input Scenario | Return Value |
| --- | --- |
| `rawbits` is `0` or `1` | The returned pointer is determined to be `1` by `__isUbuf`, but the pointer is not necessarily a **Unified Buffer** address that can be safely accessed. |
| `rawbits` is all 1s | The returned pointer is determined to be `0` by `__isGlobal`, `__isUbuf`, and `__isLocal`. |

## Constraints

This API is not supported in mixed **SIMD** and **SIMT** programming scenarios.

## Required Header Files

To use this API, include the **simt_api/device_functions.h** header file.

```
#include "simt_api/device_functions.h"
```

## Example

SIMT programming scenario:

```
__global__ __launch_bounds__(1024) void kernel__cvta_ubuf_to_generic(uint32_t* dst, uint32_t* src)
{
    __ubuf__ uint32_t data_ptr[1024];
    int idx = threadIdx.x + blockIdx.x * blockDim.x;
    data_ptr[threadIdx.x] = src[idx];
    size_t addr = __cvta_generic_to_ubuf(data_ptr + threadIdx.x);
    void* ptr = __cvta_ubuf_to_generic(addr);
    dst[idx] = *(uint32_t*)ptr;
}
```

