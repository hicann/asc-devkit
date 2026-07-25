# Ascend 950PR/Ascend 950DT New Feature Navigation

This document indexes the new hardware features of Ascend 950PR/Ascend 950DT (NPU architecture version 3510) compared with the previous generation (2201), and provides links to the corresponding programming models, APIs, and operator practice materials for each feature.

> Applicable chips: Ascend 950PR, Ascend 950DT (`__NPU_ARCH__` = 3510)

> For details about the architecture changes, see [2201-to-3510 Architecture Changes](../zh/guide/跨代迁移兼容性指南/3510架构迁移指导/2201到3510架构变更.md).

---

## New Feature Overview

| No. | New Feature | Corresponding Programming Model and Feature | Corresponding API | Corresponding Operator Practice Material |
|-----|-------------|---------------------------------------------|-------------------|------------------------------------------|
| 1 | **RegBase architecture**: The AIV core Vector computation switches from MemBase to RegBase. Data is moved from UB to the register for computation, and intermediate results are operated directly in the register without writing back to UB. | [Reg Vector Computation Programming](../zh/guide/编程指南/编程模型/AI-Core-SIMD编程/基于Tensor的CPP编程/Reg矢量计算编程.md) | [Reg Vector Computation API List](../zh/api/SIMD-API/基础API/Reg矢量计算/Reg矢量计算.md) | [VF Fusion Optimization](../zh/guide/算子实践参考/SIMD算子性能优化/矢量计算/VF性能优化/VF融合优化.md), [VF Loop Optimization](../zh/guide/算子实践参考/SIMD算子性能优化/矢量计算/VF性能优化/VF循环优化.md) |
| 2 | **SIMT programming model**: A new SIMT hardware unit (DCache, Warp Scheduler, 128 KB Register File) is added to support thread-level parallel programming. | [AI Core SIMT Programming](../zh/guide/编程指南/编程模型/AI-Core-SIMT编程/AI-Core-SIMT编程.md) | [SIMT API List](../zh/api/SIMT-API/概述.md) | [SIMT Operator Implementation](../zh/guide/算子实践参考/SIMT算子实现/基础知识.md), [SIMT Operator Performance Optimization](../zh/guide/算子实践参考/SIMT算子性能优化/内存访问/访存合并.md) |
| 3 | **SIMD and SIMT hybrid programming**: SIMD and SIMT code work together in the same kernel. | [SIMD and SIMT Hybrid Programming](<../zh/guide/编程指南/高级编程/高级AI-Core编程模型/SIMD与SIMT混合编程/概述.md>) | [Hybrid Programming API List](../zh/api/SIMT-API/SIMD与SIMT混合编程简介/API列表-148.md), [Built-in Variables](../zh/api/SIMT-API/SIMD与SIMT混合编程简介/扩展语法/内置变量-145.md) | [Hybrid Programming Operator Implementation](../zh/guide/算子实践参考/SIMD与SIMT混合算子实现/基础知识.md), [Hybrid Programming Performance Optimization](../zh/guide/算子实践参考/SIMD与SIMT混合算子性能优化/内存访问/使用Unified-Buffer提升内存访问效率.md) |
| 4 | **HiF8 (hifloat8_t) data type**: The Cube computation unit supports matrix multiplication with the HiF8 data type. | Data types supported by Cube in [3510 Architecture Specifications](../zh/guide/编程指南/高级编程/硬件实现/架构规格/NPU架构版本3510.md) | [Built-in Data Type Description](../zh/api/SIMD-API/基础API/数据结构/内置数据类型.md), [Cast Type Conversion](../zh/api/SIMD-API/基础API/Reg矢量计算/类型转换/Cast.md), [asc_float2hif8 (deprecated)](../zh/api/SIMD-API/C-API/reg/data_type_convert/asc_float2hif8_deprecated.md) | Under development |
| 5 | **UB-to-L1 Buffer path**: Data can be moved directly from UB to L1 Buffer without going through GM. | [3510 Architecture Changes - Move Unit](../zh/guide/跨代迁移兼容性指南/3510架构迁移指导/2201到3510架构变更.md) | Newly supported [UB-to-L1 Buffer Data Move](../zh/api/SIMD-API/基础API/矩阵计算（ISASI）/矩阵计算的搬入/DataCopy（UBToL1连续数据搬运）.md) | Under development |
| 6 | **L0C-to-UB path**: Data can be moved directly from L0C Buffer to UB without going through GM. | [3510 Architecture Changes - Move Unit](../zh/guide/跨代迁移兼容性指南/3510架构迁移指导/2201到3510架构变更.md) | [asc_copy_l0c2ub](../zh/api/SIMD-API/C-API/cube_datamove/asc_copy_l0c2ub.md) | [Matrix Multiplication Result Accumulation](../zh/guide/算子实践参考/SIMD算子性能优化/矩阵计算/通过L0C-Buffer数据暂存实现高效的矩阵乘结果累加.md) |
| 7 | **ND-DMA move instruction**: Extends the DataCopy capability to freely configure the dimension information and stride of the data to be moved in. | [3510 Architecture Changes - Move Unit](../zh/guide/跨代迁移兼容性指南/3510架构迁移指导/2201到3510架构变更.md) | [Multi-dimensional Data Move ISASI](../zh/api/SIMD-API/基础API/Memory矢量计算/数据搬运/DataCopy（GMToUB多维数据搬运NDDMA）.md), [asc_ndim_copy_gm2ub](../zh/api/SIMD-API/C-API/vector_datamove/asc_ndim_copy_gm2ub.md) | [Reducing Move Count in Non-contiguous Move Scenarios](../zh/guide/算子实践参考/SIMD算子性能优化/内存访问/非连续搬运场景减少搬运次数.md) |
| 8 | **SSBuffer inter-core storage**: A new intra-core storage unit that supports access by AIC cores and AIV cores through Scalar. | Under development | Not applicable | Under development |
| 9 | **Mutex synchronization**: Synchronization between intra-core asynchronous pipeline instructions, similar to the lock mechanism in CPUs. | Under development | [Mutex ISASI](<../zh/api/SIMD-API/基础API/同步控制/核内同步/Mutex(ISASI).md>), [asc_lock](../zh/api/SIMD-API/C-API/sync/asc_lock.md) | Under development |
| 10 | **CrossCore inter-core synchronization**: AIV0 and AIV1 can independently trigger AIC waiting. | Under development | [CrossCoreSetFlag](<../zh/api/SIMD-API/基础API/同步控制/核间同步/CrossCoreSetFlag(ISASI).md>), [CrossCoreWaitFlag](<../zh/api/SIMD-API/基础API/同步控制/核间同步/CrossCoreWaitFlag(ISASI).md>) | [CV Fusion Operator Implementation](../zh/guide/算子实践参考/SIMD算子实现/融合算子编程/CV融合/算子实现.md) |
| 11 | **MX (MicroScaling) data type**: Supports FP8_E4M3/MXFP4/8 low-bit matrix operations, halving memory usage and doubling compute throughput. | LoadData extension in [3510 Architecture Changes - Move Unit](../zh/guide/跨代迁移兼容性指南/3510架构迁移指导/2201到3510架构变更.md) | [asc_mmad_mx](../zh/api/SIMD-API/C-API/cube_compute/asc_mmad_mx.md), [asc_copy_l12l0a_mx](../zh/api/SIMD-API/C-API/cube_datamove/asc_copy_l12l0a_mx.md), [asc_copy_l12l0b_mx](../zh/api/SIMD-API/C-API/cube_datamove/asc_copy_l12l0b_mx.md) | [Matmul Feature Introduction](../zh/guide/算子实践参考/SIMD算子实现/矩阵编程（高阶API）/特性场景/Matmul特性介绍.md), [MxMatmul Scenario](../zh/guide/算子实践参考/SIMD算子实现/矩阵编程（高阶API）/特性场景/MxMatmul场景.md), [Matrix Multiplication Quantization/Dequantization](../zh/guide/算子实践参考/SIMD算子实现/矩阵编程（高阶API）/特性场景/矩阵乘输出的量化-反量化.md) |
| 12 | **Fixpipe enhancement**: Adds NZ2DN in-flight conversion, enabling Fixpipe to convert NZ-format data to DN format in flight. | [3510 Architecture Changes - Move Unit](../zh/guide/跨代迁移兼容性指南/3510架构迁移指导/2201到3510架构变更.md) | [Fixpipe API](../zh/api/SIMD-API/基础API/矩阵计算（ISASI）/矩阵计算的搬出/Fixpipe（L0C到GM数据搬运）.md), [asc_copy_l0c2gm](../zh/api/SIMD-API/C-API/cube_datamove/asc_copy_l0c2gm/asc_copy_l0c2gm_arch_3510.md), [asc_copy_l0c2l1](../zh/api/SIMD-API/C-API/cube_datamove/asc_copy_l0c2l1/asc_copy_l0c2l1_arch_3510.md) | [Matrix Multiplication Result Accumulation](../zh/guide/算子实践参考/SIMD算子性能优化/矩阵计算/通过L0C-Buffer数据暂存实现高效的矩阵乘结果累加.md) |
| 13 | **UB interconnect/bank structure change**: In the 3510 architecture, the UB changes from 16 bank groups (each with three 4 KB banks) to 8 bank groups (each with two 16 KB banks). | [3510 Architecture Specifications - Storage Unit](../zh/guide/编程指南/高级编程/硬件实现/架构规格/NPU架构版本3510.md) | Not applicable | [Avoiding UB Bank Conflicts Overview](../zh/guide/算子实践参考/SIMD算子性能优化/内存访问/避免UB的bank冲突/概述.md), [2201 Bank Conflict](../zh/guide/算子实践参考/SIMD算子性能优化/内存访问/避免UB的bank冲突/避免bank冲突（NPU架构版本2201）.md), [3510 Bank Conflict](../zh/guide/算子实践参考/SIMD算子性能优化/内存访问/避免UB的bank冲突/避免bank冲突（NPU架构版本3510）.md), [SIMT Avoiding Bank Conflicts](../zh/guide/算子实践参考/SIMT算子性能优化/内存访问/避免UB的Bank冲突.md) |

---

## Material Status Description

| Status Label | Description |
|--------------|-------------|
| (With link) | The material for this feature already exists in the docs directory. |
| **Under development** | No dedicated material is available for this feature yet. It will be added later. |
| **Not applicable** | This feature does not involve related material, and no material is planned. |

---

## Related Documents

- [Cross-Generation Migration Compatibility Guide - Overview](../zh/guide/跨代迁移兼容性指南/概述.md)
- [2201-to-3510 Architecture Changes](../zh/guide/跨代迁移兼容性指南/3510架构迁移指导/2201到3510架构变更.md)
- [Basic API Migration Guide](../zh/guide/跨代迁移兼容性指南/3510架构迁移指导/2201迁移3510指导/基础API迁移指导.md)
- [High-level API Migration Guide](../zh/guide/跨代迁移兼容性指南/3510架构迁移指导/2201迁移3510指导/高阶API迁移指导.md)
- [Operator Compilation Migration Guide](../zh/guide/跨代迁移兼容性指南/3510架构迁移指导/2201迁移3510指导/算子编译迁移指导.md)
- [NPU Architecture Version 3510 Specifications](../zh/guide/编程指南/高级编程/硬件实现/架构规格/NPU架构版本3510.md)
