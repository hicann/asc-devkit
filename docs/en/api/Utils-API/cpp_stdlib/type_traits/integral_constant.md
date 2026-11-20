# integral\_constant

<!-- md-trans-meta sourceCommit=22e1e8c65e425d4c0fb07294311f2eac049ab0b3 translatedAt=2026-11-10T10:18:21.281Z pushedAt=2026-11-13T08:46:49.227Z nl=2 -->

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
<!-- @ref: asc-devkit/res/docs/zh/api/Utils-API/cpp_stdlib/type_traits/integral_constant_res.md#id1 -->

## Description

integral\_constant is a struct with template parameters, defined in the <type\_traits\> header file. It encapsulates a compile-time constant integer value and serves as the foundational component for many type traits and compile-time computations in the standard library.

integral\_constant provides the following functions:

1.  Encapsulates a compile-time constant by wrapping an int or bool value into a specific type, so that the value can be manipulated and passed at compile time.
2.  Type identification: each distinct integral\_constant instance is a unique type, which can be used for template specialization or overload resolution.
3.  Supports implicit conversion to the wrapped value type, making it convenient to use directly in contexts where the value is required.
4.  The function call operator allows an instance to be invoked like a function to obtain its value.

integral\_constant provides several commonly used specializations, as follows:

-   Std::true\_type: an alias of integral\_constant<bool, true\>.
-   Std::false\_type: an alias of integral\_constant<bool, false\>.
-   Numeric constant: for example, Std::integral\_constant<int, 42\>.
-   Simplified form of a numeric constant, Std::Int, whose numeric type is size\_t: for example, Std::Int<42\>.

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

**Table 1** Template parameters

| Parameter | Description |
| --- | --- |
| Tp | Data type of the value. |
| v | Constant value. |

## Constraints

-   The Int type is a shorthand alias for the integral\_constant numeric structure, and its numeric type must be of type size\_t.
-   The template parameter Tp does not support floating-point types such as float, because template parameters must be determined at compile time, and floating-point precision issues may prevent accurate representation at compile time.

## Return Value

None

## Examples

-   Numeric type encapsulation

    ```
    // The following example is a googletest-based UT example.
    using IntTrue = AscendC::Std::integral_constant<int, 1>;
    using IntFalse = AscendC::Std::integral_constant<int, 0>;
    // Test the value static constant.
    EXPECT_EQ(IntTrue::value, 1);
    EXPECT_EQ(IntFalse::value, 0);
    // Test the operator() overload.
    EXPECT_EQ(IntTrue()(), 1);
    EXPECT_EQ(IntFalse()(), 0);
    // Test the type definitions.
    EXPECT_TRUE((AscendC::Std::is_same<typename IntTrue::value_type, int>::value));
    EXPECT_TRUE((AscendC::Std::is_same<typename IntTrue::type, IntTrue>::value));
    ```

-   Specialized type: bool

    ```
    // The following example is a googletest-based UT example.
    using TrueType = AscendC::Std::true_type;
    using FalseType = AscendC::Std::false_type;
    // Test the value static constant.
    EXPECT_TRUE(TrueType::value);
    EXPECT_FALSE(FalseType::value);
    // Test the operator() overload.
    EXPECT_TRUE(TrueType()());
    EXPECT_FALSE(FalseType()());
    // Test the type definitions.
    EXPECT_TRUE((AscendC::Std::is_same<typename TrueType::value_type, bool>::value));
    EXPECT_TRUE((AscendC::Std::is_same<typename TrueType::type, TrueType>::value));
    EXPECT_TRUE((AscendC::Std::is_same<typename FalseType::type, FalseType>::value));
    ```

-   Specialized type: Int

    ```
    // The following example is a googletest-based UT example.
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
    // Verify the operator() overload.
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
