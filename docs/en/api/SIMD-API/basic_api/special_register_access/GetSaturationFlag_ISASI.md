# GetSaturationFlag(ISASI)

<!-- md-trans-meta sourceCommit=e5451c7feba11f9e259f4fbc19c4bf82f8c0496c translatedAt=2026-08-27T13:18:18.849Z -->

## Applicable Products

<!-- npu="950" id1 -->
- Ascend 950PR/Ascend 950DT: Not supported
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

The header file path is `"basic_api/kernel_operator_common_intf.h"`.

Obtains the enabled state of the saturation mode during computation/precision conversion.

## Prototype

```cpp
template <SaturationMode mode>
__aicore__ inline bool GetSaturationFlag()
```

## Parameters

**Table 1** Template parameter description

| Parameter | Description |
| -------- | -------- |
| mode | Corresponding saturation mode configured.<br>When **mode** is set to **FLOAT**, saturation mode is enabled, and inf outputs are saturated to ±MAX while NaN outputs are saturated to 0; when saturation mode is disabled, inf/NaN outputs remain unchanged.<br>When **mode** is set to **CAST**, saturation mode is enabled, and overflow values are saturated to ±MAX; when saturation mode is disabled, overflow values are truncated according to the bit width of the target data type, keeping the low bits and discarding the high bits.<br>enum class SaturationMode : uint8_t {<br>   FLOAT,     // Control the saturation mode for floating-point computation and floating-point precision conversion. Only half and bfloat16_t are supported for floating-point data types.<br>   CAST       // Control the precision conversion saturation mode for floating-point-to-integer or integer-to-integer conversion.<br>};<br><br>|

## Return Value

Returns **true** if the saturation mode is enabled, and **false** if it is not enabled.

## Constraints

None

## Example

The following example queries whether the saturation mode for precision conversion during integer-to-integer conversion is enabled.

```cpp
bool res = AscendC::GetSaturationFlag<AscendC::SaturationMode::CAST>();
```
