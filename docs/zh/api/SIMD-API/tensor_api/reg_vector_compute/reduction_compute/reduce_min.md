# reduce_min

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

头文件路径：`"tensor_api/experimental/vector_compute.h"`。

该接口根据输入[reg_tensor](../reg_tensor/reg_tensor.md)携带的mask，对源操作数执行归约最小值操作，并返回计算结果。通过模板参数`scope`选择归约范围，支持对所有有效元素求最小值，以及对每个`DataBlock`（32B）内的有效元素求最小值。

`scope`为`reduce_scope::element`时，计算公式如下：

$$
\begin{aligned}
dst_0 &= \min\{src_i \mid mask_i = 1\} \\
dst_1 &= \operatorname{argmin}\{src_i \mid mask_i = 1\}
\end{aligned}
$$

`scope`为`reduce_scope::datablock`时，计算公式如下：

$$
dst_k = \min\{src_i \mid kB \le i < (k + 1)B,\ mask_i = 1\}
$$

其中，$k$表示`DataBlock`的索引，$B$为一个`DataBlock`内的元素个数。

## 函数原型

```cpp
template <reduce_scope scope = reduce_scope::element, typename DataType>
__simd_callee__ inline reg_tensor<DataType> reduce_min(const reg_tensor<DataType>& src)
```

## 参数说明

**表1**  模板参数说明

| 参数名 | 描述 |
| --- | --- |
| scope | 归约范围，类型为`reduce_scope`，默认值为`reduce_scope::element`。取值为`reduce_scope::element`时，对所有有效元素求最小值；取值为`reduce_scope::datablock`时，对每个`DataBlock`内的有效元素求最小值。 |
| DataType | 源操作数和返回值中数据寄存器的元素类型，由函数实参自动推导。支持的数据类型请参考[数据类型](#数据类型)。 |

**表2**  参数说明

| 参数名 | 输入/输出 | 描述 |
| --- | --- | --- |
| src | 输入 | 源操作数，类型为[reg_tensor](../reg_tensor/reg_tensor.md)&lt;DataType&gt;。其中，`src.reg`保存矢量数据，`src.mask`用于控制各元素是否参与归约。`src.mask`中与元素对应的比特位为1时，该元素参与归约；为0时，该元素不参与归约。 |

## 数据类型

`DataType`支持的数据类型为：`int16_t`、`uint16_t`、`half`、`int32_t`、`uint32_t`、`float`。

## 返回值说明

返回归约最小值结果，类型为`reg_tensor<DataType>`，返回值的`mask`与`src.mask`保持一致。

- `scope`为`reduce_scope::element`时，最小值保存在`reg`的第0个元素，最小值在`src`中的索引原始位模式保存在`reg`的第1个元素，其他元素置0。如果存在多个最小值，则保留最小的索引。
- `scope`为`reduce_scope::datablock`时，每个`DataBlock`内的最小值连续保存在`reg`的前8个元素，其他元素置0；该模式仅输出最小值，不输出索引。

## 约束说明

- `src.mask`需通过`with_mask`接口预先设置。未设置时，mask的内容不确定，会导致参与归约的元素位置错误。
- 未被`src.mask`选中的元素被视为对应数据类型的最大值，浮点数类型的最大值为`+inf`。
- `scope`为`reduce_scope::element`时，若`src.mask`全为0，则将对应数据类型的最大值写入返回值`reg`的第0个元素，并将其余元素置0。
- `scope`为`reduce_scope::datablock`时，若某个`DataBlock`内的元素均不参与计算，则将对应数据类型的最大值写入返回值`reg`的对应位置。
- 比较时遵循$min(-0, +0) = -0$。
- `scope`为`reduce_scope::element`时，如果输入数据中存在nan，则将nan写入返回值`reg`的第0个元素，并将第一个nan的索引写入返回值`reg`的第1个元素。
- 返回值的`mask`与`src.mask`保持一致。如果只需搬出有效归约结果，调用`store`前需要根据结果布局重新调用`with_mask`绑定新的mask。
- `scope`为`reduce_scope::element`时，接口同时返回最小值和索引。索引按照`DataType`的位模式存储在返回值`reg`的第1个元素中，读取索引时需要按`DataType`对应的位宽转换为整数类型。例如，`DataType`为`half`时，可按`uint16_t`读取索引原始位模式；`DataType`为`float`时，可按`uint32_t`读取索引原始位模式。

## 调用示例

```cpp
#include "tensor_api/experimental/vector_compute.h"

template <typename InputTensor, typename OutputTensor>
__simd_vf__ inline void reduce_min_example(InputTensor input, OutputTensor output)
{
    auto mask = asc::te::experimental::all_mask<float>();
    auto src = asc::te::experimental::load(input, asc::te::make_coord(0)).with_mask(mask);

    auto elemMin = asc::te::experimental::reduce_min(src);
    elemMin.with_mask(asc::te::experimental::update_mask<float>(2));
    asc::te::experimental::store(output, asc::te::make_coord(0), elemMin);

    auto blockMin = asc::te::experimental::reduce_min<asc::te::experimental::reduce_scope::datablock>(src);
    blockMin.with_mask(asc::te::experimental::update_mask<float>(8));
    asc::te::experimental::store(output, asc::te::make_coord(8), blockMin);
}
```
