# remove\_pointer

<!-- md-trans-meta sourceCommit=c17b32aa8d725bcb43135ab25add5a26d62ad5c0 translatedAt=2026-09-06T12:20:50.898Z pushedAt=2026-09-11T04:52:28.468Z -->

## Applicable Products

- Ascend 950PR/Ascend 950DT: supported
- Atlas A3 training products/Atlas A3 inference products: supported
- Atlas A2 training products/Atlas A2 inference products: supported
- Atlas 200I/500 A2 inference product: not supported
- Atlas inference products AI Core: not supported
- Atlas inference products Vector Core: not supported
- Atlas training products: not supported

## Description

At compile time, removes the pointer qualifier from a given type, which can be used for type conversion at compile time.

## Prototype

```
template <typename Tp>
struct remove_pointer;
```

## Parameters

**Table 1** Template parameter description

| Parameter | Description |
| --- | --- |
| Tp | Type to be processed, including basic types (such as int and float), compound types (such as arrays and references), user-defined types (such as classes and structs), and pointer types themselves. |

## Constraints

None

## Return Value

remove\_pointer is a struct that provides a nested type **type**, which represents the type after removing the pointer qualifier. Access this type through remove\_pointer<Tp\>::type.

## Example

```
// Test non-pointer type
ascendc_assert((AscendC::Std::is_same_v<AscendC::Std::remove_pointer<int>::type, int>));
ascendc_assert((AscendC::Std::is_same_v<AscendC::Std::remove_pointer<double>::type, double>));
ascendc_assert((AscendC::Std::is_same_v<AscendC::Std::remove_pointer<char>::type, char>));
// Test pointer type
ascendc_assert((AscendC::Std::is_same_v<AscendC::Std::remove_pointer<int*>::type, int>));
ascendc_assert((AscendC::Std::is_same_v<AscendC::Std::remove_pointer<double*>::type, double>));
ascendc_assert((AscendC::Std::is_same_v<AscendC::Std::remove_pointer<char*>::type, char>));
// Test const pointer type
ascendc_assert((AscendC::Std::is_same_v<AscendC::Std::remove_pointer<int* const>::type, int>));
ascendc_assert((AscendC::Std::is_same_v<AscendC::Std::remove_pointer<double* const>::type, double>));
ascendc_assert((AscendC::Std::is_same_v<AscendC::Std::remove_pointer<char* const>::type, char>));
// Test volatile type
ascendc_assert((AscendC::Std::is_same_v<AscendC::Std::remove_pointer<int* volatile>::type, int>));
ascendc_assert((AscendC::Std::is_same_v<AscendC::Std::remove_pointer<double* volatile>::type, double>));
ascendc_assert((AscendC::Std::is_same_v<AscendC::Std::remove_pointer<char* volatile>::type, char>));
// Test const and volatile type
ascendc_assert((AscendC::Std::is_same_v<AscendC::Std::remove_pointer<int* const volatile>::type, int>));
ascendc_assert((AscendC::Std::is_same_v<AscendC::Std::remove_pointer<double* const volatile>::type, double>));
ascendc_assert((AscendC::Std::is_same_v<AscendC::Std::remove_pointer<char* const volatile>::type, char>));

ascendc_assert((AscendC::Std::is_same_v<AscendC::Std::remove_pointer_t<int>, int>));
ascendc_assert((AscendC::Std::is_same_v<AscendC::Std::remove_pointer_t<double>, double>));

ascendc_assert((AscendC::Std::is_same_v<AscendC::Std::remove_pointer_t<int*>, int>));
ascendc_assert((AscendC::Std::is_same_v<AscendC::Std::remove_pointer_t<double*>, double>));

ascendc_assert((AscendC::Std::is_same_v<AscendC::Std::remove_pointer_t<int* const>, int>));
ascendc_assert((AscendC::Std::is_same_v<AscendC::Std::remove_pointer_t<double* const>, double>));

ascendc_assert((AscendC::Std::is_same_v<AscendC::Std::remove_pointer_t<int* volatile>, int>));
ascendc_assert((AscendC::Std::is_same_v<AscendC::Std::remove_pointer_t<double* volatile>, double>));

ascendc_assert((AscendC::Std::is_same_v<AscendC::Std::remove_pointer_t<int* const volatile>, int>));
ascendc_assert((AscendC::Std::is_same_v<AscendC::Std::remove_pointer_t<double* const volatile>, double>));
```

