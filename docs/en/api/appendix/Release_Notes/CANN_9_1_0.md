# C API

<!-- md-trans-meta sourceCommit=754fb95859dead826727a591e674261c6a829a08 translatedAt=2026-10-29T11:11:53.667Z pushedAt=2026-10-29T11:36:41.636Z nl=2 -->

CANN 9.1.0 continues to improve the C API for the Ascend 950PR&950DT products (3510 architecture), adds vector computation, data movement, scalar operation, matrix computation, synchronization control, system variable, and cache control APIs, and fixes the function naming, type declaration, and address self-update signature issues in 9.0.0.

## New Features

### Vector Computation

- `asc_bitsort` and `asc_bitsort_sync`: The scores and indices are stored in two source operands respectively. The scores are sorted in descending order, and the sorted scores and their corresponding indices are combined into a data structure stored in the destination operand.
- `asc_mrgsort4` and `asc_mrgsort4_sync`: Merge up to four already-sorted queues into one queue, with the result sorted by the score field in descending order.
- `asc_transpose` and `asc_transpose_sync`: Transpose a 16×16 two-dimensional matrix data block.
- `asc_transto5hd_b8`, `asc_transto5hd_b8_sync`, `asc_transto5hd_b16`, `asc_transto5hd_b16_sync`, `asc_transto5hd_b32`, and `asc_transto5hd_b32_sync`: Perform data format conversion, generally used to convert the NCHW format to the NC1HWC0 format, and can also be used to transpose two-dimensional matrix data blocks.
- `asc_set_va_reg`: Set the address of `asc_transto5hd_b8` and associate the operand address sequence with the address register.

### data movement

- `asc_copy_gm2ub` and `asc_copy_gm2ub_sync`: Move data from Global Memory to the Unified Buffer (UB).
- `asc_copy_ub2gm` and `asc_copy_ub2gm_sync`: Move data from the UB to Global Memory.
- `asc_copy_ub2ub` and `asc_copy_ub2ub_sync`: Move data from the UB to the UB.
- `asc_copy_ub2l1` and `asc_copy_ub2l1_sync`: Move data from the UB to the L1 Buffer.
- `asc_set_copy_pad_val`: Used together with `asc_copy_gm2ub_align` to set the data values to be padded on the left and right sides of a contiguous data block during movement. Ascend 950PR&950DT products support `int8_t`, `uint8_t`, `int16_t`, `uint16_t`, `half`, `bfloat16_t`, `int32_t`, `uint32_t`, and `float`. Data of types `fp4x2_e2m1_t`, `fp4x2_e1m2_t`, `hifloat8_t`, `fp8_e8m0_t`, `fp8_e5m2_t`, and `fp8_e4m3fn_t` must be converted to `int8_t` before being passed in.
- `asc_ndim_copy_dci`: Performs the Data Cache Invalidate (DCI) operation required for N-dimensional movement scenarios.
- `asc_copy_gm2l1`, `asc_copy_gm2l1_sync`: Move data from Global Memory to the L1 Buffer.
- `asc_copy_gm2l1_align`, `asc_copy_gm2l1_align_sync`: Pad data in Global Memory and then move it to the L1 Buffer.
- `asc_copy_gm2l1_nd2nz`, `asc_copy_gm2l1_nd2nz_sync`: Complete ND-to-NZ format conversion during the movement from Global Memory to the L1 Buffer.
- `asc_copy_gm2l1_dn2nz`, `asc_copy_gm2l1_dn2nz_sync`: Move matrix data from Global Memory to the L1 Buffer and complete DN-to-NZ, NCHW-to-NC1HWC0, or NCHW-to-C1HWNC0 format conversion. When the D dimension is not aligned to the C0 size, zeros are padded in the innermost dimension of the L1 Buffer.
- `asc_copy_l12l0a`, `asc_copy_l12l0a_sync`, `asc_copy_l12l0b`, `asc_copy_l12l0b_sync`: Move a 512-byte matrix from the L1 Buffer to the L0A Buffer or L0B Buffer, supporting 2D format movement, 2D format transposed movement, and 3D format movement.
- `asc_copy_l12l0b_trans`, `asc_copy_l12l0b_trans_sync`: Transpose a matrix from the L1 Buffer and move it to the L0B Buffer.
- `asc_copy_l12l0b_mx`, `asc_copy_l12l0b_mx_sync`: Move a Microscaling (MX) matrix from the L1 Buffer to the L0B Buffer.
- `asc_copy_l0c2gm`, `asc_copy_l0c2gm_sync`: Move data from the L0C Buffer to Global Memory, supporting quantization, ReLU, Leaky ReLU, channel split, and NZ2ND in-line control.
- `asc_copy_l0c2ub`, `asc_copy_l0c2ub_sync`: Move data from the L0C Buffer to the UB, supporting quantization, ReLU, Leaky ReLU, and channel split in-line control.
- `asc_copy_l12fb`, `asc_copy_l12fb_sync`: Move quantization parameters from the L1 Buffer to the Fixpipe Buffer.
- `asc_copy_l12bt`, `asc_copy_l12bt_sync`: Move the bias data used for matrix multiplication from the L1 Buffer to the BiasTable Buffer.
- `asc_copy_l12ub`, `asc_copy_l12ub_sync`: Move data from the L1 Buffer to the UB.
- `asc_fill_l1`, `asc_fill_l1_sync`: Assign the binary value of `value` to the destination operand to initialize the Local Memory of the L1 Buffer.
- `asc_set_l13d_fmatrix_b`: Set the feature map attribute description, which is used to configure the fill value for the 3D format movement APIs `asc_copy_l12l0a` and `asc_copy_l12l0b`. Use this API when obtaining the feature map attributes from the right matrix.
- `asc_set_l0c2gm_lrelu_alpha`: Set the Leaky ReLU alpha value used in the computation of `asc_copy_l0c2l1` or `asc_copy_l0c2gm`. This value supports only the `half` and `float` data types.
- `asc_set_l13d_padding`: Set the padding attribute description, which is used to configure the padding value when calling `asc_copy_l12l0a`.

