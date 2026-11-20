# exp_diff

<!-- md-trans-meta sourceCommit=7e60dec365f6254fedd6bbb74d7032e62730ccec translatedAt=2026-10-27T03:36:37.713Z pushedAt=2026-10-27T13:03:35.399Z nl=2 -->

## Applicable Products

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
- AI Core in Atlas inference products: Not supported
<!-- end id5 -->
<!-- npu="310p" id6 -->
- Vector Core in Atlas inference products: Not supported
<!-- end id6 -->
<!-- npu="910" id7 -->
- Atlas training products: Not supported
<!-- end id7 -->

## Description

Header file path: `"tensor_api/experimental/arch/vector/composite_compute.h"`.

This API performs element-wise subtraction of `src0` and `src1` according to `src0.mask`, and then returns the result with the difference as the exponent of e. The calculation formula is as follows:

- When the input data type is `float`:

$$
dst_i = e^{(src0_i - src1_i)}
$$

- When the input data type is `half`:

$$
dst_i = e^{(cast\_f16\_to\_f32(src0_i) - cast\_f16\_to\_f32(src1_i))}
$$

When the input data type is `float`, the calculation is performed on consecutive elements, and `src_pos` does not need to be specified. When the input data type is `half`, `src_pos` must be specified to select the even positions or odd positions.

## Function Prototype

```cpp
template <typename T, typename U>
__simd_callee__ inline reg_tensor<T> exp_diff(const reg_tensor<U>& src0, const reg_tensor<U>& src1)

template <typename T, typename U, typename PositionType>
__simd_callee__ inline reg_tensor<T> exp_diff(const reg_tensor<U>& src0, const reg_tensor<U>& src1, PositionType src_pos)
```

## Parameters

**Table 1** Template parameter description

| Parameter | Description |
| --- | --- |
| T | Element type of the return value. For supported data types, see [data type](#data-type). It must be explicitly specified when calling. |
| U | Element type of the source operands. `half` indicates a 16-bit floating-point type. For supported data types, see [data type](#data-type). |
| PositionType | Position parameter type, automatically deduced from `src_pos`. |

**Table 2** Parameters

| Parameter | Input/Output | Description |
| --- | --- | --- |
| src0 | Input | Minuend, of type `reg_tensor<U>`. `src0.mask` is numbered by output element. When the corresponding position is `1`, the output element participates in the computation; when it is `0`, the output element does not participate. |
| src1 | Input | Subtrahend, of the same type as `src0`. `src1.mask` is not used in this computation. |
| src_pos | Input | Required when `U` is `half`. Only `ASC_POSITION_EVEN` and `ASC_POSITION_ODD` are supported, indicating selection of even positions and odd positions, respectively. |

## Return Value

Returns `reg_tensor<T>`. The `mask` of the return value is the same as `src0.mask`. When `U` is `half`, `ASC_POSITION_EVEN` and `ASC_POSITION_ODD` make the `i`-th output element read the `2 * i`-th and `2 * i + 1`-th elements of the source operand, respectively.

## Data Type

**Table 3** Data type combinations

| src0 | src1 | dst |
| --- | --- | --- |
| half | half | float |
| float | float | float |

## Constraints

- `src0.mask` must be set in advance through the `with_mask` API. If it is not set, the content of the mask is undefined, which may cause incorrect positions of the elements participating in the computation.
- When the corresponding position of `src0.mask` is `0`, the corresponding position of the returned value `reg` is set to zero.
- When `U` is `half`, the `i`-th bit of `src0.mask` controls the `i`-th `T` output element. The mask should be generated using `all_mask<T>`, `make_mask<..., T>`, or `update_mask<T>`.

## Example

The following example shows a typical calling method for `half` input.

```cpp
#include "tensor_api/experimental/vector_ops.h"

template <typename Src0Tensor, typename Src1Tensor, typename DstTensor>
__simd_vf__ inline void exp_diff_vf(
    const Src0Tensor src0, const Src1Tensor src1, DstTensor dst, uint16_t repeat_times, uint32_t total,
    uint32_t src_one_repeat_size, uint32_t dst_one_repeat_size)
{
    uint32_t remain = total;
    for (uint16_t i = 0; i < repeat_times; ++i) {
        const auto src_coord = asc::te::make_coord(i * src_one_repeat_size);
        const auto dst_coord = asc::te::make_coord(i * dst_one_repeat_size);
        auto mask = asc::te::experimental::update_mask<float>(remain);
        auto src0_reg = asc::te::experimental::load(src0, src_coord).with_mask(mask);
        auto src1_reg = asc::te::experimental::load(src1, src_coord).with_mask(mask);
        auto dst_reg = asc::te::experimental::exp_diff<float, half>(src0_reg, src1_reg, ASC_POSITION_ODD);
        asc::te::experimental::store(dst, dst_coord, dst_reg);
    }
}
```
