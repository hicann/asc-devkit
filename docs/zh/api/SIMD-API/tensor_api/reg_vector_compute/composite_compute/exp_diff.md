# exp_diff

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

该接口根据`src0.mask`将`src0`与`src1`按元素相减，再以差值作为e的指数，返回计算结果。计算公式如下：

- 输入数据类型为`float`时：

$$
dst_i = e^{(src0_i - src1_i)}
$$

- 输入数据类型为`half`时：

$$
dst_i = e^{(cast\_f16\_to\_f32(src0_i) - cast\_f16\_to\_f32(src1_i))}
$$

输入数据类型为`float`时按连续元素计算，无需指定`src_pos`。输入数据类型为`half`时必须指定`src_pos`，选择偶数位置或奇数位置。

## 函数原型

```cpp
template <typename T, typename U>
__simd_callee__ inline reg_tensor<T> exp_diff(const reg_tensor<U>& src0, const reg_tensor<U>& src1)

template <typename T, typename U, typename PositionType>
__simd_callee__ inline reg_tensor<T> exp_diff(const reg_tensor<U>& src0, const reg_tensor<U>& src1, PositionType src_pos)
```

## 参数说明

**表1** 模板参数说明

| 参数名 | 描述 |
| --- | --- |
| T | 返回值的元素类型。支持的数据类型请参考[数据类型](#数据类型)。调用时必须显式指定。 |
| U | 源操作数的元素类型。`half`表示16位浮点类型。支持的数据类型请参考[数据类型](#数据类型)。 |
| PositionType | 位置参数类型，由`src_pos`自动推导。 |

**表2** 参数说明

| 参数名 | 输入/输出 | 描述 |
| --- | --- | --- |
| src0 | 输入 | 被减数，类型为`reg_tensor<U>`。`src0.mask`按输出元素编号，对应位置为`1`时该输出元素参与计算，为`0`时不参与计算。 |
| src1 | 输入 | 减数，类型与`src0`相同。`src1.mask`不用于本次计算。 |
| src_pos | 输入 | `U`为`half`时必填，仅支持`ASC_POSITION_EVEN`和`ASC_POSITION_ODD`，分别表示选择偶数位置和奇数位置。 |

## 返回值说明

返回`reg_tensor<T>`，返回值的`mask`与`src0.mask`相同。`U`为`half`时，`ASC_POSITION_EVEN`和`ASC_POSITION_ODD`分别使第`i`个输出元素读取源操作数的第`2 * i`和第`2 * i + 1`个元素。

## 数据类型

**表3** 数据类型组合

| src0 | src1 | dst |
| --- | --- | --- |
| half | half | float |
| float | float | float |

## 约束说明

- `src0.mask`需通过`with_mask`接口预先设置。未设置时，mask的内容不确定，会导致参与计算的元素位置错误。
- `src0.mask`对应位置为`0`时，返回值`reg`的对应位置置零。
- `U`为`half`时，`src0.mask`的第`i`位控制第`i`个`T`输出元素。mask应使用`all_mask<T>`、`make_mask<..., T>`或`update_mask<T>`生成。

## 调用示例

以下示例展示`half`输入的典型调用方式。

```cpp
#include "tensor_api/experimental/vector_compute.h"

template <typename Src0Tensor, typename Src1Tensor, typename DstTensor>
__simd_vf__ inline void exp_diff_vf(
    const Src0Tensor src0, const Src1Tensor src1, DstTensor dst, uint16_t repeat_times, uint32_t total,
    uint32_t src_one_repeat_size, uint32_t dst_one_repeat_size)
{
    uint32_t remain = total;
    for (uint16_t i = 0; i < repeat_times; ++i) {
        const auto src_coord = asc::te::make_coord(i * src_one_repeat_size);
        const auto dst_coord = asc::te::make_coord(i * dst_one_repeat_size);
        auto mask = asc::te::experimental::update_mask<float>(remain);
        auto src0_reg = asc::te::experimental::load(src0, src_coord).with_mask(mask);
        auto src1_reg = asc::te::experimental::load(src1, src_coord).with_mask(mask);
        auto dst_reg = asc::te::experimental::exp_diff<float, half>(src0_reg, src1_reg, ASC_POSITION_ODD);
        asc::te::experimental::store(dst, dst_coord, dst_reg);
    }
}
```
