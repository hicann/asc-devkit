# squeeze

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
<!-- npu="910" id4 -->
- Atlas 训练系列产品：不支持
<!-- end id4 -->
<!-- npu="310p" id5 -->
- Atlas 推理系列产品AI Core：不支持
<!-- end id5 -->
<!-- npu="310p" id6 -->
- Atlas 推理系列产品Vector Core：不支持
<!-- end id6 -->
<!-- npu="310b" id7 -->
- Atlas 200I/500 A2 推理产品：不支持
<!-- end id7 -->

## 功能说明

头文件路径为：`tensor_api/tensor.h`。

squeeze用于删除layout的shape中大小为1的指定维度，并同步删除stride中的对应维度。输入为Tensor时，接口更新Tensor的layout，Tensor的Engine、存储位置和数据起始地址保持不变。

接口支持以下两种方式指定待删除维度：

- 通过模板参数指定shape顶层维度的索引。
- 通过与shape结构相同的Pattern指定维度。Pattern中使用`_1`标记待删除维度，使用`_`标记保留维度，嵌套Pattern会递归处理嵌套shape。

返回的layout或Tensor保留输入layout携带的LayoutPattern和layout_trait信息。

## 函数原型

- 通过维度索引删除大小为1的维度。

    ```cpp
    template <size_t... SqueezeDims, typename Input>
    __aicore__ inline constexpr auto squeeze(const Input& x)
    ```

- 通过Pattern删除大小为1的维度。

    ```cpp
    template <typename Pattern, typename Input>
    __aicore__ inline constexpr auto squeeze(const Input& x, const Pattern& pattern)
    ```

## 参数说明

**表1**  模板参数说明

| 参数名 | 描述 |
| :--- | :--- |
| SqueezeDims | 待删除维度在shape顶层结构中的索引。支持同时指定多个索引，索引顺序不影响结果。 |
| Input | 输入对象类型，需要为layout类型或Tensor类型。 |
| Pattern | Pattern类型，需要为与输入layout的shape结构相同的元组类型。 |

**表2**  参数说明

| 参数名 | 输入/输出 | 描述 |
| :--- | :--- | :--- |
| x | 输入 | 待处理的layout或Tensor对象。 |
| pattern | 输入 | 维度选择Pattern。`_1`表示删除对应的大小为1的维度，`_`表示保留对应维度。Pattern可由`make_coord`构造。 |

## 返回值说明

输入为layout时，返回删除指定维度后的layout对象；输入为Tensor时，返回使用新layout描述的Tensor对象。返回对象保留原layout的LayoutPattern和layout_trait信息。

## 约束说明

- `SqueezeDims`至少需要包含一个维度索引，并指向输入shape的顶层维度。
- `pattern`需要与输入layout的shape具有相同的嵌套结构。
- 对于编译期常量维度，只有大小为1的维度会被删除；被指定但大小不为1的维度会被保留。
- 对于运行时维度，接口不会校验对应shape值是否为1。用户需要保证被指定删除的运行时维度大小为1，否则返回的layout或Tensor不能正确描述原始数据。
- 删除维度后，如果当前层只保留一个嵌套元组，接口会展开该层。例如，shape从`(1, (M, N))`转换为`(M, N)`。
- 删除维度只改变shape和stride的结构，不移动或修改内存中的数据。

## 调用示例

### 按维度索引删除

```cpp
#include "tensor_api/tensor.h"

using namespace asc::te;

auto layout = make_layout(make_shape(_1{}, _4{}, _1{}), make_stride(_4{}, _1{}, _1{}));
auto squeezed_layout = squeeze<0, 2>(layout);

// squeezed_layout的shape为(4)，stride为(1)。
```

### 按Pattern删除

```cpp
#include "tensor_api/tensor.h"

using namespace asc::te;

auto layout = make_layout(make_shape(_1{}, _4{}, _1{}), make_stride(_4{}, _1{}, _1{}));
auto pattern = make_coord(_1{}, _, _1{});
auto squeezed_layout = squeeze(layout, pattern);

// squeezed_layout的shape为(4)，stride为(1)。
```

### 删除Tensor的Batch维度

```cpp
#include "tensor_api/tensor.h"

using namespace asc::te;

__aicore__ inline void squeeze_tensor_example()
{
    constexpr uint32_t batch = 1;
    constexpr uint32_t m = 32;
    constexpr uint32_t n = 64;

    __cbuf__ half l1_buffer[batch * m * n];
    auto batch_layout = make_frame_layout<nz_layout_ptn, half>(batch, m, n);
    auto batch_tensor = make_tensor(make_mem_ptr(l1_buffer), batch_layout);

    auto matrix_tensor = squeeze<0>(batch_tensor);
    // matrix_tensor与batch_tensor指向相同的数据，layout中不再包含Batch维度。
}
```
