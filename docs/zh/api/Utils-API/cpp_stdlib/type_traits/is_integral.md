# is\_integral

## 产品支持情况

<!-- npu="950" id1 -->
- Ascend 950PR/Ascend 950DT：支持
<!-- end id1 -->
<!-- npu="A3" id2 -->
- Atlas A3 训练系列产品/Atlas A3 推理系列产品：支持
<!-- end id2 -->
<!-- npu="910b" id3 -->
- Atlas A2 训练系列产品/Atlas A2 推理系列产品：支持
<!-- end id3 -->
<!-- npu="310b" id4 -->
- Atlas 200I/500 A2 推理产品：不支持
<!-- end id4 -->
<!-- npu="310p" id5 -->
- Atlas 推理系列产品AI Core：不支持
- Atlas 推理系列产品Vector Core：不支持
<!-- end id5 -->
<!-- npu="910" id6 -->
- Atlas 训练系列产品：不支持
<!-- end id6 -->
<!-- @ref: asc-devkit/res/docs/zh/api/Utils-API/cpp_stdlib/type_traits/is_integral_res.md#id1 -->

## 功能说明

在程序编译时，检测一个类型是否为整数类型，可以用于在编译时进行类型检查和条件处理。

## 函数原型

```
template <typename T>
struct is_integral;
```

## 参数说明

**表1**  模板参数说明

| 参数名 | 含义 |
| --- | --- |
| T | 需要检测的类型，包括基本数据类型、修饰类型等。 |

## 约束说明

无

## 返回值说明

is\_integral的静态常量成员value用于获取返回的布尔值，is\_integral<T\>::value取值如下：

-   true：Tp是数组类型。
-   false：Tp不是数组类型。

## 调用示例

```
// 自定义测试类型
struct MyStruct{};
using FuncType = int(int);

// legal integer type
AscendC::printf("AscendC::Std::is_integral::value:%d\n", AscendC::Std::is_integral<bool>::value);
AscendC::printf("AscendC::Std::is_integral::value:%d\n", AscendC::Std::is_integral<char>::value);
AscendC::printf("AscendC::Std::is_integral::value:%d\n", AscendC::Std::is_integral<int>::value);
AscendC::printf("AscendC::Std::is_integral::value:%d\n", AscendC::Std::is_integral<long long>::value);
AscendC::printf("AscendC::Std::is_integral::value:%d\n", AscendC::Std::is_integral<unsigned int>::value);

// Integer type limited by CV
AscendC::printf("AscendC::Std::is_integral::value:%d\n", AscendC::Std::is_integral<const int>::value);
AscendC::printf("AscendC::Std::is_integral::value:%d\n", AscendC::Std::is_integral<volatile long>::value);

// non-integer type
AscendC::printf("AscendC::Std::is_integral::value:%d\n", AscendC::Std::is_integral<float>::value);
AscendC::printf("AscendC::Std::is_integral::value:%d\n", AscendC::Std::is_integral<double>::value);
AscendC::printf("AscendC::Std::is_integral::value:%d\n", AscendC::Std::is_integral<int*>::value);
AscendC::printf("AscendC::Std::is_integral::value:%d\n", AscendC::Std::is_integral<int&>::value);
AscendC::printf("AscendC::Std::is_integral::value:%d\n", AscendC::Std::is_integral<int[5]>::value);
AscendC::printf("AscendC::Std::is_integral::value:%d\n", AscendC::Std::is_integral<FuncType>::value);
AscendC::printf("AscendC::Std::is_integral::value:%d\n", AscendC::Std::is_integral<MyStruct>::value);
AscendC::printf("AscendC::Std::is_integral::value:%d\n", AscendC::Std::is_integral<void>::value);
```

```
// 执行结果：
AscendC::Std::is_integral::value:1
AscendC::Std::is_integral::value:1
AscendC::Std::is_integral::value:1
AscendC::Std::is_integral::value:1
AscendC::Std::is_integral::value:1
AscendC::Std::is_integral::value:1
AscendC::Std::is_integral::value:1
AscendC::Std::is_integral::value:0
AscendC::Std::is_integral::value:0
AscendC::Std::is_integral::value:0
AscendC::Std::is_integral::value:0
AscendC::Std::is_integral::value:0
AscendC::Std::is_integral::value:0
AscendC::Std::is_integral::value:0
AscendC::Std::is_integral::value:0
```
