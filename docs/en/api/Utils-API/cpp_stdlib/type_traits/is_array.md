# is\_array

<!-- md-trans-meta sourceCommit=22e1e8c65e425d4c0fb07294311f2eac049ab0b3 translatedAt=2026-11-10T10:19:09.598Z pushedAt=2026-11-13T08:46:49.229Z nl=2 -->

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
<!-- @ref: asc-devkit/res/docs/zh/api/Utils-API/cpp_stdlib/type_traits/is_array_res.md#id1 -->

## Description

At compile time, detects whether a type is an array type, which can be used for compile-time type checking and conditional processing.

## Prototype

```
template <typename Tp>
struct is_array;
```

## Parameters

**Table 1** Template parameters

| Parameter | Description |
| --- | --- |
| Tp | Type to be detected, including basic types (such as int and float), compound types (such as pointers and references), user-defined types (such as classes and structs), and array types themselves. |

## Constraints

None

## Return Value

The static constant member value of is\_array is used to obtain the returned Boolean value. The values of is\_array<Tp\>::value are as follows:

-   true: Tp is an array type.
-   false: Tp is not an array type.

## Examples

```
// Define a custom test type.
struct MyStruct{};
using FuncType = int(int);

// Legitimate array types
AscendC::printf("AscendC::Std::is_array::value:%d\n", AscendC::Std::is_array<int[5]>::value);
AscendC::printf("AscendC::Std::is_array::value:%d\n", AscendC::Std::is_array<char[]>::value);
AscendC::printf("AscendC::Std::is_array::value:%d\n", AscendC::Std::is_array<double[2][3]>::value);

// Array types limited by CV
AscendC::printf("AscendC::Std::is_array::value:%d\n", AscendC::Std::is_array<const int[10]>::value);
AscendC::printf("AscendC::Std::is_array::value:%d\n", AscendC::Std::is_array<volatile char[3]>::value);

// non-array types
AscendC::printf("AscendC::Std::is_array::value:%d\n", AscendC::Std::is_array<int*>::value);
AscendC::printf("AscendC::Std::is_array::value:%d\n", AscendC::Std::is_array<int>::value);
AscendC::printf("AscendC::Std::is_array::value:%d\n", AscendC::Std::is_array<double>::value);
AscendC::printf("AscendC::Std::is_array::value:%d\n", AscendC::Std::is_array<MyStruct>::value);
AscendC::printf("AscendC::Std::is_array::value:%d\n", AscendC::Std::is_array<FuncType>::value);
AscendC::printf("AscendC::Std::is_array::value:%d\n", AscendC::Std::is_array<void>::value);
```

```
// Execution result:
AscendC::Std::is_array::value:1
AscendC::Std::is_array::value:1
AscendC::Std::is_array::value:1
AscendC::Std::is_array::value:1
AscendC::Std::is_array::value:1
AscendC::Std::is_array::value:0
AscendC::Std::is_array::value:0
AscendC::Std::is_array::value:0
AscendC::Std::is_array::value:0
AscendC::Std::is_array::value:0
AscendC::Std::is_array::value:0
```
