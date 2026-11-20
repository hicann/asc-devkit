# Entry-Level Feature Implementation<a name="ZH-CN_TOPIC_000000_MATMUL_LEAKYRELU_ENTRY"></a>

<!-- md-trans-meta sourceCommit=b6142b1cb0ae3dfa4b9063f8bd59db99b19fc426 translatedAt=2026-11-09T10:43:49.264Z pushedAt=2026-11-19T06:32:19.841Z nl=2 -->

This section uses the Matmul+LeakyRelu fused operator as an example to describe the basic method of implementing cube-vector fused computing based on the Ascend C programming model, helping developers quickly get started with fused operator development.

## Operator Overview

The Matmul+LeakyRelu fused computing formula is as follows:

$$
C_{ij} = \begin{cases}
\sum_{k} A_{ik} \times B_{kj} & \text{if } \sum_{k} A_{ik} \times B_{kj} \geq 0 \\
0.001 \times \sum_{k} A_{ik} \times B_{kj} & \text{if } \sum_{k} A_{ik} \times B_{kj} < 0
\end{cases}
$$

- A is the left cube with a shape of [M, K] and a data type of half; B is the right cube with a shape of [K, N] and a data type of half
- C is the output cube with a shape of [M, N] and a data type of half

## Operator Implementation

This sample implements federated computing of Matmul and LeakyRelu based on the C++ tensor programming mode, demonstrating the programming mode in which the Cube unit (cube computing unit) and the Vector unit (vector computing unit) work collaboratively. The Cube core performs cube multiplication, and the Vector core performs LeakyRelu activation computation.

**Data flow path**:

Cube side:

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
          ND->Nz              Nz->Zn(transpose)
```

Inter-core synchronization and vector side:

```
Cube core:  Fixpipe -> GM(C:ND,half)
                    │
                    │ CrossCoreSetFlag / CrossCoreWaitFlag
                    ▼
Vector core: GM(C:ND,half) -> UB(VECCALC,half) -> UB(VECCALC,half) -> GM(C:ND,half)
                      │                   │                   │
                  DataCopyPad          LeakyRelu          DataCopyPad
            MTE2 transfer (baseM/2×baseN)    VEC compute            MTE3 write out
```

**Kernel declaration**: Use `__global__ __mix__(1, 2)` to declare a hybrid kernel function. One AIC (Cube) and two AIVs (vector) form a logical core, with a Cube:Vector core ratio of 1:2.

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

For the complete sample code, see the [Matmul+LeakyRelu getting-started sample](../../../../../../../examples/01_simd_cpp_api/00_introduction/03_fusion_operation/matmul_leakyrelu_basic_api).

## Optimization Direction Analysis

| Sequence Number | Optimization Direction | Problem in the Current Implementation | Expected Optimization Benefit |
|------|-----------|--------------|------------|
| 1 | L1/L0 double-buffering pipeline | In the Cube core, the GM→L1→L0A/L0B→L0C→GM stages are executed serially | Ping-Pong double buffering enables MTE2 data movement and MTE1/Mmad computation to run in parallel, greatly improving throughput |
| 2 | Fine-grained parallelism between Fixpipe and Mmad | Mmad and Fixpipe are executed serially on the whole block | Split into smaller-grained blocks so that Mmad and Fixpipe execute alternately, reducing pipeline bubbles |
| 3 | UB double buffering | In the Vector core, GM→UB→computation→GM is executed serially | UB Ping-Pong double buffering enables MTE2 data movement and VEC computation to run in parallel |
| 4 | Vector core VF fusion | The MemBase API is currently used, and intermediate results must be written back to UB | Use RegBase + asc_vf_call for VF fusion so that intermediate computation is completed in registers |
| 5 | Large-packet data movement | Currently only a single baseM×baseN block is moved | Increase singleCoreM/singleCoreN to reduce the number of data movement operations |
| 6 | GM relay optimization | Cube core results must be relayed to the Vector core through GM (L0C→GM→UB) | Ascend 950PR supports Fixpipe writing directly to UB (L0C→UB), eliminating the GM relay overhead |
