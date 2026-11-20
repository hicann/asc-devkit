---
okftype: reference.concept
description: "Reg data store overview page: summarizes APIs that write vector data registers, mask registers, and unaligned registers to the Unified Buffer, categorizes them by aligned and unaligned, immediate offset, address register offset, and Post Update modes, and lists their respective alignment requirements. The APIs are used within Vector Function and take effect only on AI Vector."
---
# Overview

<!-- md-trans-meta sourceCommit=54f1d966cfcc22290ddc2c6312bbe4c6ac2a9232 translatedAt=2026-11-16T01:40:03.393758+00:00 pushedAt=2026-11-16T02:10:57.338Z nl=2 -->

## Reg Data Store Overview

Reg data store APIs are used to write data from vector data registers, mask registers, or unaligned registers into the Unified Buffer (UB). These APIs are used within Vector Functions (functions marked with `__simd_vf__`) and take effect only on AIV.

### Key Feature Description

#### Comparison of Aligned Contiguous Store Methods

The following presents four address offset modes, each illustrated by an example that adds two inputs of length 1024 and data type `half` element-wise and writes the results contiguously to the UB. A single iteration processes 128 elements, with 8 iterations in total, and the final result is `dst[i] = src0[i] + src1[i]`.

**Table 1** Comparison of aligned contiguous store methods

| Scenario | Store Mode | Destination Address Maintenance Mode | Applicable Scenario |
| --- | --- | --- | --- |
| Scenario 1 | User manual offset address store | The caller calculates the destination address for each store through pointer arithmetic. | The destination address offset rule is simple, and the address needs to be explicitly controlled by the user. |
| Scenario 2 | Store through API immediate offset | Passes the fixed element offset relative to the destination base address as an `int32_t` parameter. | The offset is determined at compile time, and the destination address pointer does not need to be modified. |
| Scenario 3 | Address register offset store | Generates `addr_reg` through [asc_update_addr_reg](../reg_addr_reg/asc_update_addr_reg.md), and the hardware updates the offset during iterations. | The offset within a Hardware Loop changes with iterations. |
| Scenario 4 | Post Update store | `asc_storealign_postupdate` automatically updates the destination address pointer after each store. | Contiguous store without requiring the user to manually maintain the destination address. |

##### Store with User-Manual Offset Address

Calls the contiguous aligned store mode of `asc_storealign`. The destination address is calculated as `dst + i * one_repeat_size`, where `one_repeat_size` is 128 `half` elements.

```cpp
vector_half src0_reg;
vector_half src1_reg;
vector_half dst_reg;
vector_bool mask = asc_create_mask_b16(PAT_ALL);
for (uint16_t i = 0; i < repeat_times; ++i) {
    asc_loadalign(src0_reg, src0 + i * one_repeat_size);
    asc_loadalign(src1_reg, src1 + i * one_repeat_size);
    asc_add(dst_reg, src0_reg, src1_reg, mask);
    asc_storealign(dst + i * one_repeat_size, dst_reg, mask);
}
```

##### Store with API Immediate Offset

Calls the immediate offset store mode of `asc_storealign`. `offset` is relative to the `dst` base address, in elements. This example calculates the offset for 8 stores using `i * 128`, covering 1024 `half` elements.

```cpp
vector_half src0_reg;
vector_half src1_reg;
vector_half dst_reg;
vector_bool mask = asc_create_mask_b16(PAT_ALL);
constexpr uint16_t REPEAT_TIMES = 8;
for (uint16_t i = 0; i < REPEAT_TIMES; ++i) {
    asc_loadalign(src0_reg, src0 + i * one_repeat_size);
    asc_loadalign(src1_reg, src1 + i * one_repeat_size);
    asc_add(dst_reg, src0_reg, src1_reg, mask);
    asc_storealign(dst, dst_reg, i * 128, mask);
}
```

##### Store with Address Register Offset

Calls the address register offset store mode of `asc_storealign`. `addr_reg` must be assigned and used in the innermost loop, with the offset in elements.

```cpp
vector_half src0_reg;
vector_half src1_reg;
vector_half dst_reg;
vector_bool mask;
addr_reg offset;
for (uint16_t i = 0; i < repeat_times; ++i) {
    offset = asc_update_addr_reg_b16(one_repeat_size);
    mask = asc_update_mask_b16(output_length);
    asc_loadalign(src0_reg, src0, offset);
    asc_loadalign(src1_reg, src1, offset);
    asc_add(dst_reg, src0_reg, src1_reg, mask);
    asc_storealign(dst, dst_reg, offset, mask);
}
```

