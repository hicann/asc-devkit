# enable\_if

<!-- md-trans-meta sourceCommit=22e1e8c65e425d4c0fb07294311f2eac049ab0b3 translatedAt=2026-11-10T10:17:57.200Z pushedAt=2026-11-13T08:46:49.225Z nl=2 -->

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
<!-- @ref: asc-devkit/res/docs/zh/api/Utils-API/cpp_stdlib/type_traits/enable_if_res.md#id1 -->

## Description

enable\_if is a template metaprogramming tool defined in the <type\_traits\> header file. It enables or disables a specific function template, class template, or template specialization at compile time based on a condition, thereby achieving finer-grained template overloading and type selection and enhancing code flexibility and safety.

enable\_if is a template struct with two template parameters: the template parameter Bp is a Boolean value that represents the condition, and the template parameter Tp is a type whose default value is void. When Bp is false, enable\_if has no nested type member. When Bp is true, enable\_if has a nested type member whose type is Tp.

## Prototype

```
template <bool Bp, typename Tp>
struct enable_if;
```

## Parameters

**Table 1** Template parameters

| Parameter | Description |
| --- | --- |
| Bp | Boolean value that represents the condition. |
| Tp | Type whose default value is void. |

## Constraints

None

## Return Value

The static constant member type of enable\_if is used to obtain the return value. The values of enable\_if<Bp,  Tp\>::type are as follows:

-   Tp: Bp is true.
-   void: Bp is false.

## Examples

```
template <typename T>
class Calculator {
public:
    // Enable this member function when T is an integer type.
    template <typename U = T>
    typename AscendC::Std::enable_if<AscendC::Std::is_integral<U>::value, U>::type
    __aicore__ inline multiply(U a, U b) {
        AscendC::PRINTF("Integral type multiplication");
        return a * b;
    }

    // Enable this member function when T is not an integer type.
    template <typename U = T>
    typename AscendC::Std::enable_if<!AscendC::Std::is_integral<U>::value, U>::type
    __aicore__ inline multiply(U a, U b) {
        AscendC::PRINTF("Non-integral type multiplication");
        return a * b;
    }
};

// General template class.
template <typename T, typename Enable = void>
class Container {
public:
    __aicore__ inline Container() {
        AscendC::PRINTF("Generic container.\n");
    }
};

// Specialized version, enabled when T is an integer type.
template <typename T>
class Container<T, typename AscendC::Std::enable_if<AscendC::Std::is_integral<T>::value>::type> {
public:
    __aicore__ inline Container() {
        AscendC::PRINTF("Integral container.\n");
    }
};

// Enable this function when T is an integer type.
template <typename T>
__aicore__ inline typename AscendC::Std::enable_if<AscendC::Std::is_integral<T>::value, T>::type add(T a, T b) {
    AscendC::PRINTF("Integral type addition.");
    return a + b;
}

// Enable this function when T is not an integer type.
template <typename T>
__aicore__ inline typename AscendC::Std::enable_if<!AscendC::Std::is_integral<T>::value, T>::type add(T a, T b) {
    AscendC::PRINTF("Non-integral type addition.");
    return a + (-b);
}

Calculator<int> intCalculator;
int intResult = intCalculator.multiply((int)2, (int)3);
AscendC::PRINTF("Result of integral multiplication: %d\n", intResult);

Calculator<float> doubleCalculator;
float doubleResult = doubleCalculator.multiply((float)2.5, (float)3.5);
AscendC::PRINTF("Result of non-integral multiplication: %f\n", doubleResult);

Container<float> genericContainer;
Container<int> integralContainer;

intResult = add(1, 2);
AscendC::PRINTF("Integer result: %d\n", intResult);

doubleResult = add((float)1.5, (float)2.5);
AscendC::PRINTF("float result: %f\n", doubleResult);
```

```
// Execution result:
Integral type multiplicationResult of integral multiplication: 6
Non-integral type multiplicationResult of non-integral multiplication: 8.750000
Generic container.
Integral container.
Integral type addition.Integer result: 3
Non-integral type addition.float result: -1.000000
```
