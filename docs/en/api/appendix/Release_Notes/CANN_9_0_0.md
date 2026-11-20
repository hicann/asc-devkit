# C API

<!-- md-trans-meta sourceCommit=22e1e8c65e425d4c0fb07294311f2eac049ab0b3 translatedAt=2026-11-17T03:50:36.931Z pushedAt=2026-11-17T09:52:41.787Z nl=2 -->

CANN 9.0.0 provides, for the first time, experimental C-language extension-layer Single Instruction Multiple Data (SIMD) APIs for the Ascend 950PR&950DT products (3510 architecture). Based on pointer programming, these APIs provide complete C programming capability. They manage local memory by declaring static arrays, and you manage synchronization and memory independently. They adapt to C development habits, expose all underlying hardware capabilities, and support fine-grained tuning and ultimate performance. The APIs cover capabilities such as data structures, data movement, scalar operations, matrix computation, synchronization control, system variables, cache control, atomic operations, other operations, Reg data movement, and Reg vector computation.

## Features

### Data Structures

- `asc_set_l0c_copy_params_config`: Sets the number of ND matrices, the start address offset of adjacent source NZ matrices, and the start address offset of adjacent destination ND matrices required when converting NZ format to ND format during L0C Buffer movement.
- `asc_load3d_v2_config`: Sets the repeat parameter of the `asc_copy_l12l0a` and `asc_copy_l12l0b` APIs.
- `asc_ndim_pad_count_config`: Used by the `asc_set_ndim_pad_count` API to set the number of padding elements on the left and right sides of each dimension of the `asc_ndim_copy_gm2ub` API.
- `asc_l13d_fmatrix_config`: Sets the feature map attribute parameters of the 3D format movement API of `asc_copy_l12l0a` and `asc_copy_l12l0b`.

### data movement

- `asc_copy_gm2ub_align` and `asc_copy_gm2ub_align_sync`: Provide unaligned data movement, moving data from Global Memory to Unified Buffer (UB), supporting 8-bit, 16-bit, and 32-bit data type movement.
- `asc_copy_ub2gm_align` and `asc_copy_ub2gm_align_sync`: Move data from UB to Global Memory, supporting 8-bit, 16-bit, and 32-bit block movement.
- `asc_ndim_copy_gm2ub`: A multidimensional data movement API that moves data from Global Memory to UB, processing up to 5 dimensions, with each dimension configurable for stride independently, and supporting inline padding.
- `asc_set_gm2ub_loop_size`: Sets the number of loop iterations in the data movement process when moving data from Global Memory to UB.
- `asc_set_gm2ub_loop1_stride` and `asc_set_gm2ub_loop2_stride`: Set the stride between adjacent iteration data blocks in the inner loop and outer loop, respectively, when moving data from Global Memory to UB.
- `asc_set_ub2gm_loop_size`: Sets the iteration counts of the inner loop and outer loop when moving data from UB to Global Memory.
- `asc_set_ub2gm_loop1_stride`, `asc_set_ub2gm_loop2_stride`: When moving data from UB to Global Memory, set the stride between data blocks of adjacent iterations in the inner loop and outer loop, respectively.
- `asc_set_gm2ub_pad`: When using `asc_copy_gm2ub_align` to move data from Global Memory to UB and the source operand is not aligned, sets the data values to be padded on the left and right sides of the continuously moved data block.
- `asc_set_ndim_loop0_stride`, `asc_set_ndim_loop1_stride`, `asc_set_ndim_loop2_stride`, `asc_set_ndim_loop3_stride`, `asc_set_ndim_loop4_stride`: Used to set the element stride between the source operand and destination operand in each dimension of the `asc_ndim_copy_gm2ub` API, supporting up to 5 dimensions.
- `asc_set_ndim_pad_count`: Sets the number of padding elements on the left and right sides of each dimension of the `asc_ndim_copy_gm2ub` API.
- `asc_set_ndim_pad_value`: When the `padding_mode` parameter of the `asc_ndim_copy_gm2ub` API is `true`, sets the fixed padding value.
- `asc_set_l13d_rpt`: Used to set the repeat parameter of the `asc_copy_l12l0a` and `asc_copy_l12l0b` APIs.
- `asc_set_l13d_fmatrix`: Sets the feature map attribute description, used to configure the padding value for the 3D-format movement APIs `asc_copy_l12l0a` and `asc_copy_l12l0b`. Use this API when obtaining feature map attributes from the left matrix.
- `asc_set_l0c_copy_prequant`: When performing inline quantization during data movement, sets the scalar quantization parameters in the quantization process.
- `asc_set_l0c_copy_params`: When converting from NZ format to ND format during the process of moving data from L0C Buffer to Global Memory or L1 Buffer, sets the configuration related to format conversion.
- `asc_copy_l0c2l1`: Moves the matrix computation result from the L0C Buffer to the L1 Buffer, and can be combined with other APIs to configure various inline capabilities.
- `asc_copy_l12l0a_mx`: Moves a Microscaling (MX) matrix from the L1 Buffer to the L0A Buffer.
- `asc_set_gm2l1_loop_size`: Sets the loop count in the data movement process when moving data from Global Memory to the L1 Buffer.
- `asc_set_gm2l1_loop1_stride` and `asc_set_gm2l1_loop2_stride`: Set the gap between adjacent iteration data blocks in the inner loop and the outer loop, respectively, when moving data from Global Memory to the L1 Buffer.
- `asc_set_gm2l1_pad`: Sets the padding values on the left and right sides of a continuously moved data block when moving unaligned data from Global Memory to the L1 Buffer.

