---
okftype: reference.concept
description: "Describes Nz2DN format conversion: converts an Nz-format cube to DN format, provides block-by-block address conversion pseudocode and configurable parameters such as matrix_num, src_stride, dst_stride, and src_nz_fractal_stride. Only the Ascend 950PR&950DT products enable this through the enable_nz2dn of the copy-out interface."
---
# NZ2DN

<!-- md-trans-meta sourceCommit=54f1d966cfcc22290ddc2c6312bbe4c6ac2a9232 translatedAt=2026-11-16T12:02:10.179Z pushedAt=2026-11-18T08:15:34.372Z nl=2 -->

## Feature Description

During the move-out process of cube computation, the core function of Nz2DN is to convert an Nz-format cube into a DN-format cube.

The Nz2DN conversion process can be referenced by the following pseudocode:

```cpp
for (int h = 0; h < matrix_num; h++) {
    src_tmp3_addr = src + h * 16 * sizeof(src_element) * src_nz_matrix_stride;
    dst_tmp3_addr = dst + h * sizeof(dst_element) * dst_matrix_stride;
    for (int i = 0; i < ((n_size + 15) / 16); i++) {
        src_tmp2_addr = src_tmp3_addr + i * sizeof(src_element) * 16 * src_stride;
        dst_tmp2_addr = dst_tmp3_addr + i * sizeof(dst_element) * 16 * dst_stride;
        for (int j = 0; j < 16; j++) {
            if (i * 16 + j < n_size) {
                src_block_addr = src_tmp2_addr + j * sizeof(src_element);
                dst_block_addr = dst_tmp2_addr + j * sizeof(dst_element) * dst_stride;
                for (int k = 0; k < m_size; k++) {
                    src_ele_addr = src_block_addr + k * sizeof(src_element) * 16 * src_nz_fractal_stride;
                    dst_ele_addr = dst_block_addr + k * sizeof(dst_element);
                }
            }
        }
    }
}
```

The parameters used in the pseudocode are described as follows:

- src indicates the source start address of the source operand move-out.
- dst indicates the destination start address of the destination operand move-out.
- The start address of each Nz cube of the source operand is src\_tmp3\_addr.
- The start address of each DN cube of the destination operand is dst\_tmp3\_addr.
- The source operand Nz cube is grouped by every 16 columns, and the start address of each data group is src\_tmp2\_addr.
- The destination operand DN cube is grouped by every 16 rows, and the start address of each data group is dst\_tmp2\_addr.
- In each data group from the source Nz cube described above, each column of elements is further treated as a data group, and the start address of each data group is src\_block\_addr.
- In each data group from the destination DN cube described above, each row of elements is further treated as a data group, and the start address of each data group is dst\_block\_addr.
- src\_ele\_addr is the address of each element in one column of the Nz cube, and dst\_ele\_addr is the address of each element in one row of the DN cube. The two are in a one-to-one correspondence, which completes the Nz2DN format conversion.

The following parameters are user-configurable parameters for Nz2DN. For their meanings and value ranges, see [asc_copy_l0c2gm](../cube_compute_store/asc_copy_l0c2gm_arch_3510.md):

- matrix_num indicates the number of cubes that need Nz2DN conversion.
- m_size and n_size indicate the dimensions of the cube in the m direction and n direction, respectively.
- src_stride indicates the interval between the start addresses of adjacent Z layouts in the source Nz cube, and the unit is C0\_Size.
- dst_stride indicates the number of elements in each row of the destination DN cube, and the unit is element.
- src_nz_matrix_stride indicates the interval between the start addresses of different source Nz cubes, and the unit is C0\_Size.
- dst_matrix_stride indicates the interval between the start addresses of adjacent destination DN cubes, and the unit is element.
- src_nz_fractal_stride indicates the address offset between adjacent rows of the source cube Nz fractal, the unit is C0\_Size.

<!-- npu="950" id1 -->
This feature applies to the Ascend 950PR&950DT products. Taking [asc_copy_l0c2gm](../cube_compute_store/asc_copy_l0c2gm_arch_3510.md) as an example, it is enabled by `enable_nz2dn`; the number of cubes and cube spacing are configured by [asc_set_l0c_copy_nz_para](../cube_store_aux_config/asc_set_l0c_copy_nz_para.md), and the source Nz fractal row spacing is configured by [asc_set_l0c_copy_channel_para](../cube_store_aux_config/asc_set_l0c_copy_channel_para.md). The mutual exclusion between channel split/merge and [dual-target mode](L0C_to_UB_dual_target_mode.md) is described in the feature constraints below.
<!-- end id1 -->

## Feature Constraints

- When the Channel Split/Channel Merge function (Nz2Nz) is enabled, this function is unavailable.
- When the L0C Buffer is moved to the Unified Buffer (UB) with dual-destination mode enabled, this function is unavailable.
- The destination operand addresses must not overlap.

## Usage Example

As shown in the following figure, there are two cubes in Nz format in the L0C Buffer. Each cube has a dimension of 32 in the N direction and 48 in the M direction. After Nz2DN conversion, the data format is changed to the layout shown in the right part.

**Figure 1** Nz2DN diagram

![](../../../../figures/fixpipe_nz2dn_c_api.png)

For this scenario, the correspondence between the parameters in the figure and the C API configuration is as follows:

- matrix_num = 2, which indicates that the number of cubes to be converted by Nz2DN is 2.
- n_size = 32, which indicates that the size of the source Nz cube in the N direction is 32 elements.
- m_size = 48, which indicates that the size of the source Nz cube in the M direction is 48 elements.
- src_stride = 80, which indicates that the interval between the start addresses of adjacent Z layouts in the source Nz cube is 80 \* C0\_Size\(C0\_Size = 16\*sizeof\(T\), where T is the data type of the operand \) bytes.
- dst_stride = 80, which indicates that the number of elements in each row of the destination DN cube is 80.
- src_nz_matrix_stride = 240, indicating that the interval between the start addresses of different source Nz cubes is 240 \* C0\_Size.
- dst_matrix_stride = 48 \* 80, indicating that the interval between the start addresses of adjacent destination DN cubes is 48 \* 80 = 3840 elements, where 48 is the number of rows between the start addresses of destination cubes, and 80 is dst_stride (the number of elements per row).
- src_nz_fractal_stride = 1: indicates that the address offset between adjacent rows of the source cube Nz fractal is 1 \* C0\_Size.
