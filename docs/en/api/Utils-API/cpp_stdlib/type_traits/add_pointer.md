# add\_pointer

<!-- md-trans-meta sourceCommit=c17b32aa8d725bcb43135ab25add5a26d62ad5c0 translatedAt=2026-09-06T11:51:54.542Z pushedAt=2026-09-11T04:50:11.512Z -->

## Applicable Products

- Ascend 950PR/Ascend 950DT: supported
- Atlas A3 training products/Atlas A3 inference products: supported
- Atlas A2 training products/Atlas A2 inference products: supported
- Atlas 200I/500 A2 inference products: not supported
- Atlas inference products AI Core: not supported
- Atlas inference products Vector Core: not supported
- Atlas training products: not supported

## Description

At compile time, adds a pointer qualifier to a specified type, which can be used for compile-time type conversion.

## Prototype

```
template <typename Tp>
struct add_pointer;
```

## Parameters

**Table 1** Template parameter description

| Parameter | Description |
| --- | --- |
| Tp | Type to be processed, including fundamental types (such as int and float), compound types (such as arrays and references), and types with pointer qualifiers. |

## Constraints

None

## Return Value

add\_pointer is a struct that provides a nested type **type**, which represents the type after adding a pointer qualifier. Access this type through add\_pointer<Tp\>::type.

## Example

```
// Test basic type
ascendc_assert((AscendC::Std::is_same_v<AscendC::Std::add_pointer<int>::type, int*>));
ascendc_assert((AscendC::Std::is_same_v<AscendC::Std::add_pointer<float>::type, float*>));

// Test void type
ascendc_assert((AscendC::Std::is_same_v<AscendC::Std::add_pointer<void>::type, void*>));
ascendc_assert((AscendC::Std::is_same_v<AscendC::Std::add_pointer<const void>::type, const void*>));

// Test reference type
ascendc_assert((AscendC::Std::is_same_v<AscendC::Std::add_pointer<int&>::type, int*>));
ascendc_assert((AscendC::Std::is_same_v<AscendC::Std::add_pointer<const int&>::type, const int*>));

// Test function type
using FuncType = void();
ascendc_assert((AscendC::Std::is_same_v<AscendC::Std::add_pointer<FuncType>::type, FuncType*>));

// Test array type
using ArrayType = int[];
ascendc_assert((AscendC::Std::is_same_v<AscendC::Std::add_pointer<ArrayType>::type, ArrayType*>));

// Test pointer type
ascendc_assert((AscendC::Std::is_same_v<AscendC::Std::add_pointer<int*>::type, int**>));
```

