# add\_pointer

<!-- md-trans-meta sourceCommit=22e1e8c65e425d4c0fb07294311f2eac049ab0b3 translatedAt=2026-11-10T10:15:47.246Z pushedAt=2026-11-13T08:46:49.218Z nl=2 -->

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
<!-- @ref: asc-devkit/res/docs/zh/api/Utils-API/cpp_stdlib/type_traits/add_pointer_res.md#id1 -->

## Description

Adds a pointer qualifier to a specified type at compile time, which can be used for type conversion during compilation.

## Prototype

```
template <typename Tp>
struct add_pointer;
```

## Parameters

**Table 1** Template parameters

| Parameter | Description |
| --- | --- |
| Tp | Type to be processed, including basic types (such as int and float), compound types (such as arrays and references), and types with pointer qualifiers. |

## Constraints

None

## Return Value

add\_pointer is a struct that provides a nested type named type, which represents the type after the pointer qualifier is added. Access this type through add\_pointer<Tp\>::type.

## Examples

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
