# is\_floating\_point

<!-- md-trans-meta sourceCommit=22e1e8c65e425d4c0fb07294311f2eac049ab0b3 translatedAt=2026-11-10T10:22:22.009Z pushedAt=2026-11-13T08:46:49.233Z nl=2 -->

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
<!-- @ref: asc-devkit/res/docs/zh/api/Utils-API/cpp_stdlib/type_traits/is_floating_point_res.md#id1 -->

## Description

Detects whether a type is a floating-point type at compile time, which can be used for compile-time type checking and conditional processing.

## Prototype

```
template <typename T>
struct is_floating_point;
```

## Parameters

**Table 1** Template parameters

| Parameter | Description |
| --- | --- |
| T | Type to be detected, including basic data types, modified types, and so on. |

## Constraints

None

## Return Value

The static constant member value of is\_floating\_point is used to obtain the returned Boolean value. is\_floating\_point<Tp\>::value takes the following values:

-   true: Tp is a floating-point type.
-   false: Tp is not a floating-point type.

## Examples

```
// Custom test type.
struct MyStruct{};

// Function type whose return value is a floating-point type.
using FuncType = double(int);

// Legal floating point type
AscendC::printf("AscendC::Std::is_floating_point::value:%d\n", AscendC::Std::is_floating_point<float>::value);
AscendC::printf("AscendC::Std::is_floating_point::value:%d\n", AscendC::Std::is_floating_point<double>::value);
AscendC::printf("AscendC::Std::is_floating_point::value:%d\n", AscendC::Std::is_floating_point<long double>::value);

// Floating point types limited by CV
AscendC::printf("AscendC::Std::is_floating_point::value:%d\n", AscendC::Std::is_floating_point<const float>::value);
AscendC::printf("AscendC::Std::is_floating_point::value:%d\n", AscendC::Std::is_floating_point<volatile double>::value);

// non-floating point type
AscendC::printf("AscendC::Std::is_floating_point::value:%d\n", AscendC::Std::is_floating_point<int>::value);
AscendC::printf("AscendC::Std::is_floating_point::value:%d\n", AscendC::Std::is_floating_point<bool>::value);
AscendC::printf("AscendC::Std::is_floating_point::value:%d\n", AscendC::Std::is_floating_point<double*>::value);
AscendC::printf("AscendC::Std::is_floating_point::value:%d\n", AscendC::Std::is_floating_point<float&>::value);
AscendC::printf("AscendC::Std::is_floating_point::value:%d\n", AscendC::Std::is_floating_point<double[5]>::value);
AscendC::printf("AscendC::Std::is_floating_point::value:%d\n", AscendC::Std::is_floating_point<FuncType>::value);
AscendC::printf("AscendC::Std::is_floating_point::value:%d\n", AscendC::Std::is_floating_point<MyStruct>::value);
AscendC::printf("AscendC::Std::is_floating_point::value:%d\n", AscendC::Std::is_floating_point<void>::value);
```

```
// Execution result:
AscendC::Std::is_floating_point::value:1
AscendC::Std::is_floating_point::value:1
AscendC::Std::is_floating_point::value:1
AscendC::Std::is_floating_point::value:1
AscendC::Std::is_floating_point::value:1
AscendC::Std::is_floating_point::value:0
AscendC::Std::is_floating_point::value:0
AscendC::Std::is_floating_point::value:0
AscendC::Std::is_floating_point::value:0
AscendC::Std::is_floating_point::value:0
AscendC::Std::is_floating_point::value:0
AscendC::Std::is_floating_point::value:0
AscendC::Std::is_floating_point::value:0
```
