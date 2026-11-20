# fill

<!-- md-trans-meta sourceCommit=98568e61facccbf29db5353153551ecc6fe6458d translatedAt=2026-11-19T02:48:01.009Z pushedAt=2026-11-19T05:54:06.662Z nl=2 -->

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

Header file path: `"tensor_api/experimental/arch/vector/data_padding.h"`.

`fill` broadcasts the scalar `value` or the specified element in the source register `src` to the positions selected by `mask` in the destination register. When `src` is used as the source operand, `part` selects the least significant or most significant element, with the least significant element selected by default.

### [Unspecified Destination Register](#unspecified-destination-register-1)

Returns a new register, with unselected elements of `mask` set to 0.

### [Specified Destination Register](#specified-destination-register-1)

Writes the broadcast result to `dst`. For elements not selected by `mask`, the `zeroing` mode sets them to 0, and the `merging` mode preserves the original value in `dst`.

## Prototype

### Unspecified Destination Register

- Broadcast the scalar `value`:

    ```cpp
    template <size_t N = 1, typename T>
    __simd_callee__ inline reg_tensor<T> fill(const T& value, const reg_tensor<bool>& mask = all_mask<T>())
    ```

- Broadcast the element specified by `part` in the source register `src`:

    ```cpp
    template <high_low_part part = high_low_part::low, size_t N = 1, typename T>
    __simd_callee__ inline reg_tensor<T> fill(const reg_tensor<T>& src, const reg_tensor<bool>& mask = all_mask<T>())
    ```

### Specified Destination Register

- Broadcast the scalar `value`:

    ```cpp
    template <merge_mode Mode = merge_mode::zeroing, size_t N = 1, typename T>
    __simd_callee__ inline reg_tensor<T> fill(reg_tensor<T>& dst, const T& value,
        const reg_tensor<bool>& mask = all_mask<T>())
    ```

- Broadcast the element specified by `part` in the source register `src`:

    ```cpp
    template <high_low_part part = high_low_part::low, merge_mode Mode = merge_mode::zeroing,
        size_t N = 1, typename T>
    __simd_callee__ inline reg_tensor<T> fill(reg_tensor<T>& dst, const reg_tensor<T>& src,
        const reg_tensor<bool>& mask = all_mask<T>())
    ```

## Parameters

### Unspecified Destination Register

**Table 1** Template parameters

