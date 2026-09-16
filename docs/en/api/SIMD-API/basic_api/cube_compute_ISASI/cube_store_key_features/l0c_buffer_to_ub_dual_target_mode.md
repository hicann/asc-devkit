# L0C Buffer to UB Dual-Target Mode

<!-- md-trans-meta sourceCommit=e9bec8b34ada6c37c20391d212da5ebf088e2e8b translatedAt=2026-08-27T15:16:09.494Z -->

## Description

Within the same AI Core, there are one **Cube Core** and two **Vector Core**s. When dual-target mode control is enabled, the M×N cube in the **L0C Buffer** is split into two halves and written simultaneously to the **UB** of each of the two **Vector Core**s, with the first half written to **SUB BLOCK0** and the second half written to **SUB BLOCK1**.

- Dual-target mode (M dimension): The cube is split along the M dimension into two matrices of shape M / 2 \* N, which are written to the two **UB**s respectively.
- Dual-target mode (N dimension): The cube is split along the N dimension into two matrices of shape M \* N / 2, which are written to the two **UB**s respectively.

## Constraints

- The dual-target mode is supported only on the **L0C Buffer**-to-**UB** path, in the normal copy mode (**NZ2NZ**) or the **NZ2ND** copy scenario. It does not support in-line functionality scenarios.
- When splitting along the M dimension, M must be a multiple of 2.
- When splitting along the N dimension, N must be a multiple of 32.

## Example

Enabling the dual-target mode in the normal copy mode is shown in the following figure. It is divided into splitting along the M dimension and splitting along the N dimension. When splitting along the M dimension, M must be a multiple of 2; when splitting along the N dimension, N must be a multiple of 32:

Splitting along the N dimension:

- **nSize** = 32, indicating that the size of the cube to be copied in the source NZ cube along the N dimension is 32 elements.
- **mSize** = 48, indicating that the size of the cube to be copied in the source NZ cube along the M dimension is 48 elements.
- **srcStride** = 64, indicating the start address offset between adjacent Z arrangements of the cube to be copied in the source NZ cube. That is, in the following figure, the interval between the start address of the first block's Z arrangement cube and the start address of the second Z arrangement cube is 64 \* C0\_Size.
- **dstStride** = 64 \* C0, indicating the start address offset between adjacent Z arrangements in the destination NZ cube. That is, in the following figure, the interval between the start address of the Z arrangement in UB0 and the start address of the Z arrangement in UB1 is 64 \* 16 elements.

Splitting along the M dimension:

- **nSize** = 32, indicating that the size of the cube to be copied in the source NZ cube along the N dimension is 32 elements.
- **mSize** = 24, indicating that the size of the cube to be copied in the source NZ cube along the M dimension is 24 elements.
- **srcStride** = 64, indicating the start address offset between adjacent Z arrangements of the cube to be copied in the source NZ cube. That is, in the following figure, the interval between the start address of the first Z arrangement cube and the start address of the second Z arrangement cube is 64 * C0_Size.
- **dstStride** = 40 * C0, indicating the start address offset between adjacent Z arrangements in the destination NZ cube. That is, in the following figure, the interval between the start address of the first Z arrangement and the start address of the second Z arrangement in UB0 (or UB1) is 40 * 16 elements.

**Figure 1** Schematic diagram of the NZ2NZ dual-target copy mode settings<a name="fig1386816164556"></a>  

![](../../../../figures/nz2nz_l0c2ub_dual_dst.png)

Enabling dual-target mode in NZ2ND mode is shown in the following figure, which is divided into splitting by the M dimension and splitting by the N dimension. When splitting by the M dimension, M must be a multiple of 2; when splitting by the N dimension, N must be a multiple of 2:

- **ndNum** = 2, indicating that the number of source NZ matrices is 2. In the figure, the red-boxed region is cube 1, and the blue-boxed region is cube 2.
- **nSize** = 32, indicating that the size of the source NZ cube (the red-boxed region or the blue-boxed region in the figure) in the N direction is 32 elements.
- **mSize** = 48, indicating that the size of the source NZ cube in the M direction is 48 elements.
- **srcStride** = 64, indicating the start address offset between adjacent Z arrangements in the source NZ cube. That is, in the red-boxed region of the following figure, the interval between the start address of the light-colored Z arrangement cube on the left and the start address of the dark-colored Z arrangement cube on the right is 64 * C0_Size.
- **dstStride** = 64, indicating that the number of elements in each row of the destination ND cube is 64.
- ndNum = 2, indicating the number of source NZ matrices.
- srcNdStride = 240, indicating that the interval between the start addresses of different NZ matrices is 240 * C0_Size.
- dstNdStride = 4096, indicating that the offset between the start addresses of adjacent destination ND matrices is 4096 elements.

**Figure 2** NZ2ND parameter dual-copy mode configuration diagram<a name="fig715654093716"></a>

![](../../../../figures/nz2nd_l0c2ub_dual_dst.png)

For the complete sample, see [fixpipe_l0c2ub](https://gitcode.com/cann/asc-devkit/tree/9.1.0/examples/01_simd_cpp_api/03_basic_api/03_matrix_compute/fixpipe_l0c2ub).
