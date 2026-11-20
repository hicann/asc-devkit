# remove\_volatile

<!-- md-trans-meta sourceCommit=22e1e8c65e425d4c0fb07294311f2eac049ab0b3 translatedAt=2026-11-10T10:28:34.962Z pushedAt=2026-11-13T08:46:49.253Z nl=2 -->

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
<!-- @ref: asc-devkit/res/docs/zh/api/Utils-API/cpp_stdlib/type_traits/remove_volatile_res.md#id1 -->

## Description

At compile time, removes the volatile qualifier from the passed template parameter type, which can be used for type conversion at compile time.

## Prototype

```
template <typename Tp>
struct remove_volatile;
```

## Parameters

**Table 1** Template parameters

| Parameter | Description |
| --- | --- |
| Tp | Type to be processed, including basic types (such as int and float), compound types (such as arrays, pointers, and references), user-defined types (such as classes and structs), and volatile-qualified types. |

## Constraints

None

## Return Value

remove\_volatile is a struct that provides a nested type named type, which represents the type after the volatile qualifier is removed. Access this type through remove\_volatile<Tp\>::type.

## Examples

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
