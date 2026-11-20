# add\_const

<!-- md-trans-meta sourceCommit=22e1e8c65e425d4c0fb07294311f2eac049ab0b3 translatedAt=2026-11-10T10:14:26.791Z pushedAt=2026-11-13T08:46:49.213Z nl=2 -->

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
<!-- @ref: asc-devkit/res/docs/zh/api/Utils-API/cpp_stdlib/type_traits/add_const_res.md#id1 -->

## Description

Adds the const qualifier to a specified type at compile time, which can be used for type conversion at compile time.

## Prototype

```
template <typename Tp>
struct add_const;
```

## Parameters

**Table 1** Template parameters

| Parameter | Description |
| --- | --- |
| Tp | Type to be processed, including basic types (such as int and float), compound types (such as arrays, pointers, and references), user-defined types (such as classes and structs), and types with the const qualifier. |

## Constraints

None

## Return Value

add\_const is a struct that provides a nested type named type, which represents the type after the const qualifier is added. Access this type through add\_const<Tp\>::type.

## Examples

```
// Test non-const type
ascendc_assert((AscendC::Std::is_same_v<AscendC::Std::add_const<int>::type, const int>));
ascendc_assert((AscendC::Std::is_same_v<AscendC::Std::add_const<double>::type, const double>));
ascendc_assert((AscendC::Std::is_same_v<AscendC::Std::add_const<char>::type, const char>));
// Test const type
ascendc_assert((AscendC::Std::is_same_v<AscendC::Std::add_const<const int>::type, const int>));
ascendc_assert((AscendC::Std::is_same_v<AscendC::Std::add_const<const double>::type, const double>));
ascendc_assert((AscendC::Std::is_same_v<AscendC::Std::add_const<const char>::type, const char>));

ascendc_assert((AscendC::Std::is_same_v<AscendC::Std::add_const_t<int>, const int>));
ascendc_assert((AscendC::Std::is_same_v<AscendC::Std::add_const_t<double>, const double>));

ascendc_assert((AscendC::Std::is_same_v<AscendC::Std::add_const_t<const int>, const int>));
ascendc_assert((AscendC::Std::is_same_v<AscendC::Std::add_const_t<const double>, const double>));
```
