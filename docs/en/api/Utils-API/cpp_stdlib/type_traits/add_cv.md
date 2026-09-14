# add\_cv

<!-- md-trans-meta sourceCommit=c17b32aa8d725bcb43135ab25add5a26d62ad5c0 translatedAt=2026-09-06T11:49:22.803Z pushedAt=2026-09-11T04:48:13.522Z -->

## Applicable Products

- Ascend 950PR/Ascend 950DT: supported
- Atlas A3 training products/Atlas A3 inference products: supported
- Atlas A2 training products/Atlas A2 inference products: supported
- Atlas 200I/500 A2 inference products: not supported
- Atlas inference products AI Core: not supported
- Atlas inference products Vector Core: not supported
- Atlas training products: not supported

## Description

Adds the **const** and **volatile** qualifiers to a specified type at compile time, which can be used for type conversion at compile time.

## Prototype

```
template <typename Tp>
struct add_cv;
```

## Parameters

**Table 1** Template parameter description

| Parameter | Description |
| --- | --- |
| Tp | Type to be processed, including basic types (such as int and float), composite types (such as arrays, pointers, and references), user-defined types (such as classes and structures), and types with a const qualifier, a volatile qualifier, or both qualifiers. |

## Constraints

None

## Return Value

add\_cv is a structure that provides a nested type **type**, which represents the type after adding the **const** and **volatile** qualifiers. Access this type through add\_cv<Tp\>::type.

## Example

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