### Scalar Operations

- `asc_clear_nthbit`, `asc_set_nthbit`: Clear or set a specified bit in a `uint64_t` value.
- `asc_atomic_add`, `asc_atomic_sub`, `asc_atomic_max`, `asc_atomic_min`, `asc_atomic_exch`, `asc_atomic_cas`: Perform scalar atomic operations on `int32_t`, `uint32_t`, `float`, `int64_t`, and `uint64_t` data in Global Memory, and return the value before the operation.
- `asc_atomic_and`, `asc_atomic_or`, `asc_atomic_xor`: Perform atomic bitwise operations on `int32_t`, `uint32_t`, `int64_t`, and `uint64_t` data in Global Memory.
- `asc_atomic_inc`: Perform an atomic increment operation on a `uint32_t` or `uint64_t` counter in Global Memory. If the counter value is greater than or equal to `val`, it is set to 0; otherwise, it is incremented by 1.
- `asc_atomic_dec`: Perform an atomic decrement operation on a `uint32_t` or `uint64_t` counter in Global Memory. If the counter value is equal to 0 or greater than `val`, it is set to `val`; otherwise, it is decremented by 1.

### Matrix Computation

- `asc_mmad`, `asc_mmad_sync`: Complete matrix multiply-accumulate operations, supporting same-precision and mixed-precision input combinations of `int8_t`, `hifloat8_t`, `fp8_e5m2_t`, `fp8_e4m3fn_t`, `half`, `bfloat16_t`, and `float`.
- `asc_enable_fp8`: Enable FP8 mode and disable HiF8 mode for matrix multiply-accumulate calculations. FP8 data in the L0A Buffer and L0B Buffer is not converted to `hifloat8_t` data before participating in matrix multiply-accumulate calculations.
- `asc_enable_hif8`: Enable HiF8 mode for matrix multiply-accumulate calculations. 8-bit data in the L0A Buffer and L0B Buffer is converted to `hifloat8_t` data before participating in matrix multiplication.
- `asc_enable_hf32_trans`: Set the rounding method for HF32 mode. Before using this API, call `asc_enable_hf32` to enable HF32 mode.
- `asc_enable_hf32`: Enable HF32 mode for matrix multiply-accumulate calculations to improve the computational performance of FP32 data, at the cost of some precision loss.
- `asc_set_fp32_mode`: Enable FP32 mode for matrix multiply-accumulate calculations. FP32 data in the L0A Buffer and L0B Buffer is not rounded before participating in matrix multiply-accumulate calculations.
- `asc_set_mmad_direction_m`, `asc_set_mmad_direction_n`: Set matrix multiply-accumulate calculations to generate results preferentially along the M direction and the N direction, respectively.

