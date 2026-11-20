---
okftype: reference.concept
description: "Describes the key points for selecting data movement APIs by category, including contiguous, high-dimensional slicing, multi-dimensional slicing, real-time format conversion, real-time quantization activation, unaligned, UB internal copy, matrix fractal, NDDMA, and Reg exchange. Emphasizes that the units of parameters with the same name vary with the path and must be verified per API."
---
# Data Movement Concepts

<!-- md-trans-meta sourceCommit=54f1d966cfcc22290ddc2c6312bbe4c6ac2a9232 translatedAt=2026-11-16T01:40:02.256795+00:00 pushedAt=2026-11-16T02:10:55.346Z nl=2 -->

Data movement APIs cover capabilities ranging from contiguous copy to multi-dimensional addressing, format conversion, and quantization and activation. When selecting an API, first determine the source and destination storage units and the executing core, and then determine whether the data is contiguous, whether it is aligned, and whether on-the-fly processing is required. Parameters with the same name in different APIs may use different units, so the corresponding API documentation should be checked before calling.

## Overview of Data Movement Function Categories

| Function Category | Main Purpose | Representative API |
| --- | --- | --- |
| Contiguous data movement | Moves a contiguous block of data with format and content unchanged | [asc_copy_gm2ub](../../vector_datamove/asc_copy_gm2ub/asc_copy_gm2ub_arch_3510.md), [asc_copy_ub2gm](../../vector_datamove/asc_copy_ub2gm/asc_copy_ub2gm_arch_3510.md) |
| High-dimensional slicing data movement | Moves non-contiguous data by fixed block count, block length, and stride. | [asc_copy_gm2l1](../../cube_datamove/cube_compute_load/asc_copy_gm2l1_highdim_split_arch_3510.md), [asc_copy_ub2ub](../../vector_datamove/asc_copy_ub2ub.md) |
| Multi-dimensional slicing and rearrangement | Moves data by 1- to 5-dimensional loops, strides, and padding. | [asc_ndim_copy_gm2ub](../../vector_datamove/asc_ndim_copy_gm2ub.md) |
| On-the-fly format conversion | Completes ND, DN, Nz, and other format conversions during data movement. | [asc_copy_gm2l1_nd2nz](../../cube_datamove/cube_compute_load/asc_copy_gm2l1_nd2nz_arch_3510.md), [asc_copy_l0c2gm](../../cube_datamove/cube_compute_store/asc_copy_l0c2gm_arch_3510.md) |
| On-the-fly quantization and activation | Completes type conversion, quantization, or activation when moving L0C results out. | [asc_copy_l0c2gm](../../cube_datamove/cube_compute_store/asc_copy_l0c2gm_arch_3510.md), [asc_copy_l0c2ub](../../cube_datamove/cube_compute_store/asc_copy_l0c2ub.md) |
| Unaligned data movement | Controls the on-chip padded layout or padding for non-32-byte block lengths explicitly. | [asc_copy_gm2ub_align](../../vector_datamove/asc_copy_gm2ub_align/asc_copy_gm2ub_align_arch_3510.md), [asc_copy_gm2l1_align](../../cube_datamove/cube_compute_load/asc_copy_gm2l1_align.md) |
| UB internal copy | Copies data contiguously or by blocks within the UB. | [asc_copy_ub2ub](../../vector_datamove/asc_copy_ub2ub.md) |
| Matrix fractal movement | Moves two-dimensional, three-dimensional, or MX matrices between L1 and L0A/L0B. | [asc_copy_l12l0a](../../cube_datamove/cube_compute_load/asc_copy_l12l0a.md), [asc_copy_l12l0b](../../cube_datamove/cube_compute_load/asc_copy_l12l0b.md) |
| Register and UB data exchange | Loads or stores vector and mask data in Vector Functions. | [Register data load](../../reg_compute/load/reg_load_overview.md), [Register data store](../../reg_compute/store/reg_store_overview.md) |

## Contiguous Data Movement

Contiguous copy reads a contiguous segment of data from the source start address and writes it to the destination start address in the original order, without changing the data format or content during data movement. Most contiguous APIs specify the total byte count through `size`; matrix movement APIs such as L1 to L0A/L0B use a complete fractal as the minimum granularity.

Contiguous copy applies to scenarios where both source and destination data are stored contiguously and the addresses meet the API alignment requirements, such as loading vector input through `asc_copy_gm2ub`, writing back vector results through `asc_copy_ub2gm`, or loading contiguous bias data through `asc_copy_l12bt`.

**Figure 1** Contiguous data movement diagram

![Contiguous data movement diagram](../../../../figures/continuous_data_copy_diagram_c_api.png "Contiguous data movement diagram")

## High-Dimensional Slicing Data Movement

