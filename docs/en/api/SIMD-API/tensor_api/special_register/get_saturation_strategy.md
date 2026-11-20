# get_saturation_strategy

<!-- md-trans-meta sourceCommit=7b36bb07889b61179ceefa6904303c10d9dd5ef4 translatedAt=2026-11-19T03:15:49.339Z pushedAt=2026-11-19T05:54:06.691Z nl=2 -->

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

Reads the current saturation control strategy, which defaults to the global saturation configuration. When the global saturation configuration is in effect, `override_strategy::use_global` is returned; when a single-instruction/API path configuration is in effect, `override_strategy::use_api` is returned. This API is commonly used to save the original configuration before temporarily switching strategies, or to perform readback verification together with [set_saturation_strategy](set_saturation_strategy.md).

## Prototype

```cpp
__aicore__ inline override_strategy get_saturation_strategy()
```

## Parameters

None

## Return Value

Current saturation control strategy, of type `override_strategy`.

## Pipeline Type

PIPE_S

## Constraints

- None

## Example

The following example calls the `get_saturation_strategy` API.

```cpp
#include "tensor_api/tensor.h"

__global__ __vector__ void get_saturation_strategy_kernel(__gm__ int8_t* output)
{
    using namespace asc::te;
    output[0] = static_cast<int8_t>(get_saturation_strategy());
}
```

