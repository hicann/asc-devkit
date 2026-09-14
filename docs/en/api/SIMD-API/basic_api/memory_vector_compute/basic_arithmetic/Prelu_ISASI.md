# Prelu \(ISASI\)<a name="ZH-CN_TOPIC_0000002193806085"></a>

<!-- md-trans-meta sourceCommit=e3c8c70b3f5d27f3770063ae4b535bc75d904e3e translatedAt=2026-09-09T11:30:53.295Z -->

## Applicable Products

<!-- npu="950" id1 -->
- Ascend 950PR/Ascend 950DT: Supported
<!-- end id1 -->
<!-- npu="A3" id2 -->
- Atlas A3 training products/Atlas A3 inference products: Not supported
<!-- end id2 -->
<!-- npu="910b" id3 -->
- Atlas A2 training products/Atlas A2 inference products: Not supported
<!-- end id3 -->
<!-- npu="310b" id4 -->
- Atlas 200I/500 A2 inference products: Not supported
<!-- end id4 -->
<!-- npu="310p" id5 -->
- Atlas inference products AI Core: Not supported
<!-- end id5 -->
<!-- npu="310p" id6 -->
- Atlas inference products Vector Core: Not supported
<!-- end id6 -->
<!-- npu="910" id7 -->
- Atlas training products: Not supported
<!-- end id7 -->

## Description<a name="section618mcpsimp"></a>

Header file path: `"basic_api/kernel_operator_vec_binary_scalar_intf.h"`.

Prelu is a binary vector computation API. If the source operand src0 is greater than 0, src0 is directly written to the destination operand dst. Otherwise, the result of src0 * src1 is written to dst. The calculation formula is as follows:

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
| dst | Output | Destination operand.<br>Type: LocalTensor. Supported TPosition: VECIN, VECCALC, VECOUT.<br>For address alignment constraints, see [Address Alignment Constraints](../../../general_description_and_constraints.md). |
| src | Input | Source operand.<br>Type: LocalTensor. Supported TPosition: VECIN, VECCALC, VECOUT.<br>For address alignment constraints, see [Address Alignment Constraints](../../../general_description_and_constraints.md). |
| count | Input | Number of elements involved in the computation. For details about this parameter, see [Continuous Computation](../SIMD_compute/continuous_computation.md). |

## Data Type

<!-- npu="950" id8 -->
For Ascend 950PR/Ascend 950DT, the supported data types are half and float.
<!-- end id8 -->

## Return Value<a name="section640mcpsimp"></a>

None

## Constraints<a name="section633mcpsimp"></a>

- For operand address alignment requirements, see [General Address Alignment Constraints](../../../general_description_and_constraints.md).
- For operand address overlap constraints, see [General Address Overlap Constraints](../../../general_description_and_constraints.md).

## Example<a name="section642mcpsimp"></a>

For more examples, see [LINK](more_sample_9.md).

```cpp
AscendC::Prelu(dstLocal, src0Local, src1Local, 512);
```

The result is as follows:

```bash
Input data src0Local: [1 -2 -3 ... -512]
Input data src1Local: [513 -514 515 ... 1]
Output data dstLocal: [1 1028 -1045 ... -512]
```