##### Post Update Store

Calls the immediate offset store mode of `asc_storealign_postupdate`. The destination address is passed by reference, and the API updates it by `one_repeat_size` elements after each store.

```cpp
vector_half src0_reg;
vector_half src1_reg;
vector_half dst_reg;
vector_bool mask;
for (uint16_t i = 0; i < repeat_times; ++i) {
    mask = asc_update_mask_b16(output_length);
    asc_loadalign_postupdate(src0_reg, src0, one_repeat_size);
    asc_loadalign_postupdate(src1_reg, src1, one_repeat_size);
    asc_add(dst_reg, src0_reg, src1_reg, mask);
    asc_storealign_postupdate(dst, dst_reg, one_repeat_size, mask);
}
```

## API List

### Vector Data Register Store

#### Aligned Store

The destination address alignment requirement of aligned store APIs varies by API, and not all require 32-byte alignment. When using APIs with an offset, the actual access address after applying the offset must also meet the alignment requirement in the table.

**Table 2** Aligned store APIs for vector data registers

| API Name | Mode | API Description | Destination Address Alignment Requirement | Finest-Grained Common Header File |
| --- | --- | --- | --- | --- |
| [asc_storealign](asc_storealign.md) | Contiguous aligned store | Writes data of one VL (256 bytes) of a vector data register contiguously to the Unified Buffer according to `mask`. | 32-byte aligned. | `c_api/reg_compute/store/storealign.h` |
| [asc_storealign](asc_storealign.md) | Immediate offset store | Writes data of one VL to `dst + offset` according to `mask`, where `offset` is in elements. | 32-byte aligned. | `c_api/reg_compute/store/storealign.h` |
| [asc_storealign](asc_storealign.md) | Address register offset store | Writes data of one VL to the offset address specified by `addr_reg` according to `mask`. | 32-byte aligned. | `c_api/reg_compute/store/storealign.h` |
| [asc_storealign](asc_storealign.md) | Non-contiguous aligned store | Configures the write positions of eight DataBlocks according to `block_stride` and `repeat_stride`. | 32-byte aligned. | `c_api/reg_compute/store/storealign.h` |
| [asc_storealign_postupdate](asc_storealign_postupdate.md) | Immediate offset Post Update store | Contiguously stores one VL according to `mask` and automatically updates the destination address pointer by element offset. | 32-byte aligned. | `c_api/reg_compute/store/storealign.h` |
| [asc_storealign_postupdate](asc_storealign_postupdate.md) | Non-contiguous aligned Post Update store | Configures the write positions of eight DataBlocks according to strides and automatically updates the destination address pointer in 32-byte units. | 32-byte aligned. | `c_api/reg_compute/store/storealign.h` |
| [asc_storealign_1st](asc_storealign_1st.md) | First element store | Writes the first element of a vector data register to the Unified Buffer. | `dtype` aligned. | `c_api/reg_compute/store/storealign.h` |
| [asc_storealign_1st](asc_storealign_1st.md) | Store first element with immediate offset | Writes the first element to `dst + offset`, where `offset` is in elements. | `dtype` aligned. | `c_api/reg_compute/store/storealign.h` |
| [asc_storealign_1st](asc_storealign_1st.md) | Store first element with address register offset | Writes the first element to the offset address specified by `addr_reg`. | `dtype` aligned. | `c_api/reg_compute/store/storealign.h` |
| [asc_storealign_1st_postupdate](asc_storealign_1st_postupdate.md) | Store first element with Post Update | After storing the first element, automatically updates the destination address pointer by the element offset. | `dtype` aligned. | `c_api/reg_compute/store/storealign.h` |
| [asc_storealign_intlv](asc_storealign_intlv.md) | Interleaved contiguous store | Interleaves two vector data registers by element and stores them, with a single store amount of `2 x VL` (512 bytes). | 32-byte aligned. | `c_api/reg_compute/store/storealign.h` |
| [asc_storealign_intlv](asc_storealign_intlv.md) | Interleaved store with immediate offset | Interleaves two registers and writes them to `dst + offset`, where `offset` is in elements. | 32-byte aligned. | `c_api/reg_compute/store/storealign.h` |
| [asc_storealign_intlv](asc_storealign_intlv.md) | Interleaved store with address register offset | Interleaves two registers and writes them to the offset address specified by `addr_reg`. | 32-byte aligned. | `c_api/reg_compute/store/storealign.h` |
| [asc_storealign_pack](asc_storealign_pack.md) | Packed contiguous store | Compresses the low half bits of valid elements according to `mask` and writes them to the Unified Buffer. | 32-byte aligned. | `c_api/reg_compute/store/storealign.h` |
| [asc_storealign_pack](asc_storealign_pack.md) | Packed store with immediate offset | Writes the packed result to `dst + offset`, where `offset` is in elements. | 32-byte aligned. | `c_api/reg_compute/store/storealign.h` |
| [asc_storealign_pack](asc_storealign_pack.md) | Packed store with address register offset | Writes the packed result to the offset address specified by `addr_reg`. | 32-byte aligned. | `c_api/reg_compute/store/storealign.h` |
| [asc_storealign_pack_postupdate](asc_storealign_pack_postupdate.md) | Packed store with Post Update | After packed store, automatically updates the destination address pointer by the element offset. | 32-byte aligned. | `c_api/reg_compute/store/storealign.h` |
| [asc_storealign_pack_quarter](asc_storealign_pack_quarter.md) | Quarter Pack contiguous store | Compresses the low 8 bits of valid 32-bit elements according to `mask` and writes them to the Unified Buffer. | 32-byte aligned. | `c_api/reg_compute/store/storealign.h` |
| [asc_storealign_pack_quarter](asc_storealign_pack_quarter.md) | Quarter Pack store with immediate offset | Writes the Quarter Pack result to `dst + offset`, where `offset` is in elements. | 32-byte aligned. | `c_api/reg_compute/store/storealign.h` |
| [asc_storealign_pack_quarter](asc_storealign_pack_quarter.md) | Quarter Pack store with address register offset | Writes the Quarter Pack result to the offset address specified by `addr_reg`. | 32-byte aligned. | `c_api/reg_compute/store/storealign.h` |
| [asc_storealign_pack_quarter_postupdate](asc_storealign_pack_quarter_postupdate.md) | Quarter Pack store with Post Update | After Quarter Pack store, automatically updates the destination address pointer by the element offset. | 32-byte aligned. | `c_api/reg_compute/store/storealign.h` |
| [asc_store](asc_store.md) | Contiguous store | Stores the entire vector data register, with a single store amount of one VL. For 32-byte aligned scenarios, use `asc_storealign`; for non-32-byte aligned scenarios, use `asc_storeunalign`. | `dtype` aligned. | `c_api/composite/storeunalign_composite.h` |
| [asc_store](asc_store.md) | Store with count | Stores the first `count` elements of the vector data register. For 32-byte aligned scenarios, use `asc_storealign`; for non-32-byte aligned scenarios, use `asc_storeunalign`. | `dtype` aligned. | `c_api/composite/storeunalign_composite.h` |

