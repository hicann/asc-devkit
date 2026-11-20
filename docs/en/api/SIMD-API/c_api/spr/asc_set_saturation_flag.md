# asc_set_saturation_flag

<!-- md-trans-meta sourceCommit=04b533bbb005268b1c783a360ecc336709d6540e translatedAt=2026-11-16T01:40:03.481837+00:00 pushedAt=2026-11-16T02:10:57.615Z nl=2 -->

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

Sets the corresponding saturation switch by saturation mode type. `enable_sat` indicates whether to enable saturation in API semantics, and the API internally handles different semantics by mode.

The meaning of `enable_sat` in each mode is as follows:

**Table 1** Saturation mode and enable_sat semantics

| saturation_mode | enable_sat=true | enable_sat=false | Description |
| :--- | :--- | :--- | :--- |
| FLOAT | Saturation | Non-saturation | Saturation control for floating-point computation and some floating-point precision conversion scenarios. The default is non-saturation. Floating-point computation supports the half data type; floating-point precision conversion supports the hifloat8_t, fp8_e8m0_t, fp8_e5m2_t, fp8_e4m3fn_t, half, and bfloat16_t data types. |
| FLOAT8 | Saturation | Non-saturation | Saturation control for fp8_e8m0_t, fp8_e5m2_t, and fp8_e4m3fn_t related scenarios. The default is saturation. |
| INT | Saturation | Truncation | Saturation/truncation control for integer computation overflow. The default is non-saturation. |
| CAST | Saturation | Truncation | Saturation control for precision conversion from floating-point to integer or integer to integer. The default is saturation. It takes effect only when the saturation strategy is `USE_GLOBAL`. |

## Prototype

```c
__aicore__ inline void asc_set_saturation_flag(asc_saturation_mode saturation_mode, bool enable_sat)
```

## Parameters

**Table 2** Parameters

| Parameter | Input/Output | Description |
| :--- | :--- | :--- |
| saturation_mode | Input | Saturation mode type. For values, see [asc_saturation_mode](../defs/enum/asc_saturation_mode.md). |
| enable_sat | Input | Whether to enable saturation. `true` enables saturation, and `false` disables saturation (truncation for INT; non-saturation or truncation for FLOAT/FLOAT8/CAST, as shown in the table above). |

## Return Value

None

## Pipeline Type

PIPE_S

## Constraints

- `enable_sat` is an API semantic parameter: the same Boolean value has different disable semantics in different modes (truncation for INT, non-saturation or truncation for FLOAT/FLOAT8/CAST). Refer to Table 1.
- Before enabling saturation mode for `FLOAT`, enable the global saturation configuration through [asc_set_saturation_strategy](asc_set_saturation_strategy.md) for it to take effect. When `saturation_mode` is `FLOAT8`, you must first enable saturation mode for `FLOAT` for it to take effect.
- When `saturation_mode` is `CAST`, set the saturation strategy to `asc_override_strategy::USE_GLOBAL` for CAST to take effect. It can be configured through [asc_set_saturation_strategy](asc_set_saturation_strategy.md).
- This API only modifies the saturation configuration of the corresponding mode and does not affect other modes.

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

For data movement and Reg cast modes, see [Cast sample](../../../../../../examples/02_simd_c_api/03_c_api/02_reg_vector_compute/cast).
