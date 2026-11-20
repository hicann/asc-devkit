# Kernel Function Configuration

<!-- md-trans-meta sourceCommit=57ffc0e98dc1ae7768d3eaadf217e1d94fdb6424 translatedAt=2026-10-27T03:44:52.209Z pushedAt=2026-10-27T09:21:55.717Z nl=2 -->

## Kernel Function Definition

A kernel function is the Device-side entry function of SIMD and SIMT hybrid programming. It coordinates the execution flow of the entire operator, including the scheduling and invocation of VFs. In the hybrid programming scenario of the vector compute unit, the function definition syntax is as follows:

```cpp
__global__ __vector__ void kernel_name(__gm__ type* param1, __gm__ type* param2, ...);
```

The key modifiers are described as follows:

-   `__global__`: A required modifier that identifies the kernel function, indicating that it can be invoked from the Host side through <<<...\>\>\>.
-   `__vector__`: A required modifier that identifies the function as executing on the AIV core on the Device side.

The kernel function definition has the following constraints:

-   The return value type must be void.
-   Input parameters support pointer types (which must be modified with `__gm__`) and Ascend C built-in data types.
-   Pointer parameters must point to memory addresses in Global Memory and must be decorated with `__gm__`.

## \_\_launch\_bounds\_\_\(N\)

When multiple threads execute concurrently, each thread using fewer registers allows more threads to be launched within a single thread block, thereby improving performance. Therefore, the compiler uses a heuristic algorithm to keep register spilling and the instruction count at a minimum while reducing register usage as much as possible. The application can restrict the launch bounds by using the `__launch_bounds__()` qualifier in the `__global__` function definition, providing additional information to help the compiler optimize this process. This is an optional configuration.

`__launch_bounds__(N)`: A function marker macro, optionally configured on the SIMT VF entry function, used to specify the maximum number of threads launched by SIMT VF at compile time. If `__launch_bounds__` is not configured, the maximum number of threads defaults to 1024. The parameter N must satisfy the following conditions:

-   N \>= dimx \* dimy \* dimz, where dimx, dimy, and dimz are the components of the dim3 structure representing the thread grid dimensions.
-   The value range of N is from 1 to 2048.

    The maximum number of threads determines the number of registers that can be allocated to each thread. For the specific correspondence, see the following table. Registers are used to store local variables in a thread. If the number of local variables exceeds the number of registers, issues such as register spilling are likely to occur. It is recommended that the maximum number of threads be consistent with the number of dim3 threads used to launch the VF task.

    **Table 1**  \_\_launch\_bounds\_\_(N) and the number of registers available per thread

    | N | Number of registers available to each thread |
    | --- | --- |
    | 1025~2048 | 16 |
    | 513~1024 | 32 |
    | 257~512 | 64 |
    | 1~256 | 127 |

## <<<\>\>\> Call

The kernel function is invoked on the host side through the <<<...\>\>\> kernel invocation operator. The syntax is as follows:

```cpp
kernel_name<<<block_num, dyn_ub_size, stream>>>(args...);
```

The configuration parameters in the kernel invocation operator are described as follows:

| Parameter | Type | Description | Constraint |
| --- | --- | --- | --- |
| block_num | uint32_t | Sets the number of cores enabled by the kernel function. | Value range: [1, 65535] |
| dyn_ub_size | uint32_t | Specifies the dynamic memory size, in bytes. | Must not exceed the maximum configurable value: 256KB - 8KB - 32KB - static memory |
| stream | aclrtStream | Maintains the execution order of asynchronous operations. | None |

> [!NOTE] Note
> 
> In the mixed programming scenario, when the kernel function is invoked through <<<>>>, the data type of the block_num parameter is uint32_t. Therefore, gridDim in the SIMT architecture is one-dimensional, that is, gridDim.x = block_num, and both gridDim.y and gridDim.z are 1.

## asc\_vf\_call Call<a name="asc-vf-call"></a>

In SIMD and SIMT mixed programming scenarios, use asc\_vf\_call to launch a SIMT VF (Vector Function) subtask. Through parameter configuration, it starts a specified number of threads to execute the specified SIMT VF function. Its function prototype is as follows:

```cpp
template <auto funcPtr, typename... Args>
__aicore__ inline void asc_vf_call(dim3 threadNums, Args &&...args)
```

The template parameters are the specified SIMT VF function name and the SIMT VF function parameters, described as follows:

**Table 2** Template parameters

| Name | Description |
| --- | --- |
| funcPtr | Specifies the SIMT entry kernel function (Kernel). |
| Args | Defines variadic parameters for passing arguments to the SIMT entry kernel function (Kernel). |

**Table 3** Parameters

| Name | Input/Output | Description |
| --- | --- | --- |
| threadNums | Input | [dim3 type](./builtin_data_types.md#dim3), used to specify the number of threads in a SIMT thread block. The total number of threads is x * y * z, which must be less than or equal to 2048 and is recommended to be a multiple of 32. |
| args | Input | Variadic parameters for passing arguments to the SIMT entry kernel function (Kernel). |

The following example shows how to use asc\_vf\_call to call the \_\_simt\_vf\_\_ function in SIMD and SIMT mixed programming scenarios.

```cpp
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
    // asc_vf_call launches the SIMT VF subtask. The function name is add_simt, and threadNums is configured as dim3{1024, 1, 1}.
    asc_vf_call<add_simt>(dim3{1024, 1, 1}, z, x, y);
}
```
