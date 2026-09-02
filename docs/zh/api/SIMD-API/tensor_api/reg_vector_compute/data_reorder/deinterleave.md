# deinterleave

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
- Atlas 推理系列产品AI Core：不支持
<!-- end id5 -->
<!-- npu="310p" id6 -->
- Atlas 推理系列产品Vector Core：不支持
<!-- end id6 -->
<!-- npu="910" id7 -->
- Atlas 训练系列产品：不支持
<!-- end id7 -->

## 功能说明

头文件路径：`"include/tensor_api/experimental/arch/vector/data_reorder.h"`。

给定源操作数寄存器src0和src1，将src0和src1中的元素解交织存入结果操作数dst0和dst1中。解交织排列方式如下图所示，其中每个方格代表一个元素：

![DeInterleave示意图](../../../../figures/reg_regdeinterleave.png)

## 函数原型

```cpp
template <typename T>
__simd_callee__ inline reg_pair<T> deinterleave(reg_tensor<T> src0, reg_tensor<T> src1);
```

## 参数说明

**表1**  模板参数说明

| 参数名 | 描述 |
| --- | --- |
| T | 目的操作数和源操作数的数据类型。支持的数据类型请参考[数据类型](#数据类型)。 |

**表2**  函数参数说明

| 参数名 | 输入/输出 | 描述 |
| --- | --- | --- |
| src0<br>src1 | 输入 | 源操作数。<br>类型为[reg_tensor]。 |

## 数据类型

支持的数据类型为：int8_t、uint8_t、fp8_e4m3fn_t、fp8_e8m0_t、fp8_e5m2_t、hifloat8_t、int16_t、uint16_t、half、bfloat16_t、int32_t、uint32_t、float。s

## 返回值说明

返回 `reg_pair<T>`，由两个 `reg_tensor<T>` 组成，存放交织后的两个结果值。返回值的 `mask` 是和数据类型T相关的满mask。

## 约束说明

- src0、src1的数据类型需要保持一致。
- src0和src1可以为同一个reg_tensor。

## 调用示例

```cpp
template <typename Src0TensorType, typename Src1TensorType, typename Dst0TensorType, typename Dst1TensorType>
__simd_vf__ inline void data_reorder_vf(
    Src0TensorType src0_tensor, Src1TensorType src1_tensor, Dst0TensorType dst0_tensor, Dst1TensorType dst1_tensor,
    uint32_t repeat_times, uint32_t one_repeat_size)
{
    for (uint16_t i = 0; i < repeat_times; ++i) {
        uint32_t offset = i * one_repeat_size;
        const auto coord = asc::te::make_coord(offset);
        auto src0_reg = asc::te::experimental::load(src0_tensor, coord);
        auto src1_reg = asc::te::experimental::load(src1_tensor, coord);
        auto result = asc::te::experimental::deinterleave(src0_reg, src1_reg);
        asc::te::experimental::store(dst0_tensor, coord, result.first);
        asc::te::experimental::store(dst1_tensor, coord, result.second);
    }
}
```
