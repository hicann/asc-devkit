# F32 Channel Split<a name="ZH-CN_TOPIC_0000002569070989"></a>

<!-- md-trans-meta sourceCommit=754fb95859dead826727a591e674261c6a829a08 translatedAt=2026-09-23T13:13:13.111Z pushedAt=2026-10-26T07:41:44.169Z nl=2 -->

## Description

During the move-out process of matrix computation, when the target type is f32 and Channel Split is enabled, the hardware can convert a 16×16 fractal matrix into a 16×8 small-z fractal matrix. In this case, each 16×16 fractal matrix is split into two independent 16×8 small-z fractal matrices.

For the **DataCopy** API, you can enable this feature by setting **channelSplit** to **true** in the **DataCopyCO12DstParams structure parameter** definition.

For the **Fixpipe** API, you can enable this feature by setting **channelSplit** to **true** in the [Fixpipe transfer parameter structure description](../cube_compute_store/Fixpipe_L0CToGM.md).

## Constraints

<!-- npu="A3,910b" id1 -->
- For the following product models:

    <!-- npu="A3" id2 -->
    Atlas A3 products
    <!-- end id2 -->

    <!-- npu="910b" id3 -->
    Atlas A2 products
    <!-- end id3 -->

    When both the input and output data types of the **Fixpipe** instruction are float, NZ is output to GM, and **NZ2ND** and **unitFlag** are not enabled, the Channel Split feature can be enabled.
<!-- end id1 -->

<!-- npu="950" id4 -->
- For Ascend 950PR&950DT products, when both the input and output data types of the **Fixpipe** instruction are float, NZ is output to GM, and **NZ2ND**, **NZ2DN**, and **unitFlag** are not enabled, the Channel Split feature can be enabled. Currently, output to L1 Buffer and Unified Buffer (UB) does not support this feature.
<!-- end id4 -->

## Example

For a complete example, see scenario 7 of the [fixpipe_l0c2gm sample](../../../../../../../examples/01_simd_cpp_api/03_basic_api/03_matrix_compute/fixpipe_l0c2gm).

As shown in the following figure, when Channel Split is enabled, the source operand with the shape \[64,32\] is split into 16 independent 16x8 fractal matrices.

**Figure 1** F32 Channel Split diagram (no invalid data)<a name="zh-cn_topic_0000002515660950_fig222295694112"></a>  

![](../../../../figures/Fixpipe_Channel_Split.png)

When Channel Split is enabled, **nSize** (the size of the source NZ matrix in the N direction) must be a multiple of 8. As shown in the following figure, the source operand with the shape \[64,24\] is split into 3 columns of 16x8 fractal matrices.

**Figure 2** F32 Channel Split diagram (with invalid data)<a name="zh-cn_topic_0000002515660950_fig5809145844116"></a>  

![](../../../../figures/Fixpipe_Channel_Split_with_dirty.png)
