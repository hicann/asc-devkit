# Reg Data Type Definition

<!-- md-trans-meta sourceCommit=20f3bc2ba9d24ddc30f5d7c0cf577fe8f93ec36c translatedAt=2026-10-28T13:36:20.152Z pushedAt=2026-10-29T02:01:15.758Z nl=2 -->

## Applicable Products

<!-- npu="950" id1 -->
- Ascend 950PR&950DT products: Supported
<!-- end id1 -->
<!-- npu="A3" id2 -->
- Atlas A3 products: Not supported
<!-- end id2 -->
<!-- npu="910b" id3 -->
- Atlas A2 products: Not supported
<!-- end id3 -->
<!-- npu="310b" id4 -->
- Atlas 200I/500 A2 inference products: Not supported
<!-- end id4 -->
<!-- npu="310p" id5 -->
- Atlas inference product AI Core: Not supported
<!-- end id5 -->
<!-- npu="310p" id6 -->
- Atlas inference product Vector Core: Not supported
<!-- end id6 -->
<!-- npu="910" id7 -->
- Atlas training products: Not supported
<!-- end id7 -->

## Vector Data Register

The vector data register stores vector data, and its length is `VL` (Vector Length). In the Ascend 950PR&950DT products, `VL` is 256 bytes and can store `VL / sizeof(dtype)` elements of the `dtype` type. For example, `vector_float` can store `256 / sizeof(float) = 64` elements of the `float` type.

Vector data register types are classified by bit-width category as follows:

**Table 1** Bit-width category

| Bit-width category | Vector data register type |
| --- | --- |
| b8 | vector_int8_t, vector_uint8_t, vector_int4x2_t, vector_fp4x2_e2m1_t, vector_fp4x2_e1m2_t, vector_hifloat8_t, vector_fp8_e4m3fn_t, vector_fp8_e5m2_t, vector_fp8_e8m0_t |
| b16 | vector_int16_t, vector_uint16_t, vector_half, vector_bfloat16_t |
| b32 | vector_int32_t, vector_uint32_t, vector_float |
| b64 | vector_int64_t, vector_uint64_t |

**Note:**

- `vector_int4x2_t`, `vector_fp4x2_e2m1_t`, and `vector_fp4x2_e1m2_t` are all packed types. Two 4-bit elements share the storage space of one byte, so they are classified into the `b8` bit-width category.

- The maximum number of vector data registers is 32. Register data exceeding this limit is written to the reserved 8KB Unified Buffer (UB) space, which may cause performance degradation. The compiler will automatically reuse registers and reserved space whose lifecycle has ended; if both are available, registers are reused first.

- The register's lifecycle is limited to within a single Vector Function (VF).

## Mask Register

The mask register's type is `vector_bool`. It is used to select the elements that participate in vector computation. Its length is `VL / 8`, that is, 32 bytes (256 bits).

As shown in Figure 1, when the operand type is `b8`, each element corresponds to a 1-bit mask; when the operand type is `b16`, each element corresponds to a 2-bit mask, and only the least significant bit (LSB) is valid; when the operand type is `b32`, each element corresponds to a 4-bit mask, and only the least significant bit is valid.

**Figure 1** Mask register

![Mask register](../../../../figures/capi_mask.png)

Each element of a `b64` operand corresponds to an 8-bit mask, and only the least significant bit is valid. Because the mask creation API does not support the `b64` mode, you need to first create a `b32` mask through [asc_create_mask_b32](../../reg_compute/reg_mask/asc_create_mask.md), and then expand it into a `b64` mask through [asc_unpack_lower](../../reg_compute/reg_permute_sel/asc_unpack.md). For example, to create a mask in which all `b64` elements are valid:

```cpp
vector_bool mask_b32 = asc_create_mask_b32(PAT_ALL);
vector_bool mask_b64;
asc_unpack_lower(mask_b64, mask_b32);
```

The mask register supports the following setting methods, which you can select based on the actual computation scenario:

**Table 2** Mask register setting methods

