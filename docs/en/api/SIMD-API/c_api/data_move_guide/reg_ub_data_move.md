---
okftype: reference.concept
description: "Describes the selection of load and store APIs between UB and vector, mask, and unaligned registers on 3510: categorized by contiguous, broadcast, interleave, compression, sampling, and Post Update modes. Provides the capacity prerequisites of VL being 256 bytes and mask being 32 bytes, and the requirement of reusing unaligned states."
---
# Reg and UB Data Movement

<!-- npu="950" id1 -->
<!-- md-trans-meta sourceCommit=54f1d966cfcc22290ddc2c6312bbe4c6ac2a9232 translatedAt=2026-11-16T01:40:02.273842+00:00 pushedAt=2026-11-16T02:10:55.377Z nl=1 -->

Reg data load APIs move data from the Unified Buffer (UB) into vector data registers, mask registers, or unaligned registers. Reg data store APIs write data from registers back to the UB. Both types of APIs take effect only on AIV and must be called within a Vector Function (a function marked with `__simd_vf__`).

For NPU architecture version 3510, the vector data register length (VL) is 256 bytes and the mask register length is 32 bytes. The number of elements that a single vector data register can hold is determined by the data bit width; for example, b16 holds 128 elements and b32 holds 64 elements. For the complete API modes, address alignment requirements, diagrams, and examples, see [Reg Data Load Overview](../reg_compute/load/reg_load_overview.md) and [Reg Data Store Overview](../reg_compute/store/reg_store_overview.md). This section summarizes API selection methods by usage scenario.

## API Selection

### UB to Reg

| API Mode | Representative API | Selection Description |
| --- | --- | --- |
| Contiguous/immediate/address register offset load | [asc_loadalign](../reg_compute/load/asc_loadalign.md) | Loads one VL. The caller can calculate the address, or use an immediate offset or address register offset. |
| Immediate offset/non-contiguous aligned Post Update load | [asc_loadalign_postupdate](../reg_compute/load/asc_loadalign_postupdate.md) | Automatically updates the source address after loading, suitable for continuous iteration. No address register offset prototype is provided. |
| Non-contiguous aligned load | [asc_loadalign](../reg_compute/load/asc_loadalign.md), [asc_loadalign_datablock_strided](../reg_compute/load/asc_loadalign_datablock_strided.md) | Loads 8 DataBlocks non-contiguously according to the configured DataBlock stride. |
| Broadcast load | [asc_loadalign_brc_datablock](../reg_compute/load/asc_loadalign_brc_datablock.md), [asc_loadalign_brc_elem](../reg_compute/load/asc_loadalign_brc_elem.md), [asc_loadalign_brc_elem2datablock](../reg_compute/load/asc_loadalign_brc_elem2datablock.md) | Supports DataBlock broadcast, single-element broadcast, and broadcasting 8 elements to 8 DataBlocks respectively. |
| Deinterleave/sample/unpack load | [asc_loadalign_deintlv](../reg_compute/load/asc_loadalign_deintlv.md), [asc_loadalign_downsample](../reg_compute/load/asc_loadalign_downsample.md), [asc_loadalign_upsample](../reg_compute/load/asc_loadalign_upsample.md), [asc_loadalign_unpack](../reg_compute/load/asc_loadalign_unpack.md), [asc_loadalign_unpack4](../reg_compute/load/asc_loadalign_unpack4.md) | Supports deinterleaving, 2x downsampling, 2x upsampling, half unpack, and quarter unpack. |
| Contiguous unaligned load (easy-to-use form) | [asc_load](../reg_compute/load/asc_load.md) | The source address only needs to be aligned to `dtype`; the API completes a contiguous load of one VL. |
| Contiguous unaligned load (high-performance form) | [asc_loadunalign_pre](../reg_compute/load/asc_loadunalign_pre.md), [asc_loadunalign](../reg_compute/load/asc_loadunalign.md), [asc_loadunalign_postupdate](../reg_compute/load/asc_loadunalign_postupdate.md) | Uses `vector_load_unalign` to save data crossing 32-byte boundaries, suitable for scenarios that require explicit reuse of the unaligned state. |
| Contiguous aligned load to the mask register | [asc_loadalign](../reg_compute/load/asc_loadalign.md), [asc_loadalign_postupdate](../reg_compute/load/asc_loadalign_postupdate.md), [asc_loadalign_mask](../reg_compute/load/asc_loadalign_mask.md) | Supports direct load, Post Update load, or load via return value. |
| Sampled load to the mask register | [asc_loadalign_downsample](../reg_compute/load/asc_loadalign_downsample.md), [asc_loadalign_downsample_postupdate](../reg_compute/load/asc_loadalign_downsample_postupdate.md), [asc_loadalign_mask_downsample](../reg_compute/load/asc_loadalign_mask_downsample.md), [asc_loadalign_upsample](../reg_compute/load/asc_loadalign_upsample.md), [asc_loadalign_upsample_postupdate](../reg_compute/load/asc_loadalign_upsample_postupdate.md), [asc_loadalign_mask_upsample](../reg_compute/load/asc_loadalign_mask_upsample.md) | Supports 2x downsampling and 2x upsampling, with optional Post Update or return value form. |

