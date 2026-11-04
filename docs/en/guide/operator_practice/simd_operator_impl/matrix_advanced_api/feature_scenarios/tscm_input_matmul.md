# Cube Multiplication with TSCM Input<a name="ZH-CN_TOPIC_0000002500468244"></a>

<!-- md-trans-meta sourceCommit=dd293a1c6b873d149bf289bb2b8098dadf9f5cbd translatedAt=2026-10-23T12:13:22.402Z pushedAt=2026-11-03T07:05:13.308Z nl=2 -->

## Feature Overview<a name="zh-cn_topic_0000002298654821_section310824820358"></a>

TSCM refers to the logical memory corresponding to the L1 Buffer space. For details about the L1 Buffer, see [Storage Unit](../../../../programming_guide/advanced_programming/hardware_implementation/basic_architecture.md#section123639375417). Developers can manage TSCM on their own to efficiently utilize hardware resources. For example, developers can cache a copy of TSCM data and flexibly configure it as the Cube A, cube B, or Bias cube of a Matmul operation in different scenarios, thereby achieving memory reuse and computational efficiency optimization. In the TSCM input scenario, users manage the entire TSCM memory space, and Matmul directly uses the passed-in TSCM memory address without moving data from Global Memory to TSCM.

## Scenarios<a name="zh-cn_topic_0000002298654821_section118051016163613"></a>

This applies to scenarios where users need to customize data movement into TSCM and manage it themselves, that is, where they need to implement the data movement function, such as non-contiguous data movement or preprocessing of the moved data. By customizing TSCM management, users can flexibly configure the MTE2 pipeline to implement global [DoubleBuffer](../../../../technical_appendix/concepts_and_terms/performance_optimization/double_buffer.md) across Matmul objects. For details about MTE2, see [Data Movement Unit](../../../../programming_guide/advanced_programming/hardware_implementation/basic_architecture.md#section123639375417).

## Constraints<a name="zh-cn_topic_0000002298654821_section14160134220363"></a>

Cube A set as TSCM input must be fully loaded in TSCM. "Fully loaded" means that all cube data is copied into and kept in TSCM at the same time.

## Example<a name="zh-cn_topic_0000002298654821_section15486294368"></a>

For the complete operator sample, see [Matmul Operator Sample with TSCM Input from GM as the Custom Data Source](../../../../../../../examples/01_simd_cpp_api/04_advanced_api/00_matmul/matmul_tscm).

```
TQue<TPosition::A1, 1> scm; // Queue logical position A1, queue depth 1
pipe->InitBuffer(scm, 1, tiling.M * tiling.Ka * sizeof(A_T)); 
// The TPosition of A_TYPE is TSCM, and the TPosition of B_TYPE is GM
Matmul<A_TYPE, B_TYPE, C_TYPE, BIAS_TYPE> mm1;
REGIST_MATMUL_OBJ(&pipe, GetSysWorkSpacePtr(), mm1);
mm1.Init(&tiling);
// Customize the data movement of cube A from GM to TSCM
auto scmTensor = scm.AllocTensor<A_T>();
DataCopy(scmTensor, gm_a, tiling.M * tiling.Ka);
scm.EnQue(scmTensor);
LocalTensor<A_T> scmLocal = scm.DeQue<A_T>();
// Set cube A as TSCM input and cube B as GM input
mm1.SetTensorA(scmLocal);
mm1.SetTensorB(gm_b);
mm1.SetBias(gm_bias);
mm1.IterateAll(gm_c);
scm.FreeTensor(scmLocal);
```
