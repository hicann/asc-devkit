# C/C++通用语法限制

## 不支持的特性

-   不支持虚函数
-   不支持虚继承
-   不支持动态内存分配，即malloc、free、new、delete均不可用
-   不支持STL
-   不支持运行时typeid
-   不支持文件系统IO
-   不支持标准库下的tuple及算法类运算，相关库函数调用需要标记aicore

## 指针与内存地址

指针解引用，如`*pointer`、`pointer->member`、`pointer[0]`等操作，不允许跨空间执行：Device侧的GM、UB指针不能在Host侧解引用，Host栈或全局地址不能在Device侧解引用，原因是Host侧与Device侧地址空间相互不可见，此类跨空间访问在编程模型层面禁止。

### 取Device函数的地址

- 允许取`__aicore__`函数的地址存入函数指针并通过函数指针调用，编译通过且运行结果正确：

    ```cpp
    __aicore__ inline float scale2(float v) { return v * 2.f; }

    __global__ __launch_bounds__(128) void probe(float* output, uint32_t count)
    {
        float (*fn)(float) = scale2;          // 取__aicore__函数地址：可用
        output[threadIdx.x] = fn(static_cast<float>(threadIdx.x));
    }
    ```

- SIMT VF与SIMD VF内的callee函数可以函数指针形式间接调用，但函数指针类型须携带与函数一致的callee标识。

    `__simt_callee__`和`__simd_callee__`标记使函数类型带上函数属性，向普通函数指针转换时报错：

    ```cpp
    __simt_callee__ float scale2(float v) { return v * 2.f; }

    __simt_vf__ __launch_bounds__(128) inline void vf_func(__ubuf__ float* out, uint32_t count)
    {
        float (*fn)(float) = scale2;
        // error: cannot initialize a variable of type 'float (*)(float)' with an lvalue of type
        //        '__simt_callee__ float (float) __attribute__((cce_simt_callable))'
    }
    ```

    函数指针类型加上callee标识后即可正常转换与调用。SIMT VF内函数指针使用`__simt_callee__`标识，SIMD VF内则使用`__simd_callee__`标识：

    ```cpp
    __simt_callee__ float scale2(float v) { return v * 2.f; }

    __simt_vf__ __launch_bounds__(128) inline void vf_func(__ubuf__ float* out, uint32_t count)
    {
        __simt_callee__ float (*fn)(float) = scale2;   // 指针类型携带callee标识
        out[threadIdx.x] = fn(21.f);                   // 编译运行正常，输出42
    }
    ```

- SIMT VF和SIMD VF函数不支持经函数指针调用或启动，只支持使用`asc_vf_call()`接口调用。

- 不允许引用Host函数的地址：核函数内引用Host函数，包括直接调用和函数指针均会编译报错：

    ```cpp
    float host_fn(float v) { return v + 100.f; }

    __global__ __launch_bounds__(128) void probe(float* output, uint32_t count)
    {
        float (*fp)(float) = host_fn;         // ERROR: reference to __host__ function 'host_fn' in __global__ function
        output[threadIdx.x] = fp(1.f);
    }
    ```
- Device代码在编译期随核函数静态链接，不支持运行时使用`dlopen`动态加载Device函数的机制。

### 核函数的地址

`__global__`核函数之间不允许互相调用，也不允许在核函数内引用核函数的地址。核函数只能由Host侧通过`<<<>>>`启动：

```cpp
__global__ void kernel_a(float* output) { ... }

__global__ __launch_bounds__(128) void probe(float* output, uint32_t count)
{
    void* p = reinterpret_cast<void*>(&kernel_a);  // ERROR: reference to __global__ function in __global__ function
    output[threadIdx.x] = p == nullptr ? 0.f : 1.f;
}
```

### 指针运算与字节重解释

指针的大小比较和`reinterpret_cast`字节重解释仅支持在同一地址空间内进行，不同地址空间之间的指针不支持这两种操作。同一地址空间内的用法如下：

```cpp
__global__ __launch_bounds__(128) void probe(float* output, uint32_t count)
{
    float* p1 = output + threadIdx.x;
    float* p2 = output + (count - 1 - threadIdx.x);
    bool cmp = p1 < p2;                                    // GM指针比较：可用
    int32_t* pi = reinterpret_cast<int32_t*>(p1);          // 字节重解释：可用
    pi[0] = static_cast<int32_t>(cmp ? 1 : 0);
}
```

