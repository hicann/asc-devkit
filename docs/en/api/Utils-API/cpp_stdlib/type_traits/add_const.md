# add\_const

<!-- md-trans-meta sourceCommit=c17b32aa8d725bcb43135ab25add5a26d62ad5c0 translatedAt=2026-09-06T11:48:08.002Z pushedAt=2026-09-11T04:48:04.941Z -->

## Applicable Products

- Ascend 950PR/Ascend 950DT: supported
- Atlas A3 training products/Atlas A3 inference products: supported
- Atlas A2 training products/Atlas A2 inference products: supported
- Atlas 200I/500 A2 inference products: not supported
- Atlas inference products AI Core: not supported
- Atlas inference products Vector Core: not supported
- Atlas training products: not supported

## Description

Adds a const qualifier to a specified type at compile time, which can be used to perform type conversion at compile time.

## Prototype

```
template <typename Tp>
struct add_const;
```

## Parameters

**Table 1** Template parameter description

| Parameter | Description |
| --- | --- |
| Tp | The type to be processed, including basic types (such as int and float), composite types (such as arrays, pointers, and references), user-defined types (such as classes and structs), and types with a const qualifier. |

## Constraints

None

## Return Value

add\_const is a structure that provides a nested type `type`, which represents the type after adding the const qualifier. Access this type through add\_const<Tp\>::type.

## Example

```
// Test non-const type
ascendc_assert((AscendC::Std::is_same_v<AscendC::Std::add_const<int>::type, const int>));
ascendc_assert((AscendC::Std::is_same_v<AscendC::Std::add_const<double>::type, const double>));
ascendc_assert((AscendC::Std::is_same_v<AscendC::Std::add_const<char>::type, const char>));
// Test const type
ascendc_assert((AscendC::Std::is_same_v<AscendC::Std::add_const<const int>::type, const int>));
ascendc_assert((AscendC::Std::is_same_v<AscendC::Std::add_const<const double>::type, const double>));
ascendc_assert((AscendC::Std::is_same_v<AscendC::Std::add_const<const char>::type, const char>));

ascendc_assert((AscendC::Std::is_same_v<AscendC::Std::add_const_t<int>, const int>));
ascendc_assert((AscendC::Std::is_same_v<AscendC::Std::add_const_t<double>, const double>));

ascendc_assert((AscendC::Std::is_same_v<AscendC::Std::add_const_t<const int>, const int>));
ascendc_assert((AscendC::Std::is_same_v<AscendC::Std::add_const_t<const double>, const double>));
```

