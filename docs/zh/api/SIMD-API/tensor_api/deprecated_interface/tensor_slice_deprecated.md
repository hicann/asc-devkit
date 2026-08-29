# base_tensor::Slice（废弃）

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

**该接口已废弃，请使用[base_tensor::slice](../tensor/tensor.md#slice)替代。**

从指定坐标处切出带有新Layout信息的子Tensor。

## 函数原型

```cpp
template <typename Coord, typename Info>
__aicore__ inline constexpr decltype(auto) Slice(const Coord& coord, const Info& info);

template <typename Coord, typename Info>
__aicore__ inline constexpr decltype(auto) Slice(const Coord& coord, const Info& info) const;
```

## 参数说明

**表1**  参数说明

| 参数名 | 输入/输出 | 描述 |
| :--- | :---: | :--- |
| Coord | 输入 | 模板参数，表示切片起始坐标类型。 |
| Info | 输入 | 模板参数，表示切片描述类型，可以为Shape或Layout。 |
| coord | 输入 | 子Tensor在当前Tensor中的起始坐标。 |
| info | 输入 | 子Tensor的Shape或Layout描述。 |

## 返回值说明

返回切片后的子Tensor。新Tensor的Engine指向切片起始位置，Layout根据`coord`和`info`生成。

## 流水类型

无

## 约束说明

`coord`、`info`的结构及取值必须满足[slice](../tensor/slice.md)接口约束。

## 调用示例

```cpp
#include "tensor_api/tensor.h"

using namespace asc::te;

auto old_slice = tensor.Slice(coord, info);  // 废弃写法
auto new_slice = tensor.slice(coord, info);  // 替代写法
```