### Scalar Operations

- `asc_sflbits`: Computes the number of consecutive bits, starting from the most significant bit, that are identical to the sign bit in the binary representation of an `int64_t` value.
- `asc_ffz`, `asc_ffs`: Obtain the positions of the first 0 and the first 1, respectively, starting from the least significant bit in the binary representation of a `uint64_t` value. Returns -1 if not found.
- `asc_popc`: Obtains the number of 1s in the binary representation of a `uint64_t` value.
- `asc_store_dev`: Writes data to a Global Memory address, bypassing the data cache.

### Matrix Computation

- `asc_set_l0c2gm_config`: Sets the vector quantization parameters in the quantization process when inline quantization is performed during data movement.
- `asc_set_l0c2gm_nz2nd`: Sets the related configuration for format conversion when converting the NZ format to the ND format during data movement.
- `asc_mmad_mx` and `asc_mmad_mx_sync`: Perform matrix multiply-accumulate operations with scaling, supporting `fp4x2_e2m1_t`, `fp4x2_e1m2_t`, `fp8_e5m2_t`, and `fp8_e4m3fn_t` input combinations.

### Synchronization Control

- `asc_sync`, `asc_sync_vec`: Wait for all pipeline operations to complete.
- `asc_sync_mte2`, `asc_sync_mte3`: Wait for the PIPE_MTE2 pipeline and the PIPE_MTE3 pipeline to complete, respectively.
- `asc_sync_notify`, `asc_sync_wait`: Set a synchronization flag and wait for a synchronization flag, respectively.
- `asc_sync_pipe`: Wait for the specified pipeline operations to complete.

### System Variables

- `asc_get_ctrl` and `asc_set_ctrl`: Read and set the value of specific bits in the CTRL control register, respectively.
- `asc_get_block_num`: Obtain the number of cores configured for the current task, used for multi-core logic control within the code.
- `asc_get_system_cycle`: Obtain the current system cycle count.
- `asc_get_program_counter`: Obtain the pointer of the program counter, which records the current execution position of the program.
- `asc_get_ar_spr`: Read the value of a specified special register.

### Cache Control