| Number | Setting Method | Involved Interface | Description |
| :-- | :------------ | :------------ | :------------ |
| 1 | Set by calling the API | [asc_create_mask](../../reg_compute/reg_mask/asc_create_mask.md) | Specifies the mask mode through parameters, that is, specifies the positions of the elements that participate in computation. |
| 2 | Set by calling the API | [asc_update_mask](../../reg_compute/reg_mask/asc_update_mask.md) | &bull; When called outside the loop, sets a fixed mask, indicating that the first `count` elements participate in computation in each loop.<br>&bull; When called inside the loop, generates the mask for the current loop based on the remaining `count` and automatically updates `count`. |
| 3 | Load from UB | [asc_loadalign](../../reg_compute/load/asc_loadalign.md), [asc_loadalign_postupdate](../../reg_compute/load/asc_loadalign_postupdate.md), and other APIs | Loads the mask from UB into the mask register. |
| 4 | Load from vector data register | [asc_copy](../../reg_compute/reg_copy/asc_copy.md) | Moves data from `vector_uint16_t` or `vector_uint32_t` to the mask register. |

**Note:**

The maximum number of mask registers is 8. Register data that exceeds this limit is written to the reserved 8KB UB space, which may cause performance degradation. The compiler will automatically reuse registers and reserved space whose lifecycle has ended; if both are available, registers are reused first.

## Unaligned Registers

Unaligned registers include `vector_load_unalign` and `vector_store_unalign`. These two types of registers serve as buffers for continuously moving unaligned data between UB and vector data registers. In this section, "unaligned" means that the data start address is not aligned to 32 bytes. During the move, the API caches the first or last block of data through the unaligned register, and then completes the continuous read or write of the data.

Before moving data in from an unaligned address, call [asc_loadunalign_pre](../../reg_compute/load/asc_loadunalign_pre.md) to initialize, and then call [asc_loadunalign](../../reg_compute/load/asc_loadunalign.md) or [asc_loadunalign_postupdate](../../reg_compute/load/asc_loadunalign_postupdate.md) to move the data in. When moving data out to an unaligned address, call [asc_storeunalign](../../reg_compute/store/asc_storeunalign.md) or [asc_storeunalign_postupdate](../../reg_compute/store/asc_storeunalign_postupdate.md) first, and then call [asc_storeunalign_post](../../reg_compute/store/asc_storeunalign_post.md) or [asc_storeunalign_post_postupdate](../../reg_compute/store/asc_storeunalign_post_postupdate.md), which matches the move-out mode, for post-processing.

**Note:**

The maximum number of both `vector_load_unalign` registers and `vector_store_unalign` registers is 4. Exceeding this limit causes an error.

### Examples

In the following example, each loop iteration processes one complete `VL`.

```cpp
__simd_vf__ inline void neg_vf(__ubuf__ int8_t* dst_addr, __ubuf__ int8_t* src_addr,
                               uint16_t one_repeat_size, uint16_t repeat_count)
{
    vector_int8_t src_reg;
    vector_int8_t dst_reg;
    vector_load_unalign load_unalign_reg;
    vector_store_unalign store_unalign_reg;
    vector_bool mask = asc_create_mask_b8(PAT_ALL);
    asc_loadunalign_pre(load_unalign_reg, src_addr);
    for (uint16_t i = 0; i < repeat_count; ++i) {
        asc_loadunalign_postupdate(src_reg, load_unalign_reg, src_addr, one_repeat_size);
        asc_neg(dst_reg, src_reg, mask);
        asc_storeunalign_postupdate(dst_addr, store_unalign_reg, dst_reg, one_repeat_size);
    }
    asc_storeunalign_post_postupdate(dst_addr, store_unalign_reg, 0);
}
```

## Address Register

The address register's type is `addr_reg`, which stores address offsets. After the address register is generated in the loop by [asc_update_addr_reg](../../reg_compute/reg_addr_reg/asc_update_addr_reg.md), it calculates the offset based on the stride set for each loop level.

The offset of `addr_reg` is calculated as `offset = Σ(index_i × stride_i)`, where `index_i` is the iteration variable of the `i`-th loop level and `stride_i` is the stride of the corresponding loop level. The offset in the address register is measured in number of elements. When `asc_update_addr_reg_b8`, `asc_update_addr_reg_b16`, and `asc_update_addr_reg_b32` are called to generate the address register, the actual address offset of each element is calculated as 1 byte, 2 bytes, and 4 bytes, respectively.

**Note:**