#### Unaligned Store

The `asc_storeunalign*` APIs temporarily store the tail block through the unaligned register `vector_store_unalign` and store the temporarily stored tail block through a post-processing API. In consecutive calls, the same unaligned register must be reused; the post-processing API must also reuse this register with the preceding main store API.

**Table 3** Vector data register unaligned store APIs

| API Name | Mode | API Description | Destination Address Alignment Requirement | Post-Processing API Used Together | Finest-Grained Common Header File |
| --- | --- | --- | --- | --- | --- |
| [asc_storeunalign](asc_storeunalign.md) | Store with count | Stores the first `count` elements; the main block is written to the Unified Buffer, and the tail block is temporarily stored in the unaligned register. The destination address is updated manually by the user. | `dtype` aligned, no 32-byte alignment required. | Immediate offset store mode of [asc_storeunalign_post](asc_storeunalign_post.md). | `c_api/reg_compute/store/storeunalign.h` |
| [asc_storeunalign_postupdate](asc_storeunalign_postupdate.md) | Immediate offset Post Update store | Stores the first `count` elements and automatically advances the destination address pointer by `count` elements; the tail block is temporarily stored in the unaligned register. | `dtype` aligned, no 32-byte alignment required. | [asc_storeunalign_post_postupdate](asc_storeunalign_post_postupdate.md). | `c_api/reg_compute/store/storeunalign.h` |
| [asc_storeunalign_postupdate](asc_storeunalign_postupdate.md) | Address register offset Post Update store | Stores a fixed VL, using and automatically updating the destination address offset represented by `addr_reg& count`; the tail block is temporarily stored in the unaligned register. | `dtype` aligned, no 32-byte alignment required. | Address register offset store mode of [asc_storeunalign_post](asc_storeunalign_post.md). | `c_api/reg_compute/store/storeunalign.h` |

