# is\_reference

<!-- md-trans-meta sourceCommit=22e1e8c65e425d4c0fb07294311f2eac049ab0b3 translatedAt=2026-11-10T10:24:46.623Z pushedAt=2026-11-13T08:46:49.242Z nl=2 -->

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
<!-- @ref: asc-devkit/res/docs/zh/api/Utils-API/cpp_stdlib/type_traits/is_reference_res.md#id1 -->

## Description

At compile time, this template detects whether a type is a reference type, which can be used for compile-time type checking and conditional processing.

## Prototype

```
template <typename Tp>
struct is_reference;
```

## Parameters

**Table 1** Template parameters

| Parameter | Description |
| --- | --- |
| Tp | Type to be detected, including basic types (such as int and float), compound types (such as arrays and pointers), user-defined types (such as classes and structures), and reference types themselves. |

## Constraints

None

## Return Value

The static constant member value of is\_reference is used to obtain the returned Boolean value. The values of is\_reference<Tp\>::value are as follows:

-   true: Tp is a reference type.
-   false: Tp is not a reference type.

## Examples

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
