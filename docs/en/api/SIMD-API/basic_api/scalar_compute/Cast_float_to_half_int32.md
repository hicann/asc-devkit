# Cast (float to half/int32\_t)<a name="ZH-CN_TOPIC_0000001787470378"></a>

<!-- md-trans-meta sourceCommit=754fb95859dead826727a591e674261c6a829a08 translatedAt=2026-09-23T18:17:32.094Z pushedAt=2026-10-26T07:41:44.756Z nl=2 -->

## Applicable Products

<!-- npu="950" id1 -->
- Ascend 950PR&950DT products: supported
<!-- end id1 -->
<!-- npu="A3" id2 -->
- Atlas A3 products: supported
<!-- end id2 -->
<!-- npu="910b" id3 -->
- Atlas A2 products: supported
<!-- end id3 -->
<!-- npu="310b" id4 -->
- Atlas 200I/500 A2 inference products: not supported
<!-- end id4 -->
<!-- npu="310p" id5 -->
- Atlas inference products AI Core: supported
<!-- end id5 -->
<!-- npu="310p" id6 -->
- Atlas inference products Vector Core: not supported
<!-- end id6 -->
<!-- npu="910" id7 -->
- Atlas training products: not supported
<!-- end id7 -->
<!-- @ref: asc-devkit/res/docs/zh/api/SIMD-API/basic_api/scalar_compute/Cast_float_to_half_int32_res.md#id1 -->

## Description<a name="section618mcpsimp"></a>

The header file path is `"basic_api/kernel_operator_scalar_intf.h"`.

Converts the data type of a scalar.

Before using this API, you need to understand [built-in data types](../data_structures/builtin_data_types.md) and [precision conversion](../data_structures/precision_conversion.md).

## Function Prototype<a name="section620mcpsimp"></a>

```cpp
template <typename T, typename U, RoundMode roundMode>
__aicore__ inline U Cast(T valueIn)
```

## Parameters<a name="section622mcpsimp"></a>

**Table 1**  Template parameter description

| Parameter | Description |
| ------ | ------ |
| T | Data type of **valueIn**. |
| U | Data type after conversion. |
| roundMode | Precision conversion processing mode, of the **RoundMode** type.<br>**RoundMode** is an enumeration type used to control the precision conversion processing mode. For details, see [Precision rounding mode](../data_structures/precision_conversion.md#tab1). The value can be:<br>&bull; **CAST_NONE**: When precision loss occurs during conversion, the **CAST_RINT** mode is used; when no precision loss occurs, no rounding is performed.<br>&bull; **CAST_RINT**: Round to the nearest even number.<br>&bull; **CAST_FLOOR**: Round toward negative infinity.<br>&bull; **CAST_CEIL**: Round toward positive infinity.<br>&bull; **CAST_ROUND**: Round half away from zero.<br>&bull; **CAST_TRUNC**: Truncation mode.<br>&bull; **CAST_ODD**: Round to the nearest odd number. |

**Table 2**  Parameter description

| Parameter | Input/Output | Description |
| ------ | ------ | ------ |
| valueIn | Input | Scalar of the data type to be converted. |

## Data Type<a name="zh-cn_topic_0000002531121342_section4219135304818"></a>

The supported data types are as follows:

- **valueIn** supports **float**.
- The return value supports **half** and int32\_t.

## Return Value<a name="section640mcpsimp"></a>

Returns the converted **U**-type scalar data.

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
int32_t intRintOut = AscendC::Cast<float, int32_t, AscendC::RoundMode::CAST_RINT>(valueIn);   // Uses CAST_RINT mode to round 1.5 to the nearest even number, returning 2.

valueIn = 2.5f;
intRoundOut = AscendC::Cast<float, int32_t, AscendC::RoundMode::CAST_ROUND>(valueIn); // Uses CAST_ROUND mode to round to the nearest integer, returning 3.
intCeilOut = AscendC::Cast<float, int32_t, AscendC::RoundMode::CAST_CEIL>(valueIn);   // Uses CAST_CEIL mode to round up, returning 3.
intFloorOut = AscendC::Cast<float, int32_t, AscendC::RoundMode::CAST_FLOOR>(valueIn); // Uses CAST_FLOOR mode to round down, returning 2.
intRintOut = AscendC::Cast<float, int32_t, AscendC::RoundMode::CAST_RINT>(valueIn);   // Uses CAST_RINT mode to round 2.5 to the nearest even number, returning 2.
```
