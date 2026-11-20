# \_\_cvta\_generic\_to\_ubuf

<!-- md-trans-meta sourceCommit=5e880a3436a090b298e3d14711a0091a7e0348b0 translatedAt=2026-10-23T11:47:21.007Z pushedAt=2026-10-23T11:51:24.038Z nl=2 -->

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
<!-- @ref: asc-devkit/res/docs/zh/api/SIMT-API/address_space_conversion_functions/__cvta_generic_to_ubuf_res.md#id1 -->

## Description

The header file path is `"simt_api/device_functions.h"`.

Converts the input pointer into the address value of the Unified Buffer (UB) memory space it points to and returns it.

## Prototype

```cpp
size_t __cvta_generic_to_ubuf(const void* ptr)
```

## Parameters

**Table 1**  Parameters

| Name | Input/Output | Description |
| --- | --- | --- |
| ptr | Input | Source operand. |

## Return Value

Address value of the UB memory space pointed to by the input pointer.
This API does not verify whether the input address is a valid address that can be safely accessed. The return value is a valid UB address only when `ptr` actually points to the UB memory space. Special input scenarios are described as follows:

| Input Scenario | Return Value |
| --- | --- |
| `ptr` is a Global Memory pointer | Undefined behavior; the return value is not a valid UB address. |
| `ptr` is `nullptr` | Returns `0x00000000fff80000`. |
| `ptr` is a stack space pointer | A compilation error is reported. |

## Constraints

This API is not supported in SIMD and SIMT mixed programming scenarios.

## Examples

SIMT programming scenario:

```cpp
#include "simt_api/device_functions.h"

__global__ __launch_bounds__(1024) void kernel__cvta_generic_to_ubuf(uint32_t* src, uint32_t total_length)
{
    __ubuf__ uint32_t ubuf_data[1024];
    int idx = threadIdx.x + blockIdx.x * blockDim.x;
    if (idx >= total_length) {
        return;
    }
    ubuf_data[threadIdx.x] = src[idx];
    size_t addr_val = __cvta_generic_to_ubuf(ubuf_data + threadIdx.x);
    if (blockIdx.x == 0 && threadIdx.x < 4) {
        printf("ubuf_data[%u] UB address value is: 0x%lx\n", threadIdx.x, addr_val);
    }
}
```

Input and output examples are as follows:

```text
ubuf_data[0] UB address value is: 0x0
ubuf_data[1] UB address value is: 0x4
ubuf_data[2] UB address value is: 0x8
ubuf_data[3] UB address value is: 0xc
```
