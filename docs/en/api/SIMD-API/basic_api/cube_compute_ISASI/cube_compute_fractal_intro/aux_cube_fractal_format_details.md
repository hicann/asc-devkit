# Auxiliary Matrix Fractal Format Details<a name="ZH-CN_TOPIC_0000002538231118"></a>

<!-- npu="950" id1 -->
<!-- md-trans-meta sourceCommit=dacba287b3c94417af1e532d9cbd29c398889dea translatedAt=2026-11-02T02:47:43.372Z pushedAt=2026-11-18T01:25:06.675Z nl=1 -->

## MX Coefficient Matrix<a name="zh-cn_topic_0000002563445163_section648585413300"></a>

For Ascend 950PR&950DT products:

Floating-point numbers are widely used in scientific computing, image processing, neural networks, and other fields. Taking AI training as an example, existing floating-point formats either have an insufficient numerical range or low precision, which affects the convergence speed and performance of models. If both the numerical range and precision requirements must be met simultaneously, the memory footprint becomes excessively large, thereby increasing the cost of data storage and transmission. Based on this situation, the industry has proposed a new floating-point format, the Microscaling (MX) format. Floating-point numbers in the MX format can support AI training and inference at lower bit widths while occupying less memory. Data formats conforming to the MX standard can achieve robust AI training and inference model precision when using 8 bits or fewer.

The MX format is a block data format. Several data elements can form a block (or a group), and data is organized in units of blocks. Data in the MX format consists of three parts:

- A shared scaling factor X with a bit width of w bits;
- Private elements P<sub>i</sub> with a bit width of d bits;
- A block size k, indicating how many low-bit data elements form a block;

All k elements P<sub>i</sub> have the same bit width and data type and share one scaling factor X. Each block containing k elements can be encoded using (w+k\*d) bits. The data type of the elements and the scaling factor can be selected independently.

The following figure shows the data structure of floating-point numbers in the MX format. S, E, and M represent the sign, exponent, and mantissa field values of a floating-point number, respectively. The shared scaling factor X is a scaling factor applied to the entire data block, and it determines the dynamic range of all elements in the data block. By introducing a shared scaling factor, data in the MX format can flexibly represent data of different ranges while maintaining a low bit width. The block size k refers to the number of low-bit data elements that form a data block (or group). The private elements P<sub>i</sub> refer to each low-bit data element in the data block. After being adjusted by the scaling factor X, these elements jointly represent a high-precision floating-point number or integer.

**Figure 1** MX format composition diagram<a name="zh-cn_topic_0000002563445163_fig3385175571011"></a>  
![](../../../../figures/mx_format.png "MX format composition diagram")

MXMmad is matrix multiplication with quantized coefficient X, that is, both the left matrix and the right matrix have corresponding quantized coefficient matrices, namely the left quantized coefficient matrix scaleA and the right quantized coefficient matrix scaleB. The calculation formula is: C = \(scaleA ⊗ A\) \* \(scaleB ⊗ B\) + Bias, where "⊗" represents broadcast multiplication. When the left/right matrix is multiplied by the left/right quantized coefficient matrix, every 32 elements in the K direction share one quantization factor. As shown in the following figure, A is the left matrix with a shape of \[M, K\]; scaleA is the left quantized coefficient matrix with a shape of \[M, K/32\]; B is the right matrix with a shape of \[K, N\]; scaleB is the right quantized coefficient matrix with a shape of \[K/32, N\].

**Figure 2** MX matrix multiplication diagram<a name="zh-cn_topic_0000002563445163_fig33303533712"></a>  
![](../../../../figures/mx_matrix_computation_demo.png "MX matrix multiplication diagram")

