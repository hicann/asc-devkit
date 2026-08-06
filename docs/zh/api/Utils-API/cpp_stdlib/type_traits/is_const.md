# is\_const

## 产品支持情况

- Ascend 950PR/Ascend 950DT：支持
- Atlas A3 训练系列产品/Atlas A3 推理系列产品：支持
- Atlas A2 训练系列产品/Atlas A2 推理系列产品：支持
- Atlas 200I/500 A2 推理产品：不支持
- Atlas 推理系列产品AI Core：不支持
- Atlas 推理系列产品Vector Core：不支持
- Atlas 训练系列产品：不支持

## 功能说明

在程序编译时，检测一个类型是否为const限定的类型，可以用于在编译时进行类型检查和条件处理。

## 函数原型

```
template <typename Tp>
struct is_const;
```

## 参数说明

**表1**  模板参数说明

| 参数名 | 含义 |
| --- | --- |
| Tp | 需要检测的类型，包括基本类型（如int、float等）、复合类型（如数组、指针、引用）、用户自定义类型（如类、结构体等），以及const限定的类型本身。 |

## 约束说明

无

## 返回值说明

is\_const的静态常量成员value用于获取返回的布尔值，is\_const<Tp\>::value取值如下：

-   true：Tp是const类型。
-   false：Tp不是const类型。

## 调用示例

```
// 自定义测试类型
struct MyStruct{int val;};

// Mismatch scenario
AscendC::printf("AscendC::Std::is_const::value:%d\n", AscendC::Std::is_const<int>::value);
AscendC::printf("AscendC::Std::is_const::value:%d\n", AscendC::Std::is_const<const int*>::value);
AscendC::printf("AscendC::Std::is_const::value:%d\n", AscendC::Std::is_const<int&>::value);
AscendC::printf("AscendC::Std::is_const::value:%d\n", AscendC::Std::is_const<const int&>::value);
AscendC::printf("AscendC::Std::is_const::value:%d\n", AscendC::Std::is_const<int&&>::value);
AscendC::printf("AscendC::Std::is_const::value:%d\n", AscendC::Std::is_const<int[5]>::value);
AscendC::printf("AscendC::Std::is_const::value:%d\n", AscendC::Std::is_const<double>::value);
AscendC::printf("AscendC::Std::is_const::value:%d\n", AscendC::Std::is_const<MyStruct>::value);
AscendC::printf("AscendC::Std::is_const::value:%d\n", AscendC::Std::is_const<void>::value);

// Matching scenario
AscendC::printf("AscendC::Std::is_const::value:%d\n", AscendC::Std::is_const<const int>::value);
AscendC::printf("AscendC::Std::is_const::value:%d\n", AscendC::Std::is_const<int const>::value);
AscendC::printf("AscendC::Std::is_const::value:%d\n", AscendC::Std::is_const<int* const>::value);
AscendC::printf("AscendC::Std::is_const::value:%d\n", AscendC::Std::is_const<const int[5]>::value);
AscendC::printf("AscendC::Std::is_const::value:%d\n", AscendC::Std::is_const<const volatile double>::value);
AscendC::printf("AscendC::Std::is_const::value:%d\n", AscendC::Std::is_const<const MyStruct>::value);
```

```
// 执行结果：
AscendC::Std::is_const::value:0
AscendC::Std::is_const::value:0
AscendC::Std::is_const::value:0
AscendC::Std::is_const::value:0
AscendC::Std::is_const::value:0
AscendC::Std::is_const::value:0
AscendC::Std::is_const::value:0
AscendC::Std::is_const::value:0
AscendC::Std::is_const::value:0
AscendC::Std::is_const::value:1
AscendC::Std::is_const::value:1
AscendC::Std::is_const::value:1
AscendC::Std::is_const::value:1
AscendC::Std::is_const::value:1
AscendC::Std::is_const::value:1
```
