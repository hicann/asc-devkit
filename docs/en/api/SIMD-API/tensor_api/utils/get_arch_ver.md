# get_arch_ver

<!-- md-trans-meta sourceCommit=7b36bb07889b61179ceefa6904303c10d9dd5ef4 translatedAt=2026-11-19T03:25:12.216Z pushedAt=2026-11-19T05:54:06.701Z nl=2 -->

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

Obtains the current AI processor architecture version number.

## Prototype

```cpp
__aicore__ inline uint32_t get_arch_ver()
```

## Parameters

None

## Return Value

Returns the current AI processor architecture version number.

## Pipeline Type

PIPE_S

## Constraints

- None

## Example

The following example calls the `get_arch_ver` API.

```cpp
#include <cstdint>

#include "tensor_api/tensor.h"

__global__ __vector__ void get_arch_ver_kernel(__gm__ int64_t* output)
{
    using namespace asc::te;
    output[0] = static_cast<int64_t>(get_arch_ver());
}
```

