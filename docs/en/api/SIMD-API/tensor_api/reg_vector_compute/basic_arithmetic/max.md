# max

<!-- md-trans-meta sourceCommit=7e60dec365f6254fedd6bbb74d7032e62730ccec translatedAt=2026-10-27T03:23:53.469Z pushedAt=2026-10-27T13:03:35.376Z nl=2 -->

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

This API computes the element-wise maximum of two source operands based on the mask carried by the input reg_tensor, and returns the computation result. It supports three operand combinations: reg_tensor with reg_tensor, reg_tensor with scalar, and scalar with reg_tensor. The calculation formula is as follows:

$$
dst_i = \max(src0_i, src1_i)
$$

## Function Prototype

- reg_tensor with reg_tensor

    ```cpp
    template <typename T>
    __simd_callee__ inline reg_tensor<T> max(
        const reg_tensor<T>& src0, const reg_tensor<T>& src1)
    ```

- reg_tensor with scalar

    ```cpp
    template <typename T>
    __simd_callee__ inline reg_tensor<T> max(
        const reg_tensor<T>& src, const T& scalar)

    template <typename T>
    __simd_callee__ inline reg_tensor<T> max(
        const T& scalar, const reg_tensor<T>& src)
    ```

## Parameters

**Table 1** Template parameter description

| Parameter | Description |
| --- | --- |
| T | Operand data type. For supported data types, see [Data Type](#data-type). |

**Table 2** Parameters

| Parameter | Input/Output | Description |
| --- | --- | --- |
| src0 | Input | Source operand in the reg_tensor with reg_tensor form, of type reg_tensor&lt;T&gt;. src0.reg stores the vector data, and src0.mask controls whether each element participates in the computation. When the bit in src0.mask corresponding to an element is 1, the element participates in the computation; when it is 0, the element does not participate. |
| src1 | Input | Source operand in the reg_tensor with reg_tensor form, of type reg_tensor&lt;T&gt;. src1.mask does not participate in this computation. |
| src | Input | Source operand in the reg_tensor with scalar form, of type reg_tensor&lt;T&gt;. src.reg stores the vector data, and src.mask controls whether each element participates in the computation. When the bit in src.mask corresponding to an element is 1, the element participates in the computation; when it is 0, the element does not participate. |
| scalar | Input | Source operand, of scalar type T. |

## Data Type

The source operands and the return value have the same data type. Supported data types are: int8_t, uint8_t, int16_t, uint16_t, half, bfloat16_t, int32_t, uint32_t, and float.

## Return Value

Returns the result of element-wise maximum computation, with the type reg_tensor&lt;T&gt;. For the reg_tensor with reg_tensor form, the mask of the return value is the same as src0.mask; for forms involving a scalar, it is the same as src.mask. When the corresponding position of the input mask is 0, the corresponding element of the return value is set to zero.

## Constraints

- The mask used to control the computation must be set in advance through the `with_mask` API. If it is not set, the content of the mask is undefined, which may cause incorrect element positions to participate in the computation.
- In the reg_tensor with reg_tensor form, when the corresponding element of src0 is -0 and the corresponding element of src1 is +0, +0 is returned.
- When reg_tensor is on the left, if the corresponding element of src is -0 and scalar is +0, +0 is returned.
- When the scalar is on the left, if scalar is +0 and the corresponding element of src is -0, +0 is returned.

## Example

```cpp
#include "tensor_api/experimental/vector_ops.h"

template <typename InputTensor, typename OutputTensor>
__simd_vf__ inline void max_example(InputTensor input0, InputTensor input1, OutputTensor output)
{
    auto mask = asc::te::experimental::all_mask<float>();
    auto src0 = asc::te::experimental::load(input0, asc::te::make_coord(0)).with_mask(mask);
    auto src1 = asc::te::experimental::load(input1, asc::te::make_coord(0)).with_mask(mask);
    auto dst = asc::te::experimental::max(src0, src1);
    asc::te::experimental::store(output, asc::te::make_coord(0), dst);
}
```
