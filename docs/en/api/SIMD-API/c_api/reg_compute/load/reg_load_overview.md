---
okftype: reference.concept
description: "Summarizes the applicable modes and address alignment requirements of Reg data load APIs by vector data register, mask register, and unaligned load categories, and provides a comparison of the four address maintenance methods for aligned contiguous loads and an explanation of b64 data movement."
---
# Overview

<!-- md-trans-meta sourceCommit=54f1d966cfcc22290ddc2c6312bbe4c6ac2a9232 translatedAt=2026-11-16T01:40:02.445552+00:00 pushedAt=2026-11-16T02:10:55.610Z nl=2 -->

## Reg Data Load Overview

The Reg data load APIs are used to load data from the Unified Buffer (UB) into vector data registers, mask registers, or unaligned registers. These APIs take effect only on AIV and must be called within a Vector Function (a function marked with `__simd_vf__`).

### General Constraints

- For the general constraints on Reg vector computation C APIs, see [General Constraints](../overview.md#general-constraints).
- The actual read address in each function mode must meet the address alignment requirements in [API List](#api-list), and the actual read range must be within the Unified Buffer address space without exceeding the boundary; otherwise, an error is reported.
- The UB capacity upper limit is 256 KB, and the user-available capacity varies with compilation options and programming scenarios. After the UB address offset, it must not exceed the actual available capacity; otherwise, an error is reported. For details, see [Storage Unit Description](../../general_description_and_constraints.md#storage-unit-description).
- If there is a data dependency between this instruction and other instructions, you must insert the synchronization instruction [asc_mem_bar](../reg_sync/asc_mem_bar.md) to ensure that the instructions execute serially and prevent abnormal data from occurring.

### Key Feature Description

#### Comparison of Aligned Contiguous Load Modes

Aligned contiguous load provides four source address maintenance methods. The following examples all contiguously load source data of length 1024 and data type `half` into vector data registers, and then write it to the UB. Each iteration processes 128 elements, with a total of 8 iterations, and the final result is `dst[i] = src[i]`.

**Table 1** Comparison of aligned contiguous load modes

| Scenario | Load Method | Source Address Maintenance Method | Applicable Scenario |
| --- | --- | --- | --- |
| Scenario 1 | Contiguous aligned load | The caller calculates the source address for each load through pointer arithmetic. | The source address offset rule is simple, and the address needs to be explicitly controlled by the user. |
| Scenario 2 | Immediate offset load | Passes the element offset relative to the source base address as an `int32_t` parameter, and the API does not modify the source address pointer. | The source base address needs to be retained, and the offset is specified by an ordinary integer. |
| Scenario 3 | Address register offset load | Generates `addr_reg` through [asc_update_addr_reg](../reg_addr_reg/asc_update_addr_reg.md), and the hardware updates the offset during iterations. | The offset within a Hardware Loop changes with iterations. |
| Scenario 4 | Post Update load | `asc_loadalign_postupdate` automatically updates the source address pointer after each load. | Contiguous load without the need for the user to manually maintain the source address. |

##### Contiguous Aligned Load

Calls the contiguous aligned load form of `asc_loadalign`. The source address is calculated by `src + i * one_repeat_size`, where `one_repeat_size` is 128 `half` elements.

```cpp
__simd_vf__ inline void load_align_continuous_vf(__ubuf__ half* dst, __ubuf__ half* src,
                                                 uint16_t one_repeat_size, uint16_t repeat_times)
{
    vector_half data_reg;
    vector_bool mask = asc_create_mask_b16(PAT_ALL);
    for (uint16_t i = 0; i < repeat_times; ++i) {
        asc_loadalign(data_reg, src + i * one_repeat_size);
        asc_storealign(dst + i * one_repeat_size, data_reg, mask);
    }
}
```

##### Immediate Offset Load

Calls the immediate offset load mode of `asc_loadalign`. `offset` is relative to the `src` base address, in elements; in this example, the offset for each load is calculated by `i * one_repeat_size`.

```cpp
__simd_vf__ inline void load_align_immediate_offset_vf(__ubuf__ half* dst, __ubuf__ half* src,
                                                       uint16_t one_repeat_size, uint16_t repeat_times)
{
    vector_half data_reg;
    vector_bool mask = asc_create_mask_b16(PAT_ALL);
    for (uint16_t i = 0; i < repeat_times; ++i) {
        asc_loadalign(data_reg, src, i * one_repeat_size);
        asc_storealign(dst + i * one_repeat_size, data_reg, mask);
    }
}
```

##### Address Register Offset Load

Calls the address register offset load mode of `asc_loadalign`. `addr_reg` must be assigned and used in the innermost loop, and in b16 mode the offset unit is elements.

```cpp
__simd_vf__ inline void load_align_addr_reg_offset_vf(__ubuf__ half* dst, __ubuf__ half* src,
                                                      uint16_t one_repeat_size, uint16_t repeat_times)
{
    vector_half data_reg;
    vector_bool mask = asc_create_mask_b16(PAT_ALL);
    addr_reg offset;
    for (uint16_t i = 0; i < repeat_times; ++i) {
        offset = asc_update_addr_reg_b16(one_repeat_size);
        asc_loadalign(data_reg, src, offset);
        asc_storealign(dst + i * one_repeat_size, data_reg, mask);
    }
}
```

##### Post Update Load

Calls the immediate offset load mode of `asc_loadalign_postupdate`. The source address is passed by reference, and the API advances it by `one_repeat_size` elements after each load.

```cpp
__simd_vf__ inline void load_align_post_update_vf(__ubuf__ half* dst, __ubuf__ half* src,
                                                  uint16_t one_repeat_size, uint16_t repeat_times)
{
    vector_half data_reg;
    vector_bool mask = asc_create_mask_b16(PAT_ALL);
    for (uint16_t i = 0; i < repeat_times; ++i) {
        asc_loadalign_postupdate(data_reg, src, one_repeat_size);
        asc_storealign(dst + i * one_repeat_size, data_reg, mask);
    }
}
```

#### Unaligned Movement Features

Unaligned data movement is used for contiguous data load or store scenarios where the effective start address meets dtype alignment but does not meet 32-byte alignment. Both load and store use [Unaligned Registers](../../defs/type/data_type_definition.md#unaligned-registers) to hold data that crosses 32-byte boundaries, ensuring the integrity of contiguous data.

##### Unaligned Load Principle

Unaligned load uses unaligned registers to buffer the preceding data in the 32-byte block where the start address is located, and then concatenates it with subsequently read data to obtain VL bytes of data starting from the effective start address.

**Figure 1** Unaligned load diagram<a id="fig-loadunalign"></a>

![Unaligned load diagram](../../../../figures/capi_loadunalign.png "Unaligned load diagram")

Assume the valid start byte address is `src_start_addr`. Align it downward to 32 bytes to obtain `aligned_src_start_addr`. The process is as follows:

1. Call [asc_loadunalign_pre](asc_loadunalign_pre.md) to buffer the data in the range `[aligned_src_start_addr, aligned_src_start_addr + 32)` in the unaligned register.
2. Call [asc_loadunalign](asc_loadunalign.md) or [asc_loadunalign_postupdate](asc_loadunalign_postupdate.md) to concatenate the buffered preceding data with the subsequently read data, obtain the data in the range `[src_start_addr, src_start_addr + VL)`, and write it to the vector data register. `asc_loadunalign_postupdate` also updates the unaligned register to buffer the preceding data required for the next contiguous load.

The actual access covers the 32-byte range aligned downward. Therefore, in addition to the valid start address satisfying dtype alignment, the aligned read range must also be within the UB address space.

**Table 2** Coordination of unaligned load APIs

| Main Load Method | Supporting API | Description |
| --- | --- | --- |
| [asc_loadunalign](asc_loadunalign.md) | [asc_loadunalign_pre](asc_loadunalign_pre.md) | The user manually updates the source address. The preprocessing API must be called before each load, and both APIs must use the same source address. |
| Immediate offset mode of [asc_loadunalign_postupdate](asc_loadunalign_postupdate.md) | [asc_loadunalign_pre](asc_loadunalign_pre.md) | The API automatically updates the source address. Call the preprocessing API before the first load. Subsequent contiguous loads can reuse the updated unaligned register. If the source address is modified separately, preprocessing must be performed again. |
| Address register offset mode of [asc_loadunalign_postupdate](asc_loadunalign_postupdate.md) | [asc_update_addr_reg](../reg_addr_reg/asc_update_addr_reg.md), [asc_loadunalign_pre](asc_loadunalign_pre.md) | The API automatically updates the address register offset. In each loop iteration, first generate the start offset, and then use the same source address and offset for preprocessing. Contiguous loads within an iteration can reuse the updated unaligned register. If the source address or offset is modified separately, preprocessing must be performed again. |

##### Unaligned Store Principle

Unaligned store writes contiguous data from the vector data register to a UB address that is not 32-byte aligned. Each main store writes the main block that can be written directly to UB, and stores the trailing data that cannot form a 32-byte aligned block in the unaligned register. During subsequent contiguous stores, the API concatenates this tail block with the start data of the current main block, writes the result to UB, and updates the current tail block.

Before the first store, the unaligned register does not need to be initialized. During consecutive calls, the same unaligned register must be reused. If the previous tail block is not contiguous with the current main block, call the post-processing API to complete writing the previous tail block before the next main store.

**Figure 2** Unaligned store diagram (first store)<a id="fig-storeunalign-empty"></a>

![Unaligned store diagram (first store)](../../../../figures/capi_storeunalign_1.png "Unaligned store diagram (first store)")

During the first store, there is no preceding tail block data in the unaligned register. The main store API writes the data that can be written directly from the vector data register to UB, and stores the trailing data in the unaligned register. After calling the post-processing API, the tail block data is written to UB.

**Figure 3** Unaligned store diagram (contiguous store)<a id="fig-storeunalign-cached"></a>

![Unaligned store diagram (contiguous store)](../../../../figures/capi_storeunalign_2.png "Unaligned store diagram (contiguous store)")

During subsequent contiguous stores, the main store API concatenates the preceding tail block in the unaligned register with the main block data in the current vector data register, writes the result to UB, and then updates the current tail block to the unaligned register. After contiguous stores end, call the post-processing API to write the last cached tail block.

**Table 3** Coordination of unaligned store APIs

| Main Store Method | Supporting API | Description |
| --- | --- | --- |
| [asc_storeunalign](../store/asc_storeunalign.md) | [asc_storeunalign_post](../store/asc_storeunalign_post.md) | The user manually updates the destination address. |
| Immediate offset mode of [asc_storeunalign_postupdate](../store/asc_storeunalign_postupdate.md) | [asc_storeunalign_post_postupdate](../store/asc_storeunalign_post_postupdate.md) | The API automatically updates the destination address. |
| Address register offset mode of [asc_storeunalign_postupdate](../store/asc_storeunalign_postupdate.md) | [asc_storeunalign_post](../store/asc_storeunalign_post.md) | The main store and post-processing APIs must reuse the same unaligned register and address register. |

##### Unaligned Load and Store Example

The following figure uses `uint32_t` data as an example to show the process of contiguous unaligned load followed by contiguous unaligned store. The pre-initialization before load and the post-processing after store are both outside the loop: during contiguous load, the unaligned register updated by `asc_loadunalign_postupdate` can be used for the next load; during contiguous store, `asc_storeunalign_postupdate` stores the tail block in the unaligned register, and the tail block is written back uniformly after the last loop iteration.

**Figure 4** Example of contiguous unaligned load and store<a id="fig-loadstoreunalign"></a>

![Example of contiguous unaligned load and store](../../../../figures/capi_loadunalign_storeunalign.png "Example of contiguous unaligned load and store")

```c
__simd_vf__ inline void load_store_unalign_vf(__ubuf__ uint32_t* dst, __ubuf__ uint32_t* src,
                                              uint32_t count, uint16_t repeat_times)
{
    vector_load_unalign ureg0;
    vector_store_unalign ureg1;
    vector_uint32_t data_reg;
    // Initialize the unaligned load only once before the loop starts.
    asc_loadunalign_pre(ureg0, src);
    for (uint16_t i = 0; i < repeat_times; ++i) {
        asc_loadunalign_postupdate(data_reg, ureg0, src, count);
        asc_storeunalign_postupdate(dst, ureg1, data_reg, count);
    }
    // Post-process the unaligned store only once after the loop ends.
    asc_storeunalign_post_postupdate(dst, ureg1, 0);
}
```

The data movement steps are as follows:

1. In ①: Before the loop starts, `asc_loadunalign_pre` buffers the data of the 32-byte block where the source address resides into `ureg0`, preparing the leading data for the first unaligned load.
2. In ②: In the first loop iteration, `asc_loadunalign_postupdate` concatenates the leading data in `ureg0` with the subsequently read data into a `data_reg` of VL length, and updates `ureg0` and `src`.
3. In ③: `asc_storeunalign_postupdate` is called for the first time to write the main block data in `data_reg` that can be written directly to the UB, cache the tail block into `ureg1`, and update `dst`.
4. In ④: In subsequent loop iterations, `asc_loadunalign_postupdate` is called again to reuse the updated `ureg0` and `src` for the next load, and updates both again.
5. In ⑤: `asc_storeunalign_postupdate` is called subsequently to concatenate the previous tail block in `ureg1` with the main block data in the current `data_reg`, write the result to the UB, update `ureg1` with the current tail block, and update `dst`.
6. In ⑥: After the loop ends, `asc_storeunalign_post_postupdate` writes the last cached tail block in `ureg1` to the UB, completing the contiguous unaligned store.

In the example, `count` is the number of elements loaded or stored per operation. `src` and `dst` are automatically updated by the load and store APIs respectively, and the corresponding unaligned registers must be reused within the loop. `count` must not exceed the number of elements that a single vector data register can hold, and the load and store address ranges must be within the actually available UB space.

#### Movement of b64 Data Type

The following APIs support the b64 data type.

**Table 4** APIs supporting the b64 data type

| Supported Data Types | API |
| --- | --- |
| `int64_t`, `uint64_t` | [asc_loadalign](asc_loadalign.md)<br>[asc_loadalign_postupdate](asc_loadalign_postupdate.md) |
| `int64_t` | [asc_load](asc_load.md)<br>[asc_loadunalign_pre](asc_loadunalign_pre.md)<br>[asc_loadunalign](asc_loadunalign.md)<br>[asc_loadunalign_postupdate](asc_loadunalign_postupdate.md) |

When `asc_loadalign`, `asc_loadunalign_pre`, or `asc_loadunalign_postupdate` accesses b64 data through an address register, use [asc_update_addr_reg_b32](../reg_addr_reg/asc_update_addr_reg.md) to generate the address register. The offset of each dimension of this API is in units of 4 bytes. Therefore, when an offset of N b64 elements is expected, the corresponding parameter should be set to `2 × N`. The `inc` of `asc_loadunalign_postupdate` is in units of the number of elements of the source data type. When advancing N b64 elements each time is expected, set it to N without multiplying by 2.

In the non-contiguous aligned load modes of `asc_loadalign` and `asc_loadalign_postupdate`, when processing `int64_t` data, the mask creation APIs do not support b64 mode. First use [asc_create_mask_b32](../reg_mask/asc_create_mask.md) to create a b32 mask, and then use [asc_unpack_lower](../reg_permute_sel/asc_unpack.md) to expand it into the mask corresponding to b64 data. In the expanded mask, every 8 consecutive bits form a group, and only the lowest bit of each group is valid, controlling one b64 element.

The following example uses `asc_loadalign` to show how to construct a b64 mask and how to convert the address register offset.

```cpp
vector_int64_t src_reg;
vector_bool mask_b32 = asc_create_mask_b32(PAT_ALL);
vector_bool mask_b64;
asc_unpack_lower(mask_b64, mask_b32);
addr_reg load_offset;
addr_reg store_offset;
for (uint16_t i = 0; i < repeatTimes; ++i) {
    load_offset = asc_update_addr_reg_b32(elements_vl * 2);
    // After the data is stored to UB, the data type is B32, so the offset does not need to be multiplied by 2.
    store_offset = asc_update_addr_reg_b32(elements_vl);
    asc_loadalign(src_reg, input, load_offset);
    asc_storealign_pack(output, src_reg, store_offset, mask_b64);
}
```

## API List

Select the appropriate API based on the data access method, the data layout after loading, and whether automatic address update is required.

### Vector Data Register Load

#### Aligned Load

**Table 5** Vector data register aligned load APIs

| API Name | Mode | API Description | Address Alignment Requirement | Finest-Granularity Common Header File |
| --- | --- | --- | --- | --- |
| [asc_loadalign](asc_loadalign.md) | Contiguous aligned load | Loads one VL of data contiguously, with the source address updated by the user. | 32 bytes | `c_api/reg_compute/load/loadalign.h` |
| [asc_loadalign](asc_loadalign.md) | Immediate offset load mode | Loads one VL of data after offsetting a specified distance from the source start address. | 32 bytes | `c_api/reg_compute/load/loadalign.h` |
| [asc_loadalign](asc_loadalign.md) | Address register offset load mode | Loads one VL of data according to the offset specified by the address register. | 32 bytes | `c_api/reg_compute/load/loadalign.h` |
| [asc_loadalign](asc_loadalign.md) | Non-contiguous aligned load mode | Loads 8 DataBlocks non-contiguously according to the configured DataBlock step. | 32 bytes | `c_api/reg_compute/load/loadalign.h` |
| [asc_loadalign_postupdate](asc_loadalign_postupdate.md) | Immediate offset load mode | Loads data of one VL length and updates the source address by the immediate offset. | 32 bytes | `c_api/reg_compute/load/loadalign.h` |
| [asc_loadalign_postupdate](asc_loadalign_postupdate.md) | Non-contiguous aligned load mode | Non-contiguously loads 8 DataBlocks and automatically updates the source address. | 32 bytes | `c_api/reg_compute/load/loadalign.h` |
| [asc_loadalign_datablock_strided](asc_loadalign_datablock_strided.md) | Non-contiguous aligned load mode | Non-contiguously loads 8 DataBlocks according to the configured DataBlock stride and returns the result through the return value. | 32 bytes | `c_api/reg_compute/load/loadalign.h` |
| [asc_loadalign_brc_datablock](asc_loadalign_brc_datablock.md) | Aligned load mode | Broadcasts one DataBlock to the entire vector data register. | 32 bytes | `c_api/reg_compute/load/loadalign.h` |
| [asc_loadalign_brc_datablock](asc_loadalign_brc_datablock.md) | Immediate offset load mode | Reads one DataBlock from the immediate offset position and broadcasts it. | 32 bytes | `c_api/reg_compute/load/loadalign.h` |
| [asc_loadalign_brc_datablock](asc_loadalign_brc_datablock.md) | Address register offset load mode | Reads one DataBlock from the address register offset position and broadcasts it. | 32 bytes | `c_api/reg_compute/load/loadalign.h` |
| [asc_loadalign_brc_datablock_postupdate](asc_loadalign_brc_datablock_postupdate.md) | Post Update load mode | Broadcasts one DataBlock to the entire vector data register and automatically updates the source address. | 32 bytes | `c_api/reg_compute/load/loadalign.h` |
| [asc_loadalign_brc_elem](asc_loadalign_brc_elem.md) | Aligned load mode | Broadcasts one element to the entire vector data register. | Aligned to `dtype` | `c_api/reg_compute/load/loadalign.h` |
| [asc_loadalign_brc_elem](asc_loadalign_brc_elem.md) | Immediate offset load mode | Reads one element from the immediate offset position and broadcasts it. | Aligned to `dtype` | `c_api/reg_compute/load/loadalign.h` |
| [asc_loadalign_brc_elem](asc_loadalign_brc_elem.md) | Address register offset load mode | Reads one element from the address register offset position and broadcasts it. | Aligned to `dtype` | `c_api/reg_compute/load/loadalign.h` |
| [asc_loadalign_brc_elem_postupdate](asc_loadalign_brc_elem_postupdate.md) | Post Update load mode | Broadcasts one element to the entire vector data register and automatically updates the source address. | Aligned to `dtype` | `c_api/reg_compute/load/loadalign.h` |
| [asc_loadalign_brc_elem2datablock](asc_loadalign_brc_elem2datablock.md) | Aligned load mode | Contiguously reads 8 elements and broadcasts each element to one DataBlock. | 16 bytes for b16, 32 bytes for b32 | `c_api/reg_compute/load/loadalign.h` |
| [asc_loadalign_brc_elem2datablock](asc_loadalign_brc_elem2datablock.md) | Immediate offset load mode | Reads 8 elements from the immediate offset position and broadcasts each to one DataBlock. | 16 bytes for b16, 32 bytes for b32 | `c_api/reg_compute/load/loadalign.h` |
| [asc_loadalign_brc_elem2datablock](asc_loadalign_brc_elem2datablock.md) | Address register offset load mode | Reads 8 elements from the address register offset position and broadcasts each to one DataBlock. | 16 bytes for b16, 32 bytes for b32 | `c_api/reg_compute/load/loadalign.h` |
| [asc_loadalign_brc_elem2datablock_postupdate](asc_loadalign_brc_elem2datablock_postupdate.md) | Post Update load mode | Contiguously reads and broadcasts 8 elements, and automatically updates the source address. | 16 bytes for b16, 32 bytes for b32 | `c_api/reg_compute/load/loadalign.h` |
| [asc_loadalign_deintlv](asc_loadalign_deintlv.md) | Aligned load mode | Reads data of 2×VL length, deinterleaves it, and writes it to two vector data registers. | 32 bytes | `c_api/reg_compute/load/loadalign.h` |
| [asc_loadalign_deintlv](asc_loadalign_deintlv.md) | Immediate offset load mode | Reads data from the immediate offset position and deinterleaves it. | 32 bytes | `c_api/reg_compute/load/loadalign.h` |
| [asc_loadalign_deintlv](asc_loadalign_deintlv.md) | Address register offset load mode | Reads data from the address register offset position and deinterleaves it. | 32 bytes | `c_api/reg_compute/load/loadalign.h` |
| [asc_loadalign_deintlv_postupdate](asc_loadalign_deintlv_postupdate.md) | Post Update load mode | Reads data, deinterleaves it into two vector data registers, and automatically updates the source address. | 32 bytes | `c_api/reg_compute/load/loadalign.h` |
| [asc_loadalign_downsample](asc_loadalign_downsample.md) | Aligned load mode | Reads data of 2×VL length and retains elements with even indices. | 32 bytes | `c_api/reg_compute/load/loadalign.h` |
| [asc_loadalign_downsample](asc_loadalign_downsample.md) | Immediate offset load mode | Reads data from the immediate offset position and performs 2x downsampling. | 32 bytes | `c_api/reg_compute/load/loadalign.h` |
| [asc_loadalign_downsample](asc_loadalign_downsample.md) | Address register offset load mode | Reads data from the address register offset position and performs 2x downsampling. | 32 bytes | `c_api/reg_compute/load/loadalign.h` |
| [asc_loadalign_downsample_postupdate](asc_loadalign_downsample_postupdate.md) | Post Update load mode | Reads data, performs 2x downsampling, and automatically updates the source address. | 32 bytes | `c_api/reg_compute/load/loadalign.h` |
| [asc_loadalign_upsample](asc_loadalign_upsample.md) | Aligned load mode | Reads data of VL/2 length and repeats each element twice. | 32 bytes | `c_api/reg_compute/load/loadalign.h` |
| [asc_loadalign_upsample](asc_loadalign_upsample.md) | Immediate offset load mode | Reads data from the immediate offset position and performs 2x upsampling. | 32 bytes | `c_api/reg_compute/load/loadalign.h` |
| [asc_loadalign_upsample](asc_loadalign_upsample.md) | Address register offset load mode | Reads data from the address register offset position and performs 2x upsampling. | 32 bytes | `c_api/reg_compute/load/loadalign.h` |
| [asc_loadalign_upsample_postupdate](asc_loadalign_upsample_postupdate.md) | Post Update load mode | Reads data and performs 2x upsampling while automatically updating the source address. | 32 bytes | `c_api/reg_compute/load/loadalign.h` |
| [asc_loadalign_unpack](asc_loadalign_unpack.md) | Aligned load mode | Reads data of VL/2 length and pads one 0 after each source element. | 32 bytes | `c_api/reg_compute/load/loadalign.h` |
| [asc_loadalign_unpack](asc_loadalign_unpack.md) | Immediate offset load mode | Reads data from the immediate offset position and pads one 0 after each source element. | 32 bytes | `c_api/reg_compute/load/loadalign.h` |
| [asc_loadalign_unpack](asc_loadalign_unpack.md) | Address register offset load mode | Reads data from the address register offset position and pads one 0 after each source element. | 32 bytes | `c_api/reg_compute/load/loadalign.h` |
| [asc_loadalign_unpack_postupdate](asc_loadalign_unpack_postupdate.md) | Post Update load mode | Reads data of VL/2 length, pads one 0 after each source element, and automatically updates the source address. | 32 bytes | `c_api/reg_compute/load/loadalign.h` |
| [asc_loadalign_unpack4](asc_loadalign_unpack4.md) | Aligned load mode | Reads data of VL/4 length and pads three 0s after each source element. | 32 bytes | `c_api/reg_compute/load/loadalign.h` |
| [asc_loadalign_unpack4](asc_loadalign_unpack4.md) | Immediate offset load mode | Reads data from the immediate offset position and pads three 0s after each source element. | 32 bytes | `c_api/reg_compute/load/loadalign.h` |
| [asc_loadalign_unpack4](asc_loadalign_unpack4.md) | Address register offset load mode | Reads data from the address register offset position and pads three 0s after each source element. | 32 bytes | `c_api/reg_compute/load/loadalign.h` |
| [asc_loadalign_unpack4_postupdate](asc_loadalign_unpack4_postupdate.md) | Post Update load mode | Reads data of VL/4 length, pads three 0s after each source element, and automatically updates the source address. | 32 bytes | `c_api/reg_compute/load/loadalign.h` |

#### Unaligned Load

**Table 6** Vector data register unaligned load APIs

| API Name | Mode | API Description | Address Alignment Requirement | Finest-Granularity Common Header File |
| --- | --- | --- | --- | --- |
| [asc_load](asc_load.md) | Contiguous unaligned load mode | Loads data of one VL length, with the source address updated by the user. This API is an ease-of-use API. | Aligned to `dtype` | `c_api/composite/loadunalign_composite.h` |
| [asc_loadunalign_pre](asc_loadunalign_pre.md) | Source address preprocessing mode | Initializes the unaligned register based on the source address. | Aligned to `dtype` | `c_api/reg_compute/load/loadunalign.h` |
| [asc_loadunalign_pre](asc_loadunalign_pre.md) | Address register offset preprocessing mode | Initializes the unaligned register based on the source address and address register offset. | Aligned to `dtype` | `c_api/reg_compute/load/loadunalign.h` |
| [asc_loadunalign](asc_loadunalign.md) | Contiguous unaligned load mode | Uses the preprocessing result to load data of one VL length, with the source address updated by the user. | Aligned to `dtype` | `c_api/reg_compute/load/loadunalign.h` |
| [asc_loadunalign_postupdate](asc_loadunalign_postupdate.md) | Contiguous unaligned load mode | Uses the preprocessing result to load a specified number of elements and automatically updates the source address and unaligned register. | Aligned to `dtype` | `c_api/reg_compute/load/loadunalign.h` |
| [asc_loadunalign_postupdate](asc_loadunalign_postupdate.md) | Address register offset load mode | Loads data from the address register offset position and automatically updates the offset and unaligned register. | Aligned to `dtype` | `c_api/reg_compute/load/loadunalign.h` |

`asc_load` is an ease-of-use API. For contiguous unaligned load scenarios with performance requirements, use `asc_loadunalign` or `asc_loadunalign_postupdate`.

### Mask Register Load

**Table 7** Mask register load APIs

| API Name | Mode | API Description | Address Alignment Requirement | Finest-Granularity Common Header File |
| --- | --- | --- | --- | --- |
| [asc_loadalign](asc_loadalign.md) | Contiguous aligned load | Contiguously loads data of VL/8 length, with the source address updated by the user. | 32 bytes | `c_api/reg_compute/load/loadalign.h` |
| [asc_loadalign](asc_loadalign.md) | Immediate offset load mode | Loads data of VL/8 length after offsetting the source start address by a specified distance. | 32 bytes | `c_api/reg_compute/load/loadalign.h` |
| [asc_loadalign](asc_loadalign.md) | Address register offset load mode | Loads data of VL/8 length according to the offset specified by the address register. | 32 bytes | `c_api/reg_compute/load/loadalign.h` |
| [asc_loadalign_postupdate](asc_loadalign_postupdate.md) | Immediate offset load mode | Loads data of VL/8 length and updates the source address by the immediate offset. | 32 bytes | `c_api/reg_compute/load/loadalign.h` |
| [asc_loadalign_mask](asc_loadalign_mask.md) | Contiguous aligned load | Contiguously loads data of VL/8 length and returns the result through the return value. | 32 bytes | `c_api/reg_compute/load/loadalign.h` |
| [asc_loadalign_downsample](asc_loadalign_downsample.md) | Aligned load mode | Reads data of VL/4 length and retains even-indexed bits. | 32 bytes | `c_api/reg_compute/load/loadalign.h` |
| [asc_loadalign_downsample](asc_loadalign_downsample.md) | Immediate offset load mode | Reads data from the immediate offset position and performs 2x downsampling. | 32 bytes | `c_api/reg_compute/load/loadalign.h` |
| [asc_loadalign_downsample](asc_loadalign_downsample.md) | Address register offset load mode | Reads data from the address register offset position and performs 2x downsampling. | 32 bytes | `c_api/reg_compute/load/loadalign.h` |
| [asc_loadalign_downsample_postupdate](asc_loadalign_downsample_postupdate.md) | Post Update load mode | Reads data, performs 2x downsampling, and automatically updates the source address. | 32 bytes | `c_api/reg_compute/load/loadalign.h` |
| [asc_loadalign_mask_downsample](asc_loadalign_mask_downsample.md) | Aligned load mode | Reads data of VL/4 length, retains even-indexed bits, and returns the result through the return value. | 32 bytes | `c_api/reg_compute/load/loadalign.h` |
| [asc_loadalign_upsample](asc_loadalign_upsample.md) | Aligned load mode | Reads data of VL/16 length and duplicates each bit twice. | 16 bytes | `c_api/reg_compute/load/loadalign.h` |
| [asc_loadalign_upsample](asc_loadalign_upsample.md) | Immediate offset load mode | Reads data from the immediate offset position and performs 2x upsampling. | 16 bytes | `c_api/reg_compute/load/loadalign.h` |
| [asc_loadalign_upsample](asc_loadalign_upsample.md) | Address register offset load mode | Reads data from the address register offset position and performs 2x upsampling. | 16 bytes | `c_api/reg_compute/load/loadalign.h` |
| [asc_loadalign_upsample_postupdate](asc_loadalign_upsample_postupdate.md) | Post Update load mode | Reads data, performs 2x upsampling, and automatically updates the source address. | 16 bytes | `c_api/reg_compute/load/loadalign.h` |
| [asc_loadalign_mask_upsample](asc_loadalign_mask_upsample.md) | Aligned load mode | Reads data of VL/16 length, duplicates each bit twice, and returns the result through the return value. | 16 bytes | `c_api/reg_compute/load/loadalign.h` |
