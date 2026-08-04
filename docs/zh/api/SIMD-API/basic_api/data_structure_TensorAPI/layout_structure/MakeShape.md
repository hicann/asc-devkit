# MakeShape

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

构造Shape对象，用于描述数据的逻辑形状。支持传入多个维度值，也支持传入嵌套的Shape子结构以构造层次化Shape。

## 函数原型

```cpp
template <typename... Ts>
__aicore__ inline constexpr Shape<Ts...> MakeShape(const Ts&... t)
```

## 参数说明

**表1** 模板参数说明

| 参数名 | 输入/输出 | 描述 |
|--------|-----------|------|
| Ts... | 输入 | Shape各元素的类型，可以是整型变量、`Std::Int`整型常量，或嵌套的Shape子结构类型。 |

**表2** 参数说明

| 参数名 | 输入/输出 | 描述 |
|--------|-----------|------|
| t | 输入 | Shape的各维度取值或子结构，可变参数。 |

## 返回值说明

返回Shape<Ts...>对象。

## 约束说明

- 输入参数不可为空
- 输入的参数为正整数。
- 支持的数据类型包括：size_t、int等整数类型或者Std::Int类型。

## 调用示例

```cpp
using namespace AscendC::Te;

// 构造普通Shape
auto shape = MakeShape(10, 20, 30);

// 构造层次化Shape
auto fractalShape = MakeShape(MakeShape(16, 8), MakeShape(32, 4));

auto dim0 = Std::get<0>(shape);                // dim0 = 10
auto innerRow = Std::get<0>(Std::get<0>(fractalShape)); // innerRow = 16
```
