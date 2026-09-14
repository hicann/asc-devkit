# Kernel Function Configuration

<!-- md-trans-meta sourceCommit=1e3d48cde951e4cd58f9cded9fce17c1f24fa02d translatedAt=2026-09-07T10:04:02.009Z pushedAt=2026-09-09T01:49:40.942Z -->

## Kernel Function Definition

A kernel function is the Device-side entry function of SIMD and SIMT hybrid programming. It coordinates the execution flow of the entire operator, including the scheduling and invocation of VFs. In the hybrid programming scenario of the vector compute unit, the function definition syntax is as follows:

```
__global__ __vector__ void kernel_name(__gm__ type* param1, __gm__ type* param2, ...);
```

The key modifiers are described as follows:

-   \_\_global\_\_: A required modifier that identifies the kernel function, indicating that it can be invoked from the Host side through <<<...\>\>\>.
-   \_\_vector\_\_: A required modifier that identifies the function as executing on the AIV core on the Device side.

The kernel function definition has the following constraints:

-   The return value type must be void.
-   Input parameters support pointer types (which must be modified with \_\_gm\_\_) and Ascend C built-in data types.
-   Pointer parameters must point to memory addresses in Global Memory and must be decorated with \_\_gm\_\_.

## \_\_launch\_bounds\_\_\(N\)

During concurrent execution by multiple threads, using fewer registers per thread allows more threads and thread blocks to reside on the AI processor, thereby improving performance. Therefore, the compiler uses a heuristic algorithm to keep register spilling and the instruction count at a minimum while reducing register usage as much as possible. An application can restrict the launch bounds by using the \_\_launch\_bounds\_\_\(\) qualifier in the \_\_global\_\_ function definition, providing additional information to assist the compiler in optimizing this process. This is an optional configuration.

\_\_launch\_bounds\_\_\(N\): A function marker macro, optionally configured on the SIMT VF entry function, used to specify the maximum number of threads launched by SIMT VF at compile time. If \_\_launch\_bounds\_\_ is not configured, the maximum number of threads defaults to 1024. The parameter N must satisfy the following conditions:

-   N \>= dimx \* dimy \* dimz, where dimx, dimy, and dimz are the components of the dim3 structure representing the thread grid dimensions.
-   The value range of N is from 1 to 2048.

    The maximum number of threads determines the number of registers that can be allocated to each thread. For the specific correspondence, see the following table. Registers are used to store local variables in a thread. If the number of local variables exceeds the number of registers, issues such as register spilling are likely to occur. It is recommended that the maximum number of threads be consistent with the number of dim3 threads used to launch the VF task.

    **Table 1** Number of threads in \_\_launch\_bounds\_\_ and the number of registers available per thread

    | Number of threads | Number of registers available per thread |
    | --- | --- |
    | 1025~2048 | 16 |
    | 513~1024 | 32 |
    | 257~512 | 64 |
    | 1~256 | 127 |

## <<<\>\>\> Call

The kernel function is invoked on the host side through the <<<...\>\>\> kernel invocation operator. The syntax is as follows:

```
kernel_name<<<block_num, dyn_ub_size, stream>>>(args...);
```

The configuration parameters in the kernel invocation operator are described as follows:

| Parameter | Type | Description | Constraints |
| --- | --- | --- | --- |
| block_num | uint32_t | Sets the number of cores enabled for the kernel function. | Value range: [1, 65535]. |
| dyn_ub_size | uint32_t | Specifies the dynamic memory size, in bytes. | Must not exceed the maximum configurable value: 256 KB - 8 KB - 32 KB - static memory. |
| stream | aclrtStream | Maintains the execution order of asynchronous operations. | None |

## asc\_vf\_call

In SIMD and SIMT hybrid programming scenarios, use asc\_vf\_call to launch a SIMT VF (Vector Function) subtask. Through parameter configuration, a specified number of threads are launched to execute the specified SIMT VF function. Its function prototype is as follows:

```
template <auto funcPtr, typename... Args>
__aicore__ inline void asc_vf_call(dim3 threadNums, Args &&...args)
```

The template parameters are the specified SIMT VF function name and the SIMT VF function parameters, described as follows:

**Table 2** Template parameters

| Parameter | Description |
| --- | --- |
| funcPtr | Specifies the SIMT entry kernel function. |
| Args | Defines variadic parameters used to pass arguments to the SIMT entry kernel function. |

**Table 3** Parameters

| Parameter | Input/Output | Description |
| --- | --- | --- |
| threadNums | Input | A **dim3** structure defined as **{dimx, dimy, dimz}**, used to specify the number of threads in a SIMT thread block. The total number of threads is **dimx** * **dimy** * **dimz**, which must be less than or equal to 2048 and is recommended to be a multiple of 32. |
| args | Input | Variadic parameters used to pass arguments to the SIMT entry kernel function. |

The following example shows how to use asc\_vf\_call to call the \_\_simt\_vf\_\_ function in a SIMD and SIMT hybrid programming scenario.

```
#include "simt_api/common_functions.h"
__simt_vf__ inline void add_simt(
    __gm__ float* dst, __gm__ float* src0, __gm__ float* src1)
{
    // SIMT code.
    int idx = threadIdx.x + blockIdx.x * blockDim.x;
    dst[idx] = src0[idx] + src1[idx];
}

__global__ __vector__ void add_custom(__gm__ float* x, __gm__ float* y, __gm__ float* z)
{
    // Launch the SIMT VF subtask with asc_vf_call. The function name is add_simt, and threadNums is configured as dim3{1024, 1, 1}.
    asc_vf_call<add_simt>(dim3{1024, 1, 1}, z, x, y);
}
```
