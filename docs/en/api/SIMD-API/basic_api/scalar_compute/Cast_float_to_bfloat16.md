# Cast (float to bfloat16\_t)<a name="ZH-CN_TOPIC_0000001623365980"></a>

<!-- md-trans-meta sourceCommit=754fb95859dead826727a591e674261c6a829a08 translatedAt=2026-09-23T18:17:28.986Z pushedAt=2026-10-26T07:41:44.753Z nl=2 -->

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
- Atlas inference products AI Core: not supported
<!-- end id5 -->
<!-- npu="310p" id6 -->
- Atlas inference products Vector Core: not supported
<!-- end id6 -->
<!-- npu="910" id7 -->
- Atlas training products: not supported
<!-- end id7 -->
<!-- @ref: asc-devkit/res/docs/zh/api/SIMD-API/basic_api/scalar_compute/Cast_float_to_bfloat16_res.md#id1 -->

## Description<a name="section618mcpsimp"></a>

The header file path is `"basic_api/kernel_operator_scalar_intf.h"`.

Converts **float** scalar data into bfloat16\_t scalar data.

Before using this API, you need to understand [built-in data types](../data_structures/builtin_data_types.md) and [precision conversion](../data_structures/precision_conversion.md).

## Prototype<a name="section620mcpsimp"></a>

```cpp
__aicore__ inline bfloat16_t Cast(const float& fVal)
```

## Parameters<a name="section622mcpsimp"></a>

**Table 1**  Parameters

| Parameter | Input/Output | Description |
| ------ | ------ | ------ |
| **fVal** | Input | Scalar data of the **float** type. |

## Return Value<a name="section640mcpsimp"></a>

Returns the converted bfloat16\_t scalar data.

## Constraints<a name="section633mcpsimp"></a>

- The float-to-bfloat16_t conversion uses [CAST_RINT rounding](../data_structures/precision_conversion.md#binary-precision-rounding-rules).
- When the input parameter is an immediate value, it must be explicitly type-cast before being passed in. For example:

    ```cpp
    // Incorrect usage. AscendC does not support converting an integer type to bfloat16_t, which causes a compilation error.
    bfloat16_t x = AscendC::Cast(1);
    // Correct usage.
    bfloat16_t x = AscendC::Cast((float)1);
    ```

## Example<a name="section19372434133520"></a>

```cpp
float m = 3.14159f;
bfloat16_t n = AscendC::Cast(m);  // n = 3.140625  float is rounded when converted to bfloat16_t
```
