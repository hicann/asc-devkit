# or

<!-- md-trans-meta sourceCommit=7e60dec365f6254fedd6bbb74d7032e62730ccec translatedAt=2026-10-27T03:49:21.552Z pushedAt=2026-10-27T13:03:35.428Z nl=2 -->

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

Header file path: `"tensor_api/experimental/arch/vector/logical_compute.h"`.

This API performs a bitwise OR (|) operation on source operands src0 and src1 according to src0.mask, and returns the computation result. When T is not bool, the operation is computed element-wise on two vector data registers; when T is bool, it is computed on the valid bits of two mask registers. The calculation formula is as follows:

$$
dst_i = src0_i \mathbin{|} src1_i
$$

## Function Prototype

```cpp
template <typename T>
__simd_callee__ inline reg_tensor<T> operator|(
    const reg_tensor<T>& src0, const reg_tensor<T>& src1)
```

## Parameters

**Table 1** Template parameter description

| Parameter | Description |
| --- | --- |
| T | Operand data type. For supported data types, see [Data Type](#data-type). |

**Table 2** Parameters

| Parameter | Input/Output | Description |
| --- | --- | --- |
| src0 | Input | Source operand, whose type is reg_tensor&lt;T&gt;. When T is not bool, src0.reg stores vector data; when T is bool, src0.reg stores mask register data. src0.mask controls whether each element or bit participates in the computation: a position participates when its value is 1 and does not participate when its value is 0. |
| src1 | Input | Source operand, whose type is reg_tensor&lt;T&gt;. When T is not bool, src1.reg stores vector data; when T is bool, src1.reg stores mask register data. src1.mask does not participate in this computation. |

## Data Type

The data types of the source operands and the return value are consistent. Supported data types are: bool, int8_t, uint8_t, int16_t, uint16_t, int32_t, uint32_t.

## Return Value

Returns the element-wise bitwise OR result, whose type is reg_tensor&lt;T&gt;. The mask of the return value is the same as src0.mask; when the corresponding position of src0.mask is 0, the corresponding element of the return value is set to zero.

## Constraints

- `src0.mask` must be set in advance through the `with_mask` API. If it is not set, the content of the mask is undefined, which may cause incorrect positions of the elements participating in the computation.
- The amount of data participating in the computation is determined by the vector length VL. When T is not bool, the number of elements is VL divided by sizeof(T); when T is bool, the number of bits participating in the computation is VL.
- Positions not selected by `src0.mask` do not participate in the computation, and the corresponding positions in the return value are set to zero.

## Example

```cpp
#include "tensor_api/experimental/vector_ops.h"

template <typename InputTensor, typename OutputTensor>
__simd_vf__ inline void or_example(InputTensor input0, InputTensor input1, OutputTensor output)
{
    using asc::te::experimental::operator|;
    auto mask = asc::te::experimental::all_mask<int32_t>();
    auto src0 = asc::te::experimental::load(input0, asc::te::make_coord(0)).with_mask(mask);
    auto src1 = asc::te::experimental::load(input1, asc::te::make_coord(0)).with_mask(mask);
    auto dst = src0 | src1;
    asc::te::experimental::store(output, asc::te::make_coord(0), dst);
}
```
