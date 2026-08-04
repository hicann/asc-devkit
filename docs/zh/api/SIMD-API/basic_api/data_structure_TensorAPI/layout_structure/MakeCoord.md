# MakeCoord

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
<!-- npu="310b" id4 -->
- Atlas 200I/500 A2 推理产品：不支持
<!-- end id4 -->
<!-- npu="310p" id5 -->
- Atlas 推理系列产品AI Core：不支持
<!-- end id5 -->
<!-- npu="310p" id6 -->
- Atlas 推理系列产品Vector Core：不支持
<!-- end id6 -->
<!-- npu="910" id7 -->
- Atlas 训练系列产品：不支持
<!-- end id7 -->

## 功能说明

需要包含的头文件为：#include "tensor_api/tensor.h"。

构造Coord对象，用于定义张量的坐标。

## 函数原型

```cpp
template <typename... Ts>
__aicore__ inline constexpr Coord<Ts...> MakeCoord(const Ts&... t)
```

## 参数说明

**表1** 模板参数类型说明

| 参数名 | 输入/输出 | 描述 |
|--------|-----------|------|
| Ts... | 输入 | 可变模板参数，表示各维坐标的数据类型。支持size_t、int等整数类型，以及Std::Int类型。 |

**表2** 参数说明

| 参数名 | 输入/输出 | 描述 |
|--------|-----------|------|
| t | 输入 | 各维度的坐标，可变参数。 |

## 返回值说明

返回Coord<Ts...>对象。

## 约束说明

- 各维度的坐标值需为非负整数。
- 参数数量必须与对应的Shape维度数量一致。

## 调用示例

```cpp
// 使用整数类型创建一个3维张量的坐标
auto coord = AscendC::Te::MakeCoord(5, 10, 15);

// 获取各维度的坐标
auto coord0 = AscendC::Std::get<0>(coord); // coord0 = 5
auto coord1 = AscendC::Std::get<1>(coord); // coord1 = 10
auto coord2 = AscendC::Std::get<2>(coord); // coord2 = 15

// 使用Std::Int类型创建一个3维张量的坐标
auto coordInt = AscendC::Te::MakeCoord(
    AscendC::Std::Int<5>{},
    AscendC::Std::Int<10>{},
    AscendC::Std::Int<15>{});

// 获取各维度的坐标
auto coordInt0 = AscendC::Std::get<0>(coordInt); // coordInt0 = 5
auto coordInt1 = AscendC::Std::get<1>(coordInt); // coordInt1 = 10
auto coordInt2 = AscendC::Std::get<2>(coordInt); // coordInt2 = 15
```
