# layout::Rank（废弃）

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

**该接口已废弃，请使用[layout::rank](../layout/layout.md#rank)替代。**

返回当前Layout或指定子维度的秩。

## 函数原型

```cpp
template <size_t... I>
__aicore__ inline constexpr decltype(auto) Rank() const;
```

## 参数说明

**表1**  模板参数说明

| 参数名 | 输入/输出 | 描述 |
| :--- | :---: | :--- |
| I... | 输入 | 模板参数，表示需要访问的子维度索引序列。不指定时处理完整Layout。 |

## 返回值说明

返回当前Layout或指定子维度Shape的顶层维度数量。

## 流水类型

无

## 约束说明

`I...`必须是Layout中的有效索引。

## 调用示例

```cpp
#include "tensor_api/tensor.h"

using namespace asc::te;

auto old_rank = layout.Rank();              // 废弃写法
auto new_rank = layout.rank();              // 替代写法
```
