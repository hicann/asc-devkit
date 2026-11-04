# Function<a name="ZH-CN_TOPIC_0000002541491859"></a>

<!-- md-trans-meta sourceCommit=f83e857605e3844db8b9de0442007cc115278a47 translatedAt=2026-10-28T09:40:48.558Z pushedAt=2026-11-03T07:08:03.579Z nl=2 -->

## Kernel Argument List Constraints<a name="section_kernel_function_argument_list_constraint"></a>

The argument list of a kernel supports only the following types:

- Basic data types, such as int32\_t, float, and so on.
- Structures composed of basic data types. Nested structures are supported, but they must be POD (Plain Old Data) types.
- Pointer types to basic data types, such as int32\_t\*, float\*, and so on. In fact, these pointers point to Global Memory.

## SIMT VF Function Restrictions<a name="section_simt_vf_constraint"></a>

SIMT VF functions marked with \_\_simt\_vf\_\_ must comply with the following constraints and restrictions:

-   Scalar parameters must not use pointers or references.
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

-   If a multi-level pointer is passed as a parameter, accessing the inner stack address through the pointer is not allowed. The following is an incorrect example.

    ```cpp
    __simt_vf__ __launch_bounds__(1024) inline void foo(__gm__ int* a, __gm__ int* b, __gm__ int* c, __ubuf__ uint64_t* s) {
        int idx = blockIdx.x * blockDim.x + threadIdx.x;
        int* stack = (int*)(s[0]);
        // error: *stack reads from a multi-level pointer, which is not allowed
        a[idx] = b[idx] + c[idx] + *stack;
    }
    __global__ __aicore__ void foo(__gm__ int* a, __gm__ int* b, __gm__ int* c) {
        int stack = 0;
        __ubuf__ uint64_t* s = ...;
        s[0] = &stack;
        asc_vf_call<foo>(dim3{256}, a, b, c, s);
    }
    ```

-   Indirect calls through function pointers are not supported. The called \_\_simt\_vf\_\_ function must be determined at compile time.
-   The inline behavior of a function is determined by the compiler. The added always\_inline or noinline is ignored.
-   Structures must not be used as parameters. The following is an incorrect example.

    ```cpp
    __simt_vf__ __launch_bounds__(1024) inline void foo(__gm__ int* a, __gm__ int* b, __gm__ int* c, struct S s) {
        // error: s is a structure parameter, which is not allowed
    }
    ```

## Restrictions on Using Structs in SIMD and SIMT Mixed Programming Scenarios

In SIMD and SIMT mixed programming scenarios, when a struct or class located in Global Memory or Unified Buffer (UB) is used to copy-construct a new struct or class, developers must manually implement the copy constructor for the struct or class. The compiler does not implicitly generate one by default.

```cpp
struct TestS {
    int a;
    float b;
    // Manually implement the copy constructor for the Global Memory space
    __callee__ TestS(const __gm__ TestS& other) {
        a = other.a;
        b = other.b;
    }
    // Manually implement the copy constructor for the UB space
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

In heterogeneous compilation scenarios, Host and Device can share the struct definition code. However, the Host side cannot recognize Device-side keywords such as `__callee__` and `__gm__`, and therefore cannot implement function overloading based on these keywords. When multiple address-space overloaded functions need to be defined, use the `__NPU_ARCH__` macro to isolate the relevant Device code to avoid function definition conflicts. `__NPU_ARCH__` is a macro predefined by the compiler and can be used to distinguish Device-side code. For details, see the "Basic Programming Guide > AI Core Programming Guide > Predefined Macros and Built-in Variables" section in [BiSheng Compiler](https://www.hiascend.com/document/detail/en/CANNCommunityEdition/920/compiler/BishengCompiler/atlas_bisheng_10_0001.html).

In addition, in SIMD and SIMT mixed programming scenarios, directly writing a struct to the Global Memory or UB address space is not supported.

```cpp
struct TestS {
    int a;
    float b;
    // Isolate the device-side code
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
    TestS t1 = data1[idx];      // Call the Global Memory copy constructor.
    data2[0] = t;               // Not supported. A compilation error is reported.
    ...
}

__global__ __vector__ void kernel_func(__gm__ TestS *data1, __gm__ TestS *data2)
{
    TestS t = data1[0];         // Call the Global Memory copy constructor.
    data2[0] = t;               // Not supported. A compilation error is reported.
    ...
}
```
