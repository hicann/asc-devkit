# SetSaturationFlag(ISASI)

<!-- md-trans-meta sourceCommit=e5451c7feba11f9e259f4fbc19c4bf82f8c0496c translatedAt=2026-08-27T13:23:38.424Z -->

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

Sets the saturation switch flag to enable or disable the saturation mode during computation and precision conversion.

## Prototype

```cpp
template <SaturationMode mode>
__aicore__ inline void SetSaturationFlag(bool enableSat)
```

## Parameters

**Table 1** Template parameter description

| Parameter | Description |
| -------- | -------- |
| mode | Corresponding saturation mode to be configured.<br>When **mode** is set to **FLOAT**, if the saturation mode is enabled, inf output is saturated to ±MAX and NaN output is saturated to 0; if the saturation mode is disabled, inf/NaN output remains unchanged.<br>When **mode** is set to **CAST**, if the saturation mode is enabled, overflow values are saturated to ±MAX; if the saturation mode is disabled, overflow values are truncated according to the bit width of the target data type, keeping the low bits and discarding the high bits.<br>enum class SaturationMode : uint8_t {<br>   FLOAT,     // Controls the saturation mode for floating-point computation and floating-point precision conversion. Floating-point data types support only half and bfloat16_t.<br>   CAST       // Controls the saturation mode for precision conversion from floating-point to integer or from integer to integer.<br>};<br><br>|

**Table 2** Parameter description

| Parameter | Input/Output | Description |
| --- | --- | --- |
| enableSat | Input | Whether to enable the corresponding saturation mode. **true** enables the saturation mode, and **false** disables it. |

## Return Value

None

## Constraints

None

## Example

The following example enables the saturation mode for precision conversion during integer-to-integer conversion.

```cpp
AscendC::SetSaturationFlag<AscendC::SaturationMode::CAST>(true);
```
