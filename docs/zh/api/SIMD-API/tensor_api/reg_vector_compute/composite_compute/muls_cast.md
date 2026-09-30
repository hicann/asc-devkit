# muls_cast

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

头文件路径：`"tensor_api/experimental/arch/vector/composite_compute.h"`。

该接口根据`src.mask`将`src`与标量`scalar`按元素相乘，再按照`CAST_ROUND`模式将结果转换为`half`类型并返回。计算公式如下：

$$
dst_i = cast\_round\_to\_f16(src_i \times scalar)
$$

默认布局为`cast_layout::zero`，转换结果写入返回寄存器中每组两个`half`元素的第0个位置（偶数索引），第1个位置置零。

## 函数原型

```cpp
template <cast_layout Layout = cast_layout::zero, typename T, typename U>
__simd_callee__ inline reg_tensor<T> muls_cast(const reg_tensor<U>& src, const U& scalar)
```

## 参数说明

**表1**模板参数说明

| 参数名 | 描述 |
| --- | --- |
| Layout | 结果布局。支持`cast_layout::zero`和`cast_layout::one`，默认值为`cast_layout::zero`。 |
| T | 返回值元素类型。支持的数据类型请参考[数据类型](#数据类型)。 |
| U | 源操作数和标量类型。支持的数据类型请参考[数据类型](#数据类型)。 |

**表2**参数说明

| 参数名 | 输入/输出 | 描述 |
| --- | --- | --- |
| src | 输入 | 源操作数，类型为`reg_tensor<U>`。`src.mask`用于标记参与计算的元素，对应位置为`1`时参与计算，为`0`时不参与计算。 |
| scalar | 输入 | `U`类型的标量乘数。 |

## 返回值说明

返回`reg_tensor<T>`。`Layout`为`cast_layout::zero`时，转换结果写入每组两个`half`元素的第0个位置（偶数索引），第1个位置置零；`Layout`为`cast_layout::one`时，转换结果写入第1个位置（奇数索引），第0个位置置零。返回值的`mask`与`src.mask`相同。

## 数据类型

**表3**数据类型组合

| src | scalar | dst |
| --- | --- | --- |
| float | float | half |

## 约束说明

- `src.mask`需通过`with_mask`接口预先设置。未设置时，mask的内容不确定，会导致参与计算的元素位置错误。
- `src.mask`对应位置为`0`时，返回值`reg`的对应位置置零。
- 类型转换按照`CAST_ROUND`模式舍入。

## 调用示例

```cpp
#include "tensor_api/experimental/vector_compute.h"

template <typename SrcTensor, typename DstTensor>
__simd_vf__ inline void muls_cast_vf(
    const SrcTensor src, DstTensor dst, float scalar, uint16_t repeat_times, uint32_t total,
    uint32_t src_one_repeat_size, uint32_t dst_one_repeat_size)
{
    uint32_t remain = total;
    for (uint16_t i = 0; i < repeat_times; ++i) {
        const auto src_coord = asc::te::make_coord(i * src_one_repeat_size);
        const auto dst_coord = asc::te::make_coord(i * dst_one_repeat_size);
        auto mask = asc::te::experimental::update_mask<float>(remain);
        auto src_reg = asc::te::experimental::load(src, src_coord).with_mask(mask);
        auto dst_reg = asc::te::experimental::muls_cast<asc::te::experimental::cast_layout::one, half>(
            src_reg, scalar);
        asc::te::experimental::store(dst, dst_coord, dst_reg);
    }
}
```
