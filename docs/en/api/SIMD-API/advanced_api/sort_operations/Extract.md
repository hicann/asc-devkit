# Extract

<!-- md-trans-meta sourceCommit=0364dcce9ab16d714270f168ec9546fd7620f90f translatedAt=2026-09-08T17:04:37.741Z -->

## Applicable Products

- Ascend 950PR/Ascend 950DT: Supported
- Atlas A3 training products/Atlas A3 inference products: Supported
- Atlas A2 training products/Atlas A2 inference products: Supported
- Atlas 200I/500 A2 inference products: Not supported
- Atlas inference products AI Core: Supported
- Atlas inference products Vector Core: Not supported
- Atlas training products: Not supported

## Description

Processes the result data of **Sort** and outputs the sorted values and indices.

## Prototype

```
template <typename T>
__aicore__ inline void Extract(const LocalTensor<T> &dstValue, const LocalTensor<uint32_t> &dstIndex, const LocalTensor<T> &sorted, const int32_t repeatTime)
```

## Parameters

**Table 1**  Template parameter description

| Parameter | Description |
| --- | --- |
| T | Data type of the operand. Supported data types are half and float. |

**Table 2**  Parameters

| Parameter | Input/Output | Description |
| --- | --- | --- |
| dstValue | Output | Destination operand.<br><br>The type is [LocalTensor](../../basic_api/data_structures/LocalTensor/LocalTensor.md), and the supported TPosition values are VECIN/VECCALC/VECOUT.<br><br>The start address of the LocalTensor must be 32-byte aligned. |
| dstIndex | Output | Destination operand.<br><br>The type is [LocalTensor](../../basic_api/data_structures/LocalTensor/LocalTensor.md), and the supported TPosition values are VECIN/VECCALC/VECOUT.<br><br>The start address of the LocalTensor must be 32-byte aligned.<br><br>This source operand is fixed to the uint32_t data type. |
| sorted | Input | Source operand.<br><br>The type is [LocalTensor](../../basic_api/data_structures/LocalTensor/LocalTensor.md), and the supported TPosition values are VECIN/VECCALC/VECOUT.<br><br>The start address of the LocalTensor must be 32-byte aligned.<br><br>The data type of the source operand must be consistent with that of the destination operand. |
| repeatTime | Input | Number of repeated iterations, of the int32_t type.<br><br>Ascend 950PR/Ascend 950DT, each iteration processes 64 float elements or 128 half elements.<br><br>Atlas A3 training products/Atlas A3 inference products, each iteration processes 64 float elements or 128 half elements.<br><br>Atlas A2 training products/Atlas A2 inference products, each iteration processes 64 float elements or 128 half elements.<br><br>Atlas inference products AI Core, each iteration extracts the elements of 16 Region Proposals and arranges them into 16 elements, and the next iteration jumps to the adjacent next group of 16 Region Proposals and the next group of 16 elements.<br><br>Value range: repeatTime∈[0,255]. |

## Return Value

None

## Constraints

-   For operand address alignment requirements, see [General Address Alignment Constraints](../../general_description_and_constraints.md#section796754519912).

## Examples

See examples in [MrgSort](MrgSort-93.md).
