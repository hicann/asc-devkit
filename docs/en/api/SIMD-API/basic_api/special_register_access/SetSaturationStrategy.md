# SetSaturationStrategy

<!-- md-trans-meta sourceCommit=754fb95859dead826727a591e674261c6a829a08 translatedAt=2026-09-23T18:34:08.699Z pushedAt=2026-10-26T07:41:44.780Z nl=2 -->

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

Sets the saturation mode configuration strategy used by the precision conversion APIs. This configuration is mainly used together with [Cast](../memory_vector_compute/type_conversion/Cast.md) for Memory vector calculation and [Cast](../reg_vector_compute/type_conversion/Cast.md) for Reg vector calculation. The configuration methods for the two types of Cast are as follows:

- When single-API saturation configuration is used, the Reg vector calculation Cast configures the single-API saturation or non-saturation mode through `CastTrait::satMode`; the Memory vector calculation Cast does not provide a single-API saturation mode parameter, and its single-API configuration is the saturation mode.
- When global saturation configuration is used, both types of Cast use the global saturation mode set by [SetSaturationFlag](SetSaturationFlag_ISASI.md).

For the specific supported data types and conversion combinations, refer to the documentation of each Cast API. You can query the currently used configuration strategy through [GetSaturationStrategy](GetSaturationStrategy.md).

## Prototype

```cpp
template <OverrideStrategy strategy>
__aicore__ inline void SetSaturationStrategy()
```

## Parameters

**Table 1** Template parameter description

| Parameter | Description |
| --- | --- |
| strategy | Saturation mode configuration strategy. The value can be:<br>&bull;`OverrideStrategy::USE_API`: Uses the saturation configuration of the precision conversion API itself.<br>&bull;`OverrideStrategy::USE_GLOBAL`: Uses the global saturation configuration set by [SetSaturationFlag](SetSaturationFlag_ISASI.md). The system uses this strategy by default.<br>The template parameter has no default value and must be explicitly specified when this API is called. |

## Return Value

None

## Constraints

None

## Example

The following example uses Memory vector calculation [Cast](../memory_vector_compute/type_conversion/Cast.md) to demonstrate the effects of the two configuration strategies. Assume that each element in `srcLocal` of the `int16_t` type is 257, the data type of both output tensors is `uint8_t`, and `count` indicates the number of elements involved in the conversion. The example disables the global precision conversion saturation mode, so that the API's own configuration and the global configuration produce different results.

```cpp
// Disable the global saturation mode for integer precision conversion.
AscendC::SetSaturationFlag<AscendC::SaturationMode::CAST>(false);

// Use the saturation configuration of the Memory vector calculation Cast itself. The conversion result is 255.
AscendC::SetSaturationStrategy<AscendC::OverrideStrategy::USE_API>();
AscendC::PipeBarrier<PIPE_ALL>();
AscendC::Cast(apiDstLocal, srcLocal, AscendC::RoundMode::CAST_NONE, count);

// Use the global non-saturation configuration. The conversion result is 1.
AscendC::SetSaturationStrategy<AscendC::OverrideStrategy::USE_GLOBAL>();
AscendC::PipeBarrier<PIPE_ALL>();
AscendC::Cast(globalDstLocal, srcLocal, AscendC::RoundMode::CAST_NONE, count);
```
