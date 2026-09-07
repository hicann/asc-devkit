# remove\_volatile

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
<!-- @ref: asc-devkit/res/docs/zh/api/Utils-API/cpp_stdlib/type_traits/remove_volatile_res.md#id1 -->

## 功能说明

在程序编译时，对传入的模板参数类型移除volatile限定符，可以用于在编译时进行类型转换。

## 函数原型

```
template <typename Tp>
struct remove_volatile;
```

## 参数说明

**表1**  模板参数说明

| 参数名 | 含义 |
| --- | --- |
| Tp | 需要处理的类型，包括基本类型（如int、float等）、复合类型（如数组、指针、引用）、用户自定义类型（如类、结构体等），以及volatile限定的类型。 |

## 约束说明

无

## 返回值说明

remove\_volatile是一个结构体，其提供一个嵌套类型type，表示移除volatile限定符后的类型。通过remove\_volatile<Tp\>::type来访问该类型。

## 调用示例

```
// Test non-volatile type
ascendc_assert((AscendC::Std::is_same_v<AscendC::Std::remove_volatile<int>::type, int>));
ascendc_assert((AscendC::Std::is_same_v<AscendC::Std::remove_volatile<double>::type, double>));
ascendc_assert((AscendC::Std::is_same_v<AscendC::Std::remove_volatile<char>::type, char>));
// Test volatile type
ascendc_assert((AscendC::Std::is_same_v<AscendC::Std::remove_volatile<volatile int>::type, int>));
ascendc_assert((AscendC::Std::is_same_v<AscendC::Std::remove_volatile<volatile double>::type, double>));
ascendc_assert((AscendC::Std::is_same_v<AscendC::Std::remove_volatile<volatile char>::type, char>));

ascendc_assert((AscendC::Std::is_same_v<AscendC::Std::remove_volatile_t<int>, int>));
ascendc_assert((AscendC::Std::is_same_v<AscendC::Std::remove_volatile_t<double>, double>));
ascendc_assert((AscendC::Std::is_same_v<AscendC::Std::remove_volatile_t<volatile int>, int>));
ascendc_assert((AscendC::Std::is_same_v<AscendC::Std::remove_volatile_t<volatile double>, double>));
```
