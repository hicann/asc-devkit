# \_\_cvta\_generic\_to\_local

<!-- md-trans-meta sourceCommit=5e880a3436a090b298e3d14711a0091a7e0348b0 translatedAt=2026-10-23T11:47:14.165Z pushedAt=2026-10-23T11:51:20.483Z nl=2 -->

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
<!-- @ref: asc-devkit/res/docs/zh/api/SIMT-API/address_space_conversion_functions/__cvta_generic_to_local_res.md#id1 -->

## Description

The header file path is `"simt_api/device_functions.h"`.

Converts the input pointer to the address value of the stack space it points to and returns it.

## Prototype

```cpp
size_t __cvta_generic_to_local(const void* ptr)
```

## Parameters

**Table 1**  Parameters

| Name | Input/Output | Description |
| --- | --- | --- |
| ptr | Input | Source operand. |

## Return Value

Address value of the stack space pointed to by the input pointer.
This API does not verify whether the input address is a valid address that can be safely accessed. The return value is a valid stack space address only when `ptr` actually points to the stack space. Special scenarios are described as follows:

| Input Scenario | Return Value |
| --- | --- |
| `ptr` is a Global Memory pointer | Undefined behavior. The return value is not a valid stack space address. |
| `ptr` is `nullptr` | Returns `0x00000000fff00000`. |
| `ptr` is a UB pointer | A compilation error occurs. |

## Constraints

This API is not supported in SIMD and SIMT mixed programming scenarios.

## Examples

SIMT programming scenario:

```cpp
#include "simt_api/device_functions.h"

__global__ __launch_bounds__(1024) void kernel__cvta_generic_to_local(uint32_t* src)
{
    int idx = threadIdx.x + blockIdx.x * blockDim.x;
    uint32_t local_data[2] = {src[idx], src[idx]};
    size_t addr_val = __cvta_generic_to_local(local_data);
    if (threadIdx.x < 4 && blockIdx.x == 4) {
        printf("threadId(%u) local_data stack space address value: 0x%lx\n", threadIdx.x, addr_val);
    }

}
```

The input and output examples are as follows:

```text
threadId(0) local_data stack space address value: 0x478
threadId(1) local_data stack space address value: 0x478
threadId(2) local_data stack space address value: 0x478
threadId(3) local_data stack space address value: 0x478
```
