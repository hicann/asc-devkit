# enable\_if

<!-- md-trans-meta sourceCommit=2737c34bbaf046b0c4e915117e38247d0f621f63 translatedAt=2026-09-06T11:57:19.271Z pushedAt=2026-09-11T04:50:19.870Z -->

## Applicable Products

- Ascend 950PR/Ascend 950DT: supported
- Atlas A3 training products/Atlas A3 inference products: supported
- Atlas A2 training products/Atlas A2 inference products: supported
- Atlas 200I/500 A2 inference products: not supported
- Atlas inference products AI Core: not supported
- Atlas inference products Vector Core: not supported
- Atlas training products: not supported

## Description

enable\_if is a template metaprogramming utility defined in the <type\_traits\> header. It can enable or disable a specific function template, class template, or template specialization at compile time based on a condition, thereby achieving finer-grained template overloading and type selection and enhancing code flexibility and safety.

enable\_if is a template struct with two template parameters: the template parameter **Bp** is a Boolean value that represents the condition, and the template parameter **Tp** is a type whose default value is **void**. When **Bp** is false, enable\_if has no nested **type** member. When **Bp** is true, enable\_if has a nested **type** member whose type is **Tp**.

## Prototype

```
template <bool Bp, typename Tp>
struct enable_if;
```

## Parameters

**Table 1** Template parameter description

| Parameter | Description |
| --- | --- |
| Bp | Boolean value that indicates the condition. |
| Tp | Type, with a default value of void. |

## Constraints

None

## Return Value

The static constant member **type** of enable\_if is used to obtain the return value. The values of enable\_if<Bp,  Tp\>::type are as follows:

-   **Tp**: **Bp** is **true**.
-   **void**: **Bp** is **false**.

## Example

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

// Generic template class.
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

