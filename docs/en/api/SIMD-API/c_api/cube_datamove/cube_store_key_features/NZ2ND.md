---
okftype: reference.concept
description: "Describes the block-by-block address relationship and configurable parameters of Nz2ND format conversion: Atlas A3/A2 enables it via nz2nd_en of asc_copy_l0c2gm and configures the cube count and interval via asc_set_l0c_copy_params; Ascend 950PR&950DT configures it via enable_nz2nd and asc_set_l0c_copy_nz_para. Parameter units of different architectures cannot be directly reused."
---
# NZ2ND

<!-- md-trans-meta sourceCommit=54f1d966cfcc22290ddc2c6312bbe4c6ac2a9232 translatedAt=2026-11-16T12:02:50.847Z pushedAt=2026-11-18T08:15:34.376Z nl=2 -->

## Feature Description

When moving a cube out, the Nz2ND function implements cube conversion from the Nz format to the ND format.

The Nz2ND conversion process can be referenced by the following pseudocode (all addresses are counted in bytes, and src_matrix_stride_64b is a unified 64B stride notation used to uniformly describe the source cube interval):

```cpp
for (i = 0; i < matrix_num; i++) {
    // fractal_size = 16 * sizeof(L0C_element)
    src_temp_nd_addr = src + src_matrix_stride_64b * fractal_size * i;
    dst_temp_nd_addr = dst + dst_matrix_stride * sizeof(dst_element) * i;
    for (j = 0; j < m_size; j++) {
        src_temp_n_addr = src_temp_nd_addr + j * 16 * sizeof(L0C_element);
        dst_temp_n_addr = dst_temp_nd_addr + j * dst_stride * sizeof(dst_element);
        for (k = 0; k < ((n_size + 15) / 16); k++) {
            src_block_addr = src_temp_n_addr + k * src_stride * 16 * sizeof(L0C_element);
            dst_block_addr = dst_temp_n_addr + k * 16 * sizeof(dst_element);
        }
    }
}
```

The parameters used in the pseudocode are described as follows:

- src indicates the start address of the source operand transfer.
- dst indicates the start address of the destination operand transfer.
- The start address of each Nz cube in the source operand is src\_temp\_nd\_addr.
- The start address of each ND cube in the destination operand is dst\_temp\_nd\_addr.
- The start address of each row in each Nz cube in the source operand is src\_temp\_n\_addr.
- The start address of each row in each ND cube in the destination operation is dst\_temp\_n\_addr.
- In each row, 16 data elements form one data block. The start address of each data block in the source operand is src\_block\_addr, and the start address of each data block in the destination operand is dst\_block\_addr.

The following parameters are user-configurable parameters of Nz2ND. For their meanings and value ranges, see [C API move-out parameters](../cube_compute_store/asc_copy_l0c2gm.md):

- m_size and n_size indicate the dimensions of the cube in the m direction and n direction, respectively.
- src_stride indicates the interval between different Z fractal columns in the Nz cube, in the unit of C0\_Size.
- dst_stride indicates the interval between different rows in the destination ND cube, in elements.
- matrix_num indicates the number of cubes to be converted by Nz2ND.
- src_matrix_stride_64b indicates the interval between different Nz cubes, in the unit of 64B, that is, 16×sizeof(L0C_element).
- dst_matrix_stride indicates the interval between different destination ND cubes, in elements.

<!-- npu="A3,910b" id1 -->
Atlas A3 products and Atlas A2 products enable this feature through the `nz2nd_en` of [asc_copy_l0c2gm (Atlas A3 products and Atlas A2 products)](../cube_compute_store/asc_copy_l0c2gm_arch_2201.md). The number of cubes and the intervals are configured by [asc_set_l0c_copy_params](../cube_store_aux_config/asc_set_l0c_copy_params.md).
<!-- end id1 -->
<!-- npu="950" id2 -->
Ascend 950PR&950DT products enable this feature through the `enable_nz2nd` of [asc_copy_l0c2gm (Ascend 950PR&950DT products)](../cube_compute_store/asc_copy_l0c2gm_arch_3510.md), and configure the number of cubes and the intervals through [asc_set_l0c_copy_nz_para](../cube_store_aux_config/asc_set_l0c_copy_nz_para.md).
<!-- end id2 -->
Parameters of different architectures cannot be directly copied. In particular, verify the unit of the source cube stride.

## Feature Constraints

- When the Channel Split/Channel Merge feature (Nz2Nz) is enabled, this feature is unavailable.
- The destination operand addresses cannot overlap.

## Usage Example

As shown in the following figure, there are two Nz-format cubes in the L0C Buffer. Each cube has a dimension of 32 in the N direction and 48 in the M direction. After Nz2ND conversion, the data format becomes the layout shown in the right part.

**Figure 1** Nz2ND schematic diagram

![](../../../../figures/fixpipe_nz2nd_c_api.png)

For this scenario, the correspondence between the parameters in the figure and the C API configuration is as follows:

- matrix_num = 2, indicates that the number of cubes requiring Nz2ND conversion is 2.
- n_size = 32, indicates that the size of the source Nz cube in the N direction is 32 elements.
- m_size = 48, indicates that the size of the source Nz cube in the M direction is 48 elements.
- src_stride = 64, indicates that the interval between the start addresses of adjacent Z layouts in the source Nz cube is 64 \* C0\_Size\(C0\_Size = 16\*sizeof\(T\), where T is the data type of the operand \) bytes.
- dst_stride = 64, indicates that the number of elements in each row of the destination ND cube is 64.
- src_matrix_stride_64b = 256, indicates different intervals between the start addresses of Nz cubes are 256 \* C0\_Size bytes.
- dst_matrix_stride = 4096, indicates the offset between the start addresses of adjacent destination ND cubes is 4096 elements.
