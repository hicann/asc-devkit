# Ascend 950PR/Ascend 950DT新增特性导航

本文档索引Ascend 950PR/Ascend 950DT（NPU架构版本3510）相比上一代（2201）新增的硬件特性，以及每个特性对应的编程模型、API接口和算子实践资料链接。

> 适用芯片：Ascend 950PR、Ascend 950DT（`__NPU_ARCH__` = 3510）

> 架构变更详情见：[2201到3510架构变更](guide/cross_gen_migration_guide/3510_arch_migration/2201_to_3510_arch_changes.md)

---

## 新增特性总览

| 序号 | 新增特性 | 对应编程模型和编程特性 | 对应API | 对应算子实践资料 |
|------|--------------|-------------------|---------|---------------|
| 1 | **RegBase架构**：AIV核Vector计算从MemBase切换到RegBase，数据从UB搬运到Register进行计算，中间结果直接在寄存器操作，无需回写UB | [Reg矢量计算编程](guide/programming_guide/programming_model/ai_core_simd_programming/cpp_tensor_programming/reg_vector_computation.md) | [Reg矢量计算API列表](api/SIMD-API/basic_api/reg_vector_compute/reg_vector_compute.md) | [VF融合优化](guide/operator_practice/simd_operator_optimization/vector_compute/vf_optimization/vf_fusion_optimization.md)、[VF循环优化](guide/operator_practice/simd_operator_optimization/vector_compute/vf_optimization/vf_loop_optimization.md) |
| 2 | **SIMT编程模型**：新增SIMT硬件单元（DCache、Warp Scheduler、128KB Register File），支持线程级并行编程 | [AI Core SIMT编程](guide/programming_guide/programming_model/ai_core_simt_programming/ai_core_simt_programming.md) | [SIMT API列表](api/SIMT-API/overview.md) | [SIMT算子实现](guide/operator_practice/simt_operator_impl/basics.md)、[SIMT算子性能优化](guide/operator_practice/simt_operator_optimization/memory_access/memory_access_merge.md) |
| 3 | **SIMD与SIMT混合编程**：同一Kernel中SIMD和SIMT代码协同工作 | [SIMD与SIMT混合编程](<guide/programming_guide/advanced_programming/advanced_ai_core_programming_model/simd_simt_hybrid_programming/overview.md>) | [混合编程API列表](api/SIMT-API/SIMD_SIMT_hybrid_programming_intro/api_list.md)、[内置变量](api/SIMT-API/SIMD_SIMT_hybrid_programming_intro/extended_syntax/builtin_variables.md) | [混合编程算子实现](guide/operator_practice/simd_simt_hybrid_operator_impl/basics.md)、[混合编程性能优化](guide/operator_practice/simd_simt_hybrid_optimization/memory_access/ub_memory_access.md) |
| 4 | **HiF8(hifloat8_t)数据类型**：Cube计算单元新增支持HiF8数据类型的矩阵乘 | [3510架构规格](guide/programming_guide/advanced_programming/hardware_implementation/architecture_spec/npu_arch_3510.md)中Cube支持的数据类型 | [内置数据类型说明](api/SIMD-API/basic_api/data_structures/builtin_data_types.md)、[Cast类型转换](api/SIMD-API/basic_api/reg_vector_compute/type_conversion/Cast.md)、[asc_float2hif8（废弃）](api/SIMD-API/c_api/reg/data_type_convert/asc_float2hif8_deprecated.md) | 资料开发中 |
| 5 | **UB到L1 Buffer通路**：支持数据直接从UB搬运到L1 Buffer，无需经GM中转 | [3510架构变更-搬运单元](guide/cross_gen_migration_guide/3510_arch_migration/2201_to_3510_arch_changes.md) | 新增支持[UB到L1 Buffer数据搬运](api/SIMD-API/basic_api/cube_compute_ISASI/cube_compute_load/DataCopy_UBToL1_continuous.md) | 资料开发中 |
| 6 | **L0C到UB通路**：支持数据直接从L0C Buffer搬运到UB，无需经GM中转 | [3510架构变更-搬运单元](guide/cross_gen_migration_guide/3510_arch_migration/2201_to_3510_arch_changes.md) | [asc_copy_l0c2ub](api/SIMD-API/c_api/cube_data_move/asc_copy_l0c2ub.md) | [矩阵乘结果累加](guide/operator_practice/simd_operator_optimization/matrix_compute/l0c_buffer_matmul_accumulate.md) |
| 7 | **ND-DMA搬运指令**：扩展DataCopy能力，可自由配置搬入数据的维度信息及Stride | [3510架构变更-搬运单元](guide/cross_gen_migration_guide/3510_arch_migration/2201_to_3510_arch_changes.md) | [多维数据搬运ISASI](api/SIMD-API/basic_api/memory_vector_compute/data_move/DataCopy_GMToUB_NDDMA.md)、[asc_ndim_copy_gm2ub](api/SIMD-API/c_api/vector_data_move/asc_ndim_copy_gm2ub.md) | [非连续搬运减少搬运次数](guide/operator_practice/simd_operator_optimization/memory_access/reduce_non_contiguous_transfer.md) |
| 8 | **SSBuffer核间存储**：新增核内存储单元，支持AIC核和AIV核通过Scalar访问 | 资料开发中 | 不涉及 | 资料开发中 |
| 9 | **Mutex同步**：核内异步流水指令间同步，功能类似CPU中的锁机制 | 资料开发中 | [Mutex ISASI](<api/SIMD-API/basic_api/sync_control/intra_core_sync/Mutex_ISASI.md>)、[asc_lock](api/SIMD-API/c_api/sync/asc_lock.md) | 资料开发中 |
| 10 | **CrossCore核间同步**：AIV0与AIV1可独立触发AIC等待 | 资料开发中 | [CrossCoreSetFlag](<api/SIMD-API/basic_api/sync_control/inter_core_sync/CrossCoreSetFlag_ISASI.md>)、[CrossCoreWaitFlag](<api/SIMD-API/basic_api/sync_control/inter_core_sync/CrossCoreWaitFlag_ISASI.md>) | [CV融合算子实现](guide/operator_practice/simd_operator_impl/fusion_operator_programming/cv_fusion/operator_impl.md) |
| 11 | **MX（MicroScaling）数据类型**：支持FP8_E4M3/MXFP4/8低比特矩阵运算，内存占用减半、算力吞吐倍增 | [3510架构变更-搬运单元](guide/cross_gen_migration_guide/3510_arch_migration/2201_to_3510_arch_changes.md)中LoadData扩展 | [asc_mmad_mx](api/SIMD-API/c_api/cube_compute/asc_mmad_mx.md)、[asc_copy_l12l0a_mx](api/SIMD-API/c_api/cube_data_move/asc_copy_l12l0a_mx.md)、[asc_copy_l12l0b_mx](api/SIMD-API/c_api/cube_data_move/asc_copy_l12l0b_mx.md) | [Matmul特性介绍](guide/operator_practice/simd_operator_impl/matrix_advanced_api/feature_scenarios/matmul_features.md)、[MxMatmul场景](guide/operator_practice/simd_operator_impl/matrix_advanced_api/feature_scenarios/mx_matmul_scenario.md)、[矩阵乘量化/反量化](guide/operator_practice/simd_operator_impl/matrix_advanced_api/feature_scenarios/matmul_output_quant_dequant.md) |
| 12 | **Fixpipe增强**：新增NZ2DN随路转换，支持Fixpipe将NZ格式数据随路转为DN格式 | [3510架构变更-搬运单元](guide/cross_gen_migration_guide/3510_arch_migration/2201_to_3510_arch_changes.md) | [Fixpipe API](api/SIMD-API/basic_api/cube_compute_ISASI/cube_compute_store/Fixpipe_L0CToGM.md)、[asc_copy_l0c2gm](api/SIMD-API/c_api/cube_data_move/asc_copy_l0c2gm/asc_copy_l0c2gm_arch_3510.md)、[asc_copy_l0c2l1](api/SIMD-API/c_api/cube_data_move/asc_copy_l0c2l1/asc_copy_l0c2l1_arch_3510.md) | [矩阵乘结果累加](guide/operator_practice/simd_operator_optimization/matrix_compute/l0c_buffer_matmul_accumulate.md) |
| 13 | **UB互连/bank结构变化**：3510架构UB从16个bank group（各3个4KB bank）变为8个bank group（各2个16KB bank） | [3510架构规格-存储单元](guide/programming_guide/advanced_programming/hardware_implementation/architecture_spec/npu_arch_3510.md) | 不涉及 | [避免UB bank冲突概述](guide/operator_practice/simd_operator_optimization/memory_access/avoid_ub_bank_conflict/overview.md)、[2201 bank冲突](guide/operator_practice/simd_operator_optimization/memory_access/avoid_ub_bank_conflict/avoid_bank_conflict_npu_arch_2201.md)、[3510 bank冲突](guide/operator_practice/simd_operator_optimization/memory_access/avoid_ub_bank_conflict/avoid_bank_conflict_npu_arch_3510.md)、[SIMT避免Bank冲突](guide/operator_practice/simt_operator_optimization/memory_access/avoid_ub_bank_conflict_simt.md) |

---

## 资料状态说明

| 状态标签 | 含义 |
|---------|------|
| （有链接） | 该特性对应资料已存在于docs目录 |
| **资料开发中** | 该特性暂无专门资料，需后续补充 |
| **不涉及** | 该特性不涉及相关资料，后续不计划补充 |

---

## 相关文档

- [跨代迁移兼容性指南-概述](guide/cross_gen_migration_guide/overview.md)
- [2201到3510架构变更](guide/cross_gen_migration_guide/3510_arch_migration/2201_to_3510_arch_changes.md)
- [基础API迁移指导](guide/cross_gen_migration_guide/3510_arch_migration/2201_to_3510_guide/basic_api_migration.md)
- [高阶API迁移指导](guide/cross_gen_migration_guide/3510_arch_migration/2201_to_3510_guide/advanced_api_migration.md)
- [算子编译迁移指导](guide/cross_gen_migration_guide/3510_arch_migration/2201_to_3510_guide/op_compilation_migration.md)
- [NPU架构版本3510规格](guide/programming_guide/advanced_programming/hardware_implementation/architecture_spec/npu_arch_3510.md)
