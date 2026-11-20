---
okftype: reference.concept
description: "Introduces the auxiliary cube fractal format: on Ascend 950PR&950DT, the MX quantization scale cube scaleA must be arranged by Zz in L1 with an L0A_MX fractal of 16x2, and scaleB must be arranged by Nn with an L0B_MX fractal of 2x16, both using the fp8_e8m0_t data type; on Atlas A2/A3, the 4-of-2 sparse index cube is arranged by Zn, and the index encoding rules and related interface entry points are provided."
---
# Auxiliary Cube Fractal Format Details

<!-- npu="950" id1 -->
<!-- md-trans-meta sourceCommit=54f1d966cfcc22290ddc2c6312bbe4c6ac2a9232 translatedAt=2026-11-16T09:58:33.754Z pushedAt=2026-11-18T08:15:34.212Z nl=1 -->

## MX Coefficient Cube

For Ascend 950PR&950DT products:

Floating-point numbers are widely used in scientific computing, image processing, neural networks, and other fields. Taking AI training as an example, existing floating-point formats either have insufficient numerical range or low precision, which affects model fusion speed and performance. If both numerical range and precision requirements must be met simultaneously, memory usage becomes excessive, thereby increasing the cost of data storage and transfer. In view of this situation, the industry has proposed a new floating-point format—the Microscaling (MX) format. Floating-point numbers in the MX format can support AI training and inference at lower bit widths while occupying less memory. Data formats compliant with the MX standard can achieve robust AI training and inference model accuracy when using 8-bit or lower bit widths.

The MX format is a block data format in which several data elements can form a block (or a group), and data is organized in units of blocks. Data in the MX format consists of three parts:

- A shared scale factor X with a bit width of w bits;
- Private elements P<sub>i</sub> with a bit width of d bits;
- A block size k, indicating how many low-bit data elements form a block;

All k elements P<sub>i</sub> have the same bit width and data type, and share a single scale factor X. Each block containing k elements can be encoded using (w+k\*d) bits. The data type of the elements and the scale factor can be selected independently.

The following figure shows the data structure of floating-point numbers in the MX format, where S, E, and M represent the sign, exponent, and mantissa field values of a floating-point number, respectively. The shared scale factor X is a scale factor applied to the entire data block, and it determines the dynamic range of all elements in the data block. By introducing a shared scale factor, data in the MX format can flexibly represent data of different ranges while maintaining a low bit width. The block size k refers to the number of low-bit data elements that form a data block (or group). Private elements P<sub>i</sub> refer to each low-bit data element in the data block. After being adjusted by the scale factor X, these elements jointly represent a high-precision floating-point number or integer.

**Figure 1** Schematic diagram of MX format composition  
![](../../../../figures/mx_format.png "Schematic diagram of MX format composition")

MX cube multiplication is cube multiplication with quantization scale X, that is, both the left cube and the right cube have corresponding quantization scale cubes: the left quantization scale cube scaleA and the right quantization scale cube scaleB. The calculation formula is: C = \(scaleA ⊗ A\) \* \(scaleB ⊗ B\) + Bias, where "⊗" represents broadcast multiplication. When the left/right cube is multiplied with the left/right quantization scale cube, every 32 elements in the K direction share one quantization factor. As shown in the following figure, A is the left cube with shape \[M, K\]; scaleA is the left quantization scale cube with shape \[M, K/32\]; B is the right cube with shape \[K, N\]; scaleB is the right quantization scale cube with shape \[K/32, N\].

**Figure 2** Schematic diagram of MX cube multiplication  
![](../../../../figures/mx_matrix_computation_demo.png "Schematic diagram of MX cube multiplication")

- **Introduction to MX scaleA coefficient cube fractal**

    - **Physical location:** Global Memory/L1 Buffer/Unified Buffer (UB): saves offline/online generated left cube quantization scale data.

    - **Design principle:** In MX cube multiplication, the A cube and the scaleA cube have a one-to-one mapping in physical addresses. The asc_copy_l12l0a_mx/asc_copy_l12l0b_mx (coefficient transfer) interface requires that the scaleA cube in the L1 Buffer satisfies row reading requirements. Therefore, in the L1 Buffer, the scaleA cube is arranged by rows in a small-z large-Z layout (Zz).

    - **Format conversion process:** Assume the original A cube size is (M, K), then the corresponding scaleA cube size is (M, K/32). The scaleA cube is divided into M1 \* K1 fractals, arranged in row-major order, in a Z-shaped pattern; each fractal contains internally M0 \* K0 elements, arranged in row-major order, with a shape such as a Z shape, so this data format is called the Zz format. Here, \(M0, K0\) represents the shape of a fractal, the fractal must match the physical memory layout of the L0A_MX Buffer, and the data type must be fp8_e8m0_t, with a shape of 16x2.

        The conversion process can be expressed by the following formula:

        ```text
        (…, B, M, K/32)->pad->(…, B, M1 * M0, K1 * K0)->reshape->(…, B, M1, M0, K1, K0)->transpose->(…, B, M1, K1, M0, K0)
        ```

    - **Fractal size constraint:** The data type is fp8\_e8m0\_t, and the fractal size is 16x2 (M0, K0)

    As shown in the following figure, if the scaleA cube on GM is in ND layout, it can be transferred to the L1 Buffer through the ND2Nz.b16/DN2Nz.b16 instructions to ensure that the fractal in the L1 Buffer is in Zz layout, satisfying the asc_copy_l12l0a_mx/asc_copy_l12l0b_mx (coefficient transfer) interface requirement.

    **Figure 3** Layout format of the scaleA cube at different positions  
    ![](../../../../figures/mx_scaleA_format.png "Layout format of the scaleA cube at different positions")

