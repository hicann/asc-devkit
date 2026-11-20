# Prelu\(ISASI\)<a name="ZH-CN_TOPIC_0000002193806085"></a>

<!-- md-trans-meta sourceCommit=00f2fa768749ee1a0d8f4a453d77d0c75cf3b952 translatedAt=2026-11-02T06:54:56.344Z pushedAt=2026-11-18T01:25:07.037Z nl=2 -->

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
<!-- @ref: asc-devkit/res/docs/zh/api/SIMD-API/basic_api/memory_vector_compute/basic_arithmetic/Prelu_ISASI_res.md#id1 -->

## Description<a name="section618mcpsimp"></a>

The header file path is `"basic_api/kernel_operator_vec_binary_scalar_intf.h"`.

Prelu is a binary vector computation API. When the source operand src0 is greater than or equal to 0, src0 is written directly to the destination operand dst; otherwise, the result of src0 * src1 is written to dst. Note: When the data in the source operand src0 is a floating-point positive zero (+0), it is treated as a positive number and output directly; when it is a floating-point negative zero (-0), it is treated as a negative number and multiplied by src1. The formula is as follows:

$dst_i = (src0_i \ge 0) ? src0_i : src0_i * src1_i$

## Prototype<a name="section620mcpsimp"></a>

```cpp
template <typename T>
__aicore__ inline void Prelu(const LocalTensor<T>& dst, const LocalTensor<T>& src0, const LocalTensor<T>& src1, const uint32_t count)
```

## Parameters<a name="section622mcpsimp"></a>

**Table 1** Template parameters

| Parameter | Description |
| ---- | ---- |
| T | Data type of the operand. |

**Table 2** Parameters

| Parameter | Input/Output | Description |
| ---- | ---- | ---- |
| dst | Output | Destination operand.<br>Type: LocalTensor. Supported TPosition values: VECIN, VECCALC, and VECOUT.<br>For address alignment constraints, see [address alignment constraints](../../../general_description_and_constraints.md). |
| src | Input | Source operand.<br>Type: LocalTensor. Supported TPosition values: VECIN, VECCALC, and VECOUT.<br>For address alignment constraints, see [address alignment constraints](../../../general_description_and_constraints.md). |
| count | Input | Number of elements involved in the calculation. For details about this parameter, see [Continuous Compute](../SIMD_compute/continuous_compute.md). |

## Data Types

<!-- npu="950" id8 -->
For Ascend 950PR&950DT products, the supported data types are half and float.
<!-- end id8 -->
<!-- @ref: asc-devkit/res/docs/zh/api/SIMD-API/basic_api/memory_vector_compute/basic_arithmetic/Prelu_ISASI_res.md#id2 -->

## Return Value<a name="section640mcpsimp"></a>

None

## Constraints<a name="section633mcpsimp"></a>

- For operand address alignment requirements, see [General Address Alignment Constraints](../../../general_description_and_constraints.md).
- For operand address overlap constraints, see [General Address Overlap Constraints](../../../general_description_and_constraints.md).
<!-- @ref: asc-devkit/res/docs/zh/api/SIMD-API/basic_api/memory_vector_compute/basic_arithmetic/Prelu_ISASI_res.md#id3 -->

## Example<a name="section642mcpsimp"></a>

For more examples, see [LINK](more_examples.md).

```cpp
AscendC::Prelu(dstLocal, src0Local, src1Local, 512);
```

The result is as follows:

```bash
Input data src0Local: [1 -2 -3 ... -512]
Input data src1Local: [513 -514 515 ... 1]
Output data dstLocal: [1 1028 -1045 ... -512]
```
