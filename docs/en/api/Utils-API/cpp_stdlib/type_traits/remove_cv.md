# remove\_cv

<!-- md-trans-meta sourceCommit=c17b32aa8d725bcb43135ab25add5a26d62ad5c0 translatedAt=2026-09-06T12:19:26.037Z pushedAt=2026-09-11T04:52:19.796Z -->

## Applicable Products

- Ascend 950PR/Ascend 950DT: supported
- Atlas A3 training products/Atlas A3 inference products: supported
- Atlas A2 training products/Atlas A2 inference products: supported
- Atlas 200I/500 A2 inference products: not supported
- Atlas inference products AI Core: not supported
- Atlas inference products Vector Core: not supported
- Atlas training products: not supported

## Description

During program compilation, removes the const qualifier or the volatile qualifier, or both, from the passed-in template parameter type. This can be used to perform type conversion at compile time.

## Prototype

```
template <typename Tp>
struct remove_cv;
```

## Parameters

**Table 1** Template parameter description

| Parameter | Description |
| --- | --- |
| Tp | Type to be processed, including basic types (such as int and float), compound types (such as arrays, pointers, and references), user-defined types (such as classes and structs), and types qualified by const, volatile, or both. |

## Constraints

None

## Return Value

remove\_cv is a struct that provides a nested type **type**, which represents the type after removing the const and volatile qualifiers. Access this type through remove\_cv<Tp\>::type.

## Example

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