跨地址空间的指针运算和`reinterpret_cast`字节重解释将触发编译报错：

-   不同地址空间指针比较大小，如`__gm__`、`__ubuf__`与栈地址不能比较大小：

    ```cpp
    __simt_vf__ __launch_bounds__(128) inline void vf_func(__gm__ float* in, __ubuf__ float* out, uint32_t count)
    {
        bool cmp = in < out;      // ERROR: comparison of distinct pointer types ('__gm__ float *' and '__ubuf__ float *')
        out[threadIdx.x] = cmp ? 1.f : 0.f;
    }
    ```

    ```cpp
    __simt_vf__ __launch_bounds__(128) inline void vf_func(__gm__ float* in, __ubuf__ float* out, uint32_t count)
    {
        float local = 1.f;
        bool cmp = in < &local;   // ERROR: comparison of distinct pointer types ('__gm__ float *' and 'float *')
        out[threadIdx.x] = cmp ? 1.f : 0.f;
    }
    ```

-   不同地址空间指针`reinterpret_cast`字节重解释：

    ```cpp
    __simt_vf__ __launch_bounds__(128) inline void vf_func(__gm__ float* in, __ubuf__ float* out, uint32_t count)
    {
        __ubuf__ float* recast = reinterpret_cast<__ubuf__ float*>(in);
        // ERROR: reinterpret_cast from '__gm__ float *' to '__ubuf__ float *' is not allowed
        recast[threadIdx.x] = in[threadIdx.x];
    }
    ```

    ```cpp
    __global__ __vector__ void kernel_func(__gm__ float* input, __gm__ float* output, uint32_t count)
    {
        float* recast = reinterpret_cast<float*>(input);
        // ERROR: reinterpret_cast from '__gm__ float *' to 'float *' is not allowed
        output[0] = recast != nullptr ? 1.f : 0.f;
    }
    ```

> [!NOTE]说明
> 上述限制针对**带地址空间限定**的指针，即`__gm__`、`__ubuf__`、栈地址之间的比较和转换。SIMT编程场景的指针形参无地址空间限定概念，核内GM指针与栈变量指针可直接比较和`reinterpret_cast`。

## 变量

### static局部变量

核函数与`__aicore__`函数内允许声明static局部变量，但**不保证多核、多线程间的读写结果确定**，原因如下：

-   **存放位置**：static局部变量位于GM中的全局数据区，被同一核函数的全部执行实例即所有核和所有线程共享，并非线程私有；
-   **多核并发竞争**：多个核并行执行同一核函数时，对同一static变量的读改写操作如`counter += 1`交错执行，无原子性与互斥保证，最终值取决于各核的执行时序；
-   **内存一致性**：各核对static变量的修改缺乏跨核的内存可见性保证，其他核可能读到修改前的旧值。

如下所示，计算结果并不确定：

```cpp
__global__  void static_life(float* output, uint32_t count)
{
    if (threadIdx.x >= count) {
        return;
    }
    static int32_t counter = 0;
    counter += 1;
    output[threadIdx.x] = static_cast<float>(counter); // output结果不确定
}
```
另外，static局部变量不允许动态初始化，初始化表达式须为编译期常量：

```cpp
__global__ void probe(float* output, uint32_t count)
{
    static int32_t v1 = 11;           // CORRECT：常量初始化
    // static int32_t v2 = count;     // 编译报错
    output[threadIdx.x] = static_cast<float>(v1);
}
```

SIMT VF和SIMD VF内不支持使用static局部变量：

```cpp
__simt_vf__ __launch_bounds__(128) inline void vf_func(__ubuf__ float* out, uint32_t count)
{
    static int32_t counter = 0;    // 编译报错
    counter += 1;
    out[threadIdx.x] = static_cast<float>(counter);
}
```

### 全局变量与namespace-scope变量

可写的全局变量包括namespace-scope的`static`变量与非常量初始化的普通全局变量，此类变量若无地址空间标识则Device侧不支持。若要在Device侧使用可写的全局变量，需为变量声明加上`__gm__`标识符，变量实际存储于Global Memory：

-   不加`__gm__`标识：Device侧编译报错。

    ```cpp
    int32_t g_counter = 10;           // 无标识：Device侧不可用

    __global__ __launch_bounds__(128) void probe(uint32_t* output)
    {
        int32_t acc = g_counter;      // 编译报错
        output[threadIdx.x] = static_cast<uint32_t>(acc);
    }
    ```

