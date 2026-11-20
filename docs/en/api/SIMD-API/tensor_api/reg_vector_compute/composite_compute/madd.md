# madd

<!-- md-trans-meta sourceCommit=7e60dec365f6254fedd6bbb74d7032e62730ccec translatedAt=2026-10-27T03:37:50.954Z pushedAt=2026-10-27T13:03:35.401Z nl=2 -->

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

This API multiplies `dst` by `src0` element-wise according to `dst.mask`, then adds the result to `src1`, and returns the computed result. The calculation formula is as follows:

$$
dst_i = dst_i \times src0_i + src1_i
$$

## Function Prototype

```cpp
template <typename T>
__simd_callee__ inline reg_tensor<T> madd(reg_tensor<T>& dst, const reg_tensor<T>& src0, const reg_tensor<T>& src1)
```

## Parameters

**Table 1** Template parameter description

| Parameter | Description |
| --- | --- |
| T | Operand data type. For supported data types, see [Data Type](#data-type). |

**Table 2** Parameters

| Parameter | Input/Output | Description |
| --- | --- | --- |
| dst | Input/Output | First multiplier and result. The API directly updates `dst.reg`, and `dst.mask` controls the elements involved in the computation. |
| src0 | Input | Second multiplier. `src0.mask` does not participate in this computation. |
| src1 | Input | Addend. `src1.mask` does not participate in this computation. |

## Return Value

Returns `reg_tensor<T>`, which stores the result of `dst * src0 + src1`. The `mask` of the return value is the same as `dst.mask`.

## Data Type

**Table 3** Data type combinations

| src0 | src1 | dst |
| --- | --- | --- |
| half | half | half |
| bfloat16_t | bfloat16_t | bfloat16_t |
| float | float | float |

## Constraints

- `dst.mask` must be set in advance through the `with_mask` API. If it is not set, the content of the mask is undefined, which may cause incorrect element positions to participate in the computation.
- When the corresponding position of `dst.mask` is `0`, the corresponding position of the return value `reg` is set to zero.

## Example

```cpp
#include "tensor_api/experimental/vector_ops.h"

template <typename DstTensor, typename Src0Tensor, typename Src1Tensor>
__simd_vf__ inline void madd_vf(
    DstTensor dst, const Src0Tensor src0, const Src1Tensor src1, uint16_t repeat_times, uint32_t total,
    uint32_t one_repeat_size)
{
    using data_type = typename DstTensor::data_type;
    uint32_t remain = total;
    for (uint16_t i = 0; i < repeat_times; ++i) {
        const uint32_t offset = i * one_repeat_size;
        const auto coord = asc::te::make_coord(offset);
        auto mask = asc::te::experimental::update_mask<data_type>(remain);
        auto dst_reg = asc::te::experimental::load(dst, coord).with_mask(mask);
        auto src0_reg = asc::te::experimental::load(src0, coord).with_mask(mask);
        auto src1_reg = asc::te::experimental::load(src1, coord).with_mask(mask);
        dst_reg = asc::te::experimental::madd(dst_reg, src0_reg, src1_reg);
        asc::te::experimental::store(dst, coord, dst_reg);
    }
}
```
