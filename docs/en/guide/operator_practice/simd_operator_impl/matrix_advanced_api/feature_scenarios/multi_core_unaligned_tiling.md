# Multi-Core Unaligned Segmentation<a name="ZH-CN_TOPIC_0000002500548096"></a>

<!-- md-trans-meta sourceCommit=e3c8c70b3f5d27f3770063ae4b535bc75d904e3e translatedAt=2026-08-26T13:35:33.124Z -->

## Description<a name="zh-cn_topic_0000002264134828_section310824820358"></a>

In multi-core scenarios, when a matrix is split and M, N, and K cannot be evenly divided by singleCoreM, singleCoreN, and singleCoreK, tail blocks appear, which is the multi-core unaligned segmentation scenario. The following figure shows the matrix blocks in the last row and last column of matrices A and B:

![](../../../../figures/nd2nz_conversion_diagram_1_new.png)

In this case, the R matrix block in matrix C is still obtained by accumulating A1\*B1+A2\*B2+A3\*B3+A4\*B4. When processing tail blocks such as A1\*B1, A2\*B2, A3\*B3, and A4\*B4, you need to set the tail block size on the kernel side. Without changing the original tiling, call the [SetTail](../../../../../api/SIMD-API/advanced_api/matrix_compute/Matmul-Kernel/SetTail.md) API to reset the singleCoreM/singleCoreN/singleCoreK for the current computation. When processing tail blocks, data is moved and computed according to the set values, that is, tailM/tailN/tailK.

## Scenarios<a name="zh-cn_topic_0000002264134828_section118051016163613"></a>

It is applicable to scenarios where multiple cores process Matmul matrix computation and tail blocks exist.

## Constraints<a name="zh-cn_topic_0000002264134828_section14160134220363"></a>

The SetTail API used to process the tail block must be called before Iterate/IterateAll.

## Calling Example<a name="zh-cn_topic_0000002264134828_section15486294368"></a>

For the complete sample of the Matmul multi-core unaligned scenario, see the [Matmul multi-core unaligned segmentation operator sample](https://gitcode.com/cann/asc-devkit/tree/9.1.0/examples/01_simd_cpp_api/04_advanced_api/00_matmul/matmul_multi_core_unaligned). The key code example of this scenario is as follows.

```
// Process the tail block
int tailM = tiling.M - mCoreIndex * tiling.singleCoreM;
tailM = tailM < tiling.singleCoreM ? tailM : tiling.singleCoreM;
int tailN = tiling.N - nCoreIndex * tiling.singleCoreN;
tailN = tailN < tiling.singleCoreN ? tailN : tiling.singleCoreN;
// When tailM < singleCoreM or tailN < singleCoreN, the tail block is considered to need processing, and the SetTail API can be called for configuration.
if (tailM < tiling.singleCoreM || tailN < tiling.singleCoreN) {
    matmulObj.SetTail(tailM, tailN);
}
```
