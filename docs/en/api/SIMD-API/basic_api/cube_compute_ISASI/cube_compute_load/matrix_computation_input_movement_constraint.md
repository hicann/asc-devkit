# Cube Computation Input Move Constraints<a name="ZH-CN_TOPIC_0000002568950895"></a>

<!-- md-trans-meta sourceCommit=e3c8c70b3f5d27f3770063ae4b535bc75d904e3e translatedAt=2026-08-27T15:56:53.834Z -->

## Alignment Constraints

For details, see [Table Data Path and Storage Hierarchy](general_description.md#zh-cn_topic_0000002543771563_table3523123518108).

- When the source address is located at **L1 Buffer**, the address must be 32-byte aligned; when the source address is located at **GM**, the address must be 1-byte aligned.
- When the destination address is located at **L0A Buffer**/**L0B Buffer**, the address must be 512-byte aligned; when the destination address is located at **L1 Buffer**, the address must be 32-byte aligned.

## Bandwidth Constraints

- **Load2D** moves data in units of 512-byte fractals.
- **Load3D** moves data in units of 512-byte fractals.
- **LoadDataWithSparse** moves the dense weight cube stored in 512-byte units into the L0B Buffer, and simultaneously moves the index cube in 128-byte units into the built-in dedicated buffer space.

## Fractal Constraints

**DataCopy** (GM->L1 Buffer), when the in-line ND-to-NZ conversion is disabled, leaves both the inter-fractal and intra-fractal layouts unchanged, such as Nz2Nz; when the ND-to-NZ conversion is enabled, it always changes the fractal layout, such as ND2Nz.

**Load2D**, when transpose is disabled, can only change the inter-fractal layout, such as Nz2Zz; when transpose is enabled, it can change both the intra-fractal and inter-fractal layouts, such as Nz2Zn.

**LoadDataWithTranspose** always changes the intra-fractal layout and can change the inter-fractal layout as needed, such as Nz2Zn.

For **LoadDataWithTranspose** in the B32 scenario, two consecutive 16*8 fractals of the source operand are merged into one 16*16 square cube, which is then transposed. Therefore, the two consecutive fractals must be merged into a square cube, which requires the cube in the L1 Buffer to satisfy the Nz layout (if and only if Row==16) or the Zn layout (if and only if Col==16).

**Load3D** is mainly used to perform the image-to-column operation on the feature map in the NC1HWC0 format and move the expanded two-dimensional cube to the corresponding memory location. Therefore, for **Load3D**, the data in the source operand must be laid out in the NC1HWC0 format, and the data in the destination operand must be laid out in a layout mode supported by the destination L0A Buffer/L0B Buffer.

<!-- npu="910b,A3" id1 -->
**LoadDataWithSparse** is mainly used to move the dense weight cube stored in 512-byte units to the L0B Buffer, and simultaneously move the index cube in 128-byte units to the built-in dedicated buffer space (for subsequent reading by the **MmadWithSparse** API). Therefore, both the dense weight cube and the index cube of the source operand must be laid out in the Zn format, where the dense weight cube uses a layout mode of 32*16*8bit=512Byte and the index cube uses a layout mode of 32*16*2bit=128Byte.
<!-- end id1 -->

During cube computation, the common fractals supported are summarized in the following table:

<!-- npu="910b,A3" id2 -->
### For Atlas A2 training products/Atlas A2 inference products and Atlas A3 training products/Atlas A3 inference products

The common fractal support status is as follows, for developer reference:

- **DataCopy**
    - GM->L1 Buffer
        - ND->Nz:

            ![](../../../../figures/datacopy_gm2l1_nd2nz.png)
        - Nz->Nz:

            ![](../../../../figures/datacopy_gm2l1_nz2nz.png)
        - Zn->Zn:

            ![](../../../../figures/datacopy_gm2l1_zn2zn.png)
- **Load2D**
    - GM->L1 Buffer (transpose not supported)
        - Nz->Nz:

            ![](../../../../figures/load2d_gm2l1_nz2nz.png)
        - Zn->Zn:

            ![](../../../../figures/load2d_gm2l1_zn2zn.png)
    - GM->L0A Buffer (transpose not supported) L1 Buffer->L0A Buffer (transpose disabled) (Nz->Zz)

        ![](../../../../figures/load2d_gm2l0a_nz2zz.png)
    - GM->L0B Buffer (transpose not supported) L1 Buffer->L0B Buffer (transpose disabled) (Zn->Zn)

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
- **Load3D**
    - L1 Buffer->L0A Buffer (NC1HWC0->Zz)

        ![](../../../../figures/load3d_l12l0a_nc1hwc02zz.png)
    - **L1 Buffer**->**L0B Buffer** (NC1HWC0->Zn)

        ![](../../../../figures/load3d_l12l0b_nc1hwc02zn.png)
- **LoadDataWithSparse**
    - **L1 Buffer**->**L0B Buffer** (Zn->Zn)

        ![](../../../../figures/loaddatawithsparse_l12l0b_zn2zn.png)

For the support status of some uncommon fractal conversions, developers can refer to the following table:

**Table 1** Extended fractal conversion support status

<a name="zh-cn_topic_0000002513935478_table471251721314"></a>

| Move Instruction | Data Path | Fractal Support Status |
| --- | --- | --- |
| **Load2D** | **GM**->**L1 Buffer** (transpose not supported) | Nz->Zz, Zn->Nn, Zz->Zz, Zz->Nz, Nn->Nn, Nn->Zn |
| **Load2D** | **GM**->**L0A Buffer** (transpose not supported),<br>**L1 Buffer**->**L0A Buffer** (transpose disabled) | Nz->Nz, Zn->Nn, Zn->Zn, Zz->Zz, Zz->Nz, Nn->Nn, Nn->Zn |
| **Load2D** | **GM**->**L0B Buffer** (transpose not supported),<br>**L1 Buffer**->**L0B Buffer** (transpose disabled) | Nz->Zz, Nz->Nz, Zn->Nn, Zz->Zz, Zz->Nz, Nn->Nn, Nn->Zn |
| **Load2D** | **L1 Buffer**->**L0A Buffer** (enable transpose, only supports the b16 data type) | Nz->Nz, Zn->Nn, Zn->Zn, Zz->Zz, Zz->Nz, Nn->Nn, Nn->Zn |
| **Load2D** | **L1 Buffer**->**L0B Buffer** (enable transpose, only supports the b16 data type) | Nz->Zz, Nz->Nz, Zn->Nn, Zz->Zz, Zz->Nz, Nn->Nn, Nn->Zn |
| **LoadDataWithTranspose** | **L1 Buffer**->**L0A Buffer** (b8),<br>**L1 Buffer**->**L0A Buffer** (b16) | Nz->Nz, Zn->Nn, Zn->Zn, Zz->Zz, Zz->Nz, Nn->Nn, Nn->Zn |
| **LoadDataWithTranspose** | **L1 Buffer**->**L0B Buffer** (b4/b8),<br>**L1 Buffer**->**L0B Buffer** (b16) | Nz->Zz, Nz->Nz, Zn->Nn, Zz->Zz, Zz->Nz, Nn->Nn, Nn->Zn |
| **LoadDataWithTranspose** | **L1 Buffer**->**L0A Buffer** (b32) | Zz->Nz, Zz->Zz |
| **LoadDataWithTranspose** | **L1 Buffer**->**L0B Buffer** (b32) | Nn->Zn, Nn->Nn |

<!-- end id2 -->

<!-- npu="950" id3 -->
### For Ascend 950PR/Ascend 950DT

**DataCopy** (GM->L1 Buffer) supports enabling in-line DN-to-NZ conversion, which always changes the fractal layout.

When transpose is disabled, **Load2DV2** keeps both the inter-fractal layout and the intra-fractal layout unchanged, such as Nz2Nz. When transpose is enabled, it changes both the intra-fractal layout and the inter-fractal layout, such as Nz2Zn.

When moving the left and right matrices, **Load2DMX** keeps both the inter-fractal layout and the intra-fractal layout unchanged when transpose is disabled, such as Nz2Nz, and changes both the intra-fractal layout and the inter-fractal layout when transpose is enabled, such as Nz2Zn. When moving the quantization coefficient cube, it keeps both the inter-fractal layout and the intra-fractal layout unchanged.

The common fractal support status is as follows, for developer reference:

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
- **Load2D**
    - GM->L1 Buffer (transpose not supported)
        - Nz->Nz:

            ![](../../../../figures/load2d_gm2l1_nz2nz.png)
        - Zn->Zn:

            ![](../../../../figures/load2d_gm2l1_zn2zn.png)
    - L1 Buffer->L0A Buffer (transpose disabled) (Nz->Nz)

        ![](../../../../figures/load2d_l12l0a_nz2nz.png)
    - L1 Buffer->L0B Buffer (transpose disabled) (Zn->Zn)

        ![](../../../../figures/load2d_l12l0b_zn2zn.png)
    - L1 Buffer->L0A Buffer (enable transpose, only supports b16 data type) (Zn->Nz)

        ![](../../../../figures/load2d_l12l0a_zn2nz.png)
    - L1 Buffer->L0B Buffer (enable transpose, only supports b16 data type) (Zn->Zn)

        ![](../../../../figures/load2d_l12l0b_zn2zn.png)
- **Load2DV2**
    - L1 Buffer->L0A Buffer (Nz->Nz)

        ![](../../../../figures/load2dv2_l12l0a_nz2nz.png)
    - L1 Buffer->L0A Buffer (enable transpose) (Zn->Nz)

        ![](../../../../figures/load2dv2_l12l0a_zn2nz.png)
    - L1 Buffer->L0B Buffer (Zn->Zn)

        ![](../../../../figures/load2dv2_l12l0b_zn2zn.png)
    - L1 Buffer->L0B Buffer (enable transpose) (Nz->Zn)

        ![](../../../../figures/load2dv2_l12l0b_nz2zn.png)
- **Load2DV2MX**
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
- **Load3D**
    - L1 Buffer->L0A Buffer (NC1HWC0->Nz)

        ![](../../../../figures/load3d_l12l0a_nc1hwc02nz.png)
    - L1 Buffer->L0B Buffer (NC1HWC0->Zn)

        ![](../../../../figures/load3d_l12l0b_nc1hwc02zn.png)

For the support status of some uncommon fractal conversions, developers can refer to the following table:

**Table 2** Extended fractal conversion support status

<a name="zh-cn_topic_0000002513935478_table150925964717"></a>

| Move Instruction | Data Path | Fractal Support Status |
| --- | --- | --- |
| Load2D | GM->L1 Buffer (transpose not supported) | Nz->Zz, Zn->Nn, Zz->Zz, Zz->Nz, Nn->Nn, Nn->Zn |
| Load2D | L1 Buffer->L0A Buffer (transpose disabled) | Nz->Nz, Zn->Nn, Zn->Zn, Zz->Zz, Zz->Nz, Nn->Nn, Nn->Zn |
| Load2D | L1 Buffer->L0B Buffer (transpose disabled) | Nz->Zz, Nz->Nz, Zn->Nn, Zz->Zz, Zz->Nz, Nn->Nn, Nn->Zn |
| Load2D | L1 Buffer->L0A Buffer (enable transpose, only supports b16 data type) | Nz->Nz, Zn->Nn, Zn->Zn, Zz->Zz, Zz->Nz, Nn->Nn, Nn->Zn |
| Load2D | L1 Buffer->L0B Buffer (enable transpose, only supports b16 data type) | Nz->Zz, Nz->Nz, Zn->Nn, Zz->Zz, Zz->Nz, Nn->Nn, Nn->Zn |
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
| LoadData (Load2D) | GM->L1 Buffer, GM->L0A Buffer, GM->L0B Buffer,<br>L1 Buffer->L0A Buffer, L1 Buffer->L0B Buffer | <br>b4 (**int4b_t**);<br>b8 (**int8_t**, **uint8_t**);<br>b16 (**int16_t**, **uint16_t**, **half**, **bfloat16_t**);<br>b32 (**int32_t**, **uint32_t**, **float**). |
| LoadDataWithTranspose | L1 Buffer->L0A Buffer | b8 (**int8_t**, **uint8_t**);<br>b16 (**half**, **bfloat16_t**);<br>b32 (**int32_t**, **uint32_t**, **float**). |
| LoadDataWithTranspose | L1 Buffer->L0B Buffer | b4 (**int4b_t**);<br>b8 (**int8_t**, **uint8_t**);<br>b16 (**half**, **bfloat16_t**);<br>b32 (**int32_t**, **uint32_t**, **float**). |
| LoadDataWithSparse | L1 Buffer->L0B Buffer | b8 (**int8_t**) |
| LoadData (Load3D) | L1 Buffer->L0A Buffer | b4 (**int4b_t**);<br>b8 (**int8_t**, **uint8_t**);<br>b16 (**half**, **bfloat16_t**);<br>b32 (**int32_t**, **uint32_t**, **float**). |
| LoadData (Load3D) | L1 Buffer->L0B Buffer | b16 (**half**, **bfloat16_t**);<br>b32 (**int32_t**, **uint32_t**, **float**). |

> [!NOTE]
>
> - **LoadData (Load2D)**: Only the L1 Buffer->L0A Buffer/L0B Buffer path can enable transpose. When transpose is enabled, the source operand and destination operand only support the b16 bit-width data type.
> - **LoadData (Load2D)**: b4 (**int4b_t**) only supports the L1 Buffer->L0A Buffer and L1 Buffer->L0B Buffer paths.
> - **LoadDataWithTranspose**: The **int4b\_t** data type is supported only when the destination operand address is located at L0B Buffer.
<!-- end id4 -->

<!-- npu="950" id5 -->
For Ascend 950PR/Ascend 950DT, the data type constraints are shown in Table 4:

**Table 4** Data type constraints

<a name="zh-cn_topic_0000002513935478_table259001716534"></a>

| Move Instruction | Data Path | Supported Data Types |
| --- | --- | --- |
| LoadData (Load2D) | GM->L1 Buffer, L1 Buffer->L0A Buffer,<br>L1 Buffer->L0B Buffer | b8 (**int8_t**, **uint8_t**);<br>b16 (**int16_t**, **uint16_t**, **half**, **bfloat16_t**);<br>b32 (**int32_t**, **uint32_t**, **float**). |
| LoadData (Load2DV2) | GM->L1 Buffer, L1 Buffer->L0A Buffer,<br>L1 Buffer->L0B Buffer | **int8_t**, **uint8_t**, **fp4x2_e2m1_t**, **fp4x2_e1m2_t**,<br>**hifloat8_t**, **fp8_e5m2_t**, **fp8_e4m3fn_t**, **half**,<br>**bfloat16_t**, **int32_t**, **uint32_t**, **float**. |
| LoadDataWithTranspose | L1 Buffer->L0B Buffer | b8 (**int8_t**, **uint8_t**);<br>b16 (**half**, **bfloat16_t**);<br>b32 (**int32_t**, **uint32_t**, **float**). |
| LoadData (Load3D) | L1 Buffer->L0A Buffer | b8 (**int8_t**, **uint8_t**, **hifloat8_t**, **fp8_e5m2_t**, **fp8_e4m3fn_t**);<br>b16 (**half**, **bfloat16_t**);<br>b32 (**int32_t**, **uint32_t**, **float**). |
| LoadData (Load3D) | L1 Buffer->L0B Buffer | b8 (**int8_t**, **uint8_t**, **hifloat8_t**, **fp8_e5m2_t**, **fp8_e4m3fn_t**);<br>b16 (**half**, **bfloat16_t**);<br>b32 (**int32_t**, **uint32_t**, **float**). |
<!-- end id5 -->

<!-- npu="950" id6 -->
- For Ascend 950PR/Ascend 950DT, **Load2DV2** moves data in units of 512-byte fractals. **Load2DMX** includes two types of cube data movement: the left and right matrices are moved in units of 512-byte fractals, and the left and right quantization coefficient matrices are moved in units of 32-byte fractals.
<!-- end id6 -->

## Move Instruction Summary

<!-- npu="910b,A3" id7 -->
Based on the above constraints such as data types and fractals, we have summarized the usage scenarios of the data input API, which are provided below for developer reference.

For Atlas A2 training products/Atlas A2 inference products and Atlas A3 training products/Atlas A3 inference products, **for the L1 Buffer->L0A Buffer/L0B Buffer path, the commonly used move instructions are Load2D, LoadDataWithTranspose, and Load3D. The instructions that can be called are summarized in Table 5:**

**Table 5** Summary of instructions called on the L1 Buffer->L0A Buffer/L0B Buffer path

<a name="zh-cn_topic_0000002513935478_table0327183312611"></a>

| Whether to transpose/Data type | b4 (int4b_t) | b8 | b16 | b32 |
| --- | --- | --- | --- | --- |
| A not transposed (enable not required for L1 Buffer->L0A Buffer) | Load3D (recommended for use) | Load2D, Load3D (recommended for use) | Load2D, Load3D (recommended for use) | Load2D, Load3D (recommended for use) |
| A transposed (enable transpose required for L1 Buffer->L0A Buffer) | Not supported | LoadDataWithTranspose | Load2D, LoadDataWithTranspose, Load3D (recommended for use) | LoadDataWithTranspose (data on L1 Buffer laid out in Nz layout (if and only if Row==16)), Load3D (recommended for use) |
| B not transposed (enable transpose required for L1 Buffer->L0B Buffer) | LoadDataWithTranspose | LoadDataWithTranspose | Load2D, LoadDataWithTranspose, Load3D (recommended for use) | LoadDataWithTranspose (data on L1 Buffer laid out in Zn layout (if and only if Col==16)), Load3D (recommended for use) |
| B transposed (enable not required for L1 Buffer->L0B Buffer) | - | Load2D | Load2D | Load2D |

> [!NOTE] Note
>
> - Because Load2D and LoadDataWithTranspose can only perform repeat moves in one direction during the move process, moving data blocks involving multiple directions requires the use of a for loop. In contrast, Load3D can complete data block moves in multiple directions by configuring mExtension and kExtension without using a for loop, so the overall scalar overhead is lower than that of Load2D and LoadDataWithTranspose. Therefore, in scenarios that satisfy the Load3D usage constraints, Load3D is recommended for use to implement data moves on the L1 Buffer->L0A Buffer/L0B Buffer path.
<!-- end id7 -->

<!-- npu="950" id8 -->
For Ascend 950PR/Ascend 950DT, Load2DV2 supports b4 (fp4)/b8/b16/b32 data types and transpose-scenario moves, and completes data block moves in multiple directions by configuring mStep and kStep. Compared with Load3D, it has smaller instruction bandwidth latency. Therefore, for the L1 Buffer->L0A Buffer/L0B Buffer path in cube multiplication without quantization coefficients, Load2DV2 is recommended for use to implement data moves. For cube multiplication with quantization coefficients, the Load2DMX instruction is required to implement data moves, which can complete not only the L1 Buffer->L0A Buffer/L0B Buffer path move but also the L1 Buffer->L0A\_MX/L0B\_MX Buffer path data move.
<!-- end id8 -->
