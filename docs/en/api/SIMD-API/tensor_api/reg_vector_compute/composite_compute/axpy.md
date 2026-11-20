# axpy

<!-- md-trans-meta sourceCommit=db86c75495b5d041b217977e24030960f4f2178e translatedAt=2026-11-19T02:46:29.337Z pushedAt=2026-11-19T05:54:06.657Z nl=2 -->

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

This API multiplies `src` with scalar `scalar` element by element according to `src.mask`, adds the result to the original value of `dst`, updates `dst.reg` in place, keeps `dst.mask` unchanged, and returns the updated `dst` by value. The formula is as follows:

$$
dst_i = scalar \times src_i + dst_i
$$

## Prototype

```cpp
template <typename T>
__simd_callee__ inline reg_tensor<T> axpy(reg_tensor<T>& dst, const reg_tensor<T>& src, const T& scalar)
```

## Parameters

**Table 1** Template parameter description

| Parameter | Description |
| --- | --- |
| T | Operand data type. For supported data types, see [Data Type](#data-type). |

**Table 2** Parameters

| Parameter | Input/Output | Description |
| --- | --- | --- |
| dst | Input/Output | Addition operand and result, type is `reg_tensor<T>`. |
| src | Input | Source operand multiplied with scalar, type is `reg_tensor<T>`. `src.mask` marks the elements that participate in the computation. |
| scalar | Input | Scalar multiplier, whose type must be exactly the same as `T`. |

## Return Value

Returns the updated `dst` by value, whose type is `reg_tensor<T>`. The `mask` of the return value is the same as `dst.mask` before calling.

## Data Type

**Table 3** Data type combinations

| src | scalar | dst |
| --- | --- | --- |
| half | half | half |
| float | float | float |

## Constraints

- `src.mask` must be set in advance through the [with_mask](../reg_tensor/reg_tensor.md#with_mask) API to determine the elements that participate in the computation. Before calling, you must ensure that `src.reg` and `dst.reg` at these positions are initialized.
- `dst.mask` does not participate in selecting the elements to compute. Before subsequent operations use the validity mask of the result, you must ensure that `dst.mask` is initialized and consistent with the required output range.

## Example

```cpp
#include "tensor_api/experimental/vector_ops.h"

template <typename DstTensor, typename SrcTensor>
__simd_vf__ inline void axpy_vf(
    DstTensor dst, const SrcTensor src, float scalar, uint16_t repeat_times, uint32_t total,
    uint32_t one_repeat_size)
{
    using data_type = typename DstTensor::data_type;
    uint32_t remain = total;
    for (uint16_t i = 0; i < repeat_times; ++i) {
        const uint32_t offset = i * one_repeat_size;
        const auto coord = asc::te::make_coord(offset);
        auto mask = asc::te::experimental::update_mask<data_type>(remain);
        auto dst_reg = asc::te::experimental::load(dst, coord).with_mask(mask);
        auto src_reg = asc::te::experimental::load(src, coord).with_mask(mask);
        asc::te::experimental::axpy(dst_reg, src_reg, scalar);
        asc::te::experimental::store(dst, coord, dst_reg);
    }
}
```
