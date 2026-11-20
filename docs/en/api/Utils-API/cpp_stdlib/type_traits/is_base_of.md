# is\_base\_of

<!-- md-trans-meta sourceCommit=22e1e8c65e425d4c0fb07294311f2eac049ab0b3 translatedAt=2026-11-10T10:19:41.510Z pushedAt=2026-11-13T08:46:49.230Z nl=2 -->

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
<!-- @ref: asc-devkit/res/docs/zh/api/Utils-API/cpp_stdlib/type_traits/is_base_of_res.md#id1 -->

## Description

is\_base\_of is a type trait utility defined in the <type\_traits\> header file. It checks at compile time whether one type is a base class of another type. This API applies to scenarios such as template metaprogramming, type checking, and conditional compilation, helping you catch potential type errors at the compilation stage and improve code robustness.

## Prototype

```
template <typename Base, typename Derived>
struct is_base_of;
```

## Parameters

**Table 1** Template parameters

| Parameter | Description |
| --- | --- |
| Base | Base class type to check, that is, whether the Base type is a base class of the Derived type. |
| Derived | Derived class type to check, that is, whether the Base type is a base class of the Derived type. |

## Constraints

None

## Return Value

The static constant member value of is\_base\_of is used to obtain the returned Boolean value. The values of is\_base\_of<Base, Derived\>::value are as follows:

-   true: The Base type is a base class of the Derived type (including the case where the Base type and the Derived type are the same type).
-   false: The Base type is not a base class of the Derived type.

## Examples

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
