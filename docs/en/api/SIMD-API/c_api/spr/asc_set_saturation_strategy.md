# asc_set_saturation_strategy

<!-- md-trans-meta sourceCommit=22e1e8c65e425d4c0fb07294311f2eac049ab0b3 translatedAt=2026-11-16T01:40:03.484837+00:00 pushedAt=2026-11-16T02:10:57.626Z nl=2 -->

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
<!-- end id5 -->
<!-- npu="310p" id6 -->
- Atlas inference products Vector Core: Not supported
<!-- end id6 -->
<!-- npu="910" id7 -->
- Atlas training products: Not supported
<!-- end id7 -->

## Description

The header file path is `"c_api/spr/spr.h"`.

Sets the saturation control strategy. The default is to enable the global saturation configuration:

- `asc_override_strategy::USE_API`: Configures saturation per instruction/API path.
- `asc_override_strategy::USE_GLOBAL`: Uses the global saturation configuration.

This strategy especially affects CAST saturation: only when `USE_GLOBAL` is set does `asc_saturation_mode::CAST` configured through [asc_set_saturation_flag](asc_set_saturation_flag.md) take effect. For details about the enum, see [asc_override_strategy](../defs/enum/asc_override_strategy.md).

## Prototype

```c
__aicore__ inline void asc_set_saturation_strategy(asc_override_strategy strategy)
```

## Parameters

**Table 1** Parameters

| Parameter | Input/Output | Description |
| :--- | :--- | :--- |
| strategy | Input | Saturation control strategy. For values, see [asc_override_strategy](../defs/enum/asc_override_strategy.md). |

## Return Value

None

## Pipeline Type

PIPE_S

## Constraints

- When the strategy is `USE_API`, the global CAST saturation configuration does not take effect. If global CAST saturation is required, set the strategy to `USE_GLOBAL` first or at the same time.
- Switching the strategy does not automatically modify the saturation switches of each [asc_saturation_mode](../defs/enum/asc_saturation_mode.md).

## Examples

Scenario: dynamic quantization, where overflow must be saturated to ±MAX instead of truncated. Set the strategy to `USE_GLOBAL` and enable CAST saturation first.

```cpp
// Outside SIMD_VF: save and switch the saturation strategy (before the dynamic quantization section starts).
asc_override_strategy old_strategy = asc_get_saturation_strategy();
bool old_cast_sat = asc_get_saturation_flag(asc_saturation_mode::CAST);
asc_set_saturation_strategy(asc_override_strategy::USE_GLOBAL);
asc_set_saturation_flag(asc_saturation_mode::CAST, true);

asc_copy_gm2ub_align(act_ub, act_gm, act_burst_bytes);
asc_copy_gm2ub_align(bias_ub, bias_gm, bias_burst_bytes);
asc_sync_notify(PIPE_MTE2, PIPE_V, EVENT_ID0);
asc_sync_wait(PIPE_MTE2, PIPE_V, EVENT_ID0);

// Inside the vector computation loop: after activation + bias addition, float → int32 (global CAST saturation takes effect).

// Outside SIMD_VF: restore the configuration from before entering this section.
asc_set_saturation_flag(asc_saturation_mode::CAST, old_cast_sat);
asc_set_saturation_strategy(old_strategy);
```
