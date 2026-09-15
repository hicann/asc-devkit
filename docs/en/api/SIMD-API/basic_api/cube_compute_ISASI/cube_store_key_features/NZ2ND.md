# NZ2ND<a name="ZH-CN_TOPIC_0000002568950981"></a>

<!-- md-trans-meta sourceCommit=unknown translatedAt=2026-08-31T11:37:53.436Z -->

## Description

When a cube is moved out, the NZ2ND function implements cube conversion from the NZ format to the ND format.

The NZ2ND conversion process can be referenced by the following pseudocode:

```cpp
for (i = 0; i < ndNum; i++) {
    // fractal_size = 16 * sizeof(L0C_element)
    src_temp_nd_addr = src + srcNdStride * fractal_size * i;
    dst_temp_nd_addr = dst + dstNdStride * sizeof(dst_element) * i;
    for (j = 0; j < mSize; j++) {
        src_temp_n_addr = src_temp_nd_addr + j * 16 * sizeof(L0C_element);
        dst_temp_n_addr = dst_temp_nd_addr + j * dstStride * sizeof(dst_element);
        for (k = 0; k < ceil(nSize / 16); k++) {
            src_block_addr = src_temp_n_addr + k * srcStride * 16 * sizeof(L0C_element);
            dst_block_addr = dst_temp_n_addr + k * 16 * sizeof(dst_element);
        }
    }
}
```

The parameters used in the pseudocode are described as follows:

- **src** indicates the start address from which the source operand is moved.
- **dst** indicates the start address to which the destination operand is moved.
- The start address of each NZ cube in the source operand is **src_temp_nd_addr**.
- The start address of each ND cube in the destination operand is **dst_temp_nd_addr**.
- The start address of each row in each NZ cube in the source operand is **src_temp_n_addr**.
- The start address of each row in each ND cube in the destination operand is **dst_temp_n_addr**.
- In each row, 16 data elements form one data block. The start address of each data block in the source operand is **src_block_addr**, and the start address of each data block in the destination operand is **dst_block_addr**.

The following parameters are user-configurable parameters of NZ2ND. For their meanings and value ranges, see [Fixpipe transfer parameters](../cube_compute_store/Fixpipe_L0CToGM.md) and [Parameter description](../cube_store_aux_config/SetFixpipeNz2ndFlag.md):

- **mSize** and **nSize** indicate the dimensions of the cube in the m direction and n direction, respectively.
- **srcStride** indicates the interval between different Z fractal columns in the NZ cube, in units of **C0_Size**.
- **dstStride** indicates the interval between different rows in the target ND cube, in units of elements.
- **ndNum** indicates the number of matrices that require NZ2ND conversion.
- **srcNdStride** indicates the interval between different NZ matrices, in units of 16\***C0_Size** (**C0_Size**=16\*sizeof(**T**), where **T** is the data type of the operand) bytes.
- **dstNdStride** indicates the interval between different target ND matrices, in units of elements.

## Constraints

- When the Channel Split/Channel Merge function (**NZ2NZ**) is enabled, this function is unavailable.
- The addresses of destination operands must not overlap.

## Example

As shown in the following figure, there are two NZ-format matrices in the L0C Buffer. Each cube has a dimension of 32 in the N direction and 48 in the M direction. After NZ2ND conversion, the data format is changed to the layout shown in the right part.

**Figure 1** NZ2ND diagram<a name="zh-cn_topic_0000002547300777_fig862819576334"></a>  

![](../../../../figures/Fixpipe_NZ2ND.png)

For this scenario, the effect shown in the preceding figure can be achieved by configuring the parameters of the **intriParams** structure:

- **ndNum** = 2, indicating that the number of matrices to be converted by NZ2ND is 2.
- **nSize** = 32, indicating that the size of the source NZ cube in the N direction is 32 elements.
- **mSize** = 48, indicating that the size of the source NZ cube in the M direction is 48 elements.
- **srcStride** = 64, indicating that the interval between the start addresses of adjacent Z layouts in the source NZ cube is 64 \* C0\_Size\(C0\_Size = 16\*sizeof\(T\), where T is the data type of the operand\) bytes.
- **dstStride** = 64, indicating that the number of elements in each row of the destination ND cube is 64.
- **srcNdStride** = 256, indicating that the interval between the start addresses of different NZ matrices is 256 * **C0_Size** bytes.
- **dstNdStride** = 4096, indicating that the offset between the start addresses of adjacent destination ND matrices is 4096 elements.
