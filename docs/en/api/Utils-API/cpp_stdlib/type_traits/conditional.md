# conditional

<!-- md-trans-meta sourceCommit=754fb95859dead826727a591e674261c6a829a08 translatedAt=2026-10-23T01:28:14.886Z pushedAt=2026-10-23T02:50:02.008Z nl=2 -->

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
<!-- @ref: asc-devkit/res/docs/zh/api/Utils-API/cpp_stdlib/type_traits/conditional_res.md#id1 -->

## Description

conditional is a type traits utility defined in the <type\_traits\> header file. It selects one of two types at compile time based on a Boolean condition. This API can be used in template metaprogramming to flexibly select the appropriate type according to different conditions, enhancing code reusability and flexibility.

conditional has a nested type member whose value depends on the value of Bp: if Bp is true, conditional<Bp, If, Then\>::type is If. If Bp is false, conditional<Bp, If, Then\>::type is Then.

## Prototype

```
template <bool Bp, typename If, typename Then>
struct conditional;
```

## Parameters

**Table 1** Template parameters

| Parameter | Description |
| --- | --- |
| Bp | A Boolean constant expression used as the condition for selecting the type. |
| If | The type selected when Bp is true. |
| Then | The type selected when Bp is false. |

## Constraints

None

## Return Value

The static constant member type of conditional is used to obtain the return value. The value of conditional<Bp, If, Then\>::type is as follows:

-   If: Bp is true.
-   Then: Bp is false.

## Examples

```
// Define two different types.
struct TypeA {
    __aicore__ inline static void print() {
        AscendC::PRINTF("This is TypeA..\n");
    }
};

struct TypeB {
    __aicore__ inline static void print() {
        AscendC::PRINTF("This is TypeB..\n");
    }
};

// Select the type based on the condition.
template <bool Condition>
__aicore__ inline void selectType() {
    using SelectedType = typename AscendC::Std::conditional<Condition, TypeA, TypeB>::type;
    SelectedType::print();
}

// Define a template function that selects different types based on the condition.
template <bool Condition>
__aicore__ inline void selectOtherType() {
    using SelectedType = typename std::conditional<Condition, int, float>::type;
    if constexpr (std::is_same_v<SelectedType, int>) {
        AscendC::PRINTF("Selected type is int.\n");
    } else {
        AscendC::PRINTF("Selected type is float.\n");
    }
}

// If the condition is true, select TypeA.
selectType<true>();
// If the condition is false, select TypeB.
selectType<false>();

// Test the case where the condition is true.
selectOtherType<true>();
// Test the case where the condition is false.
selectOtherType<false>();
```

```
// Execution result:
This is TypeA..
This is TypeB..
Selected type is int.
Selected type is float.
```
