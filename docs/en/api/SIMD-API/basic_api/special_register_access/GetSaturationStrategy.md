# GetSaturationStrategy

<!-- md-trans-meta sourceCommit=754fb95859dead826727a591e674261c6a829a08 translatedAt=2026-09-23T18:28:04.787Z pushedAt=2026-10-26T07:41:44.775Z nl=2 -->

## Applicable Products

<!-- npu="950" id1 -->
- Ascend 950PR&950DT products: supported
<!-- end id1 -->
<!-- npu="A3" id2 -->
- Atlas A3 products: not supported
<!-- end id2 -->
<!-- npu="910b" id3 -->
- Atlas A2 products: not supported
<!-- end id3 -->
<!-- npu="310b" id4 -->
- Atlas 200I/500 A2 inference products: not supported
<!-- end id4 -->
<!-- npu="310p" id5 -->
- Atlas inference products AI Core: not supported
<!-- end id5 -->
<!-- npu="310p" id6 -->
- Atlas inference products Vector Core: not supported
<!-- end id6 -->
<!-- npu="910" id7 -->
- Atlas training products: not supported
<!-- end id7 -->

## Description

The header file path is `"basic_api/kernel_operator_common_intf.h"`.

Obtains the saturation mode configuration strategy currently used by the precision conversion API. The saturation mode is used to handle scenarios where the result overflows or INF/NAN occurs during precision conversion. For details, see [SetSaturationFlag Description](SetSaturationFlag_ISASI.md#description). This API is used to query the configuration source selected through [SetSaturationStrategy](SetSaturationStrategy.md), and is mainly used together with [Cast](../memory_vector_compute/type_conversion/Cast.md) for Memory vector calculation and [Cast](../reg_vector_compute/type_conversion/Cast.md) for Reg vector calculation:

- When the API's own saturation configuration is used, the Reg vector calculation Cast configures the saturation or non-saturation mode of a single API through `CastTrait::satMode`; the Memory vector calculation Cast does not provide a single-API saturation mode parameter, and its single-API configuration is the saturation mode.
- When the global saturation configuration is used, both types of Cast use the global saturation mode set by [SetSaturationFlag](SetSaturationFlag_ISASI.md).

## Prototype

```cpp
__aicore__ inline OverrideStrategy GetSaturationStrategy()
```

## Parameters

None

## Return Value

The return value type is `OverrideStrategy`, which indicates the currently used saturation mode configuration strategy.

**Table 1**  Return Value

| Return Value | Description |
| --- | --- |
| `OverrideStrategy::USE_API` | Uses the saturation configuration of the precision conversion API itself. |
| `OverrideStrategy::USE_GLOBAL` | Uses the global saturation configuration set by [SetSaturationFlag](SetSaturationFlag_ISASI.md). The system adopts this strategy by default. |

## Constraints

None

## Example

The following example queries whether the global saturation mode set by `SetSaturationFlag` is currently in use.

```cpp
bool useGlobal = AscendC::GetSaturationStrategy() == AscendC::OverrideStrategy::USE_GLOBAL;
```
