# Function<a name="ZH-CN_TOPIC_0000002541491859"></a>

<!-- md-trans-meta sourceCommit=a5f9b8d2cc4861b1828236bf8fd36c72102b875e translatedAt=2026-08-26T12:42:03.276Z -->

## Kernel Function Parameter List Constraints<a name="section_kernel_function_argument_list_constraint"></a>

The argument list of a kernel function supports only the following types:

- Basic data types, such as int32\_t and float.
- Structures composed of basic data types. Nested structures are supported, but they must be POD (Plain Old Data) types.
- Pointer types to basic data types, such as int32\_t\* and float\*. These pointers actually point to Global Memory.

## SIMT VF Function Constraints<a name="section_simt_vf_constraint"></a>

SIMT VF functions marked with \_\_simt\_vf\_\_ must comply with the following constraints and restrictions:

-   Scalar parameters must not use pointers or references.
-   Stack arrays must not be passed as parameters to a SIMT VF function. Pointer parameters of a SIMT VF function must explicitly use the `__gm__` or `__ubuf__` address space qualifier. The following incorrect example causes a compilation error.

    ```
    __simt_vf__ __launch_bounds__(1024) inline void foo(__gm__ int* a, __gm__ int* b, __gm__ int* c, int* array) {
        int idx = blockIdx.x * blockDim.x + threadIdx.x;
        a[idx] = b[idx] + c[idx] + array[0];
    }
    __global__ __aicore__ void foo(__gm__ int* a, __gm__ int* b, __gm__ int* c) {
        int array[5] = {0,1,2,3,4};
        asc_vf_call<foo>(dim3{256}, a, b, c, array); // error: cannot initialize a parameter of type '__gm__ int *' with an lvalue of type 'int[5]'
    }
    ```

-   If multi-level pointers appear in the passed parameters, accessing the inner stack address pointer is not allowed. The following is an incorrect example.

    ```
    __simt_vf__ __launch_bounds__(1024) inline void foo(__gm__ int* a, __gm__ int* b, __gm__ int* c, __ubuf__ uint64_t* s) {
        int idx = blockIdx.x * blockDim.x + threadIdx.x;
        int* stack = (int*)(s[0]);
        // error: *stack indicates reading from a multi-level pointer, which is not allowed
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

    ```
    __simt_vf__ __launch_bounds__(1024) inline void foo(__gm__ int* a, __gm__ int* b, __gm__ int* c, struct S s) {
        // error: s indicates a structure parameter, which is not allowed
    }
    ```
