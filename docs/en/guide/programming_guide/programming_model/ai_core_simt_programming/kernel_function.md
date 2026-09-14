# Kernel Function<a name="ZH-CN_TOPIC_0000002554438935"></a>

<!-- md-trans-meta sourceCommit=e3c8c70b3f5d27f3770063ae4b535bc75d904e3e translatedAt=2026-08-20T11:34:07.741Z pushedAt=2026-08-26T03:07:05.002Z -->

As introduced in [Overview](./overview.md), a function that can be executed in parallel on the AI processor (NPU) and called by the host is called a kernel function. A kernel function is designed to be run concurrently by a large number of threads. Each thread has its own registers and stack, and they work together to complete massive data compute tasks.

## Defining Kernel Functions<a name="section962384613331"></a>

Ascend C allows developers to define custom kernel functions to extend C++. When defining a kernel function, specific function qualifiers are introduced so that the compiler can recognize that the function needs to be compiled on the device side and can be called from the host side.

The following is an example of defining a kernel function:

```
// Example of defining a kernel function
__global__ void vec_add(float* x, float* y, float* z) 
{
    // Specific computation logic
}
```

The following rules must be followed when defining a kernel function:

-   Use the function type qualifier \_\_global\_\_ to identify it as a kernel function.

-   The kernel function must have a void return type.

