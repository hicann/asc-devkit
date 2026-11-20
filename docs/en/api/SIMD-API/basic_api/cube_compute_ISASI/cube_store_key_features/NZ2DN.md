# NZ2DN<a name="ZH-CN_TOPIC_0000002550560514"></a>

<!-- md-trans-meta sourceCommit=7ef8ad7bdbdb0cef0f561d391759ecb8b4e4043a translatedAt=2026-11-02T04:25:53.907Z pushedAt=2026-11-18T01:25:06.791Z nl=2 -->

## Description

During the move-out process of matrix computation, the core function of **NZ2DN** is to convert an **NZ**-format matrix into a **DN**-format matrix.

The **NZ2DN** conversion process can be referenced by the following pseudocode:

```cpp
for (int h = 0; h < dnNum; h++) {
    src_tmp3_addr = src + h * 16 * sizeof(src_element) * srcNzMatrixStride;
    dst_tmp3_addr = dst + h * sizeof(dst_element) * dstDnMatrixStride;
    for (int i = 0; i < ceil(nSize / 16); i++) {
        src_tmp2_addr = src_tmp3_addr + i * sizeof(src_element) * 16 * srcStride;
        dst_tmp2_addr = dst_tmp3_addr + i * sizeof(dst_element) * 16 * dstStride;
        for (int j = 0; j < 16; j++) {
            if (i * 16 + j < nSize) {
                src_block_addr = src_tmp2_addr + j * sizeof(src_element);
                dst_block_addr = dst_tmp2_addr + j * sizeof(dst_element) * dstStride;
                for (int k = 0; k < mSize; k++) {
                    src_ele_addr = src_block_addr + k * sizeof(src_element) * 16 * srcNzC0Stride;
                    dst_ele_addr = dst_block_addr + k * sizeof(dst_element);
                }
            }
        }
    }
}
```

The parameters used in the pseudocode are described as follows:

- **src** indicates the start address from which the source operand is moved.
- **dst** indicates the start address to which the destination operand is moved.
- The start address of each **NZ** matrix of the source operand is src\_tmp3\_addr.
- The start address of each **DN** matrix of the destination operand is dst\_tmp3\_addr.
- The source operand **NZ** matrix is divided into groups of 16 columns, and the start address of each group of data is src\_tmp2\_addr.
- The destination operand **DN** matrix is divided into groups of 16 rows, and the start address of each group of data is dst\_tmp2\_addr.
- In each group of data from the source NZ matrix described above, each column of elements is further treated as a group of data, and the start address of each group of data is src\_block\_addr.
- In each group of data from the destination DN matrix described above, each row of elements is further treated as a group of data, and the start address of each group of data is dst\_block\_addr.
- src\_ele\_addr is the address of each element in a column of the NZ matrix, and dst\_ele\_addr is the address of each element in a row of the DN matrix. The two are in a one-to-one correspondence, which completes the NZ2DN format conversion.

The following parameters are user-configurable parameters for NZ2DN. For their meanings and value ranges, see [Fixpipe transfer parameters (FixpipeParamsArch3510)](../cube_compute_store/Fixpipe_L0CToGM.md#parameters):

- **dnNum** indicates the number of matrices to be converted from NZ to DN.
- **mSize** and **nSize** indicate the dimensions of the matrix in the m direction and n direction, respectively.
- **srcStride** indicates the interval between the start addresses of adjacent Z layouts in the source NZ matrix, and the unit is C0\_Size.
- **dstStride** indicates the number of elements in each row of the destination DN matrix, and the unit is element.
- **srcNzMatrixStride** indicates the interval between the start addresses of different source NZ matrices, and the unit is C0\_Size.
- **dstDnMatrixStride** indicates the interval between the start addresses of adjacent destination DN matrices, and the unit is element.
- **srcNzC0Stride** indicates the address offset between adjacent rows of the source matrix NZ fractal, in C0_Size.

## Constraints

- When the Channel Split/Channel Merge feature (NZ2NZ) is enabled, this feature is unavailable.
- When dual-destination mode is enabled for transferring the L0C Buffer to the Unified Buffer (UB), this function is unavailable.
- The destination operand addresses must not overlap.

## Example

As shown in the following figure, there are two NZ-format matrices in the L0C Buffer. Each matrix has a dimension of 32 in the N direction and 48 in the M direction. After NZ2DN conversion, the data format is changed to the layout shown in the right part.

**Figure 1** NZ2DN diagram<a name="fig3981319172211"></a>  

![](../../../../figures/fixpipe_nz2dn.png)

For this scenario, the effect shown in the preceding figure can be achieved by configuring the parameters of the **intriParams** structure:

- **dnNum** = 2, indicating that the number of matrices to be converted by NZ2DN is 2.
- **nSize** = 32, indicating that the size of the source NZ matrix in the N direction is 32 elements.
- **mSize** = 48, indicating that the size of the source NZ matrix in the M direction is 48 elements.
- **srcStride** = 80, indicating that the interval between the start addresses of adjacent Z layouts in the source NZ matrix is 80 \* C0\_Size\(C0\_Size = 16\*sizeof\(T\), where T is the data type of the operand \) bytes.
- **dstStride** = 80, indicating that the number of elements in each row of the destination DN matrix is 80.
- **srcNzMatrixStride** = 240, indicates that the interval between the start addresses of different source NZ matrices is 240 \* C0\_Size.
- **dstDnMatrixStride** = 48 \* 80, indicates that the interval between the start addresses of adjacent destination DN matrices is 48 \* 80 = 3840 elements, where 48 is the number of interval rows and 80 is **dstStride** (the number of elements per row).
- **srcNzC0Stride** = 1: indicates that the address offset between adjacent rows of the source matrix NZ fractal is 1 \* C0\_Size.
