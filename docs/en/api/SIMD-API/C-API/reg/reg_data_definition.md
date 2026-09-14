# reg Data Definition

<!-- md-trans-meta sourceCommit=835d92789c5d36ac73f0d37066ed365b64c05433 translatedAt=2026-09-04T11:57:46.164Z pushedAt=2026-09-05T12:14:28.984Z -->

## Vector Data Register

The vector data register is used to store vector data. Its bit width is VL (Vector Length), and it can store VL/sizeof(T) data elements (where T indicates the data type). In the Ascend 950PR/Ascend 950DT versions, VL = 256B. For example, for the vector data type vector_float, the number of elements that this register can store is 256B / sizeof(float) = 64.

The following lists all vector data register data types, classified by bit width:

| Bit Width      | Vector Data Type|
| --------- | --------- |
| b8        | vector_int8_t/vector_uint8_t/vector_int4x2_t/vector_fp4x2_e2m1_t/vector_fp4x2_e1m2_t/vector_hifloat8_t/vector_fp8_e4m3fn_t/vector_fp8_e5m2_t/vector_fp8_e8m0_t |
| b16       | vector_int16_t/vector_uint16_t/vector_half/vector_bfloat16_t |
| b32       | vector_int32_t/vector_uint32_t/vector_float |
| b64       | vector_int64_t/vector_uint64_t |

**Note:**
For the three vector data types vector_int4x2_t, vector_fp4x2_e2m1_t, and vector_fp4x2_e1m2_t, the memory layout requires two elements to be packed into a one-byte storage unit.

### Example<a name="example-1"></a>

```cpp
vector_half dst;
half index = 0;
asc_arange(dst, index);
```

## Mask Register

The data type of the mask register is **vector_bool**, which is used to select the elements participating in vector computation. Its bit width is VL/8. When moving a mask register, the address offset unit in **ubuf** is bytes.

![vector_bool](../figures/vector_bool.png)

### Example<a name="example-2"></a>

```cpp
uint32_t length = 255;
vector_bool mask = asc_create_mask_b16(PAT_ALL); // Create a mask register with all elements set to True.
vector_bool mask = asc_update_mask_b16(length); // Generate the corresponding mask register based on the specific number of elements to be operated on, calculated from the vector.
```

## Unaligned Registers

Unaligned registers include **vector_load_unalign** and **vector_store_unalign**. These registers serve as buffers for continuous unaligned data transfer between the UB and vector data registers, where "unaligned" specifically means that the data start address is not aligned to 32 bytes. During the transfer, unaligned data is first loaded into the dedicated unaligned register, and then the data is read or written in blocks through the corresponding transfer APIs.

Before reading an unaligned address, **vector_load_unalign** should be initialized through **asc_loadunalign_pre**, and then **asc_loadunalign** is used. When writing an unaligned address, **asc_storeunalign** should be used first, followed by **asc_storeunalign_post** for processing.

### Example<a name="example-3"></a>

```cpp
constexpr uint32_t one_repeat_size = 256 / sizeof(int8_t); // VL / sizeof(T)
uint32_t total_length = 255;
uint32_t repeat_time = (total_length + one_repeat_size - 1) / one_repeat_size;
__simd_vf__ inline void neg_vf(__ubuf__ int8_t* dst_addr, __ubuf__ int8_t* src_addr, uint32_t count,
    uint32_t one_repeat_size, uint16_t repeat_time)
{
    vector_int8_t src;
    vector_int8_t dst;
    vector_load_unalign ureg0;
    vector_store_unalign ureg1;
    vector_bool mask;
    for (uint16_t i = 0; i < repeat_time; ++i) {
        mask = asc_update_mask_b8(count);
        asc_loadunalign_pre(ureg0, src_addr + i * one_repeat_size); // Initialize before the unaligned load.
        asc_loadunalign(src, ureg0, src_addr + i * one_repeat_size); // Load the source data in an unaligned manner for use with vector_load_unalign.
        asc_neg(dst, src, mask);
        asc_storeunalign(dst_addr + i * one_repeat_size, ureg1, dst, one_repeat_size); // Store the destination data in an unaligned manner for use with vector_store_unalign.
        asc_storeunalign_post(dst_addr + i * one_repeat_size, ureg1, 0); // Process the tail block of the unaligned store.
    }
}
```

## Address Register

The data type of the address register is **iter_reg**, which is used to store address offsets. **iter_reg** is initialized through **asc_create_iter_reg** and then used within the loop to store address offsets. **iter_reg** increments automatically in each loop iteration according to the configured step.

### Example<a name="example-4"></a>

```cpp
constexpr uint32_t one_repeat_size = 256 / sizeof(int8_t); // VL / sizeof(T)
uint32_t total_length = 256;
uint32_t repeat_time = total_length / one_repeat_size;
__simd_vf__ inline void add_vf(__ubuf__ int8_t* dst_addr, __ubuf__ int8_t* src0_addr, __ubuf__ int8_t* src1_addr, uint32_t count,
    uint32_t one_repeat_size, uint16_t repeat_time)
{
    vector_int8_t src0;
    vector_int8_t src1;
    vector_int8_t dst;
    vector_bool mask;
    iter_reg addr_reg;
    for (uint16_t i = 0; i < repeat_time; ++i) {
        addr_reg = asc_create_iter_reg_b8(one_repeat_size); // Initialize iter_reg so that the address is offset by one_repeat_size in each loop iteration.
        mask = asc_update_mask_b8(count);
        asc_loadalign(src0, src0_addr, addr_reg);
        asc_loadalign(src1, src1_addr, addr_reg);
        asc_add(dst, src0, src1, mask);
        asc_storealign(dst_addr, dst, addr_reg, mask);
    }
}
```
