---
okftype: reference.concept
description: "Describes the address conversion and parameter units for maintaining the Nz large fractal layout before and after NZ2NZ moves out: the target stride is counted in 32 bytes for 2201 and in elements for 3510, and gives the small fractal width and 512B small z fractal constraints when F32 Channel Split and B8/B4 Channel Merge are enabled."
---
# NZ2NZ

<!-- md-trans-meta sourceCommit=54f1d966cfcc22290ddc2c6312bbe4c6ac2a9232 translatedAt=2026-11-16T12:02:58.037Z pushedAt=2026-11-18T08:15:34.378Z nl=2 -->

## Feature Description

During the move-out process of cube computation, the core function of Nz2Nz is to maintain the Nz large fractal layout before and after the cube computation result is moved out; output type conversion and Channel Split/Channel Merge may change the small fractal width. The following pseudocode describes only the per-block address relationship when channel split/merge is not enabled.

<!-- npu="A3,910b" id1 -->
Take the following product models as examples:
<!-- npu="A3" id2 -->
Atlas A3 products
<!-- end id2 -->
<!-- npu="910b" id3 -->
Atlas A2 products
<!-- end id3 -->
The Nz2Nz conversion process can be referenced by the following pseudocode (src/dst and temporary addresses are all counted in byte addresses):

```cpp
for (j = 0; j < m_size; j++) {
    src_temp_n_addr = src + j * sizeof(L0C_element) * 16;
    dst_temp_n_addr = dst + j * 16 * sizeof(DST_element);
    for (k = 0; k < ((n_size + 15) / 16); k++) {
        src_block_addr = src_temp_n_addr + k * src_stride * sizeof(L0C_element) * 16;
        dst_block_addr = dst_temp_n_addr + k * dst_stride * 32;
    }
}
```

The parameters used in the pseudocode are described as follows:

- src indicates the start address from which the source operand is moved.
- dst indicates the start address to which the destination operand is moved.
- The start address of each row of the Nz cube in the source operand is src\_temp\_n\_addr.
- The start address of each row of the Nz cube in the destination operand is dst\_temp\_n\_addr.
- In each row, data blocks are split at a granularity of 16 data elements. The start address of each data block in the source operand is src\_block\_addr, and the start address of each data block in the destination operand is dst\_block\_addr.

The following parameters are user-configurable parameters for Nz2Nz (the destination stride parameter name for 2201 is dst_stride_dst, which is uniformly denoted as dst_stride in this document). For their meanings and value ranges, see [C API store-out parameters](../cube_compute_store/asc_copy_l0c2gm.md):

- m_size and n_size indicate the dimensions in the M direction and N direction of the cube, respectively.
- src_stride indicates the interval between different Z fractal columns in the source Nz cube, in units of C0\_Size (C0\_Size=16\*sizeof\(T\), where T is the data type of the operand).
- dst_stride indicates the interval between different Z fractal columns in the destination Nz cube, in units of 32 bytes.
<!-- end id1 -->

<!-- npu="950" id4 -->
Taking the Ascend 950PR&950DT products as an example, the Nz2Nz conversion process can refer to the following pseudocode (src/dst and temporary addresses are all counted as byte addresses):

```cpp
for (j = 0; j < m_size; j++) {
    src_temp_n_addr = src + j * sizeof(L0C_element) * 16;
    dst_temp_n_addr = dst + j * 16 * sizeof(DST_element);
    for (k = 0; k < ((n_size + 15) / 16); k++) {
        src_block_addr = src_temp_n_addr + k * src_stride * sizeof(L0C_element) * 16;
        dst_block_addr = dst_temp_n_addr + k * dst_stride * sizeof(DST_element);
    }
}
```

The parameters used in the pseudocode are described as follows:

- src indicates the start address from which the source operand is moved.
- dst indicates the start address to which the destination operand is moved.
- The start address of each row of the Nz cube in the source operand is src\_temp\_n\_addr.
- The start address of each row of the Nz cube in the destination operand is dst\_temp\_n\_addr.
- In each row, data blocks are split at a granularity of 16 data elements. The start address of each data block in the source operand is src\_block\_addr, and the start address of each data block in the destination operand is dst\_block\_addr.

The following parameters are user-configurable parameters of Nz2Nz. For their meanings and value ranges, see [C API store-out parameters](../cube_compute_store/asc_copy_l0c2gm.md):

