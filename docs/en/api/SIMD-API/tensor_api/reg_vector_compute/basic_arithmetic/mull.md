# mull

<!-- md-trans-meta sourceCommit=22e1e8c65e425d4c0fb07294311f2eac049ab0b3 translatedAt=2026-10-27T03:25:10.170Z pushedAt=2026-10-27T13:03:35.379Z nl=2 -->

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

This API multiplies two 32-bit integer source operands element by element according to the mask carried by src0, and returns a result consisting of the low 32 bits and the high 32 bits. The calculation formula is as follows:

$$
\{high_i, low_i\} = src0_i \times src1_i
$$

## Function Prototype

```cpp
template <typename T>
__simd_callee__ inline reg_pair<T> mull(
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
| src0 | Input | Source operand, of type reg_tensor&lt;T&gt;. src0.reg stores the vector data, and src0.mask controls whether each element participates in the computation. |
| src1 | Input | Source operand, of type reg_tensor&lt;T&gt;. src1.mask does not participate in this computation. |

## Data Type

The data types of the source operands and the two registers in the return value must be consistent. Supported data types are: int32_t and uint32_t.

## Return Value

Returns `reg_pair<T>`. `first` stores the low 32 bits of the product, and `second` stores the high 32 bits of the product. The masks of both registers are the same as `src0.mask`.

## Constraints

src0.mask must be set in advance through the `with_mask` API. If it is not set, the content of mask is undefined, which may cause incorrect element positions to participate in the computation.

## Example

```cpp
#include "tensor_api/tensor.h"
#include "tensor_api/experimental/arch/vector/basic_arithmetic.h"
#include "tensor_api/experimental/arch/vector/mask_reg_compute.h"
#include "tensor_api/experimental/arch/vector/reg_data_load.h"
#include "tensor_api/experimental/arch/vector/reg_data_store.h"

template <typename InputTensor, typename OutputTensor>
__simd_vf__ inline void mull_example(InputTensor input0, InputTensor input1, OutputTensor output)
{
    auto mask = asc::te::experimental::all_mask<uint32_t>();
    auto src0 = asc::te::experimental::load(input0, asc::te::make_coord(0)).with_mask(mask);
    auto src1 = asc::te::experimental::load(input1, asc::te::make_coord(0));
    auto result = asc::te::experimental::mull(src0, src1);
    asc::te::experimental::store(output, asc::te::make_coord(0), result.first);
    asc::te::experimental::store(output, asc::te::make_coord(1), result.second);
}
```
