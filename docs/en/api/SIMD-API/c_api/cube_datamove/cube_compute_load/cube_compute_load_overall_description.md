---
okftype: reference.concept
description: "Provides an overall description of the classification, data paths, and storage hierarchy of cube data load interfaces, and lists the interfaces for GM-to-L1, L1-to-L0A/L0B, sparse and MX scenarios, as well as the supporting auxiliary configuration interfaces."
---
# Overview of Cube Data Load

<!-- md-trans-meta sourceCommit=54f1d966cfcc22290ddc2c6312bbe4c6ac2a9232 translatedAt=2026-11-16T11:06:44.334Z pushedAt=2026-11-18T08:15:34.294Z nl=2 -->

Cube data load is a core category of interfaces used for data movement in the Ascend C programming framework, mainly implementing efficient data transfer between Global Memory and L0A Buffer/L0B Buffer. This category of interfaces provides multiple data movement modes, which can meet the data movement requirements of the left and right cubes in cube computation, and is usually used together with the Mmad interface.

The cube computation data load interfaces support multiple data movement scenarios, including but not limited to **2D format data movement, 3D format conversion, compressed data decompression, cube transpose movement, and sparsification data processing**. By flexibly configuring different parameter structures, developers can precisely control key information such as the data movement path, start position, and movement length, fully utilize the hardware data movement capability, and maximize the overall execution performance of the operator.

The cube data load interfaces support multiple data paths, each corresponding to a specific storage hierarchy and access characteristics.

**Table 1** **Data paths and storage hierarchy**

| Source Location | Source Address Alignment Requirement | Destination Location | Destination Address Alignment Requirement | Typical Application Scenarios |
| --- | --- | --- | --- | --- |
| Global Memory | 1 byte | L1 Buffer (`__cbuf__`, A cube) | 32 bytes | Load left cube data from global memory to L1 Buffer. |
| Global Memory | 1 byte | L1 Buffer (`__cbuf__`, B cube) | 32 bytes | Load right cube data from global memory to L1 Buffer. |
| Global Memory | 1 byte | L0A Buffer (`__ca__`) | 512 bytes | Load left cube data from global memory to L0A Buffer. |
| Global Memory | 1 byte | L0B Buffer (`__cb__`) | 512 bytes | Load right cube data from global memory to L0B Buffer. |
| L1 Buffer (`__cbuf__`, A cube) | 32 bytes | L0A Buffer (`__ca__`) | 512 bytes | Load left cube data from L1 Buffer to L0A Buffer. |
| L1 Buffer (`__cbuf__`, B cube) | 32 bytes | L0B Buffer (`__cb__`) | 512 bytes | Load right cube data from L1 Buffer to L0B Buffer. |
| L1 Buffer | 32 bytes | BiasTable Buffer | 64 bytes | Load the Bias data required for cube computation from L1 Buffer to BiasTable Buffer. |
| L1 Buffer | 32 bytes | Fixpipe Buffer | 128 bytes | Load data such as quantization parameters required during Fixpipe movement from L1 Buffer to Fixpipe Buffer. |

<!-- npu="950" id1 -->
> [!NOTE] Note
> Due to hardware changes, the Ascend 950PR&950DT products remove the data paths from GM to L0A Buffer and L0B Buffer. As a result, the original data movement from GM to L0A Buffer and L0B Buffer needs to be split into two steps, namely data movement from GM to L1 Buffer and data movement from L1 Buffer to L0A Buffer and L0B Buffer.
<!-- end id1 -->

## Interface Classification and Usage Scenarios for GM->L1 Buffer

**Table 2** GM->L1 Buffer interfaces

| Interface Category | Main Functions | Supported Path | Typical Application Scenarios |
| --- | --- | --- | --- |
| [asc_copy_gm2l1](asc_copy_gm2l1.md), contiguous/high-dimensional split overload | Copies laid-out data, preserving content and format | GM-\>L1 Buffer | Complete cube, non-contiguous cube slices |
| [asc_copy_gm2l1_nd2nz](asc_copy_gm2l1_nd2nz.md) | Moves data and converts ND to Nz | GM-\>L1 Buffer | Linear cube input |
| [asc_copy_gm2l1](asc_copy_gm2l1.md), 2D fractal overload | Moves data by fractal | GM-\>L1 Buffer | Offline fractal data |

<!-- npu="950" id2 -->
For the Ascend 950PR&950DT products, the following GM->L1 Buffer interfaces are added. Developers should refer to Table 3.

**Table 3** GM->L1 Buffer interfaces

| Interface Category | Main Functions | Supported Path | Typical Application Scenarios |
| --- | --- | --- | --- |
| [asc_copy_gm2l1_dn2nz](asc_copy_gm2l1_dn2nz.md) | DN-\>Nz | GM-\>L1 Buffer | Column-major cube input |
| [asc_copy_gm2l1_align](asc_copy_gm2l1_align.md) | Unaligned movement and left/right padding | GM-\>L1 Buffer | Byte length or boundary does not meet ordinary block copy requirements |
| [asc_copy_gm2l1 (2D)](asc_copy_gm2l1_2d_arch_3510.md) | 2DV2 fractal movement, with configurable L2 Cache policy | GM-\>L1 Buffer | Multi-directional fractal block input |
| [asc_copy_gm2l1 (high-dimensional split)](asc_copy_gm2l1_highdim_split_arch_3510.md) | Supports `asc_channel_pad_mode` channel padding/compression | GM-\>L1 Buffer | Expand the valid region by 32B channels or remove high-order bytes |
<!-- end id2 -->

<!-- npu="950" id5 -->

## Interface Classification and Usage Scenarios for UB->L1 Buffer

**Table 4** UB->L1 Buffer interfaces

