# is\_convertible

<!-- md-trans-meta sourceCommit=2737c34bbaf046b0c4e915117e38247d0f621f63 translatedAt=2026-09-06T12:06:41.156Z pushedAt=2026-09-11T07:34:50.688Z -->

## Applicable Products

- Ascend 950PR/Ascend 950DT: supported
- Atlas A3 training products/Atlas A3 inference products: supported
- Atlas A2 training products/Atlas A2 inference products: supported
- Atlas 200I/500 A2 inference products: not supported
- Atlas inference products AI Core: not supported
- Atlas inference products Vector Core: not supported
- Atlas training products: not supported

## Description

is\_convertible is a type conversion check tool defined in the <type\_traits\> header file. It provides a mechanism for performing type conversion checks at compile time: determining whether an implicit conversion is possible between two types and returning the result as a Boolean value. This API can be applied in scenarios such as template metaprogramming, function overload resolution, and static assertion to catch potential type conversion errors during the compilation phase and avoid runtime errors.

## Prototype

```
template <typename From, typename To>
struct is_convertible;
```

## Parameters

**Table 1** Template parameter description

| Parameter | Description |
| --- | --- |
| From | Source type, that is, the original type to be converted. |
| To | Target type, that is, the target type to convert to. |

## Constraints

Neither the source type nor the target type supports abstract classes or polymorphic types.

## Return Value

is\_convertible's static constant member value is used to obtain the returned Boolean value. is\_convertible<From, To\>::value takes the following values:

-   true: An object of the From type can be implicitly converted to the To type.
-   false: An object of the From type cannot be implicitly converted to the To type.

## Example

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

