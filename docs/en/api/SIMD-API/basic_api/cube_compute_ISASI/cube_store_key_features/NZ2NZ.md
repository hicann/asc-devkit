# NZ2NZ<a name="ZH-CN_TOPIC_0000002538231204"></a>

<!-- md-trans-meta sourceCommit=e9bec8b34ada6c37c20391d212da5ebf088e2e8b translatedAt=2026-08-27T15:21:11.714Z -->

## Description

During the copy-out process of cube calculation, the core function of NZ2NZ is to keep the NZ data layout unchanged before and after the cube calculation result is copied out.

<!-- npu="A3,910b" id1 -->
The following product models are used as examples:
<!-- npu="A3" id2 -->
Atlas A3 training products/Atlas A3 inference products
<!-- end id2 -->
<!-- npu="910b" id3 -->
Atlas A2 training products/Atlas A2 inference products
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

- **src** indicates the start address for copying the source operand.
- **dst** indicates the start address for copying the destination operand.
- The start address of each row of the NZ cube in the source operand is **src_temp_n_addr**.
- The start address of each row of the NZ cube in the destination operand is `dst_temp_n_addr`.
- In each row, data blocks are segmented at a granularity of 16 data elements. The start address of each data block in the source operand is `src_block_addr`, and the start address of each data block in the destination operand is `dst_block_addr`.

The following parameters are user-configurable parameters of NZ2NZ. For their meanings and value ranges, see [Fixpipe copy parameters](../cube_compute_store/Fixpipe_L0CToGM.md):

- **mSize** and **nSize** indicate the dimensions of the cube in the M direction and N direction, respectively.
- **srcStride** indicates the interval between different Z fractal columns in the source NZ cube, with the unit being C0_Size (C0_Size = 16*sizeof(T), where T is the data type of the operand).
- **dstStride** indicates the interval between different Z fractal columns in the destination NZ cube, with the unit being 32 bytes.
<!-- end id1 -->

<!-- npu="950" id4 -->
Taking Ascend 950PR/Ascend 950DT as an example, the NZ2NZ conversion process can be referenced from the following pseudocode:

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

- **src** indicates the start address from which the source operand is copied.
- **dst** indicates the start address from which the destination operand is copied.
- The start address of each row of the NZ cube in the source operand is **src\_temp\_n\_addr**.
- The start address of each row of the NZ cube in the destination operand is **dst\_temp\_n\_addr**.
- In each row, data blocks are segmented at a granularity of 16 data elements. The start address of each data block in the source operand is **src\_block\_addr**, and the start address of each data block in the destination operand is **dst\_block\_addr**.

The following parameters are user-configurable parameters of **NZ2NZ**. For their meanings and value ranges, see [Fixpipe copy parameters](../cube_compute_store/Fixpipe_L0CToGM.md):

- **mSize** and **nSize** indicate the dimensions of the cube in the M and N directions, respectively.
- **srcStride** indicates the interval between different Z fractal columns in the source NZ cube, in units of **C0\_Size** (**C0\_Size** = 16 \* sizeof(T), where T is the data type of the operand).
- **dstStride** indicates the interval between different Z fractal columns in the destination NZ cube, in units of element.
<!-- end id4 -->

## Constraints

- When NZ2NZ is enabled together with the F32 Channel Split feature, the small fractal dimension in the NZ fractal copied out to GM is 16\*8, ensuring that the small z fractal size is 512B.
- For B8 data type NZ output, Channel Merge is enabled by default, and the small fractal dimension copied out to the GM/L1 Buffer is 16\*32, ensuring that the small z fractal size is 512B.
- For B4 data type NZ output, Channel Merge is enabled by default, and the small fractal dimension copied out to the GM/L1 Buffer is 16\*64, ensuring that the small z fractal size is 512B.
- The addresses of destination operands must not overlap.

## Example

As shown in the following figure, there is an NZ-format cube in the L0C Buffer. The cube has a dimension of 48 in the N direction and 24 in the M direction. After NZ2NZ conversion, the data format is changed to the arrangement shown in the right part.

<!-- npu="A3,910b" id6 -->
**Figure 1** NZ2NZ diagram ([NPU architecture version 2201](https://gitcode.com/cann/asc-devkit/blob/9.1.0/docs/en/guide/programming_guide/language_extension/SIMT-BuiltIn_keyword.md))<a name="zh-cn_topic_0000002515820868_fig199191646174210"></a>  

![](../../../../figures/Fixpipe_NZ2NZ.png)

The parameter setting example in the diagram (copying through the Fixpipe API and removing dummy data) and the explanation are as follows:

- When the number of data elements in the M direction is not a multiple of 16, dummy data is additionally read during the copy-in, and the dummy data is discarded after being written to the destination. A cube block is defined as a continuous 16\*16 data block. The number of cube blocks is M/16 rounded up, and the length of a cube block is M\*16\*sizeof\(T\), where T is the data type.
- **nSize**=48, which indicates that the size of the cube to be copied in the source NZ cube in the N direction is 48 elements.
- **mSize**=24, which indicates that the size of the cube to be copied in the source NZ cube in the M direction is 24 elements.
- **srcStride**=64, which indicates the start address offset between adjacent Z arrangements of the cube to be copied in the source NZ cube, that is, the interval between the start address of the first Z arrangement and the start address of the second Z arrangement in the following figure is 64 \* C0\_Size.
- **dstStride** = 40, which indicates the start address offset between adjacent Z arrangements in the destination NZ cube, that is, the interval between the start address of the first Z arrangement and the start address of the second Z arrangement in the following figure is 40\*32 bytes.
<!-- end id6 -->

<!-- npu="950" id5 -->
**Figure 2** NZ2NZ schematic diagram ([NPU architecture version 3510](https://gitcode.com/cann/asc-devkit/blob/9.1.0/docs/en/guide/programming_guide/language_extension/SIMT-BuiltIn_keyword.md))<a name="zh-cn_topic_0000002515820868_fig199191646174233"></a>  

![](../../../../figures/Fixpipe_NZ2NZ_950.png)

The parameter setting example in the schematic diagram (copying and removing dummy data through the Fixpipe API) and the explanation are as follows:

- When the number of data elements in the M direction is not a multiple of 16, dummy data is additionally read during copy-in and discarded after being written to the destination. A cube block is defined as a consecutive 16\*16 data block. The number of data blocks is M/16 rounded up, and the length of a cube block is M\*16\*sizeof\(T\), where T is the data type.
- **nSize**=48, indicating that the size of the cube to be copied in the source NZ cube in the N direction is 48 elements.
- **mSize**=24, indicating that the size of the cube to be copied in the source NZ cube in the M direction is 24 elements.
- **srcStride**=64, indicating the start address offset between adjacent Z arrangements of the cube to be copied in the source NZ cube, that is, the interval between the start address of the first Z arrangement and the start address of the second Z arrangement in the following figure is 64 \* C0\_Size.
- **dstStride** = 40 * 16, indicating the start address offset between adjacent Z arrangements in the destination NZ cube, that is, the interval between the start address of the first Z arrangement and the start address of the second Z arrangement in the following figure is 40\*16 elements.
<!-- end id5 -->
