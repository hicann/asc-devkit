# Cast (Multi-Type to float)<a name="ZH-CN_TOPIC_0000001623525812"></a>

<!-- md-trans-meta sourceCommit=754fb95859dead826727a591e674261c6a829a08 translatedAt=2026-09-23T18:17:29.112Z pushedAt=2026-10-26T07:41:44.755Z nl=2 -->

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
- Atlas 200I/500 A2 inference product: not supported
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
<!-- @ref: asc-devkit/res/docs/zh/api/SIMD-API/basic_api/scalar_compute/Cast_multi_type_to_float_res.md#id1 -->

## Description<a name="section618mcpsimp"></a>

The header file path is `"basic_api/kernel_operator_scalar_intf.h"`.

This API converts the input data to the float type.

Before using this API, you need to understand [built-in data types](../data_structures/builtin_data_types.md) and [precision conversion](../data_structures/precision_conversion.md).

## Prototype<a name="section620mcpsimp"></a>

<!-- npu="910b,A3" id16 -->
The following prototypes are supported only by:

<!-- npu="910b" id8 -->
- Atlas A2 products
<!-- end id8 -->

<!-- npu="A3" id9 -->
- Atlas A3 products
<!-- end id9 -->

Converts the bfloat16_t type to float:

```cpp
__aicore__ inline float Cast(const bfloat16_t& bVal)
```
<!-- end id16 -->

<!-- npu="950" id10 -->
The following prototypes are supported only by the Ascend 950PR&950DT products:

Supports conversion of multiple data types to float:

```cpp
template <typename T, typename U = float,
        typename = Std::enable_if_t<
        (Std::is_same<T, bfloat16_t>::value || Std::is_same<T, hifloat8_t>::value ||
            Std::is_same<T, fp8_e5m2_t>::value || Std::is_same<T, fp8_e4m3fn_t>::value ||
            Std::is_same<T, fp4x2_e1m2_t>::value || Std::is_same<T, fp4x2_e2m1_t>::value), 
        void>>
__aicore__ constexpr inline U Cast(T bVal)
```
<!-- end id10 -->

## Parameters<a name="section622mcpsimp"></a>

<!-- npu="950" id11 -->
**Table 1**  Template parameter description

| Parameter | Description |
| ------ | ------ |
| T | Data type of the operand. |
| U | Data type of the return value. |

<!-- end id11 -->

**Table 2**  API parameter description

| Parameter | Input/Output | Description |
| -------- | --------- | ------------------ |
| bVal     | Input      | Scalar data to be converted. |

## Data Types<a name="zh-cn_topic_0000002562161257_section4219135304818"></a>

- The supported operand data types are as follows:

  <!-- npu="950" id12 -->
  - Ascend 950PR&950DT products supports fp4x2_e2m1_t, fp4x2_e1m2_t, hifloat8_t, fp8_e5m2_t, fp8_e4m3fn_t, and bfloat16_t.
  <!-- end id12 -->
  <!-- npu="A3" id13 -->
  - Atlas A3 products supports bfloat16_t.
  <!-- end id13 -->
  <!-- npu="910b" id14 -->
  - Atlas A2 products supports bfloat16_t.
  <!-- end id14 -->

<!-- npu="950" id15 -->
- To satisfy the 1-byte data size, the fp4x2_e1m2_t and fp4x2_e2m1_t types are constructed by concatenating two 4-bit scalar data values. During conversion, the value converted is the lower 4 bits, that is, bits [0:3], of the fp4x2_e1m2_t and fp4x2_e2m1_t scalar data.
<!-- end id15 -->

- The supported return value data type is float.

## Return Value<a name="section640mcpsimp"></a>

Returns the converted float-type scalar data.

## Constraints<a name="section633mcpsimp"></a>

When the input parameter is an immediate value, an explicit type conversion is required, for example:

```cpp
// Incorrect usage. AscendC does not support converting an integer type to float, which causes a compilation error.
float x = AscendC::Cast(1);
// Correct usage.
float x = AscendC::Cast((bfloat16_t)1);
```

## Example<a name="section19372434133520"></a>

```cpp
float fVal = 3.14159;
bfloat16_t bVal = AscendC::Cast(fVal); // Returns 3.140625.
fVal = AscendC::Cast(bVal);            // Returns 3.140625.
```
