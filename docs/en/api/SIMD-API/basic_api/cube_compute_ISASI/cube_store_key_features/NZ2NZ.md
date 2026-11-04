# NZ2NZ<a name="ZH-CN_TOPIC_0000002538231204"></a>

<!-- md-trans-meta sourceCommit=754fb95859dead826727a591e674261c6a829a08 translatedAt=2026-09-23T13:16:37.263Z pushedAt=2026-10-26T07:41:44.177Z nl=2 -->

## Description

During the move-out process of matrix computation, the core function of NZ2NZ is to keep the NZ data arrangement unchanged before and after the matrix computation result is moved out.

<!-- npu="A3,910b" id1 -->
The following product models are used as examples:
<!-- npu="A3" id2 -->
Atlas A3 products
<!-- end id2 -->
<!-- npu="910b" id3 -->
Atlas A2 products
<!-- end id3 -->
The NZ2NZ conversion process can be referenced by the following pseudocode:

```cpp
for (j = 0; j < mSize; j++) {
    src_temp_n_addr = src + j * sizeof(L0C_element) * 16;
    dst_temp_n_addr = dst + j * 16 * sizeof(DST_element);
    for (k = 0; k < ceil(nSize / 16); k++) {
        src_block_addr = src_temp_n_addr + k * srcStride * sizeof(L0C_element) * 16;
        dst_block_addr = dst_temp_n_addr + k * dstStride * 32;
    }
}
```

The parameters used in the pseudocode are described as follows:

- **src** indicates the start address from which the source operand is moved.
- **dst** indicates the start address to which the destination operand is moved.
- The start address of each row of the NZ matrix in the source operand is src\_temp\_n\_addr.
- The start address of each row of the NZ matrix in the destination operand is dst\_temp\_n\_addr.
- In each row, data blocks are segmented at a granularity of 16 data elements. The start address of each data block in the source operand is src\_block\_addr, and the start address of each data block in the destination operand is dst\_block\_addr.

The following parameters are user-configurable parameters of NZ2NZ. For their meanings and value ranges, see [Fixpipe transfer parameters](../cube_compute_store/Fixpipe_L0CToGM.md):

- **mSize** and **nSize** indicate the dimensions of the matrix in the M direction and N direction, respectively.
- **srcStride** indicates the interval between different Z fractal columns in the source NZ matrix, with a unit of C0\_Size (C0\_Size=16\*sizeof\(T\), where **T** is the data type of the operand).
- **dstStride** indicates the interval between different Z fractal columns in the destination NZ matrix, with a unit of 32 bytes.
<!-- end id1 -->

<!-- npu="950" id4 -->
Taking the Ascend 950PR&950DT products as an example, refer to the following pseudocode for the NZ2NZ conversion process:

```cpp
for (j = 0; j < mSize; j++) {
    src_temp_n_addr = src + j * sizeof(L0C_element) * 16;
    dst_temp_n_addr = dst + j * 16 * sizeof(DST_element);
    for (k = 0; k < ceil(nSize / 16); k++) {
        src_block_addr = src_temp_n_addr + k * srcStride * sizeof(L0C_element) * 16;
        dst_block_addr = dst_temp_n_addr + k * dstStride * sizeof(DST_element);
    }
}
```

The parameters used in the pseudocode are described as follows:

- **src** indicates the start address from which the source operand is moved.
- **dst** indicates the start address from which the destination operand is moved.
- The start address of each row of the NZ matrix in the source operand is src\_temp\_n\_addr.
- The start address of each row of the NZ matrix in the destination operand is dst\_temp\_n\_addr.
- In each row, data blocks are divided at a granularity of 16 data elements. The start address of each data block in the source operand is src\_block\_addr, and the start address of each data block in the destination operand is dst\_block\_addr.

The following parameters are user-configurable parameters of NZ2NZ. For their meanings and value ranges, see [Fixpipe transfer parameters](../cube_compute_store/Fixpipe_L0CToGM.md):

