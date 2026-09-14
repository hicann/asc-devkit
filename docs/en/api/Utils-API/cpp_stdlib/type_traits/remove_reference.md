# remove\_reference

<!-- md-trans-meta sourceCommit=c17b32aa8d725bcb43135ab25add5a26d62ad5c0 translatedAt=2026-09-06T12:22:21.802Z pushedAt=2026-09-11T04:52:35.584Z -->

## Applicable Products

- Ascend 950PR/Ascend 950DT: supported
- Atlas A3 training products/Atlas A3 inference products: supported
- Atlas A2 training products/Atlas A2 inference products: supported
- Atlas 200I/500 A2 inference product: not supported
- Atlas inference products AI Core: not supported
- Atlas inference products Vector Core: not supported
- Atlas training products: not supported

## Description

At compile time, removes reference qualifiers from a given type, including lvalue references T& and rvalue references T&&. It can be used to perform type conversion at compile time.

## Prototype

```
template <typename Tp>
struct remove_reference;
```

## Parameters

**Table 1** Template parameter description

| Parameter | Description |
| --- | --- |
| Tp | Type to be processed, including basic types (such as int and float), compound types (such as arrays and pointers), user-defined types (such as classes and structs), and types qualified with an lvalue reference & or rvalue reference &&. |

## Constraints

None

## Return Value

remove\_reference is a structure that provides a nested type **type**, which represents the type after removing the reference qualifier. Access this type through remove\_reference<Tp\>::type.

## Example

```
// Test non-reference type
ascendc_assert((AscendC::Std::is_same_v<AscendC::Std::remove_reference<int>::type, int>));
ascendc_assert((AscendC::Std::is_same_v<AscendC::Std::remove_reference<double>::type, double>));
ascendc_assert((AscendC::Std::is_same_v<AscendC::Std::remove_reference<char>::type, char>));
// Test lvalue reference type
ascendc_assert((AscendC::Std::is_same_v<AscendC::Std::remove_reference<int&>::type, int>));
ascendc_assert((AscendC::Std::is_same_v<AscendC::Std::remove_reference<double&>::type, double>));
ascendc_assert((AscendC::Std::is_same_v<AscendC::Std::remove_reference<char&>::type, char>));
// Test rvalue reference type
ascendc_assert((AscendC::Std::is_same_v<AscendC::Std::remove_reference<int&&>::type, int>));
ascendc_assert((AscendC::Std::is_same_v<AscendC::Std::remove_reference<double&&>::type, double>));
ascendc_assert((AscendC::Std::is_same_v<AscendC::Std::remove_reference<char&&>::type, char>));

ascendc_assert((AscendC::Std::is_same_v<AscendC::Std::remove_reference_t<int>, int>));
ascendc_assert((AscendC::Std::is_same_v<AscendC::Std::remove_reference_t<double>, double>));

ascendc_assert((AscendC::Std::is_same_v<AscendC::Std::remove_reference_t<int&>, int>));
ascendc_assert((AscendC::Std::is_same_v<AscendC::Std::remove_reference_t<double&>, double>));

ascendc_assert((AscendC::Std::is_same_v<AscendC::Std::remove_reference_t<int&&>, int>));
ascendc_assert((AscendC::Std::is_same_v<AscendC::Std::remove_reference_t<double&&>, double>));
```

