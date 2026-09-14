# add\_volatile

<!-- md-trans-meta sourceCommit=c17b32aa8d725bcb43135ab25add5a26d62ad5c0 translatedAt=2026-09-06T11:54:23.304Z pushedAt=2026-09-11T04:50:15.622Z -->

## Applicable Products

- Ascend 950PR/Ascend 950DT: supported
- Atlas A3 training products/Atlas A3 inference products: supported
- Atlas A2 training products/Atlas A2 inference products: supported
- Atlas 200I/500 A2 inference products: not supported
- Atlas inference products AI Core: not supported
- Atlas inference products Vector Core: not supported
- Atlas training products: not supported

## Description

During program compilation, adding the volatile qualifier to a specified type can be used to perform type conversion at compile time.

## Prototype

```
template <typename Tp>
struct add_volatile;
```

## Parameters

**Table 1** Template parameter description

| Parameter | Description |
| --- | --- |
| Tp | Type to be processed, including basic types (such as **int**, **float**, etc.), compound types (such as arrays, pointers, references), user-defined types (such as classes, structures, etc.), and types with the **volatile** qualifier. |

## Constraints

None

## Return Value

add\_volatile is a structure that provides a nested type **type**, which represents the type after adding the volatile qualifier. Access this type through add\_volatile<Tp\>::type.

## Example

```
// Test non-volatile type
ascendc_assert((AscendC::Std::is_same_v<AscendC::Std::add_volatile<int>::type, volatile int>));
ascendc_assert((AscendC::Std::is_same_v<AscendC::Std::add_volatile<double>::type, volatile double>));
ascendc_assert((AscendC::Std::is_same_v<AscendC::Std::add_volatile<char>::type, volatile char>));
// Test volatile type
ascendc_assert((AscendC::Std::is_same_v<AscendC::Std::add_volatile<volatile int>::type, volatile int>));
ascendc_assert((AscendC::Std::is_same_v<AscendC::Std::add_volatile<volatile double>::type, volatile double>));
ascendc_assert((AscendC::Std::is_same_v<AscendC::Std::add_volatile<volatile char>::type, volatile char>));

ascendc_assert((AscendC::Std::is_same_v<AscendC::Std::add_volatile_t<int>, volatile int>));
ascendc_assert((AscendC::Std::is_same_v<AscendC::Std::add_volatile_t<double>, volatile double>));

ascendc_assert((AscendC::Std::is_same_v<AscendC::Std::add_volatile_t<volatile int>, volatile int>));
ascendc_assert((AscendC::Std::is_same_v<AscendC::Std::add_volatile_t<volatile double>, volatile double>));
```

