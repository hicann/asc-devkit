# reduce_min

<!-- md-trans-meta sourceCommit=9af36c4d4bf8a2627889aee5abbf2c6e7950326e translatedAt=2026-10-27T03:56:35.057Z pushedAt=2026-10-27T13:03:35.443Z nl=2 -->

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

Header file path: `"tensor_api/experimental/arch/vector/reduction_compute.h"`.

This API performs a reduction minimum operation on the source operand based on the mask carried by the input [reg_tensor](../reg_tensor/reg_tensor.md), and returns the computation result. The reduction scope is selected through the template parameter `scope`, which supports computing the minimum over all valid elements and computing the minimum over the valid elements in each `DataBlock` (32B).

When `scope` is `reduce_scope::element`, the calculation formula is as follows:

$$
\begin{aligned}
dst_0 &= \min\{src_i \mid mask_i = 1\} \\
dst_1 &= \operatorname{argmin}\{src_i \mid mask_i = 1\}
\end{aligned}
$$

When `scope` is `reduce_scope::datablock`, the calculation formula is as follows:

$$
dst_k = \min\{src_i \mid kB \le i < (k + 1)B,\ mask_i = 1\}
$$

Here, $k$ represents the index of the `DataBlock`, and $B$ is the number of elements within one `DataBlock`.

## Function Prototype

```cpp
template <reduce_scope scope = reduce_scope::element, typename DataType>
__simd_callee__ inline reg_tensor<DataType> reduce_min(const reg_tensor<DataType>& src)
```

## Parameters

**Table 1** Template parameter description

| Parameter | Description |
| --- | --- |
| scope | Reduction scope, type is `reduce_scope`, defaulting to `reduce_scope::element`. When set to `reduce_scope::element`, computes the minimum over all valid elements; when set to `reduce_scope::datablock`, computes the minimum over the valid elements in each `DataBlock`. |
| DataType | Element type of the data register in the source operand and return value, automatically deduced from the function arguments. For supported data types, see [data type](#data-type). |

**Table 2**  Parameters

| Parameter | Input/Output | Description |
| --- | --- | --- |
| src | Input | Source operand, type is [reg_tensor](../reg_tensor/reg_tensor.md)&lt;DataType&gt;. `src.reg` stores the vector data, and `src.mask` controls whether each element participates in the reduction. When the bit in `src.mask` corresponding to an element is 1, the element participates in the reduction; when it is 0, the element does not participate. |

## Data Type

The data types supported by `DataType` are: `int16_t`, `uint16_t`, `half`, `int32_t`, `uint32_t`, and `float`.

## Return Value

Returns the reduction minimum result, whose type is `reg_tensor<DataType>`. The `mask` of the return value is consistent with `src.mask`.

- When `scope` is `reduce_scope::element`, the minimum value is stored in element 0 of `reg`, the raw bit pattern of the index of the minimum value in `src` is stored in element 1 of `reg`, and the other elements are set to 0. If multiple minimum values exist, the smallest index is retained.
- When `scope` is `reduce_scope::datablock`, the minimum value within each `DataBlock` is stored consecutively in the first 8 elements of `reg`, and the other elements are set to 0. In this mode, only the minimum value is output, and no index is output.

## Constraints

- `src.mask` must be set in advance through the `with_mask` API. If it is not set, the content of the mask is undefined, which may cause incorrect positions of the elements participating in the reduction.
- Elements not selected by `src.mask` are treated as the maximum value of the corresponding data type. For floating-point types, the maximum value is `+inf`.
- When `scope` is `reduce_scope::element`, if all bits of `src.mask` are 0, the maximum value of the corresponding data type is written to element 0 of the return value `reg`, and the remaining elements are set to 0.
- When `scope` is `reduce_scope::datablock`, if none of the elements within a `DataBlock` participate in the computation, the maximum value of the corresponding data type is written to the corresponding position of the return value `reg`.
- During comparison, $min(-0, +0) = -0$ is followed.
- When `scope` is `reduce_scope::element`, if nan exists in the input data, nan is written to element 0 of the return value `reg`, and the index of the first nan is written to element 1 of the return value `reg`.
- The `mask` of the return value is consistent with `src.mask`. If only the valid reduction results need to be moved out, call `with_mask` again to bind a new mask according to the result layout before calling `store`.
- When `scope` is `reduce_scope::element`, the API returns both the minimum value and the index. The index is stored in the first element of the return value `reg` according to the bit pattern of `DataType`. When reading the index, it must be converted to an integer type according to the bit width corresponding to `DataType`. For example, when `DataType` is `half`, the raw bit pattern of the index can be read as `uint16_t`; when `DataType` is `float`, the raw bit pattern of the index can be read as `uint32_t`.

## Example

```cpp
#include "tensor_api/experimental/vector_ops.h"

template <typename InputTensor, typename OutputTensor>
__simd_vf__ inline void reduce_min_example(InputTensor input, OutputTensor output)
{
    auto mask = asc::te::experimental::all_mask<float>();
    auto src = asc::te::experimental::load(input, asc::te::make_coord(0)).with_mask(mask);

    auto elemMin = asc::te::experimental::reduce_min(src);
    elemMin.with_mask(
        asc::te::experimental::make_mask<asc::te::experimental::mask_pattern::vl2, float>());
    asc::te::experimental::store(output, asc::te::make_coord(0), elemMin);

    auto blockMin = asc::te::experimental::reduce_min<asc::te::experimental::reduce_scope::datablock>(src);
    blockMin.with_mask(
        asc::te::experimental::make_mask<asc::te::experimental::mask_pattern::vl8, float>());
    asc::te::experimental::store(output, asc::te::make_coord(8), blockMin);
}
```