-   加`__gm__`标识后Device侧正常使用。

    ```cpp
    __gm__ int32_t g_counter = 10;    // __gm__标记：Device侧可用

    __global__ __launch_bounds__(128) void probe(uint32_t* output)
    {
        int32_t acc = g_counter;            // 编译运行正常
        output[threadIdx.x] = static_cast<uint32_t>(acc);
    }
    ```

### const限定符变量

全局、namespace或类作用域声明的const限定变量默认属于Host变量，除非带有Device侧内存空间标注。Ascend C Device侧可直接使用、引用与取地址常量表达式初始化的const全局变量：

```cpp
const int ConstVar = 10;                  // 常量表达式初始化的全局const
const float ConstFloatVar = 5.0f;

__global__ __launch_bounds__(128) void probe(float* output, uint32_t count)
{
    int arr[ConstVar];                    // CORRECT：内建整型直接用作数组维度
    float v = ConstFloatVar;              // CORRECT：浮点const直接使用
    const int& ref = ConstVar;            // CORRECT：引用全局const变量
    const int* p = &ConstVar;             // CORRECT：取全局const变量地址
    output[threadIdx.x] = static_cast<float>(arr[ConstVar - 1]) + v
                        + static_cast<float>(ref) * 0.8f + static_cast<float>(*p) * 0.6f;
}
```

extern声明且未在本编译单元内以常量表达式初始化的const变量不可在核函数内使用：

```cpp
extern const int ExternVar;               // 本单元内无常量初始化式

__global__ void probe(float* output, uint32_t count)
{
    output[threadIdx.x] = static_cast<float>(ExternVar);
    // ERROR: reference to __host__ variable 'ExternVar' in __global__ function
}
```

> [!NOTE]说明
> 从C++14起，推荐使用constexpr变量替代const限定变量，C++17起也可使用inline constexpr变量。constexpr变量不受类型限制，且语义上明确为编译期常量，Device侧使用行为与const一致。

### const和constexpr全局变量的取地址与字节重解释

常量表达式初始化的const或constexpr全局变量（namespace-scope）在Device侧可取地址，所得指针指向常量地址空间。可以用`reinterpret_cast`改变指针所指的数据类型，但转换后的指针仍须指向该常量地址空间。以`__aicore__`函数中将`const int*`转换为`const float*`为例：

```cpp
constexpr int ASCRT_NAN_F_VALUE = 0x7fffffffU;

__aicore__ inline float nanf(float x)
{
    float res = *(reinterpret_cast<const float*>(&ASCRT_NAN_F_VALUE));
    (void)x;
    return res;
}
```

但是，SIMD与SIMT混合编程场景，在使用`-O0`编译选项（`CMAKE_BUILD_TYPE=Debug`）时，对未标记`__gm__`的const或constexpr全局变量取地址会触发编译器后端错误；即使只通过同类型指针读取，也会报错：

```cpp
constexpr int NANF_BITS = 0x7fffffffU;

__simt_vf__ __launch_bounds__(128) void nanf_worker_bad(__gm__ float* out)
{
    const int* ip = &NANF_BITS;              // ERROR(-O0)：编译器后端错误
    out[0] = static_cast<float>(*ip);
}
```

强烈建议使用以下更通用的方法：

-   为全局变量加`__gm__`标识，并以`__gm__`指针接收；
-   不取全局变量地址，在SIMT VF函数内用局部联合体将整数的位模式按float类型解释。

如下所示：

```cpp
__gm__ constexpr int GM_NANF_BITS = 0x7fffffffU;

__simt_vf__ __launch_bounds__(128) void nanf_worker(__gm__ float* out)
{
    const __gm__ int* ip = &GM_NANF_BITS;    // __gm__对象配__gm__指针，-O0下可用
    out[0] = *reinterpret_cast<const __gm__ float*>(ip);  // 按浮点位模式读取
}
```

第二种方法不依赖全局变量的地址：

```cpp
__simt_vf__ __launch_bounds__(128) void nanf_worker_by_bits(__gm__ float* out)
{
    union Data {
        int i;
        float f;
    } data;
    data.i = 0x7fffffff;
    out[0] = data.f;                         // 按float解释整数位模式
}
```

