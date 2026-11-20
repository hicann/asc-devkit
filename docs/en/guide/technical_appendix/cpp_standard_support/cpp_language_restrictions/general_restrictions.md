# C/C++ General Syntax Restrictions

<!-- md-trans-meta sourceCommit=1441ba26dce83a523b232c23a0a1b369532d9b38 translatedAt=2026-11-09T11:13:15.282Z pushedAt=2026-11-19T06:32:20.004Z nl=2 -->

## Unsupported Features

-   Virtual functions are not supported.
-   Virtual inheritance is not supported.
-   Dynamic memory allocation is not supported, that is, malloc, free, new, and delete are unavailable.
-   STL is not supported.
-   Runtime typeid is not supported.
-   File system I/O is not supported.
-   Tuple and algorithm operations in the standard library are not supported. Related library function calls need to be marked with aicore.

## Pointers and Memory Addresses

Pointer dereferencing, such as `*pointer`, `pointer->member`, and `pointer[0]`, is not allowed across address spaces: GM and UB pointers on the device side cannot be dereferenced on the host side, and Host stack or global addresses cannot be dereferenced on the device side. This is because the Host-side and Device-side address spaces are not visible to each other, and such cross-space access is prohibited at the programming model level.

### Taking the Address of a Device Function

- Taking the address of a `__aicore__` function, storing it in a function pointer, and calling it through the function pointer is allowed. The code compiles successfully and produces correct results:

    ```cpp
    __aicore__ inline float scale2(float v) { return v * 2.f; }

    __global__ __launch_bounds__(128) void probe(float* output, uint32_t count)
    {
        float (*fn)(float) = scale2;          // Take the address of a __aicore__ function: allowed
        output[threadIdx.x] = fn(static_cast<float>(threadIdx.x));
    }
    ```

- Callee functions in SIMT VF and SIMD VF can be called indirectly through function pointers, but the function pointer type must carry the same callee identifier as the function.

    The `__simt_callee__` and `__simd_callee__` markers attach function attributes to the function type. Converting to an ordinary function pointer reports an error:

    ```cpp
    __simt_callee__ float scale2(float v) { return v * 2.f; }

    __simt_vf__ __launch_bounds__(128) inline void vf_func(__ubuf__ float* out, uint32_t count)
    {
        float (*fn)(float) = scale2;
        // error: cannot initialize a variable of type 'float (*)(float)' with an lvalue of type
        //        '__simt_callee__ float (float) __attribute__((cce_simt_callable))'
    }
    ```

    After adding the callee identifier to the function pointer type, conversion and invocation work normally. Function pointers in SIMT VF use the `__simt_callee__` identifier, while those in SIMD VF use the `__simd_callee__` identifier:

    ```cpp
    __simt_callee__ float scale2(float v) { return v * 2.f; }

    __simt_vf__ __launch_bounds__(128) inline void vf_func(__ubuf__ float* out, uint32_t count)
    {
        __simt_callee__ float (*fn)(float) = scale2;   // The pointer type carries the callee identifier
        out[threadIdx.x] = fn(21.f);                   // Compiles and runs correctly, outputting 42
    }
    ```

- SIMT VF and SIMD VF functions do not support invocation or launch through function pointers. They can only be called through the `asc_vf_call()` API.

- Referencing the address of a Host function is not allowed: referencing a Host function inside a kernel function, including direct calls and function pointers, causes a compilation error:

    ```cpp
    float host_fn(float v) { return v + 100.f; }

    __global__ __launch_bounds__(128) void probe(float* output, uint32_t count)
    {
        float (*fp)(float) = host_fn;         // ERROR: reference to __host__ function 'host_fn' in __global__ function
        output[threadIdx.x] = fp(1.f);
    }
    ```
- Device code is statically linked with the kernel function at compile time, and the mechanism of dynamically loading Device functions at runtime using `dlopen` is not supported.

### Kernel Function Address

`__global__` kernel functions cannot call each other, nor can the address of a kernel function be referenced within a kernel function. A kernel function can only be launched by the host side through `<<<>>>`:

