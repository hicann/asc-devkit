# leaky_relu

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

该接口根据src携带的mask，使用标量斜率对源操作数按元素执行Leaky ReLU计算，并返回计算结果。计算公式如下：

$$
dst_i = \begin{cases}
src_i, & src_i > 0 \\
src_i \times slope, & src_i \le 0
\end{cases}
$$

## 函数原型

```cpp
template <typename T>
__simd_callee__ inline reg_tensor<T> leaky_relu(
    const reg_tensor<T>& src, const T& slope)
```

## 参数说明

**表1**  模板参数说明

| 参数名 | 描述 |
| --- | --- |
| T | reg_tensor元素类型。支持的数据类型请参考[数据类型](#数据类型)。 |

**表2**  参数说明

| 参数名 | 输入/输出 | 描述 |
| --- | --- | --- |
| src | 输入 | 源操作数，类型为reg_tensor&lt;T&gt;。其中，src.reg保存矢量数据，src.mask用于控制各元素是否参与计算。src.mask中与元素对应的比特位为1时，该元素参与计算；为0时，该元素不参与计算。 |
| slope | 输入 | src中对应元素小于或等于0时使用的标量斜率，数据类型与T相同。 |

## 数据类型

src与返回值的数据类型保持一致。支持的数据类型为：half、float。

## 返回值说明

返回Leaky ReLU计算结果，类型为reg_tensor&lt;T&gt;。返回值mask与src.mask相同；src.mask对应位置为0时，返回值的对应元素置零。

## 约束说明

src.mask需通过`with_mask`接口预先设置。未设置时，mask的内容不确定，会导致参与计算的元素位置错误。

## 调用示例

```cpp
#include "tensor_api/tensor.h"
#include "tensor_api/experimental/arch/vector/basic_arithmetic.h"
#include "tensor_api/experimental/arch/vector/mask_reg_compute.h"
#include "tensor_api/experimental/arch/vector/reg_data_load.h"
#include "tensor_api/experimental/arch/vector/reg_data_store.h"

template <typename InputTensor, typename OutputTensor>
__simd_vf__ inline void leaky_relu_example(InputTensor input, OutputTensor output)
{
    auto mask = asc::te::experimental::all_mask<float>();
    auto src = asc::te::experimental::load(input, asc::te::make_coord(0)).with_mask(mask);
    auto dst = asc::te::experimental::leaky_relu(src, 0.1f);
    asc::te::experimental::store(output, asc::te::make_coord(0), dst);
}
```
