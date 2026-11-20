# cast

<!-- md-trans-meta sourceCommit=63c0b0eaa8a2923ff3c351fb2a0592ad269f94d6 translatedAt=2026-11-19T02:59:37.553Z pushedAt=2026-11-19T05:54:06.678Z nl=2 -->

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

Header file path: `"tensor_api/experimental/arch/vector/type_conversion.h"`.

Namespace: `asc::te::experimental`.

`cast` is used for register data type conversion. It converts the valid elements in the source operand `reg_tensor<SrcType>` to `DstType`, supporting floating-point to integer, floating-point to floating-point, integer to floating-point, and integer to integer conversions. Bit-width or precision changes may occur during conversion. The following behaviors can be configured through the compile-time `cast_options`:

- [cast_layout](../aux_data_types/cast_layout.md): When the source type and destination type bit widths differ, the valid computation amount of a single instruction is determined by the type with a larger bit width. This configuration specifies the position of the element with a smaller bit width in the same group of elements.
- [cast_round_mode](../aux_data_types/cast_round_mode.md): Specifies the rounding mode when precision loss occurs.
- [cast_sat_mode](../aux_data_types/cast_sat_mode.md): Specifies whether to use saturation or non-saturation processing when the conversion result exceeds the destination type representation range.

The API uses `src.mask` to filter the valid elements in the source operand. The underlying conversion uses zeroing mode, and the destination elements not selected by the mask are set to 0. The return value saves the conversion result and passes `src.mask` through unchanged. When a bit width change occurs, the result and mask must be processed according to the destination data layout before moving out. For details, see [cast_layout and Bit Width Change](#cast_layout-and-bit-width-change).

## Prototype

```cpp
template <typename DstType,
          const cast_options& opt,
          typename SrcType>
__simd_callee__ inline reg_tensor<DstType> cast(const reg_tensor<SrcType>& src)
```

## Parameters

**Table 1** Template parameter description

| Parameter | Description |
| --- | --- |
| `DstType` | Destination element type, which must be different from `SrcType`. For supported data type combinations, see [Data Type](#data-type). |
| `opt` | Compile-time constant of the `cast_options` type that must be explicitly specified. For the configuration combinations supported by different conversions, see [Data Type](#data-type). |
| `SrcType` | Source element type, which is automatically deduced from the function argument and does not need to be specified explicitly. |

**Table 2** Parameters

| Parameter | Input/Output | Description |
| --- | --- | --- |
| `src` | Input | Source register. `src.reg` holds the data to be converted, and `src.mask` indicates the valid elements that participate in the conversion. Both the data and the mask must be initialized before the call. |

The `cast_options` configuration items are described as follows:

| Configuration Item | Values and Description |
| --- | --- |
| `layout` | [cast_layout](../aux_data_types/cast_layout.md) type. `zero`, `one`, `two`, and `three` indicate that the element with a smaller bit width is at positions 0, 1, 2, and 3 in the same group of elements, respectively. The available positions are determined by the bit width ratio. For same-bit-width conversions where position selection is not required, use `unknown`. |
| `round` | [cast_round_mode](../aux_data_types/cast_round_mode.md) type. `rint` indicates rounding to the nearest even. `round` indicates rounding to the nearest with ties away from zero. `floor`, `ceil`, and `trunc` indicate rounding toward negative infinity, positive infinity, and zero, respectively. Some conversions also support `odd` or `hybrid`. When no rounding mode needs to be selected, use `unknown`. |
| `sat` | [cast_sat_mode](../aux_data_types/cast_sat_mode.md) type. `not_sat` indicates the non-saturation mode, and `sat` indicates the saturation mode. When no saturation mode needs to be selected, use `unknown`. For specific behavior, see [Saturation and Non-saturation Modes](#saturation-and-non-saturation-modes). |

> [!NOTE]
> The three fields of `cast_options{}` default to `unknown`. Currently supported conversions do not accept all three fields being `unknown` simultaneously. Set the corresponding fields according to the type combination.

## Data Type

The following table shows the complete range of currently supported configurations. The values in the `layout`, `round`, and `sat` columns of each row can be combined arbitrarily; types or configuration combinations that are not listed will cause a compile-time error.

| Source Type → Destination Type | `layout` | `round` | `sat` |
| --- | --- | --- | --- |
| `bfloat16_t → float`, `half → float`, `int16_t → float`, `int8_t → half`, `uint8_t → half`, `int16_t → int32_t`, `int32_t → int64_t`, `int8_t → int16_t`, `int16_t → uint32_t`, `uint16_t → uint32_t`, `uint8_t → uint16_t`, `hifloat8_t → half` | `zero`, `one` | `unknown` | `unknown` |
| `float → bfloat16_t`, `bfloat16_t → int32_t`, `float → int16_t`, `float → int64_t`, `half → int8_t`, `half → uint8_t` | `zero`, `one` | `floor`, `rint`, `round`, `ceil`, `trunc` | `not_sat`, `sat` |
| `bfloat16_t → half`, `float → int32_t`, `half → int16_t` | `unknown` | `floor`, `rint`, `round`, `ceil`, `trunc` | `not_sat`, `sat` |
| `half → bfloat16_t`, `int32_t → float`, `int16_t → half` | `unknown` | `floor`, `rint`, `round`, `ceil`, `trunc` | `unknown` |
| `float → half` | `zero`, `one` | `floor`, `rint`, `round`, `ceil`, `trunc`, `odd` | `not_sat`, `sat` |
| `int64_t → float`, `half → int32_t` | `zero`, `one` | `floor`, `rint`, `round`, `ceil`, `trunc` | `unknown` |
| `int32_t → int16_t`, `int64_t → int32_t`, `int16_t → uint8_t`, `int32_t → uint16_t`, `uint32_t → int16_t`, `uint16_t → uint8_t`, `uint32_t → uint16_t` | `zero`, `one` | `unknown` | `not_sat`, `sat` |
| `int8_t → int32_t`, `uint8_t → uint32_t`, `hifloat8_t → float`, `int4x2_t → half`, `int4x2_t → int16_t`, `fp4x2_e1m2_t → bfloat16_t`, `fp4x2_e2m1_t → bfloat16_t`, `int4x2_t → bfloat16_t`, `fp8_e4m3fn_t → float`, `fp8_e5m2_t → float` | `zero`, `one`, `two`, `three` | `unknown` | `unknown` |
| `int32_t → uint8_t`, `uint32_t → uint8_t` | `zero`, `one`, `two`, `three` | `unknown` | `not_sat`, `sat` |
| `float → hifloat8_t` | `zero`, `one`, `two`, `three` | `round`, `hybrid` | `not_sat`, `sat` |
| `half → hifloat8_t` | `zero`, `one` | `round`, `hybrid` | `not_sat`, `sat` |
| `half → int4x2_t` | `zero`, `one`, `two`, `three` | `floor`, `rint`, `round`, `ceil`, `trunc` | `not_sat`, `sat` |
| `float → fp8_e4m3fn_t`, `float → fp8_e5m2_t` | `zero`, `one`, `two`, `three` | `rint` | `not_sat`, `sat` |
| `bfloat16_t → fp4x2_e1m2_t`, `bfloat16_t → fp4x2_e2m1_t` | `zero`, `one`, `two`, `three` | `floor`, `rint`, `round`, `ceil`, `trunc` | `unknown` |

The types supported by `cast` are register element types. For conversions involving `int4x2_t`, you can load a Tensor whose element type in UB is `int4b_t` and then use the returned `reg_tensor<int4x2_t>` to participate in the conversion. The result of `cast<int4x2_t>` can be written to an `int4b_t` Tensor through `store`.

## Return Value

Returns `reg_tensor<DstType>`. The `reg` of the return value saves the conversion result, and `mask` is the same as `src.mask`.

## Constraints

- `DstType` and `SrcType` must be different; conversion between the same type will cause a compile-time error.
- `src.reg` and `src.mask` must be initialized before calling. `cast` interprets the mask according to the source operand type and data layout, and will not check whether the mask matches the source data.
- The return value only passes `src.mask` through unchanged and will not regenerate the mask according to `DstType`. When a bit width change occurs, the caller must guarantee that the mask, `layout`, and the move-in/move-out sideband modes match each other.
- The API only supports the complete configuration combinations listed in the [Data Type](#data-type) table. Other type or option combinations will cause a compile-time error.
- Register calculation should be completed in the `__simd_vf__` function or the `__simd_callee__` function.

## Key Features


### Saturation and Non-saturation Modes

In different conversion scenarios, the meanings of `cast_sat_mode::not_sat` and `cast_sat_mode::sat` are as follows.

| Conversion Scenario | `not_sat` | `sat` |
| --- | --- | --- |
| Floating-point to integer | When the input exceeds the destination type range, the result is truncated to the data width of the destination format, that is, the low-order bits are retained. For example, when the `half` value 257 is converted to `uint8_t`, the result is 1. When the input is positive or negative infinity, the corresponding boundary value of the destination type is returned; when the input is NaN, 0 is returned. | When the input exceeds the destination type range, the result is clamped to the destination type boundary. For example, when the `half` value 257 is converted to `uint8_t`, the result is 255, and when negative infinity is converted to `uint8_t`, the result is 0; when the input is NaN, 0 is returned. |
| Floating-point to floating-point | When the destination type can represent NaN or positive/negative infinity, the corresponding special value is retained; when a finite input exceeds the destination type range, it is converted according to the non-saturation rules of the destination floating-point format. | When the input exceeds the destination type range, the result is clamped to the destination type boundary; NaN is converted to 0 by default. |
| Integer to floating-point | This scenario does not provide the `sat` configuration. The conversion precision is determined by the destination floating-point format and the `round` configuration supported by the corresponding row. | Not supported. |
| Integer-to-integer | For a narrowing conversion where the result exceeds the destination type range, the result is truncated to the data width of the destination format. For example, converting the `int32_t` value 256 to `uint8_t` yields 0. A general widening conversion maintains the value. In particular, for `int16_t` to `uint32_t` conversion, `opt.sat` must be configured as the fixed `unknown` value in the table, but the conversion itself uses saturation semantics, with a negative number yielding 0. | When the input exceeds the destination type range, the result is clamped to the destination type boundary. |

### Rounding Mode

The rounding mode affects the result only when the conversion may lose precision. For example, floating-point to integer conversion, wide floating-point to narrow floating-point conversion, and conversion from `int32_t` or `int64_t` to `float` may need rounding; an exact widening conversion will not additionally change the exact result. You must use the mode supported by the corresponding conversion in the [Data Type](#data-type) table; unsupported modes will cause a compile-time error.

Conversion from `float` or `half` to `hifloat8_t` supports `cast_round_mode::round` and `cast_round_mode::hybrid`.

### cast_layout and Bit Width Change

When the source and destination types have different bit widths, the number of logical elements processed by a single conversion instruction is determined by the type with the larger bit width. `layout` describes the position of the element with the smaller bit width in each group of data:

- When the bit-width ratio is 2, `zero` or `one` can be selected, corresponding to the 0th or 1st position in each group.
- When the bit-width ratio is 4, `zero`, `one`, `two`, or `three` can be selected, corresponding to the 0th through 3rd positions in each group.
- When the source and destination types have the same bit width, use `unknown`. The specific value is subject to the [Data Type](#data-type) table.

`cast` only completes register calculation and will not implicitly adjust the data layout in the Unified Buffer (UB). When processing tensor data consecutively, `asc::te::experimental::load`, `cast_options::layout`, and the data layout when moving out should be kept consistent. When the source type is 2x or 4x narrower than the destination type, `load_sideband_mode::unpack` or `load_sideband_mode::unpack4` can be used respectively to move in, and then directly move out according to the destination type. When the source type is wider than the destination type, the valid results after conversion are scattered in the destination register; if a contiguous array is required, extract the valid results first, then regenerate the destination type mask and directly move out.

For the tail block, it is recommended to calculate the number of logical elements per register according to the type with a larger bit width between the source and destination types, and call `update_mask` with the same type to generate the conversion mask. For narrowing conversion, after extracting the valid results, the move-out mask must also be regenerated according to the destination type and the number of valid elements in this round; the conversion mask cannot be directly reused.

`store` only writes register data to the corresponding UB positions according to the mask, and will not automatically gather the scattered valid results. If a contiguous all-valid or prefix-valid mask is set for the destination type and then directly moved out, the 0s within the group will also be written out; if only the positions of the valid results are selected, intervals still remain in the output instead of a contiguous array. Reusing the conversion mask as-is also does not guarantee that the result positions within the destination group are selected.

The purpose of using [deinterleave](../data_reorder/deinterleave.md) is to remove these intervals within the group, concentrate the valid results to the front of the register, and then directly move out according to the destination type. For example, for quadruple narrowing using `layout::two`, the result extraction process is as follows, where `a`, `b`, and `c` indicate the values after conversion:

```text
after conversion:                 [0, 0, a, 0, 0, 0, b, 0, 0, 0, c, 0, ...]
first deinterleave fetch:    [0, a, 0, b, 0, c, ...]
second deinterleave fetch:   [a, b, c, ...]
```

`deinterleave` places the even-indexed elements of the two input registers into `first` and the odd-indexed elements into `second`. Here, the same register is used as two inputs to utilize only the even/odd index filtering function: the first filtering selects the odd/even set where the target position resides; for quadruple narrowing, the same operation is performed once more on the filtered result to select the second-level odd/even index of the target position within that set. In the table below, the two inputs of each deinterleaving are the same, and the second deinterleaving uses the result selected by the first as its two inputs:

| Narrowing bit width ratio | `layout` | First deinterleave selection | Second deinterleave selection |
| --- | --- | --- | --- |
| 2:1 | `zero` | `first` | not required |
| 2:1 | `one` | `second` | not required |
| 4:1 | `zero` | `first` | `first` |
| 4:1 | `one` | `second` | `first` |
| 4:1 | `two` | `first` | `second` |
| 4:1 | `three` | `second` | `second` |

This table applies to ordinary types and tail blocks that are complete or have a contiguous prefix of valid elements. When the same register is used as two inputs, the filtered data appears repeatedly in the output, and the conversion result of this round occupies only the first half or first quarter of the final register. Currently, `deinterleave` sets a full mask for the output, so before moving out, a destination type mask must be generated separately using the number of valid elements in this round to avoid writing out duplicate data. A separate move-out count should be used, and `remain` that has already been consumed by the conversion mask should not be updated again. For non-consecutive valid elements, matching data and mask reordering are also required.

### 4-bit Packed Type

On registers, int4b_t packs two logical elements into one byte. The memory Tensor uses `int4b_t`, while the register uses `int4x2_t`. Each storage unit of `int4x2_t`, `fp4x2_e2m1_t`, and `fp4x2_e1m2_t` contains two 4-bit elements. When these types are converted to or from 16-bit types, the instruction reads and writes two elements as a group. When converting from a 16-bit type to a 4-bit packed type, the even bits of each group's mask serve as the valid control bits.

## Example

The following example processes one conversion block at a time. `validCount` indicates the number of valid elements in this round, ranging from 1 to 64. Taking a 256 Byte register as an example, up to 64 32-bit logical elements can be processed at once. The source and destination Tensors use the types in the corresponding examples respectively, and the start addresses meet the move alignment requirements. Even if `validCount` is less than 64, the source buffer must still accommodate the complete move-in range: 64 `half` values in the first example, and 64 `float` and 64 `int32_t` values in the latter two examples respectively. The following input and output list only the valid elements. The remaining source elements in the buffer can be filled with 0, and the values outside the valid destination range maintain their original values.

### Bit Width Increases from Small to Large

The following example converts `half` to `int32_t` using rounding toward negative infinity. The source type bit width is half of the destination type, so the `unpack` mode is used to move in, and the mask is generated with the larger bit width `int32_t`.

| `validCount` | Valid input (`half`) | Expected valid output (`int32_t`) |
| --- | --- | --- |
| 4 | `[1.5, -2.25, 3.0, 4.75]` | `[1, -3, 3, 4]` |

```cpp
constexpr asc::te::experimental::cast_options f16ToS32Options = {
    asc::te::experimental::cast_layout::zero,
    asc::te::experimental::cast_round_mode::floor,
    asc::te::experimental::cast_sat_mode::unknown
};

template <typename SrcTensor, typename DstTensor>
__simd_vf__ inline void cast_f16_to_s32(const SrcTensor src, DstTensor dst, uint32_t validCount)
{
    auto srcReg = asc::te::experimental::load<asc::te::experimental::load_sideband_mode::unpack>(
        src, asc::te::make_coord(0));
    uint32_t remain = validCount;
    srcReg.with_mask(asc::te::experimental::update_mask<int32_t>(remain));

    auto dstReg = asc::te::experimental::cast<int32_t, f16ToS32Options>(srcReg);
    asc::te::experimental::store(dst, asc::te::make_coord(0), dstReg);
}
```

The move-in range in this example is 64 `half` values. The mask selects `validCount` logical elements at 32-bit granularity, and the conversion result can be directly written out using the same mask. When processing the complete 64 elements, `all_mask<int32_t>()` can also be used.

### Bit Width Decreases

The following example converts `float` to `int16_t` using rounding to nearest even and saturation mode. `layout::zero` writes the result at position 0 of each group of two `int16_t` slots. The consecutive results are fetched through one deinterleaving, and after resetting the mask at 16-bit granularity, they are directly moved out.

| `validCount` | Valid input (`float`) | Expected valid output (`int16_t`) |
| --- | --- | --- |
| 4 | `[1.5, 2.5, -1.5, 40000.0]` | `[2, 2, -2, 32767]` |

```cpp
constexpr asc::te::experimental::cast_options f32ToS16Options = {
    asc::te::experimental::cast_layout::zero,
    asc::te::experimental::cast_round_mode::rint,
    asc::te::experimental::cast_sat_mode::sat
};

template <typename SrcTensor, typename DstTensor>
__simd_vf__ inline void cast_f32_to_s16(const SrcTensor src, DstTensor dst, uint32_t validCount)
{
    auto srcReg = asc::te::experimental::load(src, asc::te::make_coord(0));
    uint32_t castRemain = validCount;
    srcReg.with_mask(asc::te::experimental::update_mask<float>(castRemain));

    auto dstReg = asc::te::experimental::cast<int16_t, f32ToS16Options>(srcReg);
    auto compact = asc::te::experimental::deinterleave(dstReg, dstReg).first;
    uint32_t storeRemain = validCount;
    compact.with_mask(asc::te::experimental::update_mask<int16_t>(storeRemain));
    asc::te::experimental::store(dst, asc::te::make_coord(0), compact);
}
```

### Quadruple Narrowing and Selecting Two Positions

The following example converts `int32_t` to `uint8_t` with saturation, and writes the result to the second of each group of 4 bytes through `layout::two`. The two deinterleavings select `first` and `second` respectively, extract the result to the first quarter before reaching the register, reset the mask at 8-bit granularity, and then directly move out. If `layout::three` is used instead, the first deinterleaving should select `second`, and the second should still select `second`.

| `validCount` | Valid input (`int32_t`) | Expected valid output (`uint8_t`) |
| --- | --- | --- |
| 4 | `[-1, 0, 255, 256]` | `[0, 0, 255, 255]` |

```cpp
constexpr asc::te::experimental::cast_options s32ToU8TwoOptions = {
    asc::te::experimental::cast_layout::two,
    asc::te::experimental::cast_round_mode::unknown,
    asc::te::experimental::cast_sat_mode::sat
};

template <typename SrcTensor, typename DstTensor>
__simd_vf__ inline void cast_s32_to_u8_two(const SrcTensor src, DstTensor dst, uint32_t validCount)
{
    auto srcReg = asc::te::experimental::load(src, asc::te::make_coord(0));
    uint32_t remain = validCount;
    srcReg.with_mask(asc::te::experimental::update_mask<int32_t>(remain));

    auto dstReg = asc::te::experimental::cast<uint8_t, s32ToU8TwoOptions>(srcReg);
    auto stage1 = asc::te::experimental::deinterleave(dstReg, dstReg).first;
    auto compact = asc::te::experimental::deinterleave(stage1, stage1).second;
    uint32_t storeRemain = validCount;
    compact.with_mask(asc::te::experimental::update_mask<uint8_t>(storeRemain));
    asc::te::experimental::store(dst, asc::te::make_coord(0), compact);
}
```
