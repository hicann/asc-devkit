# Memory Hierarchy<a name="ZH-CN_TOPIC_0000002523359048"></a>

<!-- md-trans-meta sourceCommit=8d22eb82fd0b279bd5d784f696a54970c40f98ac translatedAt=2026-10-28T09:21:49.914Z pushedAt=2026-11-03T07:08:03.567Z nl=2 -->

SIMT threads can access multiple memory spaces. The following table summarizes the scope and lifecycle of common memory types in SIMT programming.

| Memory Type | Thread Scope | Lifecycle | Physical Location |
|---------|----------|---------|---------|
| Global memory | Grid | Application | Device |
| Shared memory | Block | Kernel function | Vector Core |
| Stack space | Thread | Kernel function | Global Memory |
| Register | Thread | Kernel function | Vector Core |

-   Global memory is a memory resource that all threads can directly access, that is, Global Memory.
-   Shared memory is memory shared by all threads within a thread block. It resides in the Unified Buffer (UB), and its lifecycle is the same as that of the thread block.
-   Each thread has its own independent stack space, which is mainly used to store thread-private temporary data.
-   Each thread has its own independent registers, which are used to store local variables.
-   In terms of access performance, registers have the highest access speed; shared memory is more efficient to access than global memory.

The memory hierarchy is shown in the following figure:  
<img src="../../../figures/memory.png" width="50%">

## Global Memory<a name="section8946131492119"></a>

