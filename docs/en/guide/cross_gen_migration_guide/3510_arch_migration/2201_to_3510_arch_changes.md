# Architecture Changes from 2201 to 3510<a name="ZH-CN_TOPIC_0000002503428217"></a>

<!-- md-trans-meta sourceCommit=ca3b72f6b4f376702407e982cf2cc174087341f5 translatedAt=2026-10-23T10:52:46.949Z pushedAt=2026-11-03T07:05:13.249Z nl=2 -->

The architecture diagram of [NPU architecture version 3510](../../programming_guide/language_extension/simd_builtin_keywords.md#npu-arch) is shown in [Figure 1](#fig1097417534526). Overall, the 3510 architecture adds the following features:

-   Multiple data paths are added.
-   The number of AI Cores is increased.
-   The Unified Buffer (UB) capacity is increased.
-   The SSBuffer on-core storage unit is added, which supports access by AIC cores and AIV cores through scalar.
-   On the basis of SIMD programming, SIMT programming and mixed SIMD and SIMT programming are supported.
-   AIV cores adopt the Regbase architecture. Compared with the Membase architecture of [NPU architecture version 2201](../../programming_guide/language_extension/simd_builtin_keywords.md#npu-arch), the Regbase architecture allows direct operations on the vector registers of the chip, achieving greater flexibility and better performance.

**Figure 1**  3510 architecture diagram<a name="fig1097417534526"></a>  
![](../../figures/npu_3510_hw_arch.png "Hardware architecture")

Specifically, the major changes in the 3510 architecture are listed in the following tables. In addition, the 3510 architecture extends the supported data types. For details, see [Data Type Introduction](../../../api/SIMD-API/basic_api/data_structures/builtin_data_types.md).

- Data movement unit<a name="section_2201_to_3510_data_move_unit_changes"></a>

    **Table 1**  Data movement unit changes

    | 3510 Changes | Impact | Affected APIs |
    |----------|------------|---------------|
    | The data path from L1 Buffer to GM is deleted. | Existing APIs do not support moving data directly from L1 Buffer to GM. Developers need to allocate a space in L1 Buffer to store the identity cube, use MMAD cube multiplication to compute the output to L0C Buffer, and then move the data from L0C Buffer to GM through [Fixpipe (L0C to GM data movement)](../../../api/SIMD-API/basic_api/cube_compute_ISASI/cube_compute_store/Fixpipe_L0CToGM.md). | DataCopy/DumpTensor |
    | The data paths from GM to L0A Buffer and L0B Buffer are deleted. | The original data movement from GM to L0A Buffer and L0B Buffer needs to be split into two steps: data movement from GM to L1 Buffer, and data movement from L1 Buffer to L0A Buffer and L0B Buffer. | LoadData |
    | The data path from UB to L1 Buffer is added. | Data can be moved directly from UB to L1 Buffer without first moving from UB to GM and then from GM to L1 Buffer. For usage, see [DataCopy (UBToL1 continuous data movement)](../../../api/SIMD-API/basic_api/cube_compute_ISASI/cube_compute_load/DataCopy_UBToL1_continuous.md). | DataCopy |
    | The ND-DMA instruction is added. | The capability of the DataCopy data movement API is extended. Compared with the base data movement API, the dimension information and stride of the data to be moved in can be configured more flexibly. For usage, see [DataCopy (GMToUB multi-dimensional data movement NDDMA)](../../../api/SIMD-API/basic_api/memory_vector_compute/data_move/DataCopy_GMToUB_NDDMA.md). | DataCopy |
    | The unidirectional data path from L0C Buffer to UB is added. | Data can be moved directly from L0C Buffer to UB without first moving from L0C Buffer to GM and then from GM to UB. For usage, see [Fixpipe (L0C to GM data movement)](../../../api/SIMD-API/basic_api/cube_compute_ISASI/cube_compute_store/Fixpipe_L0CToGM.md). | Fixpipe |
    | The LoadData movement instruction is extended. | Data movement in MicroScaling (MX) scenarios is added. For usage, see [LoadData](../../../api/SIMD-API/basic_api/cube_compute_ISASI/cube_compute_load/LoadData_2D_MX.md). | LoadData |
    | The DN fractal is added, and transpose is no longer supported for L1Buffer to L0A Buffer. | For the new feature, see [LoadDataWithTranspose](../../../api/SIMD-API/basic_api/cube_compute_ISASI/cube_compute_load/LoadDataWithTranspose.md). | LoadDataWithTranspose |
    | Fixpipe adds NZ2DN in-path conversion (implementing in-path conversion from NZ to DN data format). | For usage, see [Fixpipe (L0C to GM data movement)](../../../api/SIMD-API/basic_api/cube_compute_ISASI/cube_compute_store/Fixpipe_L0CToGM.md). | Fixpipe |
    | The DataCopy movement dimension is enhanced. | DataCopy supports loop-mode movement on the paths between L1 Buffer and GM, and between GM and UB. For usage, see [SetLoopModePara](../../../api/SIMD-API/basic_api/memory_vector_compute/data_move_aux_config/SetLoopModePara.md). | DataCopy |
    | In the 3510 architecture version, the hardware instructions related to L0A Buffer and L0B Buffer initialization are deleted. | Use the base API [Fill](../../../api/SIMD-API/basic_api/cube_compute_ISASI/cube_load_aux_config/Fill.md) to initialize the LocalTensor at a specific storage location to a specific value. Direct initialization of LocalTensor on L0A Buffer and L0B Buffer is not supported. | Fill |

- Compute unit<a name="section_2201_to_3510_compute_unit_changes"></a>

    **Table 2**  Compute unit changes

    | 3510 Changes | Impact | Affected APIs |
    |----------|------------|---------------|
    | The Cube compute unit does not support the s4 type. | For cube multiplication of the int4b_t data type, developers need to first Cast the int4b_t data to the int8_t type, and then perform Cube computation. | Mmad |
    | The Cube compute unit does not support the ZZ-to-ZN fractal change on L0A. | In the L0A splitting scenario, the L0A address of the left cube needs to be recalculated for cube multiplication. | LoadData/LoadDataWithTranspose |
    | The Vector Core Membase architecture is switched to the Regbase architecture. | The performance of some base API scenarios decreases. | Base API high-dimensional splitting mode |
    | The hardware does not support the Subnormal function. The base API and some advanced APIs implement the Subnormal function through software emulation. | For base APIs that support the config parameter, developers can configure the Subnormal computation mode by setting the config template parameter. For some base APIs and advanced APIs that do not pass the config parameter when called, the processing is affected by the compilation option `--cce-ftz`, which defaults to `true`. For details, see [Vector compute](./2201_to_3510_guide/basic_api_migration.md#vector-compute). | Ln/Sqrt/Rsqrt/Div/Reciprocal/Exp and related advanced APIs |
    | Computation of 4:2 sparse cubes is not supported. | Developers need to use the Vector Core capability to perform dense-to-sparse conversion on the cube. | LoadDataWithSparse/MmadWithSparse |

- Storage unit<a name="section_2201_to_3510_storage_unit_changes"></a>

    **Table 3**  Storage unit changes

    | 3510 Changes | Impact | Affected APIs |
    |----------|------------|---------------|
    | The boundary value setting of the L1 Buffer space is deleted. | In the 3510 architecture, the hardware deletes the registers related to the L1 Buffer boundary value setting, and the SetLoadDataBoundary API is no longer supported. For details, see [Basic API migration guide](./2201_to_3510_guide/basic_api_migration.md). | SetLoadDataBoundary |
    | The UB structure changes. For a comparison between the UB structure of the 2201 architecture and that of the 3510 architecture, see [bank structure comparison](../../operator_practice/simd_operator_optimization/memory_access/avoid_ub_bank_conflict/overview.md). | In the 2201 architecture, UB is divided into 16 bank groups, each bank group contains 3 banks, and each bank is 4KB in size. In the 3510 architecture, UB is divided into 8 bank groups, each bank group contains 2 banks, and each bank is 16KB in size. If a UB conflict occurs, developers can refer to [avoiding UB bank conflicts](../../operator_practice/simd_operator_optimization/memory_access/avoid_ub_bank_conflict/overview.md) to resolve the UB conflict. | / |


- Synchronization

    **Table 4**  Synchronization changes

    | 3510 Changes | Impact | Affected APIs |
    |----------|------------|---------------|
    | Added Mutex capability. | Mutex is used for synchronization between intra-core asynchronous pipeline instructions. Its function is similar to the lock mechanism in traditional CPUs. Synchronization dependencies between pipelines are completed by locking a specified pipeline and then releasing it. For details about the usage, see [Mutex (ISASI)](../../../api/SIMD-API/basic_api/sync_control/intra_core_sync/Mutex_ISASI.md). | Mutex |
    | Added inter-core synchronization control mode. | For synchronization control inside the AI Core, AIV0 and AIV1 can independently trigger AIC waiting. For details about the usage, see [CrossCoreSetFlag(ISASI)](../../../api/SIMD-API/basic_api/sync_control/inter_core_sync/CrossCoreSetFlag_ISASI.md). | CrossCoreSetFlag/CrossCoreWaitFlag |

- Others

    **Table 5**  Other changes

    | 3510 Changes | Impact | Affected APIs |
    |----------|------------|---------------|
    | Deleted AIPP hardware-level instructions and implemented the AIPP function through software emulation. | The performance of AIPP APIs may degrade. | SetAippFunctions/LoadImageToLocal |
    | Because related registers are deleted in the 3510 architecture version, the UB exception debugging API is also removed. | The debugging API has no impact on functions. | CheckLocalMemoryIA |
    | Because related registers are deleted in the 3510 architecture version, the capability of obtaining the maximum/minimum values and indexes of all ReduceRepeat repeats is also removed.| The RegBase base capability is now available, and it can be manually implemented through [Reduce](../../../api/SIMD-API/basic_api/reg_vector_compute/reduction_compute/Reduce.md).| GetReduceRepeatMaxMinSpr |
