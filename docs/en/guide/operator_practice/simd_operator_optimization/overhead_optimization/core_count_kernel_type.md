# Setting an Appropriate Core Count and Operator Kernel Type<a name="ZH-CN_TOPIC_0000002369641145"></a>

<!-- md-trans-meta sourceCommit=2daa1daa17f1ebe2799e2709e073446994e14981 translatedAt=2026-10-23T12:49:15.672Z pushedAt=2026-11-03T07:05:13.345Z nl=2 -->

During operator execution, additional startup overhead or header overhead may be incurred for the following reasons:

1.  **Core startup**: Each core needs to perform initialization operations when starting up, loading the necessary configurations and resources.
2.  **Core instruction fetch TLB MISS**: When a core accesses memory, if the corresponding page table entry is not present in the Translation Lookaside Buffer (TLB), the page table entry must be loaded from memory, which causes additional latency.
3.  **Same-address access conflict**: Due to hardware limitations, conflicts may occur when multiple cores access the same memory address simultaneously, causing additional latency.
4.  **Variable resource initialization**: Before operator execution, some variables and resources need to be initialized, which may also introduce additional performance overhead.

The header overhead increases as the number of cores used increases. The following figure shows how this header overhead varies with the startup core count.

**Figure 1** Header overhead versus startup core count<a name="fig118431911104314"></a>  
![](../../../figures/header_cost.png "Header overhead versus startup core count")

**For operators whose overall duration is at the microsecond level and whose per-core computation takes little time, performance can be improved by reducing the startup core count and increasing the per-core computation workload.** The essence of this optimization is to strike a balance between the header overhead time and the per-core computation time. To achieve optimal performance, developers need to find the most appropriate core count setting through hands-on experimentation.

-   For custom operator projects, you can set the number of cores used by the operator through the SetSimdNumBlocks API in TilingFunc (the default function provided by the operator project for computing Tiling on the host side). For details about the setting method, see SetSimdNumBlocks. For Kernel direct invocation projects, you can specify the number of cores used by the operator when <<<\>\>\> is called.
-   In addition, the kernel function type of an operator also affects the number of cores started for the operator. Taking a vector-only operator as an example, if the operator is executed in mixed startup mode, the scheduler starts both Vector cores and Cube cores. However, in this case, the Cube cores have no actual computation instructions but still incur the overhead of core startup and core initialization. Therefore, it is recommended that you set an appropriate kernel function type to minimize the overhead.

    Generally, an operator project automatically identifies the operator type based on the instructions used by the operator. However, this feature cannot distinguish the ratio of AIC to AIV, and by default it dispatches tasks at an AIC:AIV ratio of 1:2. In addition, the automatic identification feature may fail because it depends on the compilation optimization result. Therefore, it is recommended that you manually set the kernel function type of the operator. For details about how to set it, see [Setting the Kernel Function Type](../../../../api/SIMD-API/basic_api/Kernel-Tiling/set_Kernel_type.md).