- `asc_icache_preload` and `asc_get_icache_preload_status`: Preload data from the Global Memory address where the instruction resides into the corresponding cache line, and obtain the instruction cache preload status, respectively.
- `asc_datacache_preload`: Preload data from the specific Global Memory address where the source address resides into the data cache.
- `asc_dcci_single` and `asc_ub_dcci_single`: Flush the single cache line where the specified Global Memory address or UB address resides, respectively.
- `asc_dcci_entire_all`, `asc_dcci_entire_out`, and `asc_dcci_entire_atomic`: Flush all cache lines in ALL, OUT, or ATOMIC mode, respectively, to maintain cache coherence between scalar unit access and Global Memory.

### Atomic Operations

- `asc_set_atomic_add_float16` and `asc_set_atomic_add_float`: Enable atomic accumulation for subsequent data movement from UB, L0C Buffer, and L1 Buffer to Global Memory, with the accumulated data types being `half` and `float`, respectively.
- `asc_set_atomic_max_float16` and `asc_set_atomic_max_float`: Set the computation result to be moved to Global Memory in an atomic comparison manner, comparing the `half` and `float` data to be moved with the existing data in Global Memory element by element, and writing the maximum value to Global Memory.
- `asc_set_store_atomic_config_v2`: Set the atomic operation enable bit and the value of the atomic operation type.

### Reg Data Movement

Provides Reg data movement APIs that use the `__simd_callee__` qualifier.

**Mask read:**

- `asc_get_mask_spr_b16`, `asc_get_mask_spr_b32`: Read the mask value from the Special Purpose Register (SPR) and return it after formatting it according to the data type.

**Data move-in:**

- `asc_loadalign`: Aligned data movement API that moves data from UB with continuous alignment into the destination operand, implementing the NORM move-in mode to move one vector length (VL) of data.
- `asc_loadalign_brc`: Aligned data movement API that moves an 8-bit, 16-bit, or 32-bit value from UB with continuous alignment and broadcasts it to all element positions of the destination operand.
- `asc_loadalign_brc_v2`: Aligned data movement API that moves a 32-byte data block (DataBlock) from UB with continuous alignment and broadcasts it to one vector length.
- `asc_loadalign_brc_v3`: Aligned data movement API that moves data from UB with continuous alignment and broadcasts each 16-bit or 32-bit element to a 32-byte data block.
- `asc_loadalign_unpack`: Aligned data movement API that moves data from UB with continuous alignment into the destination operand, implementing the unpack move-in mode: loads half a vector length of data as 8-bit, 16-bit, or 32-bit unsigned integers, expands it to 16-bit, 32-bit, or 64-bit data of one vector length, and pads the intermediate positions with 0.
- `asc_loadalign_unpack_v2`: Aligned data movement API that moves continuously aligned data from UB into the destination operand, implementing the decompression move-in mode: loads one quarter of a vector length of data as 8-bit unsigned integers, expands them to 32-bit data of one vector length, and pads the intermediate positions with 0.
- `asc_loadalign_upsample`: Aligned data movement API that moves continuously aligned data from UB into the destination operand, implementing the 2x upsampling move-in mode: loads half a vector length of data, with each input element repeated twice.
- `asc_loadalign_downsample`: Aligned data movement API that moves continuously aligned data from UB into the destination operand, implementing the 2x downsampling move-in mode: loads 2x vector length of data, keeping every other element.
- `asc_loadalign_deintlv`: Reads 2x vector length of continuous data starting from a 32-byte-aligned address in UB, completes deinterleaving during loading, and writes the results into two destination vector data registers.
- `asc_loadalign_postupdate`, `asc_loadalign_brc_postupdate`, `asc_loadalign_brc_postupdate_v2`, `asc_loadalign_brc_postupdate_v3`: Implement the corresponding aligned move-in modes and enable Post Update. The source operand address is automatically updated after the API call.
- `asc_loadalign_unpack_postupdate`, `asc_loadalign_unpack_postupdate_v2`, `asc_loadalign_upsample_postupdate`, `asc_loadalign_downsample_postupdate`, `asc_loadalign_deintlv_postupdate`: Implement the corresponding decompression, 2x upsampling, 2x downsampling, or deinterleaving move-in modes and enable Post Update. The source operand address is automatically updated after the API call.
- `asc_loadunalign_pre`: Used for initialization before unaligned data move-in. It must be used together with `asc_loadunalign` or `asc_loadunalign_postupdate`.
- `asc_loadunalign`, `asc_loadunalign_postupdate`: Suitable for scenarios where data is continuously moved from a non-32-byte-aligned address in UB into a vector data register. The latter supports automatic update of the source operand address.
- `asc_load`: Supports moving data from a non-32-byte-aligned source address in UB to a vector data register, with a movement amount of one vector length.
- `asc_gather`: Gathers the source operand into the destination operand element by element according to index positions.
- `asc_gather_datablock`: Given the base address and index of the source operand in UB, collects the source operand into the destination operand in 32-byte data blocks according to the index positions.

