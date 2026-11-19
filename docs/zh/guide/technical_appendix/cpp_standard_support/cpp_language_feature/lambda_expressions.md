# Lambda表达式

本文介绍Lambda表达式在Ascend C Device侧的使用方式。Device侧支持C++11 Lambda表达式的完整形态。

Ascend C的Lambda是**Device侧局部闭包**：核函数参数仅支持基础类型、POD结构体与指针，见[核函数参数列表限制](../cpp_language_restrictions/general_restrictions.md#section_kernel_func_param_restriction)，不支持Host侧lambda传入核函数的使用方法，因此也没有跨执行空间的Lambda限制与闭包类型traits。

SIMT编程场景、SIMD与SIMT混合编程场景下Lambda的使用约束略有不同，下文分别展开说明。

## SIMT编程场景

SIMT编程场景下，核函数与`__aicore__`函数内定义的Lambda表达式无需额外标记，直接使用即可：

```cpp
__global__ __launch_bounds__(THREAD_COUNT) void kernel(float* output)
{
    float scale = 2.f;
    float bias = 1.f;
    // 核函数内lambda：值捕获，无需标记函数属性
    auto affine = [scale, bias](float v) { return v * scale + bias; };
    // 引用捕获：可修改外部变量
    float counter = 0.f;
    auto bump = [&counter]() { counter += 1.f; };
    bump();
    // 无捕获lambda可转函数指针
    float (*fn)(float, float) = [](float a, float b) { return a + b; };
    output[threadIdx.x] = affine(3.f) + counter + fn(10.f, 5.f);
}
```

SIMT编程场景，**namespace-scope的lambda对象受全局变量规则约束**：非constexpr闭包对象属于Host变量，核函数内调用编译报错：

```cpp
float g_arr[2] = {1.f, 2.f};
auto g_lambda = []() { return g_arr[0]; };   // 非constexpr闭包对象：Host变量

__global__ __launch_bounds__(THREAD_COUNT) void probe(float* output)
{
    output[threadIdx.x] = static_cast<float>(g_lambda());
    // ERROR: reference to __host__ variable g_lambda in __global__ function
}
```

**正确写法**：闭包对象声明为constexpr以在编译期求值，或将lambda定义在核函数内部：

```cpp
constexpr auto g_sq = [](int32_t v) { return v * v; };   // constexpr闭包：可用

__global__ __launch_bounds__(THREAD_COUNT) void probe(float* output)
{
    output[threadIdx.x] = static_cast<float>(g_sq(6));   // 编译运行正常，输出36
}
```

Lambda捕获在实际算子开发中的用法请参考[Lambda捕获样例](../../../../../../examples/03_simt_api/02_features/02_language_features/00_lambda_operator_capture/README.md)。

## SIMD与SIMT混合编程场景

SIMD与SIMT混合编程场景下，Device侧定义的Lambda表达式**必须显式标记函数属性**，否则编译报错。标记的选择规则如下：

**表1**  SIMD与SIMT混合编程Lambda标记选择规则

| Lambda定义位置 | 应使用的标记 | 说明 |
| --- | --- | --- |
| `__simt_vf__`函数体内 | `__simt_callee__` | 与所在VF函数的callee标记一致。 |
| `__simd_vf__`函数体内 | `__simd_callee__` | 与所在VF函数的callee标记一致。 |
| `__simt_callee__`、`__simd_callee__`、`__aicore__`辅助函数体内 | 与宿主函数一致的`__simt_callee__`、`__simd_callee__`、`__aicore__` | SIMD与SIMT混合编程中`__aicore__`辅助函数内的lambda需标记`__aicore__`。 |
| SIMD与SIMT混合编程核函数体内，即`__global__`函数的SIMD侧scalar代码 | `__aicore__` | 未标记会触发编译报错。 |
| `__callee__`函数体内 | `__callee__` | 与宿主函数的callee标记一致。 |

标记位置在Lambda的参数列表之后：

```cpp
__simt_vf__ __launch_bounds__(THREAD_COUNT) inline void simt_lambda_by_value(
    __ubuf__ float* in, __ubuf__ float* out, uint32_t count, float scale, float bias)
{
    if (threadIdx.x >= count) {
        return;
    }
    // __simt_callee__ 标识该lambda函数属性
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

SIMD与SIMT混合编程下Lambda捕获的完整样例请参考[混合编程Lambda捕获样例](../../../../../../examples/05_simd_simt_hybrid/03_features/lambda_operator_hybrid/README.md)。

### VF函数内支持的Lambda能力

SIMD与SIMT混合编程场景下，VF函数内Lambda各能力项的支持情况如下表所示：

**表2**  SIMD与SIMT混合编程VF函数内Lambda能力列表

| 能力 | 是否支持 | 说明 |
| --- | --- | --- |
| 值捕获标量 | 是 | 写法：`[scale, bias](float v) __simt_callee__ { ... }`。 |
| 引用捕获 | 是 | 写法：`[&counter](float v) __simt_callee__ { ... }`，可修改VF函数内的局部变量。 |
| 捕获`__ubuf__`与`__gm__`指针 | 是 | Lambda体内可直接访问UB、GM内存，指针来源须为VF函数形参或VF函数内的合法局部指针。 |
| 捕获`this`与`[*this]` | 是 | `__simt_callee__`成员函数内`[this]`与C++17 `[*this]`按值捕获均可用。 |
| mutable | 是 | 写法：`[m]() __simt_callee__ mutable { ... }`。函数属性标记须置于`mutable`**之前**，置于其后报语法错误`expected body of lambda expression`。 |
| 泛型Lambda（C++14） | 是 | 写法：`[](auto x) __simt_callee__ { ... }`。 |
| init-capture（C++14） | 是 | 写法：`[sum = base * 2.f](float x) __simt_callee__ { ... }`。 |
| 嵌套Lambda | 是 | 外层Lambda返回内层Lambda时，内层与外层均须携带函数属性标记。 |
| 立即调用（IIFE） | 是 | 写法：`[](float a) __simt_callee__ { ... }(2.f)`。 |
| 后置返回类型 | 是 | 写法：`[](float x) __simt_callee__ -> float { ... }`，标记须置于`-> T`之前。 |
| 作为函数模板实参 | 是 | 可作为实参传递给`__simt_callee__`、`__simd_callee__`函数模板，如`applyTwice(f, v)`。 |
| callee Lambda调用callee Lambda | 是 | 捕获另一闭包对象后可直接调用。 |
| `__simt_callee__`函数返回Lambda | 是 | `__simt_callee__ auto makeAdder(float bias)`形式的函数可返回闭包对象。 |
| constexpr Lambda（C++17） | 部分 | 须携带callee函数属性标记，否则编译通过但运行结果错误；配合`static_assert`或将结果落于`constexpr`变量后使用均可用。详见使用约束1。 |
| 无参数列表形式 | 部分 | `[v] __simt_callee__ { ... }`可编译，但触发C++2b扩展告警`lambda without a parameter clause`；C++17标准下应显式书写空参数列表`()`。详见使用约束3。 |
| callee标记Lambda转函数指针 | 部分 | 向普通函数指针转换报`no viable conversion`；函数指针类型携带如`__simt_callee__ float (*fn)(float)`的相同callee标记后可正常转换与调用。详见使用约束4。 |
| namespace-scope Lambda对象 | 否 | 非constexpr闭包对象视为可写全局变量，VF函数内访问报`global variable ... is not allowed in aicore function`；constexpr闭包对象可用。详见使用约束5。 |
| callee Lambda跨执行空间使用 | 否 | 携带callee标记的闭包仅可在对应VF函数及其callee链内使用，跨执行空间调用报`simt_callee function can only be called by simt_vf/simt_callee function`。详见使用约束2。 |

以下将对lambda的使用约束逐项展开说明：

-   **约束1：constexpr lambda需加函数标识**：VF函数内的constexpr lambda必须携带与所在空间一致的callee函数标识，否则**编译通过但运行结果错误**：

    ```cpp
    __simt_vf__ __launch_bounds__(THREAD_COUNT) inline void vf_func(__ubuf__ float* out, uint32_t count)
    {
        if (threadIdx.x >= count) {
            return;
        }
        // constexpr lambda未携带callee标识：编译通过，但运行期sq(8)返回错误值
        constexpr auto sq = [](int32_t v) constexpr { return v * v; };
        out[threadIdx.x] = static_cast<float>(sq(8));
    }
    ```

    **正确写法**：constexpr lambda按所在执行空间携带callee标识，SIMT VF内的标识`__simt_callee__`，SIMD VF内的标识`__simd_callee__`：

    ```cpp
    __simt_vf__ __launch_bounds__(THREAD_COUNT) inline void vf_func(__ubuf__ float* out, uint32_t count)
    {
        if (threadIdx.x >= count) {
            return;
        }
        // 携带__simt_callee__标识：编译运行正常，输出64
        constexpr auto sq = [](int32_t v) __simt_callee__ constexpr { return v * v; };
        out[threadIdx.x] = static_cast<float>(sq(8));
    }
    ```

-   **约束2：lambda的类型不可跨执行空间使用**：标记callee的Lambda只能在对应VF函数及其callee链内定义和使用。`__simt_callee__`函数返回的闭包只能在SIMT VF及其callee链内消费，跨到MainScalar等其他执行空间调用编译报错：

    ```cpp
    __simt_callee__ auto makeAdder(float bias)
    {
        return [bias](float v) __simt_callee__ { return v + bias; };
    }

    __global__ __vector__ void kernel_func(__gm__ float* output, uint32_t count)
    {
        auto add5 = makeAdder(5.f);          // ERROR: no matching function for call to makeAdder
        float r = add5(1.f);                 // （candidate not viable: simt_callee function can only be
                                             //  called by simt_vf/simt_callee function）
        for (uint32_t i = 0; i < count; ++i) { output[i] = r; }
    }
    ```

    **正确写法**：闭包的生产与消费保持在同一执行空间内，即lambda在哪个空间使用就在哪个空间定义：

    ```cpp
    __global__ __vector__ void kernel_func(__gm__ float* output, uint32_t count)
    {
        float bias = 5.f;
        // MainScalar内定义__aicore__ lambda，与所在执行空间一致
        auto add5 = [bias](float v) __aicore__ { return v + bias; };
        float r = add5(1.f);                 // 编译运行正常：6
        for (uint32_t i = 0; i < count; ++i) { output[i] = r; }
    }
    ```

-   **约束3：无参数列表形式须写显式括号**：省略参数列表的写法，如`[v] __simt_callee__ { ... }`，是C++2b特性；当前默认按C++17编译，该写法可编译但会触发扩展告警，应写显式参数列表`()`：

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

    **正确写法**：lambda参数列表显式写空括号：

    ```cpp
    __simt_vf__ __launch_bounds__(THREAD_COUNT) inline void vf_func(__ubuf__ float* out, uint32_t count)
    {
        if (threadIdx.x >= count) { return; }
        float v = 5.f;
        auto f = [v]() __simt_callee__ { return v; };   // 显式()：无告警，运行正常
        out[threadIdx.x] = f();                          // 输出5
    }
    ```

-   **约束4：callee标记lambda转函数指针需同标识**：callee标记lambda向**普通**函数指针转换会报`no viable conversion`；函数指针类型本身加上对应的callee标识即可正常转换与调用。

    ```cpp
    __simt_vf__ __launch_bounds__(THREAD_COUNT) inline void vf_func(__ubuf__ float* out, uint32_t count)
    {
        if (threadIdx.x >= count) { return; }
        float (*fn)(float) = [](float v) __simt_callee__ { return v * 2.f; };
        // ERROR: no viable conversion from lambda closure type to 'float (*)(float)'
        out[threadIdx.x] = fn(21.f);
    }
    ```

    **正确写法**：函数指针类型携带与lambda一致的callee标识：

    ```cpp
    __simt_vf__ __launch_bounds__(THREAD_COUNT) inline void vf_func(__ubuf__ float* out, uint32_t count)
    {
        if (threadIdx.x >= count) { return; }
        __simt_callee__ float (*fn)(float) = [](float v) __simt_callee__ { return v * 2.f; };
        out[threadIdx.x] = fn(21.f);   // 编译运行正常，输出42
    }
    ```

    不需要函数指针形态时，闭包经函数模板实参传递同样可用：

    ```cpp
    template <typename F>
    __simt_callee__ float applyTwice(F f, float v) { return f(f(v)); }

    __simt_vf__ __launch_bounds__(THREAD_COUNT) inline void vf_func(__ubuf__ float* out, uint32_t count)
    {
        if (threadIdx.x >= count) { return; }
        auto dbl = [](float v) __simt_callee__ { return v * 2.f; };
        out[threadIdx.x] = applyTwice(dbl, 3.f);   // 模板实参传递：编译运行正常，输出12
    }
    ```

-   **约束5：namespace-scope lambda对象需constexpr**：非constexpr的namespace-scope闭包对象属于可写全局变量，VF函数内调用编译报错；constexpr闭包可用。

    ```cpp
    float g_arr[2] = {1.f, 2.f};
    auto g_lambda = []() { return g_arr[0]; };   // 非constexpr闭包对象：全局变量

    __simt_vf__ __launch_bounds__(THREAD_COUNT) inline void vf_func(__ubuf__ float* out, uint32_t count)
    {
        if (threadIdx.x >= count) { return; }
        out[threadIdx.x] = static_cast<float>(g_lambda());
        // ERROR: global variable 'g_lambda' is not allowed in aicore function
    }
    ```

    **正确写法**：闭包对象声明为constexpr，或将lambda定义在VF函数内部：

    ```cpp
    constexpr auto g_sq = [](int32_t v) { return v * v; };   // constexpr闭包：可用

    __simt_vf__ __launch_bounds__(THREAD_COUNT) inline void vf_func(__ubuf__ float* out, uint32_t count)
    {
        if (threadIdx.x >= count) { return; }
        out[threadIdx.x] = static_cast<float>(g_sq(6));      // 编译运行正常
    }
    ```
