# get_program_counter

<!-- md-trans-meta sourceCommit=7b36bb07889b61179ceefa6904303c10d9dd5ef4 translatedAt=2026-11-19T03:27:36.320Z pushedAt=2026-11-19T05:54:06.704Z nl=2 -->

## Supported Products

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
<!-- end id5 -->
<!-- npu="310p" id6 -->
- Atlas inference products Vector Core: Not supported
<!-- end id6 -->
<!-- npu="910" id7 -->
- Atlas training products: Not supported
<!-- end id7 -->

## Function Description

The header file path is `tensor_api/tensor.h`.

Obtains the pointer to the program counter, which records the current program execution position. It is commonly used to locate the current execution position during debugging, and can also be used together with APIs such as [icache_preload](../cache_control/icache_preload.md) to use the current instruction address as the start address for prefetching.

## Prototype

```cpp
__aicore__ inline int64_t get_program_counter()
```

## Parameters

None

## Return Value

Returns the program counter pointer of the int64_t type.

## Pipeline Type

PIPE_S

## Constraints

- This API returns a 64-bit program counter value, where bits 0 to 47 are the valid address bits and the upper 16 bits are invalid. When using the value, clear the invalid upper bits with the mask `0xFFFFFFFFFFFF` (all lower 48 bits set to 1) to obtain the valid 48-bit address of the current instruction in GM.

## Example

The following example calls the `get_program_counter` API.

```cpp
#include <cstdint>

#include "tensor_api/tensor.h"

__global__ __vector__ void get_program_counter_kernel(__gm__ int64_t* output)
{
    using namespace asc::te;
    output[0] = get_program_counter();
}
```

