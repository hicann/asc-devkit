# Extract

<!-- md-trans-meta sourceCommit=754fb95859dead826727a591e674261c6a829a08 translatedAt=2026-10-08T11:20:10.798Z pushedAt=2026-10-27T11:50:12.297Z nl=2 -->

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
- Atlas inference products AI Core: Supported
- Vector Core of Atlas inference products: Not supported
<!-- end id5 -->
<!-- npu="910" id6 -->
- Atlas training products: Not supported
<!-- end id6 -->
<!-- @ref: asc-devkit/res/docs/zh/api/SIMD-API/adv_api/sort_operations/Extract_res.md#id1 -->

## Description

Processes the result data of Sort and outputs the sorted value and index.

## Prototype

```
template <typename T>
__aicore__ inline void Extract(const LocalTensor<T>& dstValue, const LocalTensor<uint32_t>& dstIndex, const LocalTensor<T>& sorted, const int32_t repeatTime)
```

## Parameters

**Table 1** Template parameter description

| Parameter | Description |
| --- | --- |
| T | Data type of the operand. Supported data types are: half, float. |

**Table 2**  Parameters

| Parameter | Input/Output | Description |
| --- | --- | --- |
| dstValue | Output | Destination operand.<br><br>Type is [LocalTensor](../../basic_api/data_structures/LocalTensor/LocalTensor.md), and the supported TPosition is VECIN/VECCALC/VECOUT.<br><br>The start address of LocalTensor requires 32-byte alignment. |
| dstIndex | Output | Destination operand.<br><br>Type is [LocalTensor](../../basic_api/data_structures/LocalTensor/LocalTensor.md), and the supported TPosition is VECIN/VECCALC/VECOUT.<br><br>The start address of LocalTensor requires 32-byte alignment.<br><br>This source operand is fixed to the **uint32_t** data type. |
| sorted | Input | Source operand.<br><br>Type is [LocalTensor](../../basic_api/data_structures/LocalTensor/LocalTensor.md), and the supported TPosition is VECIN/VECCALC/VECOUT.<br><br>The start address of LocalTensor requires 32-byte alignment.<br><br>The data type of the source operand must be consistent with that of the destination operand. |
| repeatTime | Input | Number of repeated iterations, of type **int32_t**.<!-- npu="950" id7 --><br><br>For the Ascend 950PR&950DT products, each iteration processes 64 float type elements or 128 half type elements.<!-- end id7 --><!-- npu="A3" id8 --><br><br>For the Atlas A3 products, each iteration processes 64 float type elements or 128 half type elements.<!-- end id8 --><!-- npu="910b" id9 --><br><br>For the Atlas A2 products, each iteration processes 64 float type elements or 128 half type elements.<!-- end id9 --><!-- npu="310p" id10 --><br><br>For the Atlas inference products AI Core, each iteration completes the extraction of 16 Region Proposals and arranges them into 16 elements, and the next iteration jumps to the adjacent next group of 16 Region Proposals and the next group of 16 elements.<!-- end id10 --><br><br>Value range: **repeatTime**∈[0,255]. |

## Return Value

None

## Constraints

-   For operand address alignment requirements, see [General Address Alignment Constraints](../../general_description_and_constraints.md#section796754519912).

## Examples

See the example in [MrgSort](MrgSort.md).
