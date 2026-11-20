# not

<!-- md-trans-meta sourceCommit=dacba287b3c94417af1e532d9cbd29c398889dea translatedAt=2026-10-27T03:47:58.087Z pushedAt=2026-10-27T13:03:35.425Z nl=2 -->

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

Namespace: `asc::te::experimental`.

Performs bitwise inversion on the elements of the input register that participate in the computation. `reg_tensor<bool>` inverts the boolean register data, while other types invert the binary encoding of each element.

$$
dst_i = \mathord{\sim}src_i
$$

## Function Prototype

```cpp
template <typename T>
__simd_callee__ inline reg_tensor<T> operator!(const reg_tensor<T>& src)
```

## Parameters

| Parameter | Input/Output | Description |
| --- | --- | --- |
| `T` | Template Parameters | Element type of the input and return value. Supported types: `bool`, `int8_t`, `uint8_t`, `int16_t`, `uint16_t`, `half`, `int32_t`, `uint32_t`, `float`. |
| `src` | Input | Source operand of type `reg_tensor<T>`. `src.mask` marks the elements that participate in the computation: a value of `1` at a position means the element participates, and `0` means it does not. |

## Return Value

Returns `reg_tensor<T>`. `reg` stores the element-wise bitwise inversion result. When the corresponding position in `src.mask` is `0`, the corresponding position in `reg` is set to zero. The `mask` of the return value is the same as `src.mask`.

## Constraints

- This API does not perform a numeric zero-value check. For non-`bool` types, the result is data of the same type obtained by bitwise-inverting the binary encoding of the input.
- `src.mask` must be set in advance through the `with_mask` API. If it is not set, the content of the mask is undefined, which may cause incorrect element positions to participate in the computation.
- The number of elements participating in the computation is determined by the vector length (VL): when `T` is not `bool`, it is `VL / sizeof(T)`; when `T` is `bool`, it is `VL` bits.
- Positions not selected by `src.mask` do not participate in the computation, and the corresponding positions in the return value are set to zero.

## Example

The following example shows a typical way to call the API in an AIV compute function.

```cpp
#include "tensor_api/tensor.h"
#include "tensor_api/experimental/arch/vector/logical_compute.h"
#include "tensor_api/experimental/arch/vector/mask_reg_compute.h"
#include "tensor_api/experimental/arch/vector/reg_data_load.h"
#include "tensor_api/experimental/arch/vector/reg_data_store.h"

template <typename InputTensor, typename OutputTensor>
__simd_vf__ inline void not_example(InputTensor input, OutputTensor output)
{
    using asc::te::experimental::operator!;

    auto mask = asc::te::experimental::all_mask<int32_t>();
    auto src = asc::te::experimental::load(input, asc::te::make_coord(0)).with_mask(mask);
    auto result = !src;
    asc::te::experimental::store(output, asc::te::make_coord(0), result);
}
```
