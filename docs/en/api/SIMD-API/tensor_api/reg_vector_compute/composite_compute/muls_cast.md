# muls_cast

<!-- md-trans-meta sourceCommit=dc9f009438d2a4868c62d066ed76526a6e83e20c translatedAt=2026-11-19T02:46:34.297Z pushedAt=2026-11-19T05:54:06.658Z nl=2 -->

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

Header file path: `"tensor_api/experimental/arch/vector/composite_compute.h"`.

This API multiplies `src` by the scalar `scalar` element-wise according to `src.mask`, then converts the result to the `half` type in `CAST_ROUND` mode and returns it. The calculation formula is as follows:

$$
dst_i = cast\_round\_to\_f16(src_i \times scalar)
$$

The default layout is [cast_layout](../aux_data_types/cast_layout.md)::zero. The conversion result is written to position 0 (even index) of each group of two `half` elements in the return register, and position 1 is set to zero.

## Prototype

```cpp
template <cast_layout Layout = cast_layout::zero, typename T, typename U>
__simd_callee__ inline reg_tensor<T> muls_cast(const reg_tensor<U>& src, const U& scalar)
```

## Parameters

**Table 1** Template parameter description

| Parameter | Description |
| --- | --- |
| Layout | [cast_layout](../aux_data_types/cast_layout.md) enumeration type, result layout. Supports `cast_layout::zero` and `cast_layout::one`, defaulting to `cast_layout::zero`. |
| T | Element type of the return value. For supported data types, see [Data Type](#data-type). |
| U | Source operand and scalar type. For supported data types, see [Data Type](#data-type). |

**Table 2** Parameters

| Parameter | Input/Output | Description |
| --- | --- | --- |
| src | Input | Source operand of type `reg_tensor<U>`. `src.mask` marks the elements that participate in the computation. A value of `1` at the corresponding position indicates participation, and `0` indicates no participation. |
| scalar | Input | Scalar multiplier of type `U`. |

## Return Value

Returns `reg_tensor<T>`. When `Layout` is `cast_layout::zero`, the conversion result is written to position 0 (even index) of each pair of two `half` elements, and position 1 is set to zero. When `Layout` is `cast_layout::one`, the conversion result is written to position 1 (odd index), and position 0 is set to zero. The `mask` of the return value is the same as `src.mask`.

## Data Type

**Table 3** Data type combinations

| src | scalar | dst |
| --- | --- | --- |
| float | float | half |

## Constraints

- `src.mask` must be set in advance through the `with_mask` API. If it is not set, the content of the mask is undefined, which may cause incorrect element positions to participate in the computation.
- When the corresponding position of `src.mask` is `0`, the corresponding position of the return value `reg` is set to zero.
- Type conversion is rounded according to the `CAST_ROUND` mode.

## Example

```cpp
#include "tensor_api/experimental/vector_ops.h"

template <typename SrcTensor, typename DstTensor>
__simd_vf__ inline void muls_cast_vf(
    const SrcTensor src, DstTensor dst, float scalar, uint16_t repeat_times, uint32_t total,
    uint32_t src_one_repeat_size, uint32_t dst_one_repeat_size)
{
    uint32_t remain = total;
    for (uint16_t i = 0; i < repeat_times; ++i) {
        const auto src_coord = asc::te::make_coord(i * src_one_repeat_size);
        const auto dst_coord = asc::te::make_coord(i * dst_one_repeat_size);
        auto mask = asc::te::experimental::update_mask<float>(remain);
        auto src_reg = asc::te::experimental::load(src, src_coord).with_mask(mask);
        auto dst_reg = asc::te::experimental::muls_cast<asc::te::experimental::cast_layout::one, half>(
            src_reg, scalar);
        asc::te::experimental::store(dst, dst_coord, dst_reg);
    }
}
```
