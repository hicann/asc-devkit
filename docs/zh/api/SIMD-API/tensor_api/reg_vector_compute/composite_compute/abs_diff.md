# abs_diff

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

该接口根据`src0.mask`将`src0`与`src1`按元素相减并求绝对值，返回计算结果。计算公式如下：

$$
dst_i = \lvert src0_i - src1_i \rvert
$$

## 函数原型

```cpp
template <typename T>
__simd_callee__ inline reg_tensor<T> abs_diff(const reg_tensor<T>& src0, const reg_tensor<T>& src1)
```

## 参数说明

**表1** 模板参数说明

| 参数名 | 描述 |
| --- | --- |
| T | 操作数数据类型。支持的数据类型请参考[数据类型](#数据类型)。 |

**表2** 参数说明

| 参数名 | 输入/输出 | 描述 |
| --- | --- | --- |
| src0 | 输入 | 被减数，类型为`reg_tensor<T>`。`src0.mask`用于标记参与计算的元素，对应位置为`1`时参与计算，为`0`时不参与计算。 |
| src1 | 输入 | 减数，类型为`reg_tensor<T>`。`src1.mask`不用于本次计算。 |

## 返回值说明

返回`reg_tensor<T>`。`reg`保存两个输入逐元素作差后的绝对值，返回值的`mask`与`src0.mask`相同。

## 数据类型

**表3** 数据类型组合

| src0 | src1 | dst |
| --- | --- | --- |
| half | half | half |
| float | float | float |

## 约束说明

- `src0.mask`需通过`with_mask`接口预先设置。未设置时，mask的内容不确定，会导致参与计算的元素位置错误。
- `src0.mask`对应位置为`0`时，返回值`reg`的对应位置置零。

## 调用示例

```cpp
#include "tensor_api/experimental/vector_compute.h"

template <typename Src0Tensor, typename Src1Tensor, typename DstTensor>
__simd_vf__ inline void abs_diff_vf(
    const Src0Tensor src0, const Src1Tensor src1, DstTensor dst, uint16_t repeat_times, uint32_t total,
    uint32_t one_repeat_size)
{
    using data_type = typename Src0Tensor::data_type;
    uint32_t remain = total;
    for (uint16_t i = 0; i < repeat_times; ++i) {
        const uint32_t offset = i * one_repeat_size;
        const auto coord = asc::te::make_coord(offset);
        auto mask = asc::te::experimental::update_mask<data_type>(remain);
        auto src0_reg = asc::te::experimental::load(src0, coord).with_mask(mask);
        auto src1_reg = asc::te::experimental::load(src1, coord).with_mask(mask);
        auto dst_reg = asc::te::experimental::abs_diff(src0_reg, src1_reg);
        asc::te::experimental::store(dst, coord, dst_reg);
    }
}
```
