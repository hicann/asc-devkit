# Overview of Cube Data Movement In

<!-- md-trans-meta sourceCommit=c891f0c5d5b2ac25c5683ff40810fa381d654ece translatedAt=2026-10-22T13:39:33.345Z pushedAt=2026-10-29T05:01:42.170Z nl=2 -->

Cube computation data movement in is a core category of APIs in the Ascend C programming framework for data movement, mainly implementing efficient data transfer between Global Memory and L0A Buffer/L0B Buffer. These APIs provide multiple data movement modes to meet the data movement requirements of the left and right cubes in cube computation, and are typically used together with the Mmad API.

The cube computation data movement in APIs support multiple data movement scenarios, including but not limited to **2D format data movement, 3D format conversion, compressed data decompression, cube transpose movement, and sparse data processing**. By flexibly configuring different parameter structures, you can precisely control key information such as the data movement path, start position, and movement length, fully utilizing the hardware's data movement capability and maximizing the overall execution performance of the operator.

The cube computation data movement in APIs support multiple data paths, each corresponding to specific storage levels and access characteristics.

**Table 1** **Data Paths and Storage Levels**

| Source Location | Source Address Alignment Requirement | Destination Location | Destination Address Alignment Requirement | Typical Application Scenarios |
| --- | --- | --- | --- | --- |
| Global Memory | 1 byte | L1 Buffer (`__cbuf__`, A matrix) | 32 bytes | Load left cube data from global memory to L1 Buffer. |
| Global Memory | 1 byte | L1 Buffer (`__cbuf__`, B matrix) | 32 bytes | Load right cube data from global memory to L1 Buffer. |
| Global Memory | 1 byte | L0A Buffer (`__ca__`) | 512 bytes | Load left cube data from global memory to L0A Buffer. |
| Global Memory | 1 byte | L0B Buffer (`__cb__`) | 512 bytes | Load right cube data from global memory to L0B Buffer. |
| L1 Buffer (`__cbuf__`, A matrix) | 32 bytes | L0A Buffer (`__ca__`) | 512 bytes | Load left cube data from L1 Buffer to L0A Buffer. |
| L1 Buffer (`__cbuf__`, B matrix) | 32 bytes | L0B Buffer (`__cb__`) | 512 bytes | Load right cube data from L1 Buffer to L0B Buffer. |
| L1 Buffer | 32 bytes | BiasTable Buffer | 64 bytes | Load the Bias data required for cube computation from L1 Buffer to BiasTable Buffer. |
| L1 Buffer | 32 bytes | Fixpipe Buffer | 128 bytes | Load data such as quantization parameters required during Fixpipe movement from L1 Buffer to Fixpipe Buffer. |

<!-- npu="950" id1 -->
> [!NOTE] Note
> Due to hardware changes, the Ascend 950PR&950DT products has removed the data paths from GM to L0A Buffer and L0B Buffer. As a result, the original data movement from GM to L0A Buffer and L0B Buffer must be split into two steps: data movement from GM to L1 Buffer and data movement from L1 Buffer to L0A Buffer and L0B Buffer.
<!-- end id1 -->

## Interface Classification and Usage Scenarios for GM->L1 Buffer

**Table 2** GM->L1 Buffer APIs

| Interface Category | Main Functions | Supported Path | Typical Application Scenarios |
| --- | --- | --- | --- |
| [asc_copy_gm2l1](asc_copy_gm2l1/asc_copy_gm2l1.md), contiguous/high-dimensional split overload | Copies laid-out data, preserving content and format | GM-\>L1 Buffer | Complete cube, non-contiguous cube slices |
| [asc_copy_gm2l1_nd2nz](asc_copy_gm2l1_nd2nz/asc_copy_gm2l1_nd2nz.md) | Moves data and converts ND to Nz | GM-\>L1 Buffer | Linear cube input |
| [asc_copy_gm2l1](asc_copy_gm2l1/asc_copy_gm2l1.md), 2D fractal overload | Moves data by fractal | GM-\>L1 Buffer | Offline fractal data |

<!-- npu="950" id2 -->
For Ascend 950PR&950DT products, the following GM->L1 Buffer APIs are added. See Table 3.

**Table 3** GM->L1 Buffer APIs

| Interface Category | Main Functions | Supported Path | Typical Application Scenarios |
| --- | --- | --- | --- |
| [asc_copy_gm2l1_dn2nz](asc_copy_gm2l1_dn2nz.md) | DN-\>Nz | GM-\>L1 Buffer | Column-major cube input |
| [asc_copy_gm2l1_align](asc_copy_gm2l1_align.md) | Unaligned data movement and left/right padding | GM-\>L1 Buffer | Byte length or boundary not meeting normal block copy requirements |
| [asc_copy_gm2l1 (2D)](asc_copy_gm2l1/asc_copy_gm2l1_2d_arch_3510.md) | 2DV2 fractal data movement with configurable L2 cache policy | GM-\>L1 Buffer | Multi-directional fractal block input |
| [asc_copy_gm2l1 (high-dimensional split)](asc_copy_gm2l1/asc_copy_gm2l1_highdim_split_arch_3510.md) | Supports `asc_channel_pad_mode` channel padding/compression | GM-\>L1 Buffer | Expand the valid region by 32B channels or remove high-order bytes |
<!-- end id2 -->

<!-- npu="950" id5 -->
## UB->L1 Buffer API Classification and Usage Scenarios

**Table 4** UB->L1 Buffer APIs

