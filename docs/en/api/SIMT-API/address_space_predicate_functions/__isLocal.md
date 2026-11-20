# \_\_isLocal

<!-- md-trans-meta sourceCommit=5e880a3436a090b298e3d14711a0091a7e0348b0 translatedAt=2026-10-23T12:00:05.888Z pushedAt=2026-10-26T06:23:22.577Z nl=2 -->

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
<!-- @ref: asc-devkit/res/docs/zh/api/SIMT-API/address_space_predicate_functions/__isLocal_res.md#id1 -->

## Description

The header file path is `"simt_api/device_functions.h"`.

Determines whether the input pointer points to an address in the stack space.

## Prototype

```cpp
unsigned int __isLocal(const void* ptr)
```

## Parameters

**Table 1**  Parameters

| Name | Input/Output | Description |
| --- | --- | --- |
| ptr | Input | Source operand. |

## Return Value

If the input pointer points to a stack space address, the function returns 1; otherwise, it returns 0.  
This API classifies the pointer based on the address space information of the input pointer, and does not verify whether the pointer is a valid address that can be safely accessed. A return value of 1 from `__isLocal` only indicates that the pointer is classified as a stack space address, and does not mean that the address can definitely be safely accessed. The special scenarios are described as follows:  

| Input Scenario | Return Value |
| --- | --- |
| `ptr` is a valid Global Memory pointer | 0 |
| `ptr` is a valid UB pointer | 0 |
| `ptr` is `nullptr` | 0 |
| `ptr` is a forged low-order address, such as `(void*)0x1` | 0 |
| `ptr` is a forged all-ones address | 0 |
| `ptr` is returned by `__cvta_local_to_generic(0)` or `__cvta_local_to_generic(1)` | 1 |
| `ptr` is returned by `__cvta_local_to_generic(all ones)` | 0 |

Therefore, you cannot determine whether `ptr` is a safely accessible stack space address solely based on `__isLocal(ptr) == 1`.

## Constraints

This API is not supported in SIMD and SIMT mixed programming scenarios.

## Examples

SIMT programming scenario:

```cpp
#include "simt_api/device_functions.h"

__global__ __launch_bounds__(1024) void kernel__isLocal(uint32_t* result)
{
    int idx = threadIdx.x + blockIdx.x * blockDim.x;
    uint32_t ptr[10];
    if(__isLocal(ptr) == 1) {
       result[idx] = 1;
    } else {
       result[idx] = 0;
    }
}
```

The input and output examples are as follows:

```text
result: 1, 1, 1, 1
```
