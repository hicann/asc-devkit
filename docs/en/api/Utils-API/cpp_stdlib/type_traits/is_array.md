# is\_array

<!-- md-trans-meta sourceCommit=c17b32aa8d725bcb43135ab25add5a26d62ad5c0 translatedAt=2026-09-06T12:01:15.643Z pushedAt=2026-09-11T04:50:27.586Z -->

## Applicable Products

- Ascend 950PR/Ascend 950DT: supported
- Atlas A3 training products/Atlas A3 inference products: supported
- Atlas A2 training products/Atlas A2 inference products: supported
- Atlas 200I/500 A2 inference products: not supported
- Atlas inference products AI Core: not supported
- Atlas inference products Vector Core: not supported
- Atlas training products: not supported

## Description

At compile time, detects whether a type is an array type, which can be used for compile-time type checking and conditional processing.

## Prototype

```
template <typename Tp>
struct is_array;
```

## Parameters

**Table 1**  Template parameter description

| Parameter | Description |
| --- | --- |
| Tp | Type to be detected, including basic types (such as int and float), composite types (such as pointers and references), user-defined types (such as classes and structs), and the array type itself. |

## Constraints

None

## Return Value

is\_array's static constant member **value** is used to obtain the returned Boolean value, and is\_array<Tp\>::value takes the following values:

-   **true**: **Tp** is an array type.
-   **false**: **Tp** is not an array type.

## Example

```
// Custom test type.
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