```cpp
__global__ void kernel_a(float* output) { ... }

__global__ __launch_bounds__(128) void probe(float* output, uint32_t count)
{
    void* p = reinterpret_cast<void*>(&kernel_a);  // ERROR: reference to __global__ function in __global__ function
    output[threadIdx.x] = p == nullptr ? 0.f : 1.f;
}
```

### Pointer Arithmetic and Byte Reinterpretation

Pointer size comparison and `reinterpret_cast` byte reinterpretation are supported only within the same address space. These two operations are not supported for pointers across different address spaces. Usage within the same address space is as follows:

```cpp
__global__ __launch_bounds__(128) void probe(float* output, uint32_t count)
{
    float* p1 = output + threadIdx.x;
    float* p2 = output + (count - 1 - threadIdx.x);
    bool cmp = p1 < p2;                                    // GM pointer comparison: supported
    int32_t* pi = reinterpret_cast<int32_t*>(p1);          // Byte reinterpretation: supported
    pi[0] = static_cast<int32_t>(cmp ? 1 : 0);
}
```

Pointer arithmetic and `reinterpret_cast` byte reinterpretation across address spaces trigger a compilation error:

-   Comparing the sizes of pointers in different address spaces, for example, `__gm__`, `__ubuf__`, and stack addresses cannot be compared by size:

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

-   `reinterpret_cast` byte reinterpretation of pointers in different address spaces:

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

> [!NOTE] Note
> The preceding restrictions apply to pointers **with address space qualifiers**, that is, comparisons and conversions among `__gm__`, `__ubuf__`, and stack addresses. In SIMT programming scenarios, pointer parameters have no address space qualifier concept, and GM pointers and stack variable pointers within a kernel can be directly compared and `reinterpret_cast`.

## Variables

### static Local Variables

Static local variables can be declared in kernel functions and `__aicore__` functions, but **the read/write results across multiple cores and threads are not guaranteed to be deterministic**, for the following reasons:

-   **Storage location**: A static local variable resides in the global data area in GM and is shared by all execution instances of the same kernel function, that is, by all cores and all threads, rather than being thread-private.
-   **Concurrent contention across cores**: When multiple cores execute the same kernel function in parallel, read-modify-write operations on the same static variable, such as `counter += 1`, are interleaved without atomicity or mutual exclusion guarantees, and the final value depends on the execution timing of each core.
-   **Memory consistency**: Modifications to a static variable by each core lack cross-core memory visibility guarantees, so other cores may read the old value before the modification.

As shown below, the computation result is not deterministic:

```cpp
__global__  void static_life(float* output, uint32_t count)
{
    if (threadIdx.x >= count) {
        return;
    }
    static int32_t counter = 0;
    counter += 1;
    output[threadIdx.x] = static_cast<float>(counter); // The output result is not deterministic.
}
```
In addition, static local variables do not allow dynamic initialization; the initialization expression must be a compile-time constant:

```cpp
__global__ void probe(float* output, uint32_t count)
{
    static int32_t v1 = 11;           // CORRECT: constant initialization
    // static int32_t v2 = count;     // Compilation error
    output[threadIdx.x] = static_cast<float>(v1);
}
```

SIMT VF and SIMD VF do not support static local variables:

```cpp
__simt_vf__ __launch_bounds__(128) inline void vf_func(__ubuf__ float* out, uint32_t count)
{
    static int32_t counter = 0;    // Compilation error
    counter += 1;
    out[threadIdx.x] = static_cast<float>(counter);
}
```

### Global Variables and namespace-scope Variables

Writable global variables include namespace-scope `static` variables and ordinary global variables initialized with non-constant values. Such variables are not supported on the device side if they have no address space identifier. To use a writable global variable on the device side, add the `__gm__` identifier to the variable declaration. The variable is then actually stored in Global Memory:

-   Without the `__gm__` identifier: a compilation error is reported on the device side.

    ```cpp
    int32_t g_counter = 10;           // No identifier: unavailable on the device side

    __global__ __launch_bounds__(128) void probe(uint32_t* output)
    {
        int32_t acc = g_counter;      // Compilation error
        output[threadIdx.x] = static_cast<uint32_t>(acc);
    }
    ```

