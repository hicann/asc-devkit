# pack

<!-- md-trans-meta sourceCommit=db86c75495b5d041b217977e24030960f4f2178e translatedAt=2026-11-19T02:49:10.960Z pushedAt=2026-11-19T05:54:06.661Z nl=2 -->

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

Compresses the low half-width data of each element in the source register in the original order and writes it to the low half or high half of the return register. `part` selects the destination location of the result, not the high or low bits to be extracted from the source elements.

- When T is int16_t or uint16_t, the low 8 bits of each element are extracted, and the result element type is uint8_t.
- When T is int32_t or uint32_t, the low 16 bits of each element are extracted, and the result element type is uint16_t.
- When T is bool, the low bit is extracted from every two bits of the mask register, that is, the bits at even positions such as indexes 0, 2, and 4, and the result is still a mask register.

Signed integer inputs are also extracted bitwise from the low half-width, and an unsigned integer register is returned without preserving the sign of the source elements.

This API takes effect only on the AIV. When `part` is `high_low_part::low`, [asc_pack_to_low](../../../c_api/reg_compute/reg_permute_sel/asc_pack_to_low.md) is called; when `part` is `high_low_part::high`, [asc_pack_to_high](../../../c_api/reg_compute/reg_permute_sel/asc_pack_to_high.md) is called.

## Prototype

```cpp
template <high_low_part part = high_low_part::low, typename T>
__simd_callee__ inline decltype(auto) pack(reg_tensor<T> src);
```

## Parameters

**Table 1** Template parameter description

| Parameter | Description |
| --- | --- |
| part | Destination location of the compression result. `high_low_part::low` indicates the low half of the result register, and `high_low_part::high` indicates the high half. The default value is `high_low_part::low`. |
| T | Element type of the source register. For supported types, see [Data Type](#data-type). |

**Table 2** Function parameter description

| Parameter | Input/Output | Description |
| --- | --- | --- |
| src | Input | Source operand, whose type is [reg_tensor&lt;T&gt;](../reg_tensor/reg_tensor.md). |

## Data Type

| T | Return value type |
| --- | --- |
| bool | reg_tensor&lt;bool&gt; |
| int16_t | reg_tensor&lt;uint8_t&gt; |
| uint16_t | reg_tensor&lt;uint8_t&gt; |
| int32_t | reg_tensor&lt;uint16_t&gt; |
| uint32_t | reg_tensor&lt;uint16_t&gt; |

## Return Value

The return type is deduced from T. For the specific mapping, see the data type table. The specified half of `reg` stores the compression result, and `mask` is `all_mask<result element type>().reg`. An all-valid mask does not mean that the other half not written contains usable result data.

## Constraints

- The packing computation does not use `src.mask` to mask elements. Ensure that the source data involved in packing is initialized.
- The `mask` of the return value is set to an all-valid mask based on the result element type, and is not restricted to the specified half by `part`. Before subsequent computation or write-back, rebind an appropriate mask based on the half where the result resides, and avoid using the data in the other half not written. See [with_mask](../reg_tensor/reg_tensor.md#with_mask).

## Example

The following code snippet is called in VF. `src.reg` is initialized, the low 8 bits are extracted and written to the low half of the result, and no mask needs to be bound for the source register. The returned object carries an all-valid mask by default. Therefore, rebind a low-half mask of the uint8_t type for the result through [make_mask](../mask_reg_compute/make_mask.md) to avoid using the data in the other half.

```cpp
__simd_callee__ inline asc::te::experimental::reg_tensor<uint8_t> pack_example(
    asc::te::experimental::reg_tensor<uint16_t> src)
{
    auto result = asc::te::experimental::pack<asc::te::experimental::high_low_part::low>(src);
    const auto low_mask = asc::te::experimental::make_mask<asc::te::experimental::mask_pattern::half, uint8_t>();
    result.with_mask(low_mask);
    return result;
}
```
