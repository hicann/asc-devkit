# get_core_id

<!-- md-trans-meta sourceCommit=7b36bb07889b61179ceefa6904303c10d9dd5ef4 translatedAt=2026-11-19T03:25:46.700Z pushedAt=2026-11-19T05:54:06.702Z nl=2 -->

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

Obtains the ID of the current AI Core. An AI Core contains one AIC and several AIVs. This API obtains the index of an AI Core relative to the other AI Cores. It is commonly used to divide tasks by core ID so that different AI Cores process different data shards.

## Prototype

```cpp
__aicore__ inline int64_t get_core_id()
```

## Parameters

None

## Return Value

Returns the ID of the current AI Core.

## Pipeline Type

PIPE_S

## Constraints

- None

## Example

The following example calls the `get_core_id` API.

```cpp
#include <cstdint>

#include "tensor_api/tensor.h"

__global__ __vector__ void get_core_id_kernel(__gm__ int64_t* output)
{
    using namespace asc::te;
    output[0] = get_core_id();
}
```

