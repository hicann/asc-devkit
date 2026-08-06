# is\_void

## 产品支持情况

- Ascend 950PR/Ascend 950DT：支持
- Atlas A3 训练系列产品/Atlas A3 推理系列产品：支持
- Atlas A2 训练系列产品/Atlas A2 推理系列产品：支持
- Atlas 200I/500 A2 推理产品：不支持
- Atlas 推理系列产品AI Core：不支持
- Atlas 推理系列产品Vector Core：不支持
- Atlas 训练系列产品：不支持

## 功能说明

在程序编译时，检测一个类型是否为void类型，可以用于在编译时进行类型检查和条件处理。

## 函数原型

```
template <typename Tp>
struct is_void;
```

## 参数说明

**表1**  模板参数说明

| 参数名 | 含义 |
| --- | --- |
| Tp | 需要检测的类型，包括基本数据类型、复合数据类型、修饰类型、模板类、用户自定义类型等。 |

## 约束说明

无

## 返回值说明

is\_void的静态常量成员value用于获取返回的布尔值，is\_void<Tp\>::value取值如下：

-   true：Tp是void类型。
-   false：Tp不是void类型。

## 调用示例

```
// 自定义测试类型
struct MyStruct{};

// void类型的函数
using FuncType = void(int);

// Mismatch scenario
AscendC::printf("AscendC::Std::is_void::value:%d\n", AscendC::Std::is_void<int>::value);
AscendC::printf("AscendC::Std::is_void::value:%d\n", AscendC::Std::is_void<void*>::value);
AscendC::printf("AscendC::Std::is_void::value:%d\n", AscendC::Std::is_void<FuncType>::value);
AscendC::printf("AscendC::Std::is_void::value:%d\n", AscendC::Std::is_void<MyStruct>::value);

// Matching scenario
AscendC::printf("AscendC::Std::is_void::value:%d\n", AscendC::Std::is_void<void>::value);
AscendC::printf("AscendC::Std::is_void::value:%d\n", AscendC::Std::is_void<const void>::value);
AscendC::printf("AscendC::Std::is_void::value:%d\n", AscendC::Std::is_void<volatile void>::value);
AscendC::printf("AscendC::Std::is_void::value:%d\n", AscendC::Std::is_void<const volatile void>::value);
```

```
// 执行结果：
AscendC::Std::is_void::value:0
AscendC::Std::is_void::value:0
AscendC::Std::is_void::value:0
AscendC::Std::is_void::value:0
AscendC::Std::is_void::value:1
AscendC::Std::is_void::value:1
AscendC::Std::is_void::value:1
AscendC::Std::is_void::value:1
```