- **Introduction to MX scaleA Coefficient Matrix Fractal**<a name="section_mx_scalea_fractal_format"></a>

    - **Physical location:** Global Memory/L1 Buffer/Unified Buffer (UB): stores offline/online generated left matrix quantization coefficient data.

    - **Design principle:** In MX matrix multiplication, matrix A and the scaleA matrix have a one-to-one mapping in physical addresses. The LoadData (MX matrix movement) API requires that the scaleA matrix in the L1 Buffer satisfies the row reading requirement. Therefore, in the L1 Buffer, the scaleA matrix is arranged in a small-z large-Z layout stored by row (Zz).

    - **Format conversion process:** Assume the original A matrix size is (M, K), then the corresponding scaleA matrix size is (M, K/32). The scaleA matrix is divided into M1 \* K1 fractals, arranged in row-major order, in a Z-shaped pattern; each fractal block internally has M0 \* K0 elements, arranged in row-major order and in a Z-shaped pattern, so this data format is called the Zz format. Here, \(M0, K0\) represents the shape of a fractal block; the fractal needs to be consistent with the physical memory arrangement of the L0A_MX Buffer, and the data type must be fp8_e8m0_t, with a shape of 16x2.

        The conversion process is expressed by the following formulas:

        ```py
        (…, B, M, K/32)->pad->(…, B, M1 * M0, K1 * K0)->reshape->(…, B, M1, M0, K1, K0)->transpose->(…, B, M1, K1, M0, K0)
        ```

    - **Fractal size constraint:** The data type is fp8\_e8m0\_t, and the fractal size is 16x2 (M0, K0).

    As shown in the following figure, if the scaleA matrix on GM is in ND arrangement, it can be moved to the L1 Buffer through the ND2NZ.b16/DN2NZ.b16 instruction to ensure that the fractal on the L1 Buffer is in Zz arrangement, satisfying the LoadData (MX matrix movement) API requirement.

    **Figure 3** Arrangement format of the scaleA matrix at different positions<a name="zh-cn_topic_0000002563445163_fig6871154411446"></a>  
    ![](../../../../figures/mx_scaleA_format.png "Arrangement format of the scaleA matrix at different positions")

- **Introduction to the MX scaleB Coefficient Matrix Fractal**<a name="section_mx_scaleb_fractal_format"></a>

    - **Physical location:** Global Memory/L1 Buffer/UB: stores the right matrix quantization coefficient data generated offline/online

    - **Design principle:** In MX matrix multiplication, the B matrix and the scaleB matrix have a one-to-one mapping in physical addresses. The LoadData (MX matrix movement) API requires that the scaleB matrix on the L1 Buffer satisfy the column reading requirement. Therefore, on the L1 Buffer, the scaleB matrix is arranged in column-major storage with a large N and small n layout (Nn).

    - **Format conversion process:** Assume the original B matrix size is (K, N). Then the corresponding scaleB matrix size is (K/32, N). The scaleB matrix is divided into K1 \* N1 fractals, arranged in column-major order, forming an N-like shape. Each fractal internally has K0 \* N0 elements, arranged in column-major order, forming an n-like shape. Therefore, this kind of data format is called the Nn format. Here, (K0, N0) represents the shape of a fractal. The fractal needs to be consistent with the physical memory layout of the L0B_MX Buffer, and the data type must be fp8_e8m0_t, with a shape of 2x16.

        The conversion process is expressed by the following formulas:

        ```py
        (…, B, K/32, N)->pad->(…, B, K1 * K0, N1 * N0)->reshape->(…, B, K1, K0, N1, N0)->transpose->(…, B, N1, K1, N0, K0)
        ```

    - **Fractal size constraint:** The data type is fp8\_e8m0\_t, and the fractal size is 2x16 (K0, N0).

    As shown in the following figure, if the scaleB matrix on Global Memory (GM) is in ND layout, it can be moved to the L1 Buffer through the ND2NZ.b16/DN2NZ.b16 instructions to ensure that the fractal on the L1 Buffer is in Nn layout, satisfying the LoadData (MX matrix movement) API requirement.

    **Figure 4** Arrangement format of the scaleB matrix at different positions<a name="zh-cn_topic_0000002563445163_fig599218260112"></a>  
    ![](../../../../figures/mx_scaleB_format.png "Arrangement format of the scaleB matrix at different positions")

For specific implementation, see: [MXmmad sample](../../../../../../../examples/01_simd_cpp_api/03_basic_api/03_matrix_compute/load_data_2dmx_l12l0).
<!-- end id1 -->

<!-- npu="A3,910b" id2 -->

## 4-choose-2 Sparse Index Matrix<a name="zh-cn_topic_0000002563445163_section1873692415233"></a>

For Atlas A2 products and Atlas A3 products:

- Definition: The index matrix generated during the densification of the original B matrix. Assume that every 4 elements of the original B matrix contain at least 2 zero elements, and the densified B matrix is a dense matrix with the 2 zeros filtered out.
- Scenario: It usually resides in Global Memory (GM). Before performing sparse matrix operations, it usually needs to be generated offline and converted to the large-Z small-n format (Zn) to adapt to the Cube unit.
- Generation process: As shown in the following figure, for every 4 elements in the original matrix B, 2 2-bit indices are generated in the index matrix and encoded according to the following rules. The indices must be within the range of \{0, 1, 2\}.
**Figure 5** Index generation diagram<a name="zh-cn_topic_0000002563445163_fig1423214854720"></a>  
![](../../../../figures/index_generation.png "Index generation diagram")
    - The first index indicates the relative position of the first non-zero element among the first 3 elements.
    - The second index indicates the relative position of the second non-zero element among the last 3 elements.
    - A uint8 element in the index matrix is composed of 4 uint2\_t. For example, in the figure above: the generated index data is 1 2 0 1, and in the Global Memory (GM) address, the index data is arranged from high-order bits to low-order bits as 1 0 2 1, where 1 0 2 1 (corresponding to the first four bits 1 2 0 1 of the index matrix) is a uint8\_t.

