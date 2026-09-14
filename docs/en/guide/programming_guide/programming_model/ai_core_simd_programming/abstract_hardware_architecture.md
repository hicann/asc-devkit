# Abstract Hardware Architecture<a name="ZH-CN_TOPIC_0000001861989988"></a>

<!-- md-trans-meta sourceCommit=d0ede7621d11bdc88cdafb221560ec6e0bfe996d translatedAt=2026-08-20T11:00:53.222Z pushedAt=2026-08-26T03:06:47.450Z -->

The AI Core is the compute core of an AI processor, and an AI processor contains multiple AI Cores. This section describes the parallel compute architecture abstraction of the AI Core, which shields the differences between different hardware. When programming with Ascend C, the abstract hardware architecture simplifies hardware details and significantly lowers the development threshold. For more detailed hardware architecture information or principles, see [Hardware Implementation](../../advanced_programming/hardware_implementation/basic_architecture.md).

## Overview<a name="section_core_components"></a>

The abstract hardware architecture of an AI Core can be divided into three types of core components: **compute units, storage units, and DMA units**. Compute units are responsible for performing scalar, vector, and matrix computations; storage units are responsible for storing input, output, and intermediate data; and DMA units are responsible for transferring data between different memory hierarchy levels. The following figure uses NPU architecture version 2201 as an example to show the positional relationships and collaboration of the three types of components in an AI Core.

