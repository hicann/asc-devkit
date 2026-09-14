# Memory Hierarchy<a name="ZH-CN_TOPIC_0000002523359048"></a>

<!-- md-trans-meta sourceCommit=4c81e54975ec65486e709c6cf9f279160b0190ce translatedAt=2026-08-20T11:38:05.864Z pushedAt=2026-08-26T03:07:07.026Z -->

SIMT threads can access multiple memory spaces. The following table summarizes the scope and lifecycle of common memory types in SIMT programming.

| Memory Type | Thread Scope | Lifecycle | Physical Location |
|---------|----------|---------|---------|
| Global memory | Grid | Application | Device |
| Shared memory | Block | Kernel function | Vector Core |
| Register | Thread | Kernel function | Vector Core |

-   Global memory is a memory resource that all threads can directly access.

-   Shared memory is memory shared by all threads within a thread block. It resides in the Unified Buffer, and its lifecycle is the same as that of the thread block.

-   Each thread has its own independent registers for storing local variables.

-   In terms of access performance, registers offer the highest access speed; shared memory is more efficient to access than global memory.

The memory hierarchy is shown in the following figure:  
<img src="../../../figures/memory.png" width="50%">

## Global Memory<a name="section8946131492119"></a>

The global memory on the device side is a memory space accessible to all threads in the entire grid. Global memory is persistent: the space allocated through global memory and the data stored in it remain until the memory space is released or the application terminates. You need to allocate and initialize global memory through the Runtime API before the kernel function is launched. During kernel function execution, each SIMT thread can read from and write to global memory, and after execution, the results can be copied back to the host. For more information and details about the Runtime API, see [*Runtime API*](https://hiascend.com/document/redirect/CannCommunityRuntimeApi).

A kernel function running on the device side can directly access global memory through pointers. The following code shows a simple example of global memory. The arrays x, y, and z are all stored in global memory, and the following kernel function enables each thread to access and store data in global memory.

```
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

Shared memory is a memory space accessible to all threads within the same thread block, located in the Unified Buffer of each Vector Core (AIV). Compared with global memory, shared memory has a smaller capacity but offers higher bandwidth and lower access latency, and can be regarded as a user-managed cache resource during kernel execution.

Users can allocate shared memory either dynamically or statically.

1.  Static allocation: allocates a memory space of a specified size. The size is determined at compile time and cannot be modified dynamically. Developers allocate and use it through array allocation.

    ```
    __global__ void add_custom(...)
    {
        __ubuf__ half static_buf[1024];
        ...
    }
    ```

    By default, the start address of the allocated static memory is aligned according to the data type. Users can also manually specify the alignment by using `__align__(N)`.

2.  Dynamic allocation: You need to specify the size of the dynamic memory through the [dyn_ubuf_size](kernel_function.md#li12421238101815) parameter in <<<\>\>\>, and the size is determined at runtime. In SIMT programming, you can allocate and use dynamic memory in the following ways.

    ```
    // Device side: declare dynamic shared memory.
    __global__ void add_custom(...)
    {
        extern __ubuf__ char dynamic_buf[];
        ...
    }

    // Host side: specify the dynamic shared memory size through the third parameter in <<<>>>.
    int32_t main(int argc, char const *argv[])
    {
        ...
        uint32_t dyn_ubuf_size = 1024 * sizeof(char);
        add_custom<<<blocks_per_grid, threads_per_block, dyn_ubuf_size, stream>>>(...);
        ...
    }
    ```

### Shared Memory Size Limit

As shown in the following figure, the total size of the Unified Buffer memory space is 256 KB. In the default compilation mode, the UB is divided into four main areas by function, which are, from low addresses to high addresses, static memory, dynamic memory, reserved space, and Data Cache. Because these four areas share the same Unified Buffer memory, when configuring the shared memory size, you should reserve sufficient space for the Data Cache (at least 32 KB) to avoid verification errors caused by insufficient available space.

![](../../../figures/ub_memory_allocation.png)

The specific structure is as follows:

1.  Static memory and dynamic memory correspond to the memory allocated through static and dynamic application methods, respectively.

2.  Reserved space: space reserved by the compiler and Ascend C, with a fixed size of 8 KB.

3.  Data Cache: a Data Cache space dedicated to SIMT, used as the data cache when SIMT threads access global memory. The configurable range of the Data Cache space is **32 KB minimum and 128 KB maximum**. The actual memory size is affected by the static and dynamic memory sizes configured by the user. The specific calculation formula is as follows:

    ```
    Data Cache = min(UB size (256 KB) - static memory - dynamic memory - reserved space (8 KB), 128 KB)
    ```

    The upper limit of the Data Cache space is **128 KB**. Even if the allocated static memory and dynamic memory are small, the actually allocated Data Cache size will not exceed this upper limit.

Because the Data Cache, reserved space, and the UB memory allocated by the user share the same UB, developers cannot use the entire UB space and must ensure that the actual access range does not exceed the allocated static and dynamic memory sizes.

The declaration or configuration of static memory and dynamic memory defines the valid range of UB memory available to the user. If the configured memory size is insufficient, or if the user directly accesses a UB address without allocating memory, the accessed address may cross the boundary of the user's dynamic and static memory and fall into the Data Cache area. Because the Data Cache capacity is determined by subtracting the static memory, dynamic memory, and reserved space from the total UB size, a smaller shared memory configuration enlarges the remaining space that can be allocated to the Data Cache. When program execution crosses the boundary of the allocated dynamic and static memory and an out-of-bounds write occurs, it may not immediately manifest as a memory allocation failure, but it may corrupt the data cache used for SIMT global memory access, resulting in unstable computation results or abnormal operator precision.

> [!NOTE] Note
>
> You can disable the reserved space by adding the [--cce-disable-vf-stack-reserved-ubuf](../../compilation_and_execution/operator_compilation/ai_core_operator_compilation_basic_usage.md#ZH-CN_TOPIC_0000002462746461) option during compilation. After this option is enabled, the compiler no longer reserves this part of the UB space, which can then be used as normal UB space. The Data Cache space can be estimated using the following formula:
> ```
> Data Cache = min(UB size (256 KB) - static memory - dynamic memory - reserved space (8 KB), 128 KB)
> ```
> When this option is used, you must still ensure that the Data Cache space is not less than 32 KB, and the upper limit of the Data Cache space remains 128 KB. After this option is enabled, the compiler cannot use the reserved UB space as the cache space for register spill, and developers must ensure that no register spill occurs.

## Registers<a name="section1137715201010"></a>

In SIMT programming, each thread has its own registers, whose lifecycle is the same as that of the kernel function. Register usage is managed by the compiler, and registers are used for local storage of a thread during kernel function execution. The number of registers available to a thread during execution is related to the maximum thread count configured when the kernel function is defined. For details, see the following table.

**Table 1** Mapping between the maximum thread count and the number of registers available to each thread

| Maximum Thread Count | Number of Registers Available to Each Thread |
| --- | --- |
| 1025–2048 | 16 |
| 513–1024 | 32 |
| 257–512 | 64 |
| 1–256 | 127 |

As shown in the preceding table, the larger the maximum thread count configured when the kernel function is defined, the fewer registers each thread can use. If the user sets an excessively large thread count while the computational complexity of each thread is high, the compiler may temporarily store data in the stack space due to a lack of sufficient registers for local variables, which can easily lead to register spill and degrade operator performance. Therefore, users should configure the maximum thread count appropriately based on the actual operator complexity.

The maximum thread count and the number of registers available during thread execution can be configured through [\_\_launch\_bounds\_\_](../../language_extension/SIMT-BuiltIn_keyword.md#li23861114618) or [\_\_maxnreg\_\_](../../language_extension/SIMT-BuiltIn_keyword.md#section_maxnreg).