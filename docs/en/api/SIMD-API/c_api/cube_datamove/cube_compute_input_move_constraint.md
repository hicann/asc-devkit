# Matrix Computation Input Move Constraints

<!-- md-trans-meta sourceCommit=c891f0c5d5b2ac25c5683ff40810fa381d654ece translatedAt=2026-10-22T13:38:14.128Z pushedAt=2026-10-28T12:45:45.501Z nl=2 -->

## Alignment Constraints

For details, see [Storage Unit Description](../general_description_and_constraints.md#storage-unit-description).

- When the source address is in the L1 Buffer, the address must be 32-byte aligned; when the source address is in GM, the address must be 1-byte aligned.
- When the destination address is in the L0A Buffer/L0B Buffer, the address must be 512-byte aligned; when the destination address is in the L1 Buffer, the address must be 32-byte aligned.

## Move Granularity Constraints

- 2D cube move is performed in units of 512-byte fractals.
- 3D cube move is performed in units of 512-byte fractals.
<!-- npu="910b,A3" id4 -->
- asc_copy_l12l0b_sparse moves the compressed weight cube stored in 512-byte units to the L0B Buffer, and simultaneously moves the index cube in 128-byte units to the built-in dedicated buffer space.
<!-- end id4 -->

## Fractal Constraints

Data movement (GM-\>L1 Buffer), when not enabled to perform ND-to-Nz conversion along the path, does not change the inter-fractal and intra-fractal layout, such as Nz2Nz. When ND-to-Nz conversion is enabled, the fractal layout is always changed, such as ND2Nz.

2D cube move, when transpose is not enabled, can only change the inter-fractal layout, such as Nz2Zz. When transpose is enabled, it can change both the intra-fractal layout and the inter-fractal layout, such as Nz2Zn.

Transpose move always changes the intra-fractal layout and can change the inter-fractal layout as needed, such as Nz2Zn.

In the B32 scenario, two consecutive 16\*8 fractals of the source operand are merged into one 16\*16 square cube, and then the transpose is performed based on this square cube. Therefore, the two consecutive fractals must be merged into a square cube, which requires the cube on the L1 Buffer to satisfy the Nz layout (if and only if Row==16) or the Zn layout (if and only if Col==16).

3D cube move is mainly used to perform the image-to-column operation on feature maps in the NC1HWC0 format and move the expanded two-dimensional cube to the corresponding memory location. Therefore, for the source operand of 3D cube move, the data must be laid out in the NC1HWC0 format, and the destination operand data must be laid out according to the layout modes supported by the destination L0A Buffer/L0B Buffer.

<!-- npu="910b,A3" id6 -->
asc_copy_l12l0b_sparse is mainly used to move the compressed weight cube stored in 512-byte units to the L0B Buffer, and at the same time move the index cube stored in 128-byte units to the built-in dedicated buffer space (for subsequent reading by the asc_mmad_sparse API). Therefore, both the compressed weight cube and the index cube in the source operand must be laid out in the Zn format, where the compressed weight cube layout is 32\*16\*8bit=512Byte and the index cube layout is 32\*16\*2bit=128Byte.
<!-- end id6 -->

During cube computation, the common fractal support status is summarized in the following table:

<!-- npu="910b,A3" id7 -->
### For Atlas A2 products and Atlas A3 products

The common fractal support status is as follows, for developers' reference:

- **Data movement**
    - GM->L1 Buffer
        - ND->Nz:

            ![](../../../figures/datacopy_gm2l1_nd2nz.png)
        - Nz->Nz:

            ![](../../../figures/datacopy_gm2l1_nz2nz.png)
        - Zn->Zn:

            ![](../../../figures/datacopy_gm2l1_zn2zn.png)
- **2D cube move**
    - GM->L1 Buffer (does not support transpose)
        - Nz->Nz:

            ![](../../../figures/load2d_gm2l1_nz2nz.png)
        - Zn->Zn:

            ![](../../../figures/load2d_gm2l1_zn2zn.png)
    - GM->L0A Buffer (does not support transpose) L1 Buffer->L0A Buffer (transpose not enabled) (Nz->Zz)

        ![](../../../figures/load2d_gm2l0a_nz2zz.png)
    - GM->L0B Buffer (does not support transpose) L1 Buffer->L0B Buffer (transpose not enabled) (Zn->Zn)

        ![](../../../figures/load2d_gm2l0b_zn2zn.png)
    - L1 Buffer->L0A Buffer (enable transpose, only supports b16 data type) (Nz->Zz)

        ![](../../../figures/load2d_l12l0a_nz2zz.png)
    - L1 Buffer->L0B Buffer (enable transpose, only supports b16 data type) (Zn->Zn)

        ![](../../../figures/load2d_l12l0b_zn2zn.png)
- **transpose move**
    - L1 Buffer->L0A Buffer (b8) (Nz->Zz)

        ![](../../../figures/loaddatawithtranspose_l12l0a_b8_nz2zz.png)
    - L1 Buffer->L0B Buffer (b4/b8) (Zn->Zn)

        ![](../../../figures/loaddatawithtranspose_l12l0b_b4b8_zn2zn.png)
    - L1 Buffer->L0A Buffer (b16) (Nz->Zz)

        ![](../../../figures/loaddatawithtranspose_l12l0a_b16_nz2zz.png)
    - L1 Buffer->L0B Buffer (b16) (Zn->Zn)

        ![](../../../figures/loaddatawithtranspose_l12l0b_b16_zn2zn.png)
    - L1 Buffer->L0A Buffer (b32) (Nz (<span>if and only if Row==16</span>)->Zz)

        ![](../../../figures/loaddatawithtranspose_l12l0a_b32_nz2zz.png)
    - L1 Buffer->L0B Buffer (b32) (Zn (<span>if and only if Col==16</span>)->Zn)

        ![](../../../figures/loaddatawithtranspose_l12l0b_b32_zn2zn.png)
- **3D cube move**
    - L1 Buffer->L0A Buffer (NC1HWC0->Zz)

        ![](../../../figures/load3d_l12l0a_nc1hwc02zz.png)
    - L1 Buffer->L0B Buffer (NC1HWC0->Zn)

        ![](../../../figures/load3d_l12l0b_nc1hwc02zn.png)
- **asc_copy_l12l0b_sparse**
    - L1 Buffer->L0B Buffer (Zn->Zn)

        ![](../../../figures/loaddatawithsparse_l12l0b_zn2zn.png)

For the support status of some uncommon fractal conversions, see the following table:

**Table 1** Extended fractal conversion support status

| Move Instruction | Data Path | Fractal Support Status |
| --- | --- | --- |
| 2D fractal move | GM->L1 Buffer (does not support transpose) | Nz->Zz, Zn->Nn, Zz->Zz, Zz->Nz, Nn->Nn, and Nn->Zn |
| 2D fractal move | GM->L0A Buffer (does not support transpose),<br>L1 Buffer->L0A Buffer (transpose not enabled) | Nz->Nz, Zn->Nn, Zn->Zn, Zz->Zz, Zz->Nz, Nn->Nn, and Nn->Zn |
| 2D fractal move | GM->L0B Buffer (does not support transpose),<br>L1 Buffer->L0B Buffer (transpose not enabled) | Nz->Zz, Nz->Nz, Zn->Nn, Zz->Zz, Zz->Nz, Nn->Nn, and Nn->Zn |
| 2D fractal move | L1 Buffer->L0A Buffer (enable transpose, only supports the b16 data type) | Nz->Nz, Zn->Nn, Zn->Zn, Zz->Zz, Zz->Nz, Nn->Nn, and Nn->Zn |
| 2D fractal move | L1 Buffer->L0B Buffer (enable transpose, only supports the b16 data type) | Nz->Zz, Nz->Nz, Zn->Nn, Zz->Zz, Zz->Nz, Nn->Nn, and Nn->Zn |
| block transpose move | L1 Buffer->L0A Buffer (b8),<br>L1 Buffer->L0A Buffer (b16) | Nz->Nz, Zn->Nn, Zn->Zn, Zz->Zz, Zz->Nz, Nn->Nn, and Nn->Zn |
| block transpose move | L1 Buffer->L0B Buffer (b4/b8),<br>L1 Buffer->L0B Buffer (b16) | Nz->Zz, Nz->Nz, Zn->Nn, Zz->Zz, Zz->Nz, Nn->Nn, and Nn->Zn |
| block transpose move | L1 Buffer->L0A Buffer (b32) | Zz->Nz and Zz->Zz |
| block transpose move | L1 Buffer->L0B Buffer (b32) | Nn->Zn and Nn->Nn |

<!-- end id7 -->

<!-- npu="950" id8 -->
### For Ascend 950PR&950DT Products

Data movement (GM-\>L1 Buffer) supports enabling in-band DN-to-Nz conversion, which always changes the fractal layout.

When transpose is not enabled, 2D cube move does not change the inter-fractal layout or the intra-fractal layout, such as Nz2Nz. When transpose is enabled, it changes both the intra-fractal layout and the inter-fractal layout, such as Nz2Zn.

The data and coefficients of an MX cube are moved separately: A/B data uses the regular 2D overload or the `_transpose` variant; `asc_copy_l12l0a_mx`/`asc_copy_l12l0b_mx` only moves E8M0 coefficients and does not provide cube data movement or a transpose switch. Coefficients must be prepared in advance according to the corresponding layout, and coefficient move itself does not change the intra-fractal layout.

The common fractal support status is as follows, for developers' reference:

- **Data movement**
    - GM->L1 Buffer
        - ND->Nz:

            ![](../../../figures/datacopy_gm2l1_nd2nz.png)
        - DN->Nz:

            ![](../../../figures/datacopy_gm2l1_dn2nz.png)
        - Nz->Nz:

            ![](../../../figures/datacopy_gm2l1_nz2nz.png)
        - Zn->Zn:

            ![](../../../figures/datacopy_gm2l1_zn2zn.png)
- **2D cube move**
    - L1 Buffer->L0A Buffer (Nz->Nz)

        ![](../../../figures/load2dv2_l12l0a_nz2nz.png)
    - L1 Buffer->L0A Buffer (enable transpose) (Zn->Nz)

        ![](../../../figures/load2dv2_l12l0a_zn2nz.png)
    - L1 Buffer->L0B Buffer (Zn->Zn)

        ![](../../../figures/load2dv2_l12l0b_zn2zn.png)
    - L1 Buffer->L0B Buffer (enable transpose) (Nz->Zn)

        ![](../../../figures/load2dv2_l12l0b_nz2zn.png)
- **Independent move of MX data and coefficients**
    - L1 Buffer->L0A Buffer (data, call asc_copy_l12l0a, Nz->Nz)

        ![](../../../figures/load2dv2mx_l12l0a_nz2nz.png)
    - L1 Buffer->L0A_MX Buffer (coefficient, call asc_copy_l12l0a_mx)

        ![](../../../figures/load2dv2mx_l12l0amx.png)
    - L1 Buffer->L0B Buffer (data, call asc_copy_l12l0b, Zn->Zn)

        ![](../../../figures/load2dv2mx_l12l0b_zn2zn.png)
    - L1 Buffer->L0B_MX Buffer (coefficient, call asc_copy_l12l0b_mx)

        ![](../../../figures/load2dv2mx_l12l0bmx.png)
- **Transpose move**
    - L1 Buffer->L0B Buffer (b4/b8) (Zn->Zn)

        ![](../../../figures/loaddatawithtranspose_l12l0b_b4b8_zn2zn.png)
    - L1 Buffer->L0B Buffer (b16) (Zn->Zn)

        ![](../../../figures/loaddatawithtranspose_l12l0b_b16_zn2zn.png)
    - L1 Buffer->L0B Buffer (b32) (Zn (<span>if and only if Col==16</span>)->Zn)

        ![](../../../figures/loaddatawithtranspose_l12l0b_b32_zn2zn.png)
- **3D cube move**
    - L1 Buffer->L0A Buffer (NC1HWC0->Nz)

        ![](../../../figures/load3d_l12l0a_nc1hwc02nz.png)
    - L1 Buffer->L0B Buffer (NC1HWC0->Zn)

        ![](../../../figures/load3d_l12l0b_nc1hwc02zn.png)

For the support status of some uncommon fractal conversions, see the following table:

**Table 2** Extended fractal conversion support status

| Move instruction | Data path | Fractal support status |
| --- | --- | --- |
| 2D cube move | GM->L1 Buffer (does not support transpose) | Nz->Zz, Zn->Nn, Zz->Zz, Zz->Nz, Nn->Nn, Nn->Zn |
| 2D cube move | L1 Buffer->L0A Buffer (transpose not enabled) | Nz->Nz, Zn->Nn, Zn->Zn, Zz->Zz, Zz->Nz, Nn->Nn, Nn->Zn |
| 2D cube move | L1 Buffer->L0B Buffer (transpose not enabled) | Nz->Zz, Nz->Nz, Zn->Nn, Zz->Zz, Zz->Nz, Nn->Nn, Nn->Zn |
| 2D cube move | L1 Buffer->L0A Buffer (enable transpose, b16 is used as an example in the figure) | Nz->Nz, Zn->Nn, Zn->Zn, Zz->Zz, Zz->Nz, Nn->Nn, Nn->Zn |
| 2D cube move | L1 Buffer->L0B Buffer (enable transpose, b16 is used as an example in the figure) | Nz->Zz, Nz->Nz, Zn->Nn, Zz->Zz, Zz->Nz, Nn->Nn, Nn->Zn |
| Transpose move | L1 Buffer->L0B Buffer (b4/b8),<br>L1 Buffer->L0B Buffer (b16) | Nz->Zz, Nz->Nz, Zn->Nn, Zz->Zz, Zz->Nz, Nn->Nn, Nn->Zn |
| Transpose move | L1 Buffer->L0B Buffer (b32) | Nn->Zn, Nn->Nn |

<!-- end id8 -->

## Data Type Constraints

<!-- npu="910b,A3" id9 -->
For Atlas A2 products and Atlas A3 products, the data type constraints are shown in Table 3:

**Table 3** Data type constraints

| API/Overload | C API Pointer Type |
| --- | --- |
| `asc_copy_gm2l1` fractal move | half, bfloat16_t, int8_t, uint8_t, int32_t, uint32_t, float |
| `asc_copy_gm2l0a`/`asc_copy_gm2l0b`; `asc_copy_l12l0a`/`asc_copy_l12l0b` 2D overload | The above types and int4b_t; the specific move bit width and transpose restrictions still follow each API |
| `asc_copy_l12l0a_trans` / `asc_copy_l12l0b_trans` | half, bfloat16_t, int8_t, uint8_t, int32_t, uint32_t, float |
| `asc_copy_l12l0b_sparse` | int8_t weight and corresponding index |
| `asc_copy_l12l0a` 3D overload | half, bfloat16_t, int8_t, uint8_t, int4b_t, int32_t, uint32_t, float |
| `asc_copy_l12l0b` 3D overload | half, bfloat16_t, int32_t, uint32_t, float |

<!-- end id9 -->

<!-- npu="950" id10 -->
For Ascend 950PR&950DT products, the data type constraints are shown in Table 4:

**Table 4** Data type constraints

| ap/overload | C API pointer type |
| --- | --- |
| `asc_copy_gm2l1` 2D overload | int8_t, uint8_t, int16_t, uint16_t, half, bfloat16_t, int32_t, uint32_t, float, fp4x2_e1m2_t, fp4x2_e2m1_t, fp8_e4m3fn_t, fp8_e5m2_t, hifloat8_t; there is also a void bit-width move overload |
| `asc_copy_l12l0a`/`asc_copy_l12l0b` 2D overload and `_transpose` | The above typed overloads and int4b_t |
| `asc_copy_l12l0b_trans` | int8_t, uint8_t, half, bfloat16_t, int32_t, uint32_t, float, int4b_t, fp4x2_e1m2_t, fp4x2_e2m1_t, fp8_e4m3fn_t, fp8_e5m2_t, hifloat8_t |
| `asc_copy_l12l0a`/`asc_copy_l12l0b` 3D overload | int8_t, uint8_t, int16_t, uint16_t, half, bfloat16_t, int32_t, uint32_t, float, fp8_e4m3fn_t, fp8_e5m2_t, hifloat8_t |
| `asc_copy_l12l0a_mx`/`asc_copy_l12l0b_mx` | fp8_e8m0_t coefficient; 32B fractal, does not move A/B data |

<!-- end id10 -->

## Move Instruction Summary

<!-- npu="910b,A3" id11 -->
Based on the preceding constraints such as data types and fractals, we have summarized the usage scenarios of the data move-in API and provide them below for developers' reference.

For the Atlas A2 products and Atlas A3 products, **on the L1 Buffer -> L0A Buffer/L0B Buffer path, the commonly used move instructions include 2D fractal move, block transpose move, and asc_copy_l12l0a/asc_copy_l12l0b (3D). The instructions that can be called are summarized in Table 5 as follows:**

**Table 5** Summary of instructions that can be called on the L1 Buffer -> L0A Buffer/L0B Buffer path

| Whether to transpose/data type | b4 (int4b_t) | b8 | b16 | b32 |
| --- | --- | --- | --- | --- |
| A not transposed (transpose not enabled for L1 Buffer -> L0A Buffer) | asc_copy_l12l0a (3D) | 2D fractal move, asc_copy_l12l0a (3D) (recommended to use) | 2D fractal move, asc_copy_l12l0a (3D) (recommended to use) | 2D fractal move, asc_copy_l12l0a (3D) (recommended to use) |
| A transposed (transpose enabled for L1 Buffer -> L0A Buffer) | Not supported | Block transpose move | 2D fractal move, block transpose move, asc_copy_l12l0a (3D) (recommended to use) | Block transpose move (data on L1 Buffer uses Nz layout if and only if Row == 16), asc_copy_l12l0a (3D) (recommended to use) |
| B not transposed (transpose enabled for L1 Buffer -> L0B Buffer) | The currently public C API has no corresponding int4 block transpose overload | Block transpose move | 2D fractal move, block transpose move, asc_copy_l12l0b (3D) (recommended to use) | Block transpose move (data on L1 Buffer uses Zn layout if and only if Col == 16), asc_copy_l12l0b (3D) (recommended to use) |
| B transposed (transpose not enabled for L1 Buffer -> L0B Buffer) | - | 2D fractal move | 2D fractal move | 2D fractal move |

> [!NOTE] Description
>
> - Because 2D fractal move and block transpose move can only perform repeat move in one direction during the move process, moving data blocks in multiple directions requires a for loop. In contrast, asc_copy_l12l0a/asc_copy_l12l0b (3D) can move data blocks in multiple directions by configuring m_extension and k_extension without using a for loop. Therefore, the overall scalar overhead is lower than that of 2D fractal move and block transpose move. As a result, in scenarios that meet the usage constraints of asc_copy_l12l0a/asc_copy_l12l0b (3D), it is recommended to use the asc_copy_l12l0a/asc_copy_l12l0b (3D) instruction to implement data movement on the L1 Buffer -> L0A Buffer/L0B Buffer path.
<!-- end id11 -->

<!-- npu="950" id12 -->
For the Ascend 950PR&950DT products, asc_copy_l12l0a/asc_copy_l12l0b (2D) supports b4 (fp4)/b8/b16/b32 data types and transpose move scenarios. It moves data blocks in multiple directions by configuring m_step and k_step, and compared with asc_copy_l12l0a/asc_copy_l12l0b (3D), the instruction has lower bandwidth latency. Therefore, for cube multiplication without quantization coefficients, it is recommended to use the asc_copy_l12l0a/asc_copy_l12l0b (2D) instruction to implement data movement on the L1 Buffer -> L0A Buffer/L0B Buffer path. For cube multiplication with quantization coefficients, the asc_copy_l12l0a_mx/asc_copy_l12l0b_mx (coefficient move) instruction is required to implement data movement, where A/B are still moved in through the 2D API. The MX API only performs coefficient move on the L1 Buffer -> L0A_MX Buffer/L0B_MX Buffer path. The two types of APIs must be called separately.
<!-- end id12 -->