**Data store-out:**

- `asc_storealign`: Stores data from a vector data register or mask register to UB with continuous alignment, implementing the NORM store-out mode and moving one vector length of data.
- `asc_storealign_1st`: Stores the first element from a vector data register to UB, ignoring the mask.
- `asc_storealign_pack`: Performs compressed store-out from a vector data register to UB, continuously storing the lower half of the bits of the valid elements in the source operand into the destination operand according to the mask.
- `asc_storealign_pack_v2`: Performs compressed store-out from a vector data register to UB, continuously storing the lower 8 bits of the valid elements in the source operand into the destination operand according to the mask.
- `asc_storealign_intlv`: Performs interleaved store-out from a vector data register to UB, ignoring the mask and storing the elements of two source operands into the destination operand in an interleaved manner.
- `asc_storealign_postupdate`, `asc_storealign_1st_postupdate`: Implement the NORM store-out mode or store out the first element, and enable Post Update. The destination operand address is automatically updated after the API call.
- `asc_storealign_pack_postupdate`, `asc_storealign_pack_postupdate_v2`: Implement compressed store-out of the lower half of the bits or the lower 8 bits, and enable Post Update. The destination operand address is automatically updated after the API call.
- `asc_storeunalign`, `asc_storeunalign_postupdate`: Apply to scenarios where data is continuously stored from a vector data register to a UB address that is not 32-byte aligned; the latter supports automatic update of the destination operand address.
- `asc_storeunalign_post`, `asc_storeunalign_post_postupdate`: Suitable for the tail block scenario where data is continuously stored from vector data registers to a UB non-32-byte-aligned address; the latter supports automatic update of the destination operand address.
- `asc_scatter`: Scatters the elements in the source operand into the destination operand based on index values.
- `asc_store`: Suitable for the scenario where data is stored from vector data registers to UB, regardless of alignment; for aligned scenarios, `asc_storealign` is recommended.

### Reg Vector Calculation

Provides Reg vector computation APIs using the `__simd_callee__` qualifier. Based on API capabilities, the supported data types include `bool`, `int4b_t`, `int8_t`, `uint8_t`, `fp4x2_e2m1_t`, `fp4x2_e1m2_t`, `hifloat8_t`, `fp8_e8m0_t`, `fp8_e5m2_t`, `fp8_e4m3fn_t`, `int16_t`, `uint16_t`, `half`, `bfloat16_t`, `int32_t`, `uint32_t`, `float`, `int64_t`, and `uint64_t`. For the specific supported combinations, refer to the description of each API.

**Registers and masks:**

- `asc_create_iter_reg_b8`, `asc_create_iter_reg_b16`, `asc_create_iter_reg_b32`: Initialize address registers, which store address offsets in loops.
- `asc_create_mask_b8`, `asc_create_mask_b16`, `asc_create_mask_b32`: Create mask registers. The `pat_mode` parameter specifies which elements participate in computation.
- `asc_update_mask_b8`, `asc_update_mask_b16`, `asc_update_mask_b32`: Generate masks based on the element count, and automatically subtract the element count of the current vector processing unit from the element count.
- `asc_copy`: Performs data movement between registers.

**Computation capabilities:**

