---
okftype: reference.concept
description: "Describes the L0C Buffer to UB dual-target mode on the 3510: an M×N cube is split into two halves along the M or N direction and written to the UB of two Vector Cores respectively, with the first half written to SUB BLOCK0 and the second half written to SUB BLOCK1. The split direction is selected via asc_dual_dst_mode of asc_copy_l0c2ub, and the row/column multiple constraints must be satisfied."
---
# L0C Buffer to UB Dual-Target Mode

<!-- md-trans-meta sourceCommit=54f1d966cfcc22290ddc2c6312bbe4c6ac2a9232 translatedAt=2026-11-16T12:01:43.185Z pushedAt=2026-11-18T08:15:34.374Z nl=2 -->

## Feature Description

This feature applies only to 3510. Within the same AI Core, there are one Cube Core and two Vector Cores. When dual-target mode control is enabled, the M×N cube in the L0C Buffer is split into two halves and simultaneously written to the Unified Buffer (UB) of each of the two Vector Cores, where the first half is written to SUB BLOCK0 and the second half is written to SUB BLOCK1.

- Dual-target mode (M dimension): splits along the M dimension into two cubes of shape M / 2 \* N, which are written to the two UBs respectively.
- Dual-target mode (N dimension): splits along the N dimension into two cubes of shape M \* N / 2, which are written to the two UBs respectively.

Call [asc_copy_l0c2ub](../cube_compute_store/asc_copy_l0c2ub.md) and use [asc_dual_dst_mode](../../defs/enum/asc_dual_dst_mode.md) to select splitting along the M or N direction. For the output layout, see [NZ2NZ](NZ2NZ.md) or [NZ2ND](NZ2ND.md), and follow the restrictions on the accompanying functions and dimensions described below.

## Feature Constraints

- The dual-target mode is supported only on the L0C Buffer to UB path, in the normal copy mode (Nz2Nz) or the Nz2ND copy scenario. It is not supported in the sideband function scenario.
- When splitting along the M dimension, M must be a multiple of 2.
- When splitting along the N dimension, N must be a multiple of 32.

## Usage Example

Enabling dual-target mode in normal copy mode is shown in the following figure, which is divided into splitting by the M dimension and splitting by the N dimension. When splitting by the M dimension, M must be a multiple of 2; when splitting by the N dimension, N must be a multiple of 32:

Splitting in the N direction:

- n_size = 32, indicating that the size of the copy cube in the source Nz cube in the N direction is 32 elements.
- m_size = 48, indicating that the size of the copy cube in the source Nz cube in the M direction is 48 elements.
- src_stride = 64, indicating the start address offset of adjacent Z arrangements of the copy cube in the source Nz cube, that is, the interval between the start address of the first block's Z arrangement cube and the start address of the second Z arrangement cube in the following figure is 64 \* C0\_Size.
- dst_stride = 64 \* C0, indicating the start address offset of adjacent Z arrangements in the destination Nz cube. This stride is applied within each UB separately, not the distance between the two UB address spaces.

Splitting in the M direction:

- n_size = 32, indicating that the size of the copy cube in the source Nz cube in the N direction is 32 elements.
- m_size = 48, indicating that the complete source cube M is 48; after splitting, each UB receives 24 rows. The number of rows after splitting cannot be used as the source m_size.
- src_stride = 64, indicates the start address offset of adjacent Z arrangements in the source Nz cube to be copied, that is, the interval between the start address of the first Z arrangement cube and the start address of the second Z arrangement cube in the figure below is 64 \* C0\_Size.
- dst_stride = 40 \* C0, indicates the start address offset of adjacent Z arrangements in the destination Nz cube, that is, the interval between the start address of the first Z arrangement and the start address of the second Z arrangement in UB0 (or UB1) in the figure below is 40 \* 16 elements.

**Figure 1** Schematic diagram of Nz2Nz dual-target copy mode settings

![](../../../../figures/nz2nz_l0c2ub_dual_dst_c_api.png)

Enabling dual-target mode in Nz2ND mode is shown in the figure below, divided into splitting by M dimension and splitting by N dimension. When splitting by M dimension, M must be a multiple of 2; when splitting by N dimension, N must be a multiple of 32:

- nd_num = 2, indicates that the number of source Nz cubes is 2. The red box area in the figure is cube 1, and the blue box area is cube 2.
- n_size = 32, indicates that the size of the source Nz cube (the red box area or the blue box area in the figure) in the N direction is 32 elements.
- m_size = 48, indicates that the size of the source Nz cube in the M direction is 48 elements.
- src_stride = 64, indicates the start address offset of adjacent Z arrangements in the source Nz cube, that is, the interval between the start address of the light-colored Z arrangement cube on the left and the start address of the dark-colored Z arrangement cube on the right in the red box area in the figure below is 64 \* C0\_Size.
- dst_stride = 64, indicates that the number of elements in each row of the destination ND cube is 64.
- src_nd_stride = 240, indicates that the interval between the start addresses of different Nz cubes is 240 \* C0\_Size.
- dst_nd_stride = 4096, indicates that the offset between the start addresses of adjacent destination ND cubes is 4096 elements.

**Figure 2** Schematic diagram of Nz2ND dual-target copy-out mode settings

![](../../../../figures/nz2nd_l0c2ub_dual_dst_c_api.png)

In the figure, `nd_num`, `src_nd_stride`, and `dst_nd_stride` correspond to `matrix_num`, `src_nz_matrix_stride`, and `dst_matrix_stride` of [asc_set_l0c_copy_nz_para](../cube_store_aux_config/asc_set_l0c_copy_nz_para.md), respectively. In this example, they are set to 2, 240, and 4096. `asc_copy_l0c2ub` selects the splitting direction through `asc_dual_dst_mode::DUAL_DST_SPLIT_M` or `DUAL_DST_SPLIT_N`. In the Nz output example, C0 is 16, and `dst_stride` is counted by destination elements.
