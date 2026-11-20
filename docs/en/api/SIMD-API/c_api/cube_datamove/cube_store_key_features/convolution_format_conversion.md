---
okftype: reference.concept
description: "Describes the format conversion of Fixpipe in convolution scenarios: NC1HWC0 data on the L0C Buffer is converted to NHWC output through Nz2ND, and on Ascend 950PR&950DT products it can also be converted to NCHW output through Nz2DN. The conversion parameters are set at the result move-out port according to the target layout."
---
# Convolution Format Conversion

<!-- md-trans-meta sourceCommit=54f1d966cfcc22290ddc2c6312bbe4c6ac2a9232 translatedAt=2026-11-16T11:59:22.278Z pushedAt=2026-11-18T08:15:34.366Z nl=2 -->

In convolution operations, the data stored in the L0C Buffer is in the NC1HWC0 format. Through the [Nz2ND](NZ2ND.md) capability, the Fixpipe instruction supports converting the data in the L0C Buffer to the NHWC format for output. The conversion diagram is as follows:

**Figure 1** Converting NC1HWC0 to NHWC format

![](../../../../figures/Fixpipe_NC1HWC0_NHWC.png)

<!-- npu="950" id1 -->
In particular, for the Ascend 950PR&950DT products, the Fixpipe instruction also supports converting the data in the L0C Buffer to the NCHW format for output through the [Nz2DN](NZ2DN.md) capability. The conversion diagram is as follows:

**Figure 2** Converting NC1HWC0 to NCHW format

![](../../../../figures/Fixpipe_NC1HWC0_NCHW.png)
<!-- end id1 -->

For the relationship between the im2col input loading, cube computation, and result output of convolution, see the convolution scenario in [Cube Computation Process](../../cube_compute/overview.md#conv2d-forward-propagation-flow). The C API result output entry is [asc_copy_l0c2gm](../cube_compute_store/asc_copy_l0c2gm.md). After selecting the current architecture version, set the format conversion parameters according to the target layout.
