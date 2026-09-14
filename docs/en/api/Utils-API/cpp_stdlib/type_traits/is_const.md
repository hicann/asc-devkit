# is\_const

<!-- md-trans-meta sourceCommit=c17b32aa8d725bcb43135ab25add5a26d62ad5c0 translatedAt=2026-09-06T12:04:56.649Z pushedAt=2026-09-11T04:50:32.252Z -->

## Applicable Products

- Ascend 950PR/Ascend 950DT: supported
- Atlas A3 training products/Atlas A3 inference products: supported
- Atlas A2 training products/Atlas A2 inference products: supported
- Atlas 200I/500 A2 inference products: not supported
- Atlas inference products AI Core: not supported
- Atlas inference products Vector Core: not supported
- Atlas training products: not supported

## Description

At compile time, detects whether a type is a const-qualified type, which can be used for type checking and conditional handling at compile time.

## Prototype

```
template <typename Tp>
struct is_const;
```

## Parameters

**Table 1** Template parameter description

| Parameter | Description |
| --- | --- |
| Tp | Type to be detected, including fundamental types (such as int and float), compound types (such as arrays, pointers, and references), user-defined types (such as classes and structs), and const-qualified types themselves. |

## Constraints

None

## Return Value

The static constant member value of is_const is used to obtain the returned Boolean value, and is_const<Tp>::value takes the following values:

-   true: **Tp** is a const type.
-   false: **Tp** is not a const type.

## Example

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

