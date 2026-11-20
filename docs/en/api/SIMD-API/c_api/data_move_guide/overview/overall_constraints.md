---
okftype: reference.concept
description: "Summarizes the address alignment, capacity, parameter units, execution scope, and synchronization rules shared by data movement APIs. Provides the architectural baseline alignment and capacity limits of each storage unit on 3510 in a table, and describes the key points for checking address overlap and configuration status."
---
# Overall Constraints

<!-- md-trans-meta sourceCommit=54f1d966cfcc22290ddc2c6312bbe4c6ac2a9232 translatedAt=2026-11-16T01:40:02.267323+00:00 pushedAt=2026-11-16T02:10:55.363Z nl=2 -->

Data movement APIs share common rules for address alignment, storage capacity, parameter units, and synchronization. Complete the checks in this section before calling an API. If a specific API document imposes stricter requirements, follow the specific API document.

## Address Alignment Constraints

The architectural baseline alignment of each storage unit in NPU architecture version 3510 is shown in the following table.

**Table 1** Address alignment requirements of storage units

| Storage Unit | Address Space or Object | Architectural Baseline Alignment | Description |
| --- | --- | --- | --- |
| Global Memory | `__gm__` | 1-byte | The API may still require 2-byte or 4-byte alignment based on the data type. |
| UB | `__ubuf__` | 32-byte | Ordinary DMA data movement usually requires 32-byte alignment. NDDMA and some Reg APIs have different requirements. |
| L1 Buffer | `__cbuf__` | 32-byte | Same as the DataBlock size. |
| L0A/L0B Buffer | `__ca__`/`__cb__` | 512-byte | Two-dimensional and three-dimensional matrix data movement usually uses a complete fractal as the granularity. |
| L0C Buffer | `__cc__` | 64-byte | Source address requirement of the L0C store API. |
| BiasTable Buffer | `uint64_t` destination address | 64-byte | Capacity limit is 4 KB. |
| Fixpipe Buffer quantize parameter space | `__fbuf__` | 128-byte | Capacity limit is 4 KB. |
| Fixpipe Buffer ReLU parameter space | `__fbuf__` | 64-byte | Capacity limit is 2 KB. |

The architectural baseline is not the final constraint for all prototypes. For example, the destination UB address of NDDMA allows 1-byte alignment, and some element or mask modes of Reg load and store can be aligned to data type, 8 bytes, or 16 bytes. When an address does not meet the requirements of an ordinary API, use the corresponding unaligned API instead. Do not read or write unallocated space by rounding the address upward or expanding the access range.

## Parameter Units

Data movement parameters commonly use the following units:

- `size` is usually in bytes, but APIs such as `asc_copy_ub2ub`, `asc_copy_ub2l1`, `asc_copy_l12bt`, and `asc_copy_l12fb` process data in 32-byte granularity, and unaligned portions may be truncated downward.
- `burst_len` and `len_burst` may be in bytes, 32-byte DataBlocks, 64-byte units, or 512-byte fractals.
- `src_stride` and `dst_stride` may represent the distance between the **start addresses** of adjacent data blocks; `src_gap` and `dst_gap` usually represent the interval from the **end address of the previous data block to the start address of the next data block**.
- In L1 to L0A/L0B two-dimensional data movement, the row/column start positions and strides may be in units of 16 elements, 32 bytes, or 512-byte fractals.
- For NDDMA, the loop size of each dimension, source stride, destination stride, and padding amount are in units of elements.
- The offset of Reg APIs may be in elements, bytes, or DataBlocks.

Parameters with the same name may have different units in different APIs. When calculating source and destination address spans, first convert all parameters to bytes according to the API documentation, and then check alignment and boundaries.

## Data Movement Amount and Storage Boundaries

The main on-chip storage capacities in [NPU architecture version 3510 hardware specifications](../../../../../guide/programming_guide/advanced_programming/hardware_implementation/architecture_spec/npu_arch_3510.md) are as follows:

| Storage Unit | Capacity Limit |
| --- | --- |
| UB | 256 KB; the actual available space is further reduced by the SIMD VF stack, reserved space, and the Data Cache when SIMD+SIMT are mixed. |
| L1 Buffer | 512KB |
| L0A/L0B Buffer | 64 KB each |
| L0C Buffer | 256KB |
| L0A_MX/L0B_MX Buffer | 4 KB each |
| BiasTable Buffer | 4KB |
| Fixpipe Buffer | 4 KB for quantization parameter space and 2 KB for ReLU parameter space |

