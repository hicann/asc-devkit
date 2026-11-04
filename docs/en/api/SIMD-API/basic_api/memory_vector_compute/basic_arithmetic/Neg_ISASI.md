# Neg\(ISASI\)<a name="ZH-CN_TOPIC_0000002193851733"></a>

<!-- md-trans-meta sourceCommit=754fb95859dead826727a591e674261c6a829a08 translatedAt=2026-10-26T08:35:49.975Z pushedAt=2026-10-28T03:14:39.420Z nl=2 -->

## Applicable Products

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
<!-- @ref: asc-devkit/res/docs/zh/api/SIMD-API/basic_api/memory_vector_compute/basic_arithmetic/Neg_ISASI_res.md#id1 -->

## Description<a name="section618mcpsimp"></a>

The header file path is `"basic_api/kernel_operator_vec_binary_scalar_intf.h"`.

Neg is a unary vector compute API that negates each element of the input tensor. The formula is as follows:

$dst_i = -src_i$

## Prototype<a name="section620mcpsimp"></a>

```cpp
template <typename T>
__aicore__ inline void Neg(const LocalTensor<T>& dst, const LocalTensor<T>& src, const uint32_t count)
```

## Parameters<a name="section176711403104"></a>

**Table 1** Template parameters

| Parameter | Description |
| ---- | ---- |
| T | Data type of the operand. |

**Table 2** Parameters

| Parameter | Input/Output | Description |
| ---- | ---- | ---- |
| dst | Output | Destination operand.<br>Type: LocalTensor. Supported TPosition values: VECIN, VECCALC, and VECOUT.<br>For address alignment constraints, see [address alignment constraints](../../../general_description_and_constraints.md). |
| src | Input | Source operand.<br>Type: LocalTensor. Supported TPosition values: VECIN, VECCALC, and VECOUT.<br>For address alignment constraints, see [address alignment constraints](../../../general_description_and_constraints.md). |
| count | Input | Number of elements involved in the computation. For details about this parameter, see [continuous compute](../SIMD_compute/continuous_compute.md). |

## Data Types

<!-- npu="950" id8 -->
For Ascend 950PR&950DT products, the supported data types are int8_t, int16_t, half, int32_t, float, and int64_t.
<!-- end id8 -->

## Return Value<a name="section14483414194"></a>

None

## Constraints<a name="section633mcpsimp"></a>

- For operand address alignment requirements, see [General Address Alignment Constraints](../../../general_description_and_constraints.md).
- For operand address overlap constraints, see [General Address Overlap Constraints](../../../general_description_and_constraints.md).

## Examples<a name="section176061616102911"></a>

For more examples, see [LINK](more_examples.md).

```cpp
AscendC::Neg(dstLocal, srcLocal, 512);
```

The result is as follows (both srcLocal and dstLocal are of the int64\_t type):

```bash
Input data (srcLocal): [1 2 3 4 ...]
Output data (dstLocal): [-1 -2 -3 -4 ...]
```
