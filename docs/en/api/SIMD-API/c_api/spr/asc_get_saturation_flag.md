---
okftype: reference.api
description: "Queries whether saturation is currently enabled by saturation mode type and returns a Boolean result. In CAST mode, whether saturation actually takes effect must also be determined in combination with the saturation strategy."
tag:
  - roles: [query]
dependencies:
  - path: docs/zh/api/SIMD-API/c_api/defs/enum/asc_saturation_mode.md
    kind: related
  - path: docs/zh/api/SIMD-API/c_api/spr/asc_set_saturation_flag.md
    kind: related
    condition: "Used together with the setting API for readback"
version: ">=9.2.0"
version_changes: []
---
# asc_get_saturation_flag

<!-- md-trans-meta sourceCommit=54f1d966cfcc22290ddc2c6312bbe4c6ac2a9232 translatedAt=2026-11-16T01:40:03.469316+00:00 pushedAt=2026-11-16T02:10:57.579Z nl=2 -->

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

Reads whether saturation is currently enabled according to the saturation mode type and returns a Boolean result.

## Prototype

```c
__aicore__ inline bool asc_get_saturation_flag(asc_saturation_mode saturation_mode)
```

## Parameters

**Table 1** Parameters

| Parameter | Input/Output | Description |
| :--- | :--- | :--- |
| saturation_mode | Input | Saturation mode type. For values, see [asc_saturation_mode](../defs/enum/asc_saturation_mode.md). |

## Return Value

`true` indicates that saturation is currently enabled in this mode; `false` indicates that it is not enabled (truncation for INT; non-saturation or truncation for FLOAT/FLOAT8/CAST).

## Pipeline Type

PIPE_S

## Constraints

- The return value is the API semantic result: `true` indicates that saturation is enabled in this mode, and `false` indicates that it is not enabled (truncation for INT; non-saturation or truncation for FLOAT/FLOAT8/CAST). For semantic differences between modes, see Table 1 in [asc_set_saturation_flag](asc_set_saturation_flag.md).
- For the `CAST` mode, even if this API returns `true`, the saturation strategy must also be `USE_GLOBAL` for the CAST saturation configuration to take effect in precision conversion. See [asc_set_saturation_strategy](asc_set_saturation_strategy.md).

## Examples

Scenario: dynamic quantization, where overflow must be saturated to ±MAX instead of truncated. Set the strategy to `USE_GLOBAL` and enable CAST saturation first.

```cpp
// Outside SIMD_VF: asc_get_saturation_flag queries the current CAST saturation switch.
bool cast_sat_enabled = asc_get_saturation_flag(asc_saturation_mode::CAST);
asc_override_strategy old_strategy = asc_get_saturation_strategy();

if (!cast_sat_enabled) {
    asc_set_saturation_strategy(asc_override_strategy::USE_GLOBAL);
    asc_set_saturation_flag(asc_saturation_mode::CAST, true);
}

asc_copy_gm2ub_align(act_ub, act_gm, act_burst_bytes);
asc_copy_gm2ub_align(bias_ub, bias_gm, bias_burst_bytes);
asc_sync_notify(PIPE_MTE2, PIPE_V, EVENT_ID0);
asc_sync_wait(PIPE_MTE2, PIPE_V, EVENT_ID0);

// Inside the vector computation loop: after activation + bias addition, float → int32 (global CAST saturation takes effect).

// Outside SIMD_VF: if CAST saturation was temporarily enabled in this section, restore the configuration from before entering this section.
if (!cast_sat_enabled) {
    asc_set_saturation_flag(asc_saturation_mode::CAST, false);
    asc_set_saturation_strategy(old_strategy);
}
```

For data movement and Reg cast modes, see [Cast sample](../../../../../../examples/02_simd_c_api/03_c_api/02_reg_vector_compute/cast).
