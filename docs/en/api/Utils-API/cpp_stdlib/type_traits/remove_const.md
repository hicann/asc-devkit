# remove\_const

<!-- md-trans-meta sourceCommit=c17b32aa8d725bcb43135ab25add5a26d62ad5c0 translatedAt=2026-09-06T12:18:05.219Z pushedAt=2026-09-11T04:51:57.724Z -->

## Applicable Products

- Ascend 950PR/Ascend 950DT: supported
- Atlas A3 training products/Atlas A3 inference products: supported
- Atlas A2 training products/Atlas A2 inference products: supported
- Atlas 200I/500 A2 inference products: not supported
- Atlas inference products AI Core: not supported
- Atlas inference products Vector Core: not supported
- Atlas training products: not supported

## Description

During program compilation, removes the const qualifier from the passed-in template parameter type, which can be used for type conversion at compile time.

## Prototype

```
template <typename Tp>
struct remove_const;
```

## Parameters

**Table 1**  Template parameter description

| Parameter | Description |
| --- | --- |
| Tp | Type to be processed, including basic types (such as int and float), composite types (such as arrays, pointers, and references), user-defined types (such as classes and structures), and const-qualified types. |

## Constraints

None

## Return Value

remove\_const is a structure that provides a nested type **type**, which represents the type after removing the const qualifier. Access this type through remove\_const<Tp\>::type.

## Example

```
// Test non-const type
ascendc_assert((AscendC::Std::is_same_v<AscendC::Std::remove_const<int>::type, int>));
ascendc_assert((AscendC::Std::is_same_v<AscendC::Std::remove_const<double>::type, double>));
ascendc_assert((AscendC::Std::is_same_v<AscendC::Std::remove_const<char>::type, char>));

// Test const type
ascendc_assert((AscendC::Std::is_same_v<AscendC::Std::remove_const<const int>::type, int>));
ascendc_assert((AscendC::Std::is_same_v<AscendC::Std::remove_const<const double>::type, double>));
ascendc_assert((AscendC::Std::is_same_v<AscendC::Std::remove_const<const char>::type, char>));

ascendc_assert((AscendC::Std::is_same_v<AscendC::Std::remove_const_t<int>, int>));
ascendc_assert((AscendC::Std::is_same_v<AscendC::Std::remove_const_t<double>, double>));
ascendc_assert((AscendC::Std::is_same_v<AscendC::Std::remove_const_t<const int>, int>));
ascendc_assert((AscendC::Std::is_same_v<AscendC::Std::remove_const_t<const double>, double>));
```

