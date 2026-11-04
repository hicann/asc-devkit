# Multi-core Unaligned Splitting<a name="ZH-CN_TOPIC_0000002500548096"></a>

<!-- md-trans-meta sourceCommit=dd293a1c6b873d149bf289bb2b8098dadf9f5cbd translatedAt=2026-10-23T12:10:59.083Z pushedAt=2026-11-03T07:05:13.306Z nl=2 -->

## Feature Overview<a name="zh-cn_topic_0000002264134828_section310824820358"></a>

In multi-core scenarios, when a cube is split and M, N, and K cannot be evenly divided by singleCoreM, singleCoreN, and singleCoreK, tail blocks are generated, which is the multi-core unaligned split scenario. The following figure shows the cube blocks in the last row and last column of cubes A and B:

![](../../../../figures/nd2nz_new_1.png)

In this case, the R cube block in cube C is still obtained through A1\*B1+A2\*B2+A3\*B3+A4\*B4 accumulation. When processing tail blocks such as A1\*B1, A2\*B2, A3\*B3, and A4\*B4, you need to set the tail block size on the kernel side. Without changing the original tiling, call the [SetTail](../../../../../api/SIMD-API/adv_api/cube_compute/Matmul_Kernel/SetTail.md) API to reset the singleCoreM/singleCoreN/singleCoreK for this computation. When processing tail blocks, data is moved and computed according to the set values, that is, tailM/tailN/tailK.

## Scenarios<a name="zh-cn_topic_0000002264134828_section118051016163613"></a>

A scenario where tail blocks exist when multiple cores process Matmul cube computation.

## Constraints<a name="zh-cn_topic_0000002264134828_section14160134220363"></a>

The SetTail API used to process the tail block must be called before Iterate/IterateAll.

## Example<a name="zh-cn_topic_0000002264134828_section15486294368"></a>

For the complete sample of the Matmul multi-core unaligned scenario, see [Matmul Multi-core Unaligned Split Operator Sample](../../../../../../../examples/01_simd_cpp_api/04_advanced_api/00_matmul/matmul_multi_core_unaligned). The key code example of this scenario is as follows.

```
// Handle the tail block
int tailM = tiling.M - mCoreIndex * tiling.singleCoreM;
tailM = tailM < tiling.singleCoreM ? tailM : tiling.singleCoreM;
int tailN = tiling.N - nCoreIndex * tiling.singleCoreN;
tailN = tailN < tiling.singleCoreN ? tailN : tiling.singleCoreN;
// When tailM < singleCoreM or tailN < singleCoreN, the tail block needs to be handled. In this case, call the SetTail API to set it
if (tailM < tiling.singleCoreM || tailN < tiling.singleCoreN) {
    matmulObj.SetTail(tailM, tailN);
}
```