### Mask Register Store

#### Aligned Store

Aligned mask register store does not support configuring an additional mask. Direct store writes 32-byte mask data; compressed store discards every other bit and contiguously writes the retained bits to the UB.

**Table 4** Mask register aligned store APIs

| API Name | Mode | API Description | Destination Address Alignment Requirement | Finest-Grained Common Header File |
| --- | --- | --- | --- | --- |
| [asc_storealign](asc_storealign.md) | Contiguous aligned store | Directly writes 32-byte mask register data to the Unified Buffer. | 32-byte aligned. | `c_api/reg_compute/store/storealign.h` |
| [asc_storealign](asc_storealign.md) | Store with immediate offset | Writes the mask data to `dst + offset`, where `offset` is in bytes. | 32-byte aligned. | `c_api/reg_compute/store/storealign.h` |
| [asc_storealign](asc_storealign.md) | Address register offset store | Writes the mask data to the offset address specified by `addr_reg`. | 32-byte aligned. | `c_api/reg_compute/store/storealign.h` |
| [asc_storealign_postupdate](asc_storealign_postupdate.md) | Immediate offset post-update store | After directly storing the mask data, automatically updates the destination address pointer by byte offset. | 32-byte aligned. | `c_api/reg_compute/store/storealign.h` |
| [asc_storealign_pack](asc_storealign_pack.md) | Packed contiguous store | Discards 1 bit every 1-bit interval and writes the retained bits to the UB after packing. | 16-byte aligned. | `c_api/reg_compute/store/storealign.h` |
| [asc_storealign_pack](asc_storealign_pack.md) | Packed immediate offset store | Writes the packed result to `dst + offset`, where `offset` is in bytes. | 16-byte aligned. | `c_api/reg_compute/store/storealign.h` |
| [asc_storealign_pack](asc_storealign_pack.md) | Packed address register offset store | Writes the packed result to the offset address specified by `addr_reg`. | 16-byte aligned. | `c_api/reg_compute/store/storealign.h` |
| [asc_storealign_pack_postupdate](asc_storealign_pack_postupdate.md) | Packed post-update store | After packed store, automatically updates the destination address pointer by byte offset. | 16-byte aligned. | `c_api/reg_compute/store/storealign.h` |

#### Unaligned Store

Unaligned mask register store also requires `vector_store_unalign` to assist in saving the tail block. The temporarily stored tail block is stored through a post-processing API, and the register is reused during post-processing.

**Table 5** Mask register unaligned store APIs

| API Name | Mode | API Description | Destination Address Alignment Requirement | Post-Processing API Used Together | Finest-Grained Common Header File |
| --- | --- | --- | --- | --- | --- |
| [asc_storeunalign_postupdate](asc_storeunalign_postupdate.md) | Mask register post-update store | For a `uint16_t` destination address, extracts the LSB every 2 bits and packs them into 16 bytes; for a `uint32_t` destination address, extracts the LSB every 4 bits and packs them into 8 bytes. After storing, automatically updates the destination address pointer, and temporarily stores the tail block in the unaligned register. | `uint16_t` destination address is 16-byte aligned; `uint32_t` destination address is 8-byte aligned. | Immediate offset store mode of [asc_storeunalign_post](asc_storeunalign_post.md). | `c_api/reg_compute/store/storeunalign.h` |

### Packed Status Store

**Table 6** Packed status store APIs

| API Name | API Description | Finest-Granularity Common Header File |
| --- | --- | --- |
| [asc_storealign_squeeze_status](asc_storealign_squeeze_status.md) | Writes the number of valid data bytes saved in the AR special register after the packed operation to the UB, supporting both no-offset and byte-offset forms. | `c_api/reg_compute/store/storealign.h` |
| [asc_storealign_squeeze_status_postupdate](asc_storealign_squeeze_status_postupdate.md) | Writes the number of valid data bytes saved in the AR special register after the packed operation to the UB, and automatically updates the destination address pointer by byte offset. | `c_api/reg_compute/store/storealign.h` |
