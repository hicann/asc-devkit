# 中级性能优化<a name="ZH-CN_TOPIC_000000_MATMUL_LEAKYRELU_MID"></a>

本节在入门功能落地的基础上，介绍Matmul融合算子的中级性能优化方法。

> **说明**：入门功能落地以Matmul+LeakyRelu融合算子作为讲解样例，而中级和高阶极限打磨以Matmul+Gelu融合算子作为讲解样例。为更好展示VF融合的性能优化手段，本章节以Matmul+Gelu融合算子作为讲解样例。GELU相比LeakyRelu计算逻辑更复杂，能更充分地体现各类优化手段的效果。

## 优化手段总览

| 编号 | 优化手段 | 说明 | 定位 |
|------|---------|------|------|
| 1 | L1/L0双缓冲Ping-Pong布局 | Cube核内MTE2搬运和MTE1/Mmad计算并行 | 流水线骨架 |
| 2 | 大包搬运 | 增大搬运粒度，减少搬运次数 | 流水线骨架 |
| 3 | 细粒度流水同步 | SetFlag/WaitFlag替代PipeBarrier实现更细粒度同步 | 流水线骨架 |
| 4 | LoadData3D替代LoadData2D | 减少MTE1指令队列占用 | 指令队列优化 |
| 5 | 多核并行切分 | 按M/N方向切分到多核并行计算 | 并行策略 |
| 6 | 常量Tiling | Tiling参数编译期确定，减少运行时Scalar计算开销 | 编译期优化 |
| 7 | UnitFlag优化 | Mmad与Fixpipe细粒度流水重叠 | MMAD-FIXPIPE流水重叠 |

## CV融合：Scenario 1 — CV融合-GM中转

中级性能优化采用CV融合-GM中转方式（A2/A3/Ascend 950PR均支持）。AIC侧通过Fixpipe将L0C计算结果写入GM，AIV侧从GM读取数据到UB后进行GELU计算，再将结果写回GM。

数据流路径：
```
GM ──(MTE2)──> L1 ──(MTE1)──> L0A/L0B ──(Cube)──> L0C ──(Fixpipe)──> GM ──(MTE2)──> UB ──(Gelu)──> UB ──(MTE3)──> GM
```

### Matmul优化手段

| Case | 优化手段 | 说明 |
|------|---------|------|
| [Case 1](../../../../../../../examples/01_simd_cpp_api/05_best_practices/01_matrix_compute/matmul_high_performance/README.md#case-1-单核tiling优化-single_core_tiling) | 单核Tiling优化 | 优化base块参数，降低访存计算比 |
| [Case 2](../../../../../../../examples/01_simd_cpp_api/05_best_practices/01_matrix_compute/matmul_high_performance/README.md#case-2-多核切分-2x12-multi_core_split_2_12) | 多核切分 2x12 | 引入多核并行计算，均衡负载分配 |
| [Case 3](../../../../../../../examples/01_simd_cpp_api/05_best_practices/01_matrix_compute/matmul_high_performance/README.md#case-3-多核切分-4x6-multi_core_split_4_6) | 多核切分 4x6 | 优化多核切分策略，地址512B对齐 |
| [Case 4](../../../../../../../examples/01_simd_cpp_api/05_best_practices/01_matrix_compute/matmul_high_performance/README.md#case-4-多核使用mdl模板-multi_core_mdl) | 多核使用MDL模板 | 启用L1多块缓存，大包搬运 |

### Gelu优化手段

| Case | 优化手段 | 说明 |
|------|---------|------|
| [Case 1](../../../../../../../examples/01_simd_cpp_api/05_best_practices/02_reg_compute/gelu_high_performance/README.md#case-1-启用regbase-api和vf融合能力) | RegBase API + VF融合 | 寄存器级计算减少中间Load/Store，利用双发特性提升IPC |

### 性能数据

以Ascend 950PR芯片数据为例：
- 单独Gelu任务耗时348.868μs，单独Matmul任务耗时2601.311μs
- Scenario 1融合后总耗时2606.91μs，与Matmul单独执行时间基本一致

这表明采用CV融合方式编写时，AIV的Vector计算时间基本被AIC的Cube计算时间完全覆盖，实现了高效的流水并行。

详细内容请参见[Matmul+Gelu中级性能优化](../../../../../../../examples/01_simd_cpp_api/05_best_practices/03_fusion_compute/matmul_gelu_high_performance/README.md)。
