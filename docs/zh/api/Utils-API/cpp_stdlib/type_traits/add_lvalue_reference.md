# add\_lvalue\_reference

## 产品支持情况

- Ascend 950PR/Ascend 950DT：支持
- Atlas A3 训练系列产品/Atlas A3 推理系列产品：支持
- Atlas A2 训练系列产品/Atlas A2 推理系列产品：支持
- Atlas 200I/500 A2 推理产品：不支持
- Atlas 推理系列产品AI Core：不支持
- Atlas 推理系列产品Vector Core：不支持
- Atlas 训练系列产品：不支持

## 功能说明

在程序编译时，为指定类型添加左值引用限定符，可以用于在编译时进行类型转换。

## 函数原型

```
template <typename Tp>
struct add_lvalue_reference;
```

## 参数说明

**表1**  模板参数说明

| 参数名 | 含义 |
| --- | --- |
| Tp | 需要处理的类型，包括基本类型（如int、float等）、复合类型（如数组、指针）、用户自定义类型（如类、结构体等），以及带有引用限定符的类型。 |

## 约束说明

无

## 返回值说明

add\_lvalue\_reference是一个结构体，其提供一个嵌套类型type，表示添加左值引用限定符后的类型。通过add\_lvalue\_reference<Tp\>::type来访问该类型。

## 调用示例

```
// Test basic type
ascendc_assert((AscendC::Std::is_same_v<AscendC::Std::add_lvalue_reference_t<int>, int&>));
ascendc_assert((AscendC::Std::is_same_v<AscendC::Std::add_lvalue_reference_t<float>, float&>));

// Test pointer type
ascendc_assert((AscendC::Std::is_same_v<AscendC::Std::add_lvalue_reference_t<int*>, int*&>));
ascendc_assert((AscendC::Std::is_same_v<AscendC::Std::add_lvalue_reference_t<const int*>, const int*&>));

// Test reference type
ascendc_assert((AscendC::Std::is_same_v<AscendC::Std::add_lvalue_reference_t<int&>, int&>));
ascendc_assert((AscendC::Std::is_same_v<AscendC::Std::add_lvalue_reference_t<int&&>, int&>));
ascendc_assert((AscendC::Std::is_same_v<AscendC::Std::add_lvalue_reference_t<const int&>, const int&>));

// Test void type
ascendc_assert((AscendC::Std::is_same_v<AscendC::Std::add_lvalue_reference_t<void>, void>));
ascendc_assert((AscendC::Std::is_same_v<AscendC::Std::add_lvalue_reference_t<const void>, const void>));

// Test function type
using FuncType = void();
ascendc_assert((AscendC::Std::is_same_v<AscendC::Std::add_lvalue_reference_t<FuncType>, FuncType&>));

// Test array type
using ArrayType = int[];
ascendc_assert((AscendC::Std::is_same_v<AscendC::Std::add_lvalue_reference_t<ArrayType>, ArrayType&>));

// Test class type
class MyClass {};
ascendc_assert((AscendC::Std::is_same_v<AscendC::Std::add_lvalue_reference_t<MyClass>, MyClass&>));
ascendc_assert((AscendC::Std::is_same_v<AscendC::Std::add_lvalue_reference_t<const MyClass>, const MyClass&>));
```
