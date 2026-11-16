# fill

## 产品支持情况

<!-- npu="950" id1 -->
- Ascend 950PR&950DT系列产品：支持
<!-- end id1 -->
<!-- npu="A3" id2 -->
- Atlas A3系列产品：不支持
<!-- end id2 -->
<!-- npu="910b" id3 -->
- Atlas A2系列产品：不支持
<!-- end id3 -->
<!-- npu="310b" id4 -->
- Atlas 200I/500 A2推理产品：不支持
<!-- end id4 -->
<!-- npu="310p" id5 -->
- Atlas推理系列产品AI Core：不支持
<!-- end id5 -->
<!-- npu="310p" id6 -->
- Atlas推理系列产品Vector Core：不支持
<!-- end id6 -->
<!-- npu="910" id7 -->
- Atlas训练系列产品：不支持
<!-- end id7 -->

## 功能说明

头文件路径：`"tensor_api/experimental/arch/vector/data_padding.h"`。

`fill`将标量`value`或源寄存器`src`中的指定元素广播到目的寄存器中由`mask`选中的位置。以`src`为源操作数时，通过`part`选择最低位或最高位元素，默认选择最低位元素。

### [不指定目的寄存器](#不指定目的寄存器-1)

返回新的寄存器，`mask`未选中的元素置0。

### [指定目的寄存器](#指定目的寄存器-1)

将广播结果写入`dst`。对于`mask`未选中的元素，`zeroing`模式将其置0，`merging`模式保留`dst`中的原值。

## 函数原型

### 不指定目的寄存器

- 广播标量`value`：

    ```cpp
    template <size_t N = 1, typename T>
    __simd_callee__ inline reg_tensor<T> fill(const T& value, const reg_tensor<bool>& mask = all_mask<T>())
    ```

- 广播源寄存器`src`中由`part`指定的元素：

    ```cpp
    template <high_low_part part = high_low_part::low, size_t N = 1, typename T>
    __simd_callee__ inline reg_tensor<T> fill(const reg_tensor<T>& src, const reg_tensor<bool>& mask = all_mask<T>())
    ```

### 指定目的寄存器

- 广播标量`value`：

    ```cpp
    template <merge_mode Mode = merge_mode::zeroing, size_t N = 1, typename T>
    __simd_callee__ inline reg_tensor<T> fill(reg_tensor<T>& dst, const T& value,
        const reg_tensor<bool>& mask = all_mask<T>())
    ```

- 广播源寄存器`src`中由`part`指定的元素：

    ```cpp
    template <high_low_part part = high_low_part::low, merge_mode Mode = merge_mode::zeroing,
        size_t N = 1, typename T>
    __simd_callee__ inline reg_tensor<T> fill(reg_tensor<T>& dst, const reg_tensor<T>& src,
        const reg_tensor<bool>& mask = all_mask<T>())
    ```

## 参数说明

### 不指定目的寄存器

**表1** 模板参数说明

| 参数名 | 描述 |
| --- | --- |
| part | `high_low_part`枚举类型，仅在广播源寄存器元素时使用。支持`high_low_part::low`和`high_low_part::high`，默认值为`high_low_part::low`。 |
| N | 待填充的寄存器数量，默认值为1。 |
| T | 标量或寄存器的元素类型。支持的数据类型请参考[数据类型](#数据类型)。 |

**表2** 函数参数说明

| 参数名 | 输入/输出 | 描述 |
| --- | --- | --- |
| value | 输入 | 待广播的标量，类型为`T`。 |
| src | 输入 | 源寄存器，类型为`reg_tensor<T>`。待广播元素由`part`指定。 |
| mask | 输入 | 类型为`reg_tensor<bool>`。`mask.reg`中置1的比特选中对应的写入位置，默认值为`all_mask<T>()`。 |

### 指定目的寄存器

**表3** 模板参数说明

| 参数名 | 描述 |
| --- | --- |
| part | `high_low_part`枚举类型，仅在广播源寄存器元素时使用。支持`high_low_part::low`和`high_low_part::high`，默认值为`high_low_part::low`。 |
| Mode | [merge_mode](../aux_data_types/merge_mode.md)枚举类型，掩码合并模式，支持`merge_mode::zeroing`和`merge_mode::merging`，默认值为`merge_mode::zeroing`。 |
| N | 待填充的寄存器数量，默认值为1。 |
| T | 标量或寄存器的元素类型。支持的数据类型请参考[数据类型](#数据类型)。 |

**表4** 函数参数说明

| 参数名 | 输入/输出 | 描述 |
| --- | --- | --- |
| dst | 输入/输出 | 目的寄存器，类型为`reg_tensor<T>`。写入广播结果时不修改`dst.mask`。 |
| value | 输入 | 待广播的标量，类型为`T`。 |
| src | 输入 | 源寄存器，类型为`reg_tensor<T>`。待广播元素由`part`指定。 |
| mask | 输入 | 类型为`reg_tensor<bool>`。`mask.reg`中置1的比特选中对应的写入位置，默认值为`all_mask<T>()`。 |

## 数据类型

两类重载支持的元素类型`T`相同：`int8_t`、`uint8_t`、`fp8_e8m0_t`、`fp8_e5m2_t`、`fp8_e4m3fn_t`、`int16_t`、`uint16_t`、`half`、`bfloat16_t`、`int32_t`、`uint32_t`、`float`。

## 返回值说明

### 不指定目的寄存器

返回包含广播结果的`reg_tensor<T>`，其`mask`成员为输入的`mask.reg`。

### 指定目的寄存器

返回填充后的`dst`，类型为`reg_tensor<T>`，保留`dst`原有的`mask`成员。

## 约束说明

- `N`当前仅支持1。
- `mask`仅筛选目的寄存器中的写入位置，不影响从`src`中读取的元素。
- 对于源操作数为`src`的重载，`part`为`high_low_part::low`时广播`src`的最低位元素（下标为0的元素），`part`为`high_low_part::high`时广播`src`的最高位元素（下标最大的元素）。
- 对于指定`dst`的重载，`Mode`为`merge_mode::zeroing`时，mask未选中的元素置0；`Mode`为`merge_mode::merging`时，mask未选中的元素保留`dst`中的原值。

## 调用示例

### 不指定目的寄存器

- 广播标量：

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

- 广播`src`的最高位元素：

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

### 指定目的寄存器

将标量广播到指定寄存器，并写回当前块的元素。

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
