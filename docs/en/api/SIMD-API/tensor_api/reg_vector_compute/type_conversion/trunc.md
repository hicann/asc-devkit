# trunc

<!-- md-trans-meta sourceCommit=98568e61facccbf29db5353153551ecc6fe6458d translatedAt=2026-11-19T03:01:48.897Z pushedAt=2026-11-19T05:54:06.679Z nl=2 -->

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

trunc rounds the floating-point elements selected by mask in the source register, and the result keeps the original data type. The rounding mode is specified by a template parameter, and `cast_round_mode::trunc` is used by default.

For finite floating-point numbers, the calculation relationship of each mode is as follows, where `x = src[i]`:

| Mode | Calculation relationship | Rounding rule |
| --- | --- | --- |
| cast_round_mode::trunc | dst[i] = trunc(x) | Round toward zero, discarding the fractional part; for example, 3.75 -> 3.0, -3.75 -> -3.0. |
| cast_round_mode::round | dst[i] = round(x) | Round to the nearest integer; round half away from zero, for example, 3.1 -> 3.0, 2.5 -> 3.0, -2.5 -> -3.0. |
| cast_round_mode::rint | dst[i] = rint(x) | Round to the nearest integer; round half to even, for example, 3.1 -> 3.0, 2.5 -> 2.0, -2.5 -> -2.0. |
| cast_round_mode::ceil | dst[i] = ceil(x) | Round toward positive infinity, for example, 2.5 -> 3.0, -2.5 -> -2.0. |
| cast_round_mode::floor | dst[i] = floor(x) | Round toward negative infinity, for example, 2.5 -> 2.0, -2.5 -> -3.0. |

When the input itself is an integer value, the value remains unchanged. The API uses `src.mask` to select the elements participating in calculation, and sets unselected return data elements to 0.

## Prototype

```cpp
template <cast_round_mode Mode = cast_round_mode::trunc, typename T>
__simd_callee__ inline reg_tensor<T> trunc(const reg_tensor<T>& src)
```

## Parameters

**Table 1** Template parameter description

| Parameter | Description |
| --- | --- |
| `Mode` | Rounding mode, defaulting to `cast_round_mode::trunc`. Supports `trunc` (round toward zero), `round` (round to the nearest integer, round half away from zero), `rint` (round to the nearest integer, round half to even), `ceil` (round toward positive infinity), and `floor` (round toward negative infinity). |
| `T` | Source and destination element type, automatically deduced from the function arguments and does not need to be explicitly specified. For supported data types, see [Data Type](#data-type). |

**Table 2** Parameters

| Parameter | Input/Output | Description |
| --- | --- | --- |
| `src` | Input | Source register, of type `reg_tensor<T>`. `src.reg` holds the data to be processed, and `src.mask` indicates the valid elements that participate in the computation. Both the data and the mask must be initialized before the call. |

## Data Type

The destination element type is the same as the source element type. The supported data types are as follows.

| `T` | Source Operand Type | Destination Operand Type |
| --- | --- | --- |
| `half` | `reg_tensor<half>` | `reg_tensor<half>` |
| `bfloat16_t` | `reg_tensor<bfloat16_t>` | `reg_tensor<bfloat16_t>` |
| `float` | `reg_tensor<float>` | `reg_tensor<float>` |

This API does not perform conversion between floating-point and integer types. To change the element type, use the [cast](cast.md) API.

## Return Value

Returns `reg_tensor<T>`:

- `reg` saves the result rounded in the selected mode, and unselected elements are set to 0.
- `mask` is the same as `src.mask`. The API does not regenerate or extend the valid range.

## Constraints

- `src.reg` and `src.mask` must be initialized before the call. `asc::te::experimental::load` only initializes the `reg` of the returned object, and a mask matching the element type must be bound through `with_mask`.
- The API does not change the element type, and does not provide saturating or non-saturating mode configuration.
- The mask merge mode is fixed to zeroing, and the merging mode is not supported. Unselected return data elements are set to 0; when a single-source `asc::te::experimental::store` is used later, the write-back is still controlled by the mask passed in the return value.
- Register computation should be performed in a `__simd_vf__` function or a `__simd_callee__` function.

## Example

The following example rounds a segment of floating-point data in the Unified Buffer (UB) toward zero. [update_mask](../mask_reg_compute/update_mask.md)`<T>(remain)` generates the mask for the current loop based on the number of remaining elements to be processed, and automatically updates `remain` through a reference parameter. Therefore, the complete register block and the tail block can use the same computation flow.

```cpp
template <typename SrcTensor, typename DstTensor>
__simd_vf__ inline void TruncRegLoop(
    const SrcTensor src, DstTensor dst, uint32_t dataSize, uint32_t columnCount)
{
    using T = typename SrcTensor::data_type;
    using RegType = typename asc::te::experimental::reg_tensor<T>::reg_type;
    static_assert(AscendC::Std::is_same_v<T, typename DstTensor::data_type>,
        "trunc requires matching source and destination data types");

    constexpr uint32_t ONE_REPEAT = sizeof(RegType) / sizeof(T);
    const uint32_t repeatTimes = (dataSize + ONE_REPEAT - 1) / ONE_REPEAT;
    uint32_t remain = dataSize;

    for (uint16_t i = 0; i < repeatTimes; ++i) {
        const uint32_t offset = i * ONE_REPEAT;
        const auto coord = asc::te::make_coord(offset / columnCount, offset % columnCount);

        auto srcReg = asc::te::experimental::load(src, coord);
        srcReg.with_mask(asc::te::experimental::update_mask<T>(remain));

        auto dstReg = asc::te::experimental::trunc(srcReg);
        asc::te::experimental::store(dst, coord, dstReg);
    }
}
```

> [!NOTE]  
> `asc::te::experimental::load` loads one complete register block at a time. When the preceding tail block approach is used, the source UB Tensor must reserve sufficient readable space at the end of the logical data; `update_mask` only controls the valid elements that participate in calculation and write-back, and does not reduce the actual loaded width.
