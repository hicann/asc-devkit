# Abstract Hardware Architecture<a name="ZH-CN_TOPIC_0000001861989988"></a>

<!-- md-trans-meta sourceCommit=ca3b72f6b4f376702407e982cf2cc174087341f5 translatedAt=2026-10-28T08:34:36.376Z pushedAt=2026-11-03T07:08:03.550Z nl=2 -->

AI Core is the compute core of an AI processor, and an AI processor contains multiple AI Cores. This section describes the parallel computation architecture abstraction of AI Core, which shields the differences between different hardware. When programming with Ascend C, the abstract hardware architecture simplifies hardware details and significantly lowers the development threshold. For more detailed hardware architecture information or principles, see [Hardware Implementation](../../advanced_programming/hardware_implementation/basic_architecture.md).

## Overview<a name="section_core_components"></a>

The abstract hardware architecture of the AI Core can be divided into three types of core components: [**compute units**](#section_compute_units), [**storage units**](#section_memory_units), and [**transfer units**](#section_memory_units). The following figure uses NPU architecture version 2201 as an example to show the positional relationships and collaboration of the three types of components within the AI Core.

**Figure 1** Abstract hardware architecture ([NPU architecture version 2201](../../language_extension/simd_builtin_keywords.md#npu-arch))<a name="fig1095152043812"></a>

<img src="../../../figures/npu_2201_2.png" alt="Abstract hardware architecture" width="840px">

Combining the three types of components described above and [Figure 1](#fig1095152043812), a single operator execution can be understood as a process in which the three types of components collaborate to complete data processing: after the host side delivers an operator task, the operator instruction sequence enters the AI Core on the device side; the compute units are responsible for control logic as well as scalar, vector, and cube computation; the storage units store input, output, and intermediate data; and the transfer units are responsible for moving data between different storage levels.

It should be noted that the process described above does not imply that each step is executed in a strictly serial manner. In a heterogeneous system, asynchronous parallelism exists between the host side and the device side, and pipelined parallelism also exists among different execution units within the device. The following sections describe the specific composition of the compute units, storage units, and transfer units respectively, and further explain the relationship between asynchronous execution and synchronization mechanisms.

## Compute Units<a name="section_compute_units"></a>

The compute units in the AI Core mainly include three types: Scalar, Vector, and Cube.

**Table 1** Compute units

<a name="zh-cn_topic_0000001588832845_table14884154362"></a>

| Component Name | Function | Corresponding Programming Concept |
| --- | --- | --- |
| Scalar | Performs scalar computation such as address calculation and loop control, and dispatches vector computation, cube computation, data transfer, and synchronization instructions to the corresponding units for execution. | Control logic in the kernel function (Kernel) and synchronization control APIs. |
| Vector | Performs vector operations. | Vector computation APIs and vector operators. |
| Cube | Performs cube operations. | Cube computation APIs and cube operators. |

## Storage Units and Transfer Units<a name="section_memory_units"></a>

Storage units can be divided into Local Memory and Global Memory based on their usage location.

-   **Local Memory**: On-chip storage of the AI Core, used to temporarily store data shards moved in from Global Memory and to hold computation outputs and intermediate results. This data can be efficiently accessed by compute units such as Vector and Cube to continue participating in on-chip computation, or moved out to Global Memory through the transfer unit. Local Memory contains on-chip resources for different programming methods and compute units, as shown in [Figure 2](#fig_simd_reg_memory_hierarchy).
-   **Global Memory**: Device-side global storage, which is the primary source or destination for data moved into and out of Local Memory.

**Figure 2**  SIMD-Reg vector computation memory hierarchy<a name="fig_simd_reg_memory_hierarchy"></a>

<img src="../../../figures/simd_reg_2.png" alt="SIMD-Reg vector computation memory hierarchy" width="520px">

The **DMA (Direct Memory Access) transfer unit** is responsible for data transfer, including moving data into and out of Local Memory from and to Global Memory, as well as data flow between different levels of Local Memory. Common transfer units include MTE1, MTE2, MTE3, and FixPipe. For details, see [Transfer Units](../../advanced_programming/hardware_implementation/basic_architecture.md#table288493152012).

## Execution Flow and Synchronization Mechanism<a name="section_programming_flow"></a>

When understanding the abstract hardware architecture, you also need to pay attention to how different units collaborate to complete the processing of a data block. This can be understood from three perspectives: **asynchronous instruction flow, synchronization signal flow, and computation data flow**.

-   **Asynchronous instruction flow**: represents the task dispatch relationship from the Scalar compute unit to other execution units, rather than the complete instruction execution process. Based on the operator instruction sequence, the Scalar compute unit dispatches computation, transfer, and other tasks to the instruction queues of units such as Vector, Cube, and DMA, and each execution unit then executes asynchronously on its own pipeline. This process can be referenced in the instruction flow shown in [Figure 1](#fig1095152043812).
-   **Computation data flow**: represents the relationship in which each execution unit accesses or updates storage resources under instruction control, rather than the complete data load, computation, and store process of a single computation task. For example, compute units such as Vector and Cube perform computation based on data in Local Memory, while the DMA transfer unit is responsible for data flow between Local Memory and Global Memory. This process can be referenced in the data flow shown in [Figure 1](#fig1095152043812).
-   **Synchronization signal flow**: represents the control path through which the Scalar compute unit constrains the execution order of tasks on different execution units by means of synchronization signals, rather than the direction of the data itself. When asynchronous tasks on different units have data dependencies or order dependencies, synchronization signals are required to constrain the execution order. This process can be referenced in the synchronization signal flow shown in [Figure 1](#fig1095152043812).

Therefore, developers must not only pay attention to how data is loaded, computed, and stored, but also understand how the synchronization mechanism ensures that asynchronous tasks are executed according to the correct dependency relationships. For details about the synchronization control APIs and how dependencies are expressed in programming, see [Intra-Core Synchronization Overview](../../../../api/SIMD-API/basic_api/sync_control/intra_core_sync/intra_core_sync_overview.md).

## Architecture Version Differences<a name="section_arch_version"></a>

**Figure 3**  Abstract hardware architecture ([NPU architecture version 3510](../../language_extension/simd_builtin_keywords.md#npu-arch))<a name="fig10951520438120"></a>

<img src="../../../figures/npu_3510_2.png" alt="Abstract hardware architecture NPU architecture version 3510" width="840px">

Compared with NPU architecture version 2201, the main differences of NPU architecture version 3510 are as follows:

-   A new [Regbase vector computation method](c_pointer_programming/reg_vector_computation.md) is added, which supports using registers directly in computation. For the differences between Membase and Regbase, see the following table.
-   The supported ranges of compute units, storage units, and data types differ. For details, see [Storage Unit Changes](../../../cross_gen_migration_guide/3510_arch_migration/2201_to_3510_arch_changes.md#section_2201_to_3510_storage_unit_changes) and [Built-in Data Types](../../../../api/SIMD-API/basic_api/data_structures/builtin_data_types.md).
-   The programming methods are more diverse. In addition to SIMD programming, SIMT programming and hybrid SIMD-SIMT programming are also supported. For guidance on selecting among the three programming methods, see [AI Core Programming Model](../programming_model_overview.md#section_ai_core_programming_model).

**Table 2**  Differences between Membase and Regbase

| Computation Method | Data Staging Location | Feature | Application Scenario |
| --- | --- | --- | --- |
| Membase | Local Memory (UB) | The result of each computation step is written back to UB | NPU architecture version 2201 |
| Regbase | Register (VF Reg) | Intermediate results can be staged in registers, reducing UB reads and writes | NPU architecture version 3510 |

>[!NOTE] Note
>- For the mapping between different product models and NPU architecture versions, see [__NPU_ARCH__](../../language_extension/simd_builtin_keywords.md#npu-arch).
>- For detailed architecture changes, see [Architecture Changes from 2201 to 3510](../../../cross_gen_migration_guide/3510_arch_migration/2201_to_3510_arch_changes.md).
