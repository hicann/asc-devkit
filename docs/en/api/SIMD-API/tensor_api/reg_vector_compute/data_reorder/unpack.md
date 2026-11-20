# unpack

<!-- md-trans-meta sourceCommit=db86c75495b5d041b217977e24030960f4f2178e translatedAt=2026-11-19T02:49:53.461Z pushedAt=2026-11-19T05:54:06.664Z nl=2 -->

## Supported Products

<!-- npu="950" id1 -->
- Ascend 950PR&950DT products: Supported
<!-- end id1 -->
<!-- npu="A3" id2 -->
- Atlas A3 products: Not supported
<!-- end id2 -->
<!-- npu="910b" id3 -->
- Atlas A2 products: Not supported
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

## Function Description

The header file path is `"tensor_api/experimental/arch/vector/reg_compute_sel.h"`, and the namespace is `asc::te::experimental`.

Selects the lower half or upper half of the source register, expands the bit width of each element to twice its original width, and writes the elements to the destination register in their original order.

- For unsigned integers, zero extension is performed, padding the high bits with 0.
- For signed integers, sign extension is performed, preserving the sign of each element.
- For the bool type, each bit in the selected half is expanded to two bits, with the low bit being the original bit and the high bit padded with 0.

This API takes effect only on the AIV.

## Prototype

```cpp
template <high_low_part part = high_low_part::low, typename T>
__simd_callee__ inline decltype(auto) unpack(reg_tensor<T> src);
```

## Parameters

**Table 1** Template parameter description

| Parameter | Description |
| --- | --- |
| part | Selection position of the source data. `high_low_part::low` selects the lower half of the source register, and `high_low_part::high` selects the upper half. The default value is `high_low_part::low`. |
| T | Element type of the source register. For supported types, see [data type](#data-type). |

**Table 2** Function parameter description

| Parameter | Input/Output | Description |
| --- | --- | --- |
| src | Input | Source operand, of type [reg_tensor&lt;T&gt;](../reg_tensor/reg_tensor.md). |

## Data Type

| T | Return Value Type |
| --- | --- |
| bool | reg_tensor&lt;bool&gt; |
| int8_t | reg_tensor<int16_t> |
| uint8_t | reg_tensor<uint16_t> |
| int16_t | reg_tensor<int32_t> |
| uint16_t | reg_tensor<uint32_t> |

## Return Value

The return type is deduced from T. For the specific mapping, see the data type table. `reg` stores the extended data, and `mask` is `all_mask<result element type>().reg`, that is, the all-valid mask set according to the result element type.

## Constraints

- `part` supports only `high_low_part::low` and `high_low_part::high`.
- The expansion computation does not use `src.mask` to mask elements. Ensure that the data in the selected half of the source register is initialized.
- The return value automatically carries the all-valid mask set according to the result element type, so there is no need to bind a mask to the source register. When only part of the result is used, rebind a mask suitable for the valid positions through [with_mask](../reg_tensor/reg_tensor.md#with_mask).

## Example

The following code snippet is called in VF. The low half of `src.reg` is initialized and sign-extended to int16_t. The returned result automatically carries the all-valid mask of the int16_t type, so there is no need to bind an additional mask.

```cpp
__simd_callee__ inline asc::te::experimental::reg_tensor<int16_t> unpack_example(
    asc::te::experimental::reg_tensor<int8_t> src)
{
    return asc::te::experimental::unpack<asc::te::experimental::high_low_part::low>(src);
}
```
