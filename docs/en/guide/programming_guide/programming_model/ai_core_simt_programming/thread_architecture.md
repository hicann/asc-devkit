# Thread Architecture<a name="ZH-CN_TOPIC_0000002554358977"></a>

<!-- md-trans-meta sourceCommit=2bc2b38d5648dc61f632186b01ba7f36c2295b13 translatedAt=2026-08-20T11:42:11.725Z pushedAt=2026-08-26T03:07:11.416Z -->

The SIMT programming model adopts a hierarchical abstract thread organization structure, as shown in the following figure. From the top level to the bottom level, the structure consists of grid (thread block grid), thread block, and thread. This hierarchical design allows developers to conveniently map parallel compute problems onto hardware resources.  
![](../../../figures/simt_thread_structure.png)

## Thread Hierarchy<a name="zh-cn_topic_0000002540938034_section8959121611415"></a>

**Thread**

A thread is the smallest unit in the entire architecture. Each thread independently completes its computation task and has its own registers and stack space.

**Thread Block**

A thread block consists of multiple threads (up to 2048 threads). The built-in variable [blockDim](../../language_extension/SIMT-BuiltIn_keyword.md#blockdim) indicates the number of threads in each dimension of a thread block. A thread block has the following characteristics:

-   Threads within the same thread block can access shared memory for data exchange.

-   Threads within a thread block can collaborate through synchronization mechanisms.

**Grid (Thread Block Grid)**

Grid is the topmost level of the SIMT thread hierarchy and consists of multiple thread blocks. The built-in variable [gridDim](../../language_extension/SIMT-BuiltIn_keyword.md#griddim) is used to represent the number of thread blocks in each dimension of the grid, and it has the following characteristics:

-   The dimensions of the grid are determined by the user's execution configuration and cannot be changed during kernel function execution;

-   All thread blocks in a grid have the same size and dimension configuration;

-   Thread blocks in the same grid are independent of each other and execute in any order;

-   A program based on the SIMT programming model executes multiple thread blocks with the same structure on the AIV core. The total number of threads executed is equal to gridDim.x\*gridDim.y\*gridDim.z\*blockDim.x\*blockDim.y\*blockDim.z.

The total number of thread blocks in the current grid cannot exceed 65535.

**Thread Index**

Under the SIMT thread architecture, each thread has a unique identifier. Developers can obtain thread identity information through built-in variables to determine the data that each thread is responsible for processing. The related built-in variables are listed in the following table:

| Built-in Variable | Description | Data Type | Constraint |
| --- | --- | --- | --- |
| gridDim | Dimension size of the grid. | dim3 | gridDim.x \* gridDim.y \* gridDim.z <= 65535 |
| blockDim | Dimension size of the thread block. | dim3 | blockDim.x \* blockDim.y \* blockDim.z <= 2048 |
| blockIdx | Index of the current thread block in the grid. | dim3 | None |
| threadIdx | Index of the current thread within the thread block. | dim3 | None |

Each thread block can be identified by the thread block index blockIdx, and each thread can be identified by the thread index threadIdx within the thread block. An example of thread index calculation is shown in the following figure:

![](../../../figures/thread_architecture_8.png)

For a one-dimensional grid and block, the index calculation formula is as follows:

```
int global_id = blockIdx.x * blockDim.x + threadIdx.x;
```

For a two-dimensional grid and block, the index calculation formula is as follows:

```
int global_id_x = blockIdx.x * blockDim.x + threadIdx.x;
int global_id_y = blockIdx.y * blockDim.y + threadIdx.y;
```

## Warp Execution Mechanism<a name="zh-cn_topic_0000002540938034_section3214318716"></a>

In fact, the threads within a thread block cannot execute concurrently. At the underlying level, threads in a thread block are divided into warps. Within a thread block, all threads are automatically divided by hardware into warps of 32 threads each in linear order, and all threads in the same warp execute the same instruction. A warp is the basic scheduling and execution unit in the SIMT architecture. Threads in a warp start execution from the same program address, each with its own instruction address counter and register state, and can branch and execute independently. Based on the warp division mechanism, it is recommended that blockDim of a thread block be set to an integer multiple of 32. Otherwise, the last warp will contain fewer than 32 threads, leaving idle thread lanes in that warp and thus reducing execution efficiency.

Although threads in a warp execute the same code, they may enter different execution paths at a branch. This situation is called branch divergence. Control flow instructions (if, switch, do, for, and while) may all cause branch divergence. When all 32 threads in a warp execute the same code branch, hardware utilization is the highest. Once branch divergence occurs, only the threads that enter the current branch (that is, the active threads) are executed, while the remaining threads are blocked and wait, thereby reducing warp execution efficiency. For example, if threads in a warp encounter a conditional branch, threads with threadIdx.x less than 8 and threads with threadIdx.x greater than or equal to 8 enter different execution paths. Because threads in the same warp must execute the same instruction synchronously, the hardware executes each branch serially: first, the branch where threadIdx.x is less than 8 is executed, while threads with threadIdx.x greater than or equal to 8 are blocked and wait; then, the branch where threadIdx.x is greater than or equal to 8 is executed, while threads with threadIdx.x less than 8 are blocked and wait. After branch divergence ends, all threads reconverge and continue execution, as shown in the following figure. In addition, threads in the same warp are independent of each other and cannot have dependencies on one another.

![](../../../figures/branch_divergence.png)

## Configuring the Maximum Thread Count

As mentioned above, threads within each thread block have independent registers and stack space, while the total registers of each AIV core are limited. The more complex the operator computation task is, the more registers are required, and the fewer threads can be launched. Currently, you can use [\_\_launch\_bounds\_\_\(N\)](../../language_extension/SIMT-BuiltIn_keyword.md#li23861114618) when defining a SIMT function to configure the maximum thread count `N`, which limits the upper bound of the number of threads that an operator can launch. At compile time, the underlying compiler allocates register resources based on this value. The larger the `N` you configure, the fewer registers are available to each thread. The specific relationship is described in the following table:

| Configured Maximum Thread Count | Number of Registers Available per Thread |
| --- | --- |
| 1025–2048 | 16 |
| 513–1024 | 32 |
| 257–512 | 64 |
| 1–256 | 127 |

Note the following:

- \_\_launch\_bounds\_\_\(N\) is an optional configuration, and its default value is 1024.

- `N` is a value determined at compile time, so it must be a static variable or a constant.

- The number of threads actually launched during operator execution cannot exceed the maximum thread count. Otherwise, a runtime error occurs.

- The configured maximum thread count should match the operator complexity. If `N` is too large, each thread is allocated too few registers, and during thread execution the compiler may choose to store some data in the stack space, which is called register spilling. This affects compute performance and may even cause stack overflow, leading to precision exceptions that affect the operator functionality. If `N` is too small, it limits the number of threads that can actually be launched, resulting in resource wasting.