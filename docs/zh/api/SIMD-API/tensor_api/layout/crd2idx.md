# crd2idx

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

crd2idx根据coord、shape和stride计算坐标对应的一维线性索引。输入为嵌套Tuple时，接口会按照相同的层次结构递归计算各维度的偏移并求和。

## 函数原型

```cpp
template <typename Coord, typename Shape, typename Stride>
__aicore__ inline constexpr auto crd2idx(
    const Coord& coord, const Shape& shape, const Stride& stride)
```

## 参数说明

**表1**  模板参数说明

| 参数名 | 输入/输出 | 描述 |
| :--- | :---: | :--- |
| Coord | 输入 | 坐标类型，可以是整数、编译期整型常量或嵌套Tuple。 |
| Shape | 输入 | shape类型，可以是整数、编译期整型常量或嵌套Tuple。 |
| Stride | 输入 | stride类型，其结构需要与shape匹配。 |

**表2**  参数说明

| 参数名 | 输入/输出 | 描述 |
| :--- | :---: | :--- |
| coord | 输入 | 待转换的坐标。 |
| shape | 输入 | 描述各维度大小的shape。 |
| stride | 输入 | 描述各维度步长的stride。 |

## 返回值说明

返回coord对应的一维线性索引，返回类型由coord和stride的计算结果推导。

## 约束说明

- coord、shape和stride为Tuple时，顶层元素数量需要一致，嵌套结构需要能够逐层对应。
- coord中的各维坐标需要位于shape描述的有效范围内。
- stride的单位与目标地址计算所使用的单位保持一致。

## 调用示例

```cpp
#include "tensor_api/tensor.h"

using namespace asc::te;

auto tensor_shape = make_shape(8, 16);
auto tensor_stride = make_stride(16, 1);
auto tensor_coord = make_coord(2, 3);

auto index = crd2idx(tensor_coord, tensor_shape, tensor_stride);
// index = 2 * 16 + 3 = 35
```
