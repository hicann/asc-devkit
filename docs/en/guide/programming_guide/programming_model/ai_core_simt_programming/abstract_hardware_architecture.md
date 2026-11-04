# Abstract Hardware Architecture<a name="ZH-CN_TOPIC_0000002556525479"></a>

<!-- md-trans-meta sourceCommit=754fb95859dead826727a591e674261c6a829a08 translatedAt=2026-10-28T09:16:23.521Z pushedAt=2026-11-03T07:08:03.563Z nl=2 -->

SIMT (Single Instruction Multiple Thread) programming on the AI Core allows instructions to address data independently, thereby supporting thread-level parallel computation. This approach is particularly suitable for scenarios such as discrete access and complex control logic<!-- npu="950" id1 -->, and is currently supported only on Ascend 950PR&950DT products<!-- end id1 -->. SIMT programming can effectively simplify the development of complex operators and irregular control flow, mitigate divergent computation involving branches, and control program complexity, thereby improving hardware utilization and energy efficiency.

As shown in [Figure 1](#fig35720259287), the AI processor contains multiple Vector Cores. Each Vector Core includes compute units, Shared Memory (located in the Unified Buffer (UB)), and registers. The Global Memory outside the cores is the global memory space shared by all Vector Cores.

**Figure 1**  SIMT abstract hardware architecture diagram<a name="fig35720259287"></a>  
![](../../../figures/simt_hw.png "SIMT abstract hardware architecture diagram")

The following describes the hardware resources involved in SIMT multi-thread computation:

-   Each thread has its own independent registers for storing local data. The number of registers is affected by the number of threads in a thread block: the more threads there are, the fewer registers each thread has.
-   The UB is divided into multiple areas by function. A portion of its memory space serves as the Shared Memory for all threads in a thread block, supporting data exchange among threads within the block; another portion serves as the Data Cache for caching global memory data.
-   The L2 Cache is a high-speed cache shared by all Vector Cores. It is located between the Global Memory and the Data Cache of each Vector Core, and is used to cache global memory data to reduce access latency. The L2 Cache is automatically managed by hardware, and no explicit configuration is required from users.
-   In SIMT mode, when data is read from the Global Memory, the data is transferred through the Data Cache unit. The data flows from the Global Memory to the Data Cache, and then from the Data Cache to the registers. The Data Cache is a portion of space reserved in the UB, and its actual size is allocated by the user.

If you are not familiar with concepts such as threads and thread blocks mentioned above, see [Thread Architecture](thread_architecture.md) to learn more about the SIMT thread architecture. You can also read [Memory Hierarchy](memory_hierarchy.md) to learn how to configure the sizes of the Shared Memory and Data Cache in the UB.
