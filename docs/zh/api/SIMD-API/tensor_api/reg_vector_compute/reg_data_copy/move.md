# move

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

头文件路径为：`"tensor_api/experimental/arch/vector/reg_data_copy.h"`，接口命名空间为`asc::te::experimental`。

完成寄存器之间的数据搬运。将`src`中被`src.mask`筛选的有效元素复制到目的寄存器的对应位置。

- `T`为非`bool`类型时，调用双参数重载，未被`src.mask`筛选的位置采用Merging模式，保持`dst.reg`中的原值。
- `T`为`bool`类型时，调用单参数重载，将`src.reg`中被`src.mask`筛选的bit复制到返回值，未被`src.mask`筛选的bit置0。

双参数重载仅使用`src.mask`控制参与搬运的元素，不修改`dst.mask`，返回值的`mask`成员与调用前的`dst.mask`相同。单参数重载会将`src.mask`写入返回值的`mask`成员。

本接口为Reg矢量搬运接口，仅在AIV上生效。

## 函数原型

```cpp
template <typename T>
__simd_callee__ inline reg_tensor<T> move(reg_tensor<T>& dst, const reg_tensor<T>& src)

__simd_callee__ inline reg_tensor<bool> move(const reg_tensor<bool>& src)
```

## 参数说明

| 参数名 | 输入/输出 | 描述 |
| --- | --- | --- |
| T | 输入 | 双参数重载的模板参数，源、目的寄存器的数据类型，不支持`bool`。单参数重载没有模板参数。 |
| dst | 输入/输出 | 双参数重载的目的寄存器，类型为`reg_tensor<T>`，数据类型须与`src`完全一致。`src.mask`部分有效时，`dst.reg`需提前初始化，否则未被筛选的位置将出现未知值。接口不修改`dst.mask`。 |
| src | 输入 | 源寄存器，类型为`reg_tensor<T>`。其`mask`成员用于指示参与复制的元素。对于双参数重载，对应位置无效的元素保留`dst.reg`中的原值；对于单参数重载，对应位置无效的bit在返回值中置0。 |

## 数据类型

- 双参数重载支持的数据类型为：uint8_t、int8_t、uint16_t、int16_t、half、bfloat16_t、uint32_t、int32_t、float。
- 单参数重载支持的数据类型为：bool。

## 返回值说明

双参数重载返回`reg_tensor<T>`类型的目的寄存器，其`mask`成员与调用前的`dst.mask`相同。单参数重载返回`reg_tensor<bool>`，其`mask`成员与`src.mask`相同。

## 约束说明

- 双参数重载的`dst`与`src`必须为数据类型相同的`reg_tensor`，且`T`不能为`bool`。
- 单参数重载仅支持`reg_tensor<bool>`。
- 调用接口前必须初始化`src.mask`，用于指定参与复制的元素。
- 双参数重载中，`src.mask`未筛选的位置保持`dst.reg`中的原值。`src.mask`部分有效时，必须在调用接口前初始化`dst.reg`。
- 双参数重载不修改`dst.mask`，返回值的`mask`成员与调用前的`dst.mask`相同。
- 单参数重载中，`src.mask`未筛选的bit在返回值中置0。
- 单参数重载会将`src.mask`写入返回值的`mask`成员。
- 寄存器数据搬运应在`__simd_vf__`函数或`__simd_callee__`函数中完成。

## 调用示例

非`bool`类型调用双参数重载：

```cpp
template <typename SrcTensor, typename DstTensor>
__simd_vf__ inline void MoveData(SrcTensor src, DstTensor dst,
    uint32_t repeat_times, uint32_t total_length,
    uint32_t one_repeat_size)
{
    using data_type = typename SrcTensor::data_type;
    const data_type init_value = static_cast<data_type>(1);
    uint32_t remain = total_length;
    for (uint16_t i = 0; i < repeat_times; ++i) {
        auto mask = asc::te::experimental::update_mask<data_type>(remain);
        uint32_t offset = i * one_repeat_size;
        const auto coord = asc::te::make_coord(offset);
        auto src_reg = asc::te::experimental::load(src, coord).with_mask(mask);
        auto dst_reg = asc::te::experimental::fill(init_value);
        auto result = asc::te::experimental::move(dst_reg, src_reg);
        asc::te::experimental::store(dst, coord, result);
    }
}
```

`bool`类型调用单参数重载：

```cpp
template <typename SrcTensor, typename DstTensor>
__simd_vf__ inline void MoveMask(SrcTensor src, DstTensor dst,
    uint32_t repeat_times, uint32_t total_length,
    uint32_t one_repeat_size)
{
    using data_type = typename SrcTensor::data_type;
    uint32_t remain = total_length;
    auto mask = asc::te::experimental::all_mask<data_type>();
    for (uint16_t i = 0; i < repeat_times; ++i) {
        auto src_reg = asc::te::experimental::update_mask<data_type>(remain);
        src_reg.with_mask(mask);
        auto result = asc::te::experimental::move(src_reg);
        uint32_t offset = i * one_repeat_size;
        const auto coord = asc::te::make_coord(offset);
        auto data_reg = asc::te::experimental::load(src, coord).with_mask(result);
        asc::te::experimental::store(dst, coord, data_reg);
    }
}
```
