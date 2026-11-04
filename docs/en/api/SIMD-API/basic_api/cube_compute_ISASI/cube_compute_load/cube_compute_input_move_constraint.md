# Matrix Computation Input Data Movement Constraints<a name="ZH-CN_TOPIC_0000002568950895"></a>

<!-- md-trans-meta sourceCommit=754fb95859dead826727a591e674261c6a829a08 translatedAt=2026-09-23T11:24:37.165Z pushedAt=2026-10-26T07:41:44.063Z nl=2 -->

## Alignment Constraints

For details, see [Table Data path and storage hierarchy](overall_description.md#zh-cn_topic_0000002543771563_table3523123518108).

- When the source address is located in the L1 Buffer, the address must be 32-byte aligned; when the source address is located in GM, the address must be 1-byte aligned.
- When the destination address is located in the L0A Buffer/L0B Buffer, the address must be 512-byte aligned; when the destination address is located in the L1 Buffer, the address must be 32-byte aligned.

## Bandwidth Constraints

- LoadData (2D matrix move) moves data in units of a 512-byte fractal.
- LoadData (convolution data move) moves data in units of a fractal with a size of 512 bytes.
- LoadDataWithSparse moves the dense weight matrix stored in units of 512 bytes to the L0B Buffer, and simultaneously moves the index matrix in units of 128 bytes to the built-in dedicated buffer space.

## Fractal Constraints

**DataCopy** (GM-\>L1 Buffer), when not enabling the on-the-fly ND-to-NZ conversion, does not change the inter-fractal and intra-fractal layout, such as Nz2Nz; when enabling the ND-to-NZ conversion, it always changes the fractal layout, such as ND2Nz.

**LoadData** (2D matrix move), when transpose is not enabled, can only change the inter-fractal layout, such as Nz2Zz; when transpose is enabled, it can change both the intra-fractal layout and the inter-fractal layout, such as Nz2Zn.

**LoadDataWithTranspose** always changes the intra-fractal layout and can change the inter-fractal layout as needed, such as Nz2Zn.

In the B32 scenario, **LoadDataWithTranspose** merges two consecutive 16\*8 fractals of the source operand into one 16\*16 square matrix, and then performs a transpose on that square matrix. Therefore, the two consecutive fractals must be merged into a square matrix, and the matrix on the L1 Buffer must satisfy the Nz layout (if and only if Row==16) or the Zn layout (if and only if Col==16).

**LoadData** (convolution data move) is mainly used to perform the image-to-column operation on the feature map in the NC1HWC0 format and move the expanded two-dimensional matrix to the corresponding memory location. Therefore, for the source operand of **LoadData** (convolution data move), the data must be laid out in the NC1HWC0 format, and the destination operand data must be laid out according to the layout mode supported by the destination address L0A Buffer/L0B Buffer.

<!-- npu="910b,A3" id1 -->
**LoadDataWithSparse** is mainly used to move the dense weight matrix stored in units of 512 bytes to the L0B Buffer, and simultaneously move the index matrix in units of 128 bytes to the built-in dedicated buffer space (for subsequent reading by the **MmadWithSparse** API). Therefore, both the source operand dense weight matrix and the index matrix need to be laid out in the Zn format, where the dense weight matrix layout is 32*16*8 bits = 512 bytes and the index matrix layout is 32*16*2 bits = 128 bytes.
<!-- end id1 -->

During matrix computation, the commonly supported fractals are summarized in the following table:

<!-- npu="910b,A3" id2 -->
### For Atlas A2 training products/Atlas A2 inference products and Atlas A3 training products/Atlas A3 inference products

The common fractal support status is as follows, for developers' reference:

- **DataCopy**
    - GM->L1 Buffer
        - ND->Nz:

            ![](../../../../figures/datacopy_gm2l1_nd2nz.png)
        - Nz->Nz:

            ![](../../../../figures/datacopy_gm2l1_nz2nz.png)
        - Zn->Zn:

            ![](../../../../figures/datacopy_gm2l1_zn2zn.png)
- **LoadData (2D matrix move)**
    - GM->L1 Buffer (does not support transpose)
        - Nz->Nz:

            ![](../../../../figures/load2d_gm2l1_nz2nz.png)
        - Zn->Zn:

            ![](../../../../figures/load2d_gm2l1_zn2zn.png)
    - GM->L0A Buffer (does not support transpose) L1 Buffer->L0A Buffer (does not enable transpose) (Nz->Zz)

        ![](../../../../figures/load2d_gm2l0a_nz2zz.png)
    - GM->L0B Buffer (does not support transpose) L1 Buffer->L0B Buffer (does not enable transpose) (Zn->Zn)

        ![](../../../../figures/load2d_gm2l0b_zn2zn.png)
    - L1 Buffer->L0A Buffer (enable transpose, only supports b16 data type) (Nz->Zz)

        ![](../../../../figures/load2d_l12l0a_nz2zz.png)
    - L1 Buffer->L0B Buffer (enable transpose, only supports b16 data type) (Zn->Zn)

        ![](../../../../figures/load2d_l12l0b_zn2zn.png)
- **LoadDataWithTranspose**
    - L1 Buffer->L0A Buffer (b8) (Nz->Zz)

        ![](../../../../figures/loaddatawithtranspose_l12l0a_b8_nz2zz.png)
    - L1 Buffer->L0B Buffer (b4/b8) (Zn->Zn)

        ![](../../../../figures/loaddatawithtranspose_l12l0b_b4b8_zn2zn.png)
    - L1 Buffer->L0A Buffer (b16) (Nz->Zz)

        ![](../../../../figures/loaddatawithtranspose_l12l0a_b16_nz2zz.png)
    - L1 Buffer->L0B Buffer (b16) (Zn->Zn)

        ![](../../../../figures/loaddatawithtranspose_l12l0b_b16_zn2zn.png)
    - L1 Buffer->L0A Buffer (b32) (Nz (<span>if and only if Row==16</span>)->Zz)

        ![](../../../../figures/loaddatawithtranspose_l12l0a_b32_nz2zz.png)
    - L1 Buffer->L0B Buffer (b32) (Zn (<span>if and only if Col==16</span>)->Zn)

        ![](../../../../figures/loaddatawithtranspose_l12l0b_b32_zn2zn.png)
- **LoadData (convolution data move)**
    - L1 Buffer->L0A Buffer (NC1HWC0->Zz)

        ![](../../../../figures/load3d_l12l0a_nc1hwc02zz.png)
    - L1 Buffer->L0B Buffer (NC1HWC0->Zn)

        ![](../../../../figures/load3d_l12l0b_nc1hwc02zn.png)
- **LoadDataWithSparse**
    - L1 Buffer->L0B Buffer (Zn->Zn)

        ![](../../../../figures/loaddatawithsparse_l12l0b_zn2zn.png)

For the support status of some uncommon fractal conversions, developers can refer to the following table:

**Table 1** Extended fractal conversion support status

<a name="zh-cn_topic_0000002513935478_table471251721314"></a>

| Move Instruction | Data Path | Fractal Support Status |
| --- | --- | --- |
| LoadData (2D matrix move) | GM->L1 Buffer (does not support transpose) | Nz->Zz, Zn->Nn, Zz->Zz, Zz->Nz, Nn->Nn, Nn->Zn |
| LoadData (2D matrix move) | GM->L0A Buffer (does not support transpose),<br>L1 Buffer->L0A Buffer (does not enable transpose) | Nz->Nz, Zn->Nn, Zn->Zn, Zz->Zz, Zz->Nz, Nn->Nn, Nn->Zn |
| LoadData (2D matrix move) | GM->L0B Buffer (does not support transpose),<br>L1 Buffer->L0B Buffer (does not enable transpose) | Nz->Zz, Nz->Nz, Zn->Nn, Zz->Zz, Zz->Nz, Nn->Nn, Nn->Zn |
| LoadData (2D matrix move) | L1 Buffer->L0A Buffer (enable transpose, only supports b16 data type) | Nz->Nz, Zn->Nn, Zn->Zn, Zz->Zz, Zz->Nz, Nn->Nn, Nn->Zn |
| LoadData (2D matrix move) | L1 Buffer->L0B Buffer (enable transpose, only supports b16 data type) | Nz->Zz, Nz->Nz, Zn->Nn, Zz->Zz, Zz->Nz, Nn->Nn, Nn->Zn |
| LoadDataWithTranspose | L1 Buffer->L0A Buffer (b8),<br>L1 Buffer->L0A Buffer (b16) | Nz->Nz, Zn->Nn, Zn->Zn, Zz->Zz, Zz->Nz, Nn->Nn, Nn->Zn |
| LoadDataWithTranspose | L1 Buffer->L0B Buffer (b4/b8),<br>L1 Buffer->L0B Buffer (b16) | Nz->Zz, Nz->Nz, Zn->Nn, Zz->Zz, Zz->Nz, Nn->Nn, Nn->Zn |
| LoadDataWithTranspose | L1 Buffer->L0A Buffer (b32) | Zz->Nz, Zz->Zz |
| LoadDataWithTranspose | L1 Buffer->L0B Buffer (b32) | Nn->Zn, Nn->Nn |

<!-- end id2 -->

<!-- npu="950" id3 -->
### For Ascend 950PR&950DT products

DataCopy (GM-\>L1 Buffer) supports enabling on-the-fly DN-to-NZ conversion, which always changes the fractal layout.

LoadData (2D matrix move V2) does not change the inter-fractal layout or the intra-fractal layout when transpose is not enabled, such as Nz2Nz; when transpose is enabled, it changes both the intra-fractal layout and the inter-fractal layout, such as Nz2Zn.

LoadData (MX matrix move), when moving the left and right matrices, does not change the inter-fractal layout or the intra-fractal layout when not enabling transpose, such as Nz2Nz; when enabling transpose, it changes both the intra-fractal layout and the inter-fractal layout, such as Nz2Zn. When moving the quantization coefficient matrix, it does not change the inter-fractal or intra-fractal layout.

The common fractal support status is as follows, for developers' reference:

- **DataCopy**
    - GM->L1 Buffer
        - ND->Nz:

            ![](../../../../figures/datacopy_gm2l1_nd2nz.png)
        - DN->Nz:

            ![](../../../../figures/datacopy_gm2l1_dn2nz.png)
        - Nz->Nz:

            ![](../../../../figures/datacopy_gm2l1_nz2nz.png)
        - Zn->Zn:

            ![](../../../../figures/datacopy_gm2l1_zn2zn.png)
- **LoadData (2D matrix move)**
    - GM->L1 Buffer (does not support transpose)
        - Nz->Nz:

            ![](../../../../figures/load2d_gm2l1_nz2nz.png)
        - Zn->Zn:

            ![](../../../../figures/load2d_gm2l1_zn2zn.png)
    - L1 Buffer->L0A Buffer (transpose not enabled) (Nz->Nz)

        ![](../../../../figures/load2d_l12l0a_nz2nz.png)
    - L1 Buffer->L0B Buffer (transpose not enabled) (Zn->Zn)

        ![](../../../../figures/load2d_l12l0b_zn2zn.png)
    - L1 Buffer->L0A Buffer (transpose enabled, only supports b16 data type) (Zn->Nz)

        ![](../../../../figures/load2d_l12l0a_zn2nz.png)
    - L1 Buffer->L0B Buffer (transpose enabled, only supports b16 data type) (Zn->Zn)

        ![](../../../../figures/load2d_l12l0b_zn2zn.png)
- **LoadData (2D matrix move V2)**
    - L1 Buffer->L0A Buffer (Nz->Nz)

        ![](../../../../figures/load2dv2_l12l0a_nz2nz.png)
    - L1 Buffer->L0A Buffer (enable transpose) (Zn->Nz)

        ![](../../../../figures/load2dv2_l12l0a_zn2nz.png)
    - L1 Buffer->L0B Buffer (Zn->Zn)

        ![](../../../../figures/load2dv2_l12l0b_zn2zn.png)
    - L1 Buffer->L0B Buffer (enable transpose) (Nz->Zn)

        ![](../../../../figures/load2dv2_l12l0b_nz2zn.png)
- **LoadData (MX matrix move)**
    - L1 Buffer->L0A Buffer (Nz->Nz)

        ![](../../../../figures/load2dv2mx_l12l0a_nz2nz.png)
    - L1 Buffer->L0A Buffer_MX

        ![](../../../../figures/load2dv2mx_l12l0amx.png)
    - L1 Buffer->L0B Buffer (Zn->Zn)

        ![](../../../../figures/load2dv2mx_l12l0b_zn2zn.png)
    - L1 Buffer->L0B Buffer_MX

        ![](../../../../figures/load2dv2mx_l12l0bmx.png)
- **LoadDataWithTranspose**
    - L1 Buffer->L0B Buffer (b4/b8) (Zn->Zn)

        ![](../../../../figures/loaddatawithtranspose_l12l0b_b4b8_zn2zn.png)
    - L1 Buffer->L0B Buffer (b16) (Zn->Zn)

        ![](../../../../figures/loaddatawithtranspose_l12l0b_b16_zn2zn.png)
    - L1 Buffer->L0B Buffer (b32) (Zn (<span>if and only if Col==16</span>)->Zn)

        ![](../../../../figures/loaddatawithtranspose_l12l0b_b32_zn2zn.png)
- **LoadData (convolution data move)**
    - L1 Buffer->L0A Buffer (NC1HWC0->Nz)

        ![](../../../../figures/load3d_l12l0a_nc1hwc02nz.png)
    - L1 Buffer->L0B Buffer (NC1HWC0->Zn)

        ![](../../../../figures/load3d_l12l0b_nc1hwc02zn.png)

For the support status of some uncommon fractal conversions, developers can refer to the following table:

**Table 2** Extended fractal conversion support status

<a name="zh-cn_topic_0000002513935478_table150925964717"></a>

| Move Instruction | Data Path | Fractal Support Status |
| --- | --- | --- |
| LoadData (2D matrix move) | GM->L1 Buffer (transpose not supported) | Nz->Zz, Zn->Nn, Zz->Zz, Zz->Nz, Nn->Nn, Nn->Zn |
| LoadData (2D matrix move) | L1 Buffer->L0A Buffer (transpose not enabled) | Nz->Nz, Zn->Nn, Zn->Zn, Zz->Zz, Zz->Nz, Nn->Nn, Nn->Zn |
| LoadData (2D matrix move) | L1 Buffer->L0B Buffer (transpose not enabled) | Nz->Zz, Nz->Nz, Zn->Nn, Zz->Zz, Zz->Nz, Nn->Nn, Nn->Zn |
| LoadData (2D matrix move) | L1 Buffer->L0A Buffer (transpose enabled, only supports b16 data type) | Nz->Nz, Zn->Nn, Zn->Zn, Zz->Zz, Zz->Nz, Nn->Nn, Nn->Zn |
| LoadData (2D matrix move) | L1 Buffer->L0B Buffer (transpose enabled, only supports b16 data type) | Nz->Zz, Nz->Nz, Zn->Nn, Zz->Zz, Zz->Nz, Nn->Nn, Nn->Zn |
| LoadDataWithTranspose | L1 Buffer->L0B Buffer (b4/b8),<br>L1 Buffer->L0B Buffer (b16) | Nz->Zz, Nz->Nz, Zn->Nn, Zz->Zz, Zz->Nz, Nn->Nn, Nn->Zn |
| LoadDataWithTranspose | L1 Buffer->L0B Buffer (b32) | Nn->Zn, Nn->Nn |

<!-- end id3 -->

## Data Type Constraints

<!-- npu="910b,A3" id4 -->
For Atlas A2 training products/Atlas A2 inference products and Atlas A3 training products/Atlas A3 inference products, the data type constraints are shown in Table 3:

**Table 3** Data type constraints

<a name="zh-cn_topic_0000002513935478_table7327153310263"></a>

| Move Instruction | Data Path | Supported Data Types |
| --- | --- | --- |
| LoadData (2D matrix move) | GM->L1 Buffer, GM->L0A Buffer, GM->L0B Buffer,<br>L1 Buffer->L0A Buffer, L1 Buffer->L0B Buffer | <br>b4 (int4b_t);<br>b8 (int8_t, uint8_t);<br>b16 (int16_t, uint16_t, half, bfloat16_t);<br>b32 (int32_t, uint32_t, float). |
| LoadDataWithTranspose | L1 Buffer->L0A Buffer | b8 (int8_t, uint8_t);<br>b16 (half, bfloat16_t);<br>b32 (int32_t, uint32_t, float). |
| LoadDataWithTranspose | L1 Buffer->L0B Buffer | b4 (int4b_t);<br>b8 (int8_t, uint8_t);<br>b16 (half, bfloat16_t);<br>b32 (int32_t, uint32_t, float). |
| LoadDataWithSparse | L1 Buffer->L0B Buffer | b8(int8_t) |
| LoadData (convolution data move) | L1 Buffer->L0A Buffer | b4 (int4b_t);<br>b8 (int8_t, uint8_t);<br>b16 (half, bfloat16_t);<br>b32 (int32_t, uint32_t, float). |
| LoadData (convolution data move) | L1 Buffer->L0B Buffer | b16 (half, bfloat16_t);<br>b32 (int32_t, uint32_t, float). |

> [!NOTE]
>
> - LoadData (2D matrix move): Only the L1 Buffer->L0A Buffer/L0B Buffer path can enable transpose. When transpose is enabled, the source and destination operands only support b16-width data types.
> - LoadData (2D matrix move): b4 (int4b_t) only supports the L1 Buffer->L0A Buffer and L1 Buffer->L0B Buffer paths.
> - LoadDataWithTranspose: It only supports the int4b\_t data type when the destination operand address is located in L0B Buffer.
<!-- end id4 -->

<!-- npu="950" id5 -->
For Ascend 950PR&950DT products, the data type constraints are shown in Table 4:

**Table 4** Data type constraints

<a name="zh-cn_topic_0000002513935478_table259001716534"></a>

| Move Instruction | Data Path | Supported Data Types |
| --- | --- | --- |
| LoadData (2D matrix move) | GM->L1 Buffer, L1 Buffer->L0A Buffer,<br>L1 Buffer->L0B Buffer | b8 (int8_t, uint8_t);<br>b16 (int16_t, uint16_t, half, bfloat16_t);<br>b32 (int32_t, uint32_t, float). |
| LoadData (2D matrix move V2) | GM->L1 Buffer, L1 Buffer->L0A Buffer,<br>L1 Buffer->L0B Buffer | int8_t, uint8_t, fp4x2_e2m1_t, fp4x2_e1m2_t,<br>hifloat8_t, fp8_e5m2_t, fp8_e4m3fn_t, half,<br>bfloat16_t, int32_t, uint32_t, float. |
| LoadDataWithTranspose | L1 Buffer->L0B Buffer | b8 (int8_t, uint8_t);<br>b16 (half, bfloat16_t);<br>b32 (int32_t, uint32_t, float). |
| LoadData (convolution data move) | L1 Buffer->L0A Buffer | b8 (int8_t, uint8_t, hifloat8_t, fp8_e5m2_t, fp8_e4m3fn_t);<br>b16 (half, bfloat16_t);<br>b32 (int32_t, uint32_t, float). |
| LoadData (convolution data move) | L1 Buffer->L0B Buffer | b8 (int8_t, uint8_t, hifloat8_t, fp8_e5m2_t, fp8_e4m3fn_t);<br>b16 (half, bfloat16_t);<br>b32 (int32_t, uint32_t, float). |

<!-- end id5 -->

<!-- npu="950" id6 -->
- For Ascend 950PR&950DT products, **LoadData** (2D matrix move V2) performs moves in units of 512-byte fractals. **LoadData** (MX matrix move) includes two types of matrix data moves: the left and right matrices are moved in units of 512-byte fractals, and the left and right quantization coefficient matrices are moved in units of 32-byte fractals.
<!-- end id6 -->

## Move Instruction Summary

<!-- npu="910b,A3" id7 -->
Based on the above constraints such as data types and fractals, we have summarized the usage scenarios of the data input API, which are provided as follows for developers' reference.

For Atlas A2 training products/Atlas A2 inference products and Atlas A3 training products/Atlas A3 inference products, **on the L1 Buffer->L0A Buffer/L0B Buffer path, the commonly used move instructions include LoadData (2D matrix move), LoadDataWithTranspose, and LoadData (convolution data move). The instructions that can be called are summarized in Table 5 as follows:**

**Table 5** Summary of instructions called on the L1 Buffer->L0A Buffer/L0B Buffer path

<a name="zh-cn_topic_0000002513935478_table0327183312611"></a>

| Transpose/Data Type | b4 (int4b_t) | b8 | b16 | b32 |
| --- | --- | --- | --- | --- |
| A without transpose (L1 Buffer->L0A Buffer does not need to enable transpose) | LoadData (convolution data move) (recommended) | LoadData (2D matrix move), LoadData (convolution data move) (recommended) | LoadData (2D matrix move), LoadData (convolution data move) (recommended) | LoadData (2D matrix move), LoadData (convolution data move) (recommended) |
| A with transpose (L1 Buffer->L0A Buffer needs to enable transpose) | Not supported | LoadDataWithTranspose | LoadData (2D matrix move), LoadDataWithTranspose, LoadData (convolution data move) (recommended) | LoadDataWithTranspose (data on L1 Buffer is in Nz layout (if and only if Row==16)), LoadData (convolution data move) (recommended) |
| B without transpose (L1 Buffer->L0B Buffer needs to enable transpose) | LoadDataWithTranspose | LoadDataWithTranspose | LoadData (2D matrix move), LoadDataWithTranspose, LoadData (convolution data move) (recommended) | LoadDataWithTranspose (data on L1 Buffer is in Zn layout (if and only if Col==16)), LoadData (convolution data move) (recommended) |
| B with transpose (L1 Buffer->L0B Buffer does not need to enable transpose) | - | LoadData (2D matrix move) | LoadData (2D matrix move) | LoadData (2D matrix move) |

> [!NOTE]
>
> - Because LoadData (2D matrix move) and LoadDataWithTranspose can only perform repeat moves in one direction during the move process, moving data blocks in multiple directions requires the use of a for loop. In contrast, LoadData (convolution data move) can complete data block moves in multiple directions by configuring mExtension and kExtension, without the need for a for loop. Therefore, the overall scalar overhead is lower than that of LoadData (2D matrix move) and LoadDataWithTranspose. Consequently, in scenarios that meet the usage constraints of LoadData (convolution data move), it is recommended to use the LoadData (convolution data move) instruction to implement data moves on the L1 Buffer->L0A Buffer/L0B Buffer path.
<!-- end id7 -->

<!-- npu="950" id8 -->
For Ascend 950PR&950DT products, LoadData (2D matrix move V2) supports b4 (fp4)/b8/b16/b32 data types as well as transpose-scenario moves. By configuring mStep and kStep, it can complete data block moves in multiple directions. Compared with LoadData (convolution data move), the instruction has lower bandwidth latency. Therefore, in matrix multiplication without quantization coefficients, it is recommended to use the LoadData (2D matrix move V2) instruction to implement data moves on the L1 Buffer->L0A Buffer/L0B Buffer path. In matrix multiplication with quantization coefficients, the LoadData (MX matrix move) instruction is required to implement data moves, which can complete not only the L1 Buffer->L0A Buffer/L0B Buffer path move but also the L1 Buffer->L0A\_MX/L0B\_MX Buffer path data move.
<!-- end id8 -->
