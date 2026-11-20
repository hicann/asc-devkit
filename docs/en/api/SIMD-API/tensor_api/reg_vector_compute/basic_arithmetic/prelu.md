# prelu

<!-- md-trans-meta sourceCommit=2fbb723e5f18978a82b4419a9437c316114602c3 translatedAt=2026-11-19T02:44:39.354Z pushedAt=2026-11-19T05:54:06.655Z nl=2 -->

## Supported Products

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
- Atlas inference products AI Core: Not supported
<!-- end id5 -->
<!-- npu="310p" id6 -->
- Atlas inference products Vector Core: Not supported
<!-- end id6 -->
<!-- npu="910" id7 -->
- Atlas training products: Not supported
<!-- end id7 -->

## Function Description

Header file path: `"tensor_api/experimental/arch/vector/basic_arithmetic.h"`.

This API performs PReLU calculation element-wise on the source operand according to the mask carried by `src`, and returns the calculation result. Internally, this API calls [asc_prelu](../../../c_api/reg_compute/reg_fused/asc_prelu.md) to perform the underlying calculation. When `src` is greater than or equal to 0, `src` is returned directly; otherwise, `src` is multiplied by `slope` and then returned. Note: When the source operand is positive zero (+0), it is treated as positive and returned directly; when the source operand is negative zero (-0), it is treated as negative and combined with `slope`. Elements whose corresponding position in `src.mask` is 1 participate in the calculation, and elements whose corresponding position is 0 are set to zero in the returned result. The calculation formula is as follows:

$$
dst_i = \begin{cases}
src_i, & src_i >= 0 \\
src_i \times slope_i, & src_i < 0
\end{cases}
$$

## Prototype

```cpp
template <typename T>
__simd_callee__ inline reg_tensor<T> prelu(
    const reg_tensor<T>& src, const reg_tensor<T>& slope)
```

## Parameters

**Table 1** Template parameter description

| Parameter | Description |
| --- | --- |
| T | Operand data type. For supported data types, see [Data Type](#data-type). |

**Table 2**  Parameters

| Parameter | Input/Output | Description |
| --- | --- | --- |
| src | Input | Source operand, of type `reg_tensor<T>`. `src.reg` stores the vector data, and `src.mask` controls whether each element participates in the computation. When the bit in `src.mask` corresponding to an element is 1, the element participates in the computation; when it is 0, the element does not participate. |
| slope | Input | Slope, whose type is reg_tensor&lt;T&gt;. slope.reg stores the slope used by each element, and slope.mask does not participate in this computation. |

## Data Type

The data types of src, slope, and the return value must be consistent. The supported data types are: half and float.

## Return Value

Returns the PReLU computation result, whose type is reg_tensor&lt;T&gt;. The mask of the return value is the same as src.mask; when the corresponding position in src.mask is 0, the corresponding element of the return value is set to zero.

## Constraints

- `src.mask` must be set in advance through the `with_mask` API. If it is not set, the content of the mask is undefined, which may cause incorrect element positions to participate in the computation.
- When the source operand is positive zero (`+0`), it is treated as positive and returned directly; when the source operand is negative zero (`-0`), it is treated as negative and combined with `slope`, with the multiplication following the IEEE 754 floating-point multiplication rules.

## Example

```cpp
#include "tensor_api/tensor.h"
#include "tensor_api/experimental/arch/vector/basic_arithmetic.h"
#include "tensor_api/experimental/arch/vector/mask_reg_compute.h"
#include "tensor_api/experimental/arch/vector/reg_data_load.h"
#include "tensor_api/experimental/arch/vector/reg_data_store.h"

template <typename InputTensor, typename OutputTensor>
__simd_vf__ inline void prelu_example(InputTensor input, InputTensor slope_input, OutputTensor output)
{
    auto mask = asc::te::experimental::all_mask<float>();
    auto src = asc::te::experimental::load(input, asc::te::make_coord(0)).with_mask(mask);
    auto slope = asc::te::experimental::load(slope_input, asc::te::make_coord(0));
    auto dst = asc::te::experimental::prelu(src, slope);
    asc::te::experimental::store(output, asc::te::make_coord(0), dst);
}
```
