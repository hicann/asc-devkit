---
okftype: reference.concept
description: "Entry overview of the data movement chapter. Describes the AI Core hierarchical storage and the data flow among GM, L1, L0, and UB. Provides the API directory and hardware pipeline division by vector, Cube, and Reg categories, and notes the synchronization requirements within the same pipeline and across pipelines."
---
# Overview

<!-- md-trans-meta sourceCommit=54f1d966cfcc22290ddc2c6312bbe4c6ac2a9232 translatedAt=2026-11-16T01:40:02.270324+00:00 pushedAt=2026-11-16T02:10:55.370Z nl=2 -->

Data movement is one of the most fundamental and critical operations in operator development. In the hierarchical storage architecture of the AI Core, input data must be moved from Global Memory (GM) into on-chip storage, and computation results must be moved from on-chip storage back to GM. When intermediate results are exchanged between Cube computation and vector computation, data flow among L1 Buffer, L0 Buffer, Unified Buffer (UB), and registers is also involved.

The C APIs for NPU architecture version 3510 directly describe the source and destination storage units and the movement form through address space qualifiers and function parameters. Developers need to select APIs based on the data path and explicitly configure addresses, lengths, strides, padding, on-the-fly conversion, and pipeline synchronization. This chapter establishes the following unified understanding before reading the detailed descriptions of each path:

- **Storage hierarchy and data paths:** Which physical storage units the AI Core contains, the directions in which data can flow, and which hardware pipeline executes each path.
- **Address spaces and execution scopes:** The physical storage corresponding to address spaces such as `__gm__`, `__cbuf__`, `__ca__`, `__cb__`, `__cc__`, and `__ubuf__`, as well as the effective scope of APIs in AIC, AIV, or Vector Function.
- **Movement function categories:** Capabilities such as contiguous data movement, high-dimensional slicing, multi-dimensional slicing, on-the-fly format conversion, on-the-fly quantization and activation, unaligned data movement, UB internal copy, matrix fractal movement, NDDMA, and Reg and UB data exchange.
- **Key parameter concepts:** The physical meanings and unit differences of `size`, `burst_count`, `burst_len`, `src_stride`, `dst_stride`, `src_gap`, `dst_gap`, and multi-dimensional loop parameters.
- **General constraints:** Address alignment, storage capacity, valid access range, API execution core, configuration state, and synchronization rules.

## API Classification

**Table 1** API categories

| Category | Primary Execution Scope | Typical Path | API Directory |
| --- | --- | --- | --- |
| Vector data movement | AIV | GM and UB, UB to L1, UB internal | [Vector data movement](../../vector_datamove/vector_datamove.md) |
| Cube data movement | AIC | GM to L1, L1 to L0A/L0B, L0C move-out, L1 to dedicated Buffer | [Cube computation data movement](../../cube_datamove/cube_datamove.md) |
| Reg load/store | Vector Function of AIV | Data exchange between UB and vector data registers or mask registers | [Reg load](../../reg_compute/load/reg_load_overview.md), [Reg store](../../reg_compute/store/reg_store_overview.md) |

## Hardware Pipeline

**Table 2** Hardware pipelines

| Pipeline | Primary Movement Directions in NPU Architecture Version 3510 |
| --- | --- |
| `PIPE_MTE1` | L1 to L0A/L0B, L1 to UB, L1 to BiasTable |
| `PIPE_MTE2` | GM to L1, GM to UB |
| `PIPE_MTE3` | UB to GM, UB to L1 |
| `PIPE_FIX` | L0C to GM/L1/UB, L1 to Fixpipe Buffer |
| `PIPE_V` | UB internal copy and vector computation |

Instructions in the same pipeline execute in order, but when a later instruction starts, the previous instruction may not have completed all data reads and writes. When overwrite or read/write dependencies exist, call [asc_sync_pipe](../../sync/intra_core_sync/asc_sync_pipe.md) to wait for the preceding instruction to complete. When the producer and consumer are in different pipelines, call the [synchronization control](../../sync/overview.md#synchronization-control-overview) APIs based on the data dependency. The pipeline to which an API belongs, the supported execution cores, and the synchronization method are subject to the specific API documentation.

## Further Reading

- [Data Movement Concepts](data_move_concept.md): Understand different data movement modes and their applicable scenarios.
- [Data Path](data_path.md): Find APIs by source and destination storage units and hardware pipelines.
- [Overall Constraints](overall_constraints.md): Check alignment, capacity, parameter units, and synchronization requirements before coding.
