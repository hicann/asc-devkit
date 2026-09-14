# is\_base\_of

<!-- md-trans-meta sourceCommit=2737c34bbaf046b0c4e915117e38247d0f621f63 translatedAt=2026-09-06T12:03:15.091Z pushedAt=2026-09-11T04:50:30.017Z -->

## Applicable Products

- Ascend 950PR/Ascend 950DT: supported
- Atlas A3 training products/Atlas A3 inference products: supported
- Atlas A2 training products/Atlas A2 inference products: supported
- Atlas 200I/500 A2 inference products: not supported
- Atlas inference products AI Core: not supported
- Atlas inference products Vector Core: not supported
- Atlas training products: not supported

## Description

is\_base\_of is a type trait defined in the <type\_traits\> header file. It checks at compile time whether one type is a base class of another type. This API can be used in scenarios such as template metaprogramming, type checking, and conditional compilation to catch potential type errors at the compilation stage and improve code robustness.

## Prototype

```
template <typename Base, typename Derived>
struct is_base_of;
```

## Parameters

**Table 1**  Template parameter description

| Name | Description |
| --- | --- |
| Base | Base class type to be checked, that is, whether the **Base** type is the base class of the **Derived** type. |
| Derived | Derived class type to be checked, that is, whether the **Base** type is the base class of the **Derived** type. |

## Constraints

None

## Return Value

The static constant member **value** of is\_base\_of is used to obtain the returned Boolean value. is\_base\_of<Base, Derived\>::value takes the following values:

-   **true**: The **Base** type is a base class of the **Derived** type (including the case where the **Base** type and the **Derived** type are the same type).
-   **false**: The **Base** type is not a base class of the **Derived** type.

## Example

```
class Base {};
class Derived : public Base {};
class Unrelated {};

// Derived class with virtual inheritance.
class Derived2 : virtual public Base {};

// Define a derived class with virtual inheritance.
class VirtualDerived : virtual public Base {};

// Define a derived class with multiple inheritance.
class MultiDerived : public Base, public VirtualDerived {};

// Template base class.
template <typename T>
class BaseTemplate {
public:
    T value;
};

// Template derived class.
template <typename T>
class DerivedTemplate : public BaseTemplate<T> {};

// Check whether Base is a base class of Derived.
AscendC::PRINTF("Is Base a base of Derived? %d\n" , AscendC::Std::is_base_of<Base, Derived>::value);

// Check whether Derived is a base class of Base (should be false).
AscendC::PRINTF("Is Derived a base of Base? %d\n" , AscendC::Std::is_base_of<Derived, Base>::value);

// Check whether Base is a base class of Unrelated (should be false).
AscendC::PRINTF("Is Base a base of Unrelated? %d\n" , AscendC::Std::is_base_of<Base, Unrelated>::value);

AscendC::PRINTF("Is Base a base of Derived (virtual inheritance)? %d\n", AscendC::Std::is_base_of<Base, Derived2>::value);

AscendC::PRINTF("Is BaseTemplate<int> a base of DerivedTemplate<int>? %d\n", AscendC::Std::is_base_of<BaseTemplate<int>, DerivedTemplate<int>>::value);

// Test whether Base is a base class of VirtualDerived (virtual inheritance case).
AscendC::PRINTF("Is Base a base of VirtualDerived? %d\n" , AscendC::Std::is_base_of<Base, VirtualDerived>::value);
// Test whether Base is a base class of MultiDerived (multiple inheritance case).
AscendC::PRINTF("Is Base a base of MultiDerived? %d\n" , AscendC::Std::is_base_of<Base, MultiDerived>::value);
```

```
// Execution result:
Is Base a base of Derived? 1
Is Derived a base of Base? 0
Is Base a base of Unrelated? 0
Is Base a base of Derived (virtual inheritance)? 1
Is BaseTemplate<int> a base of DerivedTemplate<int>? 1
Is Base a base of VirtualDerived? 1
Is Base a base of MultiDerived? 1
```