- Basic arithmetic: `asc_add`, `asc_addc`, `asc_add_scalar`, `asc_sub`, `asc_subc`, `asc_mul`, `asc_mul_scalar`, `asc_madd`, `asc_mull`, `asc_max`, `asc_max_scalar`, `asc_min`, `asc_min_scalar`, `asc_abs`, `asc_neg`, `asc_abs_sub`, `asc_axpy`, `asc_relu`, `asc_leakyrelu`, `asc_prelu`, `asc_exp`, `asc_ln`, `asc_sqrt`, `asc_div`.
- Logic and shift: `asc_and`, `asc_or`, `asc_xor`, `asc_not`, `asc_shiftleft`, `asc_shiftleft_scalar`, `asc_shiftright`, `asc_shiftright_scalar`.
- Comparison and selection: `asc_eq`, `asc_ne`, `asc_lt`, `asc_le`, `asc_gt`, `asc_ge`, `asc_eq_scalar`, `asc_ne_scalar`, `asc_lt_scalar`, `asc_le_scalar`, `asc_gt_scalar`, `asc_ge_scalar`, `asc_select`, `asc_squeeze`, `asc_squeeze_v2`.
- The type conversion APIs are grouped as follows. Functions in the same group provide round-down, round-up, round-toward-zero, round-to-nearest-even, or round-to-nearest-away-from-zero rounding modes, as well as saturated or non-saturated modes, depending on their capability.
  - Conversion of `int4b_t`, `fp4x2_e2m1_t`, and `fp4x2_e1m2_t`:
    - Conversion from `half` to `int4b_t`: `asc_half2int4x2_rd`, `asc_half2int4x2_rd_sat`, `asc_half2int4x2_rd_v2`, `asc_half2int4x2_rd_sat_v2`, `asc_half2int4x2_rd_v3`, `asc_half2int4x2_rd_sat_v3`, `asc_half2int4x2_rd_v4`, `asc_half2int4x2_rd_sat_v4`. In the 9.0.0 public header file, the APIs for other rounding modes incorrectly reuse the round-down function names listed above.
    - Conversion from `int4b_t` to other types: `asc_int4x22half`, `asc_int4x22bfloat16`, `asc_int4x22int16`.
    - Conversion from `bfloat16_t` to `fp4x2_e2m1_t`: `asc_bfloat162e2m1x2_rd`, `asc_bfloat162e2m1x2_rd_v2`, `asc_bfloat162e2m1x2_rd_v3`, `asc_bfloat162e2m1x2_rd_v4`, `asc_bfloat162e2m1x2_rn`, `asc_bfloat162e2m1x2_rn_v2`, `asc_bfloat162e2m1x2_rn_v3`, `asc_bfloat162e2m1x2_rn_v4`, `asc_bfloat162e2m1x2_rna`, `asc_bfloat162e2m1x2_rna_v2`, `asc_bfloat162e2m1x2_rna_v3`, `asc_bfloat162e2m1x2_rna_v4`, `asc_bfloat162e2m1x2_ru`, `asc_bfloat162e2m1x2_ru_v2`, `asc_bfloat162e2m1x2_ru_v3`, `asc_bfloat162e2m1x2_ru_v4`, `asc_bfloat162e2m1x2_rz`, `asc_bfloat162e2m1x2_rz_v2`, `asc_bfloat162e2m1x2_rz_v3`, `asc_bfloat162e2m1x2_rz_v4`.
    - Conversion from `fp4x2_e2m1_t` to `bfloat16_t`: `asc_e2m1x22bfloat16`.
    - Conversion from `bfloat16_t` to `fp4x2_e1m2_t`: `asc_bfloat162e1m2x2_rd`, `asc_bfloat162e1m2x2_rd_v2`, `asc_bfloat162e1m2x2_rd_v3`, `asc_bfloat162e1m2x2_rd_v4`, `asc_bfloat162e1m2x2_rn`, `asc_bfloat162e1m2x2_rn_v2`, `asc_bfloat162e1m2x2_rn_v3`, `asc_bfloat162e1m2x2_rn_v4`, `asc_bfloat162e1m2x2_rna`, `asc_bfloat162e1m2x2_rna_v2`, `asc_bfloat162e1m2x2_rna_v3`, `asc_bfloat162e1m2x2_rna_v4`, `asc_bfloat162e1m2x2_ru`, `asc_bfloat162e1m2x2_ru_v2`, `asc_bfloat162e1m2x2_ru_v3`, `asc_bfloat162e1m2x2_ru_v4`, `asc_bfloat162e1m2x2_rz`, `asc_bfloat162e1m2x2_rz_v2`, `asc_bfloat162e1m2x2_rz_v3`, `asc_bfloat162e1m2x2_rz_v4`.
    - Conversion from `fp4x2_e1m2_t` to `bfloat16_t`: `asc_e1m2x22bfloat16`.
  - Conversion of `hifloat8_t`, `fp8_e5m2_t`, and `fp8_e4m3fn_t`:
    - `hifloat8_t` conversion: `asc_float2hif8_rh`, `asc_float2hif8_rh_sat`, `asc_float2hif8_rh_v2`, `asc_float2hif8_rh_sat_v2`, `asc_float2hif8_rh_v3`, `asc_float2hif8_rh_sat_v3`, `asc_float2hif8_rh_v4`, `asc_float2hif8_rh_sat_v4`, `asc_float2hif8_rna`, `asc_float2hif8_rna_sat`, `asc_float2hif8_rna_v2`, `asc_float2hif8_rna_sat_v2`, `asc_float2hif8_rna_v3`, `asc_float2hif8_rna_sat_v3`, `asc_float2hif8_rna_v4`, `asc_float2hif8_rna_sat_v4`, `asc_half2hif8_rh`, `asc_half2hif8_rh_sat`, `asc_half2hif8_rh_v2`, `asc_half2hif8_rh_sat_v2`, `asc_half2hif8_rna`, `asc_half2hif8_rna_sat`, `asc_half2hif8_rna_v2`, `asc_half2hif8_rna_sat_v2`, `asc_hif82half`.
    - `fp8_e5m2_t` conversion: `asc_float2e5m2_rn`, `asc_float2e5m2_rn_sat`, `asc_float2e5m2_rn_v2`, `asc_float2e5m2_rn_sat_v2`, `asc_float2e5m2_rn_v3`, `asc_float2e5m2_rn_sat_v3`, `asc_float2e5m2_rn_v4`, `asc_float2e5m2_rn_sat_v4`, `asc_e5m22float`.
    - `fp8_e4m3fn_t` conversion: `asc_float2e4m3_rn`, `asc_float2e4m3_rn_sat`, `asc_float2e4m3_rn_v2`, `asc_float2e4m3_rn_sat_v2`, `asc_float2e4m3_rn_v3`, `asc_float2e4m3_rn_sat_v3`, `asc_float2e4m3_rn_v4`, `asc_float2e4m3_rn_sat_v4`, `asc_e4m32float`.
    - `half` to `bfloat16_t` conversion: `asc_half2bfloat16_rd`, `asc_half2bfloat16_rn`, `asc_half2bfloat16_rna`, `asc_half2bfloat16_ru`, `asc_half2bfloat16_rz`.
    - `float` to `bfloat16_t` conversion: `asc_float2bfloat16_rd`, `asc_float2bfloat16_rd_sat`, `asc_float2bfloat16_rd_v2`, `asc_float2bfloat16_rd_sat_v2`, `asc_float2bfloat16_rd_sync`, `asc_float2bfloat16_rn`, `asc_float2bfloat16_rn_sat`, `asc_float2bfloat16_rn_v2`, `asc_float2bfloat16_rn_sat_v2`, `asc_float2bfloat16_rn_sync`, `asc_float2bfloat16_rna`, `asc_float2bfloat16_rna_sat`, `asc_float2bfloat16_rna_v2`, `asc_float2bfloat16_rna_sat_v2`, `asc_float2bfloat16_rna_sync`, `asc_float2bfloat16_ru`, `asc_float2bfloat16_ru_sat`, `asc_float2bfloat16_ru_v2`, `asc_float2bfloat16_ru_sat_v2`, `asc_float2bfloat16_ru_sync`, `asc_float2bfloat16_rz`, `asc_float2bfloat16_rz_sat`, `asc_float2bfloat16_rz_v2`, `asc_float2bfloat16_rz_sat_v2`, `asc_float2bfloat16_rz_sync`.
  - Other conversions between `half`, `bfloat16_t`, and `float`: `asc_half2float`, `asc_bfloat162half`, `asc_bfloat162float`, `asc_float2half`.
    - `int8_t` to `half` conversion: `asc_int82half`.
    - `uint8_t` to `half` conversion: `asc_uint82half`.
    - `int16_t` to floating-point type conversion: `asc_int162half`, `asc_int162float`.
    - `half` to 8-bit integer conversion: `asc_half2int8`, `asc_half2uint8`.
    - Convert `half` to `int16_t`: `asc_half2int16_rd`, `asc_half2int16_rd_sat`, `asc_half2int16_rd_sync`, `asc_half2int16_rn`, `asc_half2int16_rn_sat`, `asc_half2int16_rn_sync`, `asc_half2int16_rna`, `asc_half2int16_rna_sat`, `asc_half2int16_rna_sync`, `asc_half2int16_ru`, `asc_half2int16_ru_sat`, `asc_half2int16_ru_sync`, `asc_half2int16_rz`, `asc_half2int16_rz_sat`, `asc_half2int16_rz_sync`.
    - Convert `half` to `int32_t`: `asc_half2int32_rd`, `asc_half2int32_rd_v2`, `asc_half2int32_rd_sync`, `asc_half2int32_rn`, `asc_half2int32_rn_v2`, `asc_half2int32_rn_sync`, `asc_half2int32_rna`, `asc_half2int32_rna_v2`, `asc_half2int32_rna_sync`, `asc_half2int32_ru`, `asc_half2int32_ru_v2`, `asc_half2int32_ru_sync`, `asc_half2int32_rz`, `asc_half2int32_rz_v2`, `asc_half2int32_rz_sync`.
    - Convert `int32_t` to `float`: `asc_int322float`.
    - Convert `float` to `int16_t`: `asc_float2int16_rd`, `asc_float2int16_rd_sat`, `asc_float2int16_rd_v2`, `asc_float2int16_rd_sat_v2`, `asc_float2int16_rd_sync`, `asc_float2int16_rn`, `asc_float2int16_rn_sat`, `asc_float2int16_rn_v2`, `asc_float2int16_rn_sat_v2`, `asc_float2int16_rn_sync`, `asc_float2int16_rna`, `asc_float2int16_rna_sat`, `asc_float2int16_rna_v2`, `asc_float2int16_rna_sat_v2`, `asc_float2int16_rna_sync`, `asc_float2int16_ru`, `asc_float2int16_ru_sat`, `asc_float2int16_ru_v2`, `asc_float2int16_ru_sat_v2`, `asc_float2int16_ru_sync`, `asc_float2int16_rz`, `asc_float2int16_rz_sat`, `asc_float2int16_rz_v2`, `asc_float2int16_rz_sat_v2`, `asc_float2int16_rz_sync`.
    - Convert `float` to `int32_t`: `asc_float2int32_rd`, `asc_float2int32_rd_sat`, `asc_float2int32_rd_sync`, `asc_float2int32_rn`, `asc_float2int32_rn_sat`, `asc_float2int32_rn_sync`, `asc_float2int32_rna`, `asc_float2int32_rna_sat`, `asc_float2int32_rna_sync`, `asc_float2int32_ru`, `asc_float2int32_ru_sat`, `asc_float2int32_ru_sync`, `asc_float2int32_rz`, `asc_float2int32_rz_sat`, `asc_float2int32_rz_sync`.
    - Convert `float` to `int64_t`: `asc_float2int64_rd`, `asc_float2int64_rd_sat`, `asc_float2int64_rd_v2`, `asc_float2int64_rd_sat_v2`, `asc_float2int64_rd_sync`, `asc_float2int64_rn`, `asc_float2int64_rn_sat`, `asc_float2int64_rn_v2`, `asc_float2int64_rn_sat_v2`, `asc_float2int64_rn_sync`, `asc_float2int64_rna`, `asc_float2int64_rna_sat`, `asc_float2int64_rna_v2`, `asc_float2int64_rna_sat_v2`, `asc_float2int64_rna_sync`, `asc_float2int64_ru`, `asc_float2int64_ru_sat`, `asc_float2int64_ru_v2`, `asc_float2int64_ru_sat_v2`, `asc_float2int64_ru_sync`, `asc_float2int64_rz`, `asc_float2int64_rz_sat`, `asc_float2int64_rz_v2`, `asc_float2int64_rz_sat_v2`, `asc_float2int64_rz_sync`.
    - Convert `int64_t` to `float`: `asc_int642float_rd`, `asc_int642float_rd_v2`, `asc_int642float_rd_sync`, `asc_int642float_rn`, `asc_int642float_rn_v2`, `asc_int642float_rn_sync`, `asc_int642float_rna`, `asc_int642float_rna_v2`, `asc_int642float_rna_sync`, `asc_int642float_ru`, `asc_int642float_ru_v2`, `asc_int642float_ru_sync`, `asc_int642float_rz`, `asc_int642float_rz_v2`, `asc_int642float_rz_sync`.
  - Integer bit-width and sign conversion: `asc_int82int16`, `asc_int82int32`, `asc_uint82uint16`, `asc_uint82uint32`, `asc_int162uint8`, `asc_int162int32`, `asc_int162uint32`, `asc_uint162uint8`, `asc_uint162uint32`, `asc_int322uint8`, `asc_int322int16`, `asc_int322uint16`, `asc_int322int64`, `asc_uint322uint8`, `asc_uint322int16`, `asc_uint322uint16`, `asc_int642int32`.
  - Floating-point rounding: `asc_ceil`, `asc_floor`, `asc_rint`, `asc_round`, `asc_trunc`. The result retains the source floating-point data type.
