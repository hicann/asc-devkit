# Concat

<!-- md-trans-meta sourceCommit=2bc2b38d5648dc61f632186b01ba7f36c2295b13 translatedAt=2026-09-08T17:02:43.610Z -->

## Applicable Products

- Ascend 950PR/Ascend 950DT: Supported
- Atlas A3 training products/Atlas A3 inference products: Supported
- Atlas A2 training products/Atlas A2 inference products: Supported
- Atlas 200I/500 A2 inference products: Not supported
- Atlas inference products AI Core: Supported
- Atlas inference products Vector Core: Not supported
- Atlas training products: Not supported

## Description

Preprocesses the data and merges the source operand **src** into the destination data **concat** in a one-to-one correspondence. After data preprocessing is complete, [Sort](Sort.md) can be performed.

## Prototype

```
template <typename T>
__aicore__ inline void Concat(LocalTensor<T> &concat, const LocalTensor<T> &src, const LocalTensor<T> &tmp, const int32_t repeatTime)
```

## Parameters

**Table 1**  Template parameter description

| Parameter | Description |
| --- | --- |
| T | Data type of the operand. Supported data types are half and float. |

**Table 2**  Parameters

| Parameter | Input/Output | Description |
| --- | --- | --- |
| concat | Output | Destination operand.<br><br>The type is [LocalTensor](../../basic_api/data_structures/LocalTensor/LocalTensor.md), and the supported TPosition values are VECIN/VECCALC/VECOUT.<br><br>The start address of the LocalTensor must be 32-byte aligned. |
| src | Input | Source operand.<br><br>The type is [LocalTensor](../../basic_api/data_structures/LocalTensor/LocalTensor.md), and the supported TPosition values are VECIN/VECCALC/VECOUT.<br><br>The start address of the LocalTensor must be 32-byte aligned.<br><br>The data type of the source operand must be consistent with that of the destination operand. |
| tmp | Input | Temporary space. It is used to store intermediate variables during complex internal computation and is provided by you. For how to obtain the temporary space size, see [GetConcatTmpSize](GetConcatTmpSize.md). The data type is consistent with that of the source operand.<br><br>The type is [LocalTensor](../../basic_api/data_structures/LocalTensor/LocalTensor.md), and the supported TPosition values are VECIN/VECCALC/VECOUT.<br><br>The start address of the LocalTensor must be 32-byte aligned. |
| repeatTime | Input | Number of repeated iterations, of the int32_t type. Each iteration processes 16 elements, and the next iteration jumps to the adjacent next group of 16 elements. Value range: repeatTime∈[0,255]. |

## Return Value

None

## Constraints

-   For operand address alignment requirements, see [General Address Alignment Constraints](../../general_description_and_constraints.md#section796754519912).

## Examples

See [Examples](MrgSort-93.md#examples) of [MrgSort](MrgSort-93.md).
