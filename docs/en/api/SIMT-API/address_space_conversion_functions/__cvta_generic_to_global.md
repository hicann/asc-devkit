# \_\_cvta\_generic\_to\_global

<!-- md-trans-meta sourceCommit=6e11244b08ff98f28bdfed28700f33faca049b68 translatedAt=2026-10-27T03:20:56.242Z pushedAt=2026-10-27T09:21:55.657Z nl=2 -->

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
<!-- @ref: asc-devkit/res/docs/zh/api/SIMT-API/address_space_conversion_functions/__cvta_generic_to_global_res.md#id1 -->

## Description

The header file path is `"simt_api/device_functions.h"`.

Converts the input pointer to the address value of the Global Memory space it points to and returns it.

## Prototype

```cpp
size_t __cvta_generic_to_global(const void* ptr)
```

## Parameters

**Table 1**  Parameters

| Parameter | Input/Output | Description |
| --- | --- | --- |
| ptr | Input | Source operand. |

## Return Value

The address value that the input pointer points to in the Global Memory space.
This API does not verify whether the input address is a valid address that can be safely accessed. The return value is a valid Global Memory address only when `ptr` actually points to the Global Memory space. Special scenarios are described as follows:

| Input Scenario | Return Value |
| --- | --- |
| `ptr` is `nullptr` | Returns `0`. |
| `ptr` is a UB or stack space pointer | A compilation error is reported.|

## Constraints

This API is not supported in SIMD and SIMT hybrid programming scenarios.

## Examples

SIMT programming scenario:

```cpp
#include "simt_api/device_functions.h"

__global__ __launch_bounds__(1024) void kernel__cvta_generic_to_global(uint32_t* src)
{
    int idx = threadIdx.x + blockIdx.x * blockDim.x;
    size_t addr_val = __cvta_generic_to_global(src + idx);
    if (idx < 4) {
        printf("The address value of src[%u] in GM memory space is: 0x%lx\n", idx, addr_val);
    }
}
```

The input and output examples are as follows:

```text
The address value of src[0] in GM memory space is: 0x120000017000
The address value of src[1] in GM memory space is: 0x120000017004
The address value of src[2] in GM memory space is: 0x120000017008
The address value of src[3] in GM memory space is: 0x12000001700c
```