When the source or destination data consists of multiple equal-length data blocks with fixed intervals between adjacent blocks, the high-dimensional slicing mode can be used. Such APIs usually include the following parameters:

- `burst_count` or `n_burst`: Number of data blocks to move.
- `burst_len` or `len_burst`: Effective length of each data block.
- `src_stride`, `dst_stride`: Distance between the start addresses of adjacent data blocks.
- `src_gap`, `dst_gap`: Interval between the end address of the previous data block and the start address of the next data block.

Parameter units vary by path. For example, on NPU architecture version 3510, `asc_copy_gm2ub` describes block length and start address spacing in bytes, `asc_copy_ub2ub` describes block length and inter-block interval in 32-byte DataBlocks, and the high-dimensional slicing mode of `asc_copy_gm2l1` describes block length and start address spacing in 32-byte units. Do not infer units from parameter names alone.

When the data blocks are contiguous at both the source and the destination, the movement result is equivalent to a contiguous copy, as shown in Figure 2; after configuring a non-zero interval, source data can be skipped or a region can be reserved at the destination, as shown in Figure 3.

**Figure 2** High-dimensional slicing contiguous copy diagram

![High-dimensional slicing contiguous copy diagram](../../../../figures/continuous_data_copy_diagram_c_api.png "High-dimensional slicing contiguous copy diagram")

**Figure 3** High-dimensional slicing non-contiguous data movement diagram

![High-dimensional slicing non-contiguous data movement diagram](../../../../figures/discontinuous_data_copy_diagram.png "High-dimensional slicing non-contiguous data movement diagram")

## Multi-Dimensional Slicing Data Movement

[asc_ndim_copy_gm2ub](../../vector_datamove/asc_ndim_copy_gm2ub.md) describes GM-to-UB data movement through up to five levels of loops. Each dimension independently configures the number of processed elements, source stride, destination stride, and left/right padding element counts, so sub-regions can be extracted from a multi-dimensional tensor, and Transpose, Broadcast, and Padding can also be implemented.

Multi-dimensional slicing is suitable for extracting ROIs from feature maps, selecting tiling sub-blocks, or skipping invalid boundary regions. The source and destination strides of each dimension are set through [asc_set_ndim_loop_stride](../../vector_datamove/asc_set_ndim_loop_stride.md), and the padding counts for dimensions 1 to 4 are set through [asc_set_ndim_pad_count](../../vector_datamove/asc_set_ndim_pad_count.md).

## On-the-Fly Format Conversion Data Movement

Cube computation inputs usually need to adopt Nz fractal layout, and outputs may also need to be restored to ND or DN layout. On-the-fly format conversion can complete layout changes during data movement, reducing additional rearrangement operations:

- [asc_copy_gm2l1_nd2nz](../../cube_datamove/cube_compute_load/asc_copy_gm2l1_nd2nz_arch_3510.md) moves ND data from GM to L1 and converts it to Nz; before calling, configure the destination Nz matrix stride and matrix count through [asc_set_gm2l1_nz_para](../../cube_datamove/cube_load_aux_config/asc_set_gm2l1_nz_para.md).
- [asc_copy_gm2l1_dn2nz](../../cube_datamove/cube_compute_load/asc_copy_gm2l1_dn2nz.md) moves DN data from GM to L1 and converts it to Nz.
- [asc_copy_l0c2gm](../../cube_datamove/cube_compute_store/asc_copy_l0c2gm_arch_3510.md), [asc_copy_l0c2l1](../../cube_datamove/cube_compute_store/asc_copy_l0c2l1_arch_3510.md), and [asc_copy_l0c2ub](../../cube_datamove/cube_compute_store/asc_copy_l0c2ub.md) can perform Nz2ND or Nz2DN conversion when moving L0C results out.

On-the-fly conversion changes the data layout and actual occupied space at the destination, so the destination stride and boundaries should be calculated according to the converted format.

**Figure 4** ND2Nz format conversion diagram

![ND2Nz format conversion diagram](../../../../figures/nd2nz_conversion_half.png "ND2Nz format conversion diagram")

## On-the-Fly Quantized Activation Data Movement

Cube computation results reside in the L0C Buffer, and the source data type is usually `float` or `int32_t`. L0C move-out APIs can combine type conversion, scalar/tensor quantization, ReLU or Leaky ReLU processing when writing to GM, L1, or UB, and can be combined with format conversions such as Nz2ND and Nz2DN.

Before enabling on-the-fly processing, set the quantization parameters, activation parameters, and format conversion parameters through the corresponding configuration APIs. For example, tensor quantization requires configuring the address where quantization parameters reside, and Nz2DN requires configuring channel parameters. The source and destination data types must match `quant_pre_mode`, and destination space should be reserved according to the converted data type and layout.

## Unaligned Data Movement.

