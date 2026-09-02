# log

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

头文件路径：`"tensor_api/experimental/vector_compute.h"`。

该接口根据src_reg.mask，对源操作数src_reg中的有效元素逐个求自然对数（以e为底），并返回计算结果。计算公式如下：

$$
dst_i = \ln(src\_reg_i)
$$

## 函数原型

```cpp
template <typename T>
__simd_callee__ inline reg_tensor<T> log(const reg_tensor<T>& src_reg)
```

## 参数说明

**表 1**  模板参数说明

| 参数名 | 描述 |
| --- | --- |
| T | 操作数数据类型。支持的数据类型请参考[数据类型](#数据类型)。 |

**表 2**  参数说明

| 参数名 | 输入/输出 | 描述 |
| --- | --- | --- |
| src_reg | 输入 | 源操作数，类型为reg_tensor&lt;T&gt;。其中，src_reg.reg保存矢量数据，src_reg.mask用于控制各元素是否参与计算。src_reg.mask中与元素对应的比特位为1时，该元素参与计算；为0时，该元素不参与计算。 |

## 数据类型

源操作数与返回值的数据类型保持一致。支持的数据类型为：half、float。

## 返回值说明

返回自然对数计算结果，类型为reg_tensor&lt;T&gt;。返回值的mask与src_reg.mask相同；src_reg.mask对应位置为0时，返回值的对应元素置零。

## 约束说明

- `src_reg.mask`需通过`with_mask`接口预先设置。未设置时，mask的内容不确定，会导致参与计算的元素位置错误。

## 调用示例

```cpp
#include "tensor_api/experimental/vector_compute.h"

template <typename InputTensor, typename OutputTensor>
__simd_vf__ inline void log_example(InputTensor input, OutputTensor output)
{
    auto src_reg = asc::te::experimental::load(input, asc::te::make_coord(0))
                       .with_mask(asc::te::experimental::all_mask<float>());
    auto dst_reg = asc::te::experimental::log(src_reg);
    asc::te::experimental::store(output, asc::te::make_coord(0), dst_reg);
}
```
