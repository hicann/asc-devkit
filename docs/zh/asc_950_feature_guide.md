# Ascend 950PR/Ascend 950DT新增特性导航

本文档索引Ascend 950PR/Ascend 950DT（NPU架构版本3510）相比上一代（2201）新增的硬件特性，以及每个特性对应的编程模型、API接口和算子实践资料链接。

> 适用芯片：Ascend 950PR、Ascend 950DT（`__NPU_ARCH__` = 3510）

> 架构变更详情见：[2201到3510架构变更](guide/跨代迁移兼容性指南/3510架构迁移指导/2201到3510架构变更.md)

---

## 新增特性总览

| 序号 | 新增特性 | 对应编程模型和编程特性 | 对应API | 对应算子实践资料 |
|------|--------------|-------------------|---------|---------------|
| 1 | **RegBase架构**：AIV核Vector计算从MemBase切换到RegBase，数据从UB搬运到Register进行计算，中间结果直接在寄存器操作，无需回写UB | [Reg矢量计算编程](guide/编程指南/编程模型/AI-Core-SIMD编程/基于Tensor的CPP编程/Reg矢量计算编程.md) | [Reg矢量计算API列表](api/SIMD-API/基础API/Reg矢量计算/Reg矢量计算.md) | [VF融合优化](guide/算子实践参考/SIMD算子性能优化/矢量计算/VF性能优化/VF融合优化.md)、[VF循环优化](guide/算子实践参考/SIMD算子性能优化/矢量计算/VF性能优化/VF循环优化.md) |
| 2 | **SIMT编程模型**：新增SIMT硬件单元（DCache、Warp Scheduler、128KB Register File），支持线程级并行编程 | [AI Core SIMT编程](guide/编程指南/编程模型/AI-Core-SIMT编程/AI-Core-SIMT编程.md) | [SIMT API列表](api/SIMT-API/概述.md) | [SIMT算子实现](guide/算子实践参考/SIMT算子实现/基础知识.md)、[SIMT算子性能优化](guide/算子实践参考/SIMT算子性能优化/内存访问/访存合并.md) |
| 3 | **SIMD与SIMT混合编程**：同一Kernel中SIMD和SIMT代码协同工作 | [SIMD与SIMT混合编程](<guide/编程指南/高级编程/高级AI-Core编程模型/SIMD与SIMT混合编程/概述.md>) | [混合编程API列表](api/SIMT-API/SIMD与SIMT混合编程简介/API列表-148.md)、[内置变量](api/SIMT-API/SIMD与SIMT混合编程简介/扩展语法/内置变量-145.md) | [混合编程算子实现](guide/算子实践参考/SIMD与SIMT混合算子实现/基础知识.md)、[混合编程性能优化](guide/算子实践参考/SIMD与SIMT混合算子性能优化/内存访问/使用Unified-Buffer提升内存访问效率.md) |
| 4 | **HiF8(hifloat8_t)数据类型**：Cube计算单元新增支持HiF8数据类型的矩阵乘 | [3510架构规格](guide/编程指南/高级编程/硬件实现/架构规格/NPU架构版本3510.md)中Cube支持的数据类型 | [内置数据类型说明](api/SIMD-API/基础API/数据结构/内置数据类型.md)、[Cast类型转换](api/SIMD-API/基础API/Reg矢量计算/类型转换/Cast.md)、[asc_float2hif8（废弃）](api/SIMD-API/C-API/reg/data_type_convert/asc_float2hif8_deprecated.md) | 资料开发中 |
| 5 | **UB到L1 Buffer通路**：支持数据直接从UB搬运到L1 Buffer，无需经GM中转 | [3510架构变更-搬运单元](guide/跨代迁移兼容性指南/3510架构迁移指导/2201到3510架构变更.md) | 新增支持[UB到L1 Buffer数据搬运](api/SIMD-API/基础API/矩阵计算（ISASI）/矩阵计算的搬入/DataCopy（UBToL1连续数据搬运）.md) | 资料开发中 |
| 6 | **L0C到UB通路**：支持数据直接从L0C Buffer搬运到UB，无需经GM中转 | [3510架构变更-搬运单元](guide/跨代迁移兼容性指南/3510架构迁移指导/2201到3510架构变更.md) | [asc_copy_l0c2ub](api/SIMD-API/C-API/cube_datamove/asc_copy_l0c2ub.md) | [矩阵乘结果累加](guide/算子实践参考/SIMD算子性能优化/矩阵计算/通过L0C-Buffer数据暂存实现高效的矩阵乘结果累加.md) |
| 7 | **ND-DMA搬运指令**：扩展DataCopy能力，可自由配置搬入数据的维度信息及Stride | [3510架构变更-搬运单元](guide/跨代迁移兼容性指南/3510架构迁移指导/2201到3510架构变更.md) | [多维数据搬运ISASI](api/SIMD-API/基础API/Memory矢量计算/数据搬运/DataCopy（GMToUB多维数据搬运NDDMA）.md)、[asc_ndim_copy_gm2ub](api/SIMD-API/C-API/vector_datamove/asc_ndim_copy_gm2ub.md) | [非连续搬运减少搬运次数](guide/算子实践参考/SIMD算子性能优化/内存访问/非连续搬运场景减少搬运次数.md) |
| 8 | **SSBuffer核间存储**：新增核内存储单元，支持AIC核和AIV核通过Scalar访问 | 资料开发中 | 不涉及 | 资料开发中 |
| 9 | **Mutex同步**：核内异步流水指令间同步，功能类似CPU中的锁机制 | 资料开发中 | [Mutex ISASI](<api/SIMD-API/基础API/同步控制/核内同步/Mutex(ISASI).md>)、[asc_lock](api/SIMD-API/C-API/sync/asc_lock.md) | 资料开发中 |
| 10 | **CrossCore核间同步**：AIV0与AIV1可独立触发AIC等待 | 资料开发中 | [CrossCoreSetFlag](<api/SIMD-API/基础API/同步控制/核间同步/CrossCoreSetFlag(ISASI).md>)、[CrossCoreWaitFlag](<api/SIMD-API/基础API/同步控制/核间同步/CrossCoreWaitFlag(ISASI).md>) | [CV融合算子实现](guide/算子实践参考/SIMD算子实现/融合算子编程/CV融合/算子实现.md) |
| 11 | **MX（MicroScaling）数据类型**：支持FP8_E4M3/MXFP4/8低比特矩阵运算，内存占用减半、算力吞吐倍增 | [3510架构变更-搬运单元](guide/跨代迁移兼容性指南/3510架构迁移指导/2201到3510架构变更.md)中LoadData扩展 | [asc_mmad_mx](api/SIMD-API/C-API/cube_compute/asc_mmad_mx.md)、[asc_copy_l12l0a_mx](api/SIMD-API/C-API/cube_datamove/asc_copy_l12l0a_mx.md)、[asc_copy_l12l0b_mx](api/SIMD-API/C-API/cube_datamove/asc_copy_l12l0b_mx.md) | [Matmul特性介绍](guide/算子实践参考/SIMD算子实现/矩阵编程（高阶API）/特性场景/Matmul特性介绍.md)、[MxMatmul场景](guide/算子实践参考/SIMD算子实现/矩阵编程（高阶API）/特性场景/MxMatmul场景.md)、[矩阵乘量化/反量化](guide/算子实践参考/SIMD算子实现/矩阵编程（高阶API）/特性场景/矩阵乘输出的量化-反量化.md) |
| 12 | **Fixpipe增强**：新增NZ2DN随路转换，支持Fixpipe将NZ格式数据随路转为DN格式 | [3510架构变更-搬运单元](guide/跨代迁移兼容性指南/3510架构迁移指导/2201到3510架构变更.md) | [Fixpipe API](api/SIMD-API/基础API/矩阵计算（ISASI）/矩阵计算的搬出/Fixpipe（L0C到GM数据搬运）.md)、[asc_copy_l0c2gm](api/SIMD-API/C-API/cube_datamove/asc_copy_l0c2gm/asc_copy_l0c2gm_arch_3510.md)、[asc_copy_l0c2l1](api/SIMD-API/C-API/cube_datamove/asc_copy_l0c2l1/asc_copy_l0c2l1_arch_3510.md) | [矩阵乘结果累加](guide/算子实践参考/SIMD算子性能优化/矩阵计算/通过L0C-Buffer数据暂存实现高效的矩阵乘结果累加.md) |
| 13 | **UB互连/bank结构变化**：3510架构UB从16个bank group（各3个4KB bank）变为8个bank group（各2个16KB bank） | [3510架构规格-存储单元](guide/编程指南/高级编程/硬件实现/架构规格/NPU架构版本3510.md) | 不涉及 | [避免UB bank冲突概述](guide/算子实践参考/SIMD算子性能优化/内存访问/避免UB的bank冲突/概述.md)、[2201 bank冲突](guide/算子实践参考/SIMD算子性能优化/内存访问/避免UB的bank冲突/避免bank冲突（NPU架构版本2201）.md)、[3510 bank冲突](guide/算子实践参考/SIMD算子性能优化/内存访问/避免UB的bank冲突/避免bank冲突（NPU架构版本3510）.md)、[SIMT避免Bank冲突](guide/算子实践参考/SIMT算子性能优化/内存访问/避免UB的Bank冲突.md) |

---

## 资料状态说明

| 状态标签 | 含义 |
|---------|------|
| （有链接） | 该特性对应资料已存在于docs目录 |
| **资料开发中** | 该特性暂无专门资料，需后续补充 |
| **不涉及** | 该特性不涉及相关资料，后续不计划补充 |

---

## 相关文档

- [跨代迁移兼容性指南-概述](guide/跨代迁移兼容性指南/概述.md)
- [2201到3510架构变更](guide/跨代迁移兼容性指南/3510架构迁移指导/2201到3510架构变更.md)
- [基础API迁移指导](guide/跨代迁移兼容性指南/3510架构迁移指导/2201迁移3510指导/基础API迁移指导.md)
- [高阶API迁移指导](guide/跨代迁移兼容性指南/3510架构迁移指导/2201迁移3510指导/高阶API迁移指导.md)
- [算子编译迁移指导](guide/跨代迁移兼容性指南/3510架构迁移指导/2201迁移3510指导/算子编译迁移指导.md)
- [NPU架构版本3510规格](guide/编程指南/高级编程/硬件实现/架构规格/NPU架构版本3510.md)