### volatile限定符变量

volatile关键字为兼容ISO C++而支持，volatile读写不提供线程间同步保证，也不保证硬件执行的记忆体访问次数与指令数一致，编译器不合并、不省略volatile访问，但多个线程对同一volatile对象的访问无顺序与可见性保证。

以for循环轮询GM标志位为例：volatile读在每轮循环中都从GM真实重新加载而不缓存到寄存器，因此循环能持续观测到GM的最新内容：

```cpp
__global__ __launch_bounds__(128) void probe(int* flag, float* output, uint32_t count)
{
    if (threadIdx.x == 0) {
        // 轮询GM标志位：volatile指针每轮循环都从GM真实读取
        volatile int* vflag = flag;
        int32_t spins = 0;
        for (uint32_t i = 0; i < 1000u; ++i) {
            spins += *vflag;                 // 每轮真实读GM，不被合并、不被省略
        }
        output[0] = static_cast<float>(spins);
    }
}
```

若去掉volatile，编译器可将循环内的多次读取合并为一次加载甚至完全消除，将无法及时获取到其他执行单元对该地址的修改。但volatile只保证访问不被优化，不保证读到其他核、其他线程写入的最新值。原因是各核对GM的修改缺乏跨核可见性与顺序保证，轮询也有可能读到旧值。

### constexpr全局只读表

编译期初始化的POD constexpr全局数组可以在核函数内运行期索引，取其地址作为指针使用同样可用：

```cpp
constexpr uint32_t TABLE[4] = {1u, 2u, 4u, 8u};

__global__ __launch_bounds__(128) void probe(float* output, uint32_t count)
{
    float acc = 0.f;
    for (uint32_t i = 0; i < 4u; ++i) {
        acc += static_cast<float>(TABLE[i]);   // 运行期索引constexpr表：可用
    }
    const uint32_t* tp = &TABLE[0];            // 取constexpr表地址：可用
    output[threadIdx.x] = acc + static_cast<float>(tp[threadIdx.x % 4u]);
}
```

**数组下标越界不被编译器拦截，属未定义行为**。运行期下标越界、编译期常量下标越界、经指针算术越界（`*(tp + idx)`）在编译期均无告警无报错，需用户自行做好越界保护。

> [!NOTE]说明
> SIMD与SIMT混合编程的SIMT VF内**直接**以运行期下标索引constexpr全局表会触发编译报错；VF内需要索引全局表时，先将表元素拷贝到VF函数局部数组再索引。

### thread_local存储

不支持`thread_local`存储：SIMT编程场景核函数的局部变量本身即线程私有，通常无需thread_local。

## 函数

### 核函数（Kernel）参数列表限制<a name="section_kernel_func_param_restriction"></a>

核函数（Kernel）的参数列表（argument list）仅支持以下类型：

- 基础数据类型，如int32\_t、float等。
- 基础数据类型组成的结构体，支持嵌套结构体形式，但必须为POD（Plain Old Data）类型。
- 基础数据类型的指针类型，如int32\_t\*、float\*等，实际上这些指针指向的是Global Memory内存。

### SIMT VF函数限制

\_\_simt\_vf\_\_标记的SIMT VF函数需要遵循以下约束和限制：

-   标量参数不允许使用指针和引用：指针形参必须显式携带`__gm__`或`__ubuf__`地址空间限定符以标识指向GM或UB内存，引用形参一律不支持。标量数据按值传入，需要输出标量结果时经指针形参写出。以下错误示例在编译期报错：

    ```cpp
    // 错误示例1：标量指针形参（无地址空间限定）
    __simt_vf__ __launch_bounds__(1024) inline void foo(__ubuf__ float* out, uint32_t count, float* scalarPtr) {
        out[threadIdx.x] = *scalarPtr;
        // error: pointer arguments to simt_vf functions must reside in '__gm__ or __ubuf__' address space
    }

    // 错误示例2：标量引用形参
    __simt_vf__ __launch_bounds__(1024) inline void foo(__ubuf__ float* out, uint32_t count, const float& scale) {
        out[threadIdx.x] = static_cast<float>(threadIdx.x) * scale;
        // error: pointer arguments to simt_vf functions must reside in '__gm__ or __ubuf__' address space
    }
    ```

    **正确写法**：标量按值传递；指针按`__gm__`或`__ubuf__`限定传入；需要输出标量结果时经指针形参写出：

    ```cpp
    // 标量按值传入 + 指针带地址空间限定
    __simt_vf__ __launch_bounds__(1024) inline void foo(__ubuf__ float* out, __gm__ float* in, uint32_t count, float scale) {
        uint32_t idx = blockIdx.x * blockDim.x + threadIdx.x;
        out[idx] = in[idx] * scale;              // scale为值拷贝
    }

    // 输出标量结果：经__ubuf__指针形参写出，代替引用输出
    __simt_vf__ __launch_bounds__(1024) inline void foo(__ubuf__ float* out, __gm__ float* in, uint32_t count, __ubuf__ float* scalarOut) {
        uint32_t idx = blockIdx.x * blockDim.x + threadIdx.x;
        out[idx] = in[idx];
        if (threadIdx.x == 0) {
            scalarOut[0] = in[0] * 2.f;          // 标量结果写UB，由核函数侧读回
        }
    }
    ```
