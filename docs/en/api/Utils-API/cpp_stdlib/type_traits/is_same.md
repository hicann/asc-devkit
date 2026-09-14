# is\_same

<!-- md-trans-meta sourceCommit=c17b32aa8d725bcb43135ab25add5a26d62ad5c0 translatedAt=2026-09-06T12:14:48.316Z pushedAt=2026-09-11T04:51:47.054Z -->

## Applicable Products

- Ascend 950PR/Ascend 950DT: supported
- Atlas A3 training products/Atlas A3 inference products: supported
- Atlas A2 training products/Atlas A2 inference products: supported
- Atlas 200I/500 A2 inference products: not supported
- Atlas inference products AI Core: not supported
- Atlas inference products Vector Core: not supported
- Atlas training products: Not supported

## Description

is\_same is a type traits utility defined in the <type\_traits\> header file. It determines at compile time whether two types are exactly the same. This API can be used in scenarios such as template metaprogramming, type checking, and conditional compilation to determine type information at the compilation stage, avoiding type mismatch issues that may occur at runtime.

## Prototype

```
template <typename Tp, typename Up>
struct is_same;
```

## Parameters

**Table 1**  Template parameter description

| Parameter | Description |
| --- | --- |
| Tp | The first type to be compared to determine whether the two types are exactly the same. |
| Up | The second type to be compared to determine whether the two types are exactly the same. |

## Constraints

None

## Return Value

The static constant member **value** of is\_same is used to obtain the returned Boolean value. The values of is\_same<Tp, Up\>::value are as follows:

-   true: **Tp** and **Up** are exactly the same type.
-   false: **Tp** and **Up** are not the same type.

## Example

```
// Define two different classes.
class ClassA {};
class ClassB {};

// Define the same class twice.
class ClassC {};
using ClassC_alias = ClassC;

// Define a simple template class.
template <typename T>
class TemplateClass {};

// Compare the same basic types.
AscendC::PRINTF("Is int the same as int? %d\n", AscendC::Std::is_same<int, int>::value);

// Compare different basic types.
AscendC::PRINTF("Is int the same as double? %d\n", AscendC::Std::is_same<int, double>::value);

// Compare different class types.
AscendC::PRINTF("Is ClassA the same as ClassB? %d\n", AscendC::Std::is_same<ClassA, ClassB>::value);

// Compare the same class types.
AscendC::PRINTF("Is ClassC the same as ClassC_alias? %d\n", AscendC::Std::is_same<ClassC, ClassC_alias>::value);

// Compare the same template instantiation types.
AscendC::PRINTF("Is TemplateClass<int> the same as TemplateClass<int>? %d\n", AscendC::Std::is_same<TemplateClass<int>, TemplateClass<int>>::value);

// Compare different template instantiation types.
AscendC::PRINTF("Is TemplateClass<int> the same as TemplateClass<double>? %d\n", AscendC::Std::is_same<TemplateClass<int>, TemplateClass<double>>::value);
```

```
// Execution result:
Is int the same as int? 1
Is int the same as double? 0
Is ClassA the same as ClassB? 0
Is ClassC the same as ClassC_alias? 1
Is TemplateClass<int> the same as TemplateClass<int>? 1
Is TemplateClass<int> the same as TemplateClass<double>? 0
```

