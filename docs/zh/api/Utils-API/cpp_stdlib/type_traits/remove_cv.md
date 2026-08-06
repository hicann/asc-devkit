# remove\_cv

## 产品支持情况

- Ascend 950PR/Ascend 950DT：支持
- Atlas A3 训练系列产品/Atlas A3 推理系列产品：支持
- Atlas A2 训练系列产品/Atlas A2 推理系列产品：支持
- Atlas 200I/500 A2 推理产品：不支持
- Atlas 推理系列产品AI Core：不支持
- Atlas 推理系列产品Vector Core：不支持
- Atlas 训练系列产品：不支持

## 功能说明

在程序编译时，对传入的模板参数类型移除const限定符或volatile限定符，或同时移除这两种限定符，可以用于在编译时进行类型转换。

## 函数原型

```
template <typename Tp>
struct remove_cv;
```

## 参数说明

**表1**  模板参数说明

| 参数名 | 含义 |
| --- | --- |
| Tp | 需要处理的类型，包括基本类型（如int、float等）、复合类型（如数组、指针、引用）、用户自定义类型（如类、结构体等），以及const限定、volatile限定或两者都限定的类型。 |

## 约束说明

无

## 返回值说明

remove\_cv是一个结构体，其提供一个嵌套类型type，表示移除const和volatile限定符后的类型。通过remove\_cv<Tp\>::type来访问该类型。

## 调用示例

```
// Test non-const and non-volatile type
ascendc_assert((AscendC::Std::is_same_v<AscendC::Std::remove_cv<int>::type, int>));
ascendc_assert((AscendC::Std::is_same_v<AscendC::Std::remove_cv<double>::type, double>));
ascendc_assert((AscendC::Std::is_same_v<AscendC::Std::remove_cv<char>::type, char>));
// Test const type
ascendc_assert((AscendC::Std::is_same_v<AscendC::Std::remove_cv<const int>::type, int>));
ascendc_assert((AscendC::Std::is_same_v<AscendC::Std::remove_cv<const double>::type, double>));
ascendc_assert((AscendC::Std::is_same_v<AscendC::Std::remove_cv<const char>::type, char>));
// Test volatile type
ascendc_assert((AscendC::Std::is_same_v<AscendC::Std::remove_cv<volatile int>::type, int>));
ascendc_assert((AscendC::Std::is_same_v<AscendC::Std::remove_cv<volatile double>::type, double>));
ascendc_assert((AscendC::Std::is_same_v<AscendC::Std::remove_cv<volatile char>::type, char>));
// Test const and volatile type
ascendc_assert((AscendC::Std::is_same_v<AscendC::Std::remove_cv<const volatile int>::type, int>));
ascendc_assert((AscendC::Std::is_same_v<AscendC::Std::remove_cv<const volatile double>::type, double>));
ascendc_assert((AscendC::Std::is_same_v<AscendC::Std::remove_cv<const volatile char>::type, char>));

ascendc_assert((AscendC::Std::is_same_v<AscendC::Std::remove_cv_t<int>, int>));
ascendc_assert((AscendC::Std::is_same_v<AscendC::Std::remove_cv_t<double>, double>));

ascendc_assert((AscendC::Std::is_same_v<AscendC::Std::remove_cv_t<const int>, int>));
ascendc_assert((AscendC::Std::is_same_v<AscendC::Std::remove_cv_t<const double>, double>));

ascendc_assert((AscendC::Std::is_same_v<AscendC::Std::remove_cv_t<volatile int>, int>));
ascendc_assert((AscendC::Std::is_same_v<AscendC::Std::remove_cv_t<volatile double>, double>));

ascendc_assert((AscendC::Std::is_same_v<AscendC::Std::remove_cv_t<const volatile int>, int>));
ascendc_assert((AscendC::Std::is_same_v<AscendC::Std::remove_cv_t<const volatile double>, double>));
```
