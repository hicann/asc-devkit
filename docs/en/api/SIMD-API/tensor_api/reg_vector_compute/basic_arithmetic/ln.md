# ln

<!-- md-trans-meta sourceCommit=7e60dec365f6254fedd6bbb74d7032e62730ccec translatedAt=2026-10-27T03:21:21.650Z pushedAt=2026-10-27T13:03:35.371Z nl=2 -->

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

Header file path: `"tensor_api/experimental/arch/vector/basic_arithmetic.h"`.

This API computes the natural logarithm (base e) of each valid element in the source operand **src** according to **src.mask**, and returns the computation result. The calculation formula is as follows:

$$
dst_i = \ln(src_i)
$$

## Function Prototype

```cpp
template <typename T>
__simd_callee__ inline reg_tensor<T> ln(const reg_tensor<T>& src)
```

## Parameters

**Table 1** Template parameter description

| Parameter | Description |
| --- | --- |
| T | Operand data type. For supported data types, see [Data Type](#data-type). |

**Table 2** Parameters

| Parameter | Input/Output | Description |
| --- | --- | --- |
| src | Input | Source operand, of type `reg_tensor<T>`. `src.reg` stores the vector data, and `src.mask` controls whether each element participates in the computation. When the bit in `src.mask` corresponding to an element is 1, the element participates in the computation; when it is 0, the element does not participate. |

## Return Value

Returns the natural logarithm computation result of type reg_tensor&lt;T&gt;. The mask of the return value is the same as **src.mask**; when the corresponding position in **src.mask** is 0, the corresponding element of the return value is set to zero.

## Data Type

**Table 3** Data type combinations

| src | dst |
| --- | --- |
| half | half |
| float | float |

## Constraints

- `src.mask` must be set in advance through the `with_mask` API. If it is not set, the content of the mask is undefined, which may cause incorrect element positions to participate in the computation.

## Example

```cpp
#include "tensor_api/experimental/vector_ops.h"

template <typename InputTensor, typename OutputTensor>
__simd_vf__ inline void ln_vf(
    const InputTensor input, OutputTensor output, uint16_t repeat_times, uint32_t total,
    uint32_t one_repeat_size)
{
    using data_type = typename InputTensor::data_type;
    uint32_t remain = total;
    for (uint16_t i = 0; i < repeat_times; ++i) {
        const uint32_t offset = i * one_repeat_size;
        const auto coord = asc::te::make_coord(offset);
        auto mask = asc::te::experimental::update_mask<data_type>(remain);
        auto src_reg = asc::te::experimental::load(input, coord).with_mask(mask);
        auto dst_reg = asc::te::experimental::ln(src_reg);
        asc::te::experimental::store(output, coord, dst_reg);
    }
}
```
