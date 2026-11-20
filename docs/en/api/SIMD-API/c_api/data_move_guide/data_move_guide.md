---
okftype: reference.concept
description: "Data movement chapter guide: lists the eight types of data paths supported by 3510 and their corresponding chapters by source and destination storage units, and explains the selection principles for contiguous, high-dimensional slicing, and NDDMA prototypes, the differences in 32-byte alignment padding, pre-processing configuration for accompanying operations, and cross-pipeline synchronization requirements."
---
# Data Movement Guide

<!-- md-trans-meta sourceCommit=54f1d966cfcc22290ddc2c6312bbe4c6ac2a9232 translatedAt=2026-11-16T01:40:02.229381+00:00 pushedAt=2026-11-16T02:10:55.300Z nl=2 -->

This chapter describes how to select C API data movement APIs based on source and destination storage units, data layout, and on-the-fly processing requirements, using the data paths and API capabilities of NPU architecture version 3510 as an example. For other architectures, refer to the specific API documentation for product support and API capabilities. Developers need to explicitly manage addresses, movement parameters, configuration states, and pipeline synchronization. When using the APIs described in this chapter, unless otherwise specified in the API documentation, the header file `c_api/asc_simd.h` can be included.

## Reading Path

1. Read [Overview](overview/overview.md) to understand the memory hierarchy, API categories, and the division of work among PIPE_MTE1, PIPE_MTE2, PIPE_MTE3, PIPE_FIX, and PIPE_V.
2. Read [Data Movement Concepts](overview/data_move_concept.md) to distinguish movement forms such as contiguous data movement, high-dimensional slicing, multi-dimensional slicing, unaligned data movement, on-the-fly conversion, matrix fractal movement, and NDDMA.
3. Read [Data Path](overview/data_path.md) to check whether a direct path exists on NPU architecture version 3510 based on the source and destination storage units, and find the corresponding C API.
4. Use the following table to enter the specific path chapter and verify the parameter units, prerequisite configuration, and synchronization requirements for each mode.
5. Before coding, check [Overall Constraints](overview/overall_constraints.md), and refer to the applicable products, parameter values, and constraints in the specific API documentation.

## Select by Data Path

| Data Path | Main Capabilities Supported by NPU Architecture Version 3510 | Detailed Description |
| --- | --- | --- |
| GM and L1/L0 | GM to L1 contiguous/slicing data movement, two-dimensional fractal movement, ND2Nz, DN2Nz, and unaligned data movement; L0C to GM on-the-fly processing | [GM and L1/L0 data movement](gm_l1_or_l0_data_move.md) |
| GM and UB | Bidirectional contiguous/slicing data movement, bidirectional unaligned data movement, GM to UB five-dimensional NDDMA | [GM and UB data movement](gm_ub_data_move.md) |
| L1 and L0 | L1 to L0A/L0B two-dimensional, three-dimensional, transpose, and MX; L0C to L1 on-the-fly processing | [L1 and L0 data movement](l1_l0_data_move.md) |
| L1/L0C and UB | L1 to UB, UB to L1, L0C to UB on-the-fly processing, and dual-destination writes | [L1/L0C and UB data movement](l1_or_l0c_ub_data_move.md) |
| L1 to BiasTable | Contiguous or high-dimensional slicing bias movement, supporting specified type conversion | [Data movement from L1 to BiasTable](l1_bias_table_data_move.md) |
| L1 to Fixpipe Buffer | Contiguous or high-dimensional slicing loading of quantization and ReLU parameters | [L1 to Fixpipe Buffer data movement](l1_fixpipe_data_move.md) |
| UB and UB | Contiguous or high-dimensional slicing copy | [UB to UB data movement](ub_ub_data_move.md) |
| Reg and UB | Aligned/unaligned load, store, broadcast, interleave, compression, and Post Update | [Reg and UB data movement](reg_ub_data_move.md) |

## API Selection Principles

- First determine the physical path based on the source and destination storage units. NPU architecture version 3510 has no direct path from GM to L0A/L0B or from L1 to GM, so do not infer the API solely from address types.
- For a contiguous block of data, prefer the contiguous prototype. When multiple equal-sized data blocks have fixed intervals between them, choose the high-dimensional slicing prototype. When up to five-dimensional padding, transpose, broadcast, or slice is required, choose NDDMA.
- The length of normal GM and UB APIs is expressed in bytes and only needs to satisfy data type byte alignment. Do not necessarily switch to the `*_align` API just because the effective length is not a multiple of 32 bytes. However, GM-to-UB writes are padded at the UB destination, and UB-to-GM also reads the padded range from UB, so UB must still reserve space for the actual access range after 32-byte padding. When you need to explicitly control the padding layout, left/right padding, or L2 cache policy, select the corresponding `*_align` high-dimensional slicing prototype. For non-32-byte block lengths from GM to L1, use `asc_copy_gm2l1_align`.
- When you need to change the matrix layout, data type, or perform quantization activation during data movement, select a GM-to-L1 or L0C move-out API with on-the-fly processing capability, and complete the associated register configuration first.
- When the data producer and consumer are in different pipelines or on different execution cores, synchronize explicitly based on data dependencies. When multiple data movements write to overlapping destination regions, serialization must also be ensured.

> [!NOTE]
> It is recommended to call [asc_init](../utils/sys_init/asc_init.md) at the kernel entry before performing data movement and computation. The padding, loop, format conversion, quantization, and activation states set by configuration APIs may affect subsequent instructions, so reconfirm them according to the current task before reuse.