-   The kernel function parameter list must comply with the [function parameter list restrictions](../../../technical_appendix/cpp_standard_support/syntax_restrictions/function.md#section_kernel_function_argument_list_constraint).

## Calling Kernel Functions<a name="section33493239410"></a>

Functions in an operator program fall into three categories: host-side execution functions, kernel functions (executed on the device side), and device-side execution functions (other than kernel functions). The following figure uses the kernel direct call operator development mode as an example to describe the call relationships among the three:

-   A host-side execution function can call other host-side execution functions, which is the function call in general C/C++ programming. It can also call a kernel function through <<<...\>\>\>.

-   A kernel function can call device-side execution functions other than kernel functions.

-   A device-side execution function (other than a kernel function) is identified by the type qualifier \_\_aicore\_\_ and can call other device-side execution functions of the same type.

**Figure 1** Call relationships among kernel functions, host-side execution functions, and device-side execution functions<a name="fig14484835135913"></a>  
![](../../../figures/call_relationships_among_three_function_types_in_an_operator_program.png "Call relationships among the three types of functions in an operator program")

The host side calls a kernel function using the kernel function call operator <<<...>>>, as shown below:

```
kernel_name<<<blocks_per_grid, threads_per_block, dyn_ubuf_size, stream>>>(argument list)
```

The parameters in <<<...\>\>\> are the execution configuration of the kernel function, which is determined by four parameters. For details, see [Kernel Function Configuration](../../language_extension/SIMT-BuiltIn_keyword.md#section97005415463):

-   blocks\_per\_grid: of the int or [dim3](../../language_extension/SIMT-BuiltIn_keyword.md#built-in-structure) type, specifies the dimensions and size of the grid. blocks\_per\_grid.x \* blocks\_per\_grid.y \* blocks\_per\_grid.z equals the total number of thread blocks to be launched, which must not exceed 65535.

-   threads\_per\_block: of the int or [dim3](../../language_extension/SIMT-BuiltIn_keyword.md#built-in-structure) type, specifies the dimensions and size of each thread block. threads\_per\_block.x \* threads\_per\_block.y \* threads\_per\_block.z equals the number of threads contained in each thread block, which must be less than or equal to the \_\_launch\_bounds\_\_ configuration.

-   <a name="li12421238101815"></a>dyn\_ubuf\_size: size_t type. This parameter specifies the number of bytes of shared memory dynamically allocated for each thread block in this call, in addition to the statically allocated memory. The unit is bytes, and the default value is 0. For details about how to use it, see the "dynamic allocation" method in [Shared Memory](./memory_hierarchy.md#section66329146410).

-   stream: aclrtStream type pointer. Specifies the associated stream, which is used to maintain the execution order of asynchronous operations.

## Grid and Thread Index Built-in Variables

In a kernel function, Ascend C provides [built-in variables](../../language_extension/SIMT-BuiltIn_keyword.md#built-in-variables) for obtaining the parameters of the execution configuration and the indexes of threads or thread blocks.

The commonly used built-in index variables are described as follows:

-   threadIdx: obtains the index of the current thread within its thread block. threadIdx.x, threadIdx.y, and threadIdx.z indicate the indexes of the current thread in the three dimensions, respectively. The range of threadIdx.x is [0, blockDim.x), the range of threadIdx.y is [0, blockDim.y), and the range of threadIdx.z is [0, blockDim.z).

-   blockDim: obtains the three-dimensional hierarchy configured in the thread block, that is, the value of the dim3 structure instance configured when the kernel function is launched. blockDim.x, blockDim.y, and blockDim.z indicate the numbers of threads in the three dimensions of the thread block, respectively. These values are specified in the execution configuration when the kernel function is launched, corresponding to the threads_per_block parameter in the kernel function configuration.

-   blockIdx: obtains the index of the current thread block in its grid, indicating the position coordinates of the thread block where the current thread resides within the entire grid.

-   gridDim: indicates how many thread blocks constitute the entire compute task in each dimension. It is specified in the execution configuration when the kernel function is launched, corresponding to the blocks_per_grid parameter in the kernel function configuration. The thread block relationship in each dimension must satisfy gridDim.x * gridDim.y * gridDim.z <= 65535.

Developers can combine the preceding variables to compute the globally unique data index (Global Index) of the current thread in the entire massive compute grid space. This enables thousands of threads executing the same code to precisely locate and process their own specific data slices.

### Boundary Check

In actual one-dimensional parallel processing development, the total length of the data to be processed often cannot be evenly divided by the size of a single thread block. Therefore, it is usually necessary to allocate extra physical threads and introduce a boundary check mechanism inside the kernel function to prevent out-of-bounds memory access.

The standard formula for calculating a one-dimensional data index is as follows:

```
int global_index = blockIdx.x * blockDim.x + threadIdx.x;
```

The following code snippet shows how to combine the index mechanism with boundary check to implement vector addition of dynamic length:

```
// 1. Kernel implementation
__global__ void vec_add(float* x, float* y, float* z, int vector_length) 
{
    // Calculate the global element index corresponding to the current thread
    int global_index = blockIdx.x * blockDim.x + threadIdx.x;

    // Boundary check: Constrain the computation range to prevent excess threads from accessing out-of-bounds memory
    if (global_index < vector_length) 
    {
        // Only threads with indices within the valid range will execute the specific addition operation
        z[global_index] = x[global_index] + y[global_index];
    }
}

// 2. Host-side invocation example
int main() 
{
    // Assume memory allocation and initialization on both the Host and Device sides are complete
    int vector_length = 1000;
    
    // Empirical configuration: Specify that each block contains 256 threads
    int threads = 256; 
    
    // Calculate parameters: Calculate the required number of blocks based on the ceiling principle
    // Algorithm: (length + threads - 1) / threads
    int blocks = (vector_length + threads - 1) / threads; // The result is 4

    // Execute invocation: Launch 4 blocks, with 256 threads per block (a total of 1024 physical threads dispatched)
    vec_add<<<blocks, threads, 0, stream>>>(dev_x, dev_y, dev_z, vector_length);
    
    // ... Subsequent stream synchronization and resource release logic ...
    return 0;
}
```

In this example, the total number of target processing elements is 1000. According to the strategy of dividing into blocks of 256 threads, the host side actually dispatches 4 thread blocks to the hardware, launching a total of $4 \times 256 = 1024$ physical threads. When the program runs to the last thread block, that is, when blockIdx.x is 3, the last 24 threads in this block (from index 1000 to 1023) derive a global_index that falls within the range [1000, 1023]. At this point, through the boundary interception of if (global_index < vector_length), these "extra" threads that exceed the valid data range directly end their tasks and exit safely. This mechanism ensures the completeness of the computation results while effectively avoiding the risk of out-of-bounds memory access.