The global memory on the Device side is a memory space accessible to all threads in the entire grid. Global memory is persistent: the space allocated through global memory and the data stored in it remain until the memory space is released or the application terminates. You need to allocate and initialize global memory through the Runtime API before the kernel function is launched. During kernel function execution, each SIMT thread can read from and write to global memory, and after execution, the results can be copied back to the Host. For more information and details about the Runtime API, see [Runtime API](https://gitcode.com/cann/runtime/blob/9.2.0/docs/en/api_ref/README.md).

A kernel function running on the Device side can directly access global memory through pointers. The following code shows a simple example of global memory. The arrays x, y, and z are all stored in global memory, and the following kernel function enables each thread to access and store data in global memory.

```cpp
__global__ void add_custom(float* x, float* y, float* z, uint64_t total_length)
{
    // Calculate global thread ID
    int32_t idx = blockIdx.x * blockDim.x + threadIdx.x;
    // Maps to the row index of output tensor
    if (idx >= total_length) {
        return;
    }
    z[idx] = x[idx] + y[idx];
}
```

## Shared Memory<a name="section66329146410"></a>

Shared memory is a memory space accessible to all threads within the same thread block, located in the UB of each Vector Core (AIV). Compared with global memory, shared memory has a smaller capacity but higher bandwidth and lower access latency, and can be regarded as a user-managed cache resource during kernel execution.

Users can allocate shared memory either statically or dynamically.

1.  Static application: Allocate a memory space of a specified size. The size is determined at compile time and cannot be modified dynamically. Developers allocate and use it through array allocation.

    ```cpp
    __global__ void add_custom(...)
    {
        __ubuf__ half static_buf[1024];
        ...
    }
    ```

    By default, the start address of the allocated static memory is aligned according to the data type. Users can also manually specify the alignment by using `__align__(N)`.

2.  Dynamic application: Users need to specify the size of the dynamic memory space through the parameter [dyn_ubuf_size](kernel_function.md#li12421238101815) in <<<\>\>\>. The size is determined at runtime. In SIMT programming, dynamic memory can be allocated and used in the following ways.

    ```cpp
    // Device side: Declare dynamic shared memory
    __global__ void add_custom(...)
    {
        extern __ubuf__ char dynamic_buf[];
        ...
    }

    // Host side: Specify the dynamic shared memory size through the third parameter in <<<>>>
    int32_t main(int argc, char const *argv[])
    {
        ...
        uint32_t dyn_ubuf_size = 1024 * sizeof(char);
        add_custom<<<blocks_per_grid, threads_per_block, dyn_ubuf_size, stream>>>(...);
        ...
    }
    ```

### UB Memory Partitioning Layout

As shown in the following figure, the total size of the UB memory space is 256KB. In addition to the static and dynamic memory requested by users, UB also needs to reserve a certain amount of space for internal use. Therefore, when configuring the static and dynamic memory sizes, users cannot use up the entire UB space:

![](../../../figures/ub_alloc.png)

UB is divided into four main areas by function. From low addresses to high addresses, they are static memory, dynamic memory, reserved space, and Data Cache. The specific structure is as follows:

1.  Static memory and dynamic memory correspond to the memory allocated through the user's static and dynamic application methods. The start address of the dynamic memory requested by the user on UB is always aligned to 32B, and the static memory is laid out according to the following start address alignment rules:
    -   By default, the start address of the requested static memory is aligned according to the data type size;
    -   The user can manually specify the alignment by using `alignas(N)`. This configuration takes precedence over the default alignment.
2.  Reserved space: The space reserved by the compiler and Ascend C, with a size of 8KB. Of this, 6KB is used as the SIMT parameter-passing register spill stack space, and 2KB is the space reserved internally for Ascend C APIs.
3.  Data Cache: The Data Cache space dedicated to SIMT, used for caching data when SIMT threads access global memory. The configurable range of the Data Cache space is **a minimum of 32KB and a maximum of 128KB**. The actual memory size is affected by the static and dynamic memory sizes configured by the user. The specific calculation formula is as follows:

    ```
    Data Cache space size = min(total UB size (256KB) - static memory - dynamic memory - reserved space (8KB by default), 128KB)
    ```

    The upper limit of the Data Cache space is **128KB**. Even if the requested static and dynamic memory is small, the actual allocated Data Cache size does not exceed this upper limit.

#### Data Cache Memory Space

The Data Cache serves as the cache for accessing GM memory, and its size affects the memory access efficiency of the operator. If the Data Cache is smaller than 32 KB, a verification error occurs and execution exits. Therefore, when allocating memory, ensure that sufficient Data Cache space is reserved.

Because the Data Cache, reserved space, and the dynamic and static memory requested by developers share the same UB, developers cannot use the entire UB space. The Data Cache capacity is calculated by subtracting the dynamic and static memory and the reserved space from the total UB size. The smaller the shared memory configured by the user, the larger the space left for the Data Cache. In other words, the shared memory size implicitly determines the Data Cache capacity until it reaches the 128KB upper limit.

If the memory size configured by the user is insufficient, or if the user directly accesses a UB address without requesting memory, the accessed address may cross the dynamic and static memory boundary and fall into the Data Cache area. This is undefined behavior and is extremely dangerous. Such out-of-bounds writes may corrupt the cached data of SIMT global memory access, causing the operator to function abnormally.

#### Disabling Reserved Space

If you need more UB memory, you can disable the reserved space to expand the available UB space. To disable it, add the following compilation options during compilation:

- [--cce-disable-vf-stack-reserved-ubuf](../../compilation_and_execution/operator_compilation/ai_core_operator_compilation.md#ZH-CN_TOPIC_0000002462746461): Disables the 6 KB stack space for SIMT parameter-passing register overflow.
- [--cce-disable-asc-reserved-ubuf](../../compilation_and_execution/operator_compilation/ai_core_operator_compilation.md#ZH-CN_TOPIC_0000002462746461): Disables the 2 KB internal reserved space of the Ascend C interface.

The preceding compilation options can be used independently or together. After they are enabled, the compiler no longer reserves the corresponding UB space, which can then be used as ordinary UB space. However, you need to verify the impact of disabling the reserved space on your own:

- After `--cce-disable-vf-stack-reserved-ubuf` is enabled, the compiler cannot use the reserved UB space as the stack space for parameter-passing register overflow. If the user function parameters are too large, a compilation error will be reported.
- Enabling `--cce-disable-asc-reserved-ubuf` has no impact for now, because this space is not used internally in the current SIMT programming scenarios.

## Stack Space

In SIMT programming, the stack space is managed by the compiler, and its lifecycle is the same as that of the kernel function. It consists of the following two parts:

-   The SIMT stack space located in Global Memory (GM).
-   The transfer stack space located in UB for parameter passing in SIMT function calls (part of the 8KB reserved space in UB).

The following describes these two types of stack space respectively.

### SIMT Stack Space on GM

The SIMT stack space on GM is used to store thread-private runtime state and is divided by purpose into SIMT generic stack space and SIMT Divergence (branch) stack space.

-   **SIMT generic stack space**  
    It is used to store thread-private temporary data, including temporary data generated by function calls, local objects that cannot be fully allocated to registers, and temporary data generated by [Registers](#registers) spill. The default size of the SIMT generic stack space per thread is 1152 bytes. Because it resides in global memory, its access latency is higher than that of register access. When there are many local variables, a deep function call chain, large thread-private arrays, or an excessively large maximum number of threads that leaves insufficient available registers per thread, the stack space usage increases accordingly and may cause stack overflow in severe cases.
-   **SIMT Divergence stack space**  
    It is used to store the program counter (PC) and active mask in SIMT branch divergence scenarios. Threads within a warp share one SIMT Divergence stack space, and the default size of the stack space per warp is 1024 bytes. The more frequent the branch divergence and the more complex the control flow, the higher the stack space usage, which may cause stack overflow in severe cases.

Developers can use the `--cce-res-usage` compiler option to view the stack space usage of a kernel function. If the stack space usage is high, prioritize reducing thread-private temporary variables, controlling the amount of data in debug prints, simplifying the function call chain, and optimizing branch divergence logic to reduce unnecessary global memory access. If the stack space size must be adjusted for business reasons, it can be configured through the configuration file loaded by the `aclInit()` API: the `simt_stack_size` option sets the SIMT generic stack space size per thread, and the `simt_divergence_stack_size` option sets the SIMT Divergence stack space size, both in bytes. A configuration example is as follows. For more details, see "SIMT Operator Stack Space Size Configuration Example" in [aclInit() API Configuration](https://gitcode.com/cann/runtime/blob/9.2.0/docs/en/api_ref/02_initialization_and_deinitialization.md#aclinit).

```json
{
  "StackSize": {
    "simt_stack_size": 1024,
    "simt_divergence_stack_size": 512
  }
}
```

### SIMT Function Call Relay Stack Space on UB

As described in [UB Memory Partitioning Layout](#ub-memory-partitioning-layout), the UB contains 8KB of reserved space, of which 6KB is used as the relay stack for parameter passing in SIMT function calls. As described in the [Parameter-Passing Register](#parameter-passing-register) section, the hardware natively supports a limited capacity of inter-function parameter-passing registers. When the total size of the parameters exceeds this capacity, the compiler uses this relay stack space to complete parameter passing, which incurs additional memory access overhead and degrades function call performance. Therefore, developers should pay attention to the parameter scale of Device-side function calls and avoid passing excessively large parameter objects.

## Registers<a name="section1137715201010"></a>

Registers are the storage resources closest to the compute units and with the highest access speed in SIMT programming. By function, they are classified into general-purpose registers and parameter-passing registers, which are described separately below.

### General-Purpose Register

Each SIMT thread has its own independent general-purpose registers, whose lifecycle is the same as that of the kernel function. Their allocation is managed uniformly by the compiler and is used for local data storage during thread execution. The total capacity of the SIMT general-purpose register file of each Vector Core (AIV) is 128KB, which is divided among all threads in the thread block running on that AIV based on the maximum number of threads. The actual number of registers available to each thread depends on the maximum number of threads configured when the kernel function is defined. The larger the maximum number of threads, the fewer registers available to each thread. The correspondence is shown in the following table.

**Table 1** Correspondence between the maximum number of threads and the number of registers available to each thread

| Maximum Thread Count | Number of Registers Available to Each Thread |
| --- | --- |
| 1025~2048 | 16 |
| 513~1024 | 32 |
| 257~512 | 64 |
| 1~256 | 127 |

As shown in the table above, the larger the configured maximum number of threads, the fewer registers available to each thread. If the maximum number of threads is configured too large and the per-thread computation logic is complex, the compiler may, due to insufficient register resources, spill some local variables or intermediate results to the SIMT generic stack space in GM, that is, register spill, which introduces additional global memory access and affects operator performance. Developers can use the `--cce-res-usage` compiler option to view the register resource usage of the kernel function and adjust the maximum number of threads or register configuration accordingly.

Developers can use [\_\_launch\_bounds\_\_](../../language_extension/simt_builtin_keywords.md#li23861114618) to configure the maximum number of threads, indirectly controlling the number of general-purpose registers per thread; they can also use [\_\_maxnreg\_\_](../../language_extension/simt_builtin_keywords.md#section_maxnreg) to directly configure the number of general-purpose registers per thread.

### Parameter-Passing Register

The parameter-passing register is dedicated to parameter passing between SIMT functions. After removing the internal occupancy, the parameter-passing register capacity available to users is `26 * 32bit`. When the total amount of passed parameters exceeds this capacity, the excess part overflows to the stack space, generating additional data movement operations from the register to the stack space, which degrades operator performance and may cause stack overflow in severe cases. Therefore, developers should pay attention to the data volume of parameters passed between SIMT functions to avoid exceeding this limit.

## Programming Suggestions

To address the issues that the limited number of registers and limited stack space can easily cause performance degradation or even stack overflow, developers can refer to the following programming suggestions for optimization:

- Use data types with smaller bit widths whenever possible. Because the general-purpose register is 32-bit wide, a variable of the `int64_t` data type occupies two general-purpose registers.
- Reduce temporary variables. Use address references instead of temporary variables whenever possible to reduce register usage.
- Use the `__inline__` keyword to modify SIMT functions so that the compiler can optimize function calls.
- Optimize branch processing logic to reduce branch jumps within a warp.
- For complex structures, plan to store the structure data in UB or GM to avoid passing excessively large parameters.
