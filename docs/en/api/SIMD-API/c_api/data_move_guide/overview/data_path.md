---
okftype: reference.concept
description: "Based on the AI Core hierarchical storage, compares the data path changes between 2201 and 3510, lists the data movement APIs available on 3510 by source, destination storage unit, and hardware pipeline, and provides a comparison of the address space and usage of each storage unit."
---
# Data Path

<!-- md-trans-meta sourceCommit=54f1d966cfcc22290ddc2c6312bbe4c6ac2a9232 translatedAt=2026-11-16T01:40:02.262322+00:00 pushedAt=2026-11-16T02:10:55.353Z nl=2 -->

## AI Core Hardware Architecture and Storage Units

The AI Core uses a hierarchical storage architecture. Compute units include Cube, Vector, and Scalar; storage units include Global Memory (GM), L1 Buffer, L0A/L0B/L0C Buffer, Unified Buffer (UB), BiasTable Buffer, Fixpipe Buffer, and SIMD Register File; MTE1, MTE2, MTE3, and Fixpipe are responsible for moving data between different storage units.

Figure 1 shows the storage and data movement paths of [NPU architecture version 2201](../../../../../guide/programming_guide/language_extension/simd_builtin_keywords.md#npu-arch), to help understand the basic relationships among the AI Core storage levels.

**Figure 1** AI Core hardware architecture under NPU architecture version 2201

![AI Core hardware architecture under NPU architecture version 2201](../../../../figures/atlas_a2_a3_architecture.png "AI Core hardware architecture under NPU architecture version 2201")

The internal structure of [NPU architecture version 3510](../../../../../guide/programming_guide/language_extension/simd_builtin_keywords.md#npu-arch) is shown in Figure 2. Compared with Figure 1, the data paths have the following main changes:

- A unidirectional data path from L0C Buffer to UB is added.
- A data path from UB to L1 Buffer is added.
- The direct path from GM to L0A/L0B is no longer provided; matrix inputs need to be moved to L1 Buffer first.
- The direct path from L1 Buffer to GM is no longer provided.
- AIV adds a SIMD Register File. Vector Function exchanges data with UB through Reg load and store APIs.

**Figure 2** AI Core hardware architecture and synchronization relationships under NPU architecture version 3510

![AI Core hardware architecture and synchronization relationships under NPU architecture version 3510](../../../../figures/npu_3510_hw_arch_sync.png "AI Core hardware architecture and synchronization relationships under NPU architecture version 3510")

The subsequent API selections in this guide are based on Ascend 950PR&950DT products (NPU architecture version 3510). For other products, refer to the product support table in the corresponding API documentation for data path and API support information.

### Storage Unit Purposes

| Storage Unit | C API Address Space or Object | Main Purpose |
| --- | --- | --- |
| GM | `__gm__` | Stores kernel inputs, outputs, and cross-core shared data. |
| L1 Buffer | `__cbuf__` | Caches Cube inputs and serves as the intermediate storage between GM and L0A/L0B. |
| L0A Buffer | `__ca__` | Stores the left matrix input for Cube computation. |
| L0B Buffer | `__cb__` | Stores the right matrix input for Cube computation. |
| L0C Buffer | `__cc__` | Stores the accumulation result of Cube computation. |
| UB | `__ubuf__` | Stores Vector inputs, outputs, and intermediate results, and serves as the on-chip storage for data exchange between AIC and AIV. |
| BiasTable Buffer | Passed as a `uint64_t` destination address | Stores the bias used by Mmad. |
| Fixpipe Buffer | `__fbuf__` | Stores the on-the-fly Vector quantization and on-the-fly ReLU parameters used when moving data out of L0C. |
| SIMD Register File | `vector_*`, `vector_bool`, and other [Register types](../../defs/type/reg_data_types.md) | Stores the vector data, mask, and address state used by Vector Function. |

## Data Path and Movement Pipeline

The following table lists the data movement functions available on NPU architecture version 3510 by source and destination storage unit. Each row corresponds to a directly selectable C API form; when the same API has multiple overloads, they are listed separately by function.

**Table 1** Data movement function overview for NPU architecture version 3510

| Source (SRC) | Destination (DST) | Pipeline | Function | C API |
| --- | --- | --- | --- | --- |
| GM | L1 Buffer | PIPE_MTE2 | Contiguous movement; configures one data block with source and destination strides set to 0. | [asc_copy_gm2l1](../../cube_datamove/cube_compute_load/asc_copy_gm2l1_highdim_split_arch_3510.md) |
| GM | L1 Buffer | PIPE_MTE2 | High-dimensional slicing data movement; describes the layout through the number of data blocks, block length, and source/destination strides. | [asc_copy_gm2l1](../../cube_datamove/cube_compute_load/asc_copy_gm2l1_highdim_split_arch_3510.md) |
| GM | L1 Buffer | PIPE_MTE2 | Performs two-dimensional matrix movement in 512-byte fractal units. | [asc_copy_gm2l1](../../cube_datamove/cube_compute_load/asc_copy_gm2l1_2d_arch_3510.md) |
| GM | L1 Buffer | PIPE_MTE2 | Converts ND data to Nz layout during movement. | [asc_copy_gm2l1_nd2nz](../../cube_datamove/cube_compute_load/asc_copy_gm2l1_nd2nz_arch_3510.md) |
| GM | L1 Buffer | PIPE_MTE2 | Converts DN data to Nz layout during movement. | [asc_copy_gm2l1_dn2nz](../../cube_datamove/cube_compute_load/asc_copy_gm2l1_dn2nz.md) |
| GM | L1 Buffer | PIPE_MTE2 | Unaligned movement, supporting Compact, Normal, left/right padding, and circular padding. | [asc_copy_gm2l1_align](../../cube_datamove/cube_compute_load/asc_copy_gm2l1_align.md) |
| L0C Buffer | GM | PIPE_FIX | Moves out matrix results, with optional combination of quantization, activation, Nz2ND/Nz2DN, and channel processing. | [asc_copy_l0c2gm](../../cube_datamove/cube_compute_store/asc_copy_l0c2gm_arch_3510.md) |
| GM | UB | PIPE_MTE2 | Contiguous movement | [asc_copy_gm2ub](../../vector_datamove/asc_copy_gm2ub/asc_copy_gm2ub_arch_3510.md) |
| GM | UB | PIPE_MTE2 | High-dimensional slicing data movement | [asc_copy_gm2ub](../../vector_datamove/asc_copy_gm2ub/asc_copy_gm2ub_arch_3510.md) |
| GM | UB | PIPE_MTE2 | Unaligned contiguous movement, with the destination padded to a 32-byte boundary. | [asc_copy_gm2ub_align](../../vector_datamove/asc_copy_gm2ub_align/asc_copy_gm2ub_align_arch_3510.md) |
| GM | UB | PIPE_MTE2 | Unaligned high-dimensional slicing data movement, supporting Compact, Normal, and left/right padding. | [asc_copy_gm2ub_align](../../vector_datamove/asc_copy_gm2ub_align/asc_copy_gm2ub_align_arch_3510.md) |
| GM | UB | PIPE_MTE2 | Padding or multi-dimensional data movement of up to five dimensions. | [asc_ndim_copy_gm2ub](../../vector_datamove/asc_ndim_copy_gm2ub.md) |
| GM | UB | PIPE_MTE2 | Implements Transpose, Broadcast, or Slice through multi-dimensional stride combinations. | [asc_ndim_copy_gm2ub](../../vector_datamove/asc_ndim_copy_gm2ub.md) |
| UB | GM | PIPE_MTE3 | Contiguous movement | [asc_copy_ub2gm](../../vector_datamove/asc_copy_ub2gm/asc_copy_ub2gm_arch_3510.md) |
| UB | GM | PIPE_MTE3 | High-dimensional slicing data movement | [asc_copy_ub2gm](../../vector_datamove/asc_copy_ub2gm/asc_copy_ub2gm_arch_3510.md) |
| UB | GM | PIPE_MTE3 | Unaligned contiguous movement, discarding padding data when writing to GM. | [asc_copy_ub2gm_align](../../vector_datamove/asc_copy_ub2gm_align/asc_copy_ub2gm_align_arch_3510.md) |
| UB | GM | PIPE_MTE3 | Unaligned high-dimensional slicing data movement, supporting Compact and Normal source layouts. | [asc_copy_ub2gm_align](../../vector_datamove/asc_copy_ub2gm_align/asc_copy_ub2gm_align_arch_3510.md) |
| UB | L1 Buffer | PIPE_MTE3 | Contiguous movement | [asc_copy_ub2l1](../../vector_datamove/asc_copy_ub2l1.md) |
| UB | L1 Buffer | PIPE_MTE3 | High-dimensional slicing data movement | [asc_copy_ub2l1](../../vector_datamove/asc_copy_ub2l1.md) |
| L1 Buffer | UB | PIPE_MTE1 | Contiguous movement, with a configurable destination AIV. | [asc_copy_l12ub](../../cube_datamove/cube_compute_store/asc_copy_l12ub.md) |
| L1 Buffer | UB | PIPE_MTE1 | High-dimensional slicing data movement, with a configurable destination AIV. | [asc_copy_l12ub](../../cube_datamove/cube_compute_store/asc_copy_l12ub.md) |
| L1 Buffer | L0A Buffer | PIPE_MTE1 | Two-dimensional fractal matrix movement | [asc_copy_l12l0a](../../cube_datamove/cube_compute_load/asc_copy_l12l0a_2d_arch_3510.md) |
| L1 Buffer | L0A Buffer | PIPE_MTE1 | Two-dimensional fractal matrix movement with transpose | [asc_copy_l12l0a_transpose](../../cube_datamove/cube_compute_load/asc_copy_l12l0a_2d_arch_3510.md) |
| L1 Buffer | L0A Buffer | PIPE_MTE1 | Three-dimensional img2col movement, supporting Feature Map, Filter, Padding, and Repeat configurations. | [asc_copy_l12l0a](../../cube_datamove/cube_compute_load/asc_copy_l12l0a_3d_arch_3510.md) |
| L1 Buffer | L0A_MX Buffer | PIPE_MTE1 | Moves the left matrix quantization coefficients used by MX Cube computation. | [asc_copy_l12l0a_mx](../../cube_datamove/cube_compute_load/asc_copy_l12l0a_mx.md) |
| L1 Buffer | L0B Buffer | PIPE_MTE1 | Two-dimensional fractal matrix movement | [asc_copy_l12l0b](../../cube_datamove/cube_compute_load/asc_copy_l12l0b_2d_arch_3510.md) |
| L1 Buffer | L0B Buffer | PIPE_MTE1 | Two-dimensional fractal matrix movement with transpose | [asc_copy_l12l0b_transpose](../../cube_datamove/cube_compute_load/asc_copy_l12l0b_2d_arch_3510.md) |
| L1 Buffer | L0B Buffer | PIPE_MTE1 | Three-dimensional img2col movement, with automatic transpose during movement. | [asc_copy_l12l0b](../../cube_datamove/cube_compute_load/asc_copy_l12l0b_3d_arch_3510.md) |
| L1 Buffer | L0B Buffer | PIPE_MTE1 | Performs two-dimensional fractal transpose by repeat and fractal interval. | [asc_copy_l12l0b_trans](../../cube_datamove/cube_compute_load/asc_copy_l12l0b_trans_arch_3510.md) |
| L1 Buffer | L0B_MX Buffer | PIPE_MTE1 | Moves the right matrix quantization coefficients used by MX Cube computation. | [asc_copy_l12l0b_mx](../../cube_datamove/cube_compute_load/asc_copy_l12l0b_mx.md) |
| L0C Buffer | L1 Buffer | PIPE_FIX | Moves out matrix results, with optional quantization, activation, and format conversion. | [asc_copy_l0c2l1](../../cube_datamove/cube_compute_store/asc_copy_l0c2l1_arch_3510.md) |
| L0C Buffer | UB | PIPE_FIX | Moves out matrix results, with optional combination of quantization, activation, format conversion, and dual-destination movement. | [asc_copy_l0c2ub](../../cube_datamove/cube_compute_store/asc_copy_l0c2ub.md) |
| L1 Buffer | BiasTable Buffer | PIPE_MTE1 | Contiguous bias movement, with optional processing from `half`/`bfloat16_t` to 32-bit data. | [asc_copy_l12bt](../../cube_datamove/cube_compute_load/asc_copy_l12bt_arch_3510.md) |
| L1 Buffer | BiasTable Buffer | PIPE_MTE1 | High-dimensional slicing bias movement | [asc_copy_l12bt](../../cube_datamove/cube_compute_load/asc_copy_l12bt_arch_3510.md) |
| L1 Buffer | Fixpipe Buffer | PIPE_FIX | Contiguous movement of quantization or ReLU parameters | [asc_copy_l12fb](../../cube_datamove/cube_compute_load/asc_copy_l12fb_arch_3510.md) |
| L1 Buffer | Fixpipe Buffer | PIPE_FIX | High-dimensional slicing data movement of quantization or ReLU parameters | [asc_copy_l12fb](../../cube_datamove/cube_compute_load/asc_copy_l12fb_arch_3510.md) |
| UB | UB | PIPE_V | Contiguous copy | [asc_copy_ub2ub](../../vector_datamove/asc_copy_ub2ub.md) |
| UB | UB | PIPE_V | High-dimensional slicing copy | [asc_copy_ub2ub](../../vector_datamove/asc_copy_ub2ub.md) |
| UB | SIMD Register File | Vector Function | Aligned, unaligned, broadcast, or masked load | [Reg data load](../../reg_compute/load/reg_load_overview.md) |
| SIMD Register File | UB | Vector Function | Aligned, unaligned, or masked store | [Reg data store](../../reg_compute/store/reg_store_overview.md) |

NPU architecture version 3510 does not support direct GM-to-L0A/L0B data movement. Data must pass through two data path segments: GM to L1 Buffer, then L1 Buffer to L0. It also does not support general direct data movement from L1 Buffer to GM or from UB to L0C. When API names are the same across multiple NPU architectures, this does not imply that NPU architecture version 3510 supports the API. Refer to the applicable products section in the API documentation.

For the modes, parameter units, and key constraints of each path, see the corresponding sections in [Data Movement Guide](../data_move_guide.md). For a complete description of the hardware storage structure, see [NPU Architecture Version 3510 Hardware Specifications](../../../../../guide/programming_guide/advanced_programming/hardware_implementation/architecture_spec/npu_arch_3510.md).
