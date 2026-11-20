---
okftype: reference.concept
description: "Explains that when the target type is f32 and channel split is enabled during L0C move-out, the 16×16 fractal cube is split into two 16×8 small z fractal cubes; Atlas A3/A2 uses channel_split and Ascend 950PR&950DT uses enable_channel_split to enable it, and requires both input and output to be float, Nz output to GM, and related conversions and unitFlag to be disabled."
---
# F32 Channel Split

<!-- md-trans-meta sourceCommit=54f1d966cfcc22290ddc2c6312bbe4c6ac2a9232 translatedAt=2026-11-16T12:00:24.345Z pushedAt=2026-11-18T08:15:34.369Z nl=2 -->

## Feature Description

During the move-out process of cube computation, when the target type is f32, if Channel Split is enabled, the hardware can convert a 16×16 fractal cube into a 16×8 small-z fractal matrix. In this case, each 16×16 fractal cube is split into two independent 16×8 small-z fractal cubes.

<!-- npu="A3,910b" id7 -->
For Atlas A3 products and Atlas A2 products, set `channel_split=true` in [asc_copy_l0c2gm (Atlas A3 products and Atlas A2 products)](../cube_compute_store/asc_copy_l0c2gm_arch_2201.md). When using it, the restrictions on the output format and [UnitFlag](../../cube_compute/mmad_compute_key_features/UnitFlag.md) described below must also be satisfied. For the differences in format conversion, see [NZ2ND](NZ2ND.md).
<!-- end id7 -->
<!-- npu="950" id8 -->
For Ascend 950PR&950DT products, set `enable_channel_split=true` in [asc_copy_l0c2gm (Ascend 950PR&950DT products)](../cube_compute_store/asc_copy_l0c2gm_arch_3510.md). When using it, the restrictions on the output format and [UnitFlag](../../cube_compute/mmad_compute_key_features/UnitFlag.md) described below must also be satisfied. For the differences in format conversion, see [NZ2ND](NZ2ND.md) and [NZ2DN](NZ2DN.md).
<!-- end id8 -->

## Feature Constraints

<!-- npu="A3,910b" id3 -->
- For the following product models:

    <!-- npu="A3" id4 -->
    Atlas A3 products
    <!-- end id4 -->

    <!-- npu="910b" id5 -->
    Atlas A2 products
    <!-- end id5 -->

    Channel Split can be enabled only when the input and output data types of the Fixpipe instruction are both float, Nz is output to GM, and Nz2ND and unitFlag are not enabled.
<!-- end id3 -->

<!-- npu="950" id6 -->
- For Ascend 950PR&950DT products, Channel Split can be enabled only when the input and output data types of the Fixpipe instruction are both float, Nz is output to GM, and Nz2ND, Nz2DN, and unitFlag are not enabled. Currently, moving out to the L1 Buffer and Unified Buffer (UB) does not support this feature.
<!-- end id6 -->

## Usage Example

As shown in the following figure, when the Channel Split feature is enabled, the source operand with shape \[64,32\] is split into 16 independent 16x8 fractal cubes.

**Figure 1** F32 Channel Split diagram (no invalid data)

![](../../../../figures/Fixpipe_Channel_Split.png)

When the Channel Split feature is enabled, n_size (the size of the source Nz cube in the N direction) must be a multiple of 8. As shown in the following figure, the source operand with shape \[64,24\] is split into 3 columns of 16x8 fractal cubes.

**Figure 2** F32 Channel Split diagram (with invalid data)

![](../../../../figures/Fixpipe_Channel_Split_with_dirty.png)
