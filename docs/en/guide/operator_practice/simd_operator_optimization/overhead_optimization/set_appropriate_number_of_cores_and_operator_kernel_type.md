# Setting an Appropriate Core Count and Operator Kernel Type<a name="ZH-CN_TOPIC_0000002369641145"></a>

<!-- md-trans-meta sourceCommit=e3c8c70b3f5d27f3770063ae4b535bc75d904e3e translatedAt=2026-08-26T14:34:30.260Z -->

During operator execution, additional startup overhead or overhead may be incurred for the following reasons:

1.  **Core startup**: Each core needs to perform initialization operations when it starts, loading the necessary configurations and resources.
2.  **Core address fetch TLB MISS**: When a core accesses memory, if the corresponding page table entry is not present in the Translation Lookaside Buffer (TLB), the page table entry must be loaded from memory, which causes additional latency.
3.  **Same-address access conflict**: Due to hardware limitations, conflicts may occur when multiple cores access the same memory address simultaneously, causing additional latency.
4.  **Variable resource initialization**: Before the operator is executed, some variables and resources need to be initialized, which may also introduce additional performance overhead.

The overhead increases as the number of cores used increases. The following figure shows how this overhead varies with the launch core count.

**Figure 1** Overhead varying with the launch core count<a name="fig118431911104314"></a>  
![](../../../figures/head_overhead_change_with_launch_core_count.png "Overhead varying with the launch core count")

**For operators whose overall duration is at the microsecond level and whose per-core computation takes little time, performance can be improved by reducing the launch core count and increasing the per-core computation.** The essence of this optimization is to strike a balance between the overhead and the per-core computation time. To achieve optimal performance, developers need to find the most appropriate core count setting through practice and experimentation.

-   For custom operator projects, you can set the core count used by the operator through the SetBlockDim API in TilingFunc (the default function provided by the operator project for computing Tiling on the Host side). For details about how to set it, see SetBlockDim. For Kernel direct invocation projects, you can specify the core count used by the operator in the <<<\>\>\> invocation.
-   In addition, the kernel type of an operator also affects the number of cores launched for the operator. Taking a vector-only operator as an example, if the operator is executed in hybrid launch mode, the scheduler launches both Vector cores and Cube cores simultaneously. However, in this case, the Cube cores have no actual computation instructions but still incur the overhead of core launch and core initialization. Therefore, it is recommended that you set an appropriate Kernel type to minimize the overhead.

    Generally, an operator project automatically identifies the operator type based on the instructions used by the operator. However, this feature cannot distinguish the ratio between AIC and AIV, and by default dispatches tasks at an AIV:AIC ratio of 1:2. In addition, the automatic identification feature may fail because it depends on the result of compilation optimization. Therefore, it is recommended that you manually set the Kernel type of the operator. For details about how to set it, see [Setting the Kernel Type](../../../../api/SIMD-API/basic_api/Kernel-Tiling/set_kernel_type.md).