- m_size and n_size indicate the dimensions of the cube in the M direction and N direction, respectively.
- src_stride indicates the interval between different Z fractal columns in the source Nz cube, in units of C0\_Size (C0\_Size=16\*sizeof\(T\), where T is the data type of the operand).
- dst_stride indicates the interval between different Z fractal columns in the destination Nz cube, in units of element.
<!-- end id4 -->

This feature is used to preserve the fractal output layout. For the store-out parameters of each architecture, see [asc_copy_l0c2gm](../cube_compute_store/asc_copy_l0c2gm.md) and [asc_copy_l0c2l1](../cube_compute_store/asc_copy_l0c2l1.md). For the relationship between the output type and the small fractal width, see [Accompanying Quantization](accompanying_quantization.md), [F32 Channel Split](F32-Channel-Split.md), and [Int8 Channel Merge](Int8-Channel-Merge.md).

## Feature Constraints

- When Nz2Nz and the F32 Channel Split feature are enabled in combination, the small fractal dimension in the Nz fractal moved out to GM is 16\*8, ensuring that the size of the small z fractal is 512B.
- For Nz output of the B8 data type, Channel Merge is enabled by default, and the small fractal dimension moved out to the GM/L1 Buffer is 16\*32, ensuring that the size of the small z fractal is 512B.
- For Nz output of the B4 data type, Channel Merge is enabled by default, and the small fractal dimension is 16\*64, ensuring that the size of the small z fractal is 512B.
- The destination operand addresses must not overlap.

## Usage Example

As shown in the following figure, there is an Nz-format cube in the L0C Buffer. The cube has a dimension of 48 in the N direction and 24 in the M direction. After Nz2Nz conversion, the data format becomes the layout shown in the right part.

<!-- npu="A3,910b" id7 -->
**Figure 1** Nz2Nz schematic diagram ([NPU architecture version 2201](../../../../../guide/programming_guide/language_extension/simd_builtin_keywords.md))

![](../../../../figures/fixpipe_nz2nz_c_api.png)

The parameter setting example in the schematic diagram (moving out through the L0C Buffer move-out interface and removing dummy data) and the explanation are as follows:

- When the number of data elements in the M direction is not a multiple of 16, the source fractal may contain padded dummy data. The move-out selects valid rows based on m_size, and dummy rows are not written to the destination position. A cube block is defined as a continuous 16\*16 data block. The number of cube blocks is M/16 rounded up, and the length of a cube block is M\*16\*sizeof\(T\), where T is the data type.
- n_size=48, indicating that the size of the cube to be moved in the source Nz cube in the N direction is 48 elements.
- m_size=24, indicating that the size of the cube to be moved in the source Nz cube in the M direction is 24 elements.
- src_stride=64, indicating the start address offset between adjacent Z layouts of the cube to be moved in the source Nz cube, that is, the interval between the start address of the first Z layout and the start address of the second Z layout in the following figure is 64 \* C0\_Size.
- dst_stride = 40, indicating the start address offset between adjacent Z layouts in the destination Nz cube, that is, the interval between the start address of the first Z layout and the start address of the second Z layout in the following figure is 40\*32 bytes.
<!-- end id7 -->

<!-- npu="950" id8 -->
**Figure 2** Nz2Nz diagram ([NPU architecture version 3510](../../../../../guide/programming_guide/language_extension/simd_builtin_keywords.md))

![](../../../../figures/fixpipe_nz2nz_c_api_950.png)

The parameter setting example in the diagram (moving data out through the L0C Buffer move-out interface and removing dummy data) and the explanation are as follows:

- When the number of data elements in the M direction is not a multiple of 16, the source fractal may contain padded dummy data. The move-out selects valid rows based on m_size, and dummy rows are not written to the destination position. A cube block is defined as a continuous data block of 16\*16, the number of data blocks is M/16 rounded up, and the length of a cube block is M\*16\*sizeof\(T\), where T is the data type.
- n_size=48, indicating that the size of the cube to be moved in the source Nz cube in the N direction is 48 elements.
- m_size=24, indicating that the size of the cube to be moved in the source Nz cube in the M direction is 24 elements.
- src_stride=64, indicating the start address offset between adjacent Z layouts of the cube to be moved in the source Nz cube, that is, the interval between the start address of the first Z layout and the start address of the second Z layout in the following figure is 64 \* C0\_Size.
- dst_stride = 40 * 16, indicating the start address offset between adjacent Z layouts in the destination Nz cube, that is, the interval between the start address of the first Z layout and the start address of the second Z layout in the following figure is 40\*16 elements.
<!-- end id8 -->
