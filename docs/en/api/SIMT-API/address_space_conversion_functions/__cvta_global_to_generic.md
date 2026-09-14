# \_\_cvta\_global\_to\_generic

<!-- md-trans-meta sourceCommit=c17b32aa8d725bcb43135ab25add5a26d62ad5c0 translatedAt=2026-08-28T07:07:56.113Z pushedAt=2026-08-28T08:52:04.030Z -->

## Applicable Products

- Ascend 950PR/Ascend 950DT: Supported
- Atlas A3 training products/Atlas A3 inference products: Not supported
- Atlas A2 training products/Atlas A2 inference products: Not supported
- Atlas 200I/500 A2 inference products: Not supported
- Atlas inference products AI Core: Not supported
- Atlas inference products Vector Core: Not supported
- Atlas training products: Not supported

## Description

Converts an address value in the Global Memory address space into the corresponding pointer and returns it.

## Function Prototype

```
void* __cvta_global_to_generic(size_t rawbits)
```

## Parameters

**Table 1**  Parameters

| Parameter | Input/Output | Description |
| --- | --- | --- |
| **rawbits** | Input | Source operand. |

## Return Value

Returns a pointer to the Global Memory space. This API does not verify whether the input is a valid Global Memory address. Examples of exceptional scenarios are described as follows:

| Input Scenario | Return Value |
| --- | --- |
| **rawbits** is **0** | The returned pointer is determined as **0** by **__isGlobal**, **__isUbuf**, and **__isLocal**. |
| **rawbits** is all ones | The returned pointer is determined as **1** by **__isGlobal**, but the pointer is not a Global Memory address that can be safely accessed. |

## Constraints

This API is not supported in SIMD and SIMT mixed programming scenarios.

## Required Header File

To use this API, include the "simt_api/device_functions.h" header file.

```
#include "simt_api/device_functions.h"
```

## Example

SIMT programming scenario:

```
__global__ __launch_bounds__(1024) void kernel__cvta_global_to_generic(uint32_t* dst, uint32_t* src)
{
    int idx = threadIdx.x + blockIdx.x * blockDim.x;
    size_t addr = __cvta_generic_to_global(src + idx);
    void* ptr = __cvta_global_to_generic(addr);
    dst[idx] = *(uint32_t*)ptr;
}
```