- **mSize** and **nSize** indicate the dimensions of the matrix in the M and N directions, respectively.
- **srcStride** indicates the interval between different Z fractal columns in the source NZ matrix, in units of C0\_Size (C0\_Size=16\*sizeof\(T\), where T is the data type of the operand).
- **dstStride** indicates the interval between different Z fractal columns in the destination NZ matrix, in elements.
<!-- end id4 -->

## Constraints

- When NZ2NZ and the F32 Channel Split feature are enabled together, the small fractal dimension in the NZ fractal moved out to GM is 16\*8, ensuring the small z fractal size is 512B.
- For B8 data type NZ output, Channel Merge is enabled by default, and the small fractal dimension moved out to GM/L1 Buffer is 16\*32, ensuring the small z fractal size is 512B.
- For B4 data type NZ output, Channel Merge is enabled by default, and the small fractal dimension moved out to GM/L1 Buffer is 16\*64, ensuring the small z fractal size is 512B.
- The destination operand addresses must not overlap.

## Example

As shown in the figure below, there is an NZ-format matrix in the L0C Buffer. The matrix has a dimension of 48 in the N direction and 24 in the M direction. After NZ2NZ conversion, the data format becomes the arrangement shown in the right part.

<!-- npu="A3,910b" id6 -->
**Figure 1** NZ2NZ diagram ([NPU architecture version 2201](../../../../../guide/programming_guide/language_extension/simd_builtin_keywords.md))<a name="zh-cn_topic_0000002515820868_fig199191646174210"></a>  

![](../../../../figures/Fixpipe_NZ2NZ.png)

The parameter setting example in the diagram (moving and removing dummy data through the Fixpipe API) and the explanation are as follows:

- When the number of data elements in the M direction is not a multiple of 16, dummy data is additionally read during the move-in, and this dummy data is discarded after being written to the destination. A matrix block is defined as a continuous 16\*16 data block. The number of matrix blocks is M/16 rounded up, and the length of a matrix block is M\*16\*sizeof\(T\), where T is the data type.
- **nSize**=48, indicating that the size of the matrix to be moved in the source NZ matrix in the N direction is 48 elements.
- **mSize**=24, indicating that the size of the matrix to be moved in the source NZ matrix in the M direction is 24 elements.
- **srcStride**=64, indicating the start address offset between adjacent Z arrangements of the matrix to be moved in the source NZ matrix, that is, the interval between the start address of the first Z arrangement and the start address of the second Z arrangement in the figure below is 64 \* C0\_Size.
- **dstStride** = 40, indicating the start address offset between adjacent Z arrangements in the destination NZ matrix, that is, the interval between the start address of the first Z arrangement and the start address of the second Z arrangement in the figure below is 40\*32 bytes.
<!-- end id6 -->

<!-- npu="950" id5 -->
**Figure 2** NZ2NZ diagram ([NPU architecture version 3510](../../../../../guide/programming_guide/language_extension/simd_builtin_keywords.md))<a name="zh-cn_topic_0000002515820868_fig199191646174233"></a>  

![](../../../../figures/Fixpipe_NZ2NZ_950.png)

The parameter setting example in the diagram (moving and removing dummy data through the Fixpipe API) and the explanation are as follows:

- When the number of data elements in the M direction is not a multiple of 16, dummy data is additionally read during the move-in, and this dummy data is discarded after being written to the destination. A matrix block is defined as a contiguous 16\*16 data block. The number of data blocks is M/16 rounded up, and the length of a matrix block is M\*16\*sizeof\(T\), where T is the data type.
- **nSize**=48, indicating that the size of the matrix to be moved in the source NZ matrix in the N direction is 48 elements.
- **mSize**=24, indicating that the size of the matrix to be moved in the source NZ matrix in the M direction is 24 elements.
- **srcStride**=64, indicating the start address offset between adjacent Z arrangements of the matrix to be moved in the source NZ matrix, that is, as shown in the figure below, the interval between the start address of the first Z arrangement and the start address of the second Z arrangement is 64 \* C0\_Size.
- **dstStride** = 40 * 16, indicating the start address offset between adjacent Z arrangements in the destination NZ matrix, that is, as shown in the figure below, the interval between the start address of the first Z arrangement and the start address of the second Z arrangement is 40\*16 elements.
<!-- end id5 -->
