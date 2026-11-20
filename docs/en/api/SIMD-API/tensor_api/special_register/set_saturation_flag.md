# set_saturation_flag

<!-- md-trans-meta sourceCommit=7b36bb07889b61179ceefa6904303c10d9dd5ef4 translatedAt=2026-11-19T03:17:50.972Z pushedAt=2026-11-19T05:54:06.694Z nl=2 -->

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

Sets the corresponding saturation switch by saturation mode type. `enable_sat` indicates whether to enable saturation in the API semantics. The API internally processes different semantics by mode.

The meaning of `enable_sat` in each mode is as follows:

**Table 1** Saturation modes and enable_sat semantics

| saturation_mode | enable_sat=true | enable_sat=false | Description |
| --- | --- | --- | --- |
| float_mode | Saturation | Non-saturation | Saturation control for floating-point computation and some floating-point precision conversion scenarios. The default is non-saturation. |
| float8_mode | Saturation | Non-saturation | Saturation control for fp8-related scenarios. The default is saturation. |
| int_mode | Saturation | Truncation | Saturation/truncation control for integer computation overflow. The default is non-saturation. |
| cast_mode | Saturation | Truncation | Precision conversion saturation control for float-to-integer or integer-to-integer conversion. The default is saturation. It takes effect only when the strategy is `override_strategy::use_global`. |

## Prototype

```cpp
__aicore__ inline void set_saturation_flag(saturation_mode mode, bool enable_sat)
```

## Parameters

**Table 2** Parameters

| Parameter | Input/Output | Description |
| --- | --- | --- |
| mode | Input | Saturation mode type, of type `saturation_mode`. For values, see **Table 1**. |
| enable_sat | Input | Whether to enable saturation. `true` enables saturation, and `false` disables saturation (truncation for `int_mode`; non-saturation or truncation for `float_mode`/`float8_mode`/`cast_mode`, see **Table 1**). |

## Return Value

None

## Pipeline Type

PIPE_S

## Constraints

- `enable_sat` is an API semantic parameter. The same Boolean value has different disable semantics in different modes (`int_mode` is truncation, and `float_mode`/`float8_mode`/`cast_mode` are non-saturation or truncation). Refer to **Table 1**.
- Before enabling saturation mode for `saturation_mode::float_mode`, enable the global saturation configuration through [set_saturation_strategy](set_saturation_strategy.md) for it to take effect.
- When `mode` is `saturation_mode::float8_mode`, first enable saturation mode for `saturation_mode::float_mode` for it to take effect.
- When `mode` is `saturation_mode::cast_mode`, also set the saturation strategy to `override_strategy::use_global` for CAST to take effect.
- This API only modifies the saturation configuration of the corresponding mode and does not affect other modes.

## Example

The following example calls the `set_saturation_flag` API.

```cpp
#include "tensor_api/tensor.h"

__global__ __vector__ void set_saturation_flag_kernel()
{
    using namespace asc::te;
    set_saturation_strategy(override_strategy::use_global);
    set_saturation_flag(saturation_mode::float_mode, true);
}
```

