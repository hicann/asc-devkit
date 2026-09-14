# is\_void

<!-- md-trans-meta sourceCommit=c17b32aa8d725bcb43135ab25add5a26d62ad5c0 translatedAt=2026-09-06T12:16:29.447Z pushedAt=2026-09-11T04:51:54.622Z -->

## Applicable Products

- Ascend 950PR/Ascend 950DT: supported
- Atlas A3 training products/Atlas A3 inference products: supported
- Atlas A2 training products/Atlas A2 inference products: supported
- Atlas 200I/500 A2 inference products: not supported
- Atlas inference products AI Core: not supported
- Atlas inference products Vector Core: not supported
- Atlas training products: not supported

## Description

At compile time, detects whether a type is the void type, which can be used for type checking and conditional processing at compile time.

## Prototype

```
template <typename Tp>
struct is_void;
```

## Parameters

**Table 1** Template parameter description

| Parameter | Description |
| --- | --- |
| **Tp** | Type to be detected, including basic data types, composite data types, modified types, template classes, and user-defined types. |

## Constraints

None

## Return Value

The static constant member **value** of is\_void is used to obtain the returned Boolean value. The values of is\_void<Tp\>::value are as follows:

-   true: **Tp** is of type **void**.
-   false: **Tp** is not of type **void**.

## Example

```
// Define a custom test type.
struct MyStruct{};

// Function of void type.
using FuncType = void(int);

// Mismatch scenario
AscendC::printf("AscendC::Std::is_void::value:%d\n", AscendC::Std::is_void<int>::value);
AscendC::printf("AscendC::Std::is_void::value:%d\n", AscendC::Std::is_void<void*>::value);
AscendC::printf("AscendC::Std::is_void::value:%d\n", AscendC::Std::is_void<FuncType>::value);
AscendC::printf("AscendC::Std::is_void::value:%d\n", AscendC::Std::is_void<MyStruct>::value);

// Matching scenario
AscendC::printf("AscendC::Std::is_void::value:%d\n", AscendC::Std::is_void<void>::value);
AscendC::printf("AscendC::Std::is_void::value:%d\n", AscendC::Std::is_void<const void>::value);
AscendC::printf("AscendC::Std::is_void::value:%d\n", AscendC::Std::is_void<volatile void>::value);
AscendC::printf("AscendC::Std::is_void::value:%d\n", AscendC::Std::is_void<const volatile void>::value);
```

```
// Execution result:
AscendC::Std::is_void::value:0
AscendC::Std::is_void::value:0
AscendC::Std::is_void::value:0
AscendC::Std::is_void::value:0
AscendC::Std::is_void::value:1
AscendC::Std::is_void::value:1
AscendC::Std::is_void::value:1
AscendC::Std::is_void::value:1
```

