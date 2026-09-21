# not

## 产品支持情况

<!-- npu="950" id1 -->
- Ascend 950PR&950DT系列产品：支持
<!-- end id1 -->
<!-- npu="A3" id2 -->
- Atlas A3训练系列产品/Atlas A3推理系列产品：不支持
<!-- end id2 -->
<!-- npu="910b" id3 -->
- Atlas A2训练系列产品/Atlas A2推理系列产品：不支持
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

命名空间：`asc::te::experimental`。

对输入寄存器中参与计算的元素逐位取反。`reg_tensor<bool>`对布尔寄存器数据取反，其他类型对元素的二进制编码取反。

$$
dst_i = \mathord{\sim}src_i
$$

## 函数原型

```cpp
template <typename T>
__simd_callee__ inline reg_tensor<T> operator!(const reg_tensor<T>& src)
```

## 参数说明

| 参数名 | 输入/输出 | 描述 |
| --- | --- | --- |
| `T` | 模板参数 | 输入和返回值的元素类型，支持`bool`、`int8_t`、`uint8_t`、`int16_t`、`uint16_t`、`half`、`int32_t`、`uint32_t`、`float`。 |
| `src` | 输入 | 源操作数，类型为`reg_tensor<T>`。`src.mask`用于标记参与计算的元素，对应位置为`1`时参与计算，为`0`时不参与计算。 |

## 返回值说明

返回`reg_tensor<T>`。`reg`保存逐元素按位取反结果，`src.mask`对应位置为`0`时，`reg`的对应位置置零。返回值的`mask`与`src.mask`相同。

## 约束说明

本接口不是数值零值判断。对于非`bool`类型，结果是输入二进制编码逐位取反后的同类型数据。

## 调用示例

以下示例展示接口在AIV计算函数中的典型调用方式。

```cpp
#include "tensor_api/tensor.h"
#include "tensor_api/experimental/arch/vector/logical_compute.h"
#include "tensor_api/experimental/arch/vector/mask_reg_compute.h"
#include "tensor_api/experimental/arch/vector/reg_data_load.h"
#include "tensor_api/experimental/arch/vector/reg_data_store.h"

template <typename InputTensor, typename OutputTensor>
__simd_vf__ inline void not_example(InputTensor input, OutputTensor output)
{
    using asc::te::experimental::operator!;

    auto mask = asc::te::experimental::all_mask<int32_t>();
    auto src = asc::te::experimental::load(input, asc::te::make_coord(0)).with_mask(mask);
    auto result = !src;
    asc::te::experimental::store(output, asc::te::make_coord(0), result);
}
```