- **Introduction to MX scaleB coefficient cube fractal**

    - **Physical location:** Global Memory/L1 Buffer/UB: saves offline/online generated right cube quantization scale data.

    - **Design principle:** In MX cube multiplication, the B cube and the scaleB cube have a one-to-one mapping in physical addresses. The asc_copy_l12l0a_mx/asc_copy_l12l0b_mx (coefficient transfer) interface requires that the scaleB cube on the L1 Buffer satisfy column reading requirements. Therefore, on the L1 Buffer, the scaleB cube is arranged by column storage in a large-N small-n layout (Nn).

    - **Format conversion process:** Assume the original B cube size is (K, N), then the corresponding scaleB cube size is (K/32, N). The scaleB cube is divided into K1 \* N1 fractals, arranged in column-major order, with an N-like shape. Each fractal contains K0 \* N0 elements internally, arranged in column-major order, with an n-like shape, so this data format is called the Nn format. Here, (K0, N0) represents the shape of a fractal, and the fractal needs to be consistent with the L0B_MX Buffer physical memory layout, and the data type must be fp8_e8m0_t, with a shape of 2x16.

        The conversion process can be expressed by the following formula:

        ```text
        (…, B, K/32, N)->pad->(…, B, K1 * K0, N1 * N0)->reshape->(…, B, K1, K0, N1, N0)->transpose->(…, B, N1, K1, N0, K0)
        ```

    - **Fractal size constraint:** The data type is fp8\_e8m0\_t, and the fractal size is 2x16 (K0, N0)

    As shown in the following figure, if the scaleB cube on Global Memory (GM) is in ND layout, it can be transferred to the L1 Buffer through the ND2Nz.b16/DN2Nz.b16 instructions, ensuring that the fractal on the L1 Buffer is in Nn layout and satisfies the asc_copy_l12l0a_mx/asc_copy_l12l0b_mx (coefficient transfer) interface requirement.

    **Figure 4** Layout format of the scaleB cube at different positions  
    ![](../../../../figures/mx_scaleB_format.png "Layout format of the scaleB cube at different positions")

For the coefficient transfer interface, see [asc_copy_l12l0a_mx](../../cube_datamove/cube_compute_load/asc_copy_l12l0a_mx.md) and [asc_copy_l12l0b_mx](../../cube_datamove/cube_compute_load/asc_copy_l12l0b_mx.md).
<!-- end id1 -->

<!-- npu="A3,910b" id2 -->

## 2-Out-of-4 Sparse Index Cube

For Atlas A2 products and Atlas A3 products:

- Definition: The index cube generated during the densification of the original B cube. Assume that every 4 elements of the original B cube contain at least 2 zero elements, and the densified B cube is a dense cube with the 2 zeros filtered out.
- Scenario: It usually resides in Global Memory (GM). Before performing sparse cube operations, it usually needs to be generated offline and converted to the large-Z small-n format (Zn) to adapt to the Cube unit.
- Generation process: The index cube generated during the densification process is shown in the following figure. Assume that every 4 elements of the original B cube contain at least 2 zero elements, and the densified B cube is a dense cube with the 2 zeros filtered out. For every 4 elements in the original cube B, two 2-bit indices are generated in the index cube and encoded according to the following rules. The indices must be within the range of \{0, 1, 2\}.

    **Figure 5** Schematic diagram of index generation  
    ![](../../../../figures/index_generation.png "Schematic diagram of index generation")
    - The first index is used to indicate the relative position of the first non-zero element among the first 3 elements.
    - The second index is used to indicate the relative position of the second non-zero element among the last 3 elements.
    - One uint8 element in the index cube is composed of four uint2\_t. For example, in the figure above: the generated index data is 1 2 0 1, and the layout of the index data in the Global Memory (GM) address from high bit to low bit is 1 0 2 1, where 1 0 2 1 (corresponding to the first four bits 1 2 0 1 of the index cube) is one uint8\_t.