- Reduction: `asc_reduce_max`, `asc_reduce_min`, `asc_reduce_sum`, `asc_reduce_max_datablock`, `asc_reduce_min_datablock`, `asc_reduce_sum_datablock`, `asc_pair_reduce_sum`.
- Data compression: `asc_pack` and `asc_pack_v2` write the low 8 bits, low 16 bits, or low 32 bits of the source operand elements into the low half or high half of the destination operand.
- Bit-width expansion: `asc_unpack_lower` and `asc_unpack_upper` select the elements from the low half and high half of the source operand, respectively, and expand their bit width. For unsigned integers, the high bits are filled with 0; for signed integers, the sign bit is extended; for the `bool` type, each bit is expanded into 2 bits with the high bit filled with 0.
- Mask decompression: `asc_unsqueeze` sets element 0 of the destination operand to 0, and sets element i to the number of 1s from element 0 to element i-1 in the mask.
- Fill and rearrangement: `asc_duplicate`, `asc_duplicate_scalar`, `asc_intlv`, and `asc_deintlv`.
- Cumulative histogram: `asc_cumulative_histogram_bin0` and `asc_cumulative_histogram_bin1` perform cumulative statistics on histogram data, counting the data distribution in the low-bit interval and high-bit interval of the source operand, respectively.
- Frequency histogram: `asc_frequency_histogram_bin0` and `asc_frequency_histogram_bin1` perform frequency statistics on the input data, counting the low-bit interval and high-bit interval, respectively, with the statistical results accumulated on top of the existing data in the destination operand.
- Index generation: `asc_arange` and `asc_arange_descend` use the passed-in value as the starting value to generate ascending and descending indices, and store the generated indices in the destination operand.
- `asc_mem_bar`: A synchronization instruction between different pipelines within a Reg computation macro function. The destination pipeline waits for all instructions on the source pipeline to complete before executing.
- `asc_clear_ar_spr`: Clears the AR special register.

### Other Operations

- `asc_init`: Initializes the Neural-network Processing Unit (NPU) state.
