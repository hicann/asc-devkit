# \_\_cvta\_ubuf\_to\_generic

<!-- md-trans-meta sourceCommit=5e880a3436a090b298e3d14711a0091a7e0348b0 translatedAt=2026-10-23T11:53:39.664Z pushedAt=2026-10-23T11:57:49.869Z nl=2 -->

## Applicable Products

<!-- npu="950" id1 -->
- Ascend 950PR&950DT products: Supported
<!-- end id1 -->
<!-- npu="A3" id2 -->
- Atlas A3 products: Not supported
<!-- end id2 -->
<!-- npu="910b" id3 -->
- Atlas A2 products: Not supported
<!-- end id3 -->
<!-- npu="310b" id4 -->
- Atlas 200I/500 A2 inference products: Not supported
<!-- end id4 -->
<!-- npu="310p" id5 -->
- Atlas inference products AI Core: Not supported
- Atlas inference products Vector Core: Not supported
<!-- end id5 -->
<!-- npu="910" id6 -->
- Atlas training products: Not supported
<!-- end id6 -->
<!-- @ref: asc-devkit/res/docs/zh/api/SIMT-API/address_space_conversion_functions/__cvta_ubuf_to_generic_res.md#id1 -->

## Description

The header file path is `"simt_api/device_functions.h"`.

Converts the address value of the Unified Buffer (UB) space to the corresponding pointer and returns it.

## Prototype

```cpp
void* __cvta_ubuf_to_generic(size_t rawbits)
```

## Parameters

**Table 1**  Parameters

| Parameter | Input/Output | Description |
| --- | --- | --- |
| rawbits | Input | Source operand. |

## Return Value

Pointer to the UB space. This API does not verify whether the input is a valid UB address. Examples of abnormal scenarios are described as follows:

| Input Scenario | Return Value |
| --- | --- |
| rawbits is 0 or 1 | The returned pointer is determined as 1 by __isUbuf(), but the pointer is not necessarily a UB address that can be safely accessed. |
| rawbits is all 1s | The returned pointer is determined as 0 by __isGlobal(), __isUbuf(), and __isLocal(). |

## Constraints

This API is not supported in SIMD and SIMT hybrid programming scenarios.

## Examples

SIMT programming scenario:

```cpp
#include "simt_api/device_functions.h"

__global__ __launch_bounds__(1024) void kernel__cvta_ubuf_to_generic(uint32_t* result, uint64_t addr_val)
{
    int idx = threadIdx.x + blockIdx.x * blockDim.x;
    uint32_t* ptr = static_cast<uint32_t*>(__cvta_ubuf_to_generic(addr_val)); // addr_val: UB address value.
    result[idx] = ptr[0];
}
```

The input and output examples are as follows:

```text
addr_val: 0x0 (pointing to data 11), 0x4 (pointing to data 22), 0x8 (pointing to data 33), 0xc (pointing to data 44)
result: 11, 22, 33, 44
```