### Synchronization Control

- `asc_lock`, `asc_unlock`: Used for asynchronous pipeline synchronization within the AI Core.
- `asc_sync_intra_arrive`, `asc_sync_intra_wait`: Used for inter-core signal synchronization.
- `asc_sync_subblock_arrive`, `asc_sync_subblock_wait`: Used for signal synchronization between subblocks within a group.
- `asc_sync_inter_arrive`, `asc_sync_inter_wait`: Used for signal synchronization between computation blocks across groups.
- `asc_sync_data_barrier`: Waits for the completion of previously initiated memory accesses within the specified range.

### System Variables

- `asc_get_smmu_tag_version`, `asc_get_phy_stack_base`, `asc_get_status`, and `asc_get_sys_virtual_base`: Obtain the System Memory Management Unit (SMMU) version information, physical stack base address, status information, and system virtual base address, respectively.
- `asc_get_block_idx`: Obtain the index of the currently running core.
- `asc_get_sub_block_num`: Obtain the number of Cube Cores or Vector Cores on an AI Core in separate mode.
- `asc_get_sub_block_id`: Obtain the Vector Core ID on the AI Core.
- `asc_get_phy_buf_addr`: Obtain the actual on-chip physical address based on the offset.
- `asc_get_core_id` and `asc_get_arch_ver`: Obtain the core ID and architecture version, respectively.
- `asc_get_vf_len`: Obtain the tensor bit width VL (Vector Length).
- `asc_set_gm2l1_nz_para`: Set the value of the MTE2_NZ_PARA register.
- `asc_set_l0c2gm_channel_para`: Set the bits of the dedicated register for channel stride parameters.
- `asc_set_l0c2gm_quant_post`: Sets the value of the QUANT_POST register. Experimental API.
- `asc_set_l0c2gm_relu_alpha`: Sets the value of the RELU_ALPHA register.
- `asc_set_l12l0_padding_val`: Sets the value of the PADDING_B register.
- `asc_set_l3d_rpt_b`: Used to set the repeat parameter for 2D format movement of `asc_copy_l12l0a` and `asc_copy_l12l0b`.

### Cache Control

- `asc_dci`: Invalidates the entire data cache without writing back cache lines. Modifications that have not been synchronized to main memory are discarded. Before calling this API, you must execute `asc_sync_data_barrier(mem_dsb_t::DSB_ALL)` to wait for all preceding memory accesses to complete.

## Bug Fixes

- `asc_sync_block_arrive` is changed from `asc_sync_block_arrive(pipe, mode, flagID)` in 9.0.0 to `asc_sync_block_arrive(pipe, flag_id)`, with the `mode` parameter removed and the event label naming unified.
- `asc_sync_block_wait` is changed from `asc_sync_block_wait(flagID)` in 9.0.0 to `asc_sync_block_wait(pipe, flag_id)`, with a pipeline parameter added and the event label naming unified.
- The parameter name of `asc_get_arch_ver` is changed from `coreVersion` to `core_version`.
- Fix the issue in `cache_ctrl.h` where the atomic operation header guard macro is reused, which may cause cache control declarations to be skipped depending on the include order.
- Unify the header file include paths in `asc_simd.h`, `misc.h`, `cache_ctrl.h`, `sync.h`, `sys_var.h`, and `utils_intf.h` to prevent include failures caused by changes to the installation directory.
- Correct the UB source pointer parameters that require automatic update in `asc_loadalign_postupdate`, `asc_loadalign_brc_postupdate`, `asc_loadalign_brc_postupdate_v2`, `asc_loadalign_brc_postupdate_v3`, `asc_loadalign_upsample_postupdate`, `asc_loadalign_downsample_postupdate`, `asc_loadalign_unpack_postupdate`, `asc_loadalign_unpack_postupdate_v2`, and `asc_loadalign_deintlv_postupdate` to pointer references, ensuring that address changes after the call are visible to the caller.
- Correct the UB destination pointer parameters that require automatic update in `asc_storealign_postupdate`, `asc_storealign_1st_postupdate`, `asc_storealign_pack_postupdate`, and `asc_storealign_pack_postupdate_v2` to pointer references, ensuring that address changes after the call are visible to the caller.
- Correct the destination parameter of `asc_storeunalign`, which does not update the address, from a pointer reference to a plain pointer, and add the address-update overload for `asc_storeunalign_postupdate`.
- Fix the rounding mode naming of the APIs that convert `half` to `int4b_t`. In 9.0.0, the APIs for the following four rounding modes were declared with downward-rounding function names, and in 9.1.0 they are corrected to function names consistent with the actual rounding modes:
  - Round up: `asc_half2int4x2_ru`, `asc_half2int4x2_ru_sat`, `asc_half2int4x2_ru_v2`, `asc_half2int4x2_ru_sat_v2`, `asc_half2int4x2_ru_v3`, `asc_half2int4x2_ru_sat_v3`, `asc_half2int4x2_ru_v4`, `asc_half2int4x2_ru_sat_v4`.
  - Round toward zero: `asc_half2int4x2_rz`, `asc_half2int4x2_rz_sat`, `asc_half2int4x2_rz_v2`, `asc_half2int4x2_rz_sat_v2`, `asc_half2int4x2_rz_v3`, `asc_half2int4x2_rz_sat_v3`, `asc_half2int4x2_rz_v4`, `asc_half2int4x2_rz_sat_v4`.
  - Round to nearest even: `asc_half2int4x2_rn`, `asc_half2int4x2_rn_sat`, `asc_half2int4x2_rn_v2`, `asc_half2int4x2_rn_sat_v2`, `asc_half2int4x2_rn_v3`, `asc_half2int4x2_rn_sat_v3`, `asc_half2int4x2_rn_v4`, `asc_half2int4x2_rn_sat_v4`.
  - Round to nearest, ties away from zero: `asc_half2int4x2_rna`, `asc_half2int4x2_rna_sat`, `asc_half2int4x2_rna_v2`, `asc_half2int4x2_rna_sat_v2`, `asc_half2int4x2_rna_v3`, `asc_half2int4x2_rna_sat_v3`, `asc_half2int4x2_rna_v4`, `asc_half2int4x2_rna_sat_v4`.