**Figure 6** Densified B matrix diagram<a name="zh-cn_topic_0000002563445163_fig1115556103916"></a>  
![](../../../../figures/density_B_matrix_demo.png "Densified B matrix diagram")

For the densification algorithm rules, refer to the following table. Here, "-" indicates that the value at that position does not matter, because it will be filtered out during the subsequent Sparse Mmad computation.

<table><thead>
<tr><th>Example</th><th>B matrix element 0</th><th>B matrix element 1</th><th>B matrix element 2</th><th>B matrix element 3</th><th>index data 0</th><th>index data 1</th>
</tr></thead><tbody>
<tr><td rowspan="6">Two of the four elements are non-zero elements</td>
<td>0</td><td>0</td><td>X</td><td>Y</td><td>2'b10</td><td>2'b10</td></tr>
<tr><td>0</td><td>X</td><td>0</td><td>Y</td><td>2'b01</td><td>2'b10</td></tr>
<tr><td>X</td><td>0</td><td>0</td><td>Y</td><td>2'b00</td><td>2'b10</td></tr>
<tr><td>0</td><td>X</td><td>Y</td><td>-</td><td>2'b01</td><td>2'b01</td></tr>
<tr><td>X</td><td>0</td><td>Y</td><td>-</td><td>2'b00</td><td>2'b01</td></tr>
<tr><td>X</td><td>Y</td><td>-</td><td>-</td><td>2'b00</td><td>2'b00</td></tr>
<tr><td rowspan="4">One of the four elements is a non-zero element</td>
<td>0</td><td>0</td><td>0</td><td>X</td><td>2'b00</td><td>2'b10</td></tr>
<tr><td>0</td><td>0</td><td>X</td><td>0</td><td>2'b10</td><td>2'b00</td></tr>
<tr><td>0</td><td>X</td><td>0</td><td>0</td><td>2'b01</td><td>2'b00</td></tr>
<tr><td>X</td><td>0</td><td>0</td><td>0</td><td>2'b00</td><td>2'b00</td></tr>
<tr><td>None of the four elements is a non-zero element</td>
<td>0</td><td>0</td><td>0</td><td>0</td><td>2'b00</td><td>2'b00</td>
</tr></tbody></table>

- **Physical location:** GM/L1 Buffer: Stores the offline-generated index matrix data.

- **Design principle:** Column reading requirement: In sparse matrix multiplication, the B matrix and the Index matrix have a one-to-one mapping in physical addresses. The LoadDataWithSparse instruction requires that the B matrix and the Index matrix satisfy the column reading requirement on the L1 Buffer. Therefore, on the GM/L1 Buffer, the Index matrix is arranged in a large-Z small-n layout stored by column (Zn).

- **Format conversion process:** Assume original B matrix size is (K, N). After 4-to-2 densification, the B matrix and the Index matrix have a size of (K/2, N). The Index matrix is divided into K1 \* N1 fractals, arranged in row-major order, with a Z-like shape. Each fractal internally has N0 \* K0 elements, arranged in column-major order, with an N-like shape, so this kind of data format is called the Zn format. Where \(N0, K0\) represents the shape of a fractal, and the fractal needs to be consistent with the B matrix fractal. The B matrix data type is int8\_t, and its small fractal shape is 16\*32. Each uint8_t data value in the Index matrix fractal is formed by concatenating 4 uint2_t elements, and its elements correspond one-to-one to the addresses of the B matrix elements. Therefore, the fractal shape is also 16\*32, but its size is 128 bytes.

    The conversion process is expressed by the following formulas:

    ```py
    (…, B, K/2, N)->pad->(…, B, K1 * K0, N1 * N0)->reshape->(…, B, K1, K0, N1, N0)->transpose->(…, B, K1, N1, N0, K0)
    ```

- **Fractal size constraint:** When stored as the **uint8** data type, the fractal size is 16x8 (N0, K0).
<!-- end id2 -->