**Figure 6** Schematic diagram of the densified B cube  
![](../../../../figures/density_B_matrix_demo.png "Schematic diagram of the densified B cube")

Here, "densification" refers to 4-choose-2 compression: for every 4 original elements, 2 storage slots are retained, and the position index is recorded; the densified B cube below is the compressed B cube. For specific rules, refer to the following table. Here, "-" indicates that the value at that position is not of concern, because it will be filtered out during the subsequent Sparse asc_mmad computation.

<table><thead>
<tr><th>Example</th><th>B cube element 0</th><th>B cube element 1</th><th>B cube element 2</th><th>B cube element 3</th><th>Index data 0</th><th>Index data 1</th>
</tr></thead><tbody>
<tr><td rowspan="6">Two non-zero elements among the four elements</td>
<td>0</td><td>0</td><td>X</td><td>Y</td><td>2'b10</td><td>2'b10</td></tr>
<tr><td>0</td><td>X</td><td>0</td><td>Y</td><td>2'b01</td><td>2'b10</td></tr>
<tr><td>X</td><td>0</td><td>0</td><td>Y</td><td>2'b00</td><td>2'b10</td></tr>
<tr><td>0</td><td>X</td><td>Y</td><td>-</td><td>2'b01</td><td>2'b01</td></tr>
<tr><td>X</td><td>0</td><td>Y</td><td>-</td><td>2'b00</td><td>2'b01</td></tr>
<tr><td>X</td><td>Y</td><td>-</td><td>-</td><td>2'b00</td><td>2'b00</td></tr>
<tr><td rowspan="4">One non-zero element among the four elements</td>
<td>0</td><td>0</td><td>0</td><td>X</td><td>2'b00</td><td>2'b10</td></tr>
<tr><td>0</td><td>0</td><td>X</td><td>0</td><td>2'b10</td><td>2'b00</td></tr>
<tr><td>0</td><td>X</td><td>0</td><td>0</td><td>2'b01</td><td>2'b00</td></tr>
<tr><td>X</td><td>0</td><td>0</td><td>0</td><td>2'b00</td><td>2'b00</td></tr>
<tr><td>No non-zero element among the four elements</td>
<td>0</td><td>0</td><td>0</td><td>0</td><td>2'b00</td><td>2'b00</td>
</tr></tbody></table>

- **Physical location:** GM/L1 Buffer: saves the offline-generated index cube data.

- **Design principle:** Column reading requirement: in sparse cube multiplication, the B cube and the Index cube have a one-to-one mapping in physical addresses. The asc_copy_l12l0b_sparse instruction requires that the B cube and the Index cube in the L1 Buffer satisfy the column reading requirement. Therefore, in the GM/L1 Buffer, the Index cube is arranged by column as a large-Z small-n layout (Zn).

- **Format conversion process:** Assume the original B cube size is (K, N). After 4-to-2 densification, the B cube and the Index cube size is (K/2, N). The Index cube is divided into K1 \* N1 fractals, arranged in row-major order, with a Z-like shape. Each fractal contains N0 \* K0 elements internally, arranged in column-major order, with a N-like shape. Therefore, this data format is called the Zn format. Here, \(N0, K0\) represents the shape of a fractal, and the fractal needs to be consistent with the B cube fractal. The B cube data type is int8\_t, and its small fractal shape is 16\*32. Each uint8\_t data in the Index cube fractal is concatenated from 4 uint2\_t elements, and its elements correspond one-to-one to the B cube element addresses. Therefore, the fractal shape is also 16\*32, but the size is 128 bytes.

    The conversion process can be expressed by the following formula:

    ```text
    (…, B, K/2, N)->pad->(…, B, K1 * K0, N1 * N0)->reshape->(…, B, K1, K0, N1, N0)->transpose->(…, B, K1, N1, N0, K0)
    ```

- **Fractal size constraint:** When stored as the uint8 data type, the fractal size is 16x8 (N0, K0).
<!-- end id2 -->

When preparing the Bias cube, see [asc_copy_l12bt](../../cube_datamove/cube_compute_load/asc_copy_l12bt.md) for the transfer interface, and see [asc_mmad](../mmad_compute/asc_mmad.md) for how Bias participates in computation; the Scale cube is used for [asc_mmad_mx](../mmad_compute/asc_mmad_mx.md), and for the loading and computation of the sparse weight and index cubes, see [asc_copy_l12l0b_sparse](../../cube_datamove/cube_compute_load/asc_copy_l12l0b_sparse.md) and [asc_mmad_sparse](../mmad_compute/asc_mmad_sparse.md) respectively.
