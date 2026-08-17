# 中级性能优化

本节基于C++ Tensor编程范式，介绍Matmul基础API算子的性能调优方法，通过4个渐进式优化步骤（L1/L0双缓冲机制、细粒度流水同步、多核并行切分、DataCopyIn优先搬运B矩阵），展示从基础实现到高性能优化的完整调优路径。

## 优化路径：

| 编号 | 优化手段 | 等级 | 定位 |
|------|---------|------|------|
| **1** | **[L1/L0双缓冲Ping-Pong布局](../../../../../../../examples/01_simd_cpp_api/05_best_practices/01_matrix_compute/matmul_basic_api_high_performance/README.md#1-l1l0-双缓冲-ping-pong-布局)** | **中级** | **流水线骨架** |
| **2** | **[细粒度流水同步](../../../../../../../examples/01_simd_cpp_api/05_best_practices/01_matrix_compute/matmul_basic_api_high_performance/README.md#2-细粒度流水同步)** | **中级** | **流水线骨架** |
| **3** | **[多核并行切分](../../../../../../../examples/01_simd_cpp_api/05_best_practices/01_matrix_compute/matmul_basic_api_high_performance/README.md#3-多核并行切分)** | **中级** | **并行策略** |
| **4** | **[DataCopyIn优先搬运B矩阵](../../../../../../../examples/01_simd_cpp_api/05_best_practices/01_matrix_compute/matmul_basic_api_high_performance/README.md#4-datacopyin-优先搬运-b-矩阵)** | **中级** | **调度策略** |
| 5 | [大包搬运](../../../../../../../examples/01_simd_cpp_api/05_best_practices/01_matrix_compute/matmul_basic_api_high_performance/README.md#5-大包搬运) | 高级 | 流水线骨架 |
| 6 | [LoadData3D替代LoadData2D](../../../../../../../examples/01_simd_cpp_api/05_best_practices/01_matrix_compute/matmul_basic_api_high_performance/README.md#6-loaddata3d-替代-loaddata2d减少指令队列占用) | 高级 | 指令队列优化 |
| 7 | [常量Tiling](../../../../../../../examples/01_simd_cpp_api/05_best_practices/01_matrix_compute/matmul_basic_api_high_performance/README.md#7-常量-tiling) | 高级 | 编译期优化 |
| 8 | [UnitFlag优化](../../../../../../../examples/01_simd_cpp_api/05_best_practices/01_matrix_compute/matmul_basic_api_high_performance/README.md#8-unitflag-优化) | 高级 | MMAD-FIXPIPE流水重叠 |
| 9 | [L2Cache优化](../../../../../../../examples/01_simd_cpp_api/05_best_practices/01_matrix_compute/matmul_basic_api_high_performance/README.md#9-l2cache-优化) | 高级 | MTE2带宽压榨 |

## 从入门到优化

[入门功能落地](./getting_started.md)实现了Matmul算子的基本功能：A/B 矩阵经 MTE2 的 `DataCopy` 从GM搬入L1（ND→NZ转换），经MTE1的 `LoadData` 搬入L0A/L0B（NZ→ZZ/ZN 转换），Cube单元执行 `Mmad`，结果经 `Fixpipe` 从L0C写回GM。

入门实现存在以下**性能瓶颈**——这也是中级优化的起点：

| 瓶颈 | 入门实现的问题 | 对应的中级优化 |
|------|---------------|---------------|
| **L1/L0 单缓冲区** | A、B矩阵在L1和L0中各只有一份缓冲区，下一轮数据必须等上一轮消费完才能搬入 | 改为Ping-Pong双缓冲，让数据搬运和计算重叠执行 |
| **流水线串行执行** | 当前实现中使用PipeBarrier和SetFlag/WaitFlag进行流水线同步，但未采用双缓冲机制，MTE2、MTE1、M、FIX各硬件单元无法充分流水并行 | 通过双缓冲和硬件事件同步，使四个单元流水并行，显著提升吞吐 |
| **多核切分固定** | 硬编码 `M / singleCoreM` 计算核数分配，仅适用于固定矩阵规模和核数场景 | 通用切分策略，支撑任意核数和矩阵规模的自动分配 |
| **B矩阵搬运等待** | A和B依次搬运，B数据到位晚导致L1缓冲区欠载 | 优先搬B → 先B后 调度 |
| MTE2单条搬运指令粒度小 | DataCopyIn每次搬入一个 `baseM×baseK` 块，MTE2指令发射次数多 | 大包搬运 → 一次搬入 `stepKa` 个block |
| MTE1指令队列压力大 | LoadData2D一次只能搬一个16×K切片，需for循环多次发射 | LoadData3D → 单条指令完成搬运 |
| Tiling参数运行时计算 | 所有参数在kernel内用Scalar计算，增加流水线停顿 | 常量Tiling → 编译期确定 |
| L0C写回阻塞 | `Fixpipe` 与Mmad串行，Cube 等待写回完成再计算 | UnitFlag → 512B粒度MMAD-FIXPIPE重叠 |
| L2Cache未充分利用 | 当前实现中B矩阵每次均从GM读取，多核反复访问GM导致MTE2带宽压力大；L2Cache纯读带宽约3~4倍于GM，但当前实现未能利用这一特性 | 沿M方向切分A矩阵，使B矩阵跨轮驻留L2Cache，减少GM访问次数，有效降低MTE2耗时 |

## 详细内容

详细内容请参考 [Matmul 基础API最佳实践样例](../../../../../../../examples/01_simd_cpp_api/05_best_practices/01_matrix_compute/matmul_basic_api_high_performance)，其中涵盖中级性能优化的4个核心步骤：

- 1、**[L1/L0 双缓冲 Ping-Pong 布局](../../../../../../../examples/01_simd_cpp_api/05_best_practices/01_matrix_compute/matmul_basic_api_high_performance/README.md#1-l1l0-双缓冲-ping-pong-布局)**：DataCopyIn（GM→L1）、DataLoad（L1→L0）与Compute三个阶段形成三级流水
- 2、**[细粒度流水同步](../../../../../../../examples/01_simd_cpp_api/05_best_practices/01_matrix_compute/matmul_basic_api_high_performance/README.md#2-细粒度流水同步)**：四类硬件事件（MTE2_MTE1、MTE1_MTE2、MTE1_M、M_MTE1）精确控制流水线
- 3、**[多核并行切分](../../../../../../../examples/01_simd_cpp_api/05_best_practices/01_matrix_compute/matmul_basic_api_high_performance/README.md#3-多核并行切分)**：M/N方向均匀切分，支撑任意核数和矩阵规模的自动分配
- 4、**[DataCopyIn 优先搬运 B 矩阵](../../../../../../../examples/01_simd_cpp_api/05_best_practices/01_matrix_compute/matmul_basic_api_high_performance/README.md#4-datacopyin-优先搬运-b-矩阵)**：先B后A调度，缓解L1缓冲区欠载

每步优化均包含原理说明与数据流向说明，并附有功能调试（printf、DumpTensor）与性能调试（msOpProf）方法说明。

> **说明**：该样例中L2Cache切分的性能分析属于高级极限打磨内容，其具体实现原理与性能数据请参考[高级极限打磨](./advanced_extreme_optimization.md)，不在本中级性能优化文档中展开。
