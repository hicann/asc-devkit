# L0C Buffer-to-UB Dual-Target Mode

<!-- md-trans-meta sourceCommit=7ef8ad7bdbdb0cef0f561d391759ecb8b4e4043a translatedAt=2026-11-02T04:25:37.092Z pushedAt=2026-11-18T01:25:06.790Z nl=2 -->

## Description

Within the same AI Core, there are one Cube Core and two Vector Cores. When dual-target mode control is enabled, the M×N matrix in the L0C Buffer is split into two halves and written simultaneously to the Unified Buffer (UB) of each of the two Vector Cores, with the first half written to SUB BLOCK0 and the second half written to SUB BLOCK1.

- Dual-target mode (M dimension): The matrix is split along the M dimension into two matrices of shape M / 2 \* N, which are written to the two UBs respectively.
- Dual-target mode (N dimension): The matrix is split along the N dimension into two matrices of shape M \* N / 2, which are written to the two UBs respectively.

## Constraints

- The dual-target mode is supported only on the L0C Buffer-to-UB path, in the normal copy mode (NZ2NZ) or the NZ2ND copy scenario. It is not supported in in-band function scenarios.
- When splitting along the M dimension, M must be a multiple of 2.
- When splitting along the N dimension, N must be a multiple of 32.

## Example

Enabling the dual-target mode in normal copy mode is shown in the following figure. It is divided into splitting by the M dimension and splitting by the N dimension. When splitting by the M dimension, M must be a multiple of 2; when splitting by the N dimension, N must be a multiple of 32:

N-direction splitting:

- **nSize** = 32, indicating that the size of the matrix to be copied in the source NZ matrix in the N direction is 32 elements.
- **mSize** = 48, indicating that the size of the matrix to be copied in the source NZ matrix in the M direction is 48 elements.
- **srcStride** = 64, indicating the start address offset between adjacent Z arrangements of the matrix to be copied in the source NZ matrix, that is, in the following figure, the interval between the start address of the first block's Z arrangement matrix and that of the second Z arrangement matrix is 64 \* C0\_Size.
- **dstStride** = 64 \* C0, indicating the start address offset between adjacent Z arrangements in the destination NZ matrix, that is, the interval between the start address of the Z arrangement in UB0 and the start address of the Z arrangement in UB1 in the following figure is 64 \* 16 elements.

M-direction splitting:

- **nSize** = 32, indicating that the size of the matrix to be copied in the source NZ matrix in the N direction is 32 elements.
- **mSize** = 24, indicating that the size of the matrix to be copied in the source NZ matrix in the M direction is 24 elements.
- **srcStride** = 64, indicating the start address offset between adjacent Z arrangements of the matrix to be copied in the source NZ matrix, that is, in the following figure, the interval between the start address of the first block's Z arrangement matrix and that of the second Z arrangement matrix is 64 \* C0\_Size.
- **dstStride** = 40 \* C0, indicating the start address offset between adjacent Z arrangements in the destination NZ matrix, that is, the interval between the start address of the first Z arrangement and the start address of the second Z arrangement in UB0 (or UB1) in the following figure is 40 \* 16 elements.

**Figure 1** NZ2NZ dual-target copy mode configuration diagram<a name="fig1386816164556"></a>  

![](../../../../figures/nz2nz_l0c2ub_dual_dst.png)

Enabling the dual-target mode in NZ2ND mode is shown in the following figure, which is divided into splitting by the M dimension and splitting by the N dimension. When splitting by the M dimension, M must be a multiple of 2; when splitting by the N dimension, N must be a multiple of 2:

- **ndNum** = 2, indicating that the number of source NZ matrices is 2. In the figure, the red-boxed area is matrix 1, and the blue-boxed area is matrix 2.
- **nSize** = 32, indicating that the size of the source NZ matrix (the red-boxed area or the blue-boxed area in the figure) in the N direction is 32 elements.
- **mSize** = 48, indicating that the size of the source NZ matrix in the M direction is 48 elements.
- **srcStride** = 64, indicating the start address offset of adjacent Z arrangements in the source NZ matrix, that is, the interval between the start address of the light-colored Z arrangement matrix on the left and the start address of the dark-colored Z arrangement matrix on the right in the red-boxed area of the following figure is 64 \* C0\_Size.
- **dstStride** = 64, indicating that the number of elements in each row of the destination ND matrix is 64.
- **ndNum** = 2, indicating the number of source NZ matrices.
- **srcNdStride** = 240 , indicating that the interval between the start addresses of different NZ matrices is 240 \* C0\_Size.
- **dstNdStride** = 4096, indicating that the offset between the start addresses of adjacent destination ND matrices is 4096 elements.

**Figure 2** Diagram of NZ2ND parameter settings in dual-copy mode<a name="fig715654093716"></a>  

![](../../../../figures/nz2nd_l0c2ub_dual_dst.png)

For the complete sample, see [fixpipe\_l0c2ub](../../../../../../../examples/01_simd_cpp_api/03_basic_api/03_matrix_compute/fixpipe_l0c2ub).
