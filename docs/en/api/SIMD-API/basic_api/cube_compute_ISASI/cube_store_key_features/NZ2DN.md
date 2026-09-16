# NZ2DN<a name="ZH-CN_TOPIC_0000002550560514"></a>

<!-- md-trans-meta sourceCommit=e9bec8b34ada6c37c20391d212da5ebf088e2e8b translatedAt=2026-08-27T15:17:22.771Z -->

## Description

During the cube computation move-out process, the core function of **NZ2DN** is to convert an NZ-format cube into a DN-format cube.

The **NZ2DN** conversion process can be referenced by the following pseudocode:

```cpp
for (int h = 0; h < dnNum; h++) {
    src_tmp3_addr = src + h ∗ 16 ∗ sizeof(src_element) ∗ srcNzCubeStride;
    dst_tmp3_addr = dst + h ∗ sizeof(dst_element) ∗ dstDnCubeStride;
    for (int i = 0; i < ceil(nSize / 16); i++) {
        src_tmp2_addr = src_tmp3_addr + i ∗ sizeof(src_element) ∗ 16 ∗ srcStride;
        dst_tmp2_addr = dst_tmp3_addr + i ∗ sizeof(dst_element) ∗ 16 ∗ dstStride;
        for (int j = 0; j < 16; j++) {
            if (i ∗ 16 + j < nSize) {
                src_block_addr = src_tmp2_addr + j ∗ sizeof(src_element);
                dst_block_addr = dst_tmp2_addr + j ∗ sizeof(dst_element) ∗ dstStride;
                for (int k = 0; k < mSize; k++) {
                    src_ele_addr = src_block_addr + k ∗ sizeof(src_element) ∗ 16 ∗ srcNzC0Stride;
                    dst_ele_addr = dst_block_addr + k ∗ sizeof(dst_element);
                }
            }
        }
    }
}
```

The parameters used in the pseudocode are described as follows:

- **src** indicates the start address of the source operand transfer.
- **dst** indicates the start address of the destination operand transfer.
- The start address of each NZ cube of the source operand is **src_tmp3_addr**.
- The start address of each DN cube of the destination operand is **dst_tmp3_addr**.
- The NZ cube of the source operand is divided into groups of 16 columns, and the start address of each group of data is **src_tmp2_addr**.
- The DN cube of the destination operand is divided into groups of 16 rows, and the start address of each group of data is **dst_tmp2_addr**.
- In each group of numbers derived from the source NZ cube, each column of elements is further treated as a group of numbers, and the start address of each group of numbers is **src_block_addr**.
- In each group of numbers derived from the destination DN cube, each row of elements is further treated as a group of numbers, and the start address of each group of numbers is **dst_block_addr**.
- **src_ele_addr** is the address of each element in a column of the NZ cube, and **dst_ele_addr** is the address of each element in a row of the DN cube. The two are in a one-to-one correspondence, which completes the NZ2DN format conversion.

The following parameters are user-configurable parameters for NZ2DN. For their meanings and value ranges, see [Fixpipe Transfer Parameters (FixpipeParamsArch3510)](../cube_compute_store/Fixpipe_L0CToGM.md#parameters):

- **dnNum** indicates the number of matrices to be converted from NZ to DN.
- **mSize** and **nSize** indicate the dimensions of the cube in the m direction and n direction, respectively.
- **srcStride** indicates the interval between the start addresses of adjacent Z layouts in the source NZ cube, with the unit being **C0_Size**.
- **dstStride** indicates the number of elements in each row of the destination DN cube, with the unit being **element**.
- **srcNzCubeStride** indicates the interval between the start addresses of different source NZ matrices, with the unit being **C0_Size**.
- **dstDnCubeStride** indicates the interval between the start addresses of adjacent destination DN matrices, with the unit being **element**.
- **srcNzC0Stride** indicates the address offset between adjacent rows of the source cube NZ fractal, and the unit is **C0_Size**.

## Constraints

- When the Channel Split/Channel Merge function (**NZ2NZ**) is enabled, this function is unavailable.
- When the dual-destination mode is enabled for moving the L0C Buffer to the UB, this function is unavailable.
- The destination operand addresses must not overlap.

## Example

As shown in the following figure, there are two matrices in NZ format in the L0C Buffer. Each cube has a dimension of 32 in the N direction and 48 in the M direction. After NZ2DN conversion, the data format becomes the layout shown in the right part.

**Figure 1** NZ2DN diagram<a name="fig3981319172211"></a>  

![](../../../../figures/fixpipe_nz2dn.png)

For this scenario, the effect shown in the figure can be achieved by configuring the parameters of the **intriParams** structure:

- **dnNum** = 2, indicating that the number of matrices to be converted by NZ2DN is 2.
- **nSize** = 32, indicating that the size of the source NZ cube in the N direction is 32 elements.
- **mSize** = 48, indicating that the size of the source NZ cube in the M direction is 48 elements.
- **srcStride** = 80, indicating that the interval between the start addresses of adjacent Z layouts in the source NZ cube is 80 \* **C0_Size** (**C0_Size** = 16\*sizeof(T), where T is the data type of the operand) bytes.
- **dstStride** = 80, indicating that the number of elements in each row of the destination DN cube is 80.
- **srcNzCubeStride** = 240, indicating that the interval between the start addresses of different source NZ matrices is 240 \* C0\_Size.
- **dstDnCubeStride** = 48 \* 80, indicating that the interval between the start addresses of adjacent destination DN matrices is 48 \* 80 = 3840 elements, where 40 is the number of interval rows and 80 is **dstStride** (the number of elements per row).
- **srcNzC0Stride** = 1: indicating that the address offset between adjacent rows of the source cube NZ fractal is 1 \* C0\_Size.
