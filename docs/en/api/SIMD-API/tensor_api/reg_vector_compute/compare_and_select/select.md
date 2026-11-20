# select

<!-- md-trans-meta sourceCommit=dacba287b3c94417af1e532d9cbd29c398889dea translatedAt=2026-10-27T03:34:04.109Z pushedAt=2026-10-27T13:03:35.394Z nl=2 -->

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

Header file path: `"tensor_api/experimental/arch/vector/compare_and_select.h"`.

Given two source operands src0 and src1, this API selects elements based on the bit values of condition to produce the destination operand. When a bit of condition is 1, the element at the corresponding position is selected from src0; when the bit is 0, it is selected from src1.

## Function Prototype

```cpp
template <typename T>
__simd_callee__ inline reg_tensor<T> select(
    const reg_tensor<bool>& condition, const reg_tensor<T>& src0, const reg_tensor<T>& src1)
```

## Parameters

| Parameter | Input/Output | Description |
| --- | --- | --- |
| condition | Input | Selection condition, of type `reg_tensor<bool>`. Each element determines whether the element at the corresponding position is selected from `src0` or `src1`. |
| src0 | Input | Source operand selected when the condition is `true`, of type `reg_tensor<T>`. Its `mask` serves as the valid element mask of the return value. |
| src1 | Input | Source operand selected when the condition is `false`, of type `reg_tensor<T>`. |

## Return Value

Returns `reg_tensor<T>`, which contains the result of element-wise selection. The `mask` of the return value is the same as `src0.mask`.

## Data Type

Supported data types: bool, int8_t, uint8_t, fp8_e4m3fn_t, hifloat8_t, fp8_e5m2_t, int16_t, uint16_t, half, bfloat16_t, int32_t, uint32_t, float.

## Constraints

Unselected elements controlled by the mask parameter of src0 result in undefined behavior.

## Example

```cpp
template <typename T, typename U, typename V>
__simd_vf__ inline void select_vf(T x_tensor, U y_tensor, V z_tensor,
    uint32_t repeat_times, uint32_t one_repeat_size)
{
    uint32_t count = TOTAL_LENGTH;
    using data_type = typename T::data_type;
    auto condition = asc::te::experimental::make_mask<asc::te::experimental::mask_pattern::vl8, data_type>();
    for (uint16_t i = 0; i < repeat_times; ++i) {
        auto mask = asc::te::experimental::update_mask<data_type>(count);
        uint32_t offset = i * one_repeat_size;
        const auto coord = asc::te::make_coord(offset);
        auto x_reg = asc::te::experimental::load(x_tensor, coord).with_mask(mask);
        auto y_reg = asc::te::experimental::load(y_tensor, coord);
        auto z_reg = asc::te::experimental::select(condition, x_reg, y_reg);
        asc::te::experimental::store(z_tensor, coord, z_reg);
    }
}
```
