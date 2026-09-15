# Cast (float to half/int32\_t)<a name="ZH-CN_TOPIC_0000001787470378"></a>

<!-- md-trans-meta sourceCommit=e5451c7feba11f9e259f4fbc19c4bf82f8c0496c translatedAt=2026-08-27T11:12:19.987Z pushedAt=2026-09-01T01:12:31.132Z -->

## Applicable Products<a name="section1550532418810"></a>

<!-- npu="950" id8 -->
- Ascend 950PR/Ascend 950DT: Supported
<!-- end id8 -->
<!-- npu="A3" id9 -->
- Atlas A3 training products/Atlas A3 inference products: Supported
<!-- end id9 -->
<!-- npu="910b" id10 -->
- Atlas A2 training products/Atlas A2 inference products: Supported
<!-- end id10 -->
<!-- npu="310b" id11 -->
- Atlas 200I/500 A2 inference products: Not supported
<!-- end id11 -->
<!-- npu="310p" id12 -->
- Atlas inference products AI Core: Supported
<!-- end id12 -->
<!-- npu="310p" id13 -->
- Atlas inference products Vector Core: Not supported
<!-- end id13 -->
<!-- npu="910" id14 -->
- Atlas training products: Not supported
<!-- end id14 -->

## Description<a name="section618mcpsimp"></a>

The header file path is `"basic_api/kernel_operator_scalar_intf.h"`.

Converts the data type of a scalar.

Before using this API, you need to understand [built-in data types](../data_structures/built_in_data_type.md) and [precision conversion](../data_structures/precision_conversion.md).

## Prototype<a name="section620mcpsimp"></a>

```cpp
template <typename T, typename U, RoundMode roundMode>
__aicore__ inline U Cast(T valueIn)
```

## Parameters<a name="section622mcpsimp"></a>

**Table 1** Template parameter description

| Parameter | Description |
| ------ | ------ |
| T | Data type of **valueIn**. |
| U | Data type after conversion. |
| roundMode | Precision conversion processing mode, of the **RoundMode** type.<br>**RoundMode** is an enumeration type used to control the precision conversion processing mode. For details, see [precision rounding mode](../data_structures/precision_conversion.md#tab1). The available values are as follows:<br>&bull; **CAST_NONE**: When precision loss occurs during conversion, the **CAST_RINT** mode is used; when no precision loss occurs, no rounding is performed.<br>&bull; **CAST_RINT**: Rounds to the nearest even number.<br>&bull; **CAST_FLOOR**: Rounds toward negative infinity.<br>&bull; **CAST_CEIL**: Rounds toward positive infinity.<br>&bull; **CAST_ROUND**: Rounds half away from zero.<br>&bull; **CAST_TRUNC**: Truncation mode.<br>&bull; **CAST_ODD**: Rounds to the nearest odd number.<br>|

**Table 2** Parameter description

| Parameter | Input/Output | Description |
| ------ | ------ | ------ |
| valueIn | Input | Scalar of the data type to be converted. |

## Data Type<a name="zh-cn_topic_0000002531121342_section4219135304818"></a>

The supported data types are as follows:

- **valueIn** supports **float**.
- The return value supports **half** and **int32_t**.

## Return Value<a name="section640mcpsimp"></a>

Returns the converted scalar data of type **U**.

## Constraints<a name="section633mcpsimp"></a>

The supported data type combinations and their corresponding **RoundMode** values are as follows:

| valueIn | Return Value | RoundMode |
| ------ | ------ | ------ |
| float | half | CAST_ODD |
| float | int32_t | CAST_ROUND |
| float | int32_t | CAST_CEIL |
| float | int32_t | CAST_FLOOR |
| float | int32_t | CAST_RINT |

## Example<a name="section837496171220"></a>

```cpp
float valueIn = 3.14159f;
half halfOut = AscendC::Cast<float, half, AscendC::RoundMode::CAST_ODD>(valueIn);             // Returns 3.142578.

valueIn = 1.5f;
int32_t intRoundOut = AscendC::Cast<float, int32_t, AscendC::RoundMode::CAST_ROUND>(valueIn); // Uses CAST_ROUND mode to round to the nearest integer, returning 2.
int32_t intCeilOut = AscendC::Cast<float, int32_t, AscendC::RoundMode::CAST_CEIL>(valueIn);   // Uses CAST_CEIL mode to round up, returning 2.
int32_t intFloorOut = AscendC::Cast<float, int32_t, AscendC::RoundMode::CAST_FLOOR>(valueIn); // Uses CAST_FLOOR mode to round down, returning 1.
int32_t intRintOut = AscendC::Cast<float, int32_t, AscendC::RoundMode::CAST_RINT>(valueIn);   // Uses CAST_RINT mode to round 1.5 to the nearest even integer, returning 2.

valueIn = 2.5f;
intRoundOut = AscendC::Cast<float, int32_t, AscendC::RoundMode::CAST_ROUND>(valueIn); // Uses CAST_ROUND mode to round to the nearest integer, returning 3.
intCeilOut = AscendC::Cast<float, int32_t, AscendC::RoundMode::CAST_CEIL>(valueIn);   // Uses CAST_CEIL mode to round up, returning 3.
intFloorOut = AscendC::Cast<float, int32_t, AscendC::RoundMode::CAST_FLOOR>(valueIn); // Uses CAST_FLOOR mode to round down, returning 2.
intRintOut = AscendC::Cast<float, int32_t, AscendC::RoundMode::CAST_RINT>(valueIn);   // Uses CAST_RINT mode to round 2.5 to the nearest even integer, returning 2.
```
