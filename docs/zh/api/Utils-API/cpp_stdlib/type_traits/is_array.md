# is\_array

## 产品支持情况

- Ascend 950PR/Ascend 950DT：支持
- Atlas A3 训练系列产品/Atlas A3 推理系列产品：支持
- Atlas A2 训练系列产品/Atlas A2 推理系列产品：支持
- Atlas 200I/500 A2 推理产品：不支持
- Atlas 推理系列产品AI Core：不支持
- Atlas 推理系列产品Vector Core：不支持
- Atlas 训练系列产品：不支持

## 功能说明

在程序编译时，检测一个类型是否为数组类型，可以用于在编译时进行类型检查和条件处理。

## 函数原型

```
template <typename Tp>
struct is_array;
```

## 参数说明

**表1**  模板参数说明

| 参数名 | 含义 |
| --- | --- |
| Tp | 需要检测的类型，包括基本类型（如int、float等）、复合类型（如指针、引用）、用户自定义类型（如类、结构体等），以及数组类型本身。 |

## 约束说明

无

## 返回值说明

is\_array的静态常量成员value用于获取返回的布尔值，is\_array<Tp\>::value取值如下：

-   true：Tp是数组类型。
-   false：Tp不是数组类型。

## 调用示例

```
// 自定义测试类型
struct MyStruct{};
using FuncType = int(int);

// Legitimate array types
AscendC::printf("AscendC::Std::is_array::value:%d\n", AscendC::Std::is_array<int[5]>::value);
AscendC::printf("AscendC::Std::is_array::value:%d\n", AscendC::Std::is_array<char[]>::value);
AscendC::printf("AscendC::Std::is_array::value:%d\n", AscendC::Std::is_array<double[2][3]>::value);

// Array types limited by CV
AscendC::printf("AscendC::Std::is_array::value:%d\n", AscendC::Std::is_array<const int[10]>::value);
AscendC::printf("AscendC::Std::is_array::value:%d\n", AscendC::Std::is_array<volatile char[3]>::value);

// non-array types
AscendC::printf("AscendC::Std::is_array::value:%d\n", AscendC::Std::is_array<int*>::value);
AscendC::printf("AscendC::Std::is_array::value:%d\n", AscendC::Std::is_array<int>::value);
AscendC::printf("AscendC::Std::is_array::value:%d\n", AscendC::Std::is_array<double>::value);
AscendC::printf("AscendC::Std::is_array::value:%d\n", AscendC::Std::is_array<MyStruct>::value);
AscendC::printf("AscendC::Std::is_array::value:%d\n", AscendC::Std::is_array<FuncType>::value);
AscendC::printf("AscendC::Std::is_array::value:%d\n", AscendC::Std::is_array<void>::value);
```

```
// 执行结果：
AscendC::Std::is_array::value:1
AscendC::Std::is_array::value:1
AscendC::Std::is_array::value:1
AscendC::Std::is_array::value:1
AscendC::Std::is_array::value:1
AscendC::Std::is_array::value:0
AscendC::Std::is_array::value:0
AscendC::Std::is_array::value:0
AscendC::Std::is_array::value:0
AscendC::Std::is_array::value:0
AscendC::Std::is_array::value:0
```
