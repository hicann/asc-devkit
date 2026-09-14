# Matrix Multiplication with TSCM Input<a name="ZH-CN_TOPIC_0000002500468244"></a>

<!-- md-trans-meta sourceCommit=d1fe9e831a69eaae67fc1f463ea1c4ea17a8f417 translatedAt=2026-08-26T13:44:41.930Z -->

## Description<a name="zh-cn_topic_0000002298654821_section310824820358"></a>

TSCM refers to the logical memory corresponding to the L1 Buffer space. For details about the L1 Buffer, see [Storage Unit](../../../../programming_guide/advanced_programming/hardware_implementation/basic_architecture.md#section123639375417). Developers can manage TSCM on their own to efficiently utilize hardware resources. For example, a developer can cache a copy of TSCM data and flexibly configure it as the A matrix, B matrix, or Bias matrix of a Matmul operation in different scenarios, thereby achieving memory reuse and optimized computing efficiency. In the TSCM input scenario, the user manages the entire TSCM memory space, and Matmul directly uses the passed-in TSCM memory address without moving data from Global Memory to TSCM.

## Scenarios<a name="zh-cn_topic_0000002298654821_section118051016163613"></a>

This applies to scenarios where users need to customize data movement into TSCM and manage it themselves, that is, they need to implement the data movement function, such as non-contiguous movement or preprocessing of the moved data. By customizing TSCM management, users can flexibly configure the MTE2 pipeline to implement global [DoubleBuffer](../../../../technical_appendix/concepts_and_terms/performance_optimization/DoubleBuffer.md) across Matmul objects. For details about MTE2, see [Move Unit](../../../../programming_guide/advanced_programming/hardware_implementation/basic_architecture.md#section123639375417).

## Constraints<a name="zh-cn_topic_0000002298654821_section14160134220363"></a>

A matrix configured as TSCM input must be fully loaded in TSCM. "Fully loaded" means that all matrix data is moved into and retained in TSCM at the same time.

## Calling Example<a name="zh-cn_topic_0000002298654821_section15486294368"></a>

For the complete operator sample, see [Matmul Operator Sample with TSCM Input and Custom Data Source from GM](https://gitcode.com/cann/asc-devkit/tree/9.1.0/examples/01_simd_cpp_api/04_advanced_api/00_matmul/matmul_tscm).

```
TQue<TPosition::A1, 1> scm; // Queue logical position A1, queue depth 1
pipe->InitBuffer(scm, 1, tiling.M * tiling.Ka * sizeof(A_T)); 
// The TPosition of A_TYPE is TSCM, and the TPosition of B_TYPE is GM
Matmul<A_TYPE, B_TYPE, C_TYPE, BIAS_TYPE> mm1;
REGIST_MATMUL_OBJ(&pipe, GetSysWorkSpacePtr(), mm1);
mm1.Init(&tiling);
// Customize the data movement of matrix A from GM to TSCM
auto scmTensor = scm.AllocTensor<A_T>();
DataCopy(scmTensor, gm_a, tiling.M * tiling.Ka);
scm.EnQue(scmTensor);
LocalTensor<A_T> scmLocal = scm.DeQue<A_T>();
// Set matrix A as TSCM input and matrix B as GM input
mm1.SetTensorA(scmLocal);
mm1.SetTensorB(gm_b);
mm1.SetBias(gm_bias);
mm1.IterateAll(gm_c);
scm.FreeTensor(scmLocal);
```