### Reg Store to UB

| API Mode | Representative API | Selection Description |
| --- | --- | --- |
| Contiguous/immediate/address register offset/non-contiguous aligned store | [asc_storealign](../reg_compute/store/asc_storealign.md) | Writes one VL contiguously according to `mask`, or writes by DataBlock stride. |
| Immediate offset/non-contiguous aligned Post Update store | [asc_storealign_postupdate](../reg_compute/store/asc_storealign_postupdate.md) | Automatically updates the destination address after storing, suitable for continuous iteration; no address register offset prototype is provided. |
| First-element store | [asc_storealign_1st](../reg_compute/store/asc_storealign_1st.md), [asc_storealign_1st_postupdate](../reg_compute/store/asc_storealign_1st_postupdate.md) | Writes only the first element of the vector data register to UB, with optional automatic update of the destination address. |
| Interleaved store | [asc_storealign_intlv](../reg_compute/store/asc_storealign_intlv.md) | Writes two vector data registers to UB after interleaving them element by element. |
| Compressed store | [asc_storealign_pack](../reg_compute/store/asc_storealign_pack.md), [asc_storealign_pack_quarter](../reg_compute/store/asc_storealign_pack_quarter.md) | Writes the lower half bits of valid elements or the lower 8 bits of 32-bit elements to UB after compression. |
| Contiguous store (easy-to-use form) | [asc_store](../reg_compute/store/asc_store.md) | Can store the entire vector data register or the first `count` elements; for specific alignment scenarios, select `asc_storealign` or the unaligned API according to the API documentation. |
| Unaligned store | [asc_storeunalign](../reg_compute/store/asc_storeunalign.md), [asc_storeunalign_postupdate](../reg_compute/store/asc_storeunalign_postupdate.md) | Uses `vector_store_unalign` to temporarily store the tail block that cannot be written out immediately, and calls the matching post-processing API after the contiguous store ends. |
| Contiguous aligned store from the mask register | [asc_storealign](../reg_compute/store/asc_storealign.md), [asc_storealign_postupdate](../reg_compute/store/asc_storealign_postupdate.md) | Supports direct store and Post Update store. |
| Compressed store from the mask register | [asc_storealign_pack](../reg_compute/store/asc_storealign_pack.md), [asc_storealign_pack_postupdate](../reg_compute/store/asc_storealign_pack_postupdate.md) | Discards every other bit and writes the retained bits to UB after compression. |
| Unaligned store from the mask register | [asc_storeunalign_postupdate](../reg_compute/store/asc_storeunalign_postupdate.md) | Extracts valid bits from the mask register and writes them to UB in packed form, then calls the matching post-processing API. |

## Address Maintenance Methods

