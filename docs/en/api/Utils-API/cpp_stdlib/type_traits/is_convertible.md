# is\_convertible

<!-- md-trans-meta sourceCommit=22e1e8c65e425d4c0fb07294311f2eac049ab0b3 translatedAt=2026-11-10T10:21:43.445Z pushedAt=2026-11-13T08:46:49.235Z nl=2 -->

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
<!-- @ref: asc-devkit/res/docs/zh/api/Utils-API/cpp_stdlib/type_traits/is_convertible_res.md#id1 -->

## Description

is\_convertible is a type conversion checking utility defined in the <type\_traits\> header. It provides a mechanism for checking type conversion at compile time: it determines whether an implicit conversion is possible between two types and returns the result as a Boolean value. This API can be used in scenarios such as template metaprogramming, function overload resolution, and static assertions to catch potential type conversion errors during compilation and avoid runtime errors.

## Prototype

```
template <typename From, typename To>
struct is_convertible;
```

## Parameters

**Table 1** Template parameters

| Parameter | Description |
| --- | --- |
| From | Source type, that is, the original type to be converted. |
| To | Target type, that is, the type to convert to. |

## Constraints

Neither the source type nor the target type supports abstract classes or polymorphic types.

## Return Value

The static constant member value of is\_convertible is used to obtain the returned Boolean value. The values of is\_convertible<From, To\>::value are as follows:

-   true: An object of type From can be implicitly converted to type To.
-   false: An object of type From cannot be implicitly converted to type To.

## Examples

```
class Base {};
class Derived : public Base {};
class Unrelated {};

// Check whether int can be implicitly converted to double.
AscendC::PRINTF("Is int convertible to double? %d\n", AscendC::Std::is_convertible<int, double>::value);

// Check whether double can be implicitly converted to int.
AscendC::PRINTF("Is double convertible to int? %d\n", AscendC::Std::is_convertible<double, int>::value);

// Check whether Derived can be converted to Base.
AscendC::PRINTF("Is Derived callable with Base? %d\n", AscendC::Std::is_convertible<Derived, Base>::value);
// Check whether Base can be converted to Derived.
AscendC::PRINTF("Is Base callable with Derived? %d\n", AscendC::Std::is_convertible<Base, Derived>::value);
// Check whether Derived can be converted to Unrelated.
AscendC::PRINTF("Is Derived callable with Unrelated? %d\n", AscendC::Std::is_convertible<Derived, Unrelated>::value);
```

```
// Execution result:
Is int convertible to double? 1
Is double convertible to int? 1
Is Derived callable with Base? 1
Is Base callable with Derived? 0
Is Derived callable with Unrelated? 0
```
