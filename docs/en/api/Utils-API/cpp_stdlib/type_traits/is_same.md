# is\_same

<!-- md-trans-meta sourceCommit=22e1e8c65e425d4c0fb07294311f2eac049ab0b3 translatedAt=2026-11-10T10:24:47.518Z pushedAt=2026-11-13T08:46:49.243Z nl=2 -->

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
<!-- @ref: asc-devkit/res/docs/zh/api/Utils-API/cpp_stdlib/type_traits/is_same_res.md#id1 -->

## Description

is\_same is a type trait utility defined in the <type\_traits\> header file. It determines at compile time whether two types are exactly the same. This API can be used in scenarios such as template metaprogramming, type checking, and conditional compilation to determine type information at the compilation stage, avoiding type mismatch issues that may occur at runtime.

## Prototype

```
template <typename Tp, typename Up>
struct is_same;
```

## Parameters

**Table 1** Template parameters

| Parameter | Description |
| --- | --- |
| Tp | The first type to be compared for exact sameness with another type. |
| Up | The second type to be compared for exact sameness with another type. |

## Constraints

None

## Return Value

The static constant member value of is\_same is used to obtain the returned Boolean value. The values of is\_same<Tp, Up\>::value are as follows:

-   true: Tp and Up are exactly the same type.
-   false: Tp and Up are not the same type.

## Examples

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

// Compare the same fundamental types.
AscendC::PRINTF("Is int the same as int? %d\n", AscendC::Std::is_same<int, int>::value);

// Compare different fundamental types.
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
