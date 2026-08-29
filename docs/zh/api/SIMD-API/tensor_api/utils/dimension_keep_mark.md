# 维度保留标记

## 产品支持情况

<!-- npu="950" id1 -->
- Ascend 950PR/Ascend 950DT：支持
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

头文件路径为：`tensor_api/tensor.h`。

Tensor API提供`keep_mark`类型和常量`_`，当前仅用于在`squeeze`接口中标记需要保留的维度。

## 原型定义

```cpp
struct keep_mark {
    __aicore__ inline constexpr keep_mark() = default;
};

inline constexpr keep_mark _{};
```

## 类型说明

**表1**  类型说明

| 类型或常量 | 描述 |
| :--- | :--- |
| `keep_mark` | 维度保留标记类型。 |
| `_` | `keep_mark`类型的全局编译期常量，用于表示保留对应维度。 |

## 调用示例

```cpp
#include "tensor_api/tensor.h"

using namespace asc::te;

// 删除大小为1的第二个维度，保留第一个和第三个维度。
auto source_layout = make_layout(make_shape(4, _1{}, 8));
auto squeezed_layout = squeeze(source_layout, make_coord(_, _1{}, _));
```
