# add\_cv

<!-- md-trans-meta sourceCommit=22e1e8c65e425d4c0fb07294311f2eac049ab0b3 translatedAt=2026-11-10T10:14:44.198Z pushedAt=2026-11-13T08:46:49.214Z nl=2 -->

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
<!-- @ref: asc-devkit/res/docs/zh/api/Utils-API/cpp_stdlib/type_traits/add_cv_res.md#id1 -->

## Description

Adds the const and volatile qualifiers to a specified type at compile time, which can be used to perform type conversion at compile time.

## Prototype

```
template <typename Tp>
struct add_cv;
```

## Parameters

**Table 1** Template parameters

| parameter name | Description |
| --- | --- |
| Tp | Type to be processed, including basic types (such as int and float), compound types (such as arrays, pointers, and references), user-defined types (such as classes and structs), and types with the const qualifier, the volatile qualifier, or both. |

## Constraints

None

## Return Value

add\_cv is a struct that provides a nested type named type, which represents the type after adding the const and volatile qualifiers. Access this type via add\_cv<Tp\>::type.

## Examples

```
// Test non-const and non-volatile type
ascendc_assert((AscendC::Std::is_same_v<AscendC::Std::add_cv<int>::type, const volatile int>));
ascendc_assert((AscendC::Std::is_same_v<AscendC::Std::add_cv<double>::type, const volatile double>));
ascendc_assert((AscendC::Std::is_same_v<AscendC::Std::add_cv<char>::type, const volatile char>));
// Test const type
ascendc_assert((AscendC::Std::is_same_v<AscendC::Std::add_cv<const int>::type, const volatile int>));
ascendc_assert((AscendC::Std::is_same_v<AscendC::Std::add_cv<const double>::type, const volatile double>));
ascendc_assert((AscendC::Std::is_same_v<AscendC::Std::add_cv<const char>::type, const volatile char>));
// Test volatile type
ascendc_assert((AscendC::Std::is_same_v<AscendC::Std::add_cv<volatile int>::type, const volatile int>));
ascendc_assert((AscendC::Std::is_same_v<AscendC::Std::add_cv<volatile double>::type, const volatile double>));
ascendc_assert((AscendC::Std::is_same_v<AscendC::Std::add_cv<volatile char>::type, const volatile char>));
// Test const and volatile type
ascendc_assert((AscendC::Std::is_same_v<AscendC::Std::add_cv<const volatile int>::type, const volatile int>));
ascendc_assert((AscendC::Std::is_same_v<AscendC::Std::add_cv<const volatile double>::type, const volatile double>));
ascendc_assert((AscendC::Std::is_same_v<AscendC::Std::add_cv<const volatile char>::type, const volatile char>));

ascendc_assert((AscendC::Std::is_same_v<AscendC::Std::add_cv_t<int>, const volatile int>));
ascendc_assert((AscendC::Std::is_same_v<AscendC::Std::add_cv_t<double>, const volatile double>));

ascendc_assert((AscendC::Std::is_same_v<AscendC::Std::add_cv_t<const int>, const volatile int>));
ascendc_assert((AscendC::Std::is_same_v<AscendC::Std::add_cv_t<const double>, const volatile double>));

ascendc_assert((AscendC::Std::is_same_v<AscendC::Std::add_cv_t<volatile int>, const volatile int>));
ascendc_assert((AscendC::Std::is_same_v<AscendC::Std::add_cv_t<volatile double>, const volatile double>));

ascendc_assert((AscendC::Std::is_same_v<AscendC::Std::add_cv_t<const volatile int>, const volatile int>));
ascendc_assert((AscendC::Std::is_same_v<AscendC::Std::add_cv_t<const volatile double>, const volatile double>));
```
