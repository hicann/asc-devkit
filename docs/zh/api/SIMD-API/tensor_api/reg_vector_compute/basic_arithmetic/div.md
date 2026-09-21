# operator/

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

该接口根据src0携带的mask，对两个源操作数按元素相除，并返回计算结果。计算公式如下：

$$
dst_i = \frac{src0_i}{src1_i}
$$

## 函数原型

```cpp
template <typename T>
__simd_callee__ inline reg_tensor<T> operator/(
    const reg_tensor<T>& src0, const reg_tensor<T>& src1)
```

## 参数说明

**表1**  模板参数说明

| 参数名 | 描述 |
| --- | --- |
| T | 操作数数据类型。支持的数据类型请参考[数据类型](#数据类型)。 |

**表2**  参数说明

| 参数名 | 输入/输出 | 描述 |
| --- | --- | --- |
| src0 | 输入 | 被除数，类型为reg_tensor&lt;T&gt;。src0.reg保存矢量数据，src0.mask控制各元素是否参与计算。 |
| src1 | 输入 | 除数，类型为reg_tensor&lt;T&gt;。src1.mask不参与本次计算。 |

## 数据类型

源操作数与返回值的数据类型保持一致。支持的数据类型为：int16_t、uint16_t、half、int32_t、uint32_t、float。

## 返回值说明

返回按元素相除的结果，类型为reg_tensor&lt;T&gt;。返回值mask与src0.mask相同；src0.mask对应位置为0时，返回值的对应元素置零。

## 约束说明

- src0.mask需通过`with_mask`接口预先设置。未设置时，mask的内容不确定，会导致参与计算的元素位置错误。
- 使用整数类型时，除数不能为0。
- 注意除零错误，结果请参考[Div特殊值/边界值输入的计算结果说明](../../../../appendix/reg_vector_compute_interface_boundary_value_summary.md#table1325925915217)。
- `half`、`float`数据类型的计算结果与标准`IEEE 754`二进制浮点除法相比可能存在1 ULP精度差异。

## 调用示例

```cpp
#include "tensor_api/tensor.h"
#include "tensor_api/experimental/arch/vector/basic_arithmetic.h"
#include "tensor_api/experimental/arch/vector/mask_reg_compute.h"
#include "tensor_api/experimental/arch/vector/reg_data_load.h"
#include "tensor_api/experimental/arch/vector/reg_data_store.h"

template <typename InputTensor, typename OutputTensor>
__simd_vf__ inline void div_example(InputTensor input0, InputTensor input1, OutputTensor output)
{
    using asc::te::experimental::operator/;
    auto mask = asc::te::experimental::all_mask<float>();
    auto src0 = asc::te::experimental::load(input0, asc::te::make_coord(0)).with_mask(mask);
    auto src1 = asc::te::experimental::load(input1, asc::te::make_coord(0));
    auto dst = src0 / src1;
    asc::te::experimental::store(output, asc::te::make_coord(0), dst);
}
```
