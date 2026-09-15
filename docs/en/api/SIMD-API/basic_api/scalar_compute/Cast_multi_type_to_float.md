# Cast (Multi-type to Float)<a name="ZH-CN_TOPIC_0000001623525812"></a>

<!-- md-trans-meta sourceCommit=e3c8c70b3f5d27f3770063ae4b535bc75d904e3e translatedAt=2026-08-27T11:17:25.418Z pushedAt=2026-09-01T01:14:41.992Z -->

## Applicable Products<a name="section1550532418810"></a>

<!-- npu="950" id17 -->
- Ascend 950PR/Ascend 950DT: Supported
<!-- end id17 -->
<!-- npu="A3" id18 -->
- Atlas A3 training products/Atlas A3 inference products: Supported
<!-- end id18 -->
<!-- npu="910b" id19 -->
- Atlas A2 training products/Atlas A2 inference products: Supported
<!-- end id19 -->
<!-- npu="310b" id20 -->
- Atlas 200I/500 A2 inference products: Not supported
<!-- end id20 -->
<!-- npu="310p" id21 -->
- Atlas inference products AI Core: Not supported
<!-- end id21 -->
<!-- npu="310p" id22 -->
- Atlas inference products Vector Core: Not supported
<!-- end id22 -->
<!-- npu="910" id23 -->
- Atlas training products: Not supported
<!-- end id23 -->

## Description<a name="section618mcpsimp"></a>

The header file path is `"basic_api/kernel_operator_scalar_intf.h"`.

This API converts the input data to the float type.

## Prototype<a name="section620mcpsimp"></a>

<!-- npu="910b,A3" id9 -->
The following prototypes are supported only by:

<!-- npu="910b" id1 -->
- Atlas A2 training products/Atlas A2 inference products
<!-- end id1 -->

<!-- npu="A3" id2 -->
- Atlas A3 training products/Atlas A3 inference products
<!-- end id2 -->

Converts the `bfloat16_t` type to the `float` type:

```cpp
__aicore__ inline float Cast(const bfloat16_t& bVal)
```
<!-- end id9 -->

<!-- npu="950" id3 -->
The following prototypes are supported only by Ascend 950PR/Ascend 950DT:

Supports conversion of multiple data types to the `float` type:

```cpp
template <typename T, typename U = float,
        typename = Std::enable_if_t<
        (Std::is_same<T, bfloat16_t>::value || Std::is_same<T, hifloat8_t>::value ||
            Std::is_same<T, fp8_e5m2_t>::value || Std::is_same<T, fp8_e4m3fn_t>::value ||
            Std::is_same<T, fp4x2_e1m2_t>::value || Std::is_same<T, fp4x2_e2m1_t>::value), 
        void>>
__aicore__ constexpr inline U Cast(T bVal)
```
<!-- end id3 -->

## Parameters<a name="section622mcpsimp"></a>

<!-- npu="950" id4 -->
**Table 1** Template parameter description

| Parameter | Description |
| ------ | ------ |
| T | Operand data type. |
| U | Return value data type. |
<!-- end id4 -->

**Table 2**  API parameter description

| Parameter | Input/Output | Meaning            |
| --------- | ------------ | ------------------ |
| bVal      | Input        | Scalar data to be converted. |

## Data Types<a name="zh-cn_topic_0000002562161257_section4219135304818"></a>

- The supported operand data types are as follows:

  <!-- npu="950" id5 -->
  - Ascend 950PR/Ascend 950DT: fp4x2_e2m1_t, fp4x2_e1m2_t, hifloat8_t, fp8_e5m2_t, fp8_e4m3fn_t, and bfloat16_t are supported.
  <!-- end id5 -->
  <!-- npu="A3" id6 -->
  - Atlas A3 training products/Atlas A3 inference products: bfloat16_t is supported.
  <!-- end id6 -->
  <!-- npu="910b" id7 -->
  - Atlas A2 training products/Atlas A2 inference products: bfloat16_t is supported.
  <!-- end id7 -->

<!-- npu="950" id8 -->
- To satisfy the 1-byte data size requirement, the fp4x2_e1m2_t and fp4x2_e2m1_t types are constructed by concatenating two 4-bit scalar data values. During conversion, the value converted is the lower 4 bits, that is, bits [0:3], of the fp4x2_e1m2_t and fp4x2_e2m1_t scalar data.
<!-- end id8 -->

- The supported return value data type is float.

## Return Value<a name="section640mcpsimp"></a>

Returns the converted float-type scalar data.

## Constraints<a name="section633mcpsimp"></a>

When the input parameter is an immediate value, explicit type conversion is required, for example:

```cpp
// Incorrect usage. AscendC does not support converting integer types to float, which causes a compilation error.
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