-   With the `__gm__` identifier, the variable can be used normally on the device side.

    ```cpp
    __gm__ int32_t g_counter = 10;    // __gm__ marker: available on the device side

    __global__ __launch_bounds__(128) void probe(uint32_t* output)
    {
        int32_t acc = g_counter;            // Compiles and runs normally
        output[threadIdx.x] = static_cast<uint32_t>(acc);
    }
    ```

### const Qualified Variables

A const-qualified variable declared at global, namespace, or class scope is a Host variable by default, unless it carries a Device-side memory space annotation. On the Ascend C device side, a const global variable initialized with a constant expression can be directly used, referenced, and have its address taken:

```cpp
const int ConstVar = 10;                  // Global const initialized with a constant expression
const float ConstFloatVar = 5.0f;

__global__ __launch_bounds__(128) void probe(float* output, uint32_t count)
{
    int arr[ConstVar];                    // CORRECT: built-in integer used directly as an array dimension
    float v = ConstFloatVar;              // CORRECT: floating-point const used directly
    const int& ref = ConstVar;            // CORRECT: reference to a global const variable
    const int* p = &ConstVar;             // CORRECT: take the address of a global const variable
    output[threadIdx.x] = static_cast<float>(arr[ConstVar - 1]) + v
                        + static_cast<float>(ref) * 0.8f + static_cast<float>(*p) * 0.6f;
}
```

A const variable declared with extern and not initialized with a constant expression in the current compilation unit cannot be used in a kernel function:

```cpp
extern const int ExternVar;               // No constant initializer in this unit

__global__ void probe(float* output, uint32_t count)
{
    output[threadIdx.x] = static_cast<float>(ExternVar);
    // ERROR: reference to __host__ variable 'ExternVar' in __global__ function
}
```

> [!NOTE] Note
> Starting from C++14, it is recommended to use constexpr variables instead of const-qualified variables. Starting from C++17, inline constexpr variables can also be used. constexpr variables are not subject to type restrictions and are semantically explicit compile-time constants. Their behavior on the device side is consistent with that of const.

### volatile Qualified Variables

The volatile keyword is supported for ISO C++ compatibility. volatile reads and writes do not provide inter-thread synchronization guarantees, nor do they guarantee that the number of memory accesses performed by the hardware matches the number of instructions. The compiler does not merge or omit volatile accesses, but accesses to the same volatile object by multiple threads have no ordering or visibility guarantees.

Take polling a GM flag in a for loop as an example: a volatile read actually reloads from GM in every loop iteration instead of being cached in a register, so the loop can continuously observe the latest content of GM:

```cpp
__global__ __launch_bounds__(128) void probe(int* flag, float* output, uint32_t count)
{
    if (threadIdx.x == 0) {
        // Poll the GM flag: the volatile pointer actually reads from GM in every loop iteration.
        volatile int* vflag = flag;
        int32_t spins = 0;
        for (uint32_t i = 0; i < 1000u; ++i) {
            spins += *vflag;                 // Actually read GM in every iteration, without being merged or omitted.
        }
        output[0] = static_cast<float>(spins);
    }
}
```

If volatile is removed, the compiler may merge multiple reads within the loop into a single load or even eliminate them entirely, so modifications to that address by other execution units will not be obtained in time. However, volatile only guarantees that the access is not optimized away; it does not guarantee reading the latest value written by other cores or other threads. This is because modifications to GM by different cores lack cross-core visibility and ordering guarantees, and polling may still read a stale value.

### constexpr Global Read-Only Table

A POD constexpr global array initialized at compile time can be indexed at runtime inside a kernel function, and taking its address for use as a pointer is also supported:

```cpp
constexpr uint32_t TABLE[4] = {1u, 2u, 4u, 8u};

__global__ __launch_bounds__(128) void probe(float* output, uint32_t count)
{
    float acc = 0.f;
    for (uint32_t i = 0; i < 4u; ++i) {
        acc += static_cast<float>(TABLE[i]);   // Index a constexpr table at runtime: supported
    }
    const uint32_t* tp = &TABLE[0];            // Take the address of a constexpr table: supported
    output[threadIdx.x] = acc + static_cast<float>(tp[threadIdx.x % 4u]);
}
```

