# move_mask

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

从源矢量数据寄存器的指定数据块中提取bit并扩展，生成掩码寄存器。

- `T`为`uint16_t`时，256B的源矢量数据寄存器分成16个数据块，每个数据块为16B。选中数据块的每个bit扩展为2bit，生成32B的掩码寄存器。
- `T`为`uint32_t`时，256B的源矢量数据寄存器分成32个数据块，每个数据块为8B。选中数据块的每个bit扩展为4bit，生成32B的掩码寄存器。

本接口为Reg矢量搬运接口，仅在AIV上生效。

## 函数原型

`Offset`的值必须在编译期确定，可传入整数字面量或`constexpr`整数常量，不支持传入运行时变量。

```cpp
template <int16_t Offset, typename T>
__simd_callee__ inline reg_tensor<bool> move_mask(const reg_tensor<T>& src)
```

## 参数说明

| 参数名 | 输入/输出 | 描述 |
| --- | --- | --- |
| Offset | 输入 | 模板参数，决定从`src`中提取的数据块。`T`为`uint16_t`时取值范围为[0, 15]；`T`为`uint32_t`时取值范围为[0, 31]。 |
| T | 输入 | 模板参数，源寄存器的数据类型，支持`uint16_t`和`uint32_t`。 |
| src | 输入 | 源矢量数据寄存器，类型为`reg_tensor<T>`。接口读取其`reg`成员，不读取`mask`成员。 |

## 数据类型

支持的数据类型为：uint16_t、uint32_t。

## 返回值说明

返回`reg_tensor<bool>`。其中，`reg`成员保存提取并扩展后的32B掩码寄存器，`mask`成员为b8模式的全有效掩码。

## 约束说明

- `src`必须为`uint16_t`或`uint32_t`类型的`reg_tensor`。
- 本接口仅读取`src.reg`，不读取`src.mask`。
- `Offset`必须是编译期常量，可传入整数字面量或`constexpr`整数常量，不支持传入运行时变量。
    - `T`为`uint16_t`时，`Offset`取值范围为[0, 15]。
    - `T`为`uint32_t`时，`Offset`取值范围为[0, 31]。
- 寄存器数据搬运应在`__simd_vf__`函数或`__simd_callee__`函数中完成。

## 关键特性说明

`T`为`uint16_t`时，每个元素对应掩码寄存器中的2bit，且仅2bit中的最低位有效。选中的16B数据块包含128bit，每个bit扩展为2bit后生成一个32B掩码寄存器。一个256B源寄存器包含16个数据块，因此`Offset`取值范围为[0, 15]。

**图1** uint16_t矢量数据寄存器生成掩码寄存器<a id="fig-move-mask-b16"></a>

![uint16_t矢量数据寄存器生成掩码寄存器](../../../../figures/capi_copy_reg2mask_b16.png)

`T`为`uint32_t`时，每个元素对应掩码寄存器中的4bit，且仅4bit中的最低位有效。选中的8B数据块包含64bit，每个bit扩展为4bit后生成一个32B掩码寄存器。一个256B源寄存器包含32个数据块，因此`Offset`取值范围为[0, 31]。

**图2** uint32_t矢量数据寄存器生成掩码寄存器<a id="fig-move-mask-b32"></a>

![uint32_t矢量数据寄存器生成掩码寄存器](../../../../figures/capi_copy_reg2mask_b32.png)

## 调用示例

```cpp
template <typename SrcTensor, typename DstTensor>
__simd_vf__ inline void MoveMaskData(SrcTensor src, DstTensor dst,
    uint32_t repeat_times, uint32_t one_repeat_size)
{
    using data_type = typename SrcTensor::data_type;
    const data_type fill_value = 1;
    for (uint32_t i = 0; i < repeat_times; ++i) {
        uint32_t offset = i * one_repeat_size;
        const auto coord = asc::te::make_coord(offset);
        auto src_reg = asc::te::experimental::load(src, coord);
        auto mask = asc::te::experimental::move_mask<0>(src_reg);
        auto result = asc::te::experimental::fill(fill_value, mask);
        asc::te::experimental::store(dst, coord, result);
    }
}
```
