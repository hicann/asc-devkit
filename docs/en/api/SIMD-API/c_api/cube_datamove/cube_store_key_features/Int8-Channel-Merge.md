---
okftype: reference.concept
description: "Describes the channel merge behavior automatically enabled by hardware when Nz output is converted to int8_t or uint8_t: 16×16 fractal cubes are merged pairwise along the N direction into 16×32, and when N is an odd multiple of 16, the last group of 16 channels is not merged. It also advises adjusting the destination buffer and stride accordingly."
---
# Int8 Channel Merge

<!-- md-trans-meta sourceCommit=54f1d966cfcc22290ddc2c6312bbe4c6ac2a9232 translatedAt=2026-11-16T12:00:56.121Z pushedAt=2026-11-18T08:15:34.371Z nl=2 -->

## Feature Description

During the move-out process of cube computation, when the target data type is converted to int8\_t or uint8\_t, this feature is automatically enabled for Nz output. The hardware expands the fractal cube from 16×16 to 16×32. The specific rules are as follows: if the number of output channels N is an even multiple of 16, then in the N direction, every two adjacent 16×16 fractal cubes are merged into a single 16×32 fractal matrix. If N is an odd multiple of 16, channels 1 through channel (N–16) are merged, while the last 16 channels remain unchanged and are not merged.

This behavior is related to the output data type and the [Nz output layout](NZ2NZ.md). When quantizing float or int32_t results to int8_t/uint8_t, see [accompanying quantization](accompanying_quantization.md) for mode selection and coefficient configuration; see [asc_copy_l0c2l1](../cube_compute_store/asc_copy_l0c2l1.md) for the interface that outputs results to L1. When allocating the destination buffer and setting the stride, the change in the small fractal width after merging must be taken into account.

## Feature Constraints

It is automatically enabled by the hardware and cannot be configured.

## Example

For example, when the target data type is s8, M is 32, and N is 48, the hardware first merges the first two 16×16 fractal cubes in the N direction into a 16×32 cube, and then directly moves the remaining 16×16 fractal cube to the L1 Buffer.

**Figure 1** Int8 Channel Merge diagram

![](../../../../figures/Fixpipe_Int8_Channel_Merge.png)
