# select

## 产品支持情况

<!-- npu="950" id1 -->
- Ascend 950PR/Ascend 950DT：支持
<!-- end id1 -->
<!-- npu="A3" id2 -->
- Atlas A3 训练系列产品/Atlas A3 推理系列产品：不支持
<!-- end id2 -->
<!-- npu="910b" id3 -->
- Atlas A2 训练系列产品/Atlas A2 推理系列产品：不支持
<!-- end id3 -->
<!-- npu="310b" id4 -->
- Atlas 200I/500 A2 推理产品：不支持
<!-- end id4 -->
<!-- npu="310p" id5 -->
- Atlas 推理系列产品 AI Core：不支持
<!-- end id5 -->
<!-- npu="310p" id6 -->
- Atlas 推理系列产品 Vector Core：不支持
<!-- end id6 -->
<!-- npu="910" id7 -->
- Atlas 训练系列产品：不支持
<!-- end id7 -->

## 功能说明

头文件路径：`"include/tensor_api/experimental/arch/vector/compare_and_select.h"`。

给定两个源操作数src0和src1，根据condition的比特位值选取元素，得到目的操作数。当condition的比特位是1时，从src0中选取对应位置的数，比特位是0时从src1选取对应位置的数。

## 函数原型

```cpp
template <typename T>
__simd_callee__ inline reg_tensor<T> select(reg_tensor<bool> condition, reg_tensor<T> src0, reg_tensor<T> src1)
```

## 参数说明

| 参数 | 输入/输出 | 描述 |
| --- | --- | --- |
| condition | 输入 | 选择条件，类型为 `reg_tensor<bool>`。每个元素决定对应位置选择 `src0` 还是 `src1`。 |
| src0 | 输入 | 条件为 `true` 时选择的源操作数，类型为 `reg_tensor<T>`。其 `mask` 作为返回值的有效元素掩码。 |
| src1 | 输入 | 条件为 `false` 时选择的源操作数，类型为 `reg_tensor<T>`。 |

## 返回值说明

返回 `reg_tensor<T>`，包含逐元素选择的结果。返回值的 `mask` 与 `src0.mask` 相同。

## 数据类型

支持的数据类型为：bool、int8_t、uint8_t、fp8_e4m3fn_t、hifloat8_t、fp8_e5m2_t、int16_t、uint16_t、half、bfloat16_t、int32_t、uint32_t、float。

## 约束说明

通过src0的mask参数控制的未选中的元素是未定义行为。

## 调用示例

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
        auto x_reg = x_tensor.load(coord).with_mask(mask);
        auto y_reg = y_tensor.load(coord).with_mask(mask);
        auto z_reg = asc::te::experimental::select((x_reg > y_reg), x_reg, y_reg);
        z_tensor.store(coord, z_reg);
    }
}
```
