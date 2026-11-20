# operator+

<!-- md-trans-meta sourceCommit=7e60dec365f6254fedd6bbb74d7032e62730ccec translatedAt=2026-10-27T03:26:53.586Z pushedAt=2026-10-27T13:03:35.381Z nl=2 -->

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

This API performs element-wise addition on two source operands based on the mask carried by the input reg_tensor, and returns the computation result. It supports three operand combinations: reg_tensor with reg_tensor, reg_tensor with scalar, and scalar with reg_tensor. The calculation formula is as follows:

$$
dst_i = src0_i + src1_i
$$

## Function Prototype

- reg_tensor with reg_tensor

    ```cpp
    template <typename T>
    __simd_callee__ inline reg_tensor<T> operator+(
        const reg_tensor<T>& src0, const reg_tensor<T>& src1)
    ```

- reg_tensor with scalar

    ```cpp
    template <typename T>
    __simd_callee__ inline reg_tensor<T> operator+(
        const reg_tensor<T>& src, const T& scalar)

    template <typename T>
    __simd_callee__ inline reg_tensor<T> operator+(
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

Returns the element-wise sum result, whose type is reg_tensor&lt;T&gt;. For the reg_tensor with reg_tensor form, the mask of the return value is the same as src0.mask; for the form containing a scalar, it is the same as src.mask. When the corresponding position of the input mask is 0, the corresponding element of the return value is set to zero.

## Constraints

### General Constraints

- The mask used to control the computation must be set in advance through the `with_mask` API. If it is not set, the content of the mask is undefined, which may cause incorrect element positions to participate in the computation.

### reg_tensor with reg_tensor Form

- For integer data, overflow during addition is handled according to the wraparound policy. For a signed type, adding 1 to the maximum value yields the minimum value of that type, and subtracting 1 from the minimum value yields the maximum value of that type; for an unsigned type, adding 1 to the maximum value yields 0.
- The half, bfloat16_t, and float types are computed according to the IEEE 754 floating-point addition rules. Overflow and the handling of special values such as nan and inf follow the floating-point operation rules.

## Example

```cpp
#include "tensor_api/experimental/vector_ops.h"

template <typename InputTensor, typename OutputTensor>
__simd_vf__ inline void add_example(InputTensor input, OutputTensor output)
{
    using asc::te::experimental::operator+;
    auto src = asc::te::experimental::load(input, asc::te::make_coord(0))
                   .with_mask(asc::te::experimental::all_mask<float>());
    auto dst = src + 1.0f;
    asc::te::experimental::store(output, asc::te::make_coord(0), dst);
}
```
