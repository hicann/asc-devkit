# or

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

该接口根据src0.mask，对源操作数src0、src1进行按位或（|）操作，并返回计算结果。T不为bool时，对两个矢量数据寄存器按元素计算；T为bool时，对两个掩码寄存器的有效bit计算。计算公式如下：

$$
dst_i = src0_i \mathbin{|} src1_i
$$

## 函数原型

```cpp
template <typename T>
__simd_callee__ inline reg_tensor<T> operator|(
    const reg_tensor<T>& src0, const reg_tensor<T>& src1)
```

## 参数说明

**表 1**  模板参数说明

| 参数名 | 描述 |
| --- | --- |
| T | 操作数数据类型。支持的数据类型请参考[数据类型](#数据类型)。 |

**表 2**  参数说明

| 参数名 | 输入/输出 | 描述 |
| --- | --- | --- |
| src0 | 输入 | 源操作数，类型为reg_tensor&lt;T&gt;。T不为bool时，src0.reg保存矢量数据；T为bool时，src0.reg保存掩码寄存器数据。src0.mask用于控制各元素或bit是否参与计算，对应位置为1时参与计算，为0时不参与计算。 |
| src1 | 输入 | 源操作数，类型为reg_tensor&lt;T&gt;。T不为bool时，src1.reg保存矢量数据；T为bool时，src1.reg保存掩码寄存器数据。src1.mask不参与本次计算。 |

## 数据类型

源操作数与返回值的数据类型保持一致。支持的数据类型为：bool、int8_t、uint8_t、int16_t、uint16_t、int32_t、uint32_t。

## 返回值说明

返回逐元素按位或结果，类型为reg_tensor&lt;T&gt;。返回值的mask与src0.mask相同；src0.mask对应位置为0时，返回值的对应元素置零。

## 约束说明

- `src0.mask`需通过`with_mask`接口预先设置。未设置时，mask的内容不确定，会导致参与计算的元素位置错误。
- 参与计算的数据量由矢量长度VL决定。T不为bool时，元素个数为VL除以sizeof(T)；T为bool时，参与计算的bit数为VL。

## 调用示例

```cpp
#include "tensor_api/experimental/vector_compute.h"

template <typename InputTensor, typename OutputTensor>
__simd_vf__ inline void or_example(InputTensor input0, InputTensor input1, OutputTensor output)
{
    using asc::te::experimental::operator|;
    auto mask = asc::te::experimental::all_mask<int32_t>();
    auto src0 = asc::te::experimental::load(input0, asc::te::make_coord(0)).with_mask(mask);
    auto src1 = asc::te::experimental::load(input1, asc::te::make_coord(0)).with_mask(mask);
    auto dst = src0 | src1;
    asc::te::experimental::store(output, asc::te::make_coord(0), dst);
}
```
