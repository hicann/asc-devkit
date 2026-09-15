# Auxiliary Cube Fractal Format Details<a name="ZH-CN_TOPIC_0000002538231118"></a>

<!-- npu="950" id1 -->
<!-- md-trans-meta sourceCommit=e3c8c70b3f5d27f3770063ae4b535bc75d904e3e translatedAt=2026-08-27T16:41:29.033Z -->

## MX Coefficient Cube<a name="zh-cn_topic_0000002563445163_section648585413300"></a>

For Ascend 950PR/Ascend 950DT:

Floating-point numbers are widely used in scientific computing, image processing, neural networks, and other fields. Taking AI training as an example, existing floating-point formats either have an insufficient numerical range or low precision, which affects the convergence speed and performance of models. Meeting both the numerical range and precision requirements simultaneously would lead to excessive memory usage, thereby increasing the cost of data storage and transmission. In view of this, the industry has proposed a new floating-point format, the Microscaling (MX) format. Floating-point numbers in the MX format can support AI training and inference at lower bit widths while occupying less memory. Data formats conforming to the MX standard can achieve robust AI training and inference model precision when using 8 bits or fewer.

The MX format is a block data format. Multiple data elements can form a block (or a group), and data is organized in blocks. Data in the MX format consists of three parts:

- A shared scaling factor X with a bit width of w bits;
- Private elements P<sub>i</sub> with a bit width of d bits;
- A block size k, indicating how many low-bit data elements form a block;

All k elements P<sub>i</sub> have the same bit width and data type and share a single scaling factor X. Each block containing k elements can be encoded using (w+k\*d) bits. The data type of the elements and the scaling factor can be selected independently.

The following figure shows the data structure of floating-point numbers in the MX format, where S, E, and M represent the sign, exponent, and mantissa field values of a floating-point number, respectively. The shared scaling factor X is a scaling factor applied to the entire data block, and it determines the dynamic range of all elements in the data block. By introducing a shared scaling factor, data in the MX format can flexibly represent data of different ranges while maintaining a low bit width. The block size k refers to the number of low-bit data elements that form a data block (or group). A private element P<sub>i</sub> refers to each low-bit data element in the data block. After being adjusted by the scaling factor X, these elements jointly represent a high-precision floating-point number or integer.

**Figure 1** Schematic diagram of the MX format composition<a name="zh-cn_topic_0000002563445163_fig3385175571011"></a>  
![](../../../../figures/mx_format.png "Schematic diagram of the MX format composition")

MXMmad is a cube multiplication with quantized coefficients X, meaning that both the left cube and the right cube have corresponding quantization coefficient matrices: the left quantization coefficient cube **scaleA** and the right quantization coefficient cube **scaleB**. The calculation formula is: C = \(scaleA ⊗ A\) \* \(scaleB ⊗ B\) + Bias, where "⊗" denotes broadcast multiplication. When the left/right cube is multiplied by the left/right quantization coefficient cube, every 32 elements along the K dimension share one quantization factor. As shown in the following figure, A is the left cube with shape \[M, K\]; **scaleA** is the left quantization coefficient cube with shape \[M, K/32\]; B is the right cube with shape \[K, N\]; and **scaleB** is the right quantization coefficient cube with shape \[K/32, N\].

**Figure 2** MX cube multiplication diagram<a name="zh-cn_topic_0000002563445163_fig33303533712"></a>  
![](../../../../figures/mx_cube_computation_demo.png "MX cube multiplication diagram")

