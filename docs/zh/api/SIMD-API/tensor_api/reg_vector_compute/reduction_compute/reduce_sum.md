# reduce_sum

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

该接口根据输入[reg_tensor](../reg_tensor/reg_tensor.md)携带的mask，对源操作数执行归约求和操作，并返回计算结果。通过模板参数`scope`选择归约范围，支持对所有有效元素求和、对每个`DataBlock`（32B）内的有效元素求和，以及对相邻两个有效元素求和。

`scope`为`reduce_scope::element`时，计算公式如下：

$$
dst_0 = \sum_{mask_i = 1} src_i
$$

`scope`为`reduce_scope::datablock`时，计算公式如下：

$$
dst_k = \sum_{kB \le i < (k + 1)B,\ mask_i = 1} src_i
$$

其中，$k$表示`DataBlock`的索引，$B$为一个`DataBlock`内的元素个数。

`scope`为`reduce_scope::pair`时，计算公式如下：

$$
dst_i = (mask_{2i} ? src_{2i} : 0) + (mask_{2i + 1} ? src_{2i + 1} : 0)
$$

## 函数原型

```cpp
template <reduce_scope scope = reduce_scope::element, typename DstType, typename SrcType>
__simd_callee__ inline reg_tensor<DstType> reduce_sum(const reg_tensor<SrcType>& src)
```

## 参数说明

**表1**  模板参数说明

| 参数名 | 描述 |
| --- | --- |
| scope | 归约范围，类型为`reduce_scope`，默认值为`reduce_scope::element`。取值为`reduce_scope::element`时，对所有有效元素求和；取值为`reduce_scope::datablock`时，对每个`DataBlock`内的有效元素求和；取值为`reduce_scope::pair`时，对相邻两个元素求和。 |
| DstType | 返回值中数据寄存器的元素类型，需要在`scope`之后显式指定。支持的数据类型请参考[数据类型](#数据类型)。 |
| SrcType | 源操作数的数据类型，由函数实参自动推导。支持的数据类型请参考[数据类型](#数据类型)。 |

**表2**  参数说明

| 参数名 | 输入/输出 | 描述 |
| --- | --- | --- |
| src | 输入 | 源操作数，类型为[reg_tensor](../reg_tensor/reg_tensor.md)&lt;SrcType&gt;。其中，`src.reg`保存矢量数据，`src.mask`用于控制各元素是否参与归约。`src.mask`中与元素对应的比特位为1时，该元素参与归约；为0时，该元素不参与归约。 |

## 数据类型

`scope`为`reduce_scope::element`或`reduce_scope::datablock`时，`SrcType`与`DstType`支持以下组合：

| SrcType | DstType |
| --- | --- |
| `int16_t` | `int32_t` |
| `uint16_t` | `uint32_t` |
| `half` | `half` |
| `int32_t` | `int32_t` |
| `uint32_t` | `uint32_t` |
| `float` | `float` |

`scope`为`reduce_scope::pair`时，`SrcType`与`DstType`必须保持一致，支持的数据类型为：`half`、`float`。

## 返回值说明

返回归约求和结果，类型为`reg_tensor<DstType>`，返回值的`mask`与`src.mask`保持一致。

- `scope`为`reduce_scope::element`时，求和结果保存在`reg`的第0个元素，其他元素置0。
- `scope`为`reduce_scope::datablock`时，每个`DataBlock`内的求和结果连续保存在`reg`的前8个元素，其他元素置0。
- `scope`为`reduce_scope::pair`时，相邻两个元素的求和结果连续保存在`reg`的低半部分，高半部分置0。

## 约束说明

- `src.mask`需通过`with_mask`接口预先设置。未设置时，mask的内容不确定，会导致参与归约的元素位置错误。
- `scope`为`reduce_scope::element`时，若`src.mask`全为0，则将0写入返回值`reg`的第0个元素；对于浮点数类型，写入+0。
- `scope`为`reduce_scope::datablock`时，若某个`DataBlock`内的元素均不参与计算，则将0写入返回值`reg`的对应位置；对于浮点数类型，写入+0。
- `scope`为`reduce_scope::pair`时，未被`src.mask`选中的元素被视为0参与相邻元素求和。
- 返回值的`mask`与`src.mask`保持一致。如果只需搬出有效归约结果，调用`store`前需要根据结果布局重新调用`with_mask`绑定新的mask。
- `reduce_scope::element`和`reduce_scope::datablock`采用二叉树累加方式逐层归约，浮点数求和结果受累加顺序影响。`reduce_scope::pair`仅对相邻两个元素进行一次求和，不继续归约低半部分结果。

## 调用示例

```cpp
#include "tensor_api/experimental/vector_compute.h"

template <typename InputTensor, typename OutputTensor>
__simd_vf__ inline void reduce_sum_example(InputTensor input, OutputTensor output)
{
    auto mask = asc::te::experimental::all_mask<float>();
    auto src = asc::te::experimental::load(input, asc::te::make_coord(0)).with_mask(mask);

    auto elemSum = asc::te::experimental::reduce_sum<asc::te::experimental::reduce_scope::element, float>(src);
    elemSum.with_mask(asc::te::experimental::update_mask<float>(1));
    asc::te::experimental::store(output, asc::te::make_coord(0), elemSum);

    auto blockSum = asc::te::experimental::reduce_sum<asc::te::experimental::reduce_scope::datablock, float>(src);
    blockSum.with_mask(asc::te::experimental::update_mask<float>(8));
    asc::te::experimental::store(output, asc::te::make_coord(8), blockSum);

    auto pairSum = asc::te::experimental::reduce_sum<asc::te::experimental::reduce_scope::pair, float>(src);
    pairSum.with_mask(asc::te::experimental::update_mask<float>(32));
    asc::te::experimental::store(output, asc::te::make_coord(16), pairSum);
}
```
