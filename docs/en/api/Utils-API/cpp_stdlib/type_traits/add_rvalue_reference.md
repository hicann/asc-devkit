# add\_rvalue\_reference

<!-- md-trans-meta sourceCommit=c17b32aa8d725bcb43135ab25add5a26d62ad5c0 translatedAt=2026-09-06T11:53:05.702Z pushedAt=2026-09-11T04:50:13.460Z -->

## Applicable Products

- Ascend 950PR/Ascend 950DT: supported
- Atlas A3 training products/Atlas A3 inference products: supported
- Atlas A2 training products/Atlas A2 inference products: supported
- Atlas 200I/500 A2 inference products: not supported
- Atlas inference products AI Core: not supported
- Atlas inference products Vector Core: not supported
- Atlas training products: not supported

## Description

At compile time, adds an rvalue reference qualifier to the specified type, which can be used for type conversion at compile time.

## Prototype

```
template <typename Tp>
struct add_rvalue_reference;
```

## Parameters

**Table 1** Template parameter description

| Parameter | Description |
| --- | --- |
| Tp | Type to be processed, including basic types (such as int, float, etc.), compound types (such as arrays, pointers), user-defined types (such as classes, structs, etc.), and types with reference qualifiers. |

## Constraints

None

## Return Value

add\_rvalue\_reference is a struct that provides a nested type `type`, which represents the type after adding the rvalue reference qualifier. Access this type through add\_rvalue\_reference<Tp\>::type.

## Example

```
// Test basic type
ascendc_assert((AscendC::Std::is_same_v<AscendC::Std::add_rvalue_reference_t<int>, int&&>));
ascendc_assert((AscendC::Std::is_same_v<AscendC::Std::add_rvalue_reference_t<float>, float&&>));

// Test pointer type
ascendc_assert((AscendC::Std::is_same_v<AscendC::Std::add_rvalue_reference_t<int*>, int*&&>));
ascendc_assert((AscendC::Std::is_same_v<AscendC::Std::add_rvalue_reference_t<const int*>, const int*&&>));

// Test reference type
ascendc_assert((AscendC::Std::is_same_v<AscendC::Std::add_rvalue_reference_t<int&>, int&>));
ascendc_assert((AscendC::Std::is_same_v<AscendC::Std::add_rvalue_reference_t<int&&>, int&&>));
ascendc_assert((AscendC::Std::is_same_v<AscendC::Std::add_rvalue_reference_t<const int&>, const int&>));

// Test void type
ascendc_assert((AscendC::Std::is_same_v<AscendC::Std::add_rvalue_reference_t<void>, void>));
ascendc_assert((AscendC::Std::is_same_v<AscendC::Std::add_rvalue_reference_t<const void>, const void>));

// Test function type
using FuncType = void();
ascendc_assert((AscendC::Std::is_same_v<AscendC::Std::add_rvalue_reference_t<FuncType>, FuncType&&>));

// Test array type
using ArrayType = int[];
ascendc_assert((AscendC::Std::is_same_v<AscendC::Std::add_rvalue_reference_t<ArrayType>, ArrayType&&>));

// Test class type
class MyClass {};
ascendc_assert((AscendC::Std::is_same_v<AscendC::Std::add_rvalue_reference_t<MyClass>, MyClass&&>));
ascendc_assert((AscendC::Std::is_same_v<AscendC::Std::add_rvalue_reference_t<const MyClass>, const MyClass&&>));
```

