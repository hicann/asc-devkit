# 3510新特性使用说明

本文介绍[NPU架构版本3510](../../programming_guide/language_extension/simd_builtin_keywords.md#npu-arch)新增特性，为开发者将算子程序从其他架构迁移至3510架构提供参考。有关3510架构的整体变更，请参见[2201到3510架构变更](../3510_arch_migration/2201_to_3510_arch_changes.md)。

## 特性总览

**表 1**  3510新特性总览<a name="table_3510_new_feature_overview"></a>

| 分类 | 特性 | 相关基础API | 相关C API |
|------|------|------------|-----------|
| 数据搬运 | [新增UB到L1 Buffer搬运数据通路](#section_ub2l1) | [DataCopy（UB到L1 Buffer）](../../../api/SIMD-API/basic_api/cube_compute_ISASI/cube_compute_load/DataCopy_UBToL1_continuous.md)、[DataCopyPad（UB到L1 Buffer）](../../../api/SIMD-API/basic_api/cube_compute_ISASI/cube_compute_load/DataCopyPad_UBToL1.md) | [asc_copy_ub2l1](../../../api/SIMD-API/c_api/vector_datamove/asc_copy_ub2l1.md) |
| 数据搬运 | [新增L1 Buffer到UB搬运数据通路](#section_l12ub) | [DataCopyL1ToUB](../../../api/SIMD-API/basic_api/cube_compute_ISASI/cube_compute_store/DataCopyL1ToUB.md) | [asc_copy_l12ub](../../../api/SIMD-API/c_api/cube_datamove/asc_copy_l12ub.md) |
| 数据搬运 | [新增L0C Buffer到UB搬运数据通路](#section_fixpipe_l0c2ub) | [Fixpipe（L0C Buffer到UB）](../../../api/SIMD-API/basic_api/cube_compute_ISASI/cube_compute_store/Fixpipe_L0CToUB.md) | [asc_copy_l0c2ub](../../../api/SIMD-API/c_api/cube_datamove/asc_copy_l0c2ub.md) |
| 数据搬运 | [新增ND-DMA多维数据搬运](#section_nddma) | [DataCopy（GM到UB多维数据搬运）](../../../api/SIMD-API/basic_api/memory_vector_compute/data_move/DataCopy_GMToUB_NDDMA.md)、[NdDmaDci](../../../api/SIMD-API/basic_api/memory_vector_compute/data_move_aux_config/NdDmaDci.md) | [asc_ndim_copy_gm2ub](../../../api/SIMD-API/c_api/vector_datamove/asc_ndim_copy_gm2ub.md) |
| 数据搬运 | [对齐/非对齐数据搬运新增loop模式](#section_loop_mode) | [SetLoopModePara](../../../api/SIMD-API/basic_api/memory_vector_compute/data_move_aux_config/SetLoopModePara.md)、[ResetLoopModePara](../../../api/SIMD-API/basic_api/memory_vector_compute/data_move_aux_config/ResetLoopModePara.md) | [asc_set_gm2l1_loop_size等](../../../api/SIMD-API/c_api/cube_datamove/asc_set_gm2l1_loop_size.md)、[asc_set_ub2gm_loop1_stride等](../../../api/SIMD-API/c_api/vector_datamove/asc_set_ub2gm_loop1_stride.md) |
| 矩阵计算 | [新增MX矩阵搬运与计算](#section_mx) | [LoadData（2D MX搬运）](../../../api/SIMD-API/basic_api/cube_compute_ISASI/cube_compute_load/LoadData_2D_MX.md)、[MmadMx](../../../api/SIMD-API/basic_api/cube_compute_ISASI/mmad_compute/MmadMx.md) | [asc_copy_l12l0a_mx](../../../api/SIMD-API/c_api/cube_datamove/asc_copy_l12l0a_mx.md)、[asc_copy_l12l0b_mx](../../../api/SIMD-API/c_api/cube_datamove/asc_copy_l12l0b_mx.md)、[asc_mmad_mx](../../../api/SIMD-API/c_api/cube_compute/asc_mmad_mx.md) |
| 矩阵计算 | [2D矩阵搬运（随路转置）能力增强](#section_transpose) | [LoadDataWithTranspose](../../../api/SIMD-API/basic_api/cube_compute_ISASI/cube_compute_load/LoadDataWithTranspose.md) | [asc_copy_l12l0b_trans（arch3510）](../../../api/SIMD-API/c_api/cube_datamove/asc_copy_l12l0b_trans/asc_copy_l12l0b_trans_arch_3510.md) |
| 矩阵计算 | [Fixpipe新增NZ2DN随路转换](#section_nz2dn) | [Fixpipe（L0C Buffer到GM、L1 Buffer、UB）](../../../api/SIMD-API/basic_api/cube_compute_ISASI/cube_compute_store/Fixpipe_L0CToGM.md) | [asc_copy_l0c2gm（arch3510）](../../../api/SIMD-API/c_api/cube_datamove/asc_copy_l0c2gm/asc_copy_l0c2gm_arch_3510.md)、[asc_copy_l0c2l1（arch3510）](../../../api/SIMD-API/c_api/cube_datamove/asc_copy_l0c2l1/asc_copy_l0c2l1_arch_3510.md)、[asc_copy_l0c2ub](../../../api/SIMD-API/c_api/cube_datamove/asc_copy_l0c2ub.md) |
| 矩阵计算 | [2D矩阵搬运能力增强](#section_loaddata_2d_v2) | [LoadData（2D矩阵搬运V2）](../../../api/SIMD-API/basic_api/cube_compute_ISASI/cube_compute_load/LoadData_2D_V2.md)、[LoadData（GM到L1 Buffer 2D V2）](../../../api/SIMD-API/basic_api/cube_compute_ISASI/cube_compute_load/LoadData_GMToL1_2DV2.md) | [asc_copy_l12l0a（2D，arch3510）](../../../api/SIMD-API/c_api/cube_datamove/asc_copy_l12l0a/asc_copy_l12l0a_2d_arch_3510.md)、[asc_copy_l12l0b（2D，arch3510）](../../../api/SIMD-API/c_api/cube_datamove/asc_copy_l12l0b/asc_copy_l12l0b_2d_arch_3510.md)、[asc_copy_gm2l1（arch3510）](../../../api/SIMD-API/c_api/cube_datamove/asc_copy_gm2l1/asc_copy_gm2l1_2d_arch_3510.md) |
| 矩阵计算 | [新增Stride卷积矩阵搬运](#section_loaddata_with_stride) | [LoadDataWithStride](../../../api/SIMD-API/basic_api/cube_compute_ISASI/cube_compute_load/LoadDataWithStride.md) | [asc_copy_l12l0a（3D，arch3510）](../../../api/SIMD-API/c_api/cube_datamove/asc_copy_l12l0a/asc_copy_l12l0a_3d_arch_3510.md)、[asc_copy_l12l0b（3D，arch3510）](../../../api/SIMD-API/c_api/cube_datamove/asc_copy_l12l0b/asc_copy_l12l0b_3d_arch_3510.md)、[asc_set_l13d_rpt](../../../api/SIMD-API/c_api/cube_datamove/asc_set_l13d_rpt.md)、[asc_set_l3d_rpt_b](../../../api/SIMD-API/c_api/cube_datamove/asc_set_l3d_rpt_b.md) |
| 矩阵计算 | [矩阵计算支持更多数据类型](#section_mmad_data_types) | [Mmad](../../../api/SIMD-API/basic_api/cube_compute_ISASI/mmad_compute/Mmad.md)、[MmadMx](../../../api/SIMD-API/basic_api/cube_compute_ISASI/mmad_compute/MmadMx.md) | [asc_mmad](../../../api/SIMD-API/c_api/cube_compute/asc_mmad.md)、[asc_mmad_mx](../../../api/SIMD-API/c_api/cube_compute/asc_mmad_mx.md) |
| 矩阵计算 | [L0A Buffer分形排布变为NZ](#section_l0a_nz) | - | - |
| 矢量计算 | [矢量计算基础API扩展](#section_vec_api_ext) | [相关基础API](#section_vec_api_ext) | - |
| 矢量计算 | [新增Regbase矢量计算方式](#section_regbase) | [Reg矢量计算基础API](../../../api/SIMD-API/basic_api/reg_vector_compute/overview.md) | [Reg矢量计算C API](../../../api/SIMD-API/c_api/reg_compute/reg_compute.md) |
| 原子操作 | [原子操作接口扩展](#section_atomic_api) | [AtomicAdd等](../../../api/SIMD-API/basic_api/atomic_operations/AtomicAdd.md) | [asc_atomic_add](../../../api/SIMD-API/c_api/atomic/scalar_atomic/asc_atomic_add.md)、[asc_atomic_max](../../../api/SIMD-API/c_api/atomic/scalar_atomic/asc_atomic_max.md)、[asc_atomic_min](../../../api/SIMD-API/c_api/atomic/scalar_atomic/asc_atomic_min.md)、[asc_atomic_exch](../../../api/SIMD-API/c_api/atomic/scalar_atomic/asc_atomic_exch.md)、[asc_atomic_cas](../../../api/SIMD-API/c_api/atomic/scalar_atomic/asc_atomic_cas.md) |
| 缓存 | [新增DCI（数据缓存失效）能力](#section_dci) | – | [asc_dci](../../../api/SIMD-API/c_api/cache_ctrl/asc_dci.md) |
| 存储 | [新增SSBuffer核内存储单元](#section_ssbuf) | [GetSsbufBaseAddr](../../../api/SIMD-API/basic_api/tool_interface/system_resources_and_variables/GetSsbufBaseAddr.md) | – |
| 同步 | [新增Mutex核内同步能力](#section_mutex) | [Mutex::Lock/Unlock](../../../api/SIMD-API/basic_api/sync_control/intra_core_sync/Mutex_ISASI.md)、[AllocMutexID](../../../api/SIMD-API/basic_api/sync_control/intra_core_sync/AllocMutexID_ISASI.md) | [asc_lock](../../../api/SIMD-API/c_api/sync/asc_lock.md)、[asc_unlock](../../../api/SIMD-API/c_api/sync/asc_unlock.md) |
| 同步 | [新增核间同步控制模式4](#section_crosscore) | [CrossCoreSetFlag](../../../api/SIMD-API/basic_api/sync_control/inter_core_sync/CrossCoreSetFlag_ISASI.md)、[CrossCoreWaitFlag](../../../api/SIMD-API/basic_api/sync_control/inter_core_sync/CrossCoreWaitFlag_ISASI.md) | [asc_sync_intra_arrive](../../../api/SIMD-API/c_api/sync/asc_sync_intra_arrive.md)、[asc_sync_intra_wait](../../../api/SIMD-API/c_api/sync/asc_sync_intra_wait.md) |

> [!NOTE]
> 除上表中所列特性以外，3510架构还新增SIMT编程模型和SIMD与SIMT混合编程，具体说明请参见：
> - SIMT编程：[AI Core SIMT编程](../../programming_guide/programming_model/ai_core_simt_programming/ai_core_simt_programming.md)、[SIMT API](../../../api/SIMT-API/overview.md)。
> - SIMD与SIMT混编：[SIMD与SIMT混合编程](../../programming_guide/advanced_programming/advanced_ai_core_programming_model/simd_simt_hybrid_programming/overview.md)。

## 特性说明

### 新增UB到L1 Buffer搬运数据通路<a name="section_ub2l1"></a>

#### 特性应用场景说明

CV融合算子中，UB向L1 Buffer搬运数据不再需要通过GM中转。

#### 功能说明

- 对于C API，使用[asc_copy_ub2l1](../../../api/SIMD-API/c_api/vector_datamove/asc_copy_ub2l1.md)接口实现UB到L1 Buffer数据搬运，无需配置编译选项。
- 对于基础API，使用[DataCopy（UB到L1 Buffer）](../../../api/SIMD-API/basic_api/cube_compute_ISASI/cube_compute_load/DataCopy_UBToL1_continuous.md)、[DataCopyPad（UB到L1 Buffer）](../../../api/SIMD-API/basic_api/cube_compute_ISASI/cube_compute_load/DataCopyPad_UBToL1.md)进行搬运，开启该特性需要配置编译选项[`ENABLE_CV_COMM_VIA_SSBUF`](../../programming_guide/compilation_and_execution/operator_compilation/ai_core_operator_compilation.md#内置编译宏开关)：
  - 开启该编译选项后，数据通过UB到L1 Buffer之间的硬件通道直接搬运。该方式无需经过GM中转，性能较高。
  - 未开启该编译选项时，数据需经由GM搬运至L1 Buffer。在此场景下，UB到L1 Buffer搬运接口采用软件仿真实现。

#### 典型样例说明<a name="section_ub2l1_sample"></a>

- [data_copy_ub2l1样例](../../../../../examples/01_simd_cpp_api/03_basic_api/00_data_movement/data_copy_ub2l1)：在Mmad矩阵乘场景下，基于DataCopy实现UB到L1 Buffer数据搬运，通过编译选项`SCENARIO_NUM`切换连续搬运和随路ND2NZ搬运两种场景，取值1表示连续搬运，取值2表示随路ND2NZ搬运。样例完整数据流为GM到UB到L1 Buffer到L0A Buffer、L0B Buffer到Mmad到L0C Buffer到GM。样例编译工程已通过`ascendc_compile_definitions`配置`-DENABLE_CV_COMM_VIA_SSBUF=true`。

### 新增L1 Buffer到UB搬运数据通路<a name="section_l12ub"></a>

#### 特性应用场景说明

CV融合算子场景，L1 Buffer向UB搬运数据。3510架构新增L1 Buffer到UB搬运数据通路。

#### 功能说明

可参考以下资料：
- 基础API：[DataCopyL1ToUB](../../../api/SIMD-API/basic_api/cube_compute_ISASI/cube_compute_store/DataCopyL1ToUB.md)。
- C API：[asc_copy_l12ub](../../../api/SIMD-API/c_api/cube_datamove/asc_copy_l12ub.md)。

### 新增L0C Buffer到UB搬运数据通路<a name="section_fixpipe_l0c2ub"></a>

#### 特性应用场景说明

CV融合算子场景，矩阵计算后的结果若需要在Vector核侧进行归一化、激活等向量计算，需要先将L0C Buffer中的数据搬运到UB。3510架构新增L0C Buffer到UB单向数据通路。

#### 功能说明

可参考以下资料：
- 基础API：[Fixpipe（L0C Buffer到UB数据搬运）](../../../api/SIMD-API/basic_api/cube_compute_ISASI/cube_compute_store/Fixpipe_L0CToUB.md)。使用带有`FixpipeParamsArch3510`参数的Fixpipe接口实现L0C Buffer到UB搬出。
- C API：[asc_copy_l0c2ub](../../../api/SIMD-API/c_api/cube_datamove/asc_copy_l0c2ub.md)。


#### 典型样例说明

[fixpipe_l0c2ub样例](../../../../../examples/01_simd_cpp_api/03_basic_api/03_matrix_compute/fixpipe_l0c2ub)：基于Fixpipe实现L0C Buffer到UB数据搬运，完整数据流为GM到L1 Buffer到L0A Buffer、L0B Buffer到Mmad到L0C Buffer到UB。

### 新增ND-DMA多维数据搬运<a name="section_nddma"></a>

#### 特性应用场景说明

Vector算子场景，从GM搬运至UB时，若涉及多维矩阵、不规则Stride，以及Padding、Transpose、Broadcast或Slice等复杂数据变换时，需要多次调用基础搬运接口。3510架构新增ND-DMA硬件指令。与基础数据搬运接口相比，该指令支持灵活配置搬入数据的维度信息和Stride，可通过一条指令完成上述复杂场景的数据搬运。

#### 功能说明

可参考以下资料：
- 基础API：[DataCopy（GMToUB多维数据搬运NDDMA）](../../../api/SIMD-API/basic_api/memory_vector_compute/data_move/DataCopy_GMToUB_NDDMA.md)、[NdDmaDci](../../../api/SIMD-API/basic_api/memory_vector_compute/data_move_aux_config/NdDmaDci.md)。
- C API：[asc_ndim_copy_gm2ub](../../../api/SIMD-API/c_api/vector_datamove/asc_ndim_copy_gm2ub.md)。

#### 典型样例说明

[data_copy_gm2ub_nddma样例](../../../../../examples/01_simd_cpp_api/03_basic_api/00_data_movement/data_copy_gm2ub_nddma)：基于ND-DMA指令实现GM到UB多维数据搬运，模拟多维Shape下移动窗口的数据搬运，包含Slice、输入Stride和不连续源数据搬运的配置方式。

### 对齐/非对齐数据搬运新增loop模式<a name="section_loop_mode"></a>

#### 特性应用场景说明

Vector算子场景，当需要将GM中的多段数据交替搬运至UB的不同地址，或沿相反方向搬运时，通常需要循环执行多条DataCopy指令，导致指令数量和流水开销增加。3510架构新增loop模式数据搬运能力，调用一次接口即可完成搬运，从而减少指令下发开销并提升搬运效率。

#### 功能说明

可参考以下资料：
- 基础API：[SetLoopModePara](../../../api/SIMD-API/basic_api/memory_vector_compute/data_move_aux_config/SetLoopModePara.md)、[ResetLoopModePara](../../../api/SIMD-API/basic_api/memory_vector_compute/data_move_aux_config/ResetLoopModePara.md)。
- C API：[asc_set_gm2l1_loop_size](../../../api/SIMD-API/c_api/cube_datamove/asc_set_gm2l1_loop_size.md)、[asc_set_ub2gm_loop1_stride](../../../api/SIMD-API/c_api/vector_datamove/asc_set_ub2gm_loop1_stride.md)等。


### 新增MX矩阵搬运与计算<a name="section_mx"></a>

#### 特性应用场景说明

MicroScaling（MX）是一种低比特量化数据格式，例如fp4x2_e2m1_t、fp4x2_e1m2_t、fp8_e5m2_t和fp8_e4m3fn_t。矩阵数据与各数据块对应的量化系数矩阵共同存储，因此矩阵计算时需同步搬运矩阵数据和量化系数矩阵。3510架构新增适用于MicroScaling场景的数据搬运能力，可同步搬运矩阵数据与量化系数矩阵，为MX低比特格式的矩阵计算提供支持。

#### 功能说明

可参考以下资料：
- 基础API：[LoadData（2D MX搬运）](../../../api/SIMD-API/basic_api/cube_compute_ISASI/cube_compute_load/LoadData_2D_MX.md)、[MmadMx](../../../api/SIMD-API/basic_api/cube_compute_ISASI/mmad_compute/MmadMx.md)。
- C API：[asc_copy_l12l0a_mx](../../../api/SIMD-API/c_api/cube_datamove/asc_copy_l12l0a_mx.md)、[asc_copy_l12l0b_mx](../../../api/SIMD-API/c_api/cube_datamove/asc_copy_l12l0b_mx.md)、[asc_mmad_mx](../../../api/SIMD-API/c_api/cube_compute/asc_mmad_mx.md)。

#### 典型样例说明

- [load_data_2dmx_l12l0样例](../../../../../examples/01_simd_cpp_api/03_basic_api/03_matrix_compute/load_data_2dmx_l12l0)：演示MX矩阵数据及其量化系数矩阵的同步搬运。
- [mmad_mx样例](../../../../../examples/01_simd_cpp_api/03_basic_api/03_matrix_compute/mmad_mx)：演示MmadMx矩阵乘加计算。

### 2D矩阵搬运（随路转置）能力增强<a name="section_transpose"></a>

#### 特性应用场景说明

在矩阵乘计算中，L1 Buffer中的右矩阵B需要以转置后的分形排布搬入L0B Buffer。对于源矩阵中分形非连续排布的场景，2201架构的搬运参数以拼接后的方块矩阵为配置单位，难以直接描述分形级间隔。3510架构增强随路转置搬运能力，可按512字节分形配置搬运起始位置、相邻迭代源Stride及迭代内源分形间隔，适用于分形拼接、非连续搬入等场景。

#### 功能说明

可参考以下资料：
- 基础API：[LoadDataWithTranspose](../../../api/SIMD-API/basic_api/cube_compute_ISASI/cube_compute_load/LoadDataWithTranspose.md)。3510架构使用`LoadData2dTransposeParamsV2`作为参数类型，支持以512字节分形为单位配置源Stride，并可通过`srcFracGap`配置每个迭代内的源分形间隔。
- C API：[asc_copy_l12l0b_trans（arch3510）](../../../api/SIMD-API/c_api/cube_datamove/asc_copy_l12l0b_trans/asc_copy_l12l0b_trans_arch_3510.md)。

### Fixpipe新增NZ2DN随路转换<a name="section_nz2dn"></a>

#### 特性应用场景说明

L0C Buffer中Mmad计算结果为NZ分形格式，部分算子输出需要列主序的DN格式。3510架构的Fixpipe接口新增NZ2DN随路转换能力，可在L0C Buffer到GM、L1 Buffer、UB搬出过程中完成格式转换。

#### 功能说明

可参考以下资料：
- 基础API：[Fixpipe](../../../api/SIMD-API/basic_api/cube_compute_ISASI/cube_compute_store/Fixpipe_L0CToGM.md)。
- C API：[asc_copy_l0c2gm（arch3510）](../../../api/SIMD-API/c_api/cube_datamove/asc_copy_l0c2gm/asc_copy_l0c2gm_arch_3510.md)、[asc_copy_l0c2l1（arch3510）](../../../api/SIMD-API/c_api/cube_datamove/asc_copy_l0c2l1/asc_copy_l0c2l1_arch_3510.md)、[asc_copy_l0c2ub](../../../api/SIMD-API/c_api/cube_datamove/asc_copy_l0c2ub.md)。

### 2D矩阵搬运能力增强<a name="section_loaddata_2d_v2"></a>

#### 特性应用场景说明

3510架构2D矩阵搬运能力增强，搬运方式更加灵活，并且支持全量数据类型的搬运。

#### 功能说明

可参考以下资料：
- 基础API：[LoadData（2D矩阵搬运V2）](../../../api/SIMD-API/basic_api/cube_compute_ISASI/cube_compute_load/LoadData_2D_V2.md)、[LoadData（GM到L1 Buffer的2D矩阵搬运V2）](../../../api/SIMD-API/basic_api/cube_compute_ISASI/cube_compute_load/LoadData_GMToL1_2DV2.md)。
- C API：[asc_copy_l12l0a（2D矩阵搬运，arch3510）](../../../api/SIMD-API/c_api/cube_datamove/asc_copy_l12l0a/asc_copy_l12l0a_2d_arch_3510.md)、[asc_copy_l12l0b（2D矩阵搬运，arch3510）](../../../api/SIMD-API/c_api/cube_datamove/asc_copy_l12l0b/asc_copy_l12l0b_2d_arch_3510.md)、[asc_copy_gm2l1（arch3510）](../../../api/SIMD-API/c_api/cube_datamove/asc_copy_gm2l1/asc_copy_gm2l1_2d_arch_3510.md)。

#### 典型样例说明

[load_data_2dv2_l12l0样例](../../../../../examples/01_simd_cpp_api/03_basic_api/03_matrix_compute/load_data_2dv2_l12l0)：演示使用2D V2指令将矩阵数据从L1 Buffer搬运至L0A Buffer。

### 新增Stride卷积矩阵搬运<a name="section_loaddata_with_stride"></a>

#### 特性应用场景说明

3510架构新增Stride卷积矩阵搬运能力，用于需要配置输出矩阵K轴方向偏移量的卷积数据搬运场景。

#### 功能说明

可参考以下资料：
- 基础API：[LoadDataWithStride](../../../api/SIMD-API/basic_api/cube_compute_ISASI/cube_compute_load/LoadDataWithStride.md)。
- C API：[asc_copy_l12l0a（3D矩阵搬运，arch3510）](../../../api/SIMD-API/c_api/cube_datamove/asc_copy_l12l0a/asc_copy_l12l0a_3d_arch_3510.md)、[asc_copy_l12l0b（3D矩阵搬运，arch3510）](../../../api/SIMD-API/c_api/cube_datamove/asc_copy_l12l0b/asc_copy_l12l0b_3d_arch_3510.md)、[asc_set_l13d_rpt](../../../api/SIMD-API/c_api/cube_datamove/asc_set_l13d_rpt.md)和[asc_set_l3d_rpt_b](../../../api/SIMD-API/c_api/cube_datamove/asc_set_l3d_rpt_b.md)。其中，`asc_set_l13d_rpt`和`asc_set_l3d_rpt_b`分别用于配置左寄存器组和右寄存器组的输出矩阵步长。

#### 典型样例说明

[load_data_with_stride样例](../../../../../examples/01_simd_cpp_api/03_basic_api/03_matrix_compute/load_data_with_stride)：演示Stride卷积矩阵搬运。

### 矩阵计算支持更多数据类型<a name="section_mmad_data_types"></a>

#### 特性应用场景说明

3510架构扩展矩阵计算支持的低比特浮点输入类型，可用于更多矩阵计算场景。

#### 功能说明

相比2201架构，3510架构新增支持以下矩阵输入类型：

- 矩阵计算新增支持hifloat8_t、fp8_e5m2_t和fp8_e4m3fn_t。
- MX矩阵计算支持fp4x2_e2m1_t、fp4x2_e1m2_t、fp8_e5m2_t和fp8_e4m3fn_t。MX计算使用fp8_e8m0_t类型的Scale矩阵。

可参考以下资料：
- 基础API：[Mmad](../../../api/SIMD-API/basic_api/cube_compute_ISASI/mmad_compute/Mmad.md)、[MmadMx](../../../api/SIMD-API/basic_api/cube_compute_ISASI/mmad_compute/MmadMx.md)。
- C API：[asc_mmad](../../../api/SIMD-API/c_api/cube_compute/asc_mmad.md)、[asc_mmad_mx](../../../api/SIMD-API/c_api/cube_compute/asc_mmad_mx.md)。

### L0A Buffer分形排布变为NZ<a name="section_l0a_nz"></a>

#### 特性应用场景说明

3510架构的L0A Buffer采用NZ排布，不再采用2201架构上的ZZ排布。

#### 功能说明

由于分形排布变化，3510架构下，在A矩阵转置且非对齐的场景下，L0A Buffer的K方向按8对齐，无[K方向对齐约束](../../../api/SIMD-API/basic_api/cube_compute_ISASI/mmad_compute_key_features/k_direction_alignment_constraint.md#ZH-CN_TOPIC_0000002569070973)。

### 矢量计算基础API扩展<a name="section_vec_api_ext"></a>

#### 特性应用场景说明

3510架构矢量计算基础API新增多项接口和参数变体。

#### 功能说明

**表 3**  矢量计算基础API扩展说明<a name="table_vec_api_ext"></a>

| API | 功能说明 |
|-----|---------|
| [Adds](../../../api/SIMD-API/basic_api/memory_vector_compute/basic_arithmetic/Adds_flexible_scalar.md) | 矢量内每个元素与标量做加法，标量输入支持配置LocalTensor单点元素，支持标量在前、标量在后两种场景。 |
| [Muls](../../../api/SIMD-API/basic_api/memory_vector_compute/basic_arithmetic/Muls_flexible_scalar.md) | 矢量内每个元素与标量做乘法，标量输入支持配置LocalTensor单点元素。 |
| [Subs](../../../api/SIMD-API/basic_api/memory_vector_compute/basic_arithmetic/Subs_flexible_scalar.md) | 矢量内每个元素与标量做减法，标量输入支持配置LocalTensor单点元素。 |
| [Divs](../../../api/SIMD-API/basic_api/memory_vector_compute/basic_arithmetic/Divs_flexible_scalar.md) | 矢量内每个元素与标量做除法，标量输入支持配置LocalTensor单点元素。 |
| [Maxs](../../../api/SIMD-API/basic_api/memory_vector_compute/basic_arithmetic/Maxs_flexible_scalar.md) | 矢量内每个元素与标量取最大值，标量输入支持配置LocalTensor单点元素。 |
| [Mins](../../../api/SIMD-API/basic_api/memory_vector_compute/basic_arithmetic/Mins_flexible_scalar.md) | 矢量内每个元素与标量取最小值，标量输入支持配置LocalTensor单点元素。 |
| [Compares](../../../api/SIMD-API/basic_api/memory_vector_compute/compare_and_select/Compares_flexible_scalar.md) | 矢量内每个元素与标量比较，标量输入支持配置LocalTensor单点元素，支持标量在前、标量在后两种场景。 |
| [Select](../../../api/SIMD-API/basic_api/memory_vector_compute/compare_and_select/Select_flexible_scalar.md) | 根据`selMask`的比特位从两个源操作数中选取元素写入目的操作数。 |
| [Neg](../../../api/SIMD-API/basic_api/memory_vector_compute/basic_arithmetic/Neg_ISASI.md) | 对源操作数取相反数。 |
| [Prelu](../../../api/SIMD-API/basic_api/memory_vector_compute/basic_arithmetic/Prelu_ISASI.md) | 带参数的ReLU计算。 |
| [Mull](../../../api/SIMD-API/basic_api/memory_vector_compute/basic_arithmetic/Mull_ISASI.md) | 32位元素相乘得到64位乘积，拆分为低32位与高32位分别写入两个目的操作数。 |
| [AbsSub](../../../api/SIMD-API/basic_api/memory_vector_compute/composite_compute/AbsSub_ISASI.md) | 计算两个源操作数差的绝对值。 |
| [ExpSub](../../../api/SIMD-API/basic_api/memory_vector_compute/composite_compute/ExpSub_ISASI.md) | 计算`exp(src0)-exp(src1)`。 |
| [MulsCast](../../../api/SIMD-API/basic_api/memory_vector_compute/composite_compute/MulsCast_ISASI.md) | 矢量与标量相乘后再进行数据类型转换。 |
| [Truncate](../../../api/SIMD-API/basic_api/memory_vector_compute/type_conversion/Truncate_ISASI.md) | 对源操作数执行向下取整操作。 |
| [Interleave](../../../api/SIMD-API/basic_api/memory_vector_compute/data_rearrange_ISASI/Interleave.md) | 将两个源操作数中的元素交织存入结果操作数。 |
| [DeInterleave](../../../api/SIMD-API/basic_api/memory_vector_compute/data_rearrange_ISASI/DeInterleave.md) | 将源操作数中的元素按交织方式拆分到两个结果操作数。 |
| [ShiftLeft](../../../api/SIMD-API/basic_api/memory_vector_compute/logical_compute/ShiftLeft_shift_amount_tensor.md) | 对源操作数中的每个元素按移位量左移，移位量由张量输入指定。 |
| [ShiftRight](../../../api/SIMD-API/basic_api/memory_vector_compute/logical_compute/ShiftRight_shift_amount_tensor.md) | 对源操作数中的每个元素按移位量右移，移位量由张量输入指定。 |
| [Ands](../../../api/SIMD-API/basic_api/memory_vector_compute/logical_compute/Ands.md) | 矢量内每个元素和标量做按位与操作，标量输入支持配置LocalTensor单点元素。 |
| [Ors](../../../api/SIMD-API/basic_api/memory_vector_compute/logical_compute/Ors.md) | 矢量内每个元素和标量做按位或操作，标量输入支持配置LocalTensor单点元素。 |


### 新增Regbase矢量计算方式<a name="section_regbase"></a>

#### 特性应用场景说明

3510架构的AIV核采用Regbase架构，中间结果可暂存于寄存器中，从而减少UB读写并提高执行效率。Regbase架构的编程模型请参见[Reg矢量计算编程](../../programming_guide/programming_model/ai_core_simd_programming/c_pointer_programming/reg_vector_computation.md)。

#### 功能说明

Reg矢量计算相关接口的完整接口清单请参考[Reg矢量计算](../../../api/SIMD-API/basic_api/reg_vector_compute/overview.md)文档。

对应的C API请参见[Reg矢量计算C API](../../../api/SIMD-API/c_api/reg_compute/reg_compute.md)。

#### 典型样例说明

- 数据搬运：[reg_load_store_align样例](../../../../../examples/01_simd_cpp_api/03_basic_api/00_data_movement/reg_load_store_align)、[reg_load_store_mask样例](../../../../../examples/01_simd_cpp_api/03_basic_api/00_data_movement/reg_load_store_mask)、[reg_load_store_unalign样例](../../../../../examples/01_simd_cpp_api/03_basic_api/00_data_movement/reg_load_store_unalign)、[reg_load_gather样例](../../../../../examples/01_simd_cpp_api/03_basic_api/00_data_movement/reg_load_gather)、[reg_store_scatter样例](../../../../../examples/01_simd_cpp_api/03_basic_api/00_data_movement/reg_store_scatter)、[reg_move样例](../../../../../examples/01_simd_cpp_api/03_basic_api/00_data_movement/reg_move)、[reg_auxscalar样例](../../../../../examples/01_simd_cpp_api/03_basic_api/00_data_movement/reg_auxscalar)。
- Reg计算：[mul样例](../../../../../examples/01_simd_cpp_api/03_basic_api/02_reg_vector_compute/mul)、[reduce样例](../../../../../examples/01_simd_cpp_api/03_basic_api/02_reg_vector_compute/reduce)、[reduce_pair样例](../../../../../examples/01_simd_cpp_api/03_basic_api/02_reg_vector_compute/reduce_pair)等。
- 高性能算子：[gelu_high_performance样例](../../../../../examples/01_simd_cpp_api/05_best_practices/02_reg_compute/gelu_high_performance)、[gelu_eltwise_high_performance样例](../../../../../examples/01_simd_cpp_api/05_best_practices/02_reg_compute/gelu_eltwise_high_performance)、[softmax_high_performance样例](../../../../../examples/01_simd_cpp_api/05_best_practices/02_reg_compute/softmax_high_performance)。

### 原子操作接口扩展<a name="section_atomic_api"></a>

#### 特性应用场景说明

在多线程或多核并发访问GM中同一数据的场景下，需要对GM地址中的数据执行原子读改写操作，以避免并发写入引发数据竞争。3510架构对原子操作基础接口做了扩充。

#### 功能说明

**表 5**  新增原子操作接口说明<a name="table_atomic_api"></a>

| 功能说明 | 相关基础API | 相关C API |
|---------|------------|-----------|
| 将GM地址上的旧值与输入标量求和并写回，返回修改前的值。 | [AtomicAdd](../../../api/SIMD-API/basic_api/atomic_operations/AtomicAdd.md) | [asc_atomic_add](../../../api/SIMD-API/c_api/atomic/scalar_atomic/asc_atomic_add.md) |
| 将GM地址上的旧值与输入标量取最大值并写回，返回修改前的值。 | [AtomicMax](../../../api/SIMD-API/basic_api/atomic_operations/AtomicMax.md) | [asc_atomic_max](../../../api/SIMD-API/c_api/atomic/scalar_atomic/asc_atomic_max.md) |
| 将GM地址上的旧值与输入标量取最小值并写回，返回修改前的值。 | [AtomicMin](../../../api/SIMD-API/basic_api/atomic_operations/AtomicMin.md) | [asc_atomic_min](../../../api/SIMD-API/c_api/atomic/scalar_atomic/asc_atomic_min.md) |
| 将GM地址上的旧值与输入标量交换，返回修改前的值。 | [AtomicExch](../../../api/SIMD-API/basic_api/atomic_operations/AtomicExch.md) | [asc_atomic_exch](../../../api/SIMD-API/c_api/atomic/scalar_atomic/asc_atomic_exch.md) |
| 将GM地址上的旧值与期望值比较，相等则写入新值，返回修改前的值。 | [AtomicCas](../../../api/SIMD-API/basic_api/atomic_operations/AtomicCas.md) | [asc_atomic_cas](../../../api/SIMD-API/c_api/atomic/scalar_atomic/asc_atomic_cas.md) |

#### 典型样例说明

[scalar_atomic_operations样例](../../../../../examples/01_simd_cpp_api/03_basic_api/06_atomic/scalar_atomic_operations)：基于AtomicAdd和AtomicCas接口，演示在GM地址上进行标量原子加和原子比较交换的实现流程，支持多核并行更新单个内存地址。

### 新增DCI（数据缓存失效）能力<a name="section_dci"></a>

#### 特性应用场景说明

算子执行结束时，需将DCache置为失效，防止后续算子读取其中的无效数据。默认情况下，编译框架会在算子尾部插入DCCI（DataCacheCleanAndInvalid）指令，在使DCache失效的同时将脏数据写回GM，因此会产生额外的Clean开销。3510架构新增DCI（DataCacheInvalid）数据缓存失效能力，DCI指令可直接使整个数据缓存失效，而不回写脏缓存行。在算子尾部使用DCI指令替代DCCI指令，可跳过将DCache脏数据同步至GM的Clean过程，从而降低算子尾部开销。

#### 功能说明

可通过以下两种方式插入DCI指令：
1. 编译选项方式：编译时配置[`--cce-no-dcache-flush`](../../operator_practice/simd_operator_optimization/overhead_optimization/dci_compile_option.md)，编译框架将在算子尾部插入DCI指令，替代默认的DCCI指令。推荐使用该方式开启DCI能力。详细说明请参考[设置DCI编译选项来减少算子尾开销](../../operator_practice/simd_operator_optimization/overhead_optimization/dci_compile_option.md)。
2. 接口方式：使用[asc_dci](../../../api/SIMD-API/c_api/cache_ctrl/asc_dci.md)接口执行数据缓存失效，调用前需先通过[asc_sync_data_barrier](../../../api/SIMD-API/c_api/sync/asc_sync_data_barrier.md)插入DSB_ALL指令，等待所有内存访问指令执行结束。

### 新增SSBuffer核内存储单元<a name="section_ssbuf"></a>

#### 特性应用场景说明

在Mix算子中，AIC核与AIV核之间需要传递控制信息和标量数据。在[NPU架构版本2201](../../programming_guide/language_extension/simd_builtin_keywords.md#npu-arch)上，AIC与AIV通过GM通信，通信延迟较高且占用GM带宽。3510架构新增SSBuffer核内存储单元，支持AI Core内的AIC核与AIV核通过Scalar直接访问SSBuffer进行核间通信，该方式无需访问GM，可降低通信延迟并减少GM带宽占用。SSBuffer适用于同一任务内AIC核与AIV核之间的小数据量通信，例如传递控制信息、地址或少量标量数据。大块数据应通过相应的数据通路搬运，不应将SSBuffer用作通用数据缓存。

#### 功能说明

- 存储规格：[SSBuffer](../../programming_guide/advanced_programming/hardware_implementation/architecture_spec/npu_arch_3510.md)是3510架构AI Core内新增的核间通信存储单元，容量为3KB。SSBuffer使用独立于L1 Buffer和UB的地址空间，对应的地址空间限定符为`__ssbuf__`。详情请参见[SIMD BuiltIn关键字](../../programming_guide/language_extension/simd_builtin_keywords.md)。
- 使用范围：

  - SSBuffer在分离模式的AIC核数与AIV核数之比为1:2的混合算子中，AIC核与对应AIV核可通过SSBuffer交换数据，核函数类型和AIC核数与AIV核数之比必须满足核间同步接口的约束。

- 访问方式：

  - Mix算子是同时包含矩阵计算和矢量计算的算子类型，MIX模式是AIC核与AIV核混合运行的内核模式。
  - 非MIX模式下，AIC、AIV0、AIV1各自独立占用1KB空间；MIX模式下AIC核与各AIV核共享整个3KB空间。
  - 通过[GetSsbufBaseAddr](../../../api/SIMD-API/basic_api/tool_interface/system_resources_and_variables/GetSsbufBaseAddr.md)接口获取SSBuffer基地址。该接口返回`__ssbuf__`指针，可基于基地址偏移进行读写访问。
  - SSBuffer仅支持通过32字节对齐的读写指令访问，通信数据块的起始地址和访问粒度均须满足32字节对齐要求。不得对SSBuffer执行小于32字节的成员读写；访问地址超出SSBuffer范围时会触发异常。

- 使用约束：

  - SSBuffer的初始内容不确定，读取结果不保证全为0。每次通信均应由生产者完整写入数据，消费者不得依赖SSBuffer中的历史内容。
  - 数据布局必须在通信双方（AIC与AIV）保持一致，且数据不得越过SSBuffer末端地址。
  - AIC和AIV启动不同任务时不能访问SSBuffer；当前任务未开启SSBuffer能力时不能访问。



#### 典型样例说明

以下代码示例演示SSBuffer核间通信的完整用法，其中AIC核数与AIV核数之比为1:2，AIV0向AIC单向传递32字节消息。使用SSBuffer前需开启[`ENABLE_CV_COMM_VIA_SSBUF`](../../programming_guide/compilation_and_execution/operator_compilation/ai_core_operator_compilation.md#内置编译宏开关)编译选项。

- 获取基地址并定义消息：

  ```cpp
  // 获取SSBuffer基地址，返回__ssbuf__指针；NPU域中该特殊基址可能显示为0或nil，不应作为普通空指针解引用。
  __ssbuf__ void* ssbuf = AscendC::GetSsbufBaseAddr();

  // 定义32字节对齐的通信消息。
  struct alignas(32) SsbufferMessage {
      uint64_t value[4];
  };

  // 基于基地址偏移定位消息区域，起始偏移须32字节对齐且不越过SSBuffer末端。
  __ssbuf__ uint8_t* ssbuffer = reinterpret_cast<__ssbuf__ uint8_t*>(ssbuf);
  __ssbuf__ SsbufferMessage* message =
      reinterpret_cast<__ssbuf__ SsbufferMessage*>(ssbuffer + 256);
  ```

- AIV核（生产者）写入消息并发布就绪标记：

  ```cpp
  if ASCEND_IS_AIV {
      // 1:2场景中仅AIV0生产本示例的消息，AIV1仍须参与模式2同步。
      if (AscendC::GetSubBlockIdx() == 0) {
          // 生产者将完整32字节消息写入SSBuffer。
          message->value[0] = 0x1122334455667788ULL;
          message->value[1] = 0x8877665544332211ULL;
          message->value[2] = 0x0123456789abcdefULL;
          message->value[3] = 0xfedcba9876543210ULL;
      }
      // SSBuffer访问位于Scalar流水，先建立Scalar到Vector的核内顺序，再以模式2发布AIV到AIC的就绪标记。
      AscendC::SetFlag<AscendC::HardEvent::S_V>(0);
      AscendC::WaitFlag<AscendC::HardEvent::S_V>(0);
      AscendC::CrossCoreSetFlag<2, PIPE_V>(AIV_TO_AIC_FLAG);
  }
  ```

- AIC核（消费者）等待标记完成后读取消息：

  ```cpp
  if ASCEND_IS_AIC {
      // 模式2等待同一逻辑AI Core上的两个AIV均发布完成。
      AscendC::CrossCoreWaitFlag<2, PIPE_M>(AIV_TO_AIC_FLAG);
      // 先建立Cube流水到Scalar读取的顺序，再读取SSBuffer中的消息。
      AscendC::SetFlag<AscendC::HardEvent::M_S>(0);
      AscendC::WaitFlag<AscendC::HardEvent::M_S>(0);
      // 消费者按通信双方约定的布局使用message中的消息数据。
      output[0] = message->value[0];
      output[1] = message->value[1];
      output[2] = message->value[2];
      output[3] = message->value[3];
  }
  ```

### 新增Mutex核内同步能力<a name="section_mutex"></a>

#### 特性应用场景说明

在Vector算子中，异步流水指令之间通常通过SetFlag和WaitFlag事件进行同步，例如GM到UB搬运（PIPE_MTE2）、向量计算（PIPE_V）和UB到GM搬运（PIPE_MTE3）。该方式需要显式配对事件，编码较为复杂，且受事件资源数量限制。3510架构新增Mutex能力，通过锁定和释放指定流水建立流水间的同步依赖。与SetFlag和WaitFlag相比，Mutex与其他流水解耦，可简化反向同步，并提供更多可用的同步信号量。

#### 功能说明

可参考以下资料：
- 基础API：[Mutex::Lock](../../../api/SIMD-API/basic_api/sync_control/intra_core_sync/Mutex_ISASI.md)、[Mutex::Unlock](../../../api/SIMD-API/basic_api/sync_control/intra_core_sync/Mutex_ISASI.md)、[AllocMutexID](../../../api/SIMD-API/basic_api/sync_control/intra_core_sync/AllocMutexID_ISASI.md)。
- C API：[asc_lock](../../../api/SIMD-API/c_api/sync/asc_lock.md)、[asc_unlock](../../../api/SIMD-API/c_api/sync/asc_unlock.md)。

#### 典型样例说明

[mutex样例](../../../../../examples/01_simd_cpp_api/03_basic_api/05_sync_control/mutex)：使用Mutex实现MTE2、V流水与MTE3流水之间的同步，在未使用SetFlag、WaitFlag的情况下正确完成数据搬运与计算。

### 新增核间同步控制模式4<a name="section_crosscore"></a>

#### 特性应用场景说明

在Mix算子中，AIV核（Vector）与AIC核（Cube）之间存在数据依赖，需要通过核间同步接口保证数据就绪后再发起后续计算。在[NPU架构版本2201](../../programming_guide/language_extension/simd_builtin_keywords.md#npu-arch)上，当AIC核等待同步时，AIV0与AIV1必须同时上报，即采用模式2实现AIC与所有AIV同步，因此同步粒度较粗。3510架构新增核间同步控制模式4，用于AIC与单个AIV同步，AIV0与AIV1可分别触发AIC等待，从而细化同步粒度、减少冗余等待并提高执行效率。

#### 功能说明

可参考以下资料：
- 基础API：[CrossCoreSetFlag](../../../api/SIMD-API/basic_api/sync_control/inter_core_sync/CrossCoreSetFlag_ISASI.md)、[CrossCoreWaitFlag](../../../api/SIMD-API/basic_api/sync_control/inter_core_sync/CrossCoreWaitFlag_ISASI.md)。
- C API：[asc_sync_intra_arrive](../../../api/SIMD-API/c_api/sync/asc_sync_intra_arrive.md)、[asc_sync_intra_wait](../../../api/SIMD-API/c_api/sync/asc_sync_intra_wait.md)。

#### 典型样例说明

[cross_core_set_wait_flag样例](../../../../../examples/01_simd_cpp_api/03_basic_api/05_sync_control/cross_core_set_wait_flag)：在Mix算子场景下，使用模式4核间同步，AIV0与AIV1分别单独触发AIC核等待，完成UB到L1 Buffer搬运与Mmad计算的流水协作。

### 其他增强说明

除上述特性外，3510架构还提供以下增强能力。

#### 架构规格

- **AI Core核数增加**：3510架构的AI Core核数较2201架构有所增加，可支持更大的片上并行规模。
- **UB容量提升**：3510架构的UB总容量提升至256KB，由16个bank组成8个bank group，每个bank group包含2个bank，每个bank大小为16KB。若发生UB冲突，可参考[避免UB的bank冲突](../../operator_practice/simd_operator_optimization/memory_access/avoid_ub_bank_conflict/overview.md)解决；[bank_conflict_3510样例](../../../../../examples/01_simd_cpp_api/05_best_practices/04_memory_access/bank_conflict_3510)提供了3510架构下UB Bank冲突场景的验证与分析方法。

#### 数据类型

- **数据类型扩展**：3510架构扩展了支持的数据类型，例如fp4x2_e2m1_t、fp4x2_e1m2_t、hifloat8_t、fp8_e8m0_t、fp8_e5m2_t和fp8_e4m3fn_t。具体请参见[数据类型介绍](../../../api/SIMD-API/basic_api/data_structures/builtin_data_types.md)。在Host端，fp4x2_e2m1_t、fp4x2_e1m2_t、hifloat8_t、fp8_e8m0_t、fp8_e5m2_t、fp8_e4m3fn_t、int4x2_t和bfloat16_t仅支持以C++模板形式定义与声明。具体请参见[约束说明](../../programming_guide/compilation_and_execution/operator_compilation/constraints.md)。

#### 缓存管理

- **新增Cache类型**：2201架构仅支持L2 Cache、DCache和ICache，3510架构新增支持NDDMA Cache。ND-DMA为多维数据搬运能力的名称，NDDMA Cache为该能力对应的缓存类型名称。多核间数据一致性说明请参见[Cache一致性](../../programming_guide/advanced_programming/memory_model/cache_coherence.md)，完整Cache类型与功能说明请参见[系统缓存概述](../../../api/SIMD-API/basic_api/cache_control/system_cache_overview.md)。
- **Cache控制接口增强**：3510架构下的[DataCacheCleanAndInvalid](../../../api/SIMD-API/basic_api/cache_control/DataCacheCleanAndInvalid.md)接口新增支持`CACHELINE_ATOMIC`参数，用于在原子操作过程中保证DCache与GM的一致性。

#### 编译选项与空间优化

- **UB空间使用与编译选项**：UB总容量为256KB，其中部分空间可能用于系统预留或划分为Data Cache。3510架构下Ascend C接口内部使用**2KB**预留UB空间，用于指令兼容或存储中间数据；2201架构下的预留UB空间为8KB。以下编译选项生效，可调整可用的UB空间大小：
  - `--cce-disable-asc-reserved-ubuf`：禁用Ascend C接口使用预留UB空间，开启后依赖预留UB空间的接口不可用，3510架构下使用预留UB空间的API范围请参考[使用预留UB空间的API列表](../../programming_guide/programming_model/ai_core_simd_programming/cpp_tensor_programming/static_tensor_programming.md#section_reserved_ubuf_api)。
  - `--cce-disable-vf-stack-reserved-ubuf`：禁用SIMD VF栈预留的UB空间。该选项仅在3510架构下生效。开启后可释放6KB预留空间作为普通UB使用，但编译器无法再使用该空间缓存寄存器溢出数据，需保证寄存器不溢出。
- **性能优化编译宏**：`NO_OVERLAP_IN_MULTI_REPEAT`可在没有地址重叠的情况下移除不必要的内存同步指令，提升基础API高维切分计算场景的性能，详情请参考[内置编译宏开关](../../programming_guide/compilation_and_execution/operator_compilation/ai_core_operator_compilation.md#内置编译宏开关)。
- **DCI编译选项**：有关`--cce-no-dcache-flush`编译选项的使用说明，请参见[新增DCI（数据缓存失效）能力](#section_dci)。
