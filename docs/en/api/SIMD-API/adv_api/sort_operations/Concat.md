# Concat

<!-- md-trans-meta sourceCommit=22e1e8c65e425d4c0fb07294311f2eac049ab0b3 translatedAt=2026-11-02T05:05:46.504Z pushedAt=2026-11-18T04:58:41.823Z nl=2 -->

## Applicable Products

<!-- npu="950" id1 -->
- Ascend 950PR&950DT products: Supported
<!-- end id1 -->
<!-- npu="A3" id2 -->
- Atlas A3 products: Supported
<!-- end id2 -->
<!-- npu="910b" id3 -->
- Atlas A2 products: Supported
<!-- end id3 -->
<!-- npu="310b" id4 -->
- Atlas 200I/500 A2 inference products: Not supported
<!-- end id4 -->
<!-- npu="310p" id5 -->
- AI Core of Atlas inference products: Supported
- Vector Core of Atlas inference products: Not supported
<!-- end id5 -->
<!-- npu="910" id6 -->
- Atlas training products: Not supported
<!-- end id6 -->
<!-- @ref: asc-devkit/res/docs/zh/api/SIMD-API/adv_api/sort_operations/Concat_res.md#id1 -->

## Description

Preprocesses data by merging the source operands **src** to be sorted into the target data **concat** in a one-to-one manner. After data preprocessing is complete, [Sort](Sort.md) can be performed.

## Prototype

```
template <typename T>
__aicore__ inline void Concat(LocalTensor<T>& concat, const LocalTensor<T>& src, const LocalTensor<T>& tmp, const int32_t repeatTime)
```

## Parameters

**Table 1** Template parameters

| Parameter | Description |
| --- | --- |
| T | Data type of the operand. Supported data types: half and float. |

**Table 2**  Parameters

| Parameter | Input/Output | Description |
| --- | --- | --- |
| concat | Output | Destination operand.<br><br>Type is [LocalTensor](../../basic_api/data_structures/LocalTensor/LocalTensor.md), and the supported TPosition is VECIN/VECCALC/VECOUT.<br><br>The start address of LocalTensor must be 32-byte aligned. |
| src | Input | Source operand.<br><br>Type is [LocalTensor](../../basic_api/data_structures/LocalTensor/LocalTensor.md), and the supported TPosition is VECIN/VECCALC/VECOUT.<br><br>The start address of LocalTensor must be 32-byte aligned.<br><br>The data type of the source operand must be consistent with that of the destination operand. |
| tmp | Input | Temporary space. It is used to store intermediate variables during complex internal computation and is provided by the developer. For how to obtain the temporary space size, see [GetConcatTmpSize](GetConcatTmpSize.md). The data type is consistent with that of the source operand.<br><br>The type is [LocalTensor](../../basic_api/data_structures/LocalTensor/LocalTensor.md), and the supported TPosition is VECIN/VECCALC/VECOUT.<br><br>The start address of the LocalTensor must be 32-byte aligned. |
| repeatTime | Input | Number of repeated iterations, of the int32_t type. Each iteration processes 16 elements, and the next iteration jumps to the adjacent next group of 16 elements. Value range: repeatTime∈[0,255]. |

## Return Value

None

## Constraints

-   For the operand address alignment requirements, see [General Address Alignment Constraints](../../general_description_and_constraints.md#section796754519912).

## Examples

See the [Examples](MrgSort.md#section642mcpsimp) in [MrgSort](MrgSort.md).
