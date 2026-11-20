# leaky_relu

<!-- md-trans-meta sourceCommit=9b28f4f9573c582598f2671adecd4bd712821920 translatedAt=2026-10-27T03:20:41.090Z pushedAt=2026-10-27T13:03:35.370Z nl=2 -->

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

This API performs element-wise Leaky ReLU computation on the source operand using a scalar slope according to the mask carried by **src**, and returns the computation result. The calculation formula is as follows:

$$
dst_i = \begin{cases}
src_i, & src_i > 0 \\
src_i \times slope, & src_i \le 0
\end{cases}
$$

## Function Prototype

```cpp
template <typename T>
__simd_callee__ inline reg_tensor<T> leaky_relu(
    const reg_tensor<T>& src, const T& slope)
```

## Parameters

**Table 1** Template parameter description

| Parameter | Description |
| --- | --- |
| T | Element type of **reg_tensor**. For supported data types, see [Data Type](#data-type). |

**Table 2**  Parameters

| Parameter | Input/Output | Description |
| --- | --- | --- |
| src | Input | Source operand, of type `reg_tensor<T>`. `src.reg` stores the vector data, and `src.mask` controls whether each element participates in the computation. When the bit in `src.mask` corresponding to an element is 1, the element participates in the computation; when it is 0, the element does not participate. |
| slope | Input | Scalar slope used when the corresponding element in **src** is less than or equal to 0, with the same data type as T. |

## Data Type

The data types of **src** and the return value are consistent. The supported data types are: half, float.

## Return Value

Returns the Leaky ReLU computation result, of type **reg_tensor**&lt;T&gt;. The mask of the return value is the same as **src.mask**; when the corresponding position in **src.mask** is 0, the corresponding element of the return value is set to zero.

## Constraints

- `src.mask` must be set in advance through the `with_mask` API. If it is not set, the content of the mask is undefined, which may cause incorrect element positions to participate in the computation.
- When the source operand is negative zero (`-0`), it is treated as a negative number, and the multiplication in the negative branch follows the IEEE 754 floating-point multiplication rules.

## Example

```cpp
#include "tensor_api/tensor.h"
#include "tensor_api/experimental/arch/vector/basic_arithmetic.h"
#include "tensor_api/experimental/arch/vector/mask_reg_compute.h"
#include "tensor_api/experimental/arch/vector/reg_data_load.h"
#include "tensor_api/experimental/arch/vector/reg_data_store.h"

template <typename InputTensor, typename OutputTensor>
__simd_vf__ inline void leaky_relu_example(InputTensor input, OutputTensor output)
{
    auto mask = asc::te::experimental::all_mask<float>();
    auto src = asc::te::experimental::load(input, asc::te::make_coord(0)).with_mask(mask);
    auto dst = asc::te::experimental::leaky_relu(src, 0.1f);
    asc::te::experimental::store(output, asc::te::make_coord(0), dst);
}
```
