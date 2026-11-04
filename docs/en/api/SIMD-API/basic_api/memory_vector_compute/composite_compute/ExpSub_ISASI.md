# ExpSub\(ISASI\)<a name="ZH-CN_TOPIC_0000002158484948"></a>

<!-- md-trans-meta sourceCommit=754fb95859dead826727a591e674261c6a829a08 translatedAt=2026-10-26T09:12:09.992Z pushedAt=2026-10-28T03:14:39.462Z nl=2 -->

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
<!-- @ref: asc-devkit/res/docs/zh/api/SIMD-API/basic_api/memory_vector_compute/composite_compute/ExpSub_ISASI_res.md#id1 -->

## Description<a name="section618mcpsimp"></a>

The header file path is `"basic_api/kernel_operator_vec_binary_intf.h"`.

Subtracts src1 from src0, computes the power of the natural constant e with the difference as the exponent, and writes the computation result into dst.

When the source operand data type is float, the calculation formula is as follows:

$dst_i = e^{(src0_i - src1_i)}$

When the source operand data type is half, the calculation formula is as follows:

$dst_i = e^{(cast\_f16\_to\_f32(src0_i) - cast\_f16\_to\_f32(src1_i))}$

## Prototype<a name="section620mcpsimp"></a>

```cpp
template <typename T, typename U>
__aicore__ inline void ExpSub(const LocalTensor<T>& dst, const LocalTensor<U>& src0, const LocalTensor<U>& src1, const uint32_t count)
```

## Parameters<a name="section622mcpsimp"></a>

**Table 1** Template parameter description

| Parameter | Description |
| :----- | :--- |
| T | Data type of the destination operand.|
| U | Data type of the source operand. |

**Table 2**  Parameters

| Parameter | Input/Output | Description |
| :----- | :-------- | :--- |
| dst | Output | Destination operand.<br>Type: [LocalTensor](../../data_structures/LocalTensor/LocalTensor.md). Supported TPosition: VECIN/VECCALC/VECOUT.<br>The start address of LocalTensor must be 32-byte aligned. |
| src0, src1 | Input | Source operand.<br>Type: [LocalTensor](../../data_structures/LocalTensor/LocalTensor.md). Supported TPosition: VECIN/VECCALC/VECOUT.<br>The start address of LocalTensor must be 32-byte aligned. |
| count | Input | Number of elements involved in the computation. |

## Data Type
<!-- npu="950" id8 -->
For Ascend 950PR&950DT products, the data types supported by T and U are half and float.
<!-- end id8 -->

## Return Value<a name="section640mcpsimp"></a>

None

## Constraints<a name="section633mcpsimp"></a>

- For operand address alignment requirements, see [General Address Alignment Constraints](../../../general_description_and_constraints.md#section796754519912).

- For operand address overlap constraints, see [General Address Overlap Constraints](../../../general_description_and_constraints.md#section668772811100).

<!-- npu="950" id9 -->
- This API is implemented for compatibility by calling the [Reg Vector Compute API](../../reg_vector_compute/reg_vector_compute.md) through VF. When the count parameter is set to 0, this API is not guaranteed to be treated as a NOP (no operation).
  <!-- end id9 -->

## Example<a name="section642mcpsimp"></a>

```cpp
AscendC::ExpSub(dstLocal, src0Local, src1Local, 512);
```

The result is as follows:

```plain
Input data src0Local: [1 2 4 ... 510]
Input data src1Local: [1 1 2 ... 510]
Output data dstLocal: [1 2.71828 7.38905 ... 1]
```
