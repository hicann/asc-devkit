# 高级极限打磨

本节基于C++ Tensor编程范式，在上文中级性能优化的基础上，进一步介绍Matmul基础API算子的高级极限打磨方法，通过5个高级优化步骤（大包搬运、LoadData3D替代LoadData2D、常量Tiling、UnitFlag优化、L2Cache优化），深入挖掘硬件性能极限。 

## 优化路径：

| 编号 | 优化手段 | 等级 | 定位 |
|------|---------|------|------|
| 1 | [L1/L0双缓冲Ping-Pong布局](../../../../../../../examples/01_simd_cpp_api/05_best_practices/01_matrix_compute/matmul_basic_api_high_performance/README.md#1-l1l0-双缓冲-ping-pong-布局) | 中级 | 流水线骨架 |
| 2 | [细粒度流水同步](../../../../../../../examples/01_simd_cpp_api/05_best_practices/01_matrix_compute/matmul_basic_api_high_performance/README.md#2-细粒度流水同步) | 中级 | 流水线骨架 |
| 3 | [多核并行切分](../../../../../../../examples/01_simd_cpp_api/05_best_practices/01_matrix_compute/matmul_basic_api_high_performance/README.md#3-多核并行切分) | 中级 | 并行策略 |
| 4 | [DataCopyIn优先搬运B矩阵](../../../../../../../examples/01_simd_cpp_api/05_best_practices/01_matrix_compute/matmul_basic_api_high_performance/README.md#4-datacopyin-优先搬运-b-矩阵) | 中级 | 调度策略 |
| **5** | **[大包搬运](../../../../../../../examples/01_simd_cpp_api/05_best_practices/01_matrix_compute/matmul_basic_api_high_performance/README.md#5-大包搬运)** | **高级** | **流水线骨架** |
| **6** | **[LoadData3D替代LoadData2D](../../../../../../../examples/01_simd_cpp_api/05_best_practices/01_matrix_compute/matmul_basic_api_high_performance/README.md#6-loaddata3d-替代-loaddata2d减少指令队列占用)** | **高级** | **指令队列优化** |
| **7** | **[常量Tiling](../../../../../../../examples/01_simd_cpp_api/05_best_practices/01_matrix_compute/matmul_basic_api_high_performance/README.md#7-常量-tiling)** | **高级** | **编译期优化** |
| **8** | **[UnitFlag优化](../../../../../../../examples/01_simd_cpp_api/05_best_practices/01_matrix_compute/matmul_basic_api_high_performance/README.md#8-unitflag-优化)** | **高级** | **MMAD-FIXPIPE流水重叠** |
| **9** | **[L2Cache优化](../../../../../../../examples/01_simd_cpp_api/05_best_practices/01_matrix_compute/matmul_basic_api_high_performance/README.md#9-l2cache-优化)** | **高级** | **MTE2带宽压榨** |

## 从中级到高级

[中级性能优化](./intermediate_optimization.md)通过L1/L0双缓冲、细粒度流水同步、多核并行切分、DataCopyIn优先搬运B矩阵四个步骤，构建了基本的流水线骨架和并行计算框架，实现了MTE2、MTE1、Cube、Fixpipe四个硬件单元的流水并行。

在此基础上，下列**性能瓶颈**仍有进一步优化空间——这也是高级极限打磨的切入点：

| 瓶颈 | 中级优化后的遗留问题 | 对应的高级优化 |
|------|---------------------|---------------|
| **MTE2搬运粒度小** | 虽已建立三级流水，但每次DataCopyIn仅搬入一个 `baseM×baseK` 块，MTE2指令发射次数多 | 大包搬运 → 一次搬入 `stepKa` 个block，减少发射次数 |
| **MTE1指令队列压力大** | LoadData2D需按 `CUBE_BLOCK`（16B）粒度循环发射多条指令，MTE1队列深度仅32，容易成为瓶颈 | LoadData3D → 单条指令完成整个base切片搬运 |
| **Scalar运行时开销** | Tiling参数在kernel内用Scalar动态计算，增加流水线停顿 | 常量Tiling → 模板参数编译期确定，消除Scalar开销 |
| **Cube/Fixpipe串行** | Mmad与Fixpipe指令级同步，Cube需等待结果全部写回才能开始下一轮计算 | UnitFlag → 512B粒度MMAD-FIXPIPE流水重叠 |
| **MTE2带宽瓶颈** | 中级优化多核并行后，B矩阵被多核反复从GM读取，MTE2成为性能瓶颈 | L2Cache优化 → B矩阵跨轮驻留L2Cache，减少GM访问 |

## 详细内容

详细内容请参考 [Matmul 基础API最佳实践样例](../../../../../../../examples/01_simd_cpp_api/05_best_practices/01_matrix_compute/matmul_basic_api_high_performance)，其中高级极限打磨涵盖5个核心步骤：

- 5、**[大包搬运](../../../../../../../examples/01_simd_cpp_api/05_best_practices/01_matrix_compute/matmul_basic_api_high_performance/README.md#5-大包搬运)**：通过 `stepKa`/`stepKb` 参数将多个基本块打包为一次DataCopyIn，减少MTE2指令发射次数
- 6、**[LoadData3D替代 LoadData2D](../../../../../../../examples/01_simd_cpp_api/05_best_practices/01_matrix_compute/matmul_basic_api_high_performance/README.md#6-loaddata3d-替代-loaddata2d减少指令队列占用)**：使用 `LoadData3DParamsV2` 单条指令替代LoadData2D的多条循环发射，降低MTE1指令队列占用
- 7、**[常量Tiling](../../../../../../../examples/01_simd_cpp_api/05_best_practices/01_matrix_compute/matmul_basic_api_high_performance/README.md#7-常量-tiling)**：所有Tiling参数通过模板参数在编译期确定，消除运行时Scalar动态计算开销
- 8、**[UnitFlag优化](../../../../../../../examples/01_simd_cpp_api/05_best_practices/01_matrix_compute/matmul_basic_api_high_performance/README.md#8-unitflag-优化)**：开启UnitFlag实现MMAD-FIXPIPE 512B 粒度流水并行，Cube计算与结果写回重叠执行
- 9、**[L2Cache优化](../../../../../../../examples/01_simd_cpp_api/05_best_practices/01_matrix_compute/matmul_basic_api_high_performance/README.md#9-l2cache-优化)**：通过M轴分轮调度使B矩阵跨轮驻留 L2Cache，缓解MTE2带宽压力

每步优化均包含原理说明与数据流向说明，并附有功能调试（printf、DumpTensor）与性能调试（msOpProf）方法说明。

> **说明**：中级优化内容请参考 [中级性能优化](./intermediate_optimization.md)，上述优化步骤5-9属于高级极限打磨范畴，建议在中级优化完成后逐步叠加。性能数据与理论分析详见README的 [性能数据分析](../../../../../../../examples/01_simd_cpp_api/05_best_practices/01_matrix_compute/matmul_basic_api_high_performance/README.md#性能数据分析) 及 [理论性能分析](../../../../../../../examples/01_simd_cpp_api/05_best_practices/01_matrix_compute/matmul_basic_api_high_performance/README.md#理论性能分析) 章节。