- **Introduction to the MX scaleA coefficient cube fractal**<a name="section_mx_scalea_fractal_format"></a>

    - **Physical location:** Global Memory/L1 Buffer/Unified Buffer: stores the left cube quantization coefficient data generated offline/online.

    - **Design principle:** In MX cube multiplication, cube A and cube **scaleA** are mapped one-to-one in physical addresses. The **Load2DV2MX** instruction requires that the **scaleA** cube in the L1 Buffer satisfy the row-read requirement. Therefore, in the L1 Buffer, the **scaleA** cube is stored in a small-z large-Z layout (Zz) in row-major order.

    - **Format conversion process:** Assuming the original A cube has a size of (M, K), the corresponding **scaleA** cube has a size of (M, K/32). The **scaleA** cube is divided into M1 \* K1 fractals, arranged in row-major order with a zigzag shape. Each fractal contains M0 \* K0 elements, arranged in row-major order with a zigzag shape, so this data format is called the Zz format. Here, \(M0, K0\) represents the shape of a fractal, which must be consistent with the physical memory layout of the L0A_MX Buffer, and the data type must be **fp8_e8m0_t** with a shape of 16x2.

        The conversion process is expressed by the following formula:

        ```py
        (…, B, M, K/32)->pad->(…, B, M1 * M0, K1 * K0)->reshape->(…, B, M1, M0, K1, K0)->transpose->(…, B, M1, K1, M0, K0)
        ```

    - **Fractal size constraint:** The data type is fp8\_e8m0\_t, and the fractal size is 16x2 (M0, K0).

    As shown in the following figure, if the **scaleA** cube in GM is in ND layout, it can be moved to the L1 Buffer using the **ND2NZ.b16**/**DN2NZ.b16** instructions to ensure that the fractal in the L1 Buffer is in Zz layout, satisfying the requirements of the **Load2DV2MX** instruction.

    **Figure 3** Layout format of the scaleA cube at different locations<a name="zh-cn_topic_0000002563445163_fig6871154411446"></a>  
    ![](../../../../figures/mx_scaleA_format.png "Layout format of the scaleA cube at different locations")

- **Introduction to the MX scaleB Coefficient Cube Fractal**<a name="section_mx_scaleb_fractal_format"></a>

    - **Physical location:** Global Memory/L1 Buffer/Unified Buffer: Stores the offline/online generated right-cube quantization coefficient data.

    - **Design principle:** In MX cube multiplication, the B cube and the scaleB cube are mapped one-to-one in physical addresses. The **Load2DV2MX** instruction requires the scaleB cube on the L1 Buffer to satisfy column-read requirements. Therefore, on the L1 Buffer, the scaleB cube is stored in a column-major layout with a large N and small n (Nn).

    - **Format conversion process:** Assume that the original B cube has a size of (K, N). The corresponding scaleB cube then has a size of (K/32, N). The scaleB cube is divided into K1 \* N1 fractals, which are arranged in column-major order, forming an N-like shape. Each fractal contains K0 \* N0 elements, which are arranged in column-major order, forming an n-like shape. Therefore, this data format is called the Nn format. Here, (K0, N0) represents the shape of a fractal. The fractal must be consistent with the physical memory layout of the L0B_MX Buffer, and the data type must be **fp8_e8m0_t**, with a shape of 2x16.

        The conversion process is expressed by the following formula:

        ```py
        (…, B, K/32, N)->pad->(…, B, K1 * K0, N1 * N0)->reshape->(…, B, K1, K0, N1, N0)->transpose->(…, B, N1, K1, N0, K0)
        ```

    - **Fractal size constraint:** The data type is **fp8\_e8m0\_t**, and the fractal size is 2x16 (K0, N0).

    As shown in the following figure, if the scaleB cube on Global Memory (GM) is in the ND layout, it can be moved to the L1 Buffer using the **ND2NZ.b16**/**DN2NZ.b16** instructions to ensure that the fractal on the L1 Buffer is in the Nn layout, satisfying the requirements of the **Load2DV2MX** instruction.

    **Figure 4** Layout format of the scaleB cube at different positions<a name="zh-cn_topic_0000002563445163_fig599218260112"></a>  
    ![](../../../../figures/scale_b_cube_layout.png "Layout format of the scaleB cube at different positions")

