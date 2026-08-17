# 中级性能优化<a name="ZH-CN_TOPIC_000000_GELU_MID"></a>

本节在入门功能落地的基础上，介绍GELU算子的中级性能优化方法。

## 性能瓶颈分析

基于入门级代码的Profiling数据，核心性能瓶颈如下：

| 指标 | 数值 | 占比 |
|:---|:---|:---|
| Task Duration | 351.525μs | - |
| aiv_vec_time | 147.895μs | 42.2% |
| aiv_scalar_time | 9.513μs | 2.7% |
| aiv_mte2_time | 320.283μs | 91.3% |
| aiv_mte3_time | 303.647μs | 86.6% |

**核心瓶颈解读**：

- **痛点1：向量计算开销占比高**：每次基础API向量计算内部会进行load → compute → store操作，数据需在UB和寄存器之间反复交互，8次向量计算产生8次Load/Store开销。
- **痛点2：指令级并行度低**：基础API每条向量指令间需插入`PipeBarrier<PIPE_V>()`同步，无法利用VF融合的双发特性。

## 优化手段：RegBase API + VF融合

**MemBase vs RegBase对比**：

| 维度 | MemBase（基础API） | RegBase（VF融合API） |
|:---|:---|:---|
| 编程接口 | Compute API（如`Mul(yLocal, xLocal, xLocal, n)`） | Reg API（如`Reg::Mul(yReg, xReg, xReg, mask)`） |
| 数据驻留 | 每条指令隐式Load/Store，中间结果写回UB | 数据显式加载到寄存器后常驻，中间结果暂存寄存器 |
| 同步方式 | 每条指令间需`PipeBarrier<PIPE_V>()`同步 | VF函数内无需显式同步，硬件自动管理依赖 |
| 适用场景 | 简单计算、单步或少步运算 | 多步骤融合计算，减少UB读写次数 |
| 性能特征 | 每步Load/Store开销大，IPC低 | 仅1次Load+1次Store，支持双发，IPC高 |

**原理解析**：VF函数内数据从UB加载到寄存器后，所有中间计算均在寄存器中完成，仅需一次LoadAlign和一次StoreAlign，消除中间结果的Load/Store开销；同时支持VF双发特性，常规计算指令并行度可达512 bytes/cycle。

```
基础API模式：                    VF融合模式：
Load → Mul → Store              LoadAlign → Mul
Load → Mul → Store                   → Mul
Load → Muls → Store                   → Muls
Load → Add → Store                    → Add
Load → Muls → Store                   → Muls
Load → Exp → Store                    → Exp
Load → Adds → Store                   → Adds
Load → Div → Store                    → Div → StoreAlign
（8次Load/Store）                 （1次Load/1次Store）
```

## 优化效果

| 指标 | 入门版 | 中级优化版 | 提升幅度 |
|:---|:---:|:---:|:---:|
| Task Duration(μs) | 351.525 | 348.868 | 0.76% |
| aiv_vec_time(μs) | 147.895 | 66.277 | **55.2%** |
| aiv_scalar_time(μs) | 9.513 | 3.03 | **68.2%** |
| IPC | - | 1.20 | - |

> 本样例为MTE2 bound，数据搬运占比超过90%，因此端到端收益不明显，但向量计算耗时的优化效果显著。

详细内容请参见[Gelu性能调优样例 Case 1](../../../../../../../examples/01_simd_cpp_api/05_best_practices/02_reg_compute/gelu_high_performance/README.md#case-1-启用regbase-api和vf融合能力)。