- `addr_reg` applies only to some data movement APIs. Check the function prototype of the specific API to confirm whether it is supported. Besides using `addr_reg`, you can also accumulate the address directly or update the address in Post Update mode. For the complete data movement methods, see [Reg Aligned Load](../../reg_compute/load/load.md#reg-aligned-load), [Reg Unaligned Load](../../reg_compute/load/load.md#reg-unaligned-load), [Reg Aligned Store](../../reg_compute/store/store.md#reg-aligned-store), and [Reg Unaligned Store](../../reg_compute/store/store.md#reg-unaligned-store).

- When using `addr_reg` for address offset, the actual accessed address must satisfy the address alignment constraints of the corresponding data movement API.

- `asc_update_addr_reg` does not provide a `b64` mode. When moving `int64_t` or `uint64_t` data, use `asc_update_addr_reg_b32` to generate the address register. One `int64_t` or `uint64_t` element occupies two `b32` offset units, so the offset of each dimension passed to `asc_update_addr_reg_b32` must be set to twice the desired 64-bit element offset. For example:

    ```cpp
    __simd_vf__ inline void load_int64_vf(__ubuf__ int64_t* src_addr,
                                          uint32_t elements_per_repeat,
                                          uint16_t repeat_count)
    {
        vector_int64_t src_reg;
        addr_reg offset_reg;
        for (uint16_t i = 0; i < repeat_count; ++i) {
            offset_reg = asc_update_addr_reg_b32(elements_per_repeat * 2);
            asc_loadalign(src_reg, src_addr, offset_reg);
        }
    }
    ```

- The maximum number of `addr_reg` registers is 8. Exceeding 8 may cause performance degradation. The compiler will automatically reuse `addr_reg` registers whose lifecycle has ended.

- Limited by the hardware loop, `addr_reg` supports at most 4 loop levels.

### Examples<a id="example4"></a>

- Single-parameter version

    ```cpp
    __simd_vf__ inline void add_vf(__ubuf__ int8_t* dst_addr, __ubuf__ int8_t* src0_addr,
                                   __ubuf__ int8_t* src1_addr, uint32_t count,
                                   uint16_t one_repeat_size, uint16_t repeat_count)
    {
        vector_int8_t src0_reg;
        vector_int8_t src1_reg;
        vector_int8_t dst_reg;
        vector_bool mask;
        addr_reg offset_reg;
        for (uint16_t i = 0; i < repeat_count; ++i) {
            offset_reg = asc_update_addr_reg_b8(one_repeat_size);
            mask = asc_update_mask_b8(count);
            asc_loadalign(src0_reg, src0_addr, offset_reg);
            asc_loadalign(src1_reg, src1_addr, offset_reg);
            asc_add(dst_reg, src0_reg, src1_reg, mask);
            asc_storealign(dst_addr, dst_reg, offset_reg, mask);
        }
    }
    ```

- Multi-parameter version (using four stride parameters as an example)

    ```cpp
    __simd_vf__ inline void addr_reg_4d_vf(
        __ubuf__ float* dst_addr,
        __ubuf__ float* src_addr,
        uint32_t n_stride,    // Offset between N dimensions (number of float elements)
        uint32_t c_stride,    // Offset between C dimensions (number of float elements)
        uint32_t h_stride,    // Offset between H dimensions (number of float elements)
        uint32_t w_stride,    // Offset between W dimensions (number of float elements)
        uint16_t n_count, uint16_t c_count, uint16_t h_count, uint16_t w_count)
    {
        vector_float src_reg, dst_reg;
        vector_bool mask = asc_create_mask_b32(PAT_ALL);
        addr_reg offset_reg;
        for (uint16_t n = 0; n < n_count; ++n) {
            for (uint16_t c = 0; c < c_count; ++c) {
                for (uint16_t h = 0; h < h_count; ++h) {
                    for (uint16_t w = 0; w < w_count; ++w) {
                        offset_reg = asc_update_addr_reg_b32(
                            w_stride, h_stride, c_stride, n_stride);
                        asc_loadalign(src_reg, src_addr, offset_reg);
                        asc_add(dst_reg, src_reg, src_reg, mask);
                        asc_storealign(dst_addr, dst_reg, offset_reg, mask);
                    }
                }
            }
        }
    }
    ```
