# AbsSub\(ISASI\)<a name="ZH-CN_TOPIC_0000002158325168"></a>

<!-- md-trans-meta sourceCommit=754fb95859dead826727a591e674261c6a829a08 translatedAt=2026-10-26T09:02:14.354Z pushedAt=2026-10-28T03:14:39.449Z nl=2 -->

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
<!-- @ref: asc-devkit/res/docs/zh/api/SIMD-API/basic_api/memory_vector_compute/composite_compute/AbsSub_ISASI_res.md#id1 -->

## Description<a name="section618mcpsimp"></a>

The header file path is `"basic_api/kernel_operator_vec_binary_intf.h"`.

Subtracts src1 from src0, computes the absolute value of the result, and writes the result to dst. The formula is as follows:

$$
dst_i = |src0_i - src1_i|
$$

## Prototype<a name="section620mcpsimp"></a>

```cpp
template <typename T>
__aicore__ inline void AbsSub(const LocalTensor<T> &dst, const LocalTensor<T> &src0, const LocalTensor<T> &src1, const uint32_t count)
```

## Parameters<a name="section622mcpsimp"></a>

**Table 1** Template parameter description

| Parameter | Description |
| :----- | :--- |
| T | Data type of the operand. |

**Table 2**  Parameters

| Parameter | Input/Output | Description |
| :----- | :-------- | :--- |
| dst | Output | Destination operand.<br>Type: [LocalTensor](../../data_structures/LocalTensor/LocalTensor.md). Supported TPosition: VECIN/VECCALC/VECOUT.<br>The start address of LocalTensor must be 32-byte aligned. |
| src0, src1 | Input | Source operands.<br>Type: [LocalTensor](../../data_structures/LocalTensor/LocalTensor.md). Supported TPosition: VECIN/VECCALC/VECOUT.<br>The start address of LocalTensor must be 32-byte aligned.<br>The data type of the two source operands must be the same as that of the destination operand. |
| count | Input | Number of elements involved in the computation. |

## Data Types
<!-- npu="950" id8 -->
For Ascend 950PR&950DT products, the data types supported by T are half and float.
<!-- end id8 -->

## Return Value<a name="section640mcpsimp"></a>

None

## Constraints<a name="section633mcpsimp"></a>

- For operand address alignment requirements, see [General Address Alignment Constraints](../../../general_description_and_constraints.md#section796754519912).

- For operand address overlap constraints, see [General Address Overlap Constraints](../../../general_description_and_constraints.md#section668772811100).

<!-- npu="950" id9 -->
- This API is implemented for compatibility by calling the [Reg vector compute API](../../reg_vector_compute/reg_vector_compute.md) through VF. When the count parameter is 0, this API is not guaranteed to be treated as a NOP (no operation).
  <!-- end id9 -->

## Example<a name="section642mcpsimp"></a>

```cpp
AscendC::AbsSub(dstLocal, src0Local, src1Local, 512);
```

The result is as follows:

```plain
Input data src0Local: [1 2 3 ... 512]
Input data src1Local: [513 514 515 ... 1024]
Output data dstLocal: [512 512 512 ... 512]
```
