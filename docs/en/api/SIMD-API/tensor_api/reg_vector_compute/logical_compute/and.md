# and

<!-- md-trans-meta sourceCommit=9b28f4f9573c582598f2671adecd4bd712821920 translatedAt=2026-10-27T03:47:27.972Z pushedAt=2026-10-27T13:03:35.420Z nl=2 -->

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

Performs a bitwise AND operation on two registers element-wise.

$$
dst_i = a_i \,\mathbin{\&}\, b_i
$$

## Function Prototype

```cpp
template <typename T>
__simd_callee__ inline reg_tensor<T> operator&(
    const reg_tensor<T>& src0, const reg_tensor<T>& src1)
```

## Parameters

**Table 1** Template parameter description

| Parameter | Description |
| --- | --- |
| T | Element type of the operands and the return value. For supported data types, see [data type](#data-type). |

**Table 2**  Parameters

| Parameter | Input/Output | Description |
| --- | --- | --- |
| `src0` | Input | First source operand, of type `reg_tensor<T>`. `src0.mask` marks the elements that participate in the computation. A position participates when its value is `1` and does not participate when its value is `0`. |
| `src1` | Input | Second source operand, of type `reg_tensor<T>`. `src1.mask` is not used in this computation. |

## Data Type

The data types of src0, src1, and the return value must be consistent. Supported data types are: `bool`, `int8_t`, `uint8_t`, `int16_t`, `uint16_t`, `int32_t`, `uint32_t`.

## Return Value

Returns `reg_tensor<T>`. `reg` stores the element-wise bitwise AND result. When the corresponding position of `src0.mask` is `0`, the corresponding position of `reg` is set to zero. The `mask` of the return value is the same as `src0.mask`.

## Constraints

- src0.mask must be set in advance through the `with_mask` API. If it is not set, the content of mask is undefined, which may cause incorrect element positions to participate in the computation.
- Constructing `reg_tensor<bool>{maskValue}` only initializes the mask of the object and does not generate boolean register data. Before being used as an operand, reg must have been written by mask generation, loading, or another boolean computation API.
- The number of elements participating in the computation is determined by the vector length (VL): when `T` is not `bool`, it is `VL / sizeof(T)`; when `T` is `bool`, it is `VL` bits.
- Positions not selected by `src0.mask` do not participate in the computation, and the corresponding positions in the return value are set to zero.

## Example

The following example shows a typical way to call the API in an AIV compute function. The example first binds a mask to the load result, then performs the computation and writes the result back to UB.

```cpp
#include "tensor_api/tensor.h"
#include "tensor_api/experimental/arch/vector/logical_compute.h"
#include "tensor_api/experimental/arch/vector/mask_reg_compute.h"
#include "tensor_api/experimental/arch/vector/reg_data_load.h"
#include "tensor_api/experimental/arch/vector/reg_data_store.h"

template <typename InputTensor, typename OutputTensor>
__simd_vf__ inline void bit_and_example(
    InputTensor input0, InputTensor input1, OutputTensor output)
{
    using asc::te::experimental::operator&;

    auto mask = asc::te::experimental::all_mask<int32_t>();
    auto src0_reg = asc::te::experimental::load(input0, asc::te::make_coord(0)).with_mask(mask);
    auto src1_reg = asc::te::experimental::load(input1, asc::te::make_coord(0)).with_mask(mask);

    auto result = src0_reg & src1_reg;
    asc::te::experimental::store(output, asc::te::make_coord(0), result);
}
```
