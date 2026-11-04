# MulsCast\(ISASI\)<a name="ZH-CN_TOPIC_0000002158325172"></a>

<!-- md-trans-meta sourceCommit=754fb95859dead826727a591e674261c6a829a08 translatedAt=2026-10-26T09:20:21.181Z pushedAt=2026-10-28T03:14:39.469Z nl=2 -->

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
<!-- @ref: asc-devkit/res/docs/zh/api/SIMD-API/basic_api/memory_vector_compute/composite_compute/MulsCast_ISASI_res.md#id1 -->

## Description<a name="section618mcpsimp"></a>

The header file path is `"basic_api/kernel_operator_vec_binary_scalar_intf.h"`.

Multiplies the first count data of the vector source operand by a scalar, converts the result to the half type in CAST\_ROUND mode, and writes the computation result into dst. This API supports both scalar-first and scalar-last scenarios.

$$
dst_i = Cast(src0_i \times scalar)
$$

$$
dst_i = Cast(scalar \times src1_i)
$$

The scalar input also supports configuring a LocalTensor single point element. The calculation formulas are as follows, where idx indicates the position coefficient of the LocalTensor single point element.

$$
dst_i = Cast(src0_i \times src1_{idx})
$$

$$
dst_i = Cast(src0_{idx} \times src1_i)
$$

## Prototype<a name="section620mcpsimp"></a>

```cpp
template <typename T0 = BinaryDefaultType, typename T1 = BinaryDefaultType, const BinaryConfig &config = DEFAULT_BINARY_CONFIG, typename T2, typename T3, typename T4>
__aicore__ inline void MulsCast(const T2 &dst, const T3 &src0, const T4 &src1, const uint32_t count)
```

## Parameters<a name="section622mcpsimp"></a>

**Table 1** Template parameter description

| Parameter | Description |
| :----- | :--- |
| T0 | Data type of the destination operand.<br>Reserved parameter, not enabled yet. It is reserved for future function extension. When it needs to be specified, pass the default value BinaryDefaultType. |
| T1 | Data type of the source operand.<br>Reserved parameter, not enabled yet. It is reserved for future function extension. When it needs to be specified, pass the default value BinaryDefaultType. |
| config | Type: BinaryConfig. It takes effect when the scalar is a LocalTensor single point element, and is used to specify the position of the single point element operand. The default value DEFAULT_BINARY_CONFIG indicates that the right operand is a scalar.<br>struct BinaryConfig {<br>    int8_t scalarTensorIndex = 1; // Specifies the position of the scalar when the scalar is a LocalTensor single point element. 0 indicates the left operand, and 1 indicates the right operand.<br>};<br>constexpr BinaryConfig DEFAULT_BINARY_CONFIG = {1}; |
| T2 | LocalTensor type. The corresponding data type is automatically inferred based on the input parameter dst. You do not need to configure this parameter. Ensure that dst meets the data type constraints. |
| T3 | LocalTensor type or scalar type. The corresponding data type is automatically inferred based on the input parameter src0. You do not need to configure this parameter. Ensure that src0 meets the data type constraints. |
| T4 | LocalTensor type or scalar type. The corresponding data type is automatically inferred based on the input parameter src1. You do not need to configure this parameter. Ensure that src1 meets the data type constraints. |

**Table 2**  Parameters

| Parameter | Input/Output | Description |
| :----- | :-------- | :--- |
| dst | Output | Destination operand.<br>Type: [LocalTensor](../../data_structures/LocalTensor/LocalTensor.md). Supported TPosition: VECIN/VECCALC/VECOUT.<br>The start address of LocalTensor must be 32-byte aligned.<br>Ascend 950PR&950DT products support the following data type: half<br>For the precision conversion rules corresponding to different data types, see [Precision Conversion Rules](overview.md#precision-conversion). |
| src0/src1 | Input | Source operand.<br>&bull; When the type is LocalTensor, it can be used as a vector operand or a scalar single point element. Supported TPosition: VECIN/VECCALC/VECOUT.<br>The start address of LocalTensor must be 32-byte aligned.<br>Ascend 950PR&950DT products support the following data type: float<br>&bull; When the type is scalar:<br>Ascend 950PR&950DT products support the following data type: float |
| count | Input | Number of elements involved in the computation. |

**Table 3**  Precision Conversion Rules

| src Type | dst Type | Type Conversion Mode Introduction |
| :----- | :------ | :------------- |
| float | half | Rounds the source operand to the nearest value representable in half using the CAST_ROUND mode, and stores the result in dst in half format (overflow is saturated by default). |

## Data Type
<!-- npu="950" id8 -->
For Ascend 950PR&950DT products, the data type supported by src is float. The data type supported by dst is half.
<!-- end id8 -->

## Return Value<a name="section640mcpsimp"></a>

None

## Constraints<a name="section633mcpsimp"></a>

-   At least one of the left operand and the right operand must be a vector. Having both the left and right operands as scalars is not supported.
-   When a single point element of LocalTensor is passed to this API as a scalar, the idx parameter must be a compile-time constant. If a variable is passed, it must be declared as constexpr.
<!-- npu="950" id9 -->
- This API is implemented for compatibility by calling the [Reg vector compute API](../../reg_vector_compute/reg_vector_compute.md) through VF. When the count parameter is 0, it is not guaranteed that this API is treated as a NOP (no operation).
  <!-- end id9 -->

## Example<a name="section642mcpsimp"></a>

```cpp
// Example with the scalar as the second operand.
AscendC::MulsCast(dstLocal, src0Local, src1Local[0], 512);

// Example with the scalar as the first operand.
static constexpr AscendC::BinaryConfig config = { 0 };
AscendC::MulsCast<BinaryDefaultType, BinaryDefaultType, config>(dstLocal, src0Local[0], src1Local, 512);
```

The result is as follows:

```plain
Input data src0Local: [6 5 11 ... ]
Input data src1Local: 2
Output data dstLocal: [12 10 22 ... ]
```
