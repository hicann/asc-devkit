# SIMD&SIMT Built-In Keywords<a name="ZH-CN_TOPIC_0000002593949295"></a>

<!-- md-trans-meta sourceCommit=e3c8c70b3f5d27f3770063ae4b535bc75d904e3e translatedAt=2026-08-26T12:25:54.013Z pushedAt=2026-09-06T07:11:01.940Z -->

## Function Execution Space Qualifiers<a name="section1074418132518"></a>

The function execution space qualifiers used in SIMD and SIMT mixed programming are consistent with those in SIMD programming. For details, see [Function Execution Space Qualifiers of SIMD Built-In Keywords](SIMD-BuiltIn_keyword.md#section1074418132518).

## VF Function Execution Qualifier

### \_\_simt\_vf\_\_<a name="section1009183746521"></a>

\_\_simt\_vf\_\_ is used to mark the SIMT VF entrypoint function, which has no return value. The function definition example is as follows:

```
__simt_vf__ inline void function_name(
    __gm__ uint32_t* gm_param,
    __ubuf__ float* ubuf_param,
    uint64_t scalar_param, ...);
```

The key modifiers in the SIMT VF function definition are described as follows:

| Modifier | Description |
| --- | --- |
| \_\_simt_vf\_\_ | Function identifier that identifies a SIMT VF function. |
| inline | Inlining is recommended; whether inlining actually occurs is determined by the compiler. |
| \_\_gm\_\_ | Memory space modifier that identifies the memory space as GM. |
| \_\_ubuf\_\_ | Memory space modifier that identifies the memory space as UB. |

A SIMT VF function can be called only in a kernel function or a \_\_aicore\_\_ function. The call API is [asc\_vf\_call](../../../api/SIMD_SIMT_hybrid_programming_intro/extended_syntax/kernel_function_config_147.md#asc_vf_call), as shown in the following example:

```
asc_vf_call<function_name>(dim3(blockDim), arg1, arg2, ...);
```

A SIMT VF function has the following constraints:

-   The input parameters support only Ascend C [built-in data types](SIMT-BuiltIn_keyword.md#section1835494915576) (int32\_t, uint32\_t, float, half, and so on) and the pointer, array, and structure types composed of them. A pointer type must point to GM or UB memory.
-   The function return type must be void.
-   Only \_\_simt\_callee\_\_ functions or \_\_callee\_\_ functions can be called within a SIMT VF function.

### \_\_simt\_callee\_\_

The \_\_simt\_callee\_\_ subfunction is a helper function called inside a SIMT VF function. SIMT built-in variables can be used inside the function. The following is an example of its definition:

```
__simt_callee__ uint32_t simt_helper(__gm__ uint32_t* gm_param, __ubuf__ float* ubuf_param, uint64_t scalar_param, ...);
```

The following is a call example:

```
uint32_t result = simt_helper(arg1, arg2, ...);
```

This function has the following constraints:

-   The input parameters support only Ascend C [built-in data types](SIMT-BuiltIn_keyword.md#section1835494915576) (int32\_t, uint32\_t, float, half, and so on) and their corresponding pointer types.
-   The function return value can only be an Ascend C [built-in data type](SIMT-BuiltIn_keyword.md#section1835494915576) (int32\_t, uint32\_t, float, half, and so on) or its corresponding pointer type.
-   Only \_\_simt\_callee\_\_ functions or \_\_callee\_\_ functions can be called inside the function.

### \_\_simd\_vf\_\_ and \_\_simd\_callee\_\_

\_\_simd\_vf\_\_ is used to mark the SIMD VF entrypoint function, and \_\_simd\_callee\_\_ is a subfunction called within the SIMD VF. For details, see the [SIMD VF function identifier of the SIMD built-in keyword](SIMD-BuiltIn_keyword.md#section192521344610).

## Address Space Qualifier<a name="section1624210295308"></a>

The address space qualifier used in SIMD and SIMT mixed programming is consistent with that in SIMD programming. For a detailed description, see the [address space qualifier of the SIMD built-in keyword](SIMD-BuiltIn_keyword.md#section1624210295308).

## Kernel Function Configuration

### Kernel Function Definition

A kernel function is the device-side entrypoint function for SIMD and SIMT mixed programming. It coordinates the execution flow of the entire operator, including the scheduling and call of VFs. In the mixed programming scenario of the vector compute unit, the function definition syntax is as follows:

```
__global__ __vector__ void kernel_name(__gm__ type* param1, __gm__ type* param2, ...);
```

The key modifiers are described as follows:

-   \_\_global\_\_: a mandatory modifier that identifies a kernel function, indicating that it can be called from the host side through <<<...\>\>\>.
-   \_\_vector\_\_: a mandatory modifier that identifies that the function is executed on the AIV core on the device side.

A kernel function definition has the following constraints:

-   The return value type must be void;
-   The input parameters support pointer types (which must be modified with \_\_gm\_\_) and Ascend C built-in data types;
-   A pointer parameter must point to a memory address in Global Memory and be modified with \_\_gm\_\_.

### \_\_launch\_bounds\_\_\(N\)

During concurrent execution of multiple threads, using fewer registers per thread allows more threads and thread blocks to reside on the AI processor, thereby improving performance. Therefore, the compiler uses a heuristic algorithm to keep register spilling and the number of instructions at a minimum while reducing register usage as much as possible. An application can limit the launch bounds by using the \_\_launch\_bounds\_\_\(\) qualifier in the \_\_global\_\_ function definition, providing additional information to assist the compiler in optimizing this process. This is an optional configuration.

\_\_launch\_bounds\_\_\(N\): a function marker macro that can be optionally configured on a SIMT VF entrypoint function to specify, at compile time, the maximum number of threads launched by the SIMT VF. The value range of parameter N is 1 to 2048. If \_\_launch\_bounds\_\_ is not configured, the maximum number of threads defaults to 1024.

The maximum number of threads determines the number of registers that can be allocated to each thread. For the specific correspondence, see the following table. Registers are used to store local variables in a thread. If the number of local variables exceeds the number of registers, issues such as stack overflow are likely to occur. It is recommended that the maximum number of threads be consistent with the number of dim3 threads used to launch the VF task.

**Table 1** Number of threads in \_\_launch\_bounds\_\_ and the number of registers available per thread

| Number of Threads | Number of Registers Available per Thread |
| --- | --- |
| 1025-2048 | 16 |
| 513-1024 | 32 |
| 257-512 | 64 |
| 1-256 | 127 |

### <<<\>\>\> Call

SIMD and SIMT mixed programming introduces SIMT Vector Function (VF) subtasks based on the kernel function execution flow of the SIMD programming model. The kernel function is still launched on the host side through <<<...\>\>\> following the SIMD programming approach. The syntax is as follows:

```
kernel_name<<<block_num, dyn_ub_size, stream>>>(args...);
```

The configuration parameters in the kernel call operator are described as follows:

| Parameter | Type | Description | Constraint |
| --- | --- | --- | --- |
| block_num | uint32_t | Sets the number of cores enabled for the kernel function. | Value range [1, 65535] |
| dyn_ub_size | uint32_t | Specifies the dynamic memory size, in bytes. | Does not exceed the maximum configurable value: 256KB - 8KB - 32KB - static memory. |
| stream | aclrtStream | Used to maintain the execution order of asynchronous operations. | None |

## asc_vf_call Call of a VF Function

### Calling asc_vf_call for SIMT VF
In SIMD and SIMT mixed programming, the SIMD kernel function serves as the device-side entrypoint. In the kernel function or a `__aicore__` function, `asc_vf_call` is used to launch a SIMT VF subtask. Through parameter configuration, a specified number of threads are launched to execute the specified SIMT VF function. Its function prototype is as follows:

```
template <auto funcPtr, typename... Args>
__aicore__ inline void asc_vf_call(dim3 threadNums, Args &&...args)
```

The template parameters are the specified SIMT VF function name and the SIMT VF function parameters, described as follows:

**Table 2** Template parameters

| Parameter | Description |
| --- | --- |
| funcPtr | Specifies the SIMT entrypoint kernel function. |
| Args | Defines variadic parameters used to pass parameters to the SIMT entrypoint kernel function. |

**Table 3** Parameters

| Parameter | Input/Output | Description |
| --- | --- | --- |
| threadNums | Input | A dim3 structure defined as {dimx, dimy, dimz}, used to specify the number of threads within a SIMT thread block. The total number of threads is dimx * dimy * dimz, which must be less than or equal to 2048 and is recommended to be a multiple of 32. |
| args | Input | Variadic parameters used to pass parameters to the SIMT entrypoint kernel function. |

The following example shows how to use asc\_vf\_call to call a \_\_simt\_vf\_\_ function in a SIMD and SIMT mixed programming scenario.

```
#include "simt_api/common_functions.h"
__simt_vf__ inline void add_simt(
    __gm__ float* dst, __gm__ float* src0, __gm__ float* src1)
{
    // SIMT code
    int idx = threadIdx.x + blockIdx.x * blockDim.x;
    dst[idx] = src0[idx] + src1[idx];
}

__global__ __vector__ void add_custom(__gm__ float* x, __gm__ float* y, __gm__ float* z)
{
    // Launch a SIMT VF subtask with asc_vf_call. The function name is add_simt, and blockDim is configured as dim3{1024, 1, 1}.
    asc_vf_call<add_simt>(dim3{1024, 1, 1}, z, x, y);
}
```

### Calling asc\_vf\_ for SIMD VF

Use the `asc_vf_call` API to call the SIMD VF entrypoint function to launch a VF subtask. For a detailed description of the API, see [asc\_vf\_call API Description](../../../api/SIMD-API/basic_api/reg_vector_compute/vf_call/asc_vf_call.md).

### Differences Between SIMT VF and SIMD VF Calls

Both SIMT VF and SIMD VF launch VF subtasks in a kernel function or a `__aicore__` function through the `asc_vf_call` API, but they target different parallel execution models: SIMT VF uses threads as the basic execution unit to implement thread-level parallelism, while SIMD VF implements data-level parallelism on contiguous data based on vector registers. The difference in execution models further determines the differences in their call forms, as shown in the following table:

| Comparison Item | SIMT VF Call | SIMD VF Call |
| --- | --- | --- |
| VF entrypoint function identifier | Modified with __simt_vf__. | Modified with __simd_vf__. |
| Call form | asc_vf_call<simt_func>(dim3(threadNums), arg1, arg2, ...) | asc_vf_call<simd_func>(arg1, arg2, ...) |
| First runtime parameter | A thread configuration of the dim3 type must be passed to specify the number of threads within the SIMT thread block. | There is no thread configuration parameter; the call parameters are passed starting from the first parameter of the SIMD VF function. |
| Pointer input parameter | Supports pointers to GM or UB, which must be modified with __gm__ or __ubuf__ according to the actual memory space. | Pointer input parameters must be modified with the __ubuf__ address space qualifier and cannot directly access GM memory. |
| Typical use | Handles irregular fragments such as complex control flow, scattered memory access, and thread-level index mapping. | Handles contiguous and regular vector computation fragments. |

## Built-in Variables<a name="zh-cn_topic_0000002571575581_section13165113520576"></a>

The following built-in variables are available only within SIMT VF:
-   gridDim<a name="zh-cn_topic_0000002571575581_li20760123812911"></a>

    A built-in global variable that can be used only in SIMT VF functions. It indicates how many thread blocks constitute the entire computing task in each dimension. gridDim.x <= 65535; gridDim.y and gridDim.z must be 1.

-   blockDim<a name="zh-cn_topic_0000002571575581_li076017381191"></a>

    A built-in global variable that can be used directly in SIMT VF functions to obtain the three-dimensional thread hierarchy of the threads configured in a thread block. blockDim.x, blockDim.y, and blockDim.z indicate the number of threads in the three dimensions of the thread block, respectively. The number of threads in each dimension must satisfy blockDim.x \* blockDim.y \* blockDim.z <= 2048.

-   blockIdx

    A built-in global variable that can be used only in SIMT VF functions to obtain the block index. It indicates the position coordinates of the thread block where the current thread resides in the entire grid. blockIdx.x ranges from 0 to gridDim.x - 1. blockIdx.y and blockIdx.z can only return 0 currently.

-   threadIdx

    A built-in global variable that can be used only in SIMT VF functions to obtain the index of the current thread within the thread block. threadIdx.x, threadIdx.y, and threadIdx.z indicate the index of the current thread in the three dimensions, respectively. threadIdx.x ranges in \[0, blockDim.x\), threadIdx.y ranges in \[0, blockDim.y\), and threadIdx.z ranges in \[0, blockDim.z\). The index of a thread within a thread block is calculated as follows:

    -   For a one-dimensional thread block, the index of a thread within the thread block is threadIdx.x.
    -   For a two-dimensional thread block, the index of a thread within the thread block is \(threadIdx.x + threadIdx.y \* blockDim.x\).
    -   For a three-dimensional thread block, the index of a thread within the thread block is \(threadIdx.x + threadIdx.y \* blockDim.x + threadIdx.z \* blockDim.x \* blockDim.y\).

-   warpSize

    A runtime variable that indicates the number of threads in a warp. The current value is fixed at 32.

For built-in variables at the SIMD kernel function level, see [built-in variables at the SIMD kernel function level](./SIMD-BuiltIn_keyword.md#built-in-variables).

### Usage Restrictions of Built-in Variables at Different Levels

The <<<...\>\>\> call configuration on the host side configures the logical core count of the outer kernel function. The thread-architecture built-in variables inside SIMT VF describe the thread hierarchy of the SIMT VF subtask. These two categories of concepts belong to different levels and cannot be mixed. The following table lists the differences between common concepts at the SIMD kernel function level and the SIMT VF thread level.

| Built-in Variable or Method | Level | Usage Location | Description |
| --- | --- | --- | --- |
| `AscendC::GetBlockNum()` | SIMD kernel function level | Kernel function or `__aicore__` function; invisible inside SIMT VF | Obtains the logical core count of the kernel function configured by the <<<...\>\>\> call on the host side, that is, `block_num`. |
| `AscendC::GetBlockIdx()` | SIMD kernel function level | Kernel function or `__aicore__` function; invisible inside SIMT VF | Obtains the logical core index of the currently executing kernel function. |
| `gridDim` | SIMT VF thread level | `__simt_vf__` or `__simt_callee__` function; externally inaccessible outside SIMT VF | Obtains the grid dimension in the SIMT VF thread hierarchy, indicating the number of thread blocks in each dimension of this SIMT VF subtask. Its value is determined by the logical core count of the kernel function configured by the <<<...\>\>\> call on the host side. |
| `blockIdx` | SIMT VF thread level | `__simt_vf__` or `__simt_callee__` function; externally inaccessible outside SIMT VF | Obtains the thread block index in the SIMT VF thread hierarchy, indicating the position of the SIMT thread block where the current thread resides in the VF execution grid. |
| `blockDim` | SIMT VF thread level | `__simt_vf__` or `__simt_callee__` function; externally inaccessible outside SIMT VF | Obtains the three-dimensional thread structure within the SIMT VF thread block. Its value is specified by the first `dim3` parameter of `asc_vf_call` when calling SIMT VF. |
| `threadIdx` | SIMT VF thread level | `__simt_vf__` or `__simt_callee__` function; externally inaccessible outside SIMT VF | Obtains the index of the current SIMT thread within the thread block. |

Note the following when using them:

-   The SIMD-side kernel function cannot use built-in variables related to the SIMT VF thread level, and SIMT VF cannot perceive the logical core number or logical core count at the SIMD kernel function level.
-   If SIMT VF needs to use the logical core number or logical core count from the outer SIMD process, it is recommended to obtain the corresponding values in the kernel function or `__aicore__` function through `AscendC::GetBlockIdx()` and `AscendC::GetBlockNum()`, and then pass them to SIMT VF as ordinary parameters.

## Built-in Data Types<a name="zh-cn_topic_0000002571575581_section1880403364916"></a><a name="cn_topic_0000002571575581_section1880403364916"></a>

The built-in data types used in SIMD and SIMT mixed programming are consistent with those in SIMT programming. For a detailed description, see also the [built-in data types of SIMT built-in Keywords](SIMT-BuiltIn_keyword.md#section1835494915576).

## Operators<a name="zh-cn_topic_0000002571575581_section186787252339"></a>

The operators supported by SIMD and SIMT mixed programming are consistent with those in SIMT programming. For detailed description, see [operators of SIMT built-in keywords](SIMT-BuiltIn_keyword.md#section186787252339).
