# is\_pointer

<!-- md-trans-meta sourceCommit=22e1e8c65e425d4c0fb07294311f2eac049ab0b3 translatedAt=2026-11-10T10:23:38.408Z pushedAt=2026-11-13T08:46:49.240Z nl=2 -->

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
<!-- @ref: asc-devkit/res/docs/zh/api/Utils-API/cpp_stdlib/type_traits/is_pointer_res.md#id1 -->

## Description

Determines whether a type is a pointer type at compile time, which can be used for compile-time type checking and conditional processing.

## Prototype

```
template <typename Tp>
struct is_pointer;
```

## Parameters

**Table 1** Template parameters

| Parameter | Description |
| --- | --- |
| Tp | Type to be checked, including basic types (such as int and float), composite types (such as arrays and function types), user-defined types (such as classes and structures), and pointer types themselves. |

## Constraints

None

## Return Value

The static constant member value of is\_pointer is used to obtain the returned Boolean value. is\_pointer<Tp\>::value takes the following values:

-   true: Tp is a pointer type.
-   false: Tp is not a pointer type.

## Examples

```
// Custom test type.
struct MyStruct{int val;};
// Function type.
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
