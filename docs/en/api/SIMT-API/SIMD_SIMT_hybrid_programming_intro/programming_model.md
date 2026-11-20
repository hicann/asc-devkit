# Programming Model

<!-- md-trans-meta sourceCommit=626c945f943983394d9c6e802021e1f2691ebee6 translatedAt=2026-10-27T03:40:07.262Z pushedAt=2026-10-27T09:21:55.710Z nl=2 -->

## Applicable Scenarios

In SIMD and SIMT mixed programming, SIMT simplifies the development of complex operators and irregular control flow, while SIMD, based on vector registers and instructions, achieves efficient data-parallel processing—that is, a single instruction processes multiple data elements—thereby improving throughput per cycle. SIMD and SIMT mixed programming allows developers to perform fine-grained mapping based on operator characteristics: regular elementwise operations obtain high bandwidth and high compute utilization through SIMD, while irregular or branch-containing computations are handled by SIMT to mitigate divergence and control complexity. At the system level, this helps improve hardware utilization and energy efficiency; it also facilitates optimizations such as operator fusion and data reuse. A single operator can contain both the continuous, regular computations that SIMD excels at and tasks such as scattered access that SIMT excels at, thereby leveraging the advantages of both SIMD and SIMT within the same operator.

## Thread Architecture

The SIMT programming model adopts a hierarchical abstract thread organization structure, as shown in the figure below. From top to bottom, the levels are: Grid (thread block grid), Thread Block, Warp, and Thread. This hierarchical design allows developers to conveniently map parallel computing problems onto hardware resources.

**Figure 1**  Thread structure schematic diagram

![](../../figures/thread_structure_62.png "Thread structure diagram-62")

**Thread**

The thread is the smallest unit in the entire architecture. Each thread independently completes its computation tasks and has its own registers and stack space.

**Thread Block**

A Thread Block is the building block of the Grid and consists of several threads (up to 2048 threads). The built-in variable [blockDim](extended_syntax/builtin_variables.md#blockdim) represents the number of threads enabled in a Thread Block. In SIMD and SIMT mixed programming scenarios, a Thread Block has the following characteristics:

-   When using [asc\_vf\_call](extended_syntax/kernel_function_config.md#asc_vf_call-call) to call a SIMT VF, you must explicitly specify the dimension information of the Thread Block;
-   Threads within the same Thread Block can access shared memory, that is, the Unified Buffer (UB), to exchange data; threads within a Thread Block can cooperate through synchronization mechanisms;
-   When defining a SIMT function, [\_\_launch\_bounds\_\_\(\)](extended_syntax/kernel_function_config.md#__launch_bounds__n) can be used to configure the maximum number of threads, which is used for pre-allocating register resources at the underlying level during compilation.

**Grid (Thread Block Grid)**

The Grid is the top level of the SIMT thread hierarchy and consists of multiple Thread Blocks. The built-in variable [gridDim](extended_syntax/builtin_variables.md#griddim) represents the number of thread blocks enabled in the Grid. In SIMD and SIMT mixed programming scenarios, the Grid has the following characteristics:

-   The dimension configuration of the Grid is determined by the number of AIV cores configured by the user at launch and cannot be changed during kernel execution;
-   All thread blocks in the Grid have the same size and dimension configuration;
-   Thread blocks within the same Grid are independent of each other and execute in arbitrary order;

The constraints are as follows:

-   The total number of thread blocks in a Grid cannot exceed 65535.
-   Because gridDim is actually the number of AIV cores configured for launch by the user, when the configured number of cores exceeds the number of physical cores, the head and tail overhead increases. It is strongly recommended that the number of AIV cores launched be less than the number of physical cores.

**Thread Index**

Each thread has a unique identifier. Developers can obtain thread information through built-in variables to determine the data each thread is responsible for processing. The relevant built-in variables are shown in the following table:

| Built-in Variable | Description | Data Type | Constraint |
| --- | --- | --- | --- |
| gridDim | Dimension size of the Grid. | dim3 | gridDim.x <= 65535; gridDim.y and gridDim.z must be 1 |
| blockDim | Dimension size of the Thread Block. | dim3 | blockDim.x * blockDim.y * blockDim.z <= 2048 |
| blockIdx | Index of the current Thread Block in the Grid. | dim3 | None |
| threadIdx | Index of the current thread within the Thread Block. | dim3 | None |

Each Thread Block can be identified by the thread block index blockIdx, and each thread can be identified by the thread index threadIdx within the Thread Block. For a one-dimensional Grid and Block, the index calculation formula is:

```cpp
int idx = blockIdx.x * blockDim.x + threadIdx.x;
```

For a two-dimensional Grid and Block, the index calculation formula is:

```cpp
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
![](../../figures/ub_memory_allocation.png "UB memory allocation diagram-63")

### UB Memory Space Description

1.  Static memory: A memory space of a specified size is allocated from the start address of the memory. Its size is determined at compile time and cannot be modified dynamically.

    ```cpp
    // Static memory is allocated through arrays, for example:
    __ubuf__ char static_buf[1024];
    ```

2.  Dynamic memory: located after static memory, a dynamic memory space whose size is specified by the parameter dyn\_ub\_size in the [<<<...\>\>\>](extended_syntax/kernel_function_config.md#-call), which can be allocated and used in the following ways:

    -   Allocate through a dynamic array.

        ```cpp
        // Dynamic memory is allocated through a dynamic array, for example:
        extern __ubuf__ char dynamic_buf[];
        ```

    -   Allocate through the Alloc API of LocalMemAllocator.

    Because dynamic memory is always allocated starting from the position after the end of static memory, using both methods simultaneously may cause address space overlap and thus lead to undefined behavior. Therefore, only one of the two methods can be used for allocation.

3.  Reserved space: System-reserved space with a fixed size of 8 KB.
4.  Data Cache: A Data Cache space dedicated to SIMT. After deducting the static memory, dynamic memory, and reserved space from the UB, the remaining memory is the Data Cache. The Data Cache is at least 32 KB. When the remaining space exceeds 128 KB, the Data Cache size is fixed at 128 KB. The specific calculation formula is as follows:

    ```
    Data Cache size = min(total UB size (256KB) - static memory - dynamic memory - reserved space (8KB), 128KB)
    ```

    If DataCache is less than 32 KB, a verification error occurs.

For more rules on the layout, reuse, and alignment of UB memory, see [UB Memory Division in SIMD and SIMT Mixed Programming Scenarios](../../../guide/programming_guide/advanced_programming/advanced_ai_core_programming_model/simd_simt_hybrid_programming/memory_hierarchy.md#ub-division).