- Corrected `asc_int162half_rc` to `asc_int162half_rz`.
- Corrected `asc_reduce_min_datablock_` to `asc_reduce_min_datablock`, removing the redundant trailing underscore.
- Corrected `asc_half2uint8_rd_sat_v2_impl`, `asc_half2uint8_rn_sat_v2_impl`, `asc_half2uint8_rna_sat_v2_impl`, `asc_half2uint8_ru_sat_v2_impl`, and `asc_half2uint8_rz_sat_v2_impl` to `asc_half2uint8_rd_sat_v2`, `asc_half2uint8_rn_sat_v2`, `asc_half2uint8_rna_sat_v2`, `asc_half2uint8_ru_sat_v2`, and `asc_half2uint8_rz_sat_v2`, respectively.
- Corrected the destination type of the following `bfloat16_t`-to-`fp4x2_e1m2_t` conversion APIs from `vector_f4e1m2x2` to `vector_fp4x2_e1m2_t`:
  - Round down: `asc_bfloat162e1m2x2_rd`, `asc_bfloat162e1m2x2_rd_v2`, `asc_bfloat162e1m2x2_rd_v3`, `asc_bfloat162e1m2x2_rd_v4`.
  - Round to nearest even: `asc_bfloat162e1m2x2_rn`, `asc_bfloat162e1m2x2_rn_v2`, `asc_bfloat162e1m2x2_rn_v3`, `asc_bfloat162e1m2x2_rn_v4`.
  - Round to nearest, ties away from zero: `asc_bfloat162e1m2x2_rna`, `asc_bfloat162e1m2x2_rna_v2`, `asc_bfloat162e1m2x2_rna_v3`, `asc_bfloat162e1m2x2_rna_v4`.
  - Round up: `asc_bfloat162e1m2x2_ru`, `asc_bfloat162e1m2x2_ru_v2`, `asc_bfloat162e1m2x2_ru_v3`, `asc_bfloat162e1m2x2_ru_v4`.
  - Round toward zero: `asc_bfloat162e1m2x2_rz`, `asc_bfloat162e1m2x2_rz_v2`, `asc_bfloat162e1m2x2_rz_v3`, `asc_bfloat162e1m2x2_rz_v4`.
- The non-standard type aliases `vector_u8` and `vector_f16` of `asc_half2uint8` and `asc_min_scalar` are corrected to `vector_uint8_t` and `vector_half`.
- The parameter names of `asc_arange` and `asc_arange_descend` are changed from `index` and unified as `value`.
- The parameter name of `asc_update_mask_b8`, `asc_update_mask_b16`, and `asc_update_mask_b32` is changed from `scalar` and unified to `value`.

## Enhancements

- `asc_copy_gm2ub_align`, `asc_copy_gm2ub_align_sync`, `asc_copy_ub2gm_align`, and `asc_copy_ub2gm_align_sync` add simplified overloads that take only `dst`, `src`, and `size`.
- `asc_copy_l0c2l1` adds `asc_copy_l0c2l1_sync`, and adds combinations of `int16_t` destination data with `int32_t` source data and `bfloat16_t` destination data with `float` source data, as well as generic overloads where the destination parameter is `__cbuf__ void*` and the source parameter is `__cc__ int32_t*` or `__cc__ float*`.
- `asc_copy_l12l0a_mx` adds `asc_copy_l12l0a_mx_sync`.
- `asc_set_atomic_add_int8`, `asc_set_atomic_add_int16`, and `asc_set_atomic_add_int`: set the subsequent data movement from UB and L0C Buffer to Global Memory to enable atomic accumulation, with the accumulated data types being `int8_t`, `int16_t`, and `int32_t`, respectively.
- `asc_gather` adds overloads that use `vector_uint32_t` indexing for `int16_t`, `uint16_t`, `half`, and `bfloat16_t` data.
- `asc_gather_datablock` adds an `int4b_t` overload and a `uint64_t` overload that does not require a mask parameter.
- `asc_loadalign`, `asc_loadalign_brc`, `asc_loadalign_brc_v2`, `asc_loadalign_upsample`, `asc_loadalign_downsample`, `asc_loadalign_unpack`, `asc_loadalign_unpack_v2`, and `asc_loadalign_deintlv` add `int4b_t` and `vector_int4x2_t` overloads.
- `asc_loadalign`, `asc_loadalign_upsample`, `asc_loadalign_downsample`, and `asc_loadalign_postupdate` add `vector_bool` overloads.
- `asc_storealign`, `asc_storealign_1st`, `asc_storealign_intlv`, `asc_storeunalign`, `asc_storeunalign_postupdate`, `asc_storeunalign_post`, `asc_storeunalign_post_postupdate`, and `asc_store` add `int4b_t` and `vector_int4x2_t` data overloads.
- `asc_storealign`, `asc_storealign_pack`, `asc_storealign_postupdate`, `asc_storealign_pack_postupdate`, and `asc_storeunalign_postupdate` add a `vector_bool` output overload.
- `asc_scatter` adds nine groups of overloads with the destination UB address first and the source vector register last, which scatter the source vector register elements into the UB according to the index. The supported data types are `int8_t`, `uint8_t`, `int16_t`, `uint16_t`, `half`, `bfloat16_t`, `int32_t`, `uint32_t`, and `float`.
- `asc_create_iter_reg_b8`, `asc_create_iter_reg_b16`, and `asc_create_iter_reg_b32` are extended from a single offset to support one to four offsets.
- `asc_copy` adds overloads that copy `vector_uint16_t` and `vector_uint32_t` vector data registers to a `vector_bool` mask register.
- `asc_duplicate_scalar` adds an overload that does not require a mask parameter.

## Deprecated APIs

This version marks the following APIs or specific overloads as `deprecated`:

### data movement

| deprecated interface or overload | Replacement API | Description |
| :--- | :--- | :--- |
| `asc_set_gm2ub_pad` | `asc_set_copy_pad_val` | Use the general movement padding value configuration API. |
| `asc_get_store_atomic_config` (Ascend 950PR&950DT products) | `asc_atomic_add` | Scalar atomic addition does not require pre-configuring and querying the data movement atomic state. |
| `asc_set_store_atomic_config_v2` | `asc_atomic_add` | Scalar atomic addition is changed to directly call the atomic API. |
| The `asc_scatter` overload with the source vector register first and the destination UB address last | The `asc_scatter` overload with the destination UB address first and the source vector register last | The supported data types remain unchanged; only the parameter order is adjusted. |
| `asc_exp_sub_v2(vector_float&, vector_float, vector_float, vector_bool)` | `asc_exp_sub` | The `asc_exp_sub_v2` overload for `half` input is retained for reading odd-index elements of the source operand; only the overload for `float` input is deprecated. |

**Note:** For details, see [deprecated interface](../deprecated_interface.md).