-   不允许将栈数组作为参数传入SIMT VF函数；SIMT VF函数的指针形参必须显式使用`__gm__`或`__ubuf__`地址空间限定符。以下错误示例会在编译期报错。

    ```cpp
    __simt_vf__ __launch_bounds__(1024) inline void foo(__gm__ int* a, __gm__ int* b, __gm__ int* c, int* array) {
        int idx = blockIdx.x * blockDim.x + threadIdx.x;
        a[idx] = b[idx] + c[idx] + array[0];
    }
    __global__ __aicore__ void foo(__gm__ int* a, __gm__ int* b, __gm__ int* c) {
        int array[5] = {0,1,2,3,4};
        asc_vf_call<foo>(dim3{256}, a, b, c, array); // error: cannot initialize a parameter of type '__gm__ int *' with an lvalue of type 'int[5]'
    }
    ```

-   VF内部禁止访问MainScalar的栈空间。

    MainScalar的栈地址如经`__ubuf__`指针携带传入VF函数后，VF内对其解引用属于无效访问：运行期读写是未定义行为，属于难以定位的静默错误。以下为错误示例：

    ```cpp
    __simt_vf__ __launch_bounds__(1024) inline void foo(__gm__ int* out, __ubuf__ uint64_t* s, uint32_t count) {
        if (threadIdx.x >= count) {
            return;
        }
        int* stack = (int*)(s[0]);          // 编译通过：从多级指针取出MainScalar栈地址
        out[threadIdx.x] = *stack;          // 运行期读到的是无关内存内容，并非栈上的stack值
    }

    __global__ __vector__ void kernel_func(__gm__ int* output, uint32_t count)
    {
        int stack = 123;                    // MainScalar栈变量
        __ubuf__ uint64_t sbuf[1];
        sbuf[0] = (uint64_t)&stack;         // 栈地址经UB指针传入VF
        asc_vf_call<foo>(dim3(1024), output, sbuf, count);
        asc_sync_notify(PIPE_V, PIPE_MTE3, EVENT_ID0);
        asc_sync_wait(PIPE_V, PIPE_MTE3, EVENT_ID0);
    }
    ```

