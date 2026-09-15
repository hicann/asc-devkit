# Cast (float to bfloat16\_t)<a name="ZH-CN_TOPIC_0000001623365980"></a>

<!-- md-trans-meta sourceCommit=e3c8c70b3f5d27f3770063ae4b535bc75d904e3e translatedAt=2026-08-27T11:12:09.167Z pushedAt=2026-08-31T03:36:33.328Z -->

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
- Atlas 200I/500 A2 inference product: Not supported
<!-- end id11 -->
<!-- npu="310p" id12 -->
- Atlas inference products AI Core: Not supported
<!-- end id12 -->
<!-- npu="310p" id13 -->
- Atlas inference products Vector Core: Not supported
<!-- end id13 -->
<!-- npu="910" id14 -->
- Atlas training products: Not supported
<!-- end id14 -->

## Description<a name="section618mcpsimp"></a>

The header file path is `"basic_api/kernel_operator_scalar_intf.h"`.

Converts a float scalar type to a bfloat16\_t scalar type.





## Prototype<a name="section620mcpsimp"></a>

```cpp
__aicore__ inline bfloat16_t Cast(const float& fVal)
```

## Parameters<a name="section622mcpsimp"></a>

**Table 1** Parameters

| Parameter | Input/Output | Description |
| ------ | ------ | ------ |
| fVal | Input | float scalar type data. |

## Return Value<a name="section640mcpsimp"></a>

Returns the converted **bfloat16\_t** scalar type data.

## Constraints<a name="section633mcpsimp"></a>

- The conversion from float to bfloat16\_t uses [CAST\_RINT rounding](../data_structures/precision_conversion.md#binary-precision-rounding-rules).
- When the input parameter is an immediate value, it must be explicitly type-cast before being passed in, for example:

    ```cpp
    // Incorrect usage. AscendC does not support converting an integer type to bfloat16_t, which causes a compilation error.
    bfloat16_t x = AscendC::Cast(1);
    // Correct usage.
    bfloat16_t x = AscendC::Cast((float)1);
    ```

## Example<a name="section19372434133520"></a>

```cpp
float m = 3.14159f;
bfloat16_t n = AscendC::Cast(m);  // n = 3.140625. The float value is rounded to bfloat16_t.
```