**Array subscript out-of-bounds is not intercepted by the compiler and is undefined behavior**. Out-of-bounds at runtime subscripts, out-of-bounds at compile-time constant subscripts, and out-of-bounds through pointer arithmetic (`*(tp + idx)`) produce no warning and no error at compile time. Users must implement out-of-bounds protection themselves.

> [!NOTE] Note
> In SIMT VF of SIMD and SIMT hybrid programming, **directly** indexing a constexpr global table with a runtime subscript triggers a compilation error. When a global table needs to be indexed inside a VF, first copy the table elements to a local array of the VF function and then index it.

### thread_local Storage

The `thread_local` storage is not supported: in SIMT programming scenarios, local variables of a kernel function are inherently thread-private, so `thread_local` is usually unnecessary.

## Functions

### Kernel Function Parameter List Restrictions<a name="section_kernel_func_param_restriction"></a>

The argument list of a kernel supports only the following types:

- Basic data types, such as int32\_t, float, and so on.
- Structures composed of basic data types. Nested structures are supported, but they must be POD (Plain Old Data) types.
- Pointer types to basic data types, such as int32\_t\*, float\*, and so on. In fact, these pointers point to Global Memory.

### SIMT VF Function Restrictions

SIMT VF functions marked with \_\_simt\_vf\_\_ must comply with the following constraints and restrictions:

-   Pointer and reference types are not allowed for scalar parameters: a pointer parameter must explicitly carry the `__gm__` or `__ubuf__` address space qualifier to identify that it points to GM or UB memory, and reference parameters are not supported at all. Scalar data is passed by value; when a scalar result needs to be output, it is written out through a pointer parameter. The following incorrect examples cause compile-time errors:

    ```cpp
    // Incorrect example 1: scalar pointer parameter (without address space qualifier)
    __simt_vf__ __launch_bounds__(1024) inline void foo(__ubuf__ float* out, uint32_t count, float* scalarPtr) {
        out[threadIdx.x] = *scalarPtr;
        // error: pointer arguments to simt_vf functions must reside in '__gm__ or __ubuf__' address space
    }

    // Incorrect example 2: scalar reference parameter
    __simt_vf__ __launch_bounds__(1024) inline void foo(__ubuf__ float* out, uint32_t count, const float& scale) {
        out[threadIdx.x] = static_cast<float>(threadIdx.x) * scale;
        // error: pointer arguments to simt_vf functions must reside in '__gm__ or __ubuf__' address space
    }
    ```

    **Correct usage**: pass scalars by value; pass pointers qualified with `__gm__` or `__ubuf__`; when a scalar result needs to be output, write it out through a pointer parameter:

    ```cpp
    // Pass scalar by value + pointer with address space qualifier
    __simt_vf__ __launch_bounds__(1024) inline void foo(__ubuf__ float* out, __gm__ float* in, uint32_t count, float scale) {
        uint32_t idx = blockIdx.x * blockDim.x + threadIdx.x;
        out[idx] = in[idx] * scale;              // scale is a value copy
    }

    // Output scalar result: write out through a __ubuf__ pointer parameter instead of a reference output
    __simt_vf__ __launch_bounds__(1024) inline void foo(__ubuf__ float* out, __gm__ float* in, uint32_t count, __ubuf__ float* scalarOut) {
        uint32_t idx = blockIdx.x * blockDim.x + threadIdx.x;
        out[idx] = in[idx];
        if (threadIdx.x == 0) {
            scalarOut[0] = in[0] * 2.f;          // Write the scalar result to UB, read back by the kernel function side
        }
    }
    ```
-   Stack arrays must not be passed as parameters to SIMT VF functions. Pointer parameters of SIMT VF functions must explicitly use the `__gm__` or `__ubuf__` address space qualifier. The following incorrect example causes a compilation error.

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

