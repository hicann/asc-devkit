# set_saturation_strategy

<!-- md-trans-meta sourceCommit=7b36bb07889b61179ceefa6904303c10d9dd5ef4 translatedAt=2026-11-19T03:17:48.255Z pushedAt=2026-11-19T05:54:06.693Z nl=2 -->

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

Sets the saturation control strategy, which enables the global saturation configuration by default. `override_strategy::use_global` indicates using the global saturation configuration, and `override_strategy::use_api` indicates configuring saturation per instruction/API path. This strategy particularly affects CAST-type saturation.

## Prototype

```cpp
__aicore__ inline void set_saturation_strategy(override_strategy strategy)
```

## Parameters

**Table 1** API parameters

| Parameter | Input/Output | Description |
| --- | --- | --- |
| strategy | Input | Saturation control strategy, of the `override_strategy` type. |

## Return Value

None

## Pipeline Type

PIPE_S

## Constraints

- When the strategy is `override_strategy::use_api`, the global CAST saturation configuration does not take effect. If global CAST saturation is required, the strategy must first or simultaneously be set to `override_strategy::use_global`.
- Switching the strategy does not automatically modify each `saturation_mode` saturation switch.

## Example

The following example calls the `set_saturation_strategy` API.

```cpp
#include "tensor_api/tensor.h"

__global__ __vector__ void set_saturation_strategy_kernel()
{
    using namespace asc::te;
    set_saturation_strategy(override_strategy::use_global);
    set_saturation_flag(saturation_mode::cast_mode, true);
}
```