-   \_\_simt\_vf\_\_函数本身不支持通过函数指针间接调用，经`asc_vf_call()`调用时模板实参在编译期确定。VF内部的\_\_simt\_callee\_\_辅助函数可使用携带callee标识的函数指针，见[取Device函数的地址](#取Device函数的地址)。
-   函数的inline行为由编译器决定，添加的always\_inline或noinline将被忽略。
-   不允许使用结构体作为参数，以下为错误示例。

    ```cpp
    __simt_vf__ __launch_bounds__(1024) inline void foo(__gm__ int* a, __gm__ int* b, __gm__ int* c, struct S s) {
        // error: s表示结构体参数，不允许使用
    }
    ```

<!-- npu="950" id1 -->
### constexpr函数的调用约束

Ascend 950PR&950DT系列产品上，编译器会将Device侧的`constexpr`函数附加上`__callee__`语义，并引入调用关系约束：`constexpr`调用的子函数必须标记为`constexpr`或`__callee__` ，否则校验报错，调用将触发编译报错：

```cpp
__aicore__ inline int32_t plain_add(int32_t a, int32_t b) { return a + b; }

constexpr __aicore__ int32_t cexpr_caller(int32_t x)
{
    return plain_add(x, 1); // 不能调用非constexpr的plain_add
}
```

**正确写法**：Device侧使用的constexpr函数须标记`constexpr __aicore__`，且其调用的函数也须为`constexpr __aicore__`或`__callee__`：

```cpp
// __callee__ int32_t cexpr_add(int32_t a, int32_t b) { return a + b; } 同样可行
constexpr __aicore__ int32_t cexpr_add(int32_t a, int32_t b) { return a + b; }

constexpr __aicore__ int32_t cexpr_caller(int32_t x)
{
    return cexpr_add(x, 1);            // constexpr调用constexpr：可用
}

__global__ __vector__ void probe(__gm__ int32_t* output)
{
    constexpr int32_t v = cexpr_caller(41);   // 编译期折叠为42
    output[0] = v;                            // 运行结果42
}
```
<!-- end id1 -->

### 递归

编译期递归即constexpr求值递归，constexpr函数需要加上函数标记。

以SIMT编程场景为例，`__aicore__ constexpr`递归函数可在核函数内以编译期结果使用：

    ```cpp
    __aicore__ constexpr uint32_t rec_sum(uint32_t n)
    {
        return n == 0u ? 0u : n + rec_sum(n - 1u);   // 编译期递归求值
    }

    __global__ __launch_bounds__(128) void probe(float* output, uint32_t count)
    {
        constexpr uint32_t s10 = rec_sum(10u);        // 先落constexpr变量：编译期折叠为55
        output[threadIdx.x] = static_cast<float>(s10);
    }
    ```

运行时递归仅在SIMT编程场景可用，其他编程场景均不支持。如下所示，SIMD与SIMT混合编程中使用运行时递归触发编译报错：

```cpp
// SIMD与SIMT混合编程的SIMT VF内：ERROR
__simt_callee__ uint32_t rec_sum(uint32_t n)
{
    return n == 0u ? 0u : n + rec_sum(n - 1u);
    // error: recursive aicore function call is not allowed
}
```

### Host函数与Device函数的调用边界

Device侧代码不可以调用或引用Host函数，编译报错`reference to __host__ function`；`__global__`函数不能调用`__global__`函数，`__aicore__`函数也不能回调`__global__`函数，核函数只能由Host侧`<<<>>>`启动，详见[指针与内存地址](#核函数的地址)。

### 可变参数

不保证Device侧的可变参数功能，即使编译未拦截报错，不建议用户在Device代码中使用：

```cpp
__aicore__ int32_t sum_all(int32_t count, ...)   // 签名可编译运行，不建议使用
{
    return count;
}
```

### inline行为

函数的inline行为由编译器决定，inline标记是否生效由编译器确定，不支持用户强制指定inline行为。

## 结构体与类

### SIMD与SIMT混合编程场景结构体使用限制

SIMD与SIMT混合编程场景下，当使用位于Global Memory或Unified Buffer（UB）中的结构体或类拷贝构造新结构体或类时，需要开发者为结构体或类手动实现拷贝构造函数，编译器不会默认隐式生成。

```cpp
struct TestS {
    int a;
    float b;
    // 手动实现Global Memory内存空间的拷贝构造函数
    __callee__ TestS(const __gm__ TestS& other) {
        a = other.a;
        b = other.b;
    }
    // 手动实现UB内存空间的拷贝构造函数
    __callee__ TestS(const __ubuf__ TestS& other) {
        a = other.a;
        b = other.b;
    }
};

__simt_vf__ void simt_kernel(__gm__ TestS *data1, __ubuf__ TestS *data2)
{
    uint32_t idx = threadIdx.x;
    TestS t1 = data1[idx];   // 调用Global Memory拷贝构造函数
    TestS t2 = data2[idx];   // 调用UB拷贝构造函数
    ...
}
```

在异构编译场景中，Host和Device可共用结构体定义代码，但Host侧无法识别`__callee__`、`__gm__`等Device侧关键字，因此无法根据这些关键字实现函数重载。当需要定义多个地址空间重载函数时，需通过宏`__NPU_ARCH__`将相关Device代码隔离，以避免函数定义冲突。`__NPU_ARCH__`是由编译预定义的宏，可用于区分Device侧代码，具体可参考[《毕昇编译器》](https://www.hiascend.com/document/detail/zh/CANNCommunityEdition/latest/compiler/BishengCompiler/atlas_bisheng_10_0001.html)中的“编程语言参考 > AI Core编程指导 > 预定义宏和内建变量”章节。

此外，SIMD与SIMT混合编程场景下不支持直接向Global Memory或UB地址空间写入结构体。

```cpp
struct TestS {
    int a;
    float b;
    //device侧代码隔离
#ifdef __NPU_ARCH__
    __callee__ TestS(const __gm__ TestS& other) {
        a = other.a;
        b = other.b;
    }
    __callee__ TestS(const __ubuf__ TestS& other) {
        a = other.a;
        b = other.b;
    }
#endif
    // host侧代码
    TestS(const TestS& other) {
        a = other.a;
        b = other.b;
    }
};

__simt_vf__ void simt_kernel(__gm__ TestS *data1, __ubuf__ TestS *data2)
{
    uint32_t idx = threadIdx.x;
    TestS t = data1[idx];      // 调用Global Memory拷贝构造函数
    data2[0] = t;               // 不支持，编译报错
    ...
}

__global__ __vector__ void kernel_func(__gm__ TestS *data1, __gm__ TestS *data2)
{
    TestS t = data1[0];         // 调用Global Memory拷贝构造函数
    data2[0] = t;               // 不支持，编译报错
    ...
}
```

### 成员函数的执行空间标记

类的成员函数含静态成员函数，在Device侧被调用时需加上`__aicore__`等函数标记，未标记的成员函数会被识别为Host函数，在核函数内调用报错：

```cpp
struct Holder {
    float value = 3.f;
    __aicore__ float read() const { return value; }   // 标记__aicore__：Device侧可用
};

__global__ __launch_bounds__(128) void probe(float* output, uint32_t count)
{
    Holder h;
    output[threadIdx.x] = h.read() + static_cast<float>(threadIdx.x) * 0.f; // 编译运行正常
}
```

```cpp
struct Holder {
    float value = 3.f;
    float read() const { return value; }              // 未标记：Host函数
};

__global__ __launch_bounds__(128) void probe(float* output, uint32_t count)
{
    Holder h;
    output[threadIdx.x] = h.read();   // 编译报错: reference to __host__ function 'read' in __global__ function
}
```

friend声明的访问权限与C++保持一致，但friend成员函数同样需标记`__aicore__`。

### 虚函数与运行期多态

不支持虚函数与运行期多态，Device侧应避免使用相关特性。

### 成员函数指针

成员函数指针在Device侧可用，指向的成员函数需标记`__aicore__`：

```cpp
struct Holder {
    float value = 3.f;
    __aicore__ float twice() const { return value * 2.f; }
};

__global__ __launch_bounds__(128) void probe(float* output, uint32_t count)
{
    Holder h;
    float (Holder::*mp)() const = &Holder::twice;   // 成员函数指针：可用
    output[threadIdx.x] = (h.*mp)();
}
```

### 运行时类型识别（RTTI）

`dynamic_cast`与运行时typeid不可用：

```cpp
struct PolyBase { __aicore__ virtual ~PolyBase() = default; };
struct PolyDerived : public PolyBase { float w = 9.f; };

__global__ void probe(float* output, uint32_t count)
{
    PolyDerived pd;
    PolyBase* pb = &pd;
    PolyDerived* down = dynamic_cast<PolyDerived*>(pb);   // 链接失败：undefined symbol: __dynamic_cast
    output[threadIdx.x] = down != nullptr ? down->w : -1.f;
}
```

### 动态分配相关的类能力

类的成员函数不支持引入`operator new`、`operator delete`依赖，Device侧不支持动态new、delete、malloc、free，见[不支持的特性](#不支持的特性)。

## 模板

### 模板在Device侧的使用

支持函数模板、类模板、变参模板：`__aicore__`函数模板实例化、核函数内类模板对象构造、`sizeof...`参数包计数、模板模板参数（`template <template <typename> class C>`）实例化：

```cpp
template <typename T>
__aicore__ inline T twice(T v) { return v + v; }

template <typename... Args>
__aicore__ inline uint32_t count_args(Args...)
{
    return sizeof...(Args);
}

__global__ __launch_bounds__(128) void probe(float* output, uint32_t count)
{
    float f = twice(5.5f);                    // 函数模板：11.0
    uint32_t c = count_args(1, 2.f, 'a');     // 变参模板：3
    output[threadIdx.x] = f + static_cast<float>(c);   // 输出14
}
```

模板函数标记随空间选择，如`__aicore__`、`__simt_callee__`、`__simd_callee__`等。

### 模板核函数

核函数本身可以是函数模板，在`<<<>>>`调用时指定模板实参：

```cpp
template <typename T>
__global__ __launch_bounds__(128) void tpl_kernel(T* output, uint32_t count)
{
    if (threadIdx.x >= count) {
        return;
    }
    output[threadIdx.x] = static_cast<T>(threadIdx.x) + static_cast<T>(1);
}

// Host侧：
tpl_kernel<float><<<1, 128, 0, stream>>>(dev, 128);
```

也支持自动推导：

```cpp
tpl_kernel<float><<<1, 128, 0, stream>>>(dev, 128);   // 显式指定：可用
tpl_kernel<<<1, 128, 0, stream>>>(dev, 128);          // 自动推导T=float：同样可用
```

### VF模板函数

VF模板函数无需显式实例化声明，模板经`asc_vf_call()`调用时显式指定实参即可完成实例化，示例代码如下：

```cpp
template <typename T>
__simt_vf__ __launch_bounds__(128) inline void vf_scale(__ubuf__ T* out, __gm__ T* in, uint32_t count, T factor)
{
    if (threadIdx.x >= count) {
        return;
    }
    out[threadIdx.x] = in[threadIdx.x] * factor;
}

__global__ __vector__ void kernel_func(__gm__ float* input, __gm__ float* output, uint32_t count)
{
    __ubuf__ float ub[128];
    // 调用点显式指定模板实参（vf_scale<float>）即完成实例化，无需显式实例化声明
    asc_vf_call<vf_scale<float>>(dim3(128), ub, input, count, 2.f);
    asc_sync_notify(PIPE_V, PIPE_MTE3, EVENT_ID0);
    asc_sync_wait(PIPE_V, PIPE_MTE3, EVENT_ID0);
    asc_copy_ub2gm_align(output, ub, 1, count * sizeof(float),
                         asc_store_l2_cache_mode::NORMAL_FIRST_VICTIM, 0, 0);
}
```

以同一模板按不同实参多次调用，如`asc_vf_call<vf_scale<float>>`与`asc_vf_call<vf_scale<int32_t>>`，会生成相互独立的实例，均无需额外声明。

`asc_vf_call()`**不支持省略模板实参由编译器自动推导**。`asc_vf_call()`的模板形参为非类型模板参数即函数指针常量`template <auto funcPtr, ...>`，显式实参必须是一个完整的函数实体，无法只写VF模板名`asc_vf_call<vf_scale>`让编译器从调用实参推导`T`：

```cpp
asc_vf_call<vf_scale>(dim3(THREAD_COUNT), ub_out, input, count, 2.f);
// error: no matching function for call to asc_vf_call
// note: candidate template ignored: invalid explicitly-specified argument
//       for template parameter funcPtr
```

须显式指定完整实例，如`asc_vf_call<vf_scale<float>>`。

### 模板默认实参中的设备内置变量

Device侧模板函数（含VF模板函数与核函数模板）的**非类型模板参数默认值中不可以使用`warpSize`、`laneid()`、`blockIdx`等设备内置变量**，此类变量只能在函数体内使用。

```cpp
template <int kNumThreads,
          int kNumWarps = kNumThreads / warpSize>   // ERROR: use of undeclared identifier 'warpSize'
__simt_vf__ __launch_bounds__(kNumThreads) inline void vf_func(__gm__ int* out, uint32_t count)
{
    out[threadIdx.x] = kNumWarps;
}
```

正确写法为模板默认实参使用字面常量或`constexpr`变量，函数体内再使用内置变量：

```cpp
template <int thread_num = 128>    // 默认值使用字面常量（也可使用constexpr变量）：编译通过
__simt_vf__ __launch_bounds__(thread_num) inline void vf_func(__gm__ int* out, uint32_t count)
{
    const int32_t warp_num = thread_num / warpSize;
    const int32_t warp_idx = threadIdx.x / warpSize;    // 函数体内使用warpSize：可用
    out[threadIdx.x] = warp_num + warp_idx;
}
```

该限制对`__simt_vf__`、`__simt_callee__`与核函数的模板函数均成立。
