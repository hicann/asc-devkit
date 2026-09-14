# is\_pointer

<!-- md-trans-meta sourceCommit=c17b32aa8d725bcb43135ab25add5a26d62ad5c0 translatedAt=2026-09-06T12:11:36.409Z pushedAt=2026-09-11T04:50:45.086Z -->

## Applicable Products

- Ascend 950PR/Ascend 950DT: supported
- Atlas A3 training products/Atlas A3 inference products: supported
- Atlas A2 training products/Atlas A2 inference products: supported
- Atlas 200I/500 A2 inference products: not supported
- Atlas inference products AI Core: not supported
- Atlas inference products Vector Core: not supported
- Atlas training products: not supported

## Description

Determines whether a type is a pointer type at compile time, which can be used for compile-time type checking and conditional processing.

## Prototype

```
template <typename Tp>
struct is_pointer;
```

## Parameters

**Table 1** Template parameter description

| Parameter | Description |
| --- | --- |
| Tp | Type to be detected, including basic types (such as int and float), composite types (such as arrays and function types), user-defined types (such as classes and structs), and pointer types themselves. |

## Constraints

None

## Return Value

The static constant member **value** of is\_pointer is used to obtain the returned Boolean value. The values of is\_pointer<Tp\>::value are as follows:

-   true: **Tp** is a pointer type.
-   false: **Tp** is not a pointer type.

## Example

```
// Custom test type
struct MyStruct{int val;};
// Function type
using FuncType = void(int);

// Legitimate pointer types
AscendC::printf("AscendC::Std::is_pointer::value:%d\n", AscendC::Std::is_pointer<int*>::value);
AscendC::printf("AscendC::Std::is_pointer::value:%d\n", AscendC::Std::is_pointer<char**>::value);
AscendC::printf("AscendC::Std::is_pointer::value:%d\n", AscendC::Std::is_pointer<void*>::value);
AscendC::printf("AscendC::Std::is_pointer::value:%d\n", AscendC::Std::is_pointer<MyStruct*>::value);
AscendC::printf("AscendC::Std::is_pointer::value:%d\n", AscendC::Std::is_pointer<int(*)[5]>::value);
AscendC::printf("AscendC::Std::is_pointer::value:%d\n", AscendC::Std::is_pointer<void(*)(int)>::value);

// Pointer types limited by CV
AscendC::printf("AscendC::Std::is_pointer::value:%d\n", AscendC::Std::is_pointer<const int*>::value);
AscendC::printf("AscendC::Std::is_pointer::value:%d\n", AscendC::Std::is_pointer<int* const>::value);
AscendC::printf("AscendC::Std::is_pointer::value:%d\n", AscendC::Std::is_pointer<volatile char*>::value);

// non-pointer types
AscendC::printf("AscendC::Std::is_pointer::value:%d\n", AscendC::Std::is_pointer<int>::value);
AscendC::printf("AscendC::Std::is_pointer::value:%d\n", AscendC::Std::is_pointer<int&>::value);
AscendC::printf("AscendC::Std::is_pointer::value:%d\n", AscendC::Std::is_pointer<int[5]>::value);
AscendC::printf("AscendC::Std::is_pointer::value:%d\n", AscendC::Std::is_pointer<double>::value);
AscendC::printf("AscendC::Std::is_pointer::value:%d\n", AscendC::Std::is_pointer<MyStruct>::value);
AscendC::printf("AscendC::Std::is_pointer::value:%d\n", AscendC::Std::is_pointer<FuncType>::value);
AscendC::printf("AscendC::Std::is_pointer::value:%d\n", AscendC::Std::is_pointer<void>::value);
```

```
// Execution result:
AscendC::Std::is_pointer::value:1
AscendC::Std::is_pointer::value:1
AscendC::Std::is_pointer::value:1
AscendC::Std::is_pointer::value:1
AscendC::Std::is_pointer::value:1
AscendC::Std::is_pointer::value:1
AscendC::Std::is_pointer::value:1
AscendC::Std::is_pointer::value:1
AscendC::Std::is_pointer::value:1
AscendC::Std::is_pointer::value:0
AscendC::Std::is_pointer::value:0
AscendC::Std::is_pointer::value:0
AscendC::Std::is_pointer::value:0
AscendC::Std::is_pointer::value:0
AscendC::Std::is_pointer::value:0
AscendC::Std::is_pointer::value:0
```

