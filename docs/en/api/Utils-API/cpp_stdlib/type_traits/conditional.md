# conditional

<!-- md-trans-meta sourceCommit=2737c34bbaf046b0c4e915117e38247d0f621f63 translatedAt=2026-09-06T11:55:47.009Z pushedAt=2026-09-11T04:50:18.150Z -->

## Applicable Products

- Ascend 950PR/Ascend 950DT: supported
- Atlas A3 training products/Atlas A3 inference products: supported
- Atlas A2 training products/Atlas A2 inference products: supported
- Atlas 200I/500 A2 inference products: not supported
- Atlas inference series products AI Core: not supported
- Atlas inference series products Vector Core: not supported
- Atlas training series products: not supported

## Description

**conditional** is a type trait defined in the <type\_traits\> header file. It selects one of two types based on a boolean condition at compile time. This interface can be used in template metaprogramming to flexibly select the appropriate type according to different conditions, enhancing the generality and flexibility of the code.

**conditional** has a nested **type** member whose value depends on the value of **Bp**: if **Bp** is true, then **conditional<Bp, If, Then\>::type** is **If**. If **Bp** is false, then **conditional<Bp, If, Then\>::type** is **Then**.

## Prototype

```
template <bool Bp, typename If, typename Then>
struct conditional;
```

## Parameters

**Table 1** Template parameter description

| Parameter | Description |
| --- | --- |
| **Bp** | A boolean constant expression used as the condition for selecting the type. |
| **If** | The type selected when **Bp** is true. |
| **Then** | The type selected when **Bp** is false. |

## Constraints

None

## Return Value

The static constant member **type** of **conditional** is used to obtain the return value. The value of **conditional**<**Bp**, **If**, Then\>::type is as follows:

-   **If**: **Bp** is true.
-   **Then**: **Bp** is false.

## Example

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

// Select a type based on the condition.
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

// When the condition is true, select TypeA.
selectType<true>();
// When the condition is false, select TypeB.
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