| Interface Category | Main Functions | Supported Path | Typical Application Scenarios |
| --- | --- | --- | --- |
| [asc_copy_ub2l1](../vector_datamove/asc_copy_ub2l1.md) continuous overload | Continuous data movement | UB-\>L1 Buffer | Vector results fed into cube computation |
| High-dimensional split overload of the same API | Configures length and interval in 32B blocks | UB-\>L1 Buffer | Non-contiguous vector result data movement |
<!-- end id5 -->

## Interface Classification and Usage Scenarios for L1 Buffer -> L0A Buffer/L0B Buffer

**Table 5** L1 Buffer -> L0A/B Buffer APIs

| Interface Category | Main Functions | Supported Path | Typical Application Scenarios |
| --- | --- | --- | --- |
| 2D overload of [asc_copy_l12l0a](asc_copy_l12l0a/asc_copy_l12l0a.md) and [asc_copy_l12l0b](asc_copy_l12l0b/asc_copy_l12l0b.md) | Data movement by 512B fractal | L1 Buffer-\>L0A Buffer/L0B Buffer | Ordinary cube input |
| 3D overload of the same APIs | Online img2col for NC1HWC0 input | L1 Buffer-\>L0A Buffer/L0B Buffer | Convolution expansion |
| [asc_copy_l12l0a_trans](asc_copy_l12l0a_trans.md) and [asc_copy_l12l0b_trans](asc_copy_l12l0b_trans/asc_copy_l12l0b_trans.md) | Transpose data movement after fractal concatenation | L1 Buffer-\>L0A Buffer/L0B Buffer | Block transpose at specific bit widths |
| [asc_copy_l12bt](asc_copy_l12bt/asc_copy_l12bt.md) | Bias data movement | L1 Buffer-\>BiasTable | Accumulation initial value |
| [asc_copy_l12fb](asc_copy_l12fb/asc_copy_l12fb.md) | Sideband coefficient data movement | L1 Buffer-\>Fixpipe Buffer | Quantization/activation |

<!-- npu="910b,A3" id8 -->
For Atlas A2 products and Atlas A3 products, the following L1 Buffer -> L0B Buffer APIs are supported. See Table 6.

**Table 6** Sparse cube data movement

| Interface Category | Main Functions | Supported Path | Typical Application Scenarios |
| --- | --- | --- | --- |
| [asc_copy_l12l0b_sparse](asc_copy_l12l0b_sparse.md) | Moves the densified weight cube and index cube required for 4-select-2 structured sparse cube computation. | L1 Buffer -> L0B Buffer | Data movement for sparse cube multiplication computation. |
<!-- end id8 -->

<!-- npu="950" id9 -->
For Ascend 950PR&950DT products, the following APIs are added. See Table 7.

**Table 7** 3510 2D and MX extensions

| API | Function | Usage Mode |
| --- | --- | --- |
| 2D overload of `asc_copy_l12l0a` / `asc_copy_l12l0b` | 2D two-dimensional fractal data movement, supporting FP4/FP8/HiFloat8 and integer bit widths | Set `m_step`, `k_step`, and source/destination strides |
| `asc_copy_l12l0a_transpose` / `asc_copy_l12l0b_transpose` | Transpose variants of the same 2DV2 instruction | Corresponds to the parameter layout of the ordinary 2D overload; not the `_trans` instruction |
| [asc_copy_l12l0a_mx](asc_copy_l12l0a_mx.md) / [asc_copy_l12l0b_mx](asc_copy_l12l0b_mx.md) | Moves only the E8M0 coefficients of the 32B fractal | Called separately from A/B data movement, matching the mapped address |
<!-- end id9 -->

## Auxiliary Configuration API Description

**Table 8** Auxiliary configuration APIs

| API | Function | Architecture/Precautions |
| --- | --- | --- |
| [asc_fill_l1](asc_fill_l1.md), [asc_fill_l0a](asc_fill_l0a.md), [asc_fill_l0b](asc_fill_l0b.md) | Initializes the buffer. | Supports L1 Buffer, L0A Buffer, and L0B Buffer. |
| [asc_set_l13d_fmatrix](asc_set_l13d_fmatrix.md), [asc_set_l13d_fmatrix_b](asc_set_l13d_fmatrix_b.md) | FeatureMap size and padding boundary. | Selected based on the A/B side and `f_matrix_ctrl`. |
| [asc_set_l12l0a_3d_padding](asc_set_l12l0a_3d_padding.md), [asc_set_l12l0b_3d_padding](asc_set_l12l0b_3d_padding.md) | Padding configuration. | Selected based on the A/B side and `f_matrix_ctrl`. |
| [asc_set_l13d_size](asc_set_l13d_size.md) | L1 Buffer boundary size. | Per the corresponding 3D API requirements. |
| [asc_set_l13d_rpt](asc_set_l13d_rpt.md) | 3D repetition and stride. | Per the corresponding overload configuration. |
| [asc_load_image_to_cbuf](asc_load_image_to_cbuf.md) | Moves an image into the L1 Buffer. | Per the supported range of this API. |
| [asc_copy_l12l0c](asc_copy_l12l0c.md) | Moves the initial value of the L1 Buffer-\>L0C Buffer. | Supplement to the initialization path. |
| [asc_set_l12l0a_3d_padding](asc_set_l12l0a_3d_padding.md), [asc_set_l12l0b_3d_padding](asc_set_l12l0b_3d_padding.md) | Typed padding value configuration. | Note the registers on both sides and the shared configuration. |
