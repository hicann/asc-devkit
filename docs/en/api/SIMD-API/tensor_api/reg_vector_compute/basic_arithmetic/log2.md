# log2

<!-- md-trans-meta sourceCommit=31c8e77fd9dca69c7ade85b7c565a043da2414dd translatedAt=2026-11-19T02:44:29.139Z pushedAt=2026-11-19T05:54:06.654Z nl=2 -->

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

This API computes the base-2 logarithm of the source operand element by element according to the mask carried by the input reg_tensor, and returns the computation result. The calculation formula is as follows:

$$
dst_i = \log_2(src_i)
$$

## Prototype

```cpp
template <typename T>
__simd_callee__ inline reg_tensor<T> log2(const reg_tensor<T>& src)
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

## Data Type

The source operand and the return value have the same data type. The supported data types are `half` and `float`.

## Return Value

Returns the result of computing the base-2 logarithm element by element, of type reg_tensor&lt;T&gt;. The mask of the return value is the same as `src.mask`; when the corresponding position in `src.mask` is 0, the corresponding element of the return value is set to zero.

## Constraints

src.mask must be set in advance through [with_mask](../reg_tensor/reg_tensor.md). If it is not set, the content of mask is undefined, which causes incorrect positions of the elements involved in the computation.

## Key Features

### Precision Promotion

When the source operand data type is half, the half value is promoted to float for computation, and the result is then converted back to half for output.

## Example

In the following example, both the input and output are half tensors in UB. Each call processes 128 elements and uses a full mask.

```cpp
#include "tensor_api/tensor.h"
#include "tensor_api/experimental/arch/vector/basic_arithmetic.h"
#include "tensor_api/experimental/arch/vector/mask_reg_compute.h"
#include "tensor_api/experimental/arch/vector/reg_data_load.h"
#include "tensor_api/experimental/arch/vector/reg_data_store.h"

template <typename InputTensor, typename OutputTensor>
__simd_vf__ inline void log2_example(InputTensor input, OutputTensor output)
{
    auto mask = asc::te::experimental::all_mask<half>();
    auto src = asc::te::experimental::load(input, asc::te::make_coord(0)).with_mask(mask);
    auto dst = asc::te::experimental::log2(src);
    asc::te::experimental::store(output, asc::te::make_coord(0), dst);
}
```
