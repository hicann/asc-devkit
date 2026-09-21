# and

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

头文件路径：`"tensor_api/experimental/arch/vector/logical_compute.h"`。

对两个寄存器逐元素执行按位与运算。

$$
dst_i = a_i \,\mathbin{\&}\, b_i
$$

## 函数原型

```cpp
template <typename T>
__simd_callee__ inline reg_tensor<T> operator&(
    const reg_tensor<T>& src0, const reg_tensor<T>& src1)
```

## 参数说明

**表1**  模板参数说明

| 参数名 | 描述 |
| --- | --- |
| T | 操作数和返回值的元素类型。支持的数据类型请参考[数据类型](#数据类型)。 |

**表2**  参数说明

| 参数名 | 输入/输出 | 描述 |
| --- | --- | --- |
| `src0` | 输入 | 第一源操作数，类型为`reg_tensor<T>`。`src0.mask`用于标记参与计算的元素，对应位置为`1`时参与计算，为`0`时不参与计算。 |
| `src1` | 输入 | 第二源操作数，类型为`reg_tensor<T>`。`src1.mask`不用于本次计算。 |

## 数据类型

src0、src1与返回值的数据类型保持一致。支持的数据类型为：`bool`、`int8_t`、`uint8_t`、`int16_t`、`uint16_t`、`int32_t`、`uint32_t`。

## 返回值说明

返回`reg_tensor<T>`。`reg`保存逐元素按位与结果，`src0.mask`对应位置为`0`时，`reg`的对应位置置零。返回值的`mask`与`src0.mask`相同。

## 约束说明

- src0.mask需通过`with_mask`接口预先设置。未设置时，mask的内容不确定，会导致参与计算的元素位置错误。
- 构造`reg_tensor<bool>{maskValue}`只初始化对象的mask，不会生成布尔寄存器数据。作为操作数前，reg必须已经由mask生成、加载或其他布尔计算接口写入。

## 调用示例

以下示例展示接口在AIV计算函数中的典型调用方式。示例先为加载结果绑定mask，
再执行计算并将结果写回UB。

```cpp
#include "tensor_api/tensor.h"
#include "tensor_api/experimental/arch/vector/logical_compute.h"
#include "tensor_api/experimental/arch/vector/mask_reg_compute.h"
#include "tensor_api/experimental/arch/vector/reg_data_load.h"
#include "tensor_api/experimental/arch/vector/reg_data_store.h"

template <typename InputTensor, typename OutputTensor>
__simd_vf__ inline void bit_and_example(
    InputTensor input0, InputTensor input1, OutputTensor output)
{
    using asc::te::experimental::operator&;

    auto mask = asc::te::experimental::all_mask<int32_t>();
    auto src0_reg = asc::te::experimental::load(input0, asc::te::make_coord(0)).with_mask(mask);
    auto src1_reg = asc::te::experimental::load(input1, asc::te::make_coord(0)).with_mask(mask);

    auto result = src0_reg & src1_reg;
    asc::te::experimental::store(output, asc::te::make_coord(0), result);
}
```
