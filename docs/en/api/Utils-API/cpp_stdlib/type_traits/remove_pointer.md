# remove\_pointer

<!-- md-trans-meta sourceCommit=22e1e8c65e425d4c0fb07294311f2eac049ab0b3 translatedAt=2026-11-10T10:27:38.928Z pushedAt=2026-11-13T08:46:49.249Z nl=2 -->

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
<!-- @ref: asc-devkit/res/docs/zh/api/Utils-API/cpp_stdlib/type_traits/remove_pointer_res.md#id1 -->

## Description

At compile time, removes the pointer qualifier from a given type, which can be used for type conversion at compile time.

## Prototype

```
template <typename Tp>
struct remove_pointer;
```

## Parameters

**Table 1** Template parameters

| Parameter | Description |
| --- | --- |
| Tp | Type to be processed, including basic types (such as int and float), compound types (such as arrays and references), user-defined types (such as classes and structs), and pointer types themselves. |

## Constraints

None

## Return Value

remove\_pointer is a struct that provides a nested type `type`, which represents the type after removing the pointer qualifier. Access this type through remove\_pointer<Tp\>::type.

## Examples

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