**Figure 1**  Abstract hardware architecture ([NPU architecture version 2201](../../language_extension/SIMD-BuiltIn_keyword.md#table65291052154114))<a name="fig1095152043812"></a>

<img src="../../../figures/abstract_hardware_architecture_npu_architecture_version_2201_2.png" alt="Abstract hardware architecture" width="840px">

Combining the three types of components described above and [Figure 1](#fig1095152043812), after the operator instruction sequence delivered from the host side enters the AI Core, the Scalar compute unit is responsible for control logic and instruction issue; compute units such as Vector and Cube perform vector computation and cube computation respectively, and the DMA unit performs data transfer. Computation data generally flows between Global Memory and Local Memory; when there is a dependency between computation and data transfer, synchronization signals are required to constrain the execution order of different units.

## Architecture Version Differences<a name="section_arch_version"></a>

**Figure 2**  Abstract hardware architecture ([NPU architecture version 3510](../../language_extension/SIMD-BuiltIn_keyword.md#table65291052154114))<a name="fig10951520438120"></a>

![](../../../figures/abstract_hardware_architecture_npu_architecture_version_3510.png "Abstract hardware architecture [NPU architecture version 3510](../../language_extension/SIMD-BuiltIn_keyword.md#table65291052154114)")

Compared with NPU architecture version 2201, the main differences of NPU architecture version 3510 are as follows:

-   A Regbase vector computation method is added, which supports using registers directly in computation. For the differences between Membase and Regbase, see the following table.

-   Computation and storage capabilities are expanded, and the supported range of some data types and the on-chip storage structure change.    

-   The data transfer capability is adjusted, and the data flow paths between some storage units differ from those in NPU architecture version 2201.

-   Programming methods are more diverse. In addition to SIMD programming, SIMT programming and hybrid SIMD-SIMT programming are supported. For guidance on selecting among the three programming methods, see [AI Core Programming Model](../programming_model_overview.md#section_ai_core_programming_model).

**Table 1**  Differences between Membase and Regbase

| Computation Method | Data Storage Location | Feature | Applicable Scenario |
| --- | --- | --- | --- |
| Membase | Local Memory (UB) | The result of each computation step is written back to the UB. | NPU architecture version 2201 |
| Regbase | Register (VF Reg) | Intermediate results can be temporarily stored in registers, reducing UB reads/writes. | NPU architecture version 3510 |

> [!NOTE] Note
> - For the mapping between different product models and NPU architecture versions, see [__NPU_ARCH__](../../language_extension/SIMD-BuiltIn_keyword.md#table65291052154114).
> - For the specific hardware implementation of NPU architecture version 2201 and NPU architecture version 3510, see [NPU Architecture Version 2201](../../advanced_programming/hardware_implementation/architecture_spec/npu_architecture_version_2201.md) and [NPU Architecture Version 3510](../../advanced_programming/hardware_implementation/architecture_spec/npu_architecture_version_3510.md), respectively.
> - For detailed architecture changes, see [Architecture Changes from 2201 to 3510](../../../cross_gen_migration_guide/3510_arch_migration/architecture_changes_from_2201_to_3510.md).

## Compute Units<a name="section_compute_units"></a>

The compute units in the AI Core mainly include three types: Scalar, Vector, and Cube.

**Table 2** Compute units

<a name="zh-cn_topic_0000001588832845_table14884154362"></a>

| Component Name | Function | Corresponding Programming Concept |
| --- | --- | --- |
| Scalar | Performs scalar computation such as address computation and loop control, and dispatches vector computation, matrix computation, data transfer, and synchronization instructions to the corresponding units for execution. | Control logic in the kernel function and synchronization control APIs. |
| Vector | Performs vector computation. | Vector computation APIs and vector operators. |
| Cube | Performs cube computation. | Cube computation APIs and cube operators. |

## Storage Units and DMA Units<a name="section_memory_units"></a>

Storage units can be classified into Local Memory and Global Memory based on their usage locations.

-   **Local Memory**: On-chip storage of the AI Core, used to temporarily store data slices moved in from Global Memory and to hold computation outputs and intermediate results. This data can be efficiently accessed by compute units such as Vector and Cube to continue participating in on-chip computation, or moved out to Global Memory through the DMA unit. Local Memory contains on-chip resources for different programming methods and compute units, as shown in [Figure 3](#fig_simd_reg_memory_hierarchy).

-   **Global Memory**: Device-side global storage, serving as the primary source or destination for data moved into and out of Local Memory.

**Figure 3**  SIMD-Reg vector computation memory hierarchy<a name="fig_simd_reg_memory_hierarchy"></a>

![](../../../figures/simd_reg_vector_computation_memory_hierarchy_1.png "SIMD-Reg vector computation memory hierarchy")

**DMA (Direct Memory Access) unit** is responsible for data transfer, including moving data into and out of Global Memory and Local Memory, as well as data flow between Local Memory at different levels. Common DMA units include MTE1, MTE2, MTE3, and FixPipe. For details, see [DMA Unit](../../advanced_programming/hardware_implementation/basic_architecture.md#table288493152012).

## Execution Flow and Synchronization Mechanism<a name="section_programming_flow"></a>

When understanding the abstract hardware architecture, you also need to pay attention to how different units collaborate to complete the processing of a data block. This can be understood from three perspectives: **asynchronous instruction flow, synchronization signal flow, and computation data flow**:

-   **Asynchronous instruction flow**: represents the task dispatch relationship from the Scalar compute unit to other execution units, rather than the complete instruction execution flow. Based on the operator instruction sequence, the Scalar compute unit dispatches tasks such as computation and data transfer to the instruction queues of units such as Vector, Cube, and DMA, and each execution unit then executes asynchronously on its own pipeline. This process can be referenced from the instruction flow shown in [Figure 1](#fig1095152043812).

-   **Computation data flow**: represents the relationship in which each execution unit accesses or updates storage resources under instruction control, rather than the complete data copy-in, computation, and copy-out flow of a single computation task. For example, compute units such as Vector and Cube perform computation based on data in Local Memory, and the DMA unit is responsible for data transfer between Local Memory and Global Memory. This process can be referenced from the data flow shown in [Figure 1](#fig1095152043812).

-   **Synchronization signal flow**: represents the control path through which the Scalar compute unit constrains the execution order of tasks on different execution units via synchronization signals, rather than the direction of data flow itself. When asynchronous tasks on different units have data dependencies or order dependencies, synchronization signals are required to constrain the execution order. This process can be referenced from the synchronization signal flow shown in [Figure 1](#fig1095152043812).

Therefore, developers must not only focus on how data is moved in, computed, and moved out, but also understand how the synchronization mechanism ensures that asynchronous tasks are executed according to the correct dependency relationships. For details about the synchronization control APIs and how dependencies are expressed in programming, see [Intra-core Synchronization Overview](../../../../api/SIMD-API/basic_api/sync_control/intra_core_sync/intra_core_synchronization_capability_overview.md).