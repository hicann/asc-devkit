# Architecture Changes from 2201 to 3510<a name="ZH-CN_TOPIC_0000002503428217"></a>

<!-- md-trans-meta sourceCommit=f6490d1e030d257ee55f3f1600bffece5a8bd3f7 translatedAt=2026-08-26T12:47:17.940Z -->

The architecture diagram of [NPU architecture version 3510](../../programming_guide/language_extension/SIMD-BuiltIn_keyword.md#table65291052154114) is shown in [Figure 1](#fig1097417534526). Overall, the 3510 architecture adds the following features:

-   Multiple data paths are added.
-   The number of AI Cores is increased.
-   The UB capacity is increased.
-   The SSBuffer on-core storage unit is added, which supports access by AIC cores and AIV cores through scalars.
-   In addition to SIMD programming, SIMT programming and hybrid SIMD/SIMT programming are supported.
-   AIV cores adopt the Regbase architecture. Compared with the Membase architecture of [NPU architecture version 2201](../../programming_guide/language_extension/SIMD-BuiltIn_keyword.md#table65291052154114), the Regbase architecture allows direct operations on the vector registers of the chip, achieving greater flexibility and better performance.

**Figure 1**  3510 architecture diagram<a name="fig1097417534526"></a>  
![](../../figures/hardware_architecture.png "Hardware architecture")

Specifically, the major changes in the 3510 architecture are listed in the following tables. In addition, the 3510 architecture extends the supported data types. For details, see the [Data Type Introduction](../../../api/SIMD-API/basic_api/data_structures/built_in_data_type.md).

- Data movement unit

    **Table 1** Data movement unit changes
    | 3510 Changes | Impact | Affected API |
    |----------|------------|---------------|
    | The data path from L1 Buffer to GM is removed. | The existing APIs do not support directly moving data from L1 Buffer to GM. Developers need to allocate a space in L1 Buffer to store the identity matrix, use MMAD matrix multiplication to compute and output to L0C Buffer, and then move the data from L0C Buffer to GM through [Fixpipe](../../../api/SIMD-API/basic_api/cube_compute_ISASI/cube_compute_store/Fixpipe_L0CToGM.md). | DataCopy/DumpTensor |
    | The data paths from GM to L0A Buffer and L0B Buffer are removed. | The original data movement from GM to L0A Buffer and L0B Buffer needs to be split into two steps: data movement from GM to L1 Buffer and data movement from L1 Buffer to L0A Buffer and L0B Buffer. | LoadData |
    | The data path from UB to L1 Buffer is added. | Data can be moved directly from UB to L1 Buffer without first moving from UB to GM and then from GM to L1 Buffer. For details about the usage, refer to [Basic data movement](../../../api/SIMD-API/basic_api/cube_compute_ISASI/cube_compute_load/DataCopy_UBToL1_continuous.md). | DataCopy |
    | The ND-DMA instruction is added. | The capability of the DataCopy data movement API is extended. Compared with the basic data movement API, the dimension information and Stride of the input data can be configured more flexibly. For details about the usage, refer to [Multi-dimensional data movement (ISASI)](../../../api/SIMD-API/basic_api/memory_vector_compute/data_move/DataCopy_GMToUB_NDDMA.md). | DataCopy |
    | The unidirectional data path from L0C Buffer to UB is added. | Data can be moved directly from L0C Buffer to UB without first moving from L0C Buffer to GM and then from GM to UB. For details about the usage, refer to [Fixpipe](../../../api/SIMD-API/basic_api/cube_compute_ISASI/cube_compute_store/Fixpipe_L0CToGM.md). | Fixpipe |
    | The LoadData movement instruction is extended. | Data movement in MicroScaling (MX) scenarios is added. For details about the usage, refer to [LoadData](../../../api/SIMD-API/basic_api/cube_compute_ISASI/cube_compute_load/Load2DMX.md). | LoadData |
    | The DN type is added, and transpose is no longer supported for L1Buffer->L0A Buffer. | For details about using the new feature, refer to [LoadDataWithTranspose](../../../api/SIMD-API/basic_api/cube_compute_ISASI/cube_compute_load/LoadDataWithTranspose.md). | LoadDataWithTranspose |
    | Fixpipe adds NZ2DN in-path conversion (implementing in-path conversion from the NZ data format to the DN data format). | For details about the usage, refer to [Fixpipe](../../../api/SIMD-API/basic_api/cube_compute_ISASI/cube_compute_store/Fixpipe_L0CToGM.md). | Fixpipe |
    | The DataCopy movement dimension is enhanced. | DataCopy supports loop-mode movement on the paths between L1 Buffer and GM and between GM and UB. For details about the usage, refer to [SetLoopModePara](../../../api/SIMD-API/basic_api/memory_vector_compute/data_move_aux_config/SetLoopModePara.md). | DataCopy |
    | In the 3510 architecture version, the hardware instructions related to L0A Buffer and L0B Buffer initialization are removed. | Use the basic API [Fill](../../../api/SIMD-API/basic_api/cube_compute_ISASI/cube_load_aux_config/Fill.md) to initialize the LocalTensor at a specific storage location to a specific value. Direct initialization of LocalTensor on L0A Buffer and L0B Buffer is not supported. | Fill |

- Compute unit

    **Table 2** Compute unit changes
    | 3510 Changes | Impact | Affected API |
    |----------|------------|---------------|
    | The Cube computation unit does not support the s4 type. | For matrix multiplication of the int4b_t data type, developers need to first Cast the int4b_t data to the int8_t type and then perform Cube computation. | Mmad |
    | The Cube computation unit does not support the ZZ-to-ZN fractal change on L0A. | In the L0A splitting scenario, the L0A address of the left matrix needs to be recalculated for matrix multiplication. | LoadData/LoadDataWithTranspose |
    | The Vector Core Membase architecture is switched to the Regbase architecture. | The performance of some basic API scenarios decreases. | Basic API high-dimensional splitting mode |
    | The hardware does not support the Subnormal function, which is currently implemented through software simulation. | Developers need to configure the Subnormal computation mode by setting the config template parameters. For details, refer to [Vector computation](./2201_to_3510_guide/basic_api_migration_guide.md#section7364115741514). | Ln/Sqrt/Rsqrt/Div/Reciprocal/Exp |
    | Computation of 4:2 sparse matrices is not supported. | Developers need to use the Vector Core capability to perform dense-to-sparse matrix conversion. | LoadDataWithSparse/MmadWithSparse |

- Storage unit

    **Table 3** Storage unit changes

    | 3510 Changes | Impact | Affected API |
    |----------|------------|---------------|
    | The boundary value setting of the L1 Buffer space is removed. | In the 3510 architecture, the hardware removes the registers related to the boundary value setting of L1 Buffer, and the SetLoadDataBoundary API is no longer supported. For details, refer to [Basic API migration guide](./2201_to_3510_guide/basic_api_migration_guide.md). | SetLoadDataBoundary |
    | The UB structure changes. For a comparison between the UB structure of the 2201 architecture and that of the 3510 architecture, refer to [Bank structure comparison](../../operator_practice/simd_operator_optimization/memory_access/avoid_ub_bank_conflict/overview.md). | In the 2201 architecture, UB is divided into 16 bank groups, each containing 3 banks, and each bank is 4 KB. In the 3510 architecture, UB is divided into 8 bank groups, each containing 2 banks, and each bank is 16 KB. If a UB conflict occurs, developers can refer to [Avoiding UB bank conflicts](../../operator_practice/simd_operator_optimization/memory_access/avoid_ub_bank_conflict/overview.md) to resolve the UB conflict. | / |


- Synchronization

    **Table 4** Synchronization changes

    | 3510 Changes | Impact | Affected API |
    |----------|------------|---------------|
    | Added the Mutex capability. | Mutex is used for synchronization between intra-core asynchronous pipeline instructions. Its function is similar to the lock mechanism in a traditional CPU. It locks a specified pipeline and then releases it to complete the synchronization dependency between pipelines. For details about the usage, refer to [Mutex (ISASI)](../../../api/SIMD-API/basic_api/sync_control/intra_core_sync/Mutex_ISASI.md). | Mutex |
    | Added the inter-core synchronization control mode. | For synchronization control inside the AI Core, AIV0 and AIV1 can independently trigger AIC waiting. For details about the usage, refer to [CrossCoreSetFlag(ISASI)](../../../api/SIMD-API/basic_api/sync_control/inter_core_sync/CrossCoreSetFlag_ISASI.md). | CrossCoreSetFlag/CrossCoreWaitFlag |

- Others

    **Table 5** Other changes

    | 3510 Changes | Impact | Affected API |
    |----------|------------|---------------|
    | Removed the AIPP hardware-level instructions and implemented the AIPP function through software emulation. | The performance of AIPP interfaces may degrade. | SetAippFunctions/LoadImageToLocal |
    | Because the related registers were removed in the 3510 architecture version, the UB exception debugging interface was also removed. | The debugging interface has no impact on functionality. | CheckLocalMemoryIA |
    | Because the related registers were removed in the 3510 architecture version, the capability of obtaining the maximum/minimum values and indexes of all ReduceRepeat repeats was also removed.| The RegBase basic capability is now available. You can manually implement this capability through [Reduce](../../../api/SIMD-API/basic_api/reg_vector_compute/reduction_compute/Reduce.md).| GetReduceRepeatMaxMinSpr |
