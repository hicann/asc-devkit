# Lambda Expressions

<!-- md-trans-meta sourceCommit=f9418c38fe93669ca5095ef7e8f02cab935baa1e translatedAt=2026-11-09T11:12:23.922Z pushedAt=2026-11-19T06:32:20.001Z nl=2 -->

This document describes how to use Lambda expressions on the Ascend C device side. The device side supports the complete form of C++11 Lambda expressions.

The Ascend C Lambda is a **device-side local closure**: kernel function parameters support only basic types, POD structs, and pointers. For details, see [Kernel Function Parameter List Restrictions](../cpp_language_restrictions/general_restrictions.md#section_kernel_func_param_restriction). Passing a host-side lambda into a kernel function is not supported, so there are no cross-execution-space Lambda restrictions or closure type traits.

The usage constraints of Lambda differ slightly between the SIMT programming scenario and the SIMD and SIMT mixed programming scenario. The following sections describe them separately.

## SIMT Programming Scenario

In the SIMT programming scenario, lambda expressions defined inside kernel functions and `__aicore__` functions require no additional markers and can be used directly:

```cpp
__global__ __launch_bounds__(THREAD_COUNT) void kernel(float* output)
{
    float scale = 2.f;
    float bias = 1.f;
    // Lambda inside a kernel function: capture by value, no function attribute marker required
    auto affine = [scale, bias](float v) { return v * scale + bias; };
    // Capture by reference: external variables can be modified
    float counter = 0.f;
    auto bump = [&counter]() { counter += 1.f; };
    bump();
    // A captureless lambda can be converted to a function pointer
    float (*fn)(float, float) = [](float a, float b) { return a + b; };
    output[threadIdx.x] = affine(3.f) + counter + fn(10.f, 5.f);
}
```

In the SIMT programming scenario, **namespace-scope lambda objects are subject to the global variable rules**: a non-constexpr closure object is a Host variable, and calling it inside a kernel function causes a compilation error:

```cpp
float g_arr[2] = {1.f, 2.f};
auto g_lambda = []() { return g_arr[0]; };   // Non-constexpr closure object: Host variable

__global__ __launch_bounds__(THREAD_COUNT) void probe(float* output)
{
    output[threadIdx.x] = static_cast<float>(g_lambda());
    // ERROR: reference to __host__ variable g_lambda in __global__ function
}
```

**Correct syntax**: declare the closure object as constexpr so that it is evaluated at compile time, or define the lambda inside the kernel function:

```cpp
constexpr auto g_sq = [](int32_t v) { return v * v; };   // constexpr closure: usable

__global__ __launch_bounds__(THREAD_COUNT) void probe(float* output)
{
    output[threadIdx.x] = static_cast<float>(g_sq(6));   // Compiles and runs correctly, output 36
}
```

For the usage of Lambda capture in actual operator development, see [Lambda capture sample](../../../../../../examples/03_simt_api/02_features/02_language_features/00_lambda_operator_capture/README.md).

## SIMD and SIMT Hybrid Programming Scenario

In the SIMD and SIMT mixed programming scenario, a Lambda expression defined on the device side **must be explicitly marked with a function attribute**; otherwise, a compilation error is reported. The rules for selecting the marker are as follows:

**Table 1** Marker selection rules for Lambda in SIMD and SIMT mixed programming

| Lambda Definition Location | Marker to Use | Description |
| --- | --- | --- |
| Inside the `__simt_vf__` function body | `__simt_callee__` | Consistent with the callee marker of the enclosing VF function. |
| Inside the `__simd_vf__` function body | `__simd_callee__` | Consistent with the callee marker of the enclosing VF function. |
| Inside the `__simt_callee__`, `__simd_callee__`, or `__aicore__` helper function body | `__simt_callee__`, `__simd_callee__`, or `__aicore__`, consistent with the host function | In SIMD and SIMT mixed programming, a lambda inside an `__aicore__` helper function must be marked with `__aicore__`. |
| Inside the SIMD and SIMT mixed programming kernel function body, that is, the SIMD-side scalar code of the `__global__` function | `__aicore__` | A compilation error is reported if it is not marked. |
| Inside the `__callee__` function body | `__callee__` | Consistent with the callee marker of the host function. |

The marker is placed after the Lambda parameter list:

```cpp
__simt_vf__ __launch_bounds__(THREAD_COUNT) inline void simt_lambda_by_value(
    __ubuf__ float* in, __ubuf__ float* out, uint32_t count, float scale, float bias)
{
    if (threadIdx.x >= count) {
        return;
    }
    // __simt_callee__ marks the attribute of this lambda function
    auto affine = [scale, bias](float v) __simt_callee__ {
        return v * scale + bias; };
    out[threadIdx.x] = affine(in[threadIdx.x]);
}

__global__ __vector__ void kernel(__gm__ float* input, __gm__ float* output)
{
    __ubuf__ float ub_in[THREAD_COUNT];
    __ubuf__ float ub_out[THREAD_COUNT];
    for (uint32_t i = 0; i < THREAD_COUNT; ++i) {
        ub_in[i] = input[i];
    }
    asc_vf_call<simt_lambda_by_value>(dim3(THREAD_COUNT), ub_in, ub_out, THREAD_COUNT, 2.f, 1.f);
    ...
}
```

For a complete example of Lambda capture in SIMD and SIMT mixed programming, see [Mixed Programming Lambda Capture Example](../../../../../../examples/05_simd_simt_hybrid/03_features/lambda_operator_hybrid/README.md).


### Lambda Capabilities Supported in VF Functions

In the SIMD and SIMT mixed programming scenario, the support status of each Lambda capability in VF functions is shown in the following table:

**Table 2** Lambda capability list in VF functions for SIMD and SIMT mixed programming

| Capability | Supported | Description |
| --- | --- | --- |
| Capture scalars by value | Yes | Syntax: `[scale, bias](float v) __simt_callee__ { ... }`. |
| Capture by reference | Yes | Syntax: `[&counter](float v) __simt_callee__ { ... }`, which can modify local variables inside the VF function. |
| Capture `__ubuf__` and `__gm__` pointers | Yes | The Lambda body can directly access UB and GM memory. The pointer source must be a VF function parameter or a valid local pointer inside the VF function. |
| Capture `this` and `[*this]` | Yes | In a `__simt_callee__` member function, both `[this]` and the C++17 `[*this]` capture by value are available. |
| mutable | Yes | Syntax: `[m]() __simt_callee__ mutable { ... }`. The function attribute marker must be placed **before** `mutable`; placing it after `mutable` reports the syntax error `expected body of lambda expression`. |
| Generic Lambda (C++14) | Yes | Syntax: `[](auto x) __simt_callee__ { ... }`. |
| init-capture (C++14) | Yes | Syntax: `[sum = base * 2.f](float x) __simt_callee__ { ... }`. |
| Nested Lambda | Yes | When an outer Lambda returns an inner Lambda, both the inner and outer Lambdas must carry the function attribute marker. |
| Immediately invoked (IIFE) | Yes | Syntax: `[](float a) __simt_callee__ { ... }(2.f)`. |
| Trailing return type | Yes | Syntax: `[](float x) __simt_callee__ -> float { ... }`. The marker must be placed before `-> T`. |
| As a function template argument | Yes | It can be passed as an argument to `__simt_callee__` and `__simd_callee__` function templates, such as `applyTwice(f, v)`. |
| callee Lambda calling callee Lambda | Yes | After capturing another closure object, it can be called directly. |
| `__simt_callee__` function returning a Lambda | Yes | A function in the form `__simt_callee__ auto makeAdder(float bias)` can return a closure object. |
| constexpr Lambda (C++17) | Partially | It must carry the callee function attribute marker; otherwise, compilation succeeds but the runtime result is incorrect. It can be used with `static_assert` or by placing the result in a `constexpr` variable. See usage constraint 1. |
| No parameter list form | Partially | `[v] __simt_callee__ { ... }` compiles but triggers the C++2b extension warning `lambda without a parameter clause`. Under the C++17 standard, an empty parameter list `()` should be written explicitly. See usage constraint 3. |
| callee-marked Lambda converted to function pointer | Partially | Conversion to an ordinary function pointer reports `no viable conversion`. After the function pointer type carries the same callee marker, such as `__simt_callee__ float (*fn)(float)`, it can be converted and called normally. See usage constraint 4. |
| namespace-scope Lambda object | No | A non-constexpr closure object is treated as a writable global variable, and accessing it inside a VF function reports `global variable ... is not allowed in aicore function`. A constexpr closure object is available. See usage constraint 5. |
| callee Lambda used across execution spaces | No | A closure carrying the callee marker can only be used within the corresponding VF function and its callee chain. Calling it across execution spaces reports `simt_callee function can only be called by simt_vf/simt_callee function`. See usage constraint 2. |

The usage constraints of Lambda are described item by item below:

-   **Constraint 1: constexpr lambda must carry the function identifier**: A constexpr lambda inside a VF function must carry the callee function identifier consistent with its execution space; otherwise, **compilation succeeds but the runtime result is incorrect**:

    ```cpp
    __simt_vf__ __launch_bounds__(THREAD_COUNT) inline void vf_func(__ubuf__ float* out, uint32_t count)
    {
        if (threadIdx.x >= count) {
            return;
        }
        // constexpr lambda without the callee identifier: compilation succeeds, but sq(8) returns an incorrect value at runtime
        constexpr auto sq = [](int32_t v) constexpr { return v * v; };
        out[threadIdx.x] = static_cast<float>(sq(8));
    }
    ```

    **Correct syntax**: The constexpr lambda carries the callee identifier according to its execution space. The identifier in a SIMT VF is `__simt_callee__`, and the identifier in a SIMD VF is `__simd_callee__`:

    ```cpp
    __simt_vf__ __launch_bounds__(THREAD_COUNT) inline void vf_func(__ubuf__ float* out, uint32_t count)
    {
        if (threadIdx.x >= count) {
            return;
        }
        // Carries the __simt_callee__ identifier: compilation and execution are normal, outputting 64
        constexpr auto sq = [](int32_t v) __simt_callee__ constexpr { return v * v; };
        out[threadIdx.x] = static_cast<float>(sq(8));
    }
    ```

-   **Constraint 2: the type of a lambda cannot be used across execution spaces**: A callee-marked Lambda can only be defined and used within the corresponding VF function and its callee chain. A closure returned by a `__simt_callee__` function can only be consumed within the SIMT VF and its callee chain. Calling it across to other execution spaces such as MainScalar reports a compilation error:

    ```cpp
    __simt_callee__ auto makeAdder(float bias)
    {
        return [bias](float v) __simt_callee__ { return v + bias; };
    }

    __global__ __vector__ void kernel_func(__gm__ float* output, uint32_t count)
    {
        auto add5 = makeAdder(5.f);          // ERROR: no matching function for call to makeAdder
        float r = add5(1.f);                 // (candidate not viable: simt_callee function can only be
                                             //  called by simt_vf/simt_callee function)
        for (uint32_t i = 0; i < count; ++i) { output[i] = r; }
    }
    ```

    **Correct syntax**: Keep the production and consumption of the closure in the same execution space, that is, define the lambda in the space where it is used:

    ```cpp
    __global__ __vector__ void kernel_func(__gm__ float* output, uint32_t count)
    {
        float bias = 5.f;
        // Define an __aicore__ lambda inside MainScalar, consistent with the execution space it is in
        auto add5 = [bias](float v) __aicore__ { return v + bias; };
        float r = add5(1.f);                 // Compiles and runs correctly: 6
        for (uint32_t i = 0; i < count; ++i) { output[i] = r; }
    }
    ```


-   **Constraint 3: An empty parameter list must be written with explicit parentheses**: The form that omits the parameter list, such as `[v] __simt_callee__ { ... }`, is a C++2b feature. The current default compilation uses C++17, so this form compiles but triggers an extension warning. Write an explicit parameter list `()` instead:

    ```cpp
    __simt_vf__ __launch_bounds__(THREAD_COUNT) inline void vf_func(__ubuf__ float* out, uint32_t count)
    {
        if (threadIdx.x >= count) { return; }
        float v = 5.f;
        auto f = [v] __simt_callee__ { return v; };   // WARNING: lambda without a parameter clause
                                                      //          is a C++2b extension [-Wc++2b-extensions]
        out[threadIdx.x] = f();
    }
    ```

    **Correct syntax**: Write empty parentheses explicitly in the lambda parameter list:

    ```cpp
    __simt_vf__ __launch_bounds__(THREAD_COUNT) inline void vf_func(__ubuf__ float* out, uint32_t count)
    {
        if (threadIdx.x >= count) { return; }
        float v = 5.f;
        auto f = [v]() __simt_callee__ { return v; };   // Explicit (): no warning, runs correctly
        out[threadIdx.x] = f();                          // Outputs 5
    }
    ```

-   **Constraint 4: Converting a callee-marked lambda to a function pointer requires the same marker**: Converting a callee-marked lambda to an **ordinary** function pointer reports `no viable conversion`. Adding the corresponding callee marker to the function pointer type itself enables normal conversion and invocation.

    ```cpp
    __simt_vf__ __launch_bounds__(THREAD_COUNT) inline void vf_func(__ubuf__ float* out, uint32_t count)
    {
        if (threadIdx.x >= count) { return; }
        float (*fn)(float) = [](float v) __simt_callee__ { return v * 2.f; };
        // ERROR: no viable conversion from lambda closure type to 'float (*)(float)'
        out[threadIdx.x] = fn(21.f);
    }
    ```

    **Correct syntax**: The function pointer type carries the same callee marker as the lambda:

    ```cpp
    __simt_vf__ __launch_bounds__(THREAD_COUNT) inline void vf_func(__ubuf__ float* out, uint32_t count)
    {
        if (threadIdx.x >= count) { return; }
        __simt_callee__ float (*fn)(float) = [](float v) __simt_callee__ { return v * 2.f; };
        out[threadIdx.x] = fn(21.f);   // Compiles and runs correctly, outputting 42
    }
    ```

    When the function pointer form is not required, the closure can also be passed as a function template argument:

    ```cpp
    template <typename F>
    __simt_callee__ float applyTwice(F f, float v) { return f(f(v)); }

    __simt_vf__ __launch_bounds__(THREAD_COUNT) inline void vf_func(__ubuf__ float* out, uint32_t count)
    {
        if (threadIdx.x >= count) { return; }
        auto dbl = [](float v) __simt_callee__ { return v * 2.f; };
        out[threadIdx.x] = applyTwice(dbl, 3.f);   // Passed as a template argument: compiles and runs correctly, outputting 12
    }
    ```

-   **Constraint 5: namespace-scope lambda objects must be constexpr**: A non-constexpr namespace-scope closure object is a writable global variable, and calling it inside a VF function causes a compilation error; a constexpr closure is usable.

    ```cpp
    float g_arr[2] = {1.f, 2.f};
    auto g_lambda = []() { return g_arr[0]; };   // Non-constexpr closure object: a global variable

    __simt_vf__ __launch_bounds__(THREAD_COUNT) inline void vf_func(__ubuf__ float* out, uint32_t count)
    {
        if (threadIdx.x >= count) { return; }
        out[threadIdx.x] = static_cast<float>(g_lambda());
        // ERROR: global variable 'g_lambda' is not allowed in aicore function
    }
    ```

    **Correct syntax**: Declare the closure object as constexpr, or define the lambda inside the VF function:

    ```cpp
    constexpr auto g_sq = [](int32_t v) { return v * v; };   // constexpr closure: usable

    __simt_vf__ __launch_bounds__(THREAD_COUNT) inline void vf_func(__ubuf__ float* out, uint32_t count)
    {
        if (threadIdx.x >= count) { return; }
        out[threadIdx.x] = static_cast<float>(g_sq(6));      // Compiles and runs correctly
    }
    ```
