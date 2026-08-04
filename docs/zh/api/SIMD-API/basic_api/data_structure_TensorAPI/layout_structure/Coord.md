# Coord

> [!NOTE]说明
>本接口为试验接口，在后续版本中可能会调整或改进，不保证后续兼容性。请开发者在使用过程中关注后续版本更新。

Coord用于定义张量的坐标，用于访问张量中特定位置的元素。

## 需要包含的头文件

```cpp
#include "tensor_api/tensor.h"
```

## 原型定义

```cpp
template <typename... Coords>
using Coord = Std::tuple<Coords...>;
```

## 参数说明

| 参数名 | 输入/输出 | 描述 |
|--------|----------|------|
| Coords... | 输入 | 可变参数模板，表示各维度的坐标。|

## API映射关系

Coord通过[MakeCoord](MakeCoord.md)函数创建，可通过[Crd2Idx](../../aux_data_structures/Coordinate/Crd2Idx.md)函数进行多维坐标到线性坐标的转换，可作为参数传入到Layout的[operator()](Layout.md)成员函数，Tensor的[Slice](../tensor_structure/Tensor.md)成员函数以及[Slice](../tensor_structure/Slice.md)接口中进行张量的偏移以及切片操作。
