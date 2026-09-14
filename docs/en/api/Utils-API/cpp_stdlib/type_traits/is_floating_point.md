# is\_floating\_point

<!-- md-trans-meta sourceCommit=c17b32aa8d725bcb43135ab25add5a26d62ad5c0 translatedAt=2026-09-06T12:08:22.877Z pushedAt=2026-09-11T04:50:36.233Z -->

## Applicable Products

- Ascend 950PR/Ascend 950DT: supported
- Atlas A3 training products/Atlas A3 inference products: supported
- Atlas A2 training products/Atlas A2 inference products: supported
- Atlas 200I/500 A2 inference products: not supported
- Atlas inference products AI Core: not supported
- Atlas inference products Vector Core: not supported
- Atlas training products: not supported

## Description

At compile time, detects whether a type is a floating-point type, which can be used for type checking and conditional processing at compile time.

## Prototype

```
template <typename T>
struct is_floating_point;
```

## Parameters

**Table 1** Template parameter description

| Parameter | Description |
| --- | --- |
| T | Type to be detected, including basic data types, modified types, and so on. |

## Constraints

None

## Return Value

is\_floating\_point's static constant member **value** is used to obtain the returned Boolean value, and is\_floating\_point<Tp\>::value takes the following values:

-   **true**: **Tp** is a floating-point type.
-   **false**: **Tp** is not a floating-point type.

## Example

```
// Custom test type.
struct MyStruct{};

// Function type whose return value is floating-point.
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

