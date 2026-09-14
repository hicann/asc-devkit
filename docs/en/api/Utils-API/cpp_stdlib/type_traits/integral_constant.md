# integral\_constant

<!-- md-trans-meta sourceCommit=2737c34bbaf046b0c4e915117e38247d0f621f63 translatedAt=2026-09-06T11:59:05.277Z pushedAt=2026-09-11T04:50:25.239Z -->

## Applicable Products

- Ascend 950PR/Ascend 950DT: supported
- Atlas A3 training products/Atlas A3 inference products: supported
- Atlas A2 training products/Atlas A2 inference products: supported
- Atlas 200I/500 A2 inference products: not supported
- Atlas inference products AI Core: not supported
- Atlas inference products Vector Core: not supported
- Atlas training products: not supported

## Description

**integral_constant** is a structure with a template parameter, defined in the <type\_traits\> header file, used to encapsulate a compile-time constant integer value. It is a fundamental component for many type traits and compile-time computations in the standard library.

The functions of **integral_constant** are as follows:

1.  Encapsulate a compile-time constant by wrapping an int or bool value into a specific type, so that the value can be manipulated and passed at compile time.
2.  Type identifier: each distinct integral\_constant instance is a unique type, which can be used for template specialization or overload resolution.
3.  Implicit conversion to the wrapped value type is supported, allowing the instance to be used directly in contexts where the value is required.
4.  The function call operator allows an instance to be called like a function to obtain its value.

integral\_constant provides several commonly used specialized versions, as follows:

-   Std::true\_type: an alias of integral\_constant<bool, true\>.
-   Std::false\_type: an alias of integral\_constant<bool, false\>.
-   Numeric constants: such as Std::integral\_constant<int, 42\>.
-   A simplified form of numeric constants, **Std::Int**, whose numeric type is size\_t: such as Std::Int<42\>.

## Prototype

```
template <typename Tp, Tp v>
struct integral_constant
{
    static constexpr const Tp value = v;
    using value_type = Tp;
    using type = integral_constant;
    inline constexpr operator value_type() const noexcept;
    inline constexpr value_type operator()() const noexcept;
};

template <size_t v>
using Int = integral_constant<size_t, v>;
```

## Parameters

**Table 1**  Template parameter description

| Parameter | Description |
| --- | --- |
| **Tp** | Data type of the value. |
| **v** | Constant value. |

## Constraints

-   The **Int** type is an alias shorthand for the integral\_constant numeric structure, and the numeric type must be the size\_t type.
-   The template parameter **Tp** does not support floating-point types such as **float**, because template parameters must be determined at compile time, and the precision issues of floating-point numbers may prevent accurate representation at compile time.

## Return Value

None

## Example

-   Numeric type encapsulation

    ```
    // The following example is a UT example based on googletest.
    using IntTrue = AscendC::Std::integral_constant<int, 1>;
    using IntFalse = AscendC::Std::integral_constant<int, 0>;
    // Test the value static constant.
    EXPECT_EQ(IntTrue::value, 1);
    EXPECT_EQ(IntFalse::value, 0);
    // Test the () operator overload.
    EXPECT_EQ(IntTrue()(), 1);
    EXPECT_EQ(IntFalse()(), 0);
    // Test the type definition.
    EXPECT_TRUE((AscendC::Std::is_same<typename IntTrue::value_type, int>::value));
    EXPECT_TRUE((AscendC::Std::is_same<typename IntTrue::type, IntTrue>::value));
    ```

-   Specialized type: bool

    ```
    // The following example is a UT example based on googletest.
    using TrueType = AscendC::Std::true_type;
    using FalseType = AscendC::Std::false_type;
    // Test the value static constant.
    EXPECT_TRUE(TrueType::value);
    EXPECT_FALSE(FalseType::value);
    // Test the () operator overload.
    EXPECT_TRUE(TrueType()());
    EXPECT_FALSE(FalseType()());
    // Test the type definition.
    EXPECT_TRUE((AscendC::Std::is_same<typename TrueType::value_type, bool>::value));
    EXPECT_TRUE((AscendC::Std::is_same<typename TrueType::type, TrueType>::value));
    EXPECT_TRUE((AscendC::Std::is_same<typename FalseType::type, FalseType>::value));
    ```

-   Specialized type: Int

    ```
    // The following example is a UT example based on googletest.
    using Zero = AscendC::Std::Int<0>;
    using One = AscendC::Std::Int<1>;
    using Large = AscendC::Std::Int<0xFFFFFFFF>;
    // Verify the value static constant.
    EXPECT_EQ(Zero::value, 0);
    EXPECT_EQ(One::value, 1);
    EXPECT_EQ(Large::value, 0xFFFFFFFF);
    // Verify the type definition.
    EXPECT_TRUE((AscendC::Std::is_same<typename Zero::value_type, size_t>::value));
    EXPECT_TRUE((AscendC::Std::is_same<typename Zero::type, Zero>::value));
    EXPECT_TRUE((AscendC::Std::is_same<Zero, AscendC::Std::integral_constant<size_t, 0>>::value));
    // Verify the () operator overload.
    EXPECT_EQ(Zero()(), 0);
    EXPECT_EQ(One()(), 1);
    EXPECT_EQ(Large()(), 0xFFFFFFFF);
    ```

-   Operations on the Int specialized type

    ```
    // Addition.
    static_assert((AscendC::Std::Int<5>::value + AscendC::Std::Int<3>::value) == 8, "Addition failed");
    // Multiplication.
    static_assert((AscendC::Std::Int<4>::value * AscendC::Std::Int<6>::value) == 24, "Multiplication failed");
    // Comparison.
    static_assert(AscendC::Std::Int<10>::value > AscendC::Std::Int<5>::value, "Comparison failed");
    static_assert(AscendC::Std::Int<7>::value != AscendC::Std::Int<77>::value, "Equality check failed");
    ```

