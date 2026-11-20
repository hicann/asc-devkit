# shift_left

<!-- md-trans-meta sourceCommit=2fbb723e5f18978a82b4419a9437c316114602c3 translatedAt=2026-11-19T02:51:33.514Z pushedAt=2026-11-19T05:54:06.665Z nl=2 -->

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

Header file path: `"tensor_api/experimental/arch/vector/logical_compute.h"`.

Shifts `src` left element-wise according to the shift amount given by a register or a scalar.

$$
dst_i = src_i \ll shift_i
$$

## Prototype

```cpp
// Register shift.
template <typename T, typename ShiftType>
__simd_callee__ inline reg_tensor<T> operator<<(
    const reg_tensor<T>& src, const reg_tensor<ShiftType>& shift)

// Scalar shift.
template <typename T>
__simd_callee__ inline reg_tensor<T> operator<<(
    const reg_tensor<T>& src, const int16_t& shift)
```

## Parameters

**Table 1** Template parameter description

| Parameter | Description |
| --- | --- |
| T | Element type of `src` and the return value. For supported data types, see [Data Type](#data-type). |
| ShiftType | Element type of the shift amount in register shift, which must be a signed integer type with matching bit width to T. |

**Table 2**  Parameters

| Parameter | Input/Output | Description |
| --- | --- | --- |
| `src` | Input | Source operand to be shifted left. `src.mask` marks the elements that participate in the computation. A value of `1` at a position indicates participation, and `0` indicates no participation. |
| `shift` | Input | Element-wise shift register or `int16_t` scalar. In register form, the `shift` type is `reg_tensor<ShiftType>`, and `ShiftType` must be a signed integer type with matching bit width to `T`; `shift.mask` is not used in this computation. |

## Data Type

| Shift form | `src` element type | `shift` type |
| --- | --- | --- |
| Register shift | `int8_t`, `uint8_t` | `reg_tensor<int8_t>` |
| Register shift | `int16_t`, `uint16_t` | `reg_tensor<int16_t>` |
| Register shift | `int32_t`, `uint32_t` | `reg_tensor<int32_t>` |
| Scalar shift | `int8_t`, `uint8_t`, `int16_t`, `uint16_t`, `int32_t`, `uint32_t` | `int16_t` |

## Return Value

Returns `reg_tensor<T>`. `reg` stores the element-wise left shift result. When the corresponding position of `src.mask` is `0`, the corresponding position of `reg` is set to zero. The `mask` of the return value is the same as `src.mask`.

## Constraints

- `src.mask` must be set in advance through the `with_mask` API. If it is not set, the content of the mask is undefined, which may cause incorrect element positions to participate in the computation.
- The behavior is undefined when the shift amount is negative. When the shift amount is greater than the bit width of the `src` element type, the result is `0`.
- The high-order bits of the left shift result that exceed the bit width of `T` are discarded, and only the low-order bits are retained.

## Example

The following example shows a typical way to call the API in an AIV compute function. The example first binds a mask to the load result, then performs the computation and writes the result back to UB.

```cpp
#include "tensor_api/tensor.h"
#include "tensor_api/experimental/arch/vector/logical_compute.h"
#include "tensor_api/experimental/arch/vector/mask_reg_compute.h"
#include "tensor_api/experimental/arch/vector/reg_data_load.h"
#include "tensor_api/experimental/arch/vector/reg_data_store.h"

template <typename InputTensor, typename OutputTensor>
__simd_vf__ inline void shift_left_example(InputTensor input, OutputTensor output)
{
    using asc::te::experimental::operator<<;

    auto src = asc::te::experimental::load(input, asc::te::make_coord(0)).with_mask(asc::te::experimental::all_mask<int32_t>());

    auto result = src << static_cast<int16_t>(2);
    asc::te::experimental::store(output, asc::te::make_coord(0), result);
}
```