-   Accessing the stack space of MainScalar is prohibited inside a VF.

    After the stack address of MainScalar is passed into a VF function through a `__ubuf__` pointer, dereferencing it inside the VF is an invalid access: reading or writing at runtime is undefined behavior and constitutes a silent error that is difficult to locate. The following is an incorrect example:

    ```cpp
    __simt_vf__ __launch_bounds__(1024) inline void foo(__gm__ int* out, __ubuf__ uint64_t* s, uint32_t count) {
        if (threadIdx.x >= count) {
            return;
        }
        int* stack = (int*)(s[0]);          // Compiles successfully: obtains the MainScalar stack address from a multi-level pointer
        out[threadIdx.x] = *stack;          // At runtime, reads unrelated memory content instead of the stack value on the stack
    }

    __global__ __vector__ void kernel_func(__gm__ int* output, uint32_t count)
    {
        int stack = 123;                    // MainScalar stack variable
        __ubuf__ uint64_t sbuf[1];
        sbuf[0] = (uint64_t)&stack;         // The stack address is passed into the VF through a UB pointer
        asc_vf_call<foo>(dim3(1024), output, sbuf, count);
        asc_sync_notify(PIPE_V, PIPE_MTE3, EVENT_ID0);
        asc_sync_wait(PIPE_V, PIPE_MTE3, EVENT_ID0);
    }
    ```

