# remove\_volatile

<!-- md-trans-meta sourceCommit=c17b32aa8d725bcb43135ab25add5a26d62ad5c0 translatedAt=2026-09-06T12:23:39.872Z pushedAt=2026-09-11T04:52:40.574Z -->

## Applicable Products

- Ascend 950PR/Ascend 950DT: supported
- Atlas A3 training products/Atlas A3 inference products: supported
- Atlas A2 training products/Atlas A2 inference products: supported
- Atlas 200I/500 A2 inference product: not supported
- Atlas inference products AI Core: not supported
- Atlas inference products Vector Core: not supported
- Atlas training products: not supported

## Description

During program compilation, removes the volatile qualifier from the passed-in template parameter type, which can be used for type conversion at compile time.

## Prototype

```
template <typename Tp>
struct remove_volatile;
```

## Parameters

**Table 1** Template parameter description

| Parameter | Description |
| --- | --- |
| Tp | The type to be processed, including basic types (such as int and float), composite types (such as arrays, pointers, and references), user-defined types (such as classes and structs), and volatile-qualified types. |

## Constraints

None

## Return Value

remove\_volatile is a structure that provides a nested type **type**, which represents the type after removing the volatile qualifier. Access this type through remove\_volatile<Tp\>::type.

## Example

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

