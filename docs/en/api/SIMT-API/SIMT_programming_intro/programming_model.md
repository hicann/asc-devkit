# Programming Model

<!-- md-trans-meta sourceCommit=2c97acf47d28b2c9fa27e8b745084f6e4f3eedea translatedAt=2026-09-09T02:30:45.265Z pushedAt=2026-09-09T10:00:56.682Z -->

## Applicable Scenarios

SIMT programming is suitable for operator scenarios with complex control flow and irregular memory access patterns. Because each thread has independent control flow and address computation capabilities, SIMT can naturally handle conditional branch logic and access memory randomly without being restricted to contiguous memory access patterns. In addition, SIMT provides mechanisms such as atomic operations and inter-thread memory barriers to support safe concurrent access to the same memory region by multiple threads, making it suitable for data structure operations such as hash tables that require conflict resolution and data synchronization. The advantage of SIMT lies in using the thread as the smallest scheduling unit to give each execution path sufficient independence, thereby achieving higher development efficiency and execution performance in scenarios with complex control flow and irregular memory access patterns.

## Thread Architecture

The SIMT programming model adopts a hierarchical abstract thread organization structure, as shown in the figure below. From top to bottom, the levels are: Grid (thread block grid), Thread Block, Warp, and Thread. This hierarchical design allows developers to conveniently map parallel computing problems onto hardware resources.

**Figure 1**  Thread structure schematic diagram

![](../../figures/thread_structure_diagram.png "Thread structure diagram")

**Thread**

The thread is the smallest unit in the entire structure. Each thread independently completes its computation tasks and has its own registers and stack space.

**Thread Block**

A Thread Block is the composing unit of the Grid and consists of several threads (up to 2048 threads). The built-in variable [blockDim](extended_syntax/built_in_variables.md#blockdim) represents the number of threads enabled in a Thread Block. In SIMT programming scenarios, a Thread Block has the following characteristics:

-   Threads within the same Thread Block can access shared memory (UB) to exchange data; threads within a Thread Block can cooperate through synchronization mechanisms;
-   When defining a SIMT function, you can use [\_\_launch\_bounds\_\_\(\)](extended_syntax/kernel_function_config.md#__launch_bounds__n) to configure the maximum number of threads.

**Grid**

The Grid is the top level of the SIMT thread hierarchy and consists of multiple Thread Blocks. The built-in variable [gridDim](extended_syntax/built_in_variables.md#griddim) represents the number of thread blocks enabled in the Grid. In SIMT programming scenarios, the Grid has the following characteristics:

-   The dimension configuration of the Grid is determined by the number of AIV cores configured by the user at launch and cannot be changed during kernel execution;
-   All thread blocks in the Grid have the same size and dimension configuration;
-   Thread blocks within the same Grid are independent of each other and execute in arbitrary order;

The constraints are as follows:

-   The total number of thread blocks in a Grid cannot exceed 65535.

**Thread Index**

Each thread has a unique identifier. Developers can obtain thread information through built-in variables to determine the data each thread is responsible for processing. The relevant built-in variables are shown in the following table:

| Built-in Variable | Description | Data Type | Constraint |
| --- | --- | --- | --- |
| gridDim | Dimension size of the Grid | dim3 | gridDim.x * gridDim.y * gridDim.z <= 65535 |
| blockDim | Dimension size of the Thread Block | dim3 | blockDim.x * blockDim.y * blockDim.z <= 2048 |
| blockIdx | Index of the current Thread Block in the Grid | dim3 | None |
| threadIdx | Index of the current thread within the Thread Block | dim3 | None |

Each Thread Block can be identified by the thread block index blockIdx, and each thread can be identified by the thread index threadIdx within the Thread Block. For a one-dimensional Grid and Block, the index calculation formula is:

```
int idx = blockIdx.x * blockDim.x + threadIdx.x;
```

For a two-dimensional Grid and Block, the index calculation formula is:

```
int x_index = blockIdx.x * blockDim.x + threadIdx.x;
int y_index = blockIdx.y * blockDim.y + threadIdx.y;
```

## Warp Execution Mechanism

A Warp is the basic scheduling and execution unit in the SIMT architecture. Each Warp contains 32 threads, which start execution from the same program address, each with its own instruction address counter and register state, and can branch and execute independently. Within a Thread Block, all threads are automatically divided by hardware into Warps of 32 threads each in linear order, and all threads in the same Warp execute the same instruction.

Although the threads in a Warp execute the same code, they can enter different execution paths through conditional branches. This situation is called Warp Divergence. When all 32 threads in a Warp execute the same code branch, hardware utilization is highest. Once Warp Divergence occurs, the hardware executes each branch path serially, and only the threads that enter the current branch (that is, the active threads) are executed, while the remaining threads are masked, thereby reducing Warp execution efficiency. In addition, threads within the same Warp are independent of each other and must not have dependencies on one another.

It is recommended that the number of threads in a Thread Block be set to a multiple of 32. If this requirement is not met, the last Warp will contain fewer than 32 threads, resulting in idle thread lanes within that Warp and thus reducing execution efficiency.

## UB Division

UB (Unified Buffer) is a memory space accessible to all threads within the same thread block, located inside each AIV. The total size of the UB memory space is 256 KB. As shown in [Figure 2](#fig9500834165913), it is divided into four main areas by function, which are, from low addresses to high addresses, static memory, dynamic memory, reserved space, and Data Cache.

**Figure 2** UB memory allocation diagram<a name="fig9500834165913"></a>  
![](../../figures/UB_memory_allocation.png "UB_memory_allocation")

## Memory Space Description

1.  Static memory: A memory space of a specified size is allocated from the start address of the memory. Its size is determined at compile time and cannot be modified dynamically.

    ```
    // Static memory is allocated through arrays.
    __ubuf__ char static_buf[1024];
    ```

2. Dynamic memory: Located after the static memory, a dynamic memory space whose size is specified by the parameter dyn\_ubuf\_size in [<<<...\>\>\>]( extended_syntax/kernel_function_config.md#-call) can be applied for in the following ways:

    ```
    // Apply for dynamic memory through a dynamic array.
    extern __ubuf__ char dynamic_buf[];
    ```

3.  Reserved space: System-reserved space with a fixed size of 8 KB.
4.  Data cache: A Data Cache space dedicated to SIMT. After deducting the static memory, dynamic memory, and reserved space from the UB, the remaining memory is the Data Cache. The Data Cache is at least 32 KB. When the remaining space exceeds 128 KB, the Data Cache size is fixed at 128 KB. The specific calculation formula is as follows:

    ```
    DataCache = min(UB总大小(256KB) – 静态内存 – 动态内存 – 预留空间(8KB), 128KB)
    ```

    If DataCache is less than 32 KB, a verification error occurs.