-   \_\_simt\_vf\_\_ The function itself does not support indirect invocation through a function pointer. When invoked through `asc_vf_call()`, the template arguments are determined at compile time. The \_\_simt\_callee\_\_ helper functions inside the VF can use a function pointer carrying the callee identifier. For details, see [Taking the Address of a Device Function](#taking-the-address-of-a-device-function).
-   The inline behavior of a function is determined by the compiler. The added always\_inline or noinline is ignored.
-   Structures must not be used as parameters. The following is an incorrect example.

    ```cpp
    __simt_vf__ __launch_bounds__(1024) inline void foo(__gm__ int* a, __gm__ int* b, __gm__ int* c, struct S s) {
        // error: s represents a struct parameter and is not allowed to be used
    }
    ```

### Recursion

Compile-time recursion is constexpr evaluation recursion. A constexpr function must carry the function marker.

Taking the SIMT programming scenario as an example, a `__aicore__ constexpr` recursive function can be used as a compile-time result inside a kernel function:

    ```cpp
    __aicore__ constexpr uint32_t rec_sum(uint32_t n)
    {
        return n == 0u ? 0u : n + rec_sum(n - 1u);   // Compile-time recursive evaluation
    }

    __global__ __launch_bounds__(128) void probe(float* output, uint32_t count)
    {
        constexpr uint32_t s10 = rec_sum(10u);        // First assign to a constexpr variable: folded to 55 at compile time
        output[threadIdx.x] = static_cast<float>(s10);
    }
    ```

Runtime recursion is available only in the SIMT programming scenario and is not supported in other programming scenarios. As shown below, using runtime recursion in SIMD and SIMT hybrid programming triggers a compilation error:

```cpp
// Inside the SIMT VF of SIMD and SIMT hybrid programming: ERROR
__simt_callee__ uint32_t rec_sum(uint32_t n)
{
    return n == 0u ? 0u : n + rec_sum(n - 1u);
    // error: recursive aicore function call is not allowed
}
```

### Calling Boundary Between Host Functions and Device Functions

Device-side code cannot call or reference Host functions; otherwise, the compiler reports the error `reference to __host__ function`. A `__global__` function cannot call another `__global__` function, and an `__aicore__` function cannot call back a `__global__` function. A kernel function can only be launched from the host side using `<<<>>>`. For details, see [Pointers and Memory Addresses](#kernel-function-address).

### Variadic Arguments

Variadic argument support on the device side is not guaranteed. Even if the compiler does not report an error, you are not advised to use variadic arguments in Device code:

```cpp
__aicore__ int32_t sum_all(int32_t count, ...)   // The signature can be compiled and run, but its use is not recommended.
{
    return count;
}
```

### inline Behavior

The inline behavior of a function is determined by the compiler. Whether the inline marker takes effect is determined by the compiler, and users are not supported to forcibly specify the inline behavior.

## Structures and Classes

### Restrictions on Using Structures in SIMD and SIMT Hybrid Programming Scenarios

In SIMD and SIMT hybrid programming scenarios, when a struct or class located in Global Memory or Unified Buffer (UB) is used to copy-construct a new struct or class, developers must manually implement the copy constructor for the struct or class. The compiler does not implicitly generate one by default.

```cpp
struct TestS {
    int a;
    float b;
    // Manually implement the copy constructor for the Global Memory address space
    __callee__ TestS(const __gm__ TestS& other) {
        a = other.a;
        b = other.b;
    }
    // Manually implement the copy constructor for the UB address space
    __callee__ TestS(const __ubuf__ TestS& other) {
        a = other.a;
        b = other.b;
    }
};

__simt_vf__ void simt_kernel(__gm__ TestS *data1, __ubuf__ TestS *data2)
{
    uint32_t idx = threadIdx.x;
    TestS t1 = data1[idx];   // Call the Global Memory copy constructor
    TestS t2 = data2[idx];   // Call the UB copy constructor
    ...
}
```

In heterogeneous compilation scenarios, Host and Device can share the struct definition code. However, the host side cannot recognize Device-side keywords such as `__callee__` and `__gm__`, and therefore cannot implement function overloading based on these keywords. When multiple address-space overloaded functions need to be defined, use the `__NPU_ARCH__` macro to isolate the related Device code to avoid function definition conflicts. `__NPU_ARCH__` is a macro predefined by the compiler and can be used to distinguish Device-side code. For details, see the "Programming Language Reference > AI Core Programming Guide > Predefined Macros and Built-in Variables" section in [BiSheng Compiler](https://www.hiascend.com/document/detail/en/CANNCommunityEdition/latest/compiler/BishengCompiler/atlas_bisheng_10_0001.html).

In addition, in SIMD and SIMT hybrid programming scenarios, directly writing a struct to the Global Memory or UB address space is not supported.

```cpp
struct TestS {
    int a;
    float b;
    // Isolate the Device-side code
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
    // Host-side code
    TestS(const TestS& other) {
        a = other.a;
        b = other.b;
    }
};

__simt_vf__ void simt_kernel(__gm__ TestS *data1, __ubuf__ TestS *data2)
{
    uint32_t idx = threadIdx.x;
    TestS t = data1[idx];      // Call the Global Memory copy constructor
    data2[0] = t;               // Not supported; a compilation error is reported
    ...
}

__global__ __vector__ void kernel_func(__gm__ TestS *data1, __gm__ TestS *data2)
{
    TestS t = data1[0];         // Call the Global Memory copy constructor.
    data2[0] = t;               // Not supported. A compilation error is reported.
    ...
}
```

### Execution Space Markers of Member Functions

When member functions of a class, including static member functions, are called on the device side, function markers such as `__aicore__` must be added. A member function without a marker is identified as a Host function, and calling it inside a kernel function reports an error:

```cpp
struct Holder {
    float value = 3.f;
    __aicore__ float read() const { return value; }   // Marked with __aicore__: available on the device side
};

__global__ __launch_bounds__(128) void probe(float* output, uint32_t count)
{
    Holder h;
    output[threadIdx.x] = h.read() + static_cast<float>(threadIdx.x) * 0.f; // Compiles and runs normally
}
```

```cpp
struct Holder {
    float value = 3.f;
    float read() const { return value; }              // Not marked: Host function
};

__global__ __launch_bounds__(128) void probe(float* output, uint32_t count)
{
    Holder h;
    output[threadIdx.x] = h.read();   // Compilation error: reference to __host__ function 'read' in __global__ function
}
```

The access permissions of friend declarations remain consistent with C++, but friend member functions must also be marked with `__aicore__`.

### Virtual Functions and Runtime Polymorphism

Virtual functions and runtime polymorphism are not supported. Avoid using related features on the device side.

### Member Function Pointer

A member function pointer is available on the device side, and the member function it points to must be marked with `__aicore__`:

```cpp
struct Holder {
    float value = 3.f;
    __aicore__ float twice() const { return value * 2.f; }
};

__global__ __launch_bounds__(128) void probe(float* output, uint32_t count)
{
    Holder h;
    float (Holder::*mp)() const = &Holder::twice;   // Member function pointer: available
    output[threadIdx.x] = (h.*mp)();
}
```

### Runtime Type Identification (RTTI)

`dynamic_cast` and runtime typeid are not available:

```cpp
struct PolyBase { __aicore__ virtual ~PolyBase() = default; };
struct PolyDerived : public PolyBase { float w = 9.f; };

__global__ void probe(float* output, uint32_t count)
{
    PolyDerived pd;
    PolyBase* pb = &pd;
    PolyDerived* down = dynamic_cast<PolyDerived*>(pb);   // Link failure: undefined symbol: __dynamic_cast
    output[threadIdx.x] = down != nullptr ? down->w : -1.f;
}
```

### Class Capabilities Related to Dynamic Allocation

Member functions of a class do not support introducing dependencies on `operator new` and `operator delete`. Dynamic `new`, `delete`, `malloc`, and `free` are not supported on the device side. For details, see [Unsupported Features](#unsupported-features).

## Templates

### Template Use on the Device Side

Function templates, class templates, and variadic templates are supported: instantiation of `__aicore__` function templates, construction of class template objects inside kernel functions, `sizeof...` parameter pack counting, and instantiation of template template parameters (`template <template <typename> class C>`):

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
    float f = twice(5.5f);                    // Function template: 11.0
    uint32_t c = count_args(1, 2.f, 'a');     // Variadic template: 3
    output[threadIdx.x] = f + static_cast<float>(c);   // Output 14
}
```

Template function markers are selected based on the space, such as `__aicore__`, `__simt_callee__`, and `__simd_callee__`.

### Template Kernel Function

A kernel function itself can be a function template. Specify the template arguments when invoking it with `<<<>>>`:

```cpp
template <typename T>
__global__ __launch_bounds__(128) void tpl_kernel(T* output, uint32_t count)
{
    if (threadIdx.x >= count) {
        return;
    }
    output[threadIdx.x] = static_cast<T>(threadIdx.x) + static_cast<T>(1);
}

// Host side:
tpl_kernel<float><<<1, 128, 0, stream>>>(dev, 128);
```

Automatic deduction is also supported:

```cpp
tpl_kernel<float><<<1, 128, 0, stream>>>(dev, 128);   // Explicit specification: available
tpl_kernel<<<1, 128, 0, stream>>>(dev, 128);          // Automatic deduction T=float: also available
```

### VF Template Function

A VF template function does not require an explicit instantiation declaration. The template is instantiated by explicitly specifying the template arguments when it is called through `asc_vf_call()`. The following is an example:

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
    // Explicitly specify the template argument (vf_scale<float>) at the call site to complete instantiation; no explicit instantiation declaration is required
    asc_vf_call<vf_scale<float>>(dim3(128), ub, input, count, 2.f);
    asc_sync_notify(PIPE_V, PIPE_MTE3, EVENT_ID0);
    asc_sync_wait(PIPE_V, PIPE_MTE3, EVENT_ID0);
    asc_copy_ub2gm_align(output, ub, 1, count * sizeof(float),
                         asc_store_l2_cache_mode::NORMAL_FIRST_VICTIM, 0, 0);
}
```

Calling the same template multiple times with different arguments, such as `asc_vf_call<vf_scale<float>>` and `asc_vf_call<vf_scale<int32_t>>`, generates mutually independent instances, neither of which requires an additional declaration.

`asc_vf_call()` **does not support omitting the template argument and letting the compiler deduce it automatically**. The template parameter of `asc_vf_call()` is a non-type template parameter, that is, a function pointer constant `template <auto funcPtr, ...>`. The explicit argument must be a complete function entity, so you cannot write only the VF template name `asc_vf_call<vf_scale>` and let the compiler deduce `T` from the call arguments:

```cpp
asc_vf_call<vf_scale>(dim3(THREAD_COUNT), ub_out, input, count, 2.f);
// error: no matching function for call to asc_vf_call
// note: candidate template ignored: invalid explicitly-specified argument
//       for template parameter funcPtr
```

You must explicitly specify the complete instance, such as `asc_vf_call<vf_scale<float>>`.
