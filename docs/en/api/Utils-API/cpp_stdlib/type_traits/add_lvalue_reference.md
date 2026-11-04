# add\_lvalue\_reference

<!-- md-trans-meta sourceCommit=754fb95859dead826727a591e674261c6a829a08 translatedAt=2026-10-23T01:24:46.267Z pushedAt=2026-10-23T02:49:52.805Z nl=2 -->

## Applicable Products

<!-- npu="950" id1 -->
- Ascend 950PR&950DT products: Supported
<!-- end id1 -->
<!-- npu="A3" id2 -->
- Atlas A3 products: Supported
<!-- end id2 -->
<!-- npu="910b" id3 -->
- Atlas A2 products: Supported
<!-- end id3 -->
<!-- npu="310b" id4 -->
- Atlas 200I/500 A2 inference products: Not supported
<!-- end id4 -->
<!-- npu="310p" id5 -->
- Atlas inference products AI Core: Not supported
- Atlas inference products Vector Core: Not supported
<!-- end id5 -->
<!-- npu="910" id6 -->
- Atlas training products: Not supported
<!-- end id6 -->
<!-- @ref: asc-devkit/res/docs/zh/api/Utils-API/cpp_stdlib/type_traits/add_lvalue_reference_res.md#id1 -->

## Description

At compile time, adds an lvalue reference qualifier to a specified type, which can be used for type conversion at compile time.

## Prototype

```
template <typename Tp>
struct add_lvalue_reference;
```

## Parameters

**Table 1** Template parameters

| Parameter | Description |
| --- | --- |
| Tp | Type to be processed, including basic types (such as int and float), composite types (such as arrays and pointers), user-defined types (such as classes and structs), and types with reference qualifiers. |

## Constraints

None

## Return Value

add\_lvalue\_reference is a struct that provides a nested type named type, which represents the type after adding the lvalue reference qualifier. Access this type through add\_lvalue\_reference<Tp\>::type.

## Examples

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