The source and destination start addresses plus the actual access span must not exceed the allocated range. In high-dimensional slicing mode, the start offset of the last data block must be added to the block length. For non-32-byte effective lengths between GM and UB, the 32-byte padded access on the UB side must be included, even when using ordinary APIs. Left and right padding must also be included in the actual span. For on-the-fly format or type conversion, the space must be recalculated based on the destination data type and destination layout. For Cube data movement, padding elements in boundary fractals must also be included.

## Execution Scope

- Cube data movement APIs usually take effect only on AIC, and vector data movement APIs usually take effect only on AIV. When called on an unsupported execution core, some APIs return directly without performing the data movement.
- Reg load and store APIs are used only in Vector Functions (functions marked with `__simd_vf__`) on AIV.
- L1 to UB APIs are issued on AIC and select the destination AIV through `sub_blockid`. When data is shared between the AIC and AIV, the corresponding inter-core synchronization requirements must also be met.
- When distinguishing AIC and AIV code paths through macros, ensure that APIs are executed only in their supported branches.

## Multi-Instruction Synchronization

Data movement instructions are issued by the Scalar and then executed asynchronously on the corresponding hardware pipeline. The completion order between different pipelines cannot be inferred solely from the source code order. Synchronization is divided into the following two categories:

- **Same-pipeline synchronization:** When the destination addresses of multiple PIPE_MTE2 or PIPE_MTE3 instructions overlap, use [asc_sync_pipe](../../sync/intra_core_sync/asc_sync_pipe.md) to wait for the preceding data movement to complete before issuing a subsequent data movement that overwrites the same area.
- **Cross-pipeline synchronization:** When the producer and consumer are on different pipelines, use [asc_sync_notify](../../sync/intra_core_sync/asc_sync_notify.md) and [asc_sync_wait](../../sync/intra_core_sync/asc_sync_wait.md) to establish event dependencies, or select the corresponding easy-to-use synchronization API according to the API documentation.

The left side of Figure 1 shows overlapping destination GM ranges for two UB-to-GM data movements, and the right side shows overlapping destination UB ranges for two GM-to-UB data movements. In both scenarios, wait for the preceding instruction to complete within the same data movement pipeline.

**Figure 1** Overlapping data movement destination addresses

![Overlapping data movement destination addresses](../../../../figures/datacopy_address_overlap_sync_diagram_c_api.png "Overlapping data movement destination addresses")

The following code shows the synchronization method when the destination areas of two GM-to-UB data movements on PIPE_MTE2 overlap:

```c
asc_copy_gm2ub(ub0, gm0, size0);
asc_sync_pipe(PIPE_MTE2);
asc_copy_gm2ub(ub1, gm1, size1);
```

If the result of a GM-to-UB data movement is subsequently read by PIPE_V, a dependency from PIPE_MTE2 to PIPE_V must be established:

```c
asc_copy_gm2ub(ub0, gm0, size0);
asc_sync_notify(PIPE_MTE2, PIPE_V, EVENT_ID0);
asc_sync_wait(PIPE_MTE2, PIPE_V, EVENT_ID0);
// Call the PIPE_V API that reads ub0 here.
```

The event ID is shared by the same pair of source and destination pipelines. The pipeline and event parameters of `asc_sync_notify` and `asc_sync_wait` must be exactly the same. For available pipeline combinations and event resources, see [Intra-core Synchronization](../../sync/overview.md#intra-core-synchronization-principles).

## Address Overlap

Whether the valid source and destination movement regions may overlap is determined by the specific API. For UB internal copy on NPU architecture version 3510, the source and destination DataBlocks that actually participate in the movement must not overlap each other. Even if the overall spans of the two address ranges can overlap, `asc_copy_ub2ub` cannot be used as a `memmove` that supports overlapping regions. When Reg instructions access overlapping UB regions and read/write conflicts exist, use [asc_mem_bar](../../reg_compute/reg_sync/asc_mem_bar.md) as required by the corresponding API.

## Initialization and Configuration State

- At the kernel entry, call [asc_init](../../utils/sys_init/asc_init.md) first to clear control state that may have been retained by preceding tasks.
- Configuration APIs such as padding, loops, Nz format conversion, quantization, activation, and matrix three-dimensional movement affect subsequent movement instructions. Before reusing a configuration, confirm that all related fields have been updated for the current task.
- When multiple cores may read the same GM and the data may be updated by other cores, refresh the NDDMA DataCache before NDDMA as required by [asc_ndim_copy_dci](../../vector_datamove/asc_ndim_copy_dci.md).

For storage space and general C API rules, see [General Description and Constraints](../../general_description_and_constraints.md).
