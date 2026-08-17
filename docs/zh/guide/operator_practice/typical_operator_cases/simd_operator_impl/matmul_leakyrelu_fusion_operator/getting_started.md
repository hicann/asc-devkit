# 入门功能落地<a name="ZH-CN_TOPIC_000000_MATMUL_LEAKYRELU_ENTRY"></a>

本节以Matmul+LeakyRelu融合算子为例，介绍基于Ascend C编程模型实现Cube-Vector融合计算的基本方法，帮助开发者快速入门融合算子开发。

## 算子概述

Matmul+LeakyRelu融合计算公式：

$$
C_{ij} = \begin{cases}
\sum_{k} A_{ik} \times B_{kj} & \text{if } \sum_{k} A_{ik} \times B_{kj} \geq 0 \\
0.001 \times \sum_{k} A_{ik} \times B_{kj} & \text{if } \sum_{k} A_{ik} \times B_{kj} < 0
\end{cases}
$$

- A为左矩阵，形状为[M, K]，数据类型为half；B为右矩阵，形状为[K, N]，数据类型为half
- C为输出矩阵，形状为[M, N]，数据类型为half

## 算子实现

本样例基于C++ Tensor编程模式实现Matmul和LeakyRelu融合计算，展示Cube单元（矩阵计算单元）和Vector单元（向量计算单元）协同计算的编程模式。Cube核完成矩阵乘计算，Vector核完成LeakyRelu激活计算。

**数据流路径**：

Cube侧：
```
GM(A:ND,half) -> L1(A:Nz,half) -> L0A(A:Nz,half) -
            │                  │                 │
          DataCopy            LoadData            │
          ND->Nz              Nz->Zz/Nz           │
                                                    │--->L0C(Nz,float) -> GM(C:ND,half)
                                                    │  │                  │
                                                    │ Mmad             Fixpipe
                                                    │ C=A×B       Nz->ND, float32->half
                                                    │
GM(B:ND,half) -> L1(B:Nz,half) -> L0B(B:Zn,half) -
            │                  │
          DataCopy            LoadData
          ND->Nz              Nz->Zn(转置)
```

核间同步与Vector侧：
```
Cube核:  Fixpipe -> GM(C:ND,half)
                    │
                    │ CrossCoreSetFlag / CrossCoreWaitFlag
                    ▼
Vector核: GM(C:ND,half) -> UB(VECCALC,half) -> UB(VECCALC,half) -> GM(C:ND,half)
                      │                   │                   │
                  DataCopyPad          LeakyRelu          DataCopyPad
            MTE2搬运（baseM/2×baseN）    VEC计算            MTE3写出
```

**核函数声明**：使用`__global__ __mix__(1, 2)`声明混合核函数，1个AIC(Cube) + 2个AIV(Vector)组成一个逻辑核，Cube:Vector核数比例为1:2。

```cpp
__global__ __mix__(1, 2) void mmad_vec_custom(__gm__ uint8_t* xMatrix, __gm__ uint8_t* yMatrix, __gm__ uint8_t* zMatrix)
{
    AscendC::InitSocState();
    KernelMatmul<M, K, N, singleCoreM, singleCoreK, singleCoreN, baseM, baseK, baseN> op;
    op.Init(xMatrix, yMatrix, zMatrix);
    op.Process();
    AscendC::PipeBarrier<PIPE_ALL>();
}
```

完整样例代码请参见[Matmul+LeakyRelu入门样例](../../../../../../../examples/01_simd_cpp_api/00_introduction/03_fusion_operation/matmul_leakyrelu_basic_api)。

## 可优化方向分析

| 序号 | 可优化方向 | 当前实现的问题 | 预期优化收益 |
|------|-----------|--------------|------------|
| 1 | L1/L0双缓冲流水线 | Cube核内GM→L1→L0A/L0B→L0C→GM各阶段串行执行 | Ping-Pong双缓冲使MTE2搬运和MTE1/Mmad计算并行，大幅提升吞吐量 |
| 2 | Fixpipe与Mmad细粒度并行 | Mmad和Fixpipe整块串行 | 拆分为更小粒度的块，Mmad和Fixpipe交替执行，减少流水线气泡 |
| 3 | UB双缓冲 | Vector核内GM→UB→计算→GM串行执行 | UB Ping-Pong双缓冲使MTE2搬运和VEC计算并行 |
| 4 | Vector核VF融合 | 当前使用MemBase API，中间结果需写回UB | 使用RegBase+asc_vf_call进行VF融合，中间计算在寄存器完成 |
| 5 | 大包搬运 | 当前仅搬运单块baseM×baseN数据 | 增大singleCoreM/singleCoreN，减少搬运次数 |
| 6 | GM中转优化 | Cube核结果需经GM中转给Vector核（L0C→GM→UB） | Ascend 950PR支持Fixpipe直接写入UB（L0C→UB），省去GM中转开销 |
