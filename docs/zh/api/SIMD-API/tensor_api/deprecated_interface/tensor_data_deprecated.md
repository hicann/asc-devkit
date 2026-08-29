# base_tensor::Data（废弃）

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

**该接口已废弃，请使用[base_tensor::data](../tensor/tensor.md#data)替代。**

获取Tensor底层数据的起始迭代器。

## 函数原型

```cpp
__aicore__ inline constexpr decltype(auto) Data() const;
__aicore__ inline constexpr decltype(auto) Data();
```

## 参数说明

无

## 返回值说明

返回`engine().begin()`的结果；const重载返回对应的常量访问结果。

## 流水类型

无

## 约束说明

Tensor绑定的Engine必须有效。

## 调用示例

```cpp
#include "tensor_api/tensor.h"

using namespace asc::te;

auto old_data = tensor.Data();               // 废弃写法
auto new_data = tensor.data();               // 替代写法
```
