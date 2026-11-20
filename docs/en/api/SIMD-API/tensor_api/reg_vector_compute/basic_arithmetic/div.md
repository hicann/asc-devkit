# operator/

<!-- md-trans-meta sourceCommit=22e1e8c65e425d4c0fb07294311f2eac049ab0b3 translatedAt=2026-10-27T03:19:45.933Z pushedAt=2026-10-27T13:03:35.368Z nl=2 -->

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

This API divides the two source operands element-wise according to the mask carried by **src0** and returns the computation result. The calculation formula is as follows:

$$
dst_i = \frac{src0_i}{src1_i}
$$

## Function Prototype

```cpp
template <typename T>
__simd_callee__ inline reg_tensor<T> operator/(
    const reg_tensor<T>& src0, const reg_tensor<T>& src1)
```

## Parameters

**Table 1** Template parameter description

| Parameter | Description |
| --- | --- |
| T | Operand data type. For supported data types, see [Data Type](#data-type). |

**Table 2**  Parameters

| Parameter | Input/Output | Description |
| --- | --- | --- |
| src0 | Input | Dividend, whose type is reg_tensor&lt;T&gt;. **src0.reg** stores the vector data, and **src0.mask** controls whether each element participates in the computation. |
| src1 | Input | Divisor, whose type is reg_tensor&lt;T&gt;. **src1.mask** does not participate in this computation. |

## Data Type

The source operands and the return value have the same data type. The supported data types are: int16_t, uint16_t, half, int32_t, uint32_t, and float.

## Return Value

Returns the element-wise division result, whose type is reg_tensor&lt;T&gt;. The return value mask is the same as **src0.mask**; when the corresponding position of **src0.mask** is 0, the corresponding element of the return value is set to zero.

## Constraints

- src0.mask must be set in advance through the `with_mask` API. If it is not set, the content of mask is undefined, which may cause incorrect element positions to participate in the computation.
- When integer types are used, the divisor cannot be 0.
- Pay attention to division-by-zero errors. For the results, see [Div special value/boundary value input computation result description](../../../../appendix/reg_vector_compute_interface_boundary_value_summary.md#table1325925915217).
- The computation results of the `half` and `float` data types may differ from standard `IEEE 754` binary floating-point division by up to 1 ULP.

## Example

```cpp
#include "tensor_api/tensor.h"
#include "tensor_api/experimental/arch/vector/basic_arithmetic.h"
#include "tensor_api/experimental/arch/vector/mask_reg_compute.h"
#include "tensor_api/experimental/arch/vector/reg_data_load.h"
#include "tensor_api/experimental/arch/vector/reg_data_store.h"

template <typename InputTensor, typename OutputTensor>
__simd_vf__ inline void div_example(InputTensor input0, InputTensor input1, OutputTensor output)
{
    using asc::te::experimental::operator/;
    auto mask = asc::te::experimental::all_mask<float>();
    auto src0 = asc::te::experimental::load(input0, asc::te::make_coord(0)).with_mask(mask);
    auto src1 = asc::te::experimental::load(input1, asc::te::make_coord(0));
    auto dst = src0 / src1;
    asc::te::experimental::store(output, asc::te::make_coord(0), dst);
}
```