For specific implementation, see [MXmmad sample](https://gitcode.com/cann/asc-devkit/tree/9.1.0/examples/01_simd_cpp_api/03_basic_api/03_cube_compute/load_data_2dmx_l12l0).
<!-- end id1 -->

<!-- npu="910b,A3" id2 -->
## 4-choose-2 Sparse Index Cube<a name="zh-cn_topic_0000002563445163_section1873692415233"></a>

For Atlas A2 training products/Atlas A2 inference products and Atlas A3 training products/Atlas A3 inference products:

- Definition: The index cube generated during the densification of the original B cube. It is assumed that every 4 elements of the original B cube contain at least 2 zero elements, and the densified B cube is a dense cube with the 2 zeros filtered out.
- Scenario: It usually resides in **Global Memory (GM)**. Before sparse cube operations, it usually needs to be generated offline and converted to the large-Z small-n format (Zn) to adapt to the **Cube** unit.
- Generation process: As shown in the following figure, for every 4 elements in the original cube B, two 2-bit indices are generated in the index cube and encoded according to the following rules. The indices must be within the range \{0, 1, 2\}.
**Figure 5** Index generation diagram<a name="zh-cn_topic_0000002563445163_fig1423214854720"></a>  
![](../../../../figures/index_generation_diagram.png "Index generation diagram")
    - The first index indicates the relative position of the first non-zero element among the first 3 elements.
    - The second index indicates the relative position of the second non-zero element among the last 3 elements.
    - One **uint8** element in the index cube is composed of four **uint2\_t** values. For example, in the preceding figure, the generated index data is 1 2 0 1, and the layout of the index data in the **Global Memory (GM)** address from high to low bits is 1 0 2 1, where 1 0 2 1 (corresponding to the first four bits 1 2 0 1 of the index cube) is one **uint8\_t**.

**Figure 6** Densified B cube diagram<a name="zh-cn_topic_0000002563445163_fig1115556103916"></a>  
![](../../../../figures/density_B_cube_demo.png "Densified B cube diagram")

For the specific densification algorithm rules, see the following table. Here, "-" indicates that the value at that position is not of concern, because it will be filtered out during the subsequent Sparse Mmad computation.

<table><thead>
<tr><th>Example</th><th>B cube element 0</th><th>B cube element 1</th><th>B cube element 2</th><th>B cube element 3</th><th>Index data 0</th><th>Index data 1</th>
</tr></thead><tbody>
<tr><td rowspan="6">Two of the four elements are non-zero elements</td>
<td>0</td><td>0</td><td>X</td><td>Y</td><td>2'b10</td><td>2'b10</td></tr>
<tr><td>0</td><td>X</td><td>0</td><td>Y</td><td>2'b01</td><td>2'b10</td></tr>
<tr><td>X</td><td>0</td><td>0</td><td>Y</td><td>2'b00</td><td>2'b10</td></tr>
<tr><td>0</td><td>X</td><td>Y</td><td>-</td><td>2'b01</td><td>2'b01</td></tr>
<tr><td>X</td><td>0</td><td>Y</td><td>-</td><td>2'b00</td><td>2'b01</td></tr>
<tr><td>X</td><td>Y</td><td>-</td><td>-</td><td>2'b00</td><td>2'b00</td></tr>
<tr><td rowspan="4">One non-zero element among the four elements.</td>
<td>0</td><td>0</td><td>0</td><td>X</td><td>2'b00</td><td>2'b10</td></tr>
<tr><td>0</td><td>0</td><td>X</td><td>0</td><td>2'b10</td><td>2'b00</td></tr>
<tr><td>0</td><td>X</td><td>0</td><td>0</td><td>2'b01</td><td>2'b00</td></tr>
<tr><td>X</td><td>0</td><td>0</td><td>0</td><td>2'b00</td><td>2'b00</td></tr>
<tr><td>None of the four elements is a non-zero element.</td>
<td>0</td><td>0</td><td>0</td><td>0</td><td>2'b00</td><td>2'b00</td>
</tr></tbody></table>

- **Physical location:** GM/L1 Buffer: Stores the index cube data generated offline.

- **Design principle:** Column read requirement: In sparse cube multiplication, the B cube and the Index cube are mapped one-to-one in physical addresses. The LoadDataWithSparse instruction requires that the B cube and the Index cube in the L1 Buffer satisfy the column read requirement. Therefore, in the GM/L1 Buffer, the Index cube is stored in a column-major large-Z small-n layout (Zn).

- **Format conversion process:** Assume that the original B cube has a size of (K, N). After 4-to-2 densification, the B cube and the Index cube have a size of (K/2, N). The Index cube is divided into K1 \* N1 fractals arranged in row-major order, forming a Z-shaped pattern. Each fractal contains N0 \* K0 elements arranged in column-major order, forming an N-shaped pattern. Therefore, this data format is called the Zn format. Here, \(N0, K0\) represents the shape of a fractal, which must be consistent with the fractal of the B cube. The B cube has a data type of int8\_t and a small fractal shape of 16\*32. In the Index cube fractal, each uint8\_t datum is composed of four uint2\_t elements concatenated together, and its elements correspond one-to-one to the addresses of the B cube elements. Therefore, the fractal shape is also 16\*32, but its size is 128 bytes.

The conversion process is expressed by the following formula:

    ```py
    (…, B, K/2, N)->pad->(…, B, K1 * K0, N1 * N0)->reshape->(…, B, K1, K0, N1, N0)->transpose->(…, B, K1, N1, N0, K0)
    ```

- **Fractal size constraint:** When stored as the uint8 data type, the fractal size is 16x8 (N0, K0).

<!-- end id2 -->