| Interface Category | Main Functions | Supported Path | Typical Application Scenarios |
| --- | --- | --- | --- |
| [asc_copy_ub2l1](../../vector_datamove/asc_copy_ub2l1.md) continuous overload | Continuous data movement | UB-\>L1 Buffer | Vector results sent to cube computation |
| Same interface high-dimensional split overload | Configure length and interval by 32B blocks | UB-\>L1 Buffer | Non-contiguous vector result movement |
<!-- end id5 -->

## Interface Classification and Usage Scenarios for L1 Buffer->L0A Buffer/L0B Buffer

**Table 5** L1 Buffer->L0A/B Buffer interfaces

| Interface Category | Main Functions | Supported Path | Typical Application Scenarios |
| --- | --- | --- | --- |
| 2D Overload of [asc_copy_l12l0a](asc_copy_l12l0a.md) and [asc_copy_l12l0b](asc_copy_l12l0b.md) | Moves data by 512B fractal | L1 Buffer-\>L0A Buffer/L0B Buffer | Ordinary cube input |
| 3D Overload of the same interfaces | NC1HWC0 input online img2col | L1 Buffer-\>L0A Buffer/L0B Buffer | Convolution expansion |
| [asc_copy_l12l0a_trans](asc_copy_l12l0a_trans.md) and [asc_copy_l12l0b_trans](asc_copy_l12l0b_trans.md) | Moves data after concatenating fractals and transposing | L1 Buffer-\>L0A Buffer/L0B Buffer | Block transpose at specific bit widths |
| [asc_copy_l12bt](asc_copy_l12bt.md) | Moves bias in | L1 Buffer-\>BiasTable | Accumulation initial value |
| [asc_copy_l12fb](asc_copy_l12fb.md) | Moves in per-lane coefficients | L1 Buffer-\>Fixpipe Buffer | Quantization/activation |

<!-- npu="910b,A3" id8 -->
For Atlas A2 products and Atlas A3 products, the following L1 Buffer->L0B Buffer interfaces are supported. Developers should refer to Table 6.

**Table 6** Sparse cube movement

| Interface Category | Main Functions | Supported Path | Typical Application Scenarios |
| --- | --- | --- | --- |
| [asc_copy_l12l0b_sparse](asc_copy_l12l0b_sparse.md) | Moves the densified weight cube and index cube required for 4-out-of-2 structured sparse cube computation. | L1 Buffer->L0B Buffer | Data movement for sparse cube multiplication computation. |
<!-- end id8 -->

<!-- npu="950" id9 -->
For Ascend 950PR&950DT products, the following interfaces are newly added. Developers should refer to Table 7.

**Table 7** 3510 2D and MX extensions

| Interface | Function | Usage Mode |
| --- | --- | --- |
| 2D Overload of `asc_copy_l12l0a` / `asc_copy_l12l0b` | 2D two-dimensional fractal movement, supporting FP4/FP8/HiFloat8 and integer bit widths | Set `m_step`, `k_step`, and source/destination strides |
| `asc_copy_l12l0a_transpose` / `asc_copy_l12l0b_transpose` | Transpose variants of the same 2DV2 instruction | Corresponds to the parameter layout of the normal 2D overload; not the `_trans` instruction |
| [asc_copy_l12l0a_mx](asc_copy_l12l0a_mx.md) / [asc_copy_l12l0b_mx](asc_copy_l12l0b_mx.md) | Moves only the E8M0 coefficients of the 32B fractal | Called separately from A/B data movement, matching the mapped addresses |
<!-- end id9 -->

## Auxiliary Configuration Interface Description

**Table 8** Auxiliary configuration interfaces

| Interface | Function | Architecture/Precautions |
| --- | --- | --- |
| [asc_fill_l1](../cube_load_aux_config/asc_fill_l1.md), [asc_fill_l0a](../cube_load_aux_config/asc_fill_l0a.md), [asc_fill_l0b](../cube_load_aux_config/asc_fill_l0b.md) | Initialize the Buffer | Supports L1 Buffer, L0A Buffer, and L0B Buffer |
| [asc_set_l13d_fmatrix](../cube_load_aux_config/asc_set_l13d_fmatrix.md), [asc_set_l13d_fmatrix_b](../cube_load_aux_config/asc_set_l13d_fmatrix_b.md) | FeatureMap size and Padding boundary | Select based on the A/B side and `f_matrix_ctrl` |
| [asc_set_l12l0a_3d_padding](../cube_load_aux_config/asc_set_l12l0a_3d_padding.md), [asc_set_l12l0b_3d_padding](../cube_load_aux_config/asc_set_l12l0b_3d_padding.md) | Padding configuration | Select based on the A/B side and `f_matrix_ctrl` |
| [asc_set_l13d_size](../cube_load_aux_config/asc_set_l13d_size.md) | L1 Buffer boundary size | As required by the corresponding 3D interface |
| [asc_set_l13d_rpt](../cube_load_aux_config/asc_set_l13d_rpt.md) | 3D repetition and stride | Configure according to the corresponding overload |
| [asc_load_image_to_cbuf](../cube_load_aux_config/asc_load_image_to_cbuf.md) | Load image into L1 Buffer | Configure according to the supported range of this interface |
| [asc_copy_l12l0c](asc_copy_l12l0c.md) | L1 Buffer-\>L0C Buffer initial value movement | Supplement to the initialization path |
| [asc_set_l12l0a_3d_padding](../cube_load_aux_config/asc_set_l12l0a_3d_padding.md), [asc_set_l12l0b_3d_padding](../cube_load_aux_config/asc_set_l12l0b_3d_padding.md) | Typed Padding value configuration | Note the registers on both sides and the shared configuration |
