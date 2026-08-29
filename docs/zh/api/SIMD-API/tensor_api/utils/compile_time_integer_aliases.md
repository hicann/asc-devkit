# 编译期整型别名

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

Tensor API提供一组[Std::Int](../../../Utils-API/cpp_stdlib/type_traits/integral_constant.md)的类型别名，用于在Layout、Shape、Stride和Coord中表达编译期整数。

## 原型定义

```cpp
using _0 = Std::Int<0>;
using _1 = Std::Int<1>;
using _2 = Std::Int<2>;
using _3 = Std::Int<3>;
using _4 = Std::Int<4>;
using _5 = Std::Int<5>;
using _6 = Std::Int<6>;
using _7 = Std::Int<7>;
using _8 = Std::Int<8>;
using _9 = Std::Int<9>;
using _10 = Std::Int<10>;
using _16 = Std::Int<16>;
using _24 = Std::Int<24>;
using _32 = Std::Int<32>;
using _64 = Std::Int<64>;
using _128 = Std::Int<128>;
using _256 = Std::Int<256>;
using _512 = Std::Int<512>;
using _1024 = Std::Int<1024>;
using _2048 = Std::Int<2048>;
using _4096 = Std::Int<4096>;
```

## 类型说明

**表1**  类型说明

| 类型 | 描述 |
| :--- | :--- |
| 原型中列出的`_N`类型 | 对指定整数值的`Std::Int`类型别名。可用类型及其对应整数值以[原型定义](#原型定义)中的列表为准。 |

编译期整型别名是数据类型，使用时需要通过`_N{}`的形式构造对象。

## 调用示例

```cpp
#include "tensor_api/tensor.h"

using namespace asc::te;

// 使用编译期整数构造shape和stride。
auto tensor_layout = make_layout(make_shape(_8{}, _16{}), make_stride(_16{}, _1{}));
```