For aligned contiguous load and store, the following address maintenance methods can be selected according to the specific API. The selection principle is consistent with the comparison tables in the two existing overviews; not every API supports all methods simultaneously:

- The caller calculates the address for each access through pointer arithmetic, suitable for scenarios with simple offset rules and requiring explicit address control.
- Passes an immediate offset relative to the base address to the API, suitable for scenarios where the base address needs to be retained and the offset can be represented by an ordinary integer.
- Generates `addr_reg` through [asc_update_addr_reg](../reg_compute/reg_addr_reg/asc_update_addr_reg.md), suitable for scenarios where the offset changes with iteration inside a Hardware Loop.
- Use the Post Update API to automatically update the address after completing a load or store.

`asc_loadalign_postupdate` and `asc_storealign_postupdate` support specifying the address update amount only through immediate parameters, along with their respective non-contiguous aligned prototypes, and do not provide the `addr_reg` parameter. For address register offsets, use non-Post Update APIs such as `asc_loadalign` or `asc_storealign`. For unaligned APIs, refer to the specific prototypes for support.

The offset unit varies by API and register type. Vector data offsets are usually in elements, mask address offsets may be in bytes, and non-contiguous modes may also use DataBlock as the unit. Refer to the API parameter descriptions for details.

## Unaligned Data Movement

Unaligned data movement is used for contiguous data access scenarios where the effective start address satisfies `dtype` alignment but not 32-byte alignment. For detailed principles and coordination, see [Unaligned Data Movement in Reg Data Load Overview](../reg_compute/load/reg_load_overview.md#unaligned-movement-features).

### Unaligned Load

`asc_load` is an easy-to-use API for contiguous unaligned loads. When explicit reuse of the unaligned state is required, call `asc_loadunalign_pre` first, then call `asc_loadunalign` or `asc_loadunalign_postupdate`. A regular `asc_loadunalign` requires preprocessing again before each load. When consecutively calling the immediate-offset form of `asc_loadunalign_postupdate`, the unaligned register updated by the API can be reused; if the source address is modified separately, preprocessing must be performed again.

An unaligned load accesses the range aligned downward to 32 bytes from the effective start address, so this range must also be within the allocated UB space.

### Unaligned Store

The main unaligned store API writes complete blocks to the UB and temporarily stores the trailing data that does not fill an aligned block in `vector_store_unalign`. Consecutive calls must reuse the same unaligned register. After completion, call the matching post-processing API according to the following table to write out the last tail block.

| Main Store Method | Matching Post-Processing API | Address Maintenance Methods |
| --- | --- | --- |
| [asc_storeunalign](../reg_compute/store/asc_storeunalign.md) | [asc_storeunalign_post](../reg_compute/store/asc_storeunalign_post.md) | The caller updates the destination address. |
| [asc_storeunalign_postupdate](../reg_compute/store/asc_storeunalign_postupdate.md) immediate-offset mode | [asc_storeunalign_post_postupdate](../reg_compute/store/asc_storeunalign_post_postupdate.md) | The main API automatically updates the destination address. |
| [asc_storeunalign_postupdate](../reg_compute/store/asc_storeunalign_postupdate.md) address register offset mode | [asc_storeunalign_post](../reg_compute/store/asc_storeunalign_post.md) | The main API automatically updates the address register, and both the main store and post-processing APIs reuse this register. |

## Constraints

- The actual access address in each mode must meet the alignment requirements of the corresponding API, and the actual access range must be within the available UB space without exceeding the boundary.
- When multiple Reg instructions access overlapping UB regions with read/write conflicts, use [asc_mem_bar](../reg_compute/reg_sync/asc_mem_bar.md) to ensure execution order.
- The b64 data type is supported only by some APIs, and the address register offset and mask construction methods differ. For details, see [b64 Data Type Movement in Reg Data Load Overview](../reg_compute/load/reg_load_overview.md#movement-of-b64-data-type).
- For register types and declaration methods, see [Register Types](../defs/type/reg_data_types.md).

<!-- end id1 -->