Data movement APIs usually require UB or L1 start addresses to be 32-byte aligned, but valid length constraints vary by API. `asc_copy_gm2ub` and `asc_copy_ub2gm` express valid length in bytes, and the length only needs to satisfy data type byte alignment and does not need to be an integer multiple of 32 bytes. For ordinary APIs, GM-to-UB still pads writes on the UB side, and UB-to-GM also pads reads from UB, so capacity calculations must not be based solely on the valid length. When explicit control of on-chip padding layout for non-32-byte data, left/right padding, L2 Cache policy, or a larger high-dimensional slicing parameter range is required, use dedicated unaligned APIs:

- For GM to UB, use [asc_copy_gm2ub_align](../../vector_datamove/asc_copy_gm2ub_align/asc_copy_gm2ub_align_arch_3510.md). The destination of the high-dimensional slicing prototype can select Compact or Normal mode and supports constant or first-element padding.
- For UB to GM, use [asc_copy_ub2gm_align](../../vector_datamove/asc_copy_ub2gm_align/asc_copy_ub2gm_align_arch_3510.md). The hardware adds dummy data when reading UB and discards the padded portion when writing to GM.
- For GM to L1, use [asc_copy_gm2l1_align](../../cube_datamove/cube_compute_load/asc_copy_gm2l1_align.md), which supports Compact, Normal, left/right padding, and loop padding modes.

Unaligned APIs may access on-chip space after alignment padding. Even if there is only a small amount of valid data, sufficient capacity must be reserved for the padded range, and you must not access unallocated addresses by expanding the data movement range through ordinary APIs.

## UB Internal Data Movement

[asc_copy_ub2ub](../../vector_datamove/asc_copy_ub2ub.md) performs contiguous or high-dimensional slicing copy within UB and runs on PIPE_V. Contiguous mode specifies the byte count through `size`; high-dimensional slicing mode describes the address layout through the number of data blocks, block length, and source/destination intervals, where block length and intervals are in units of 32-byte DataBlocks.

The source and destination DataBlocks actually involved in data movement must not overlap. If elements need to be selected by mask in a Vector Function, or data processing needs to be completed in registers, Reg load, compute, and store APIs should be used instead of expanding the capability boundary of UB copy.

## Matrix Fractal Movement.

L1-to-L0A/L0B APIs use matrix fractals as the primary movement granularity to prepare left and right matrices for Cube computation. NPU architecture version 3510 supports the following modes:

- Two-dimensional fractal movement: Loads data according to row/column start positions, fractal strides, and source/destination matrix strides, with an optional prototype that supports transpose.
- Three-dimensional img2col movement: Extracts convolution windows according to Feature Map, Filter, Stride, and Padding configurations, and supports Repeat mode.
- Two-dimensional fractal transpose: Completes transpose through [asc_copy_l12l0b_trans](../../cube_datamove/cube_compute_load/asc_copy_l12l0b_trans_arch_3510.md) according to repeat count and fractal interval.
- MX quantization coefficient movement: Moves quantization coefficients to the corresponding MX Buffer through `asc_copy_l12l0a_mx` or `asc_copy_l12l0b_mx` and establishes mapping with the matrix data address.

The complete fractal of two-dimensional movement is usually 512 bytes, and L0A/L0B destination addresses usually require 512-byte alignment. The specific fractal shape varies with data bit width and A/B matrix direction.

## Multi-Dimensional Data Movement NDDMA

NDDMA describes GM-to-UB data access with 1- to 5-dimensional loops. Each dimension can independently configure the element count, source stride, destination stride, and left/right padding. Stride combinations enable multi-dimensional padding, transpose, broadcast, and slice, making it more suitable for irregular multi-dimensional layouts than fixed data block modes.

The unit of stride in each dimension is the number of elements. Padding supports constant padding and nearest-value padding; the constant value is set through [asc_set_ndim_pad_value](../../vector_datamove/asc_set_ndim_pad_value.md). When multiple cores may read the same GM block and the data may be updated by other cores, call [asc_ndim_copy_dci](../../vector_datamove/asc_ndim_copy_dci.md) before the data movement to refresh the NDDMA DataCache. NDDMA covers only the GM-to-UB direction.

## Register and UB Data Exchange.

Vector Function reads UB data into vector data registers, mask registers, or unaligned registers through load APIs, and writes results back to UB through store APIs. Aligned access usually uses 32 bytes as the baseline; for tail blocks or arbitrary start addresses, use unaligned load and store APIs and reuse the `vector_load_unalign` or `vector_store_unalign` state as specified.

The effective element count of Reg APIs is determined by both the data type and the mask. When multiple Reg instructions access overlapping UB regions and have read/write conflicts, use [asc_mem_bar](../../reg_compute/reg_sync/asc_mem_bar.md) to establish ordering according to the API constraints.
