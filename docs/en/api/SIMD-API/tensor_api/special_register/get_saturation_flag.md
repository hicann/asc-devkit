# get_saturation_flag

<!-- md-trans-meta sourceCommit=7b36bb07889b61179ceefa6904303c10d9dd5ef4 translatedAt=2026-11-19T03:14:37.295Z pushedAt=2026-11-19T05:54:06.691Z nl=2 -->

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

Reads whether saturation is currently enabled by saturation mode type, and returns a boolean result.

## Prototype

```cpp
__aicore__ inline bool get_saturation_flag(saturation_mode mode)
```

## Parameters

**Table 1** Parameters

| Parameter | Input/Output | Description |
| --- | --- | --- |
| mode | Input | Saturation mode type, of type `saturation_mode`. |

## Return Value

`true` indicates that saturation is enabled in the current mode; `false` indicates disabled (for `int_mode`, it is truncated; for `float_mode`/`float8_mode`/`cast_mode`, it is non-saturated or truncated).

## Pipeline Type

PIPE_S

## Constraints

- The return value is the result at the API semantic level: `true` indicates that saturation is enabled in the mode, and `false` indicates disabled (for `int_mode`, it is truncated; for `float_mode`/`float8_mode`/`cast_mode`, it is non-saturated or truncated). For the semantic differences between modes, see Table 1 in [set_saturation_flag](set_saturation_flag.md).
- For `cast_mode`, even if this API returns `true`, the strategy must also be `override_strategy::use_global` for the CAST saturation configuration to actually take effect in precision conversion.

## Example

The following example calls the `get_saturation_flag` API.

```cpp
#include "tensor_api/tensor.h"

__global__ __vector__ void get_saturation_flag_kernel(__gm__ bool* output)
{
    using namespace asc::te;
    output[0] = get_saturation_flag(saturation_mode::float_mode);
}
```

