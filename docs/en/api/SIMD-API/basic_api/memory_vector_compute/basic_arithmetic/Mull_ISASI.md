# Mull\(ISASI\)<a name="ZH-CN_TOPIC_0000002193851737"></a>

<!-- md-trans-meta sourceCommit=f266635ab179c912cc9641061c2d27246be9170c translatedAt=2026-11-02T06:49:17.897Z pushedAt=2026-11-18T01:25:07.029Z nl=2 -->

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
<!-- @ref: asc-devkit/res/docs/zh/api/SIMD-API/basic_api/memory_vector_compute/basic_arithmetic/Mull_ISASI_res.md#id1 -->

## Description<a name="section1769835112451"></a>

The header file path is `"basic_api/kernel_operator_vec_binary_scalar_intf.h"`.

Mull is a binary vector compute API. It performs element-wise multiplication on the first count input data src0 and src1, writes the result to dst0, and writes the overflow part to dst1. The computation process is as follows:

![](../../../../figures/mull.png)

## Prototype<a name="section620mcpsimp"></a>

```cpp
template <typename T>
__aicore__ inline void Mull(const LocalTensor<T>& dst0, const LocalTensor<T>& dst1, const LocalTensor<T>& src0, const LocalTensor<T>& src1, const uint32_t count)
```

## Parameters<a name="section622mcpsimp"></a>

**Table 1** Template parameters

| Parameter | Description |
| ---- | ---- |
| T | Data type of the operand. |

**Table 2** Parameters

| Parameter | Input/Output | Description |
| ---- | ---- | ---- |
| dst0, dst1 | Output | Destination operand.<br>Type: LocalTensor. Supported TPosition values: VECIN, VECCALC, and VECOUT.<br>For address alignment constraints, see [address alignment constraints](../../../general_description_and_constraints.md). |
| src0, src1 | Input | Source operand.<br>Type: LocalTensor. Supported TPosition values: VECIN, VECCALC, and VECOUT.<br>For address alignment constraints, see [address alignment constraints](../../../general_description_and_constraints.md). |
| count | Input | Number of elements involved in the calculation. For details about this parameter, see [Continuous Compute](../SIMD_compute/continuous_compute.md). |

## Data Types

<!-- npu="950" id8 -->
For the Ascend 950PR&950DT products, the supported data types are int32_t and uint32_t.
<!-- end id8 -->
<!-- @ref: asc-devkit/res/docs/zh/api/SIMD-API/basic_api/memory_vector_compute/basic_arithmetic/Mull_ISASI_res.md#id2 -->

## Return Value<a name="section640mcpsimp"></a>

None

## Constraints<a name="section633mcpsimp"></a>

- For operand address alignment requirements, see [General Address Alignment Constraints](../../../general_description_and_constraints.md).
- For operand address overlap constraints, see [General Address Overlap Constraints](../../../general_description_and_constraints.md).
<!-- @ref: asc-devkit/res/docs/zh/api/SIMD-API/basic_api/memory_vector_compute/basic_arithmetic/Mull_ISASI_res.md#id3 -->

## Example<a name="section642mcpsimp"></a>

For more examples, see [LINK](more_examples.md).

```cpp
AscendC::Mull(dst0Local, dst1Local, src0Local, src1Local, 512);
```

The result is as follows:

```bash
Input data src0Local: [-2039520159    95829766 -1522913829 ... ]
Input data src1Local: [-1791575569  -291976917 -1973597151 ... ]
Output data dst0Local: [-1560359025   212356098  1219226939 ... ]
Output data dst1Local: [850752575   -6514620  699800065 ... ]
```
