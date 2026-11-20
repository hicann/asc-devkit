# reduce_sum

<!-- md-trans-meta sourceCommit=9af36c4d4bf8a2627889aee5abbf2c6e7950326e translatedAt=2026-10-27T03:57:14.003Z pushedAt=2026-10-27T13:03:35.447Z nl=2 -->

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

This API performs a reduction summation operation on the source operand based on the mask carried by the input [reg_tensor](../reg_tensor/reg_tensor.md), and returns the computation result. The reduction scope is selected through the template parameter `scope`, which supports summation over all valid elements, summation over the valid elements within each `DataBlock` (32B), and summation over two adjacent valid elements.

When `scope` is `reduce_scope::element`, the calculation formula is as follows:

$$
dst_0 = \sum_{mask_i = 1} src_i
$$

When `scope` is `reduce_scope::datablock`, the calculation formula is as follows:

$$
dst_k = \sum_{kB \le i < (k + 1)B,\ mask_i = 1} src_i
$$

Here, $k$ represents the index of the `DataBlock`, and $B$ is the number of elements within one `DataBlock`.

When `scope` is `reduce_scope::pair`, the calculation formula is as follows:

$$
dst_i = (mask_{2i} ? src_{2i} : 0) + (mask_{2i + 1} ? src_{2i + 1} : 0)
$$

## Function Prototype

```cpp
template <reduce_scope scope = reduce_scope::element, typename DstType, typename SrcType>
__simd_callee__ inline reg_tensor<DstType> reduce_sum(const reg_tensor<SrcType>& src)
```

## Parameters

**Table 1** Template parameter description

| Parameter | Description |
| --- | --- |
| scope | Reduction scope, whose type is `reduce_scope`, with a default value of `reduce_scope::element`. When the value is `reduce_scope::element`, all valid elements are summed; when the value is `reduce_scope::datablock`, the valid elements within each `DataBlock` are summed; when the value is `reduce_scope::pair`, each pair of adjacent elements is summed. |
| DstType | Element type of the data register in the return value, which must be explicitly specified after `scope`. For supported data types, see [Data Type](#data-type). |
| SrcType | Data type of the source operand, which is automatically deduced from the function argument. For supported data types, see [Data Type](#data-type). |

**Table 2**  Parameters

| Parameter | Input/Output | Description |
| --- | --- | --- |
| src | Input | Source operand, whose type is [reg_tensor](../reg_tensor/reg_tensor.md)&lt;SrcType&gt;. `src.reg` stores the vector data, and `src.mask` controls whether each element participates in the reduction. When the bit in `src.mask` corresponding to an element is 1, the element participates in the reduction; when it is 0, the element does not participate in the reduction. |

## Data Type

When `scope` is `reduce_scope::element` or `reduce_scope::datablock`, `SrcType` and `DstType` support the following combinations:

| SrcType | DstType |
| --- | --- |
| `int16_t` | `int32_t` |
| `uint16_t` | `uint32_t` |
| `half` | `half` |
| `int32_t` | `int32_t` |
| `uint32_t` | `uint32_t` |
| `float` | `float` |

When `scope` is `reduce_scope::pair`, `SrcType` and `DstType` must be consistent, and the supported data types are `half` and `float`.

## Return Value

Returns the reduction summation result, whose type is `reg_tensor<DstType>`. The `mask` of the return value is consistent with `src.mask`.

- When `scope` is `reduce_scope::element`, the summation result is stored in the 0th element of `reg`, and the other elements are set to 0.
- When `scope` is `reduce_scope::datablock`, the summation result within each `DataBlock` is stored consecutively in the first 8 elements of `reg`, and the other elements are set to 0.
- When `scope` is `reduce_scope::pair`, the summation result of each pair of adjacent elements is stored consecutively in the lower half of `reg`, and the upper half is set to 0.

## Constraints

- `src.mask` must be set in advance through the `with_mask` API. If it is not set, the content of the mask is undefined, which may cause incorrect positions of the elements participating in the reduction.
- When `scope` is `reduce_scope::element`, if all bits of `src.mask` are 0, 0 is written to the 0th element of the return value `reg`; for floating-point types, +0 is written.
- When `scope` is `reduce_scope::datablock`, if all elements in a `DataBlock` do not participate in the computation, 0 is written to the corresponding position of the return value `reg`; for floating-point types, +0 is written.
- When `scope` is `reduce_scope::pair`, elements not selected by `src.mask` are treated as 0 and participate in the summation with adjacent elements.
- The `mask` of the return value is consistent with `src.mask`. If only the valid reduction results need to be moved out, call `with_mask` again to bind a new mask according to the result layout before calling `store`.
- `reduce_scope::element` and `reduce_scope::datablock` perform reduction layer by layer using a binary tree accumulation method, and the floating-point summation result is affected by the accumulation order. `reduce_scope::pair` performs summation only once on two adjacent elements and does not continue to reduce the lower-half results.

## Example

```cpp
#include "tensor_api/experimental/vector_ops.h"

template <typename InputTensor, typename OutputTensor>
__simd_vf__ inline void reduce_sum_example(InputTensor input, OutputTensor output)
{
    auto mask = asc::te::experimental::all_mask<float>();
    auto src = asc::te::experimental::load(input, asc::te::make_coord(0)).with_mask(mask);

    auto elemSum = asc::te::experimental::reduce_sum<asc::te::experimental::reduce_scope::element, float>(src);
    elemSum.with_mask(
        asc::te::experimental::make_mask<asc::te::experimental::mask_pattern::vl1, float>());
    asc::te::experimental::store(output, asc::te::make_coord(0), elemSum);

    auto blockSum = asc::te::experimental::reduce_sum<asc::te::experimental::reduce_scope::datablock, float>(src);
    blockSum.with_mask(
        asc::te::experimental::make_mask<asc::te::experimental::mask_pattern::vl8, float>());
    asc::te::experimental::store(output, asc::te::make_coord(8), blockSum);

    auto pairSum = asc::te::experimental::reduce_sum<asc::te::experimental::reduce_scope::pair, float>(src);
    pairSum.with_mask(
        asc::te::experimental::make_mask<asc::te::experimental::mask_pattern::vl32, float>());
    asc::te::experimental::store(output, asc::te::make_coord(16), pairSum);
}
```
