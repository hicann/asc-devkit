# is\_const

<!-- md-trans-meta sourceCommit=22e1e8c65e425d4c0fb07294311f2eac049ab0b3 translatedAt=2026-11-10T10:20:38.415Z pushedAt=2026-11-13T08:46:49.232Z nl=2 -->

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
<!-- @ref: asc-devkit/res/docs/zh/api/Utils-API/cpp_stdlib/type_traits/is_const_res.md#id1 -->

## Description

At compile time, checks whether a type is const-qualified, which can be used for compile-time type checking and conditional processing.

## Prototype

```
template <typename Tp>
struct is_const;
```

## Parameters

**Table 1** Template parameters

| Parameter | Description |
| --- | --- |
| Tp | Type to be checked, including basic types (such as int and float), compound types (such as arrays, pointers, and references), user-defined types (such as classes and structs), and const-qualified types themselves. |

## Constraints

None

## Return Value

The static constant member value of is\_const is used to obtain the returned Boolean value. is\_const<Tp\>::value takes the following values:

-   true: Tp is a const type.
-   false: Tp is not a const type.

## Examples

```
// Custom test type.
struct MyStruct{int val;};

// Mismatch scenario
AscendC::printf("AscendC::Std::is_const::value:%d\n", AscendC::Std::is_const<int>::value);
AscendC::printf("AscendC::Std::is_const::value:%d\n", AscendC::Std::is_const<const int*>::value);
AscendC::printf("AscendC::Std::is_const::value:%d\n", AscendC::Std::is_const<int&>::value);
AscendC::printf("AscendC::Std::is_const::value:%d\n", AscendC::Std::is_const<const int&>::value);
AscendC::printf("AscendC::Std::is_const::value:%d\n", AscendC::Std::is_const<int&&>::value);
AscendC::printf("AscendC::Std::is_const::value:%d\n", AscendC::Std::is_const<int[5]>::value);
AscendC::printf("AscendC::Std::is_const::value:%d\n", AscendC::Std::is_const<double>::value);
AscendC::printf("AscendC::Std::is_const::value:%d\n", AscendC::Std::is_const<MyStruct>::value);
AscendC::printf("AscendC::Std::is_const::value:%d\n", AscendC::Std::is_const<void>::value);

// Matching scenario
AscendC::printf("AscendC::Std::is_const::value:%d\n", AscendC::Std::is_const<const int>::value);
AscendC::printf("AscendC::Std::is_const::value:%d\n", AscendC::Std::is_const<int const>::value);
AscendC::printf("AscendC::Std::is_const::value:%d\n", AscendC::Std::is_const<int* const>::value);
AscendC::printf("AscendC::Std::is_const::value:%d\n", AscendC::Std::is_const<const int[5]>::value);
AscendC::printf("AscendC::Std::is_const::value:%d\n", AscendC::Std::is_const<const volatile double>::value);
AscendC::printf("AscendC::Std::is_const::value:%d\n", AscendC::Std::is_const<const MyStruct>::value);
```

```
// Execution result:
AscendC::Std::is_const::value:0
AscendC::Std::is_const::value:0
AscendC::Std::is_const::value:0
AscendC::Std::is_const::value:0
AscendC::Std::is_const::value:0
AscendC::Std::is_const::value:0
AscendC::Std::is_const::value:0
AscendC::Std::is_const::value:0
AscendC::Std::is_const::value:0
AscendC::Std::is_const::value:1
AscendC::Std::is_const::value:1
AscendC::Std::is_const::value:1
AscendC::Std::is_const::value:1
AscendC::Std::is_const::value:1
AscendC::Std::is_const::value:1
```
