# sub_carry

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

头文件路径：`"tensor_api/experimental/arch/vector/basic_arithmetic.h"`。

该接口根据src0.mask，对输入元素执行逐元素减法，并返回结果和借位。两个重载分别为不带输入借位、带输入借位，计算公式如下：

$$
\{second_i, first_i\} = \{0, src0_i\} - \{0, src1_i\}
$$

$$
\{second_i, first_i\} = \{0, src0_i\} - \{0, src1_i\} - borrow\_src_i
$$

返回值second表示每个元素的借位输出。

## 函数原型

```cpp
template <typename T>
__simd_callee__ inline reg_pair<T, bool> sub_carry(
    const reg_tensor<T>& src0, const reg_tensor<T>& src1)

template <typename T>
__simd_callee__ inline reg_pair<T, bool> sub_carry(
    const reg_tensor<T>& src0, const reg_tensor<T>& src1, const reg_tensor<bool>& borrow_src)
```

## 参数说明

**表1** 模板参数说明

| 参数名 | 描述 |
| --- | --- |
| T | 操作数数据类型。支持的数据类型请参考[数据类型](#数据类型)。 |

**表2** 参数说明

| 参数名 | 输入/输出 | 描述 |
| --- | --- | --- |
| src0 | 输入 | 被减数，类型为reg_tensor&lt;T&gt;。其中，src0.reg保存矢量数据，src0.mask用于控制各元素是否参与计算。src0.mask中与元素对应的比特位为1时，该元素参与计算；为0时，该元素不参与计算。 |
| src1 | 输入 | 减数，类型为reg_tensor&lt;T&gt;。src1.mask不参与本次计算。 |
| borrow_src | 输入 | 借位输入，类型为reg_tensor&lt;bool&gt;。对应元素为1时再减1，仅第二个重载使用。borrow_src.mask不参与本次计算。 |

## 返回值说明

返回类型为`reg_pair<T, bool>`：`first`保存减法结果，`second`保存借位。两个成员的`mask`均设置为`src0.mask`。

## 数据类型

**表3** 数据类型组合

| src0 | src1 | borrow_src | first | second |
| --- | --- | --- | --- | --- |
| int32_t | int32_t | bool | int32_t | bool |
| uint32_t | uint32_t | bool | uint32_t | bool |

## 约束说明

- src0.mask需通过`with_mask`接口预先设置。未设置时，mask的内容不确定，会导致参与计算的元素位置错误。
- borrow_src.reg必须已初始化。

## 调用示例

### 不带输入借位

```cpp
#include "tensor_api/experimental/vector_compute.h"

template <typename InputTensor, typename OutputTensor>
__simd_vf__ inline void sub_carry_vf(
    const InputTensor input0, const InputTensor input1, OutputTensor output, uint16_t repeat_times,
    uint32_t total, uint32_t one_repeat_size)
{
    using data_type = typename InputTensor::data_type;
    uint32_t remain = total;
    for (uint16_t i = 0; i < repeat_times; ++i) {
        const uint32_t offset = i * one_repeat_size;
        const auto coord = asc::te::make_coord(offset);
        auto mask = asc::te::experimental::update_mask<data_type>(remain);
        auto src0_reg = asc::te::experimental::load(input0, coord).with_mask(mask);
        auto src1_reg = asc::te::experimental::load(input1, coord);
        auto result = asc::te::experimental::sub_carry(src0_reg, src1_reg);
        asc::te::experimental::store(output, coord, result.first);
    }
}
```

### 带输入借位

```cpp
#include "tensor_api/experimental/vector_compute.h"

template <typename InputTensor, typename OutputTensor>
__simd_vf__ inline void sub_carry_with_borrow_vf(
    const InputTensor input0, const InputTensor input1, OutputTensor output, uint16_t repeat_times,
    uint32_t total, uint32_t one_repeat_size)
{
    using data_type = typename InputTensor::data_type;
    uint32_t remain = total;
    for (uint16_t i = 0; i < repeat_times; ++i) {
        const uint32_t offset = i * one_repeat_size;
        const auto coord = asc::te::make_coord(offset);
        auto mask = asc::te::experimental::update_mask<data_type>(remain);
        auto src0_reg = asc::te::experimental::load(input0, coord).with_mask(mask);
        auto src1_reg = asc::te::experimental::load(input1, coord);
        auto borrow_src = asc::te::experimental::none_mask<data_type>();
        auto result = asc::te::experimental::sub_carry(src0_reg, src1_reg, borrow_src);
        asc::te::experimental::store(output, coord, result.first);
    }
}
```
