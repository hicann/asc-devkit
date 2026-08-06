# is\_floating\_point

## 产品支持情况

- Ascend 950PR/Ascend 950DT：支持
- Atlas A3 训练系列产品/Atlas A3 推理系列产品：支持
- Atlas A2 训练系列产品/Atlas A2 推理系列产品：支持
- Atlas 200I/500 A2 推理产品：不支持
- Atlas 推理系列产品AI Core：不支持
- Atlas 推理系列产品Vector Core：不支持
- Atlas 训练系列产品：不支持

## 功能说明

在程序编译时，检测一个类型是否为浮点类型，可以用于在编译时进行类型检查和条件处理。

## 函数原型

```
template <typename T>
struct is_floating_point;
```

## 参数说明

**表1**  模板参数说明

| 参数名 | 含义 |
| --- | --- |
| T | 需要检测的类型，包括基本数据类型、修饰类型等。 |

## 约束说明

无

## 返回值说明

is\_floating\_point的静态常量成员value用于获取返回的布尔值，is\_floating\_point<Tp\>::value取值如下：

-   true：Tp是浮点类型。
-   false：Tp不是浮点类型。

## 调用示例

```
// 自定义测试类型
struct MyStruct{};

// 返回值为浮点的函数类型
using FuncType = double(int);

// Legal floating point type
AscendC::printf("AscendC::Std::is_floating_point::value:%d\n", AscendC::Std::is_floating_point<float>::value);
AscendC::printf("AscendC::Std::is_floating_point::value:%d\n", AscendC::Std::is_floating_point<double>::value);
AscendC::printf("AscendC::Std::is_floating_point::value:%d\n", AscendC::Std::is_floating_point<long double>::value);

// Floating point types limited by CV
AscendC::printf("AscendC::Std::is_floating_point::value:%d\n", AscendC::Std::is_floating_point<const float>::value);
AscendC::printf("AscendC::Std::is_floating_point::value:%d\n", AscendC::Std::is_floating_point<volatile double>::value);

// non-floating point type
AscendC::printf("AscendC::Std::is_floating_point::value:%d\n", AscendC::Std::is_floating_point<int>::value);
AscendC::printf("AscendC::Std::is_floating_point::value:%d\n", AscendC::Std::is_floating_point<bool>::value);
AscendC::printf("AscendC::Std::is_floating_point::value:%d\n", AscendC::Std::is_floating_point<double*>::value);
AscendC::printf("AscendC::Std::is_floating_point::value:%d\n", AscendC::Std::is_floating_point<float&>::value);
AscendC::printf("AscendC::Std::is_floating_point::value:%d\n", AscendC::Std::is_floating_point<double[5]>::value);
AscendC::printf("AscendC::Std::is_floating_point::value:%d\n", AscendC::Std::is_floating_point<FuncType>::value);
AscendC::printf("AscendC::Std::is_floating_point::value:%d\n", AscendC::Std::is_floating_point<MyStruct>::value);
AscendC::printf("AscendC::Std::is_floating_point::value:%d\n", AscendC::Std::is_floating_point<void>::value);
```

```
// 执行结果：
AscendC::Std::is_floating_point::value:1
AscendC::Std::is_floating_point::value:1
AscendC::Std::is_floating_point::value:1
AscendC::Std::is_floating_point::value:1
AscendC::Std::is_floating_point::value:1
AscendC::Std::is_floating_point::value:0
AscendC::Std::is_floating_point::value:0
AscendC::Std::is_floating_point::value:0
AscendC::Std::is_floating_point::value:0
AscendC::Std::is_floating_point::value:0
AscendC::Std::is_floating_point::value:0
AscendC::Std::is_floating_point::value:0
AscendC::Std::is_floating_point::value:0
```
