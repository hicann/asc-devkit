# NZ2NZ

<!-- md-trans-meta sourceCommit=754fb95859dead826727a591e674261c6a829a08 translatedAt=2026-10-22T12:58:03.109Z pushedAt=2026-10-28T12:45:45.434Z nl=2 -->

## Description

During the move-out of cube computation results, the core function of Nz2Nz is to keep the Nz large-fractal layout unchanged before and after the move-out. Output type conversion and Channel Split/Channel Merge may change the small-fractal width. The following pseudo-code describes only the per-block address relationship when channel split/merge is not enabled.

<!-- npu="A3,910b" id1 -->
Take the following product models as an example:
<!-- npu="A3" id2 -->
Atlas A3 products
<!-- end id2 -->
<!-- npu="910b" id3 -->
Atlas A2 products
<!-- end id3 -->
The Nz2Nz conversion process can be referenced from the following pseudo-code (src/dst and temporary addresses are all counted as byte addresses):

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

The parameters used in the pseudo-code are described as follows:

- src indicates the start address from which the source operand is moved.
- dst indicates the start address to which the destination operand is moved.
- The start address of each row of the Nz cube in the source operand is src\_temp\_n\_addr.
- The start address of each row of the Nz cube in the destination operand is dst\_temp\_n\_addr.
- In each row, data blocks are split at a granularity of 16 data elements. The start address of each data block in the source operand is src\_block\_addr, and the start address of each data block in the destination operand is dst\_block\_addr.

The following parameters are user-configurable parameters of Nz2Nz (the destination stride parameter is named dst_stride_dst_d in 2201, and is uniformly referred to as dst_stride in this document). For their meanings and value ranges, see [C API move-out parameters](../asc_copy_l0c2gm/asc_copy_l0c2gm.md):

- m_size and n_size indicate the dimensions of the cube in the M and N directions, respectively.
- src_stride indicates the interval between different Z fractal columns in the source Nz cube, in units of C0\_Size (C0\_Size=16\*sizeof\(T\), where T is the data type of the operand).
- dst_stride indicates the interval between different Z fractal columns in the destination Nz cube, in units of 32 bytes.
<!-- end id1 -->

<!-- npu="950" id4 -->
Taking the Ascend 950PR&950DT products as an example, the Nz2Nz conversion process can be referenced from the following pseudo-code (src/dst and temporary addresses are all counted as byte addresses):

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

The parameters used in the pseudo-code are described as follows:

- src indicates the start address from which the source operand is moved.
- dst indicates the start address to which the destination operand is moved.
- The start address of each row of the Nz cube in the source operand is src\_temp\_n\_addr.
- The start address of each row of the Nz cube in the destination operand is dst\_temp\_n\_addr.
- In each row, data blocks are segmented at a granularity of 16 data elements. The start address of each data block in the source operand is src\_block\_addr, and the start address of each data block in the destination operand is dst\_block\_addr.

The following parameters are user-configurable parameters of Nz2Nz. For their meanings and value ranges, see [C API move-out parameters](../asc_copy_l0c2gm/asc_copy_l0c2gm.md):

- m_size and n_size represent the dimensions of the cube in the M and N directions, respectively.
- src_stride represents the interval between different Z fractal columns in the source Nz cube, in units of C0\_Size (C0\_Size=16\*sizeof\(T\), where T is the data type of the operand).
- dst_stride represents the interval between different Z fractal columns in the destination Nz cube, in units of elements.
<!-- end id4 -->

## Constraints

- When Nz2Nz is enabled together with the F32 Channel Split feature, the small fractal dimension in the Nz fractal moved out to GM is 16\*8, ensuring that the small z fractal size is 512B.
- For Nz output of the B8 data type, Channel Merge is enabled by default, and the small fractal dimension moved out to the GM/L1 Buffer is 16\*32, ensuring that the small z fractal size is 512B.
- For Nz output of the B4 data type, Channel Merge is enabled by default, and the small fractal dimension is 16\*64, ensuring that the small z fractal size is 512B.
- Destination operand addresses must not overlap.

## Exampless

As shown in the following figure, there is an Nz-format cube in the L0C Buffer. The cube has a dimension of 48 in the N direction and 24 in the M direction. After Nz2Nz conversion, the data format becomes the layout shown in the right part.

<!-- npu="A3,910b" id7 -->
**Figure 1** Nz2Nz diagram ([NPU architecture version 2201](../../../../../guide/programming_guide/language_extension/simd_builtin_keywords.md))

![](../../../../figures/fixpipe_nz2nz_c_api.png)

The parameter setting example in the diagram (moving data through the L0C Buffer move-out API and removing dummy data) and the explanation are as follows:

- When the number of data elements in the M direction is not a multiple of 16, the source fractal may contain padded dummy data. The move-out selects valid rows based on m_size, and dummy rows are not written to the destination position. A cube block is defined as a continuous 16\*16 data block. The number of data blocks is M/16 rounded up, and the length of a cube block is M\*16\*sizeof\(T\), where T is the data type.
- n_size=48, indicating that the size of the cube to be moved in the source Nz cube in the N direction is 48 elements.
- m_size=24, indicating that the size of the cube to be moved in the source Nz cube in the M direction is 24 elements.
- src_stride=64, indicating the start address offset between adjacent Z layouts of the cube to be moved in the source Nz cube, that is, the interval between the start address of the first Z layout and the start address of the second Z layout in the following figure is 64 \* C0\_Size.
- dst_stride = 40, indicating the start address offset between adjacent Z layouts in the destination Nz cube, that is, the interval between the start address of the first Z layout and the start address of the second Z layout in the following figure is 40\*32 bytes.
<!-- end id7 -->

<!-- npu="950" id8 -->
**Figure 2** Nz2Nz diagram ([NPU architecture version 3510](../../../../../guide/programming_guide/language_extension/simd_builtin_keywords.md))

![](../../../../figures/fixpipe_nz2nz_c_api_950.png)

The following is an example of parameter settings in the diagram (moving data out through the L0C Buffer move-out API and removing dummy data) and the explanation:

- When the number of data elements in the M direction is not a multiple of 16, the source fractal may contain padded dummy data. During move-out, valid rows are selected based on m_size, and dummy rows are not written to the destination. A cube block is defined as a contiguous data block of 16\*16, the number of cube blocks is M/16 rounded up, and the length of a cube block is M\*16\*sizeof\(T\), where T is the data type.
- n_size=48, indicating that the size of the cube to be moved in the source Nz cube in the N direction is 48 elements.
- m_size=24, indicating that the size of the cube to be moved in the source Nz cube in the M direction is 24 elements.
- src_stride=64, indicating the start address offset between adjacent Z layouts of the cube to be moved in the source Nz cube, that is, the interval between the start address of the first Z layout and the start address of the second Z layout in the following figure is 64 \* C0\_Size.
- dst_stride = 40 * 16, indicating the start address offset between adjacent Z layouts in the destination Nz cube, that is, the interval between the start address of the first Z layout and the start address of the second Z layout in the following figure is 40\*16 elements.
<!-- end id8 -->
