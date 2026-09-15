# F32 Channel Split<a name="ZH-CN_TOPIC_0000002569070989"></a>

<!-- md-trans-meta sourceCommit=unknown translatedAt=2026-08-31T11:36:55.150Z -->

## Description

During the move-out process of cube computation, when the target type is f32 and Channel Split is enabled, the hardware can convert a 16×16 fractal cube into 16×8 small-z fractal matrices. In this case, each 16×16 fractal cube is split into two independent 16×8 small-z fractal matrices.

For the DataCopy API, Channel Split can be enabled by setting **channelSplit** to **true** in the **DataCopyCO12DstParams** structure parameter definition.

For the Fixpipe API, Channel Split can be enabled by setting **channelSplit** to **true** in [Fixpipe Move-out Parameter Structure Description](../cube_compute_store/Fixpipe_L0CToGM.md).

## Constraints

<!-- npu="A3,910b" id1 -->
- For the following product models:

    <!-- npu="A3" id2 -->
    Atlas A3 training products/Atlas A3 inference products
    <!-- end id2 -->

    <!-- npu="910b" id3 -->
    Atlas A2 training products/Atlas A2 inference products
    <!-- end id3 -->

    When both the input and output data types of the **Fixpipe** instruction are **float**, the output is in NZ format to GM, and **NZ2ND** and **unitFlag** are not enabled, the Channel Split feature can be enabled.
<!-- end id1 -->

<!-- npu="950" id4 -->
- For Ascend 950PR/Ascend 950DT, when both the input and output data types of the **Fixpipe** instruction are **float**, the output is in NZ format to GM, and **NZ2ND**, **NZ2DN**, and **unitFlag** are not enabled, the Channel Split feature can be enabled. Currently, moving out to L1 Buffer and UB does not support this feature.
<!-- end id4 -->

## Example

As shown in the following figure, when Channel Split is enabled, the source operand with a shape of \[64,32\] is split into 16 independent 16x8 fractal matrices.

**Figure 1** F32 Channel Split diagram (without invalid data)<a name="zh-cn_topic_0000002515660950_fig222295694112"></a>  

![](../../../../figures/Fixpipe_Channel_Split.png)

When Channel Split is enabled, **nSize** (the size of the source NZ cube in the N direction) must be a multiple of 8. As shown in the following figure, the source operand with a shape of \[64,24\] is split into 3 columns of 16x8 fractal matrices.

**Figure 2** F32 Channel Split diagram (with invalid data)<a name="zh-cn_topic_0000002515660950_fig5809145844116"></a>  

![](../../../../figures/Fixpipe_Channel_Split_with_dirty.png)
