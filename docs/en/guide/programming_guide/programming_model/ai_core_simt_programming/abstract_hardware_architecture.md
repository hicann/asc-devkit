# Abstract Hardware Architecture<a name="ZH-CN_TOPIC_0000002556525479"></a>

<!-- md-trans-meta sourceCommit=4c81e54975ec65486e709c6cf9f279160b0190ce translatedAt=2026-08-20T11:31:24.074Z pushedAt=2026-08-26T03:07:01.209Z -->

Single Instruction Multiple Thread (SIMT) programming on the AI Core allows instructions to address data independently, thereby supporting thread-level parallel computation. This approach is particularly suitable for scenarios such as discrete access and complex control logic, and is currently supported only on Ascend 950PR/Ascend 950DT. SIMT programming can effectively simplify the development of complex operators and irregular control flow, mitigate divergent computation involving branches, and control program complexity, thereby improving hardware utilization and energy efficiency.

As shown in [Figure 1](#fig35720259287), the AI processor contains multiple Vector Cores. Each Vector Core includes compute units, Shared Memory (located in the Unified Buffer), and registers. The Global Memory outside the cores is shared by all Vector Cores.

**Figure 1**  SIMT abstract hardware architecture diagram<a name="fig35720259287"></a>  
![](../../../figures/simt_abstract_hardware_architecture_diagram.png "SIMT abstract hardware architecture diagram")

The following describes the hardware resources involved in SIMT multi-threaded computation:

-   Each thread has its own registers for storing local data. The number of registers is affected by the number of threads in a thread block: the more threads there are, the fewer registers each thread has.

-   The Unified Buffer is divided into multiple areas by function. Part of its memory space serves as the shared memory for all threads in a thread block, supporting data exchange among threads within the block. Another part serves as the Data Cache for caching global memory data.

-   The L2 Cache is a high-speed cache shared by all Vector Cores. It sits between the Global Memory and the Data Cache of each Vector Core, and is used to cache global memory data to reduce access latency. The L2 Cache is managed automatically by hardware, so users do not need to configure it explicitly.

-   In SIMT mode, when reading data from the Global Memory, data transfer is completed through the Data Cache unit. The data flows from the Global Memory to the Data Cache, and then from the Data Cache to the registers. The Data Cache is a portion of space reserved in the Unified Buffer, and its actual size is allocated by the user.

If you are not familiar with concepts such as threads and thread blocks mentioned above, see [Thread Architecture](thread_architecture.md) to learn more about the SIMT thread architecture. You can also read [Memory Hierarchy](memory_hierarchy.md) to learn how to configure the sizes of the shared memory and Data Cache in the Unified Buffer.