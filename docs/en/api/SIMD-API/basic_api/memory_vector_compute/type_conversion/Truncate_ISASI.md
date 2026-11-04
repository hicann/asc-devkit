# Truncate\(ISASI\)<a name="ZH-CN_TOPIC_0000002158484952"></a>

<!-- md-trans-meta sourceCommit=754fb95859dead826727a591e674261c6a829a08 translatedAt=2026-10-26T10:55:24.844Z pushedAt=2026-10-28T03:14:39.598Z nl=2 -->

## Applicable Products<a name="section1550532418810"></a>

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
<!-- @ref: asc-devkit/res/docs/zh/api/SIMD-API/basic_api/memory_vector_compute/type_conversion/Truncate_ISASI_res.md#id1 -->

## Description<a name="section618mcpsimp"></a>

The header file path is `"basic_api/kernel_operator_vec_vconv_intf.h"`.

Truncates the floating-point elements of the source operand to integer bits, while the data type of the source operand remains unchanged.

## Prototype<a name="section620mcpsimp"></a>

```
template <typename T, RoundMode roundMode>
__aicore__ inline void Truncate(const LocalTensor<T> &dst, const LocalTensor<T> &src, const uint32_t count)
```

## Parameters<a name="section622mcpsimp"></a>

**Table 1** Template parameter description

| Parameter | Description |
| :----- | :--- |
| T | Data type of the operand. |
| roundMode | Rounding mode. The value can be as follows.<br>**RoundMode::CAST_RINT**: Returns the nearest integer, and if two integers are equally close, the even one is selected.<br>**RoundMode::CAST_ROUND**: Rounds to the nearest integer.<br>**RoundMode::CAST_FLOOR**: Rounds down.<br>**RoundMode::CAST_CEIL**: Rounds up.<br>**RoundMode::CAST_TRUNC**: Truncates the decimal part. |


**Table 2**  Parameters

| Parameter | Input/Output | Description |
| :----- | :-------- | :--- |
| dst | Output | Destination operand.<br>Type: [LocalTensor](../../data_structures/LocalTensor/LocalTensor.md). Supported TPosition: VECIN/VECCALC/VECOUT.<br>The start address of LocalTensor must be 32-byte aligned. |
| src | Input | Source operand.<br>Type: [LocalTensor](../../data_structures/LocalTensor/LocalTensor.md). Supported TPosition: VECIN/VECCALC/VECOUT.<br>The start address of LocalTensor must be 32-byte aligned.<br>The data type of the source operand must be the same as that of the destination operand. |
| count | Input | Number of elements involved in the computation. |

## Data Types
<!-- npu="950" id8 -->
- For the Ascend 950PR&950DT products, the data types supported by T are: half, float, bfloat16_t.
<!-- end id8 -->

## Return Value<a name="section640mcpsimp"></a>

None

## Constraints<a name="section633mcpsimp"></a>

-   For operand address alignment requirements, see [General Address Alignment Constraints](../../../general_description_and_constraints.md#section796754519912).
<!-- npu="950" id9 -->
-   For the Ascend 950PR&950DT products, this API achieves compatibility by calling the [Reg vector compute](../../reg_vector_compute/reg_vector_compute.md) API through VF. When the count parameter is 0, this API is not guaranteed to be treated as a NOP (no operation).
<!-- end id9 -->

## Example<a name="section642mcpsimp"></a>

```
AscendC::Truncate<T, RoundMode::CAST_FLOOR>(dstLocal, srcLocal, 512);
```

The result is as follows:

```
Input data srcLocal: [-37.063736    64.17277    -33.448692 ... ]
Output data dstLocal: [-38  64 -34 ... ]
```
