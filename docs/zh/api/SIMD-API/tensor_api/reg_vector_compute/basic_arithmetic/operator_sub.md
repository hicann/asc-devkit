# operator-

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

该接口根据输入reg_tensor携带的mask，对两个源操作数进行按元素求差操作，并返回计算结果。支持reg_tensor与reg_tensor、reg_tensor与标量、标量与reg_tensor三种操作数组合。计算公式如下：

$$
dst_i = src0_i - src1_i
$$

## 函数原型

- reg_tensor与reg_tensor

    ```cpp
    template <typename T>
    __simd_callee__ inline reg_tensor<T> operator-(
        const reg_tensor<T>& src0, const reg_tensor<T>& src1)
    ```

- reg_tensor与标量

    ```cpp
    template <typename T>
    __simd_callee__ inline reg_tensor<T> operator-(
        const reg_tensor<T>& src, const T& scalar)

    template <typename T>
    __simd_callee__ inline reg_tensor<T> operator-(
        const T& scalar, const reg_tensor<T>& src)
    ```

## 参数说明

**表 1**  模板参数说明

| 参数名 | 描述 |
| --- | --- |
| T | 操作数数据类型。支持的数据类型请参考[数据类型](#数据类型)。 |

**表 2**  参数说明

| 参数名 | 输入/输出 | 描述 |
| --- | --- | --- |
| src0 | 输入 | reg_tensor与reg_tensor形式中的被减数，类型为reg_tensor&lt;T&gt;。其中，src0.reg保存矢量数据，src0.mask用于控制各元素是否参与计算。src0.mask中与元素对应的比特位为1时，该元素参与计算；为0时，该元素不参与计算。 |
| src1 | 输入 | reg_tensor与reg_tensor形式中的减数，类型为reg_tensor&lt;T&gt;。src1.mask不参与本次计算。 |
| src | 输入 | reg_tensor与标量形式中的reg_tensor操作数，类型为reg_tensor&lt;T&gt;。其中，src.reg保存矢量数据，src.mask用于控制各元素是否参与计算。src.mask中与元素对应的比特位为1时，该元素参与计算；为0时，该元素不参与计算。 |
| scalar | 输入 | 源操作数，类型为标量T。 |

## 数据类型

源操作数与返回值的数据类型保持一致。支持的数据类型为：int8_t、uint8_t、int16_t、uint16_t、half、bfloat16_t、int32_t、uint32_t、float。

## 返回值说明

返回逐元素求差结果，类型为reg_tensor&lt;T&gt;。reg_tensor与reg_tensor形式的返回值mask与src0.mask相同，包含标量的形式与src.mask相同；输入mask对应位置为0时，返回值的对应元素置零。

## 约束说明

- 用于控制计算的mask需通过`with_mask`接口预先设置。未设置时，mask的内容不确定，会导致参与计算的元素位置错误。
- 整型数据减法溢出时按环绕策略处理。有符号类型的最小值减1得到该类型的最大值，最大值加1得到该类型的最小值；无符号类型的0减1得到该类型的最大值。
- half、bfloat16_t和float类型按IEEE 754浮点减法规则计算，溢出以及nan、inf等特殊值的处理遵循浮点运算规则。
- 标量位于左侧时，按scalar减src中对应元素的顺序计算。

## 调用示例

```cpp
#include "tensor_api/experimental/vector_compute.h"

template <typename InputTensor, typename OutputTensor>
__simd_vf__ inline void sub_example(InputTensor input0, InputTensor input1, OutputTensor output)
{
    using asc::te::experimental::operator-;
    auto mask = asc::te::experimental::all_mask<float>();
    auto src0 = asc::te::experimental::load(input0, asc::te::make_coord(0)).with_mask(mask);
    auto src1 = asc::te::experimental::load(input1, asc::te::make_coord(0)).with_mask(mask);
    auto dst = src0 - src1;
    asc::te::experimental::store(output, asc::te::make_coord(0), dst);
}
```
