# Abstract Hardware Architecture<a name="ZH-CN_TOPIC_0000002593949261"></a>

<!-- md-trans-meta sourceCommit=7ef8ad7bdbdb0cef0f561d391759ecb8b4e4043a translatedAt=2026-10-23T14:05:37.385Z pushedAt=2026-11-03T07:05:13.412Z nl=2 -->

Ascend C supports SIMD and SIMT hybrid programming to achieve efficient coordination between vector-level parallelism and thread-level parallelism. The AI Core consists of AIC and AIV. The AIV core supports both SIMT and SIMD working modes, enabling hybrid SIMD and SIMT programming within the AIV, as well as fused programming between the SIMT of the AIV and the SIMD of the AIC. The overall hardware architecture is shown in the following figure:

**Figure 1** Hardware architecture schematic diagram of SIMD and SIMT hybrid programming  
![](../../../../figures/simt_arch.png)

## Vector Function Working Mechanism<a name="zh-cn_topic_0000002571695539_section471921191712"></a>

As shown in the preceding hardware architecture, the compute units of the AIV core include both SIMD and SIMT units. For these two hardware compute units, Ascend C abstracts the software concept of Vector Function, which represents a specific functional code segment executed on the SIMT or SIMD hardware compute unit. Users write SIMD Vector Functions or SIMT Vector Functions to invoke the corresponding execution units to complete computation tasks, and call different types of Vector Functions in the operator kernel function to switch between SIMD and SIMT hardware units.

As shown in the following figure, simt\_func indicates a code segment executed on the SIMT hardware unit, and simd\_func indicates a code segment executed on the SIMD hardware unit. The code segment attributes are identified by [\_\_simt\_vf\_\_](kernel_and_vf_functions.md#zh-cn_topic_0000002571578013_section1780955884616) and [\_\_simd\_vf\_\_](../../../language_extension/simd_builtin_keywords.md#section192521344610).

**Figure 2** VF schematic diagram for SIMD and SIMT hybrid programming  
![](../../../../figures/simt_vf.png)

During AIV execution, the Scalar compute unit dispatches Vector Functions to the Vector Function Queue. Each Vector Function is then executed serially and asynchronously with the MTE.

### SIMD Vector Function

When the AIV core is in SIMD working mode, the execution model follows the [Reg vector computation method](../../../programming_model/ai_core_simd_programming/c_pointer_programming/reg_vector_computation.md). The hardware units in the core that participate in Reg vector computation include:

- **Reg vector execution unit**: used to execute Reg vector computation. It reads data from registers, completes the computation, and writes the results back to registers.
- **DMA unit**: used to perform Reg vector transfer, responsible for moving data between registers and the Unified Buffer (UB).
- **Aux Scalar**: handles the scalar computation (such as address calculation) required by the Reg vector execution unit and the Reg data movement unit.

Although the Reg vector execution unit, DMA unit, and Aux Scalar belong to different hardware execution units, they are all assigned to the PIPE_V pipeline during actual execution. This architecture leads to two direct consequences:

1. When there are register dependencies, the hardware ensures data dependency correctness in instruction order, so users do not need to explicitly insert synchronization. However, cross-register reads and writes to the same UB region require explicit synchronization, because there is no automatic ordering constraint between the data movement unit and the computation unit.
2. When there is no data dependency, the Reg vector execution unit and the DMA unit can **issue and execute in parallel**.

**Figure 3** SIMD Reg vector computation schematic diagram  
<img src="../../../../figures/reg_exec.png" title="Reg execution unit" style="zoom:80%;"/>

### SIMT Vector Function

When the AIV core operates in SIMT mode, the execution model follows the [thread architecture](../../../programming_model/ai_core_simt_programming/thread_architecture.md) composed of Thread, Block, and Grid. A SIMT VF corresponds to the execution context of one thread block. Threads within a thread block are divided into multiple Warps by linear thread ID, and each Warp contains 32 threads.

Threads within a warp share the same instruction stream, and each thread maintains independent execution states such as thread index, registers, and stack. The hardware issues instructions based on the thread active mask: when the control flow of threads within a warp is consistent, high execution efficiency can be maintained; when branch divergence occurs, the hardware executes active threads in batches along different branch paths, and the effective parallelism decreases accordingly.

**Figure 4** SIMT thread architecture schematic diagram  
<img src="../../../../figures/simt_thread.png" title="SIMT thread architecture schematic diagram" style="zoom:80%;"/>
