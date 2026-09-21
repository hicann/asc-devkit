# shift_left

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

按照寄存器或标量给出的移位量，对`src`逐元素左移。

$$
dst_i = src_i \ll shift_i
$$

## 函数原型

```cpp
template <typename T, typename U>
__simd_callee__ inline reg_tensor<T> operator<<(
    const reg_tensor<T>& src, const U& shift)
```

## 参数说明

**表1**  模板参数说明

| 参数名 | 描述 |
| --- | --- |
| T | src和返回值的元素类型。支持的数据类型请参考[数据类型](#数据类型)。 |
| U | 移位量类型，可以是与T位宽匹配的有符号reg_tensor，或int16_t标量。 |

**表2**  参数说明

| 参数名 | 输入/输出 | 描述 |
| --- | --- | --- |
| `src` | 输入 | 待左移的源操作数。`src.mask`用于标记参与计算的元素，对应位置为`1`时参与计算，为`0`时不参与计算。 |
| `shift` | 输入 | 逐元素移位寄存器或`int16_t`标量。寄存器形式的`shift.mask`不用于本次计算。 |

## 数据类型

| 移位形式 | `src`元素类型 | `shift`类型 |
| --- | --- | --- |
| 寄存器移位 | `int8_t`、`uint8_t` | `reg_tensor<int8_t>` |
| 寄存器移位 | `int16_t`、`uint16_t` | `reg_tensor<int16_t>` |
| 寄存器移位 | `int32_t`、`uint32_t` | `reg_tensor<int32_t>` |
| 标量移位 | `int8_t`、`uint8_t`、`int16_t`、`uint16_t`、`int32_t`、`uint32_t` | `int16_t` |

## 返回值说明

返回`reg_tensor<T>`。`reg`保存逐元素左移结果，`src.mask`对应位置为`0`时，`reg`的对应位置置零。返回值的`mask`与`src.mask`相同。

## 约束说明

- src.mask需通过`with_mask`接口预先设置。未设置时，mask的内容不确定，会导致参与计算的元素位置错误。
- 移位量为负数时行为未定义。移位量大于`src`元素类型位宽时，结果为`0`。
- 左移结果超出`T`位宽的高位被丢弃，只保留低位结果。

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
__simd_vf__ inline void shift_left_example(InputTensor input, OutputTensor output)
{
    using asc::te::experimental::operator<<;

    auto src = asc::te::experimental::load(input, asc::te::make_coord(0)).with_mask(asc::te::experimental::all_mask<int32_t>());

    auto result = src << static_cast<int16_t>(2);
    asc::te::experimental::store(output, asc::te::make_coord(0), result);
}
```
