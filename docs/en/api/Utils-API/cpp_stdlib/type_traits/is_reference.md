# is\_reference

<!-- md-trans-meta sourceCommit=c17b32aa8d725bcb43135ab25add5a26d62ad5c0 translatedAt=2026-09-06T12:13:09.238Z pushedAt=2026-09-11T04:50:49.847Z -->

## Applicable Products

- Ascend 950PR/Ascend 950DT: supported
- Atlas A3 training products/Atlas A3 inference products: supported
- Atlas A2 training products/Atlas A2 inference products: supported
- Atlas 200I/500 A2 inference products: not supported
- Atlas inference products AI Core: not supported
- Atlas inference products Vector Core: not supported
- Atlas training products: not supported

## Description

At compile time, detects whether a type is a reference type, which can be used for type checking and conditional processing at compile time.

## Prototype

```
template <typename Tp>
struct is_reference;
```

## Parameters

**Table 1** Template parameter description

| Parameter | Description |
| --- | --- |
| Tp | The type to be detected, including basic types (such as int and float), composite types (such as arrays and pointers), user-defined types (such as classes and structs), and reference types themselves. |

## Constraints

None

## Return Value

is\_reference's static constant member value is used to obtain the returned Boolean value, and is\_reference<Tp\>::value takes the following values:

-   true: **Tp** is a reference type.
-   false: **Tp** is not a reference type.

## Example

```
// Custom test type.
struct MyStruct{int val;};
// Function type.
using FuncType = void(int);

// Legitimate reference type
AscendC::printf("AscendC::Std::is_reference::value:%d\n", AscendC::Std::is_reference<int&>::value);
AscendC::printf("AscendC::Std::is_reference::value:%d\n", AscendC::Std::is_reference<int&&>::value);
AscendC::printf("AscendC::Std::is_reference::value:%d\n", AscendC::Std::is_reference<int(&)[5]>::value);
AscendC::printf("AscendC::Std::is_reference::value:%d\n", AscendC::Std::is_reference<int*&>::value);
AscendC::printf("AscendC::Std::is_reference::value:%d\n", AscendC::Std::is_reference<MyStruct&>::value);

// CV restricted reference types
AscendC::printf("AscendC::Std::is_reference::value:%d\n", AscendC::Std::is_reference<const int&>::value);
AscendC::printf("AscendC::Std::is_reference::value:%d\n", AscendC::Std::is_reference<volatile double&&>::value);
AscendC::printf("AscendC::Std::is_reference::value:%d\n", AscendC::Std::is_reference<const MyStruct&>::value);

// non-reference type
AscendC::printf("AscendC::Std::is_reference::value:%d\n", AscendC::Std::is_reference<int>::value);
AscendC::printf("AscendC::Std::is_reference::value:%d\n", AscendC::Std::is_reference<int*>::value);
AscendC::printf("AscendC::Std::is_reference::value:%d\n", AscendC::Std::is_reference<int[5]>::value);
AscendC::printf("AscendC::Std::is_reference::value:%d\n", AscendC::Std::is_reference<double>::value);
AscendC::printf("AscendC::Std::is_reference::value:%d\n", AscendC::Std::is_reference<MyStruct>::value);
AscendC::printf("AscendC::Std::is_reference::value:%d\n", AscendC::Std::is_reference<FuncType>::value);
AscendC::printf("AscendC::Std::is_reference::value:%d\n", AscendC::Std::is_reference<void>::value);
```

```
// Execution result:
AscendC::Std::is_reference::value:1
AscendC::Std::is_reference::value:1
AscendC::Std::is_reference::value:1
AscendC::Std::is_reference::value:1
AscendC::Std::is_reference::value:1
AscendC::Std::is_reference::value:1
AscendC::Std::is_reference::value:1
AscendC::Std::is_reference::value:1
AscendC::Std::is_reference::value:0
AscendC::Std::is_reference::value:0
AscendC::Std::is_reference::value:0
AscendC::Std::is_reference::value:0
AscendC::Std::is_reference::value:0
AscendC::Std::is_reference::value:0
AscendC::Std::is_reference::value:0
```

