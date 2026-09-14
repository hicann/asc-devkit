# Kernel Function Configuration

<!-- md-trans-meta sourceCommit=2c97acf47d28b2c9fa27e8b745084f6e4f3eedea translatedAt=2026-09-09T02:28:25.211Z pushedAt=2026-09-09T10:00:56.674Z -->

## Kernel Function Definition

A kernel function is the Device-side entry function of SIMT programming, responsible for coordinating the execution flow of the entire operator. The function definition syntax is as follows:

```
__global__ void kernel_name(uint32_t* param1, float* param2, ...);
```

The key modifiers are described as follows:

-   \_\_global\_\_: A required modifier that identifies the kernel function, indicating that it can be invoked from the Host side through <<<...\>\>\>.

The kernel function definition has the following constraints:

-   The return value type must be void.
-   Input parameters support basic data types and pointer types to basic data types, such as int32\_t, **float**, int32\_t\*, float\*, and so on.
-   Pointer parameters must point to memory addresses in Global Memory.

## \_\_launch\_bounds\_\_\(N\)

During concurrent execution by multiple threads, using fewer registers per thread allows more threads and thread blocks to reside on the AI processor, thereby improving performance. Therefore, the compiler uses a heuristic algorithm to keep register spilling and the instruction count at a minimum while reducing register usage as much as possible. An application can restrict the launch bounds by using the \_\_launch\_bounds\_\_\(\) qualifier in the \_\_global\_\_ function definition, providing additional information to assist the compiler in optimizing this process. This is an optional configuration.

\_\_launch\_bounds\_\_\(\): A function marker macro, optionally configured on the kernel function, used to specify the maximum number of threads launched by the kernel function at compile time. If \_\_launch\_bounds\_\_ is not configured, the maximum number of threads defaults to 1024. The parameter N must satisfy the following conditions:

-   N \>= dimx \* dimy \* dimz, where dimx, dimy, and dimz are the components of the dim3 structure representing the thread grid dimensions.
-   The value range of N is from 1 to 2048.

    The maximum number of threads determines the number of registers that can be allocated to each thread. For the specific correspondence, see the following table. Registers are used to store local variables in a thread. If the number of local variables exceeds the number of registers, issues such as register spilling are likely to occur. It is recommended that the maximum number of threads be consistent with the number of dim3 threads used to launch the kernel function.

    **Table 1** Number of threads in \_\_launch\_bounds\_\_ and the number of registers available per thread

    | Number of threads | Number of registers available per thread |
    | --- | --- |
    | 1025~2048 | 16 |
    | 513~1024 | 32 |
    | 257~512 | 64 |
    | 1~256 | 127 |

## <<<\>\>\> Call

In SIMT programming, when using <<<\>\>\> to call a function decorated with the \_\_global\_\_ qualifier, the execution configuration must be specified, in the following form:

```
<<<blocks_per_grid, threads_per_block, dyn_ubuf_size, stream>>>
```

The execution configuration is determined by four parameters. For detailed usage, see [Kernel Function Configuration](https://gitcode.com/cann/asc-devkit/blob/9.1.0/docs/guide/programming_guide/language_extension/SIMT-BuiltIn_keyword.md#kernel-function-configuration):

-   blocks\_per\_grid: int or dim3 type, used to specify the dimensions and scale of the grid. blocks\_per\_grid.x \* blocks\_per\_grid.y \* blocks\_per\_grid.z equals the total number of thread blocks launched.
-   threads\_per\_block: int or dim3 type, used to specify the dimensions and scale of each thread block. threads\_per\_block.x \* threads\_per\_block.y \* threads\_per\_block.z equals the number of threads contained in each thread block, and must be less than or equal to the \_\_launch\_bounds\_\_ configuration.
-   dyn\_ubuf\_size: size\_t type, used to specify the size of shared memory dynamically allocated for each thread block, in bytes. This memory is used for arrays. For detailed usage, see the "dynamic allocation" method in [Shared Memory](https://gitcode.com/cann/asc-devkit/blob/9.1.0/docs/guide/programming_guide/programming_model/ai_core_simt_programming/memory_hierarchy.md#shared-memory).
-   stream: aclrtStream type pointer, specifying the associated stream, used to maintain the execution order of asynchronous operations.

The following example shows how to declare and call a kernel function.

```
// Declare.
__global__ void add_custom(float* x, float* y, float* z, uint64_t total_length);
// Call.
add_custom<<<blocks_per_grid, threads_per_block, dyn_ubuf_size, stream>>>(x, y, z, 1024);
```
