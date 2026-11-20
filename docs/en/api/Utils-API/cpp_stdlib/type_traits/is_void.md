# is\_void

<!-- md-trans-meta sourceCommit=22e1e8c65e425d4c0fb07294311f2eac049ab0b3 translatedAt=2026-11-10T10:25:52.332Z pushedAt=2026-11-13T08:46:49.245Z nl=2 -->

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
<!-- @ref: asc-devkit/res/docs/zh/api/Utils-API/cpp_stdlib/type_traits/is_void_res.md#id1 -->

## Description

Checks whether a type is the void type at compile time, which can be used for compile-time type checking and conditional processing.

## Prototype

```
template <typename Tp>
struct is_void;
```

## Parameters

**Table 1** Template parameters

| Parameter | Description |
| --- | --- |
| Tp | Type to be checked, including basic data types, composite data types, modified types, template classes, user-defined types, and so on. |

## Constraints

None

## Return Value

The static constant member value of is\_void is used to obtain the returned Boolean value. is\_void<Tp\>::value takes the following values:

-   true: Tp is of the void type.
-   false: Tp is not of the void type.

## Examples

```
// Custom test type.
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