| Parameter | Description |
| --- | --- |
| part | `high_low_part` enumeration type, used only when broadcasting source register elements. Supports `high_low_part::low` and `high_low_part::high`, with a default value of `high_low_part::low`. |
| N | Number of registers to be filled. The default value is 1. |
| T | Element type of scalar or register. For supported data types, see [Data Type](#data-type). |

**Table 2** Function parameter description

| Parameter | Input/Output | Description |
| --- | --- | --- |
| value | Input | Scalar to be broadcast, of type `T`. |
| src | Input | Source register, of type `reg_tensor<T>`. The elements to be broadcast are specified by `part`. |
| mask | Input | Type is `reg_tensor<bool>`. Bits set to 1 in `mask.reg` select the corresponding write positions. The default value is `all_mask<T>()`. |

### Specified Destination Register

**Table 3** Template parameters

| Parameter | Description |
| --- | --- |
| part | `high_low_part` enumeration type, used only when broadcasting source register elements. Supports `high_low_part::low` and `high_low_part::high`, and the default value is `high_low_part::low`. |
| Mode | [merge_mode](../aux_data_types/merge_mode.md) enumeration type, mask merge mode. Supports `merge_mode::zeroing` and `merge_mode::merging`, and the default value is `merge_mode::zeroing`. |
| N | Number of registers to be filled, and the default value is 1. |
| T | Element type of scalar or register. For supported data types, see [Data Type](#data-type). |

**Table 4** Function parameters

| Parameter | Input/Output | Description |
| --- | --- | --- |
| dst | Input/Output | Destination register, of type `reg_tensor<T>`. `dst.mask` is not modified when the broadcast result is written. |
| value | Input | Scalar to be broadcast, of type `T`. |
| src | Input | Source register, of type `reg_tensor<T>`. The elements to be broadcast are specified by `part`. |
| mask | Input | Of type `reg_tensor<bool>`. Bits set to 1 in `mask.reg` select the corresponding write positions, and the default value is `all_mask<T>()`. |

## Data Type

The two overloads support the same element type `T`: `int8_t`, `uint8_t`, `fp8_e8m0_t`, `fp8_e5m2_t`, `fp8_e4m3fn_t`, `int16_t`, `uint16_t`, `half`, `bfloat16_t`, `int32_t`, `uint32_t`, and `float`.

## Return Value

### Unspecified Destination Register

Returns a `reg_tensor<T>` containing the broadcast result, whose `mask` member is the input `mask.reg`.

### Specified Destination Register

Returns the filled `dst`, whose type is `reg_tensor<T>`, retaining the original `mask` member of `dst`.

## Constraints

- `N` currently supports only 1.
- `mask` only filters the write positions in the destination register and does not affect the elements read from `src`.
- For the overload with `src` as the source operand, when `part` is `high_low_part::low`, the least significant element of `src` (the element with subscript 0) is broadcast; when `part` is `high_low_part::high`, the most significant element of `src` (the element with the largest subscript) is broadcast.
- For the overload that specifies `dst`, when `Mode` is `merge_mode::zeroing`, unselected elements are set to 0; when `Mode` is `merge_mode::merging`, unselected elements retain the original value in `dst`.

## Example

### Unspecified Destination Register

- Broadcast scalar:

    ```cpp
    template <typename T, typename U>
    __simd_vf__ inline void fill_vf(T y_tensor, U scalar, uint32_t repeat_times, uint32_t one_repeat_size)
    {
        uint32_t count = total_length;
        using data_type = typename T::data_type;
        for (uint16_t i = 0; i < repeat_times; ++i) {
            auto mask = asc::te::experimental::update_mask<data_type>(count);
            uint32_t offset = i * one_repeat_size;
            const auto coord = asc::te::make_coord(offset);
            auto y_reg = asc::te::experimental::fill(scalar, mask);
            asc::te::experimental::store(y_tensor, coord, y_reg);
        }
    }
    ```

- Broadcast the most significant element of `src`:

    ```cpp
    template <typename T, typename U>
    __simd_vf__ inline void fill_vf(T x_tensor, U y_tensor, uint32_t repeat_times, uint32_t one_repeat_size)
    {
        uint32_t count = total_length;
        using data_type = typename T::data_type;
        for (uint16_t i = 0; i < repeat_times; ++i) {
            auto mask = asc::te::experimental::update_mask<data_type>(count);
            uint32_t offset = i * one_repeat_size;
            const auto coord = asc::te::make_coord(offset);
            auto x_reg = asc::te::experimental::load(x_tensor, coord).with_mask(mask);
            auto y_reg = asc::te::experimental::fill<asc::te::experimental::high_low_part::high>(x_reg, mask);
            asc::te::experimental::store(y_tensor, coord, y_reg);
        }
    }
    ```

### Specified Destination Register

The scalar is broadcast to the specified register and written back to the elements of the current block.

```cpp
template <typename T, typename U>
__simd_vf__ inline void fill_vf(T y_tensor, U scalar, uint32_t repeat_times, uint32_t one_repeat_size)
{
    uint32_t count = total_length;
    using data_type = typename T::data_type;
    for (uint16_t i = 0; i < repeat_times; ++i) {
        auto mask = asc::te::experimental::update_mask<data_type>(count);
        uint32_t offset = i * one_repeat_size;
        const auto coord = asc::te::make_coord(offset);
        auto y_reg = asc::te::experimental::load(y_tensor, coord).with_mask(mask);
        y_reg = asc::te::experimental::fill<asc::te::experimental::merge_mode::merging>(
            y_reg, scalar, mask);
        asc::te::experimental::store(y_tensor, coord, y_reg);
    }
}
```
