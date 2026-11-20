# fma

<!-- md-trans-meta sourceCommit=7e60dec365f6254fedd6bbb74d7032e62730ccec translatedAt=2026-10-27T03:37:20.144Z pushedAt=2026-10-27T13:03:35.398Z nl=2 -->

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

This API multiplies `src0` and `src1` element-wise according to `src0.mask`, then adds the result to `src2`, and returns the computed result. The calculation formula is as follows:

$$
dst_i = src0_i \times src1_i + src2_i
$$

## Function Prototype

```cpp
template <typename T>
__simd_callee__ inline reg_tensor<T> fma(const reg_tensor<T>& src0, const reg_tensor<T>& src1, const reg_tensor<T>& src2)
```

## Parameters

**Table 1** Template parameter description

| Parameter | Description |
| --- | --- |
| T | Operand data type. For supported data types, see [Data Type](#data-type). |

**Table 2** Parameters

| Parameter | Input/Output | Description |
| --- | --- | --- |
| src0 | Input | First multiplier, of type `reg_tensor<T>`. `src0.mask` marks the elements that participate in the computation. A value of `1` at a position indicates participation, and `0` indicates non-participation. |
| src1 | Input | Second multiplier, of type `reg_tensor<T>`. `src1.mask` is not used in this computation. |
| src2 | Input | Addend, of type `reg_tensor<T>`. `src2.mask` is not used in this computation. |

## Return Value

Returns `reg_tensor<T>`. `reg` stores the element-wise fused multiply-add result of `src0 * src1 + src2`, and the `mask` of the return value is the same as `src0.mask`.

## Data Type

**Table 3** Data type combinations

| src0 | src1 | src2 | dst |
| --- | --- | --- | --- |
| half | half | half | half |
| bfloat16_t | bfloat16_t | bfloat16_t | bfloat16_t |
| float | float | float | float |

## Constraints

- `src0.mask` must be set in advance through the `with_mask` API. If it is not set, the content of the mask is undefined, which may cause incorrect positions of the elements participating in the computation.
- When the corresponding position of `src0.mask` is `0`, the corresponding position of the returned value `reg` is set to zero.

## Example

```cpp
#include "tensor_api/experimental/vector_ops.h"

template <typename Src0Tensor, typename Src1Tensor, typename Src2Tensor, typename DstTensor>
__simd_vf__ inline void fma_vf(
    const Src0Tensor src0, const Src1Tensor src1, const Src2Tensor src2, DstTensor dst, uint16_t repeat_times,
    uint32_t total, uint32_t one_repeat_size)
{
    using data_type = typename Src0Tensor::data_type;
    uint32_t remain = total;
    for (uint16_t i = 0; i < repeat_times; ++i) {
        const uint32_t offset = i * one_repeat_size;
        const auto coord = asc::te::make_coord(offset);
        auto mask = asc::te::experimental::update_mask<data_type>(remain);
        auto src0_reg = asc::te::experimental::load(src0, coord).with_mask(mask);
        auto src1_reg = asc::te::experimental::load(src1, coord).with_mask(mask);
        auto src2_reg = asc::te::experimental::load(src2, coord).with_mask(mask);
        auto dst_reg = asc::te::experimental::fma(src0_reg, src1_reg, src2_reg);
        asc::te::experimental::store(dst, coord, dst_reg);
    }
}
```
