# make_stride

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

构造stride对象，用于描述各维度在内存中的步长。支持传入多个步长值，也支持传入嵌套的stride子结构以构造层次化stride。

## 函数原型

```cpp
template <typename FirstStride, typename... Strides>
__aicore__ inline constexpr stride<FirstStride, Strides...> make_stride(
    const FirstStride& first_stride, const Strides&... strides)
```

## 参数说明

**表1**  模板参数说明

| 参数名 | 输入/输出 | 描述 |
|--------|-----------|------|
| FirstStride | 输入 | 第一个stride元素的类型，可以是整型变量、`Std::Int`整型常量，或嵌套的stride子结构类型。 |
| Strides... | 输入 | 其余stride元素的类型。 |

**表2**  参数说明

| 参数名 | 输入/输出 | 描述 |
|--------|-----------|------|
| first_stride | 输入 | 第一个维度的步长或子结构。 |
| strides... | 输入 | 其余维度的步长或子结构。 |

## 返回值说明

返回`stride<FirstStride, Strides...>`对象。

## 约束说明

- 输入参数不可为空。
- 各参数必须为非负整数。
- 支持的数据类型包括：size_t、int等整数类型或者Std::Int类型。

## 调用示例

```cpp
#include "tensor_api/tensor.h"

using namespace asc::te;

// 构造普通stride
auto stride = make_stride(1, 100, 200);

// 构造层次化stride
auto fractal_stride = make_stride(make_stride(1, 16), make_stride(32, 512));

auto stride0 = Std::get<0>(stride);                    // stride0 = 1
auto inner_stride = Std::get<1>(Std::get<0>(fractal_stride)); // inner_stride = 16
```
