# asc_set_l13d_rpt

<!-- md-trans-meta sourceCommit=0ece502ce5e30f12c91b14fc87b28b1b051cb796 translatedAt=2026-08-26T10:59:26.104Z -->

## Applicable Products

<!-- npu="950" id1 -->
- Ascend 950PR/Ascend 950DT: Supported
<!-- end id1 -->
<!-- npu="A3" id2 -->
- Atlas A3 training products/Atlas A3 inference products: Supported
<!-- end id2 -->
<!-- npu="910b" id3 -->
- Atlas A2 training products/Atlas A2 inference products: Supported
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

Used to set the **repeat** parameter of the **Load3Dv2** API.

## Prototype

```cpp
__aicore__ inline void asc_set_l13d_rpt(asc_load3d_v2_config& config)
```

## Parameters

| Parameter | Input/Output | Description |
| :----- | :------- | :------- |
| **config** | Input | Used to set the **repeat** parameter of the **Load3Dv2** API. For details, see [asc_load3d_v2_config.md](../struct/asc_load3d_v2_config.md). |

## Return Value

None

## Pipeline Type

PIPE_S

## Constraints

- Must be used together with [asc_load3d_v2_config.md](../struct/asc_load3d_v2_config.md) to set the **repeat** parameter of the **Load3Dv2** API.

## Example

```cpp
asc_load3d_v2_config config;
asc_set_l13d_rpt(config); // Set the repeat-related parameters of Load3D to the default values: rpt_stride: 0, rpt_time: 1, rpt_mode: 0.
```