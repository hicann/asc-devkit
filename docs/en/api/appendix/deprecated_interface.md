# Deprecated APIs<a name="ZH-CN_TOPIC_0000001724165365"></a>

<!-- md-trans-meta sourceCommit=28b6cff9e78c6de0dfa0a7b19c256fb6ad06a312 translatedAt=2026-10-29T10:36:44.723Z pushedAt=2026-10-29T11:30:05.704Z nl=2 -->

## Basic API

- [WriteGmByPassDCache\(ISASI\)](../SIMD-API/basic_api/deprecated_interface/WriteGmByPassDCache_ISASI_deprecated.md) API

    This API will be deprecated in subsequent versions. Please use the [WriteGmBypassDCache\(ISASI\)](../SIMD-API/basic_api/scalar_compute/WriteGmBypassDCache_ISASI.md) API instead.

- [ReadGmByPassDCache\(ISASI\)](../SIMD-API/basic_api/deprecated_interface/ReadGmByPassDCache_ISASI_deprecated.md) API

    This API will be deprecated in subsequent versions. Please use the [ReadGmBypassDCache\(ISASI\)](../SIMD-API/basic_api/scalar_compute/ReadGmBypassDCache_ISASI.md) API instead.

## C API

### Atomic Operations

<!-- npu="950" id1 -->
- [asc_set_store_atomic_config_v2](../SIMD-API/c_api/deprecated_interface/asc_set_store_atomic_config_v2_deprecated.md) API

    This API will be deprecated in subsequent versions. Please use the [asc_atomic_add](../SIMD-API/c_api/atomic/scalar_atomic/asc_atomic_add.md) API.

- [asc_get_store_atomic_config](../SIMD-API/c_api/atomic/scalar_atomic/asc_get_store_atomic_config.md) API (only for Ascend 950PR&950DT products)

    This API is deprecated on Ascend 950PR&950DT products. Please use the [asc_atomic_add](../SIMD-API/c_api/atomic/scalar_atomic/asc_atomic_add.md) API.
<!-- end id1 -->

- [asc_set_atomic_none](../SIMD-API/c_api/deprecated_interface/asc_set_atomic_none_deprecated.md) API

    This API will be deprecated in subsequent versions. Please use the [asc_disable_dma_atomic](../SIMD-API/c_api/atomic/datamove_atomic/asc_disable_dma_atomic.md) API.
- [asc_set_atomic_add_int](../SIMD-API/c_api/deprecated_interface/asc_set_atomic_add_int_deprecated.md) API

    This API will be deprecated in subsequent versions. Please use the [asc_set_atomic_add_int32](../SIMD-API/c_api/atomic/datamove_atomic/asc_set_atomic_add.md) API.

- [asc_set_atomic_max_int](../SIMD-API/c_api/deprecated_interface/asc_set_atomic_max_int_deprecated.md) API

    This API will be deprecated in subsequent versions. Please use the [asc_set_atomic_max_int32](../SIMD-API/c_api/atomic/datamove_atomic/asc_set_atomic_max.md) API.

- [asc_set_atomic_min_int](../SIMD-API/c_api/deprecated_interface/asc_set_atomic_min_int_deprecated.md) API

    This API will be deprecated in subsequent versions. Please use the [asc_set_atomic_min_int32](../SIMD-API/c_api/atomic/datamove_atomic/asc_set_atomic_min.md) API.

<!-- npu="950" id2 -->
### System Variables

- [asc_get_block_idx](../SIMD-API/c_api/deprecated_interface/asc_get_block_idx_deprecated.md) API

    This API will be deprecated in subsequent versions. Please use the built-in variable [block_idx](../../guide/programming_guide/language_extension/simd_builtin_keywords.md#built-in-variables) together with the [asc_get_sub_block_num](../SIMD-API/c_api/utils/sys_var/asc_get_sub_block_num.md) and [asc_get_sub_block_id](../SIMD-API/c_api/utils/sys_var/asc_get_sub_block_id.md) APIs to compute the logical core index.

- [asc_get_block_num](../SIMD-API/c_api/deprecated_interface/asc_get_block_num_deprecated.md) API

    This API will be deprecated in subsequent versions. Please use the [built-in variable block_num](../../guide/programming_guide/language_extension/simd_builtin_keywords.md#built-in-variables).

### Special Register Access

- [asc_get_ar_spr](../SIMD-API/c_api/deprecated_interface/asc_get_ar_spr_deprecated.md) API

    This API will be deprecated in subsequent versions. Please use the [asc_get_squeeze_status](../SIMD-API/c_api/spr/asc_get_squeeze_status.md) API.

<!-- end id2 -->

<!-- npu="950" id3 -->
### Reg Vector Calculation

#### Reg Data Load

- [asc_loadalign_brc_v2](../SIMD-API/c_api/deprecated_interface/asc_loadalign_v1_deprecated.md) API (no offset prototype)

    This API will be deprecated in subsequent versions. Please use the [asc_loadalign_brc_datablock](../SIMD-API/c_api/reg_compute/load/asc_loadalign_brc_datablock.md) API.

- [asc_loadalign_brc_v3](../SIMD-API/c_api/deprecated_interface/asc_loadalign_v1_deprecated.md) API (no offset prototype)

    This API will be deprecated in subsequent versions. Please use the [asc_loadalign_brc_elem2datablock](../SIMD-API/c_api/reg_compute/load/asc_loadalign_brc_elem2datablock.md) API.

- [asc_loadalign_unpack_v2](../SIMD-API/c_api/deprecated_interface/asc_loadalign_v1_deprecated.md) API (no offset prototype)

    This API will be deprecated in subsequent versions. Please use the [asc_loadalign_unpack4](../SIMD-API/c_api/reg_compute/load/asc_loadalign_unpack4.md) API.

- [asc_loadalign_brc_v2](../SIMD-API/c_api/deprecated_interface/asc_loadalign_v2_deprecated.md) API (addr_reg offset prototype)

    This API will be deprecated in subsequent versions. Please use the [asc_loadalign_brc_datablock](../SIMD-API/c_api/reg_compute/load/asc_loadalign_brc_datablock.md) API.

- [asc_loadalign_brc_v3](../SIMD-API/c_api/deprecated_interface/asc_loadalign_v2_deprecated.md) API (addr_reg offset prototype)

    This API will be deprecated in subsequent versions. Please use the [asc_loadalign_brc_elem2datablock](../SIMD-API/c_api/reg_compute/load/asc_loadalign_brc_elem2datablock.md) API instead.

- [asc_loadalign_unpack_v2](../SIMD-API/c_api/deprecated_interface/asc_loadalign_v2_deprecated.md) API (addr_reg offset prototype)

    This API will be deprecated in subsequent versions. Please use the [asc_loadalign_unpack4](../SIMD-API/c_api/reg_compute/load/asc_loadalign_unpack4.md) API instead.

- [asc_loadalign_brc_v2](../SIMD-API/c_api/deprecated_interface/asc_loadalign_v3_deprecated.md) API (int32_t offset prototype)

    This API will be deprecated in subsequent versions. Please use the [asc_loadalign_brc_datablock](../SIMD-API/c_api/reg_compute/load/asc_loadalign_brc_datablock.md) API instead.

- [asc_loadalign_brc_v3](../SIMD-API/c_api/deprecated_interface/asc_loadalign_v3_deprecated.md) API (int32_t offset prototype)

    This API will be deprecated in subsequent versions. Please use the [asc_loadalign_brc_elem2datablock](../SIMD-API/c_api/reg_compute/load/asc_loadalign_brc_elem2datablock.md) API instead.

- [asc_loadalign_unpack_v2](../SIMD-API/c_api/deprecated_interface/asc_loadalign_v3_deprecated.md) API (int32_t offset prototype)

    This API will be deprecated in subsequent versions. Please use the [asc_loadalign_unpack4](../SIMD-API/c_api/reg_compute/load/asc_loadalign_unpack4.md) API instead.

- [asc_loadalign_brc_postupdate_v2](../SIMD-API/c_api/deprecated_interface/asc_loadalign_v4_deprecated.md) API (int32_t offset Post Update prototype)

    This API will be deprecated in subsequent versions. Please use the [asc_loadalign_brc_datablock_postupdate](../SIMD-API/c_api/reg_compute/load/asc_loadalign_brc_datablock_postupdate.md) API.

- [asc_loadalign_brc_postupdate_v3](../SIMD-API/c_api/deprecated_interface/asc_loadalign_v4_deprecated.md) API (int32_t offset Post Update prototype)

    This API will be deprecated in subsequent versions. Please use the [asc_loadalign_brc_elem2datablock_postupdate](../SIMD-API/c_api/reg_compute/load/asc_loadalign_brc_elem2datablock_postupdate.md) API.

- [asc_loadalign_unpack_postupdate_v2](../SIMD-API/c_api/deprecated_interface/asc_loadalign_v4_deprecated.md) API (int32_t offset Post Update prototype)

    This API will be deprecated in subsequent versions. Please use the [asc_loadalign_unpack4_postupdate](../SIMD-API/c_api/reg_compute/load/asc_loadalign_unpack4_postupdate.md) API.

- [asc_loadalign (multi-mode overload)](../SIMD-API/c_api/reg_compute/load/asc_loadalign.md) API

    The original `asc_loadalign(dst, src)` implemented multiple load modes, including NORM/BRC/UP2X/DOWN2X/UNPACK/BRCV2/BRCV3/DEINTLV, through function-name overloading. It is now narrowed down to only the NORM mode, and the other modes have been split into independent functions. Please use the new version of [asc_loadalign](../SIMD-API/c_api/reg_compute/load/asc_loadalign.md).

- [asc_loadalign (deintlv mode)](../SIMD-API/c_api/reg_compute/load/asc_loadalign_deintlv.md) API

    One of the overloads of the original asc_loadalign, which has been split into the independent function [asc_loadalign_deintlv](../SIMD-API/c_api/reg_compute/load/asc_loadalign_deintlv.md).

- [asc_loadalign (deintlv+postupdate mode)](../SIMD-API/c_api/reg_compute/load/asc_loadalign_deintlv_postupdate.md) API

    One of the original asc_loadalign overloads, which has been split into the independent function [asc_loadalign_deintlv_postupdate](../SIMD-API/c_api/reg_compute/load/asc_loadalign_deintlv_postupdate.md).

- [asc_loadalign (brc_datablock mode)](../SIMD-API/c_api/reg_compute/load/asc_loadalign_brc_datablock.md) API

    One of the original asc_loadalign overloads, which has been split into the independent function [asc_loadalign_brc_datablock](../SIMD-API/c_api/reg_compute/load/asc_loadalign_brc_datablock.md).

- [asc_loadalign (brc_elem mode)](../SIMD-API/c_api/reg_compute/load/asc_loadalign_brc_elem.md) API

    One of the original asc_loadalign overloads, which has been split into the independent function [asc_loadalign_brc_elem](../SIMD-API/c_api/reg_compute/load/asc_loadalign_brc_elem.md).

- [asc_loadalign (brc_elem2datablock mode)](../SIMD-API/c_api/reg_compute/load/asc_loadalign_brc_elem2datablock.md) API

    One of the original asc_loadalign overloads, which has been split into the independent function [asc_loadalign_brc_elem2datablock](../SIMD-API/c_api/reg_compute/load/asc_loadalign_brc_elem2datablock.md).

- [asc_loadalign (downsample mode)](../SIMD-API/c_api/reg_compute/load/asc_loadalign_downsample.md) API

    One of the original asc_loadalign overloads, which has been split into the independent function [asc_loadalign_downsample](../SIMD-API/c_api/reg_compute/load/asc_loadalign_downsample.md).

- [asc_loadalign (unpack mode)](../SIMD-API/c_api/reg_compute/load/asc_loadalign_unpack.md) API

    One of the original asc_loadalign overloads, which has been split into the independent function [asc_loadalign_unpack](../SIMD-API/c_api/reg_compute/load/asc_loadalign_unpack.md).

- [asc_loadalign (unpack4 mode)](../SIMD-API/c_api/reg_compute/load/asc_loadalign_unpack4.md) API

    One of the original asc_loadalign overloads, which has been split into the independent function [asc_loadalign_unpack4](../SIMD-API/c_api/reg_compute/load/asc_loadalign_unpack4.md).

- [asc_loadalign (upsample mode)](../SIMD-API/c_api/reg_compute/load/asc_loadalign_upsample.md) API

    One of the original asc_loadalign overloads, which has been split into the independent function [asc_loadalign_upsample](../SIMD-API/c_api/reg_compute/load/asc_loadalign_upsample.md).

#### Reg Data Store

- [asc_storealign_pack_v2](../SIMD-API/c_api/deprecated_interface/asc_storealign_v1_deprecated.md) API (no offset prototype)

    This API will be deprecated in subsequent versions. Please use the [asc_storealign_pack_quarter](../SIMD-API/c_api/reg_compute/store/asc_storealign_pack_quarter.md) API.

- [asc_storealign_pack_v2](../SIMD-API/c_api/deprecated_interface/asc_storealign_v2_deprecated.md) API (addr_reg offset prototype)

    This API will be deprecated in subsequent versions. Please use the [asc_storealign_pack_quarter](../SIMD-API/c_api/reg_compute/store/asc_storealign_pack_quarter.md) API.

- [asc_storealign_pack_v2](../SIMD-API/c_api/deprecated_interface/asc_storealign_v3_deprecated.md) API (int32_t offset prototype)

    This API will be deprecated in subsequent versions. Please use the [asc_storealign_pack_quarter](../SIMD-API/c_api/reg_compute/store/asc_storealign_pack_quarter.md) API.

- [asc_storealign_pack_postupdate_v2](../SIMD-API/c_api/deprecated_interface/asc_storealign_v4_deprecated.md) API

    This API will be deprecated in subsequent versions. Please use the [asc_storealign_pack_quarter_postupdate](../SIMD-API/c_api/reg_compute/store/asc_storealign_pack_quarter_postupdate.md) API.

- [asc_storealign (multi-mode overload)](../SIMD-API/c_api/reg_compute/store/asc_storealign.md) API

    The original `asc_storealign(dst, src, mask)` implemented multiple store modes such as NORM/FIRST/PACK/PACKV2/INTLV through function name overloading. It is now narrowed to only the NORM mode, and the other modes have been split into independent functions. Please use the new version of [asc_storealign](../SIMD-API/c_api/reg_compute/store/asc_storealign.md).

- [asc_storealign (1st mode)](../SIMD-API/c_api/reg_compute/store/asc_storealign_1st.md) API

    One of the original asc_storealign overloads, which has been split into the independent function [asc_storealign_1st](../SIMD-API/c_api/reg_compute/store/asc_storealign_1st.md).

- [asc_storealign (intlv mode)](../SIMD-API/c_api/reg_compute/store/asc_storealign_intlv.md) API

    One of the original asc_storealign overloads, which has been split into the independent function [asc_storealign_intlv](../SIMD-API/c_api/reg_compute/store/asc_storealign_intlv.md).

- [asc_storealign (pack mode)](../SIMD-API/c_api/reg_compute/store/asc_storealign_pack.md) API

    One of the original asc_storealign overloads, which has been split into the independent function [asc_storealign_pack](../SIMD-API/c_api/reg_compute/store/asc_storealign_pack.md).

- [asc_storealign (pack_quarter mode)](../SIMD-API/c_api/reg_compute/store/asc_storealign_pack_quarter.md) API

    One of the original asc_storealign overloads, which has been split into the independent function [asc_storealign_pack_quarter](../SIMD-API/c_api/reg_compute/store/asc_storealign_pack_quarter.md).

- [asc_storealign (postupdate mode)](../SIMD-API/c_api/reg_compute/store/asc_storealign_postupdate.md) API

    One of the overloads of the original asc_storealign, which has been split into the independent function [asc_storealign_postupdate](../SIMD-API/c_api/reg_compute/store/asc_storealign_postupdate.md).

- [asc_scatter (reg_store)](../SIMD-API/c_api/deprecated_interface/asc_scatter_deprecated.md) API

    This API will be deprecated in subsequent versions. Please use the [asc_scatter (reg_store)](../SIMD-API/c_api/reg_compute/scatter/asc_scatter.md) API.

- [asc_storeunalign_post](../SIMD-API/c_api/deprecated_interface/asc_storeunalign_post_deprecated.md) API (no offset prototype)

    This API will be deprecated in subsequent versions. Please use the [asc_squeeze_and_storeunalign_finalize](../SIMD-API/c_api/reg_compute/reg_permute_sel/asc_squeeze_and_storeunalign_finalize.md) API.

- [asc_storeunalign_postupdate](../SIMD-API/c_api/deprecated_interface/asc_storeunalign_postupdate_deprecated.md) API (prototype without specifying the store offset)

    This API will be deprecated in subsequent versions. Please use the [asc_squeeze_and_storeunalign](../SIMD-API/c_api/reg_compute/reg_permute_sel/asc_squeeze_and_storeunalign.md) API.

- [asc_storeunalign_postupdate](../SIMD-API/c_api/deprecated_interface/asc_storeunalign_postupdate_deprecated.md) API (b8 data type prototype for the mask register store mode)

    This API will be deprecated in subsequent versions. Please use the b16 or b32 data type prototype.

<!-- end id3 -->

<!-- npu="950" id4 -->
#### Logical Calculation

- [asc_and](../SIMD-API/c_api/deprecated_interface/asc_and_deprecated.md) API (floating-point type overload)

    The floating-point type overload of `asc_and` is deprecated. Please select the corresponding integer overload by bit width instead. For details, see [asc_and](../SIMD-API/c_api/reg_compute/reg_logic/asc_and.md).

- [asc_or](../SIMD-API/c_api/deprecated_interface/asc_or_deprecated.md) API (floating-point type overload)

    The floating-point type overload of `asc_or` is deprecated. Please select the corresponding integer overload by bit width instead. For details, see [asc_or](../SIMD-API/c_api/reg_compute/reg_logic/asc_or.md).

#### Type Conversion

- [asc_float2bfloat16](../SIMD-API/c_api/deprecated_interface/asc_float2bfloat16_deprecated.md) API (three-parameter prototype that controls the destination operand write position through the API suffix)

    The old prototype has been deprecated. Please use the explicit position parameter overload of [asc_float2bfloat16](../SIMD-API/c_api/reg_compute/reg_convert/asc_float2bfloat16.md).

- [asc_int322int16](../SIMD-API/c_api/deprecated_interface/asc_int322int16_deprecated.md) API (three-parameter prototype that controls the destination operand write position through the API suffix)

    The old prototype has been deprecated. Please use the explicit position parameter overload of [asc_int322int16](../SIMD-API/c_api/reg_compute/reg_convert/asc_int322int16.md).

- [asc_int322int64](../SIMD-API/c_api/deprecated_interface/asc_int322int64_deprecated.md) API (three-parameter prototype that controls the source operand read position through the API suffix)

    The old prototype has been deprecated. Please use the explicit position parameter overload of [asc_int322int64](../SIMD-API/c_api/reg_compute/reg_convert/asc_int322int64.md).

- [asc_int322uint16](../SIMD-API/c_api/deprecated_interface/asc_int322uint16_deprecated.md) API (three-parameter prototype that controls the destination operand write position through the API suffix)

    The old prototype has been deprecated. Please use the explicit position parameter overload of [asc_int322uint16](../SIMD-API/c_api/reg_compute/reg_convert/asc_int322uint16.md).

- [asc_uint82uint16](../SIMD-API/c_api/deprecated_interface/asc_uint82uint16_deprecated.md) API (three-parameter prototype that controls the source operand read position through the API suffix)

    The old prototype has been deprecated. Please use the explicit position parameter overload of [asc_uint82uint16](../SIMD-API/c_api/reg_compute/reg_convert/asc_uint82uint16.md).

- [asc_float2half](../SIMD-API/c_api/deprecated_interface/asc_float2half_deprecated.md) API (three-parameter prototype that controls the destination operand write position through the API suffix)

    The old prototype has been deprecated. Please use the explicit position parameter overload of [asc_float2half](../SIMD-API/c_api/reg_compute/reg_convert/asc_float2half.md).

- [asc_float2int16](../SIMD-API/c_api/deprecated_interface/asc_float2int16_deprecated.md) API (three-parameter prototype that controls the destination operand write position through the API suffix)

    The old prototype has been deprecated. Please use the explicit position parameter overload of [asc_float2int16](../SIMD-API/c_api/reg_compute/reg_convert/asc_float2int16.md).

- [asc_float2int64](../SIMD-API/c_api/deprecated_interface/asc_float2int64_deprecated.md) API (three-parameter prototype that controls the destination operand write position through the API suffix)

    The old prototype has been deprecated. Please use the explicit position parameter overload of [asc_float2int64](../SIMD-API/c_api/reg_compute/reg_convert/asc_float2int64.md).

- [asc_hif82half](../SIMD-API/c_api/deprecated_interface/asc_hif82half_deprecated.md) API (three-parameter prototype that controls the source operand read position through the API suffix)

    The old prototype has been deprecated. Please use the explicit position parameter overload of [asc_hif82half](../SIMD-API/c_api/reg_compute/reg_convert/asc_hif82half.md).

- [asc_int162float](../SIMD-API/c_api/deprecated_interface/asc_int162float_deprecated.md) API (three-parameter prototype that controls the source operand read position through the API suffix)

    The old prototype has been deprecated. Please use the explicit position parameter overload of [asc_int162float](../SIMD-API/c_api/reg_compute/reg_convert/asc_int162float.md).

- [asc_int162int32](../SIMD-API/c_api/deprecated_interface/asc_int162int32_deprecated.md) API (three-parameter prototype that controls the source operand read position through the API suffix)

    The old prototype has been deprecated. Please use the explicit position parameter overload of [asc_int162int32](../SIMD-API/c_api/reg_compute/reg_convert/asc_int162int32.md).

- [asc_int162uint32](../SIMD-API/c_api/deprecated_interface/asc_int162uint32_deprecated.md) API (three-parameter prototype that controls the source operand read position through the API suffix)

    The old prototype has been deprecated. Please use the explicit position parameter overload of [asc_int162uint32](../SIMD-API/c_api/reg_compute/reg_convert/asc_int162uint32.md).

- [asc_int162uint8](../SIMD-API/c_api/deprecated_interface/asc_int162uint8_deprecated.md) API (three-parameter prototype that controls the destination operand write position through the API suffix)

    The old prototype has been deprecated. Please use the explicit position parameter overload of [asc_int162uint8](../SIMD-API/c_api/reg_compute/reg_convert/asc_int162uint8.md).

- [asc_int642float](../SIMD-API/c_api/deprecated_interface/asc_int642float_deprecated.md) API (three-parameter prototype that controls the destination operand write position through the API suffix)

    The old prototype has been deprecated. Please use the explicit position parameter overload of [asc_int642float](../SIMD-API/c_api/reg_compute/reg_convert/asc_int642float.md).

- [asc_int642int32](../SIMD-API/c_api/deprecated_interface/asc_int642int32_deprecated.md) API (three-parameter prototype that controls the destination operand write position through the API suffix)

    The old prototype has been deprecated. Please use the explicit position parameter overload of [asc_int642int32](../SIMD-API/c_api/reg_compute/reg_convert/asc_int642int32.md).

- [asc_int82half](../SIMD-API/c_api/deprecated_interface/asc_int82half_deprecated.md) API (a three-parameter prototype that controls the source operand read position through the API suffix)

    The old prototype has been deprecated. Please use the explicit position parameter overload of [asc_int82half](../SIMD-API/c_api/reg_compute/reg_convert/asc_int82half.md).

- [asc_int82int16](../SIMD-API/c_api/deprecated_interface/asc_int82int16_deprecated.md) API (a three-parameter prototype that controls the source operand read position through the API suffix)

    The old prototype has been deprecated. Please use the explicit position parameter overload of [asc_int82int16](../SIMD-API/c_api/reg_compute/reg_convert/asc_int82int16.md).

- [asc_uint162uint32](../SIMD-API/c_api/deprecated_interface/asc_uint162uint32_deprecated.md) API (a three-parameter prototype that controls the source operand read position through the API suffix)

    The old prototype has been deprecated. Please use the explicit position parameter overload of [asc_uint162uint32](../SIMD-API/c_api/reg_compute/reg_convert/asc_uint162uint32.md).

- [asc_uint162uint8](../SIMD-API/c_api/deprecated_interface/asc_uint162uint8_deprecated.md) API (a three-parameter prototype that controls the destination operand write position through the API suffix)

    The old prototype has been deprecated. Please use the explicit position parameter overload of [asc_uint162uint8](../SIMD-API/c_api/reg_compute/reg_convert/asc_uint162uint8.md).

- [asc_uint322int16](../SIMD-API/c_api/deprecated_interface/asc_uint322int16_deprecated.md) API (a three-parameter prototype that controls the destination operand write position through the API suffix)

    The old prototype has been deprecated. Please use the explicit position parameter overload of [asc_uint322int16](../SIMD-API/c_api/reg_compute/reg_convert/asc_uint322int16.md).

- [asc_uint322uint16](../SIMD-API/c_api/deprecated_interface/asc_uint322uint16_deprecated.md) API (the three-parameter prototype that controls the destination operand write position through the API suffix)

    The old prototype has been deprecated. Please use the explicit position parameter overload of [asc_uint322uint16](../SIMD-API/c_api/reg_compute/reg_convert/asc_uint322uint16.md).

- [asc_uint82half](../SIMD-API/c_api/deprecated_interface/asc_uint82half_deprecated.md) API (the three-parameter prototype that controls the source operand read position through the API suffix)

    The old prototype has been deprecated. Please use the explicit position parameter overload of [asc_uint82half](../SIMD-API/c_api/reg_compute/reg_convert/asc_uint82half.md).

- [asc_bfloat162float](../SIMD-API/c_api/deprecated_interface/asc_bfloat162float_deprecated.md) API (the three-parameter prototype that controls the source operand read position through the API suffix)

    The old prototype has been deprecated. Please use the explicit position parameter overload of [asc_bfloat162float](../SIMD-API/c_api/reg_compute/reg_convert/asc_bfloat162float.md).

- [asc_bfloat162int32](../SIMD-API/c_api/deprecated_interface/asc_bfloat162int32_deprecated.md) API (the three-parameter prototype that controls the source operand read position through the API suffix)

    The old prototype has been deprecated. Please use the explicit position parameter overload of [asc_bfloat162int32](../SIMD-API/c_api/reg_compute/reg_convert/asc_bfloat162int32.md).

- [asc_half2float](../SIMD-API/c_api/deprecated_interface/asc_half2float_deprecated.md) API (the three-parameter prototype that controls the source operand read position through the API suffix)

    The old prototype has been deprecated. Please use the explicit position parameter overload of [asc_half2float](../SIMD-API/c_api/reg_compute/reg_convert/asc_half2float.md).

- [asc_half2hif8](../SIMD-API/c_api/deprecated_interface/asc_half2hif8_deprecated.md) API (three-parameter prototype that controls the destination operand write position through the API suffix)

    The old prototype has been deprecated. Please use the explicit position parameter overload of [asc_half2hif8](../SIMD-API/c_api/reg_compute/reg_convert/asc_half2hif8.md).

- [asc_half2int32](../SIMD-API/c_api/deprecated_interface/asc_half2int32_deprecated.md) API (three-parameter prototype that controls the source operand read position through the API suffix)

    The old prototype has been deprecated. Please use the explicit position parameter overload of [asc_half2int32](../SIMD-API/c_api/reg_compute/reg_convert/asc_half2int32.md).

- [asc_half2int8](../SIMD-API/c_api/deprecated_interface/asc_half2int8_deprecated.md) API (three-parameter prototype that controls the destination operand write position through the API suffix)

    The old prototype has been deprecated. Please use the explicit position parameter overload of [asc_half2int8](../SIMD-API/c_api/reg_compute/reg_convert/asc_half2int8.md).

- [asc_half2uint8](../SIMD-API/c_api/deprecated_interface/asc_half2uint8_deprecated.md) API (three-parameter prototype that controls the destination operand write position through the API suffix)

    The old prototype has been deprecated. Please use the explicit position parameter overload of [asc_half2uint8](../SIMD-API/c_api/reg_compute/reg_convert/asc_half2uint8.md).

- [asc_bfloat162e2m1x2](../SIMD-API/c_api/deprecated_interface/asc_bfloat162e2m1x2_deprecated.md) API

    The old prototype has been deprecated. Please use the explicit position parameter overload of [asc_bfloat162e2m1x2](../SIMD-API/c_api/reg_compute/reg_convert/asc_bfloat162e2m1x2.md).

- [asc_bfloat162e1m2x2](../SIMD-API/c_api/deprecated_interface/asc_bfloat162e1m2x2_deprecated.md) API

    The old prototype has been deprecated. Please use the explicit position parameter overload of [asc_bfloat162e1m2x2](../SIMD-API/c_api/reg_compute/reg_convert/asc_bfloat162e1m2x2.md).

- [asc_e2m1x22bfloat16](../SIMD-API/c_api/deprecated_interface/asc_e2m1x22bfloat16_deprecated.md) API

    The old prototype has been deprecated. Please use the explicit position parameter overload of [asc_e2m1x22bfloat16](../SIMD-API/c_api/reg_compute/reg_convert/asc_e2m1x22bfloat16.md).

- [asc_e1m2x22bfloat16](../SIMD-API/c_api/deprecated_interface/asc_e1m2x22bfloat16_deprecated.md) API

    The old prototype has been deprecated. Please use the explicit position parameter overload of [asc_e1m2x22bfloat16](../SIMD-API/c_api/reg_compute/reg_convert/asc_e1m2x22bfloat16.md).

- [asc_float2e5m2](../SIMD-API/c_api/deprecated_interface/asc_float2e5m2_deprecated.md) API

    The old prototype has been deprecated. Please use the explicit position parameter overload of [asc_float2e5m2](../SIMD-API/c_api/reg_compute/reg_convert/asc_float2e5m2.md).

- [asc_e5m22float](../SIMD-API/c_api/deprecated_interface/asc_e5m22float_deprecated.md) API

    The old prototype has been deprecated. Please use the explicit position parameter overload of [asc_e5m22float](../SIMD-API/c_api/reg_compute/reg_convert/asc_e5m22float.md).

- [asc_float2e4m3](../SIMD-API/c_api/deprecated_interface/asc_float2e4m3_deprecated.md) API

    The old prototype has been deprecated. Please use the explicit position parameter overload of [asc_float2e4m3](../SIMD-API/c_api/reg_compute/reg_convert/asc_float2e4m3.md).

- [asc_e4m32float](../SIMD-API/c_api/deprecated_interface/asc_e4m32float_deprecated.md) API

    The old prototype has been deprecated. Please use the explicit position parameter overload of [asc_e4m32float](../SIMD-API/c_api/reg_compute/reg_convert/asc_e4m32float.md).

- [asc_float2hif8](../SIMD-API/c_api/deprecated_interface/asc_float2hif8_deprecated.md) API

    The old prototype has been deprecated. Please use the explicit position parameter overload of [asc_float2hif8](../SIMD-API/c_api/reg_compute/reg_convert/asc_float2hif8.md).

- [asc_hif82float](../SIMD-API/c_api/deprecated_interface/asc_hif82float_deprecated.md) API

    The old prototype has been deprecated. Please use the explicit position parameter overload of [asc_hif82float](../SIMD-API/c_api/reg_compute/reg_convert/asc_hif82float.md).

- [asc_half2int4x2](../SIMD-API/c_api/deprecated_interface/asc_half2int4x2_deprecated.md) API (three-parameter prototype that controls the destination operand write position through the API suffix)

    The old prototype has been deprecated. Please use the explicit position parameter overload of [asc_half2int4x2](../SIMD-API/c_api/reg_compute/reg_convert/asc_half2int4x2.md).

- [asc_int4x22int16](../SIMD-API/c_api/deprecated_interface/asc_int4x22int16_deprecated.md) API (three-parameter prototype that controls the destination operand write position through the API suffix)

    The old prototype has been deprecated. Please use the explicit position parameter overload of [asc_int4x22int16](../SIMD-API/c_api/reg_compute/reg_convert/asc_int4x22int16.md).

- [asc_int4x22half](../SIMD-API/c_api/deprecated_interface/asc_int4x22half_deprecated.md) API (three-parameter prototype that controls the destination operand write position through the API suffix)

    The old prototype has been deprecated. Please use the explicit position parameter overload of [asc_int4x22half](../SIMD-API/c_api/reg_compute/reg_convert/asc_int4x22half.md).

- [asc_int4x22bfloat16](../SIMD-API/c_api/deprecated_interface/asc_int4x22bfloat16_deprecated.md) API (three-parameter prototype that controls the destination operand write position through the API suffix)

    The old prototype has been deprecated. Please use the explicit position parameter overload of [asc_int4x22bfloat16](../SIMD-API/c_api/reg_compute/reg_convert/asc_int4x22bfloat16.md).

- [asc_int82int32](../SIMD-API/c_api/deprecated_interface/asc_int82int32_deprecated.md) API

    The old prototype has been deprecated. Please use the explicit position parameter overload of [asc_int82int32](../SIMD-API/c_api/reg_compute/reg_convert/asc_int82int32.md).

- [asc_int322uint8](../SIMD-API/c_api/deprecated_interface/asc_int322uint8_deprecated.md) API

    The old prototype has been deprecated. Please use the explicit position parameter overload of [asc_int322uint8](../SIMD-API/c_api/reg_compute/reg_convert/asc_int322uint8.md).

- [asc_uint82uint32](../SIMD-API/c_api/deprecated_interface/asc_uint82uint32_deprecated.md) API

    The old prototype has been deprecated. Please use the explicit position parameter overload of [asc_uint82uint32](../SIMD-API/c_api/reg_compute/reg_convert/asc_uint82uint32.md).

- [asc_uint322uint8](../SIMD-API/c_api/deprecated_interface/asc_uint322uint8_deprecated.md) API

    The old prototype has been deprecated. Please use the explicit position parameter overload of [asc_uint322uint8](../SIMD-API/c_api/reg_compute/reg_convert/asc_uint322uint8.md).

#### Compound Calculation

- [asc_muls](../SIMD-API/c_api/deprecated_interface/asc_muls_deprecated.md) API

    This API is deprecated. Use the explicit position parameter overload of [asc_mul_scalar_float2half_rn](../SIMD-API/c_api/reg_compute/reg_fused/asc_mul_scalar_float2half_rn.md) instead.

- [asc_exp_sub](../SIMD-API/c_api/deprecated_interface/asc_exp_sub_deprecated.md) API

    The `asc_exp_sub` prototype with `half` input and without the `src_pos` parameter, and the `asc_exp_sub_v2` prototype, are deprecated. For `float` input, use the `asc_exp_sub` prototype. For `half` input, use the `asc_exp_sub_half2float` prototype with the explicit `src_pos` parameter.

#### Layout Transformation

- [asc_squeeze_v2](../SIMD-API/c_api/deprecated_interface/asc_squeeze_deprecated.md) API

    This API will be deprecated in subsequent versions. Please use the [asc_squeeze_and_storeunalign](../SIMD-API/c_api/reg_compute/reg_permute_sel/asc_squeeze_and_storeunalign.md) API.

- [asc_squeeze_with_status](../SIMD-API/c_api/deprecated_interface/asc_squeeze_with_status_deprecated.md) API

    This API will be deprecated in subsequent versions. Please use the [asc_squeeze_and_storeunalign](../SIMD-API/c_api/reg_compute/reg_permute_sel/asc_squeeze_and_storeunalign.md) API.

- [asc_clear_ar_spr](../SIMD-API/c_api/deprecated_interface/asc_clear_ar_spr_deprecated.md) API

    This API will be deprecated in subsequent versions. Please use the [asc_squeeze_and_storeunalign_init](../SIMD-API/c_api/reg_compute/reg_permute_sel/asc_squeeze_and_storeunalign_init.md) API.

- [asc_pack](../SIMD-API/c_api/deprecated_interface/asc_pack_deprecated.md) API

    This API will be deprecated in subsequent versions. Please use the [asc_pack_to_low](../SIMD-API/c_api/reg_compute/reg_permute_sel/asc_pack_to_low.md) API.

- [asc_pack_v2](../SIMD-API/c_api/deprecated_interface/asc_pack_deprecated.md) API

    This API will be deprecated in subsequent versions. Please use the [asc_pack_to_high](../SIMD-API/c_api/reg_compute/reg_permute_sel/asc_pack_to_high.md) API instead.

#### Address Register Operations

- [asc_create_iter_reg](../SIMD-API/c_api/deprecated_interface/asc_create_iter_reg_deprecated.md) API

    This API will be deprecated in subsequent versions. Please use the [asc_update_addr_reg](../SIMD-API/c_api/reg_compute/reg_addr_reg/asc_update_addr_reg.md) API.

<!-- end id4 -->

### Synchronous Control

- [asc_sync_vec (prototype without input parameters)](../SIMD-API/c_api/sync/intra_core_sync/asc_sync_vec.md) API

    The `asc_sync_vec()` API is deprecated. Please use [asc_sync](../SIMD-API/c_api/sync/intra_core_sync/asc_sync.md) to perform synchronization operations for all pipelines.

### Matrix Computation

- [asc_enable_fp8 (deprecated)](../SIMD-API/c_api/deprecated_interface/asc_enable_fp8_deprecated.md) API

    Please use the [asc_disable_hif8](../SIMD-API/c_api/cube_compute/asc_disable_hif8.md) API.

- [asc_set_fp32_mode (deprecated)](../SIMD-API/c_api/deprecated_interface/asc_set_fp32_mode_deprecated.md) API

    Please use the [asc_disable_hf32](../SIMD-API/c_api/cube_compute/asc_disable_hf32.md) API.

- [asc_enable_hf32_trans (deprecated)](../SIMD-API/c_api/deprecated_interface/asc_enable_hf32_trans_deprecated.md) API

    Please use the [asc_set_hf32_round_mode](../SIMD-API/c_api/cube_compute/asc_set_hf32_round_mode.md) API.

- [asc_set_l0c2gm_quant_post (deprecated)](../SIMD-API/c_api/deprecated_interface/asc_set_l0c2gm_quant_post_deprecated.md) API

    This API is deprecated, and no replacement API is available.

- [asc_set_l0c2gm_nz2nd (deprecated)](../SIMD-API/c_api/deprecated_interface/asc_set_l0c2gm_nz2nd_deprecated.md) API: Please use the [asc_set_l0c_copy_nz_para](../SIMD-API/c_api/cube_datamove/asc_set_l0c_copy_nz_para.md) API.
- [asc_set_l0c2gm_channel_para (deprecated)](../SIMD-API/c_api/deprecated_interface/asc_set_l0c2gm_channel_para_deprecated.md) API: Please use the [asc_set_l0c_copy_channel_para](../SIMD-API/c_api/cube_datamove/asc_set_l0c_copy_channel_para.md) API.
- [asc_set_l0c2gm_config (deprecated)](../SIMD-API/c_api/deprecated_interface/asc_set_l0c2gm_config_deprecated.md) API: Please use the [asc_set_l0c_copy_config](../SIMD-API/c_api/cube_datamove/asc_set_l0c_copy_config.md) API.
- [asc_set_l12l0_padding_val (deprecated)](../SIMD-API/c_api/deprecated_interface/asc_set_l12l0_padding_val_deprecated.md) API: Please use the [asc_set_l12l0b_3d_padding](../SIMD-API/c_api/cube_datamove/asc_set_l12l0b_3d_padding.md) API.
- [asc_set_l13d_padding (deprecated)](../SIMD-API/c_api/deprecated_interface/asc_set_l13d_padding_deprecated.md) API: Please use the [asc_set_l12l0a_3d_padding](../SIMD-API/c_api/cube_datamove/asc_set_l12l0a_3d_padding.md) API.
- [asc_set_l0c2gm_relu_alpha (deprecated)](../SIMD-API/c_api/deprecated_interface/asc_set_l0c2gm_relu_alpha_deprecated.md) API: Please use the [asc_set_l0c_copy_relu_alpha](../SIMD-API/c_api/cube_datamove/asc_set_l0c_copy_relu_alpha.md) API.
- [asc_set_l0c2gm_lrelu_alpha (deprecated)](../SIMD-API/c_api/deprecated_interface/asc_set_l0c2gm_lrelu_alpha_deprecated.md) API: Please use the [asc_set_l0c_copy_lrelu_alpha](../SIMD-API/c_api/cube_datamove/asc_set_l0c_copy_lrelu_alpha.md) API.
- [asc_copy_gm2l1_align (deprecated)](../SIMD-API/c_api/deprecated_interface/asc_copy_gm2l1_align_deprecated.md) API (old regular overload and synchronous API): Please use the [asc_copy_gm2l1_align](../SIMD-API/c_api/cube_datamove/asc_copy_gm2l1_align.md) API and the [asc_sync](../SIMD-API/c_api/sync/intra_core_sync/asc_sync.md) API as replacements.
- [asc_copy_gm2l1_dn2nz (deprecated)](../SIMD-API/c_api/deprecated_interface/asc_copy_gm2l1_dn2nz_deprecated.md) API: The old regular overload and `asc_copy_gm2l1_dn2nz_sync` are deprecated. Please use the [asc_copy_gm2l1_dn2nz](../SIMD-API/c_api/cube_datamove/asc_copy_gm2l1_dn2nz.md) API and the [asc_sync](../SIMD-API/c_api/sync/intra_core_sync/asc_sync.md) API as replacements.
- [asc_copy_gm2l1_nd2nz (deprecated)](../SIMD-API/c_api/deprecated_interface/asc_copy_gm2l1_nd2nz_deprecated.md) API: The old regular overload and `asc_copy_gm2l1_nd2nz_sync` are deprecated. Please use the [asc_copy_gm2l1_nd2nz](../SIMD-API/c_api/cube_datamove/asc_copy_gm2l1_nd2nz/asc_copy_gm2l1_nd2nz_arch_3510.md) API and the [asc_sync](../SIMD-API/c_api/sync/intra_core_sync/asc_sync.md) API as replacements.
- [asc_copy_gm2l1 (old high-dimensional split form, deprecated)](../SIMD-API/c_api/deprecated_interface/asc_copy_gm2l1_highdim_deprecated.md) API: Please use the [asc_copy_gm2l1_arch_3510 (high-dimensional split data movement)](../SIMD-API/c_api/cube_datamove/asc_copy_gm2l1/asc_copy_gm2l1_highdim_split_arch_3510.md) API with the [asc_channel_pad_mode](../SIMD-API/c_api/defs/enum/asc_channel_pad_mode.md) parameter and the [asc_sync](../SIMD-API/c_api/sync/intra_core_sync/asc_sync.md) API as replacements.
- [asc_copy_gm2l1 (2D matrix move legacy form, deprecated)](../SIMD-API/c_api/deprecated_interface/asc_copy_gm2l1_2d_deprecated.md) API: Please use the [asc_copy_gm2l1_arch_3510 (2D matrix move mode)](../SIMD-API/c_api/cube_datamove/asc_copy_gm2l1/asc_copy_gm2l1_2d_arch_3510.md) API with the [asc_load_l2_cache_mode](../SIMD-API/c_api/defs/enum/asc_load_l2_cache_mode.md) parameter and the [asc_sync](../SIMD-API/c_api/sync/intra_core_sync/asc_sync.md) API instead.
- [asc_copy_l12ub_sync (deprecated)](../SIMD-API/c_api/deprecated_interface/asc_copy_l12ub_sync_deprecated.md) API: Please use the [asc_copy_l12ub](../SIMD-API/c_api/cube_datamove/asc_copy_l12ub.md) API and the [asc_sync](../SIMD-API/c_api/sync/intra_core_sync/asc_sync.md) API instead.
- [asc_copy_l0c2gm (deprecated)](../SIMD-API/c_api/deprecated_interface/asc_copy_l0c2gm_deprecated.md) API: The old parameter form and the `asc_copy_l0c2gm_sync` API are deprecated. Please use the [asc_copy_l0c2gm](../SIMD-API/c_api/cube_datamove/asc_copy_l0c2gm/asc_copy_l0c2gm_arch_3510.md) API and the [asc_sync](../SIMD-API/c_api/sync/intra_core_sync/asc_sync.md) API instead.
- [asc_copy_l0c2l1 (deprecated)](../SIMD-API/c_api/deprecated_interface/asc_copy_l0c2l1_deprecated.md) API: The old parameter form and the `asc_copy_l0c2l1_sync` API are deprecated. Please use the [asc_copy_l0c2l1](../SIMD-API/c_api/cube_datamove/asc_copy_l0c2l1/asc_copy_l0c2l1_arch_3510.md) API and the [asc_sync](../SIMD-API/c_api/sync/intra_core_sync/asc_sync.md) API instead.
- [asc_copy_l0c2ub (deprecated)](../SIMD-API/c_api/deprecated_interface/asc_copy_l0c2ub_deprecated.md) API: The old parameter form and the `asc_copy_l0c2ub_sync` API are deprecated. Please use the [asc_copy_l0c2ub](../SIMD-API/c_api/cube_datamove/asc_copy_l0c2ub.md) API and the [asc_sync](../SIMD-API/c_api/sync/intra_core_sync/asc_sync.md) API instead.

- [asc_mmad (deprecated)](../SIMD-API/c_api/deprecated_interface/asc_mmad_deprecated.md) API

    The `asc_mmad()` and `asc_mmad_s4()` APIs that use the `uint8_t unit_flag` or `uint8_t feat_offset` input parameter are deprecated. Please use the [asc_mmad](../SIMD-API/c_api/cube_compute/asc_mmad.md) API. The `asc_mmad_sync()` and `asc_mmad_s4_sync()` APIs are deprecated. Please use the [asc_mmad](../SIMD-API/c_api/cube_compute/asc_mmad.md) API and the [asc_sync](../SIMD-API/c_api/sync/intra_core_sync/asc_sync.md) API instead.

<!-- npu="950" id5 -->
- [asc_mmad_mx (deprecated)](../SIMD-API/c_api/deprecated_interface/asc_mmad_mx_deprecated.md) API

    The `asc_mmad_mx()` API that uses the `uint8_t unit_flag` input parameter is deprecated. Please use the [asc_mmad_mx](../SIMD-API/c_api/cube_compute/asc_mmad_mx.md) API. The `asc_mmad_mx_sync()` API is deprecated. Please use the [asc_mmad_mx](../SIMD-API/c_api/cube_compute/asc_mmad_mx.md) API and the [asc_sync](../SIMD-API/c_api/sync/intra_core_sync/asc_sync.md) API instead.
<!-- end id5 -->

<!-- npu="A3,910b" id6 -->
- [asc_mmad_sparse (deprecated)](../SIMD-API/c_api/deprecated_interface/asc_mmad_sparse_deprecated.md) API

    The `asc_mmad_sparse()` API that uses the `uint8_t unit_flag` input parameter is deprecated. Please use the [asc_mmad_sparse](../SIMD-API/c_api/cube_compute/asc_mmad_sparse.md) API instead. The `asc_mmad_sparse_sync()` API is deprecated. Please use the [asc_mmad_sparse](../SIMD-API/c_api/cube_compute/asc_mmad_sparse.md) API and the [asc_sync](../SIMD-API/c_api/sync/intra_core_sync/asc_sync.md) API instead.
<!-- end id6 -->
### Matrix Computation Data Movement

- [asc_copy_gm2l0a_sync (deprecated)](../SIMD-API/c_api/deprecated_interface/asc_copy_gm2l0a_sync_deprecated.md) API

    `asc_copy_gm2l0a_sync()` is deprecated. Please use the [corresponding non-synchronous API](../SIMD-API/c_api/cube_datamove/asc_copy_gm2l0a.md) and the [asc_sync](../SIMD-API/c_api/sync/intra_core_sync/asc_sync.md) API instead.

- [asc_copy_gm2l0b_sync (deprecated)](../SIMD-API/c_api/deprecated_interface/asc_copy_gm2l0b_sync_deprecated.md) API

    `asc_copy_gm2l0b_sync()` is deprecated. Please use the [corresponding non-synchronous API](../SIMD-API/c_api/cube_datamove/asc_copy_gm2l0b.md) and the [asc_sync](../SIMD-API/c_api/sync/intra_core_sync/asc_sync.md) API instead.

- [asc_copy_gm2l1_align_sync (deprecated)](../SIMD-API/c_api/deprecated_interface/asc_copy_gm2l1_align_sync_deprecated.md) API

    `asc_copy_gm2l1_align_sync()` is deprecated. Please use the [corresponding non-synchronous API](../SIMD-API/c_api/cube_datamove/asc_copy_gm2l1_align.md) and the [asc_sync](../SIMD-API/c_api/sync/intra_core_sync/asc_sync.md) API instead.

- [asc_copy_gm2l1_dn2nz_sync (deprecated)](../SIMD-API/c_api/deprecated_interface/asc_copy_gm2l1_dn2nz_sync_deprecated.md) API

    `asc_copy_gm2l1_dn2nz_sync()` is deprecated. Please use the [corresponding non-synchronous API](../SIMD-API/c_api/cube_datamove/asc_copy_gm2l1_dn2nz.md) and the [asc_sync](../SIMD-API/c_api/sync/intra_core_sync/asc_sync.md) API instead.

- [asc_copy_gm2l1_nd2nz_sync_arch_2201 (deprecated)](../SIMD-API/c_api/deprecated_interface/asc_copy_gm2l1_nd2nz_sync_arch_2201_deprecated.md) API

    `asc_copy_gm2l1_nd2nz_sync()` is deprecated. Please use the [corresponding non-synchronous API](../SIMD-API/c_api/cube_datamove/asc_copy_gm2l1_nd2nz/asc_copy_gm2l1_nd2nz_arch_2201.md) and the [asc_sync](../SIMD-API/c_api/sync/intra_core_sync/asc_sync.md) API instead.

- [asc_copy_gm2l1_nd2nz_sync_arch_3510 (deprecated)](../SIMD-API/c_api/deprecated_interface/asc_copy_gm2l1_nd2nz_sync_arch_3510_deprecated.md) API

    `asc_copy_gm2l1_nd2nz_sync()` is deprecated. Please use the [corresponding non-synchronous API](../SIMD-API/c_api/cube_datamove/asc_copy_gm2l1_nd2nz/asc_copy_gm2l1_nd2nz_arch_3510.md) and the [asc_sync](../SIMD-API/c_api/sync/intra_core_sync/asc_sync.md) API instead.

- [asc_copy_gm2l1_sync_2d_arch_3510 (deprecated)](../SIMD-API/c_api/deprecated_interface/asc_copy_gm2l1_sync_2d_arch_3510_deprecated.md) API

    `asc_copy_gm2l1_sync()` is deprecated. Please use the [corresponding non-synchronous API](../SIMD-API/c_api/cube_datamove/asc_copy_gm2l1/asc_copy_gm2l1_2d_arch_3510.md) and the [asc_sync](../SIMD-API/c_api/sync/intra_core_sync/asc_sync.md) API instead.

- [asc_copy_gm2l1_sync_arch_2201 (deprecated)](../SIMD-API/c_api/deprecated_interface/asc_copy_gm2l1_sync_arch_2201_deprecated.md) API

    `asc_copy_gm2l1_sync()`, `asc_copy_gm2l1_pad1_sync()`, `asc_copy_gm2l1_pad2_sync()`, `asc_copy_gm2l1_pad3_sync()`, `asc_copy_gm2l1_pad4_sync()`, `asc_copy_gm2l1_pad5_sync()`, `asc_copy_gm2l1_pad6_sync()`, `asc_copy_gm2l1_pad7_sync()`, and `asc_copy_gm2l1_pad8_sync()` are deprecated. Please use the [corresponding non-synchronous API](../SIMD-API/c_api/cube_datamove/asc_copy_gm2l1/asc_copy_gm2l1_arch_2201.md) and the [asc_sync](../SIMD-API/c_api/sync/intra_core_sync/asc_sync.md) API instead.

- [asc_copy_gm2l1_sync_highdim_split_arch_3510 (deprecated)](../SIMD-API/c_api/deprecated_interface/asc_copy_gm2l1_sync_highdim_split_arch_3510_deprecated.md) API

    `asc_copy_gm2l1_sync()` is deprecated. Please use the [corresponding non-synchronous API](../SIMD-API/c_api/cube_datamove/asc_copy_gm2l1/asc_copy_gm2l1_highdim_split_arch_3510.md) and the [asc_sync](../SIMD-API/c_api/sync/intra_core_sync/asc_sync.md) API instead.

- [asc_copy_l0c2gm_sync_arch_2201 (deprecated)](../SIMD-API/c_api/deprecated_interface/asc_copy_l0c2gm_sync_arch_2201_deprecated.md) API

    `asc_copy_l0c2gm_sync()` is deprecated. Please use the [corresponding non-synchronous API](../SIMD-API/c_api/cube_datamove/asc_copy_l0c2gm/asc_copy_l0c2gm_arch_2201.md) and [asc_sync](../SIMD-API/c_api/sync/intra_core_sync/asc_sync.md) instead.

- [asc_copy_l0c2l1_sync_arch_2201 (deprecated)](../SIMD-API/c_api/deprecated_interface/asc_copy_l0c2l1_sync_arch_2201_deprecated.md) API

    `asc_copy_l0c2l1_sync()` is deprecated. Please use the [corresponding non-synchronous API](../SIMD-API/c_api/cube_datamove/asc_copy_l0c2l1/asc_copy_l0c2l1_arch_2201.md) and [asc_sync](../SIMD-API/c_api/sync/intra_core_sync/asc_sync.md) instead.

- [asc_copy_l0c2l1_sync_arch_3510 (deprecated)](../SIMD-API/c_api/deprecated_interface/asc_copy_l0c2l1_sync_arch_3510_deprecated.md) API

    `asc_copy_l0c2l1_sync()` is deprecated. Please use the [corresponding non-synchronous API](../SIMD-API/c_api/cube_datamove/asc_copy_l0c2l1/asc_copy_l0c2l1_arch_3510.md) and [asc_sync](../SIMD-API/c_api/sync/intra_core_sync/asc_sync.md) instead.

- [asc_copy_l12bt_sync_arch_2201 (deprecated)](../SIMD-API/c_api/deprecated_interface/asc_copy_l12bt_sync_arch_2201_deprecated.md) API

    `asc_copy_l12bt_sync()` is deprecated. Please use the [corresponding non-synchronous API](../SIMD-API/c_api/cube_datamove/asc_copy_l12bt/asc_copy_l12bt_arch_2201.md) and [asc_sync](../SIMD-API/c_api/sync/intra_core_sync/asc_sync.md) instead.

- [asc_copy_l12bt_sync_arch_3510 (deprecated)](../SIMD-API/c_api/deprecated_interface/asc_copy_l12bt_sync_arch_3510_deprecated.md) API

    `asc_copy_l12bt_sync()` is deprecated. Please use the [corresponding non-synchronous API](../SIMD-API/c_api/cube_datamove/asc_copy_l12bt/asc_copy_l12bt_arch_3510.md) and [asc_sync](../SIMD-API/c_api/sync/intra_core_sync/asc_sync.md) instead.

- [asc_copy_l12fb_sync_arch_2201 (deprecated)](../SIMD-API/c_api/deprecated_interface/asc_copy_l12fb_sync_arch_2201_deprecated.md) API

    `asc_copy_l12fb_sync()` is deprecated. Please use the [corresponding non-synchronous API](../SIMD-API/c_api/cube_datamove/asc_copy_l12fb/asc_copy_l12fb_arch_2201.md) and the [asc_sync](../SIMD-API/c_api/sync/intra_core_sync/asc_sync.md) API instead.

- [asc_copy_l12fb_sync_arch_3510 (deprecated)](../SIMD-API/c_api/deprecated_interface/asc_copy_l12fb_sync_arch_3510_deprecated.md) API

    `asc_copy_l12fb_sync()` is deprecated. Please use the [corresponding non-synchronous API](../SIMD-API/c_api/cube_datamove/asc_copy_l12fb/asc_copy_l12fb_arch_3510.md) and the [asc_sync](../SIMD-API/c_api/sync/intra_core_sync/asc_sync.md) API instead.

- [asc_copy_l12gm_sync (deprecated)](../SIMD-API/c_api/deprecated_interface/asc_copy_l12gm_sync_deprecated.md) API

    `asc_copy_l12gm_sync()` is deprecated. Please use the [corresponding non-synchronous API](../SIMD-API/c_api/cube_datamove/asc_copy_l12gm.md) and the [asc_sync](../SIMD-API/c_api/sync/intra_core_sync/asc_sync.md) API instead.

- [asc_copy_l12l0a_mx_sync (deprecated)](../SIMD-API/c_api/deprecated_interface/asc_copy_l12l0a_mx_sync_deprecated.md) API

    `asc_copy_l12l0a_mx_sync()` is deprecated. Please use the [corresponding non-synchronous API](../SIMD-API/c_api/cube_datamove/asc_copy_l12l0a_mx.md) and the [asc_sync](../SIMD-API/c_api/sync/intra_core_sync/asc_sync.md) API instead.

- [asc_copy_l12l0a_sync_2d_arch_3510 (deprecated)](../SIMD-API/c_api/deprecated_interface/asc_copy_l12l0a_sync_2d_arch_3510_deprecated.md) API

    `asc_copy_l12l0a_sync()` and `asc_copy_l12l0a_transpose_sync()` are deprecated. Please use the [corresponding non-synchronous API](../SIMD-API/c_api/cube_datamove/asc_copy_l12l0a/asc_copy_l12l0a_2d_arch_3510.md) and the [asc_sync](../SIMD-API/c_api/sync/intra_core_sync/asc_sync.md) API instead.

- [asc_copy_l12l0a_sync_3d_arch_3510 (deprecated)](../SIMD-API/c_api/deprecated_interface/asc_copy_l12l0a_sync_3d_arch_3510_deprecated.md) API

    `asc_copy_l12l0a_sync()` is deprecated. Please use [the corresponding non-synchronous API](../SIMD-API/c_api/cube_datamove/asc_copy_l12l0a/asc_copy_l12l0a_3d_arch_3510.md) and the [asc_sync](../SIMD-API/c_api/sync/intra_core_sync/asc_sync.md) API instead.

- [asc_copy_l12l0a_sync_arch_2201 (deprecated)](../SIMD-API/c_api/deprecated_interface/asc_copy_l12l0a_sync_arch_2201_deprecated.md) API

    `asc_copy_l12l0a_sync()` is deprecated. Please use [the corresponding non-synchronous API](../SIMD-API/c_api/cube_datamove/asc_copy_l12l0a/asc_copy_l12l0a_arch_2201.md) and the [asc_sync](../SIMD-API/c_api/sync/intra_core_sync/asc_sync.md) API instead.

- [asc_copy_l12l0a_trans_sync (deprecated)](../SIMD-API/c_api/deprecated_interface/asc_copy_l12l0a_trans_sync_deprecated.md) API

    `asc_copy_l12l0a_trans_sync()` is deprecated. Please use [the corresponding non-synchronous API](../SIMD-API/c_api/cube_datamove/asc_copy_l12l0a_trans.md) and the [asc_sync](../SIMD-API/c_api/sync/intra_core_sync/asc_sync.md) API instead.

- [asc_copy_l12l0b_mx_sync (deprecated)](../SIMD-API/c_api/deprecated_interface/asc_copy_l12l0b_mx_sync_deprecated.md) API

    `asc_copy_l12l0b_mx_sync()` is deprecated. Please use [the corresponding non-synchronous API](../SIMD-API/c_api/cube_datamove/asc_copy_l12l0b_mx.md) and the [asc_sync](../SIMD-API/c_api/sync/intra_core_sync/asc_sync.md) API instead.

- [asc_copy_l12l0b_sparse_sync (deprecated)](../SIMD-API/c_api/deprecated_interface/asc_copy_l12l0b_sparse_sync_deprecated.md) API

    `asc_copy_l12l0b_sparse_sync()` is deprecated. Please use [the corresponding non-synchronous API](../SIMD-API/c_api/cube_datamove/asc_copy_l12l0b_sparse.md) and the [asc_sync](../SIMD-API/c_api/sync/intra_core_sync/asc_sync.md) API instead.

- [asc_copy_l12l0b_sync_2d_arch_3510 (deprecated)](../SIMD-API/c_api/deprecated_interface/asc_copy_l12l0b_sync_2d_arch_3510_deprecated.md) API

    `asc_copy_l12l0b_sync()` and `asc_copy_l12l0b_transpose_sync()` are deprecated. Please use the [corresponding non-synchronous API](../SIMD-API/c_api/cube_datamove/asc_copy_l12l0b/asc_copy_l12l0b_2d_arch_3510.md) and [asc_sync](../SIMD-API/c_api/sync/intra_core_sync/asc_sync.md) APIs instead.

- [asc_copy_l12l0b_sync_3d_arch_3510 (deprecated)](../SIMD-API/c_api/deprecated_interface/asc_copy_l12l0b_sync_3d_arch_3510_deprecated.md) API

    `asc_copy_l12l0b_sync()` is deprecated. Please use the [corresponding non-synchronous API](../SIMD-API/c_api/cube_datamove/asc_copy_l12l0b/asc_copy_l12l0b_3d_arch_3510.md) and [asc_sync](../SIMD-API/c_api/sync/intra_core_sync/asc_sync.md) APIs instead.

- [asc_copy_l12l0b_sync_arch_2201 (deprecated)](../SIMD-API/c_api/deprecated_interface/asc_copy_l12l0b_sync_arch_2201_deprecated.md) API

    `asc_copy_l12l0b_sync()` is deprecated. Please use the [corresponding non-synchronous API](../SIMD-API/c_api/cube_datamove/asc_copy_l12l0b/asc_copy_l12l0b_arch_2201.md) and [asc_sync](../SIMD-API/c_api/sync/intra_core_sync/asc_sync.md) APIs instead.

- [asc_copy_l12l0b_trans_sync_arch_2201 (deprecated)](../SIMD-API/c_api/deprecated_interface/asc_copy_l12l0b_trans_sync_arch_2201_deprecated.md) API

    `asc_copy_l12l0b_trans_sync()` is deprecated. Please use the [corresponding non-synchronous API](../SIMD-API/c_api/cube_datamove/asc_copy_l12l0b_trans/asc_copy_l12l0b_trans_arch_2201.md) and [asc_sync](../SIMD-API/c_api/sync/intra_core_sync/asc_sync.md) APIs instead.

- [asc_copy_l12l0b_trans_sync_arch_3510 (deprecated)](../SIMD-API/c_api/deprecated_interface/asc_copy_l12l0b_trans_sync_arch_3510_deprecated.md) API

    `asc_copy_l12l0b_trans_sync()` is deprecated. Please use the [corresponding non-synchronous API](../SIMD-API/c_api/cube_datamove/asc_copy_l12l0b_trans/asc_copy_l12l0b_trans_arch_3510.md) and [asc_sync](../SIMD-API/c_api/sync/intra_core_sync/asc_sync.md) APIs instead.

- [asc_copy_l12l0c_sync (deprecated)](../SIMD-API/c_api/deprecated_interface/asc_copy_l12l0c_sync_deprecated.md) API

    `asc_copy_l12l0c_sync()` is deprecated. Please use the [corresponding non-synchronous API](../SIMD-API/c_api/cube_datamove/asc_copy_l12l0c.md) and the [asc_sync](../SIMD-API/c_api/sync/intra_core_sync/asc_sync.md) API instead.

- [asc_copy_l12ub_sync (deprecated)](../SIMD-API/c_api/deprecated_interface/asc_copy_l12ub_sync_deprecated.md) API

    `asc_copy_l12ub_sync()` is deprecated. Please use the [corresponding non-synchronous API](../SIMD-API/c_api/cube_datamove/asc_copy_l12ub.md) and the [asc_sync](../SIMD-API/c_api/sync/intra_core_sync/asc_sync.md) API instead.

- [asc_fill_l0a_sync (deprecated)](../SIMD-API/c_api/deprecated_interface/asc_fill_l0a_sync_deprecated.md) API

    `asc_fill_l0a_sync()` is deprecated. Please use the [corresponding non-synchronous API](../SIMD-API/c_api/cube_datamove/asc_fill_l0a.md) and the [asc_sync](../SIMD-API/c_api/sync/intra_core_sync/asc_sync.md) API instead.

- [asc_fill_l0b_sync (deprecated)](../SIMD-API/c_api/deprecated_interface/asc_fill_l0b_sync_deprecated.md) API

    `asc_fill_l0b_sync()` is deprecated. Please use the [corresponding non-synchronous API](../SIMD-API/c_api/cube_datamove/asc_fill_l0b.md) and the [asc_sync](../SIMD-API/c_api/sync/intra_core_sync/asc_sync.md) API instead.

- [asc_fill_l1_sync (deprecated)](../SIMD-API/c_api/deprecated_interface/asc_fill_l1_sync_deprecated.md) API

    `asc_fill_l1_sync()` is deprecated. Please use the [corresponding non-synchronous API](../SIMD-API/c_api/cube_datamove/asc_fill_l1.md) and the [asc_sync](../SIMD-API/c_api/sync/intra_core_sync/asc_sync.md) API instead.

- [asc_load_image_to_cbuf_sync (deprecated)](../SIMD-API/c_api/deprecated_interface/asc_load_image_to_cbuf_sync_deprecated.md) API

    `asc_load_image_to_cbuf_sync()` is deprecated. Please use the [corresponding non-synchronous API](../SIMD-API/c_api/cube_datamove/asc_load_image_to_cbuf.md) and the [asc_sync](../SIMD-API/c_api/sync/intra_core_sync/asc_sync.md) API as a replacement.

### Vector Data Movement

<!-- npu="950" id7 -->
- [asc_set_gm2ub_pad](../SIMD-API/c_api/deprecated_interface/asc_set_gm2ub_pad_deprecated.md) API

    This API will be deprecated in subsequent versions. Please use the [asc_set_copy_pad_val](../SIMD-API/c_api/vector_datamove/asc_set_copy_pad_val.md) API instead.
<!-- end id7 -->

<!-- npu="950" id8 -->
- [asc_copy_gm2ub_align](../SIMD-API/c_api/deprecated_interface/asc_copy_gm2ub_align_arch_3510_deprecated.md) API (only the original high-dimensional split-and-move function prototype on Ascend 950PR&950DT products)

    On Ascend 950PR&950DT products, the original high-dimensional split-and-move function prototype of this API has been deprecated. Please use the [asc_load_l2_cache_mode](../SIMD-API/c_api/defs/enum/asc_load_l2_cache_mode.md) type enumeration value to configure the L2 cache management policy.
<!-- end id8 -->

<!-- npu="950" id9 -->
- [asc_copy_ub2gm_align](../SIMD-API/c_api/deprecated_interface/asc_copy_ub2gm_align_arch_3510_deprecated.md) API (only the original high-dimensional split-and-move function prototype on Ascend 950PR&950DT products)

    On Ascend 950PR&950DT products, the original high-dimensional split-and-move function prototype of this API has been deprecated. Please use the [asc_store_l2_cache_mode](../SIMD-API/c_api/defs/enum/asc_store_l2_cache_mode.md) type enumeration value to configure the L2 cache management policy.
<!-- end id9 -->

<!-- npu="950" id10 -->
- [asc_ndim_copy_gm2ub](../SIMD-API/c_api/deprecated_interface/asc_ndim_copy_gm2ub_deprecated.md) API

    The original function prototype of this API has been deprecated. Please use the [asc_load_l2_cache_mode](../SIMD-API/c_api/defs/enum/asc_load_l2_cache_mode.md) type enumeration value to configure the L2 cache management policy.
<!-- end id10 -->

- [asc_copy_gm2ub_align_sync_arch_2201 (deprecated)](../SIMD-API/c_api/deprecated_interface/asc_copy_gm2ub_align_sync_arch_2201_deprecated.md) API

    `asc_copy_gm2ub_align_sync()` is deprecated. Use the [corresponding non-synchronous API](../SIMD-API/c_api/vector_datamove/asc_copy_gm2ub_align/asc_copy_gm2ub_align_arch_2201.md) and [asc_sync](../SIMD-API/c_api/sync/intra_core_sync/asc_sync.md) API instead.

- [asc_copy_gm2ub_align_sync_arch_3510 (deprecated)](../SIMD-API/c_api/deprecated_interface/asc_copy_gm2ub_align_sync_arch_3510_deprecated.md) API

    `asc_copy_gm2ub_align_sync()` is deprecated. Use the [corresponding non-synchronous API](../SIMD-API/c_api/vector_datamove/asc_copy_gm2ub_align/asc_copy_gm2ub_align_arch_3510.md) and [asc_sync](../SIMD-API/c_api/sync/intra_core_sync/asc_sync.md) API instead.

- [asc_copy_gm2ub_sync_arch_2201 (deprecated)](../SIMD-API/c_api/deprecated_interface/asc_copy_gm2ub_sync_arch_2201_deprecated.md) API

    `asc_copy_gm2ub_sync()` is deprecated. Use the [corresponding non-synchronous API](../SIMD-API/c_api/vector_datamove/asc_copy_gm2ub/asc_copy_gm2ub_arch_2201.md) and [asc_sync](../SIMD-API/c_api/sync/intra_core_sync/asc_sync.md) API instead.

- [asc_copy_gm2ub_sync_arch_3510 (deprecated)](../SIMD-API/c_api/deprecated_interface/asc_copy_gm2ub_sync_arch_3510_deprecated.md) API

    `asc_copy_gm2ub_sync()` is deprecated. Use the [corresponding non-synchronous API](../SIMD-API/c_api/vector_datamove/asc_copy_gm2ub/asc_copy_gm2ub_arch_3510.md) and [asc_sync](../SIMD-API/c_api/sync/intra_core_sync/asc_sync.md) API instead.

- [asc_copy_sync (deprecated)](../SIMD-API/c_api/deprecated_interface/asc_copy_sync_deprecated.md) API

    `asc_copy_sync()` is deprecated. Use the [corresponding non-synchronous API](../SIMD-API/c_api/vector_datamove/asc_copy.md) and [asc_sync](../SIMD-API/c_api/sync/intra_core_sync/asc_sync.md) API instead.

- [asc_copy_ub2gm_align_sync_arch_2201 (deprecated)](../SIMD-API/c_api/deprecated_interface/asc_copy_ub2gm_align_sync_arch_2201_deprecated.md) API

    `asc_copy_ub2gm_align_sync()` is deprecated. Please use [the corresponding non-synchronous API](../SIMD-API/c_api/vector_datamove/asc_copy_ub2gm_align/asc_copy_ub2gm_align_arch_2201.md) and [asc_sync](../SIMD-API/c_api/sync/intra_core_sync/asc_sync.md) instead.

- [asc_copy_ub2gm_align_sync_arch_3510 (deprecated)](../SIMD-API/c_api/deprecated_interface/asc_copy_ub2gm_align_sync_arch_3510_deprecated.md) API

    `asc_copy_ub2gm_align_sync()` is deprecated. Please use [the corresponding non-synchronous API](../SIMD-API/c_api/vector_datamove/asc_copy_ub2gm_align/asc_copy_ub2gm_align_arch_3510.md) and [asc_sync](../SIMD-API/c_api/sync/intra_core_sync/asc_sync.md) instead.

- [asc_copy_ub2gm_sync_arch_2201 (deprecated)](../SIMD-API/c_api/deprecated_interface/asc_copy_ub2gm_sync_arch_2201_deprecated.md) API

    `asc_copy_ub2gm_sync()` is deprecated. Please use [the corresponding non-synchronous API](../SIMD-API/c_api/vector_datamove/asc_copy_ub2gm/asc_copy_ub2gm_arch_2201.md) and [asc_sync](../SIMD-API/c_api/sync/intra_core_sync/asc_sync.md) instead.

- [asc_copy_ub2gm_sync_arch_3510 (deprecated)](../SIMD-API/c_api/deprecated_interface/asc_copy_ub2gm_sync_arch_3510_deprecated.md) API

    `asc_copy_ub2gm_sync()` is deprecated. Please use [the corresponding non-synchronous API](../SIMD-API/c_api/vector_datamove/asc_copy_ub2gm/asc_copy_ub2gm_arch_3510.md) and [asc_sync](../SIMD-API/c_api/sync/intra_core_sync/asc_sync.md) instead.

- [asc_copy_ub2l1_sync (deprecated)](../SIMD-API/c_api/deprecated_interface/asc_copy_ub2l1_sync_deprecated.md) API

    `asc_copy_ub2l1_sync()` is deprecated. Please use [asc_copy_ub2l1](../SIMD-API/c_api/vector_datamove/asc_copy_ub2l1.md) and [asc_sync](../SIMD-API/c_api/sync/intra_core_sync/asc_sync.md) instead.

- [asc_copy_ub2ub_sync (deprecated)](../SIMD-API/c_api/deprecated_interface/asc_copy_ub2ub_sync_deprecated.md) API

    `asc_copy_ub2ub_sync()` is deprecated. Please use the [corresponding non-synchronous API](../SIMD-API/c_api/vector_datamove/asc_copy_ub2ub.md) and [asc_sync](../SIMD-API/c_api/sync/intra_core_sync/asc_sync.md) APIs instead.

### Memory Vector Calculation

#### Basic Arithmetic

- [asc_abs_sync](../SIMD-API/c_api/deprecated_interface/asc_abs_sync_deprecated.md) API

    The `asc_abs_sync()` API is deprecated. Please use the [asc_abs](../SIMD-API/c_api/vector_compute/vector_arith/asc_abs.md) API and the [asc_sync](../SIMD-API/c_api/sync/intra_core_sync/asc_sync.md) API.

- [asc_add_scalar_sync (deprecated)](../SIMD-API/c_api/deprecated_interface/asc_add_scalar_sync_deprecated.md) API

    `asc_add_scalar_sync()` is deprecated. Please use the [corresponding non-synchronous API](../SIMD-API/c_api/vector_compute/vector_arith/asc_add_scalar.md) and the [asc_sync](../SIMD-API/c_api/sync/intra_core_sync/asc_sync.md) API instead.

- [asc_add_sync (deprecated)](../SIMD-API/c_api/deprecated_interface/asc_add_sync_deprecated.md) API

    `asc_add_sync()` is deprecated. Please use the [corresponding non-synchronous API](../SIMD-API/c_api/vector_compute/vector_arith/asc_add.md) and the [asc_sync](../SIMD-API/c_api/sync/intra_core_sync/asc_sync.md) API instead.

- [asc_div_sync (deprecated)](../SIMD-API/c_api/deprecated_interface/asc_div_sync_deprecated.md) API

    `asc_div_sync()` is deprecated. Please use the [corresponding non-synchronous API](../SIMD-API/c_api/vector_compute/vector_arith/asc_div.md) and the [asc_sync](../SIMD-API/c_api/sync/intra_core_sync/asc_sync.md) API instead.

- [asc_exp_sync (deprecated)](../SIMD-API/c_api/deprecated_interface/asc_exp_sync_deprecated.md) API

    `asc_exp_sync()` is deprecated. Please use [the corresponding non-synchronous API](../SIMD-API/c_api/vector_compute/vector_arith/asc_exp.md) and the [asc_sync](../SIMD-API/c_api/sync/intra_core_sync/asc_sync.md) API instead.

- [asc_log_sync (deprecated)](../SIMD-API/c_api/deprecated_interface/asc_log_sync_deprecated.md) API

    `asc_log_sync()` is deprecated. Please use [the corresponding non-synchronous API](../SIMD-API/c_api/vector_compute/vector_arith/asc_log.md) and the [asc_sync](../SIMD-API/c_api/sync/intra_core_sync/asc_sync.md) API instead.

- [asc_max_scalar_sync (deprecated)](../SIMD-API/c_api/deprecated_interface/asc_max_scalar_sync_deprecated.md) API

    `asc_max_scalar_sync()` is deprecated. Please use [the corresponding non-synchronous API](../SIMD-API/c_api/vector_compute/vector_arith/asc_max_scalar.md) and the [asc_sync](../SIMD-API/c_api/sync/intra_core_sync/asc_sync.md) API instead.

- [asc_max_sync (deprecated)](../SIMD-API/c_api/deprecated_interface/asc_max_sync_deprecated.md) API

    `asc_max_sync()` is deprecated. Please use [the corresponding non-synchronous API](../SIMD-API/c_api/vector_compute/vector_arith/asc_max.md) and the [asc_sync](../SIMD-API/c_api/sync/intra_core_sync/asc_sync.md) API instead.

- [asc_min_scalar_sync (deprecated)](../SIMD-API/c_api/deprecated_interface/asc_min_scalar_sync_deprecated.md) API

    `asc_min_scalar_sync()` is deprecated. Please use [the corresponding non-synchronous API](../SIMD-API/c_api/vector_compute/vector_arith/asc_min_scalar.md) and the [asc_sync](../SIMD-API/c_api/sync/intra_core_sync/asc_sync.md) API instead.

- [asc_min_sync (deprecated)](../SIMD-API/c_api/deprecated_interface/asc_min_sync_deprecated.md) API

    `asc_min_sync()` is deprecated. Please use the [corresponding non-synchronous API](../SIMD-API/c_api/vector_compute/vector_arith/asc_min.md) and the [asc_sync](../SIMD-API/c_api/sync/intra_core_sync/asc_sync.md) API instead.

- [asc_mul_scalar_sync (deprecated)](../SIMD-API/c_api/deprecated_interface/asc_mul_scalar_sync_deprecated.md) API

    `asc_mul_scalar_sync()` is deprecated. Please use the [corresponding non-synchronous API](../SIMD-API/c_api/vector_compute/vector_arith/asc_mul_scalar.md) and the [asc_sync](../SIMD-API/c_api/sync/intra_core_sync/asc_sync.md) API instead.

- [asc_mul_sync (deprecated)](../SIMD-API/c_api/deprecated_interface/asc_mul_sync_deprecated.md) API

    `asc_mul_sync()` is deprecated. Please use the [corresponding non-synchronous API](../SIMD-API/c_api/vector_compute/vector_arith/asc_mul.md) and the [asc_sync](../SIMD-API/c_api/sync/intra_core_sync/asc_sync.md) API instead.

- [asc_rcp_sync (deprecated)](../SIMD-API/c_api/deprecated_interface/asc_rcp_sync_deprecated.md) API

    `asc_rcp_sync()` is deprecated. Please use the [corresponding non-synchronous API](../SIMD-API/c_api/vector_compute/vector_arith/asc_rcp.md) and the [asc_sync](../SIMD-API/c_api/sync/intra_core_sync/asc_sync.md) API instead.

- [asc_rsqrt_sync (deprecated)](../SIMD-API/c_api/deprecated_interface/asc_rsqrt_sync_deprecated.md) API

    `asc_rsqrt_sync()` is deprecated. Please use the [corresponding non-synchronous API](../SIMD-API/c_api/vector_compute/vector_arith/asc_rsqrt.md) and the [asc_sync](../SIMD-API/c_api/sync/intra_core_sync/asc_sync.md) API instead.

- [asc_sqrt_sync (deprecated)](../SIMD-API/c_api/deprecated_interface/asc_sqrt_sync_deprecated.md) API

    `asc_sqrt_sync()` is deprecated. Please use the [corresponding non-synchronous API](../SIMD-API/c_api/vector_compute/vector_arith/asc_sqrt.md) and the [asc_sync](../SIMD-API/c_api/sync/intra_core_sync/asc_sync.md) API instead.

- [asc_sub_scalar_sync (deprecated)](../SIMD-API/c_api/deprecated_interface/asc_sub_scalar_sync_deprecated.md) API

    `asc_sub_scalar_sync()` is deprecated. Please use the [corresponding non-synchronous API](../SIMD-API/c_api/vector_compute/vector_arith/asc_sub_scalar.md) and the [asc_sync](../SIMD-API/c_api/sync/intra_core_sync/asc_sync.md) API instead.

- [asc_sub_sync (deprecated)](../SIMD-API/c_api/deprecated_interface/asc_sub_sync_deprecated.md) API

    `asc_sub_sync()` is deprecated. Please use the [corresponding non-synchronous API](../SIMD-API/c_api/vector_compute/vector_arith/asc_sub.md) and the [asc_sync](../SIMD-API/c_api/sync/intra_core_sync/asc_sync.md) API instead.

#### Broadcast Operation

- [asc_brcb_sync (deprecated)](../SIMD-API/c_api/deprecated_interface/asc_brcb_sync_deprecated.md) API

    `asc_brcb_sync()` is deprecated. Please use the [corresponding non-synchronous API](../SIMD-API/c_api/vector_compute/vector_broadcast/asc_brcb.md) and [asc_sync](../SIMD-API/c_api/sync/intra_core_sync/asc_sync.md) instead.

- [asc_duplicate_sync (deprecated)](../SIMD-API/c_api/deprecated_interface/asc_duplicate_sync_deprecated.md) API

    `asc_duplicate_sync()` is deprecated. Please use the [corresponding non-synchronous API](../SIMD-API/c_api/vector_compute/vector_broadcast/asc_duplicate.md) and [asc_sync](../SIMD-API/c_api/sync/intra_core_sync/asc_sync.md) instead.

#### Comparison Calculation

- [asc_eq_scalar_sync (deprecated)](../SIMD-API/c_api/deprecated_interface/asc_eq_scalar_sync_deprecated.md) API

    `asc_eq_scalar_sync()` is deprecated. Please use the [corresponding non-synchronous API](../SIMD-API/c_api/vector_compute/vector_compare/asc_eq_scalar.md) and [asc_sync](../SIMD-API/c_api/sync/intra_core_sync/asc_sync.md) instead.

- [asc_eq_sync (deprecated)](../SIMD-API/c_api/deprecated_interface/asc_eq_sync_deprecated.md) API

    `asc_eq_sync()` is deprecated. Please use the [corresponding non-synchronous API](../SIMD-API/c_api/vector_compute/vector_compare/asc_eq.md) and [asc_sync](../SIMD-API/c_api/sync/intra_core_sync/asc_sync.md) instead.

- [asc_ge_scalar_sync (deprecated)](../SIMD-API/c_api/deprecated_interface/asc_ge_scalar_sync_deprecated.md) API

    `asc_ge_scalar_sync()` is deprecated. Please use the [corresponding non-synchronous API](../SIMD-API/c_api/vector_compute/vector_compare/asc_ge_scalar.md) and [asc_sync](../SIMD-API/c_api/sync/intra_core_sync/asc_sync.md) instead.

- [asc_ge_sync (deprecated)](../SIMD-API/c_api/deprecated_interface/asc_ge_sync_deprecated.md) API

    `asc_ge_sync()` is deprecated. Please use the [corresponding non-synchronous API](../SIMD-API/c_api/vector_compute/vector_compare/asc_ge.md) and [asc_sync](../SIMD-API/c_api/sync/intra_core_sync/asc_sync.md) instead.

- [asc_gt_scalar_sync (deprecated)](../SIMD-API/c_api/deprecated_interface/asc_gt_scalar_sync_deprecated.md) API

    `asc_gt_scalar_sync()` is deprecated. Please use [the corresponding non-synchronous API](../SIMD-API/c_api/vector_compute/vector_compare/asc_gt_scalar.md) and [asc_sync](../SIMD-API/c_api/sync/intra_core_sync/asc_sync.md) instead.

- [asc_gt_sync (deprecated)](../SIMD-API/c_api/deprecated_interface/asc_gt_sync_deprecated.md) API

    `asc_gt_sync()` is deprecated. Please use [the corresponding non-synchronous API](../SIMD-API/c_api/vector_compute/vector_compare/asc_gt.md) and [asc_sync](../SIMD-API/c_api/sync/intra_core_sync/asc_sync.md) instead.

- [asc_le_scalar_sync (deprecated)](../SIMD-API/c_api/deprecated_interface/asc_le_scalar_sync_deprecated.md) API

    `asc_le_scalar_sync()` is deprecated. Please use [the corresponding non-synchronous API](../SIMD-API/c_api/vector_compute/vector_compare/asc_le_scalar.md) and [asc_sync](../SIMD-API/c_api/sync/intra_core_sync/asc_sync.md) instead.

- [asc_le_sync (deprecated)](../SIMD-API/c_api/deprecated_interface/asc_le_sync_deprecated.md) API

    `asc_le_sync()` is deprecated. Please use [the corresponding non-synchronous API](../SIMD-API/c_api/vector_compute/vector_compare/asc_le.md) and [asc_sync](../SIMD-API/c_api/sync/intra_core_sync/asc_sync.md) instead.

- [asc_lt_scalar_sync (deprecated)](../SIMD-API/c_api/deprecated_interface/asc_lt_scalar_sync_deprecated.md) API

    `asc_lt_scalar_sync()` is deprecated. Please use [the corresponding non-synchronous API](../SIMD-API/c_api/vector_compute/vector_compare/asc_lt_scalar.md) and [asc_sync](../SIMD-API/c_api/sync/intra_core_sync/asc_sync.md) instead.

- [asc_lt_sync (deprecated)](../SIMD-API/c_api/deprecated_interface/asc_lt_sync_deprecated.md) API

    `asc_lt_sync()` is deprecated. Please use the [corresponding non-synchronous API](../SIMD-API/c_api/vector_compute/vector_compare/asc_lt.md) and the [asc_sync](../SIMD-API/c_api/sync/intra_core_sync/asc_sync.md) API instead.

- [asc_ne_scalar_sync (deprecated)](../SIMD-API/c_api/deprecated_interface/asc_ne_scalar_sync_deprecated.md) API

    `asc_ne_scalar_sync()` is deprecated. Please use the [corresponding non-synchronous API](../SIMD-API/c_api/vector_compute/vector_compare/asc_ne_scalar.md) and the [asc_sync](../SIMD-API/c_api/sync/intra_core_sync/asc_sync.md) API instead.

- [asc_ne_sync (deprecated)](../SIMD-API/c_api/deprecated_interface/asc_ne_sync_deprecated.md) API

    `asc_ne_sync()` is deprecated. Please use the [corresponding non-synchronous API](../SIMD-API/c_api/vector_compute/vector_compare/asc_ne.md) and the [asc_sync](../SIMD-API/c_api/sync/intra_core_sync/asc_sync.md) API instead.

#### Type Conversion

- [asc_bfloat162float_sync (deprecated)](../SIMD-API/c_api/deprecated_interface/asc_bfloat162float_sync_deprecated.md) API

    `asc_bfloat162float_sync()` is deprecated. Use the [corresponding non-synchronous API](../SIMD-API/c_api/vector_compute/vector_convert/asc_bfloat162float.md) and the [asc_sync](../SIMD-API/c_api/sync/intra_core_sync/asc_sync.md) API instead.

- [asc_bfloat162int32_sync (deprecated)](../SIMD-API/c_api/deprecated_interface/asc_bfloat162int32_sync_deprecated.md) API

    `asc_bfloat162int32_rn_sync()`, `asc_bfloat162int32_rna_sync()`, `asc_bfloat162int32_rd_sync()`, `asc_bfloat162int32_ru_sync()`, and `asc_bfloat162int32_rz_sync()` are deprecated. Use the [corresponding non-synchronous API](../SIMD-API/c_api/vector_compute/vector_convert/asc_bfloat162int32.md) and the [asc_sync](../SIMD-API/c_api/sync/intra_core_sync/asc_sync.md) API instead.

- [asc_float2bfloat16_sync (deprecated)](../SIMD-API/c_api/deprecated_interface/asc_float2bfloat16_sync_deprecated.md) API

    `asc_float2bfloat16_rn_sync()`, `asc_float2bfloat16_rna_sync()`, `asc_float2bfloat16_rd_sync()`, `asc_float2bfloat16_ru_sync()`, and `asc_float2bfloat16_rz_sync()` are deprecated. Use the [corresponding non-synchronous API](../SIMD-API/c_api/vector_compute/vector_convert/asc_float2bfloat16.md) and the [asc_sync](../SIMD-API/c_api/sync/intra_core_sync/asc_sync.md) API instead.

- [asc_float2float_sync (deprecated)](../SIMD-API/c_api/deprecated_interface/asc_float2float_sync_deprecated.md) API

    `asc_float2float_rn_sync()`, `asc_float2float_rd_sync()`, `asc_float2float_ru_sync()`, `asc_float2float_rna_sync()`, and `asc_float2float_rz_sync()` are deprecated. Use the [corresponding non-synchronous API](../SIMD-API/c_api/vector_compute/vector_convert/asc_float2float.md) and the [asc_sync](../SIMD-API/c_api/sync/intra_core_sync/asc_sync.md) API instead.

- [asc_float2half_sync (deprecated)](../SIMD-API/c_api/deprecated_interface/asc_float2half_sync_deprecated.md) API

    `asc_float2half_sync()`, `asc_float2half_rn_sync()`, `asc_float2half_rna_sync()`, `asc_float2half_rd_sync()`, `asc_float2half_ru_sync()`, `asc_float2half_rz_sync()`, and `asc_float2half_ro_sync()` are deprecated. Please use the [corresponding non-synchronous API](../SIMD-API/c_api/vector_compute/vector_convert/asc_float2half.md) and the [asc_sync](../SIMD-API/c_api/sync/intra_core_sync/asc_sync.md) API instead.

- [asc_float2int16_sync (deprecated)](../SIMD-API/c_api/deprecated_interface/asc_float2int16_sync_deprecated.md) API

    `asc_float2int16_rn_sync()`, `asc_float2int16_rna_sync()`, `asc_float2int16_rd_sync()`, `asc_float2int16_ru_sync()`, and `asc_float2int16_rz_sync()` are deprecated. Please use the [corresponding non-synchronous API](../SIMD-API/c_api/vector_compute/vector_convert/asc_float2int16.md) and the [asc_sync](../SIMD-API/c_api/sync/intra_core_sync/asc_sync.md) API instead.

- [asc_float2int32_sync (deprecated)](../SIMD-API/c_api/deprecated_interface/asc_float2int32_sync_deprecated.md) API

    `asc_float2int32_rn_sync()`, `asc_float2int32_rna_sync()`, `asc_float2int32_rd_sync()`, `asc_float2int32_ru_sync()`, and `asc_float2int32_rz_sync()` are deprecated. Please use the [corresponding non-synchronous API](../SIMD-API/c_api/vector_compute/vector_convert/asc_float2int32.md) and the [asc_sync](../SIMD-API/c_api/sync/intra_core_sync/asc_sync.md) API instead.

- [asc_float2int64_sync (deprecated)](../SIMD-API/c_api/deprecated_interface/asc_float2int64_sync_deprecated.md) API

    `asc_float2int64_rn_sync()`, `asc_float2int64_rna_sync()`, `asc_float2int64_rd_sync()`, `asc_float2int64_ru_sync()`, and `asc_float2int64_rz_sync()` are deprecated. Please use the [corresponding non-synchronous API](../SIMD-API/c_api/vector_compute/vector_convert/asc_float2int64.md) and the [asc_sync](../SIMD-API/c_api/sync/intra_core_sync/asc_sync.md) API instead.

- [asc_half2float_sync (deprecated)](../SIMD-API/c_api/deprecated_interface/asc_half2float_sync_deprecated.md) API

    `asc_half2float_sync()` is deprecated. Please use the [corresponding non-synchronous API](../SIMD-API/c_api/vector_compute/vector_convert/asc_half2float.md) and the [asc_sync](../SIMD-API/c_api/sync/intra_core_sync/asc_sync.md) API instead.

- [asc_half2int16_sync (deprecated)](../SIMD-API/c_api/deprecated_interface/asc_half2int16_sync_deprecated.md) API

    `asc_half2int16_rn_sync()`, `asc_half2int16_rna_sync()`, `asc_half2int16_rd_sync()`, `asc_half2int16_ru_sync()`, and `asc_half2int16_rz_sync()` are deprecated. Please use the [corresponding non-synchronous API](../SIMD-API/c_api/vector_compute/vector_convert/asc_half2int16.md) and the [asc_sync](../SIMD-API/c_api/sync/intra_core_sync/asc_sync.md) API instead.

- [asc_half2int32_sync (deprecated)](../SIMD-API/c_api/deprecated_interface/asc_half2int32_sync_deprecated.md) API

    `asc_half2int32_rn_sync()`, `asc_half2int32_rna_sync()`, `asc_half2int32_rd_sync()`, `asc_half2int32_ru_sync()`, and `asc_half2int32_rz_sync()` are deprecated. Please use the [corresponding non-synchronous API](../SIMD-API/c_api/vector_compute/vector_convert/asc_half2int32.md) and the [asc_sync](../SIMD-API/c_api/sync/intra_core_sync/asc_sync.md) API instead.

- [asc_half2int4_sync (deprecated)](../SIMD-API/c_api/deprecated_interface/asc_half2int4_sync_deprecated.md) API

    `asc_half2int4_sync()`, `asc_half2int4_rn_sync()`, `asc_half2int4_rd_sync()`, `asc_half2int4_rna_sync()`, `asc_half2int4_ru_sync()`, and `asc_half2int4_rz_sync()` are deprecated. Please use the [corresponding non-synchronous API](../SIMD-API/c_api/vector_compute/vector_convert/asc_half2int4.md) and the [asc_sync](../SIMD-API/c_api/sync/intra_core_sync/asc_sync.md) API instead.

- [asc_half2int8_sync (deprecated)](../SIMD-API/c_api/deprecated_interface/asc_half2int8_sync_deprecated.md) API

    `asc_half2int8_sync()`, `asc_half2int8_rn_sync()`, `asc_half2int8_rna_sync()`, `asc_half2int8_rd_sync()`, `asc_half2int8_ru_sync()`, and `asc_half2int8_rz_sync()` are deprecated. Please use the [corresponding non-synchronous API](../SIMD-API/c_api/vector_compute/vector_convert/asc_half2int8.md) and the [asc_sync](../SIMD-API/c_api/sync/intra_core_sync/asc_sync.md) API instead.

- [asc_half2uint8_sync (deprecated)](../SIMD-API/c_api/deprecated_interface/asc_half2uint8_sync_deprecated.md) API

    `asc_half2uint8_sync()`, `asc_half2uint8_rn_sync()`, `asc_half2uint8_rd_sync()`, `asc_half2uint8_rna_sync()`, `asc_half2uint8_ru_sync()`, and `asc_half2uint8_rz_sync()` are deprecated. Please use the [corresponding non-synchronous API](../SIMD-API/c_api/vector_compute/vector_convert/asc_half2uint8.md) and the [asc_sync](../SIMD-API/c_api/sync/intra_core_sync/asc_sync.md) API instead.

- [asc_int162float_sync (deprecated)](../SIMD-API/c_api/deprecated_interface/asc_int162float_sync_deprecated.md) API

    `asc_int162float_sync()` is deprecated. Please use the [corresponding non-synchronous API](../SIMD-API/c_api/vector_compute/vector_convert/asc_int162float.md) and the [asc_sync](../SIMD-API/c_api/sync/intra_core_sync/asc_sync.md) API instead.

- [asc_int162half_sync (deprecated)](../SIMD-API/c_api/deprecated_interface/asc_int162half_sync_deprecated.md) API

    `asc_int162half_sync()`, `asc_int162half_rn_sync()`, `asc_int162half_rna_sync()`, `asc_int162half_rd_sync()`, `asc_int162half_ru_sync()`, and `asc_int162half_rz_sync()` are deprecated. Please use the [corresponding non-synchronous API](../SIMD-API/c_api/vector_compute/vector_convert/asc_int162half.md) and the [asc_sync](../SIMD-API/c_api/sync/intra_core_sync/asc_sync.md) API instead.

- [asc_int322float_sync (deprecated)](../SIMD-API/c_api/deprecated_interface/asc_int322float_sync_deprecated.md) API

    `asc_int322float_sync()`, `asc_int322float_rn_sync()`, `asc_int322float_rna_sync()`, `asc_int322float_rd_sync()`, `asc_int322float_ru_sync()`, and `asc_int322float_rz_sync()` are deprecated. Please use the [corresponding non-synchronous API](../SIMD-API/c_api/vector_compute/vector_convert/asc_int322float.md) and the [asc_sync](../SIMD-API/c_api/sync/intra_core_sync/asc_sync.md) API instead.

- [asc_int322int16_sync (deprecated)](../SIMD-API/c_api/deprecated_interface/asc_int322int16_sync_deprecated.md) API

    `asc_int322int16_sync()` is deprecated. Please use the [corresponding non-synchronous API](../SIMD-API/c_api/vector_compute/vector_convert/asc_int322int16.md) and the [asc_sync](../SIMD-API/c_api/sync/intra_core_sync/asc_sync.md) API instead.

- [asc_int322int64_sync (deprecated)](../SIMD-API/c_api/deprecated_interface/asc_int322int64_sync_deprecated.md) API

    `asc_int322int64_sync()` is deprecated. Please use the [corresponding non-synchronous API](../SIMD-API/c_api/vector_compute/vector_convert/asc_int322int64.md) and the [asc_sync](../SIMD-API/c_api/sync/intra_core_sync/asc_sync.md) API instead.

- [asc_int42half_sync (deprecated)](../SIMD-API/c_api/deprecated_interface/asc_int42half_sync_deprecated.md) API

    `asc_int42half_sync()` is deprecated. Please use the [corresponding non-synchronous API](../SIMD-API/c_api/vector_compute/vector_convert/asc_int42half.md) and the [asc_sync](../SIMD-API/c_api/sync/intra_core_sync/asc_sync.md) API instead.

- [asc_int642float_sync (deprecated)](../SIMD-API/c_api/deprecated_interface/asc_int642float_sync_deprecated.md) API

    `asc_int642float_rd_sync()`, `asc_int642float_rn_sync()`, `asc_int642float_rna_sync()`, `asc_int642float_ru_sync()`, and `asc_int642float_rz_sync()` are deprecated. Please use the [corresponding non-synchronous API](../SIMD-API/c_api/vector_compute/vector_convert/asc_int642float.md) and the [asc_sync](../SIMD-API/c_api/sync/intra_core_sync/asc_sync.md) API instead.

- [asc_int642int32_sync (deprecated)](../SIMD-API/c_api/deprecated_interface/asc_int642int32_sync_deprecated.md) API

    `asc_int642int32_sync()` is deprecated. Please use the [corresponding non-synchronous API](../SIMD-API/c_api/vector_compute/vector_convert/asc_int642int32.md) and the [asc_sync](../SIMD-API/c_api/sync/intra_core_sync/asc_sync.md) API instead.

- [asc_int82half_sync (deprecated)](../SIMD-API/c_api/deprecated_interface/asc_int82half_sync_deprecated.md) API

    `asc_int82half_sync()` is deprecated. Please use the [corresponding non-synchronous API](../SIMD-API/c_api/vector_compute/vector_convert/asc_int82half.md) and the [asc_sync](../SIMD-API/c_api/sync/intra_core_sync/asc_sync.md) API instead.

- [asc_uint82half_sync (deprecated)](../SIMD-API/c_api/deprecated_interface/asc_uint82half_sync_deprecated.md) API

    `asc_uint82half_sync()` is deprecated. Please use the [corresponding non-synchronous API](../SIMD-API/c_api/vector_compute/vector_convert/asc_uint82half.md) and the [asc_sync](../SIMD-API/c_api/sync/intra_core_sync/asc_sync.md) API instead.

#### Compound Calculation

- [asc_add_relu_sync (deprecated)](../SIMD-API/c_api/deprecated_interface/asc_add_relu_sync_deprecated.md) API

    `asc_add_relu_sync()` is deprecated. Please use the [corresponding non-synchronous API](../SIMD-API/c_api/vector_compute/vector_fused/asc_add_relu.md) and the [asc_sync](../SIMD-API/c_api/sync/intra_core_sync/asc_sync.md) API instead.

- [asc_axpy_sync (deprecated)](../SIMD-API/c_api/deprecated_interface/asc_axpy_sync_deprecated.md) API

    `asc_axpy_sync()` is deprecated. Please use the [corresponding non-synchronous API](../SIMD-API/c_api/vector_compute/vector_fused/asc_axpy.md) and the [asc_sync](../SIMD-API/c_api/sync/intra_core_sync/asc_sync.md) API instead.

- [asc_deq_int162b8_sync (deprecated)](../SIMD-API/c_api/deprecated_interface/asc_deq_int162b8_sync_deprecated.md) API

    `asc_deq_int162b8_h_sync()` and `asc_deq_int162b8_l_sync()` are deprecated. Please use the [corresponding non-synchronous API](../SIMD-API/c_api/vector_compute/vector_fused/asc_deq_int162b8.md) and the [asc_sync](../SIMD-API/c_api/sync/intra_core_sync/asc_sync.md) API instead.

- [asc_deq_int322half_sync (deprecated)](../SIMD-API/c_api/deprecated_interface/asc_deq_int322half_sync_deprecated.md) API

    `asc_deq_int322half_sync()` is deprecated. Please use the [corresponding non-synchronous API](../SIMD-API/c_api/vector_compute/vector_fused/asc_deq_int322half.md) and the [asc_sync](../SIMD-API/c_api/sync/intra_core_sync/asc_sync.md) API instead.

- [asc_fma_sync (deprecated)](../SIMD-API/c_api/deprecated_interface/asc_fma_sync_deprecated.md) API

    `asc_fma_sync()` is deprecated. Use the [corresponding non-synchronous API](../SIMD-API/c_api/vector_compute/vector_fused/asc_fma.md) and [asc_sync](../SIMD-API/c_api/sync/intra_core_sync/asc_sync.md) instead.

- [asc_leakyrelu_sync (deprecated)](../SIMD-API/c_api/deprecated_interface/asc_leakyrelu_sync_deprecated.md) API

    `asc_leakyrelu_sync()` is deprecated. Use the [corresponding non-synchronous API](../SIMD-API/c_api/vector_compute/vector_fused/asc_leakyrelu.md) and [asc_sync](../SIMD-API/c_api/sync/intra_core_sync/asc_sync.md) instead.

- [asc_mul_add_relu_sync (deprecated)](../SIMD-API/c_api/deprecated_interface/asc_mul_add_relu_sync_deprecated.md) API

    `asc_mul_add_relu_sync()` is deprecated. Use the [corresponding non-synchronous API](../SIMD-API/c_api/vector_compute/vector_fused/asc_mul_add_relu.md) and [asc_sync](../SIMD-API/c_api/sync/intra_core_sync/asc_sync.md) instead.

- [asc_mul_add_sync (deprecated)](../SIMD-API/c_api/deprecated_interface/asc_mul_add_sync_deprecated.md) API

    `asc_mul_add_sync()` is deprecated. Use the [corresponding non-synchronous API](../SIMD-API/c_api/vector_compute/vector_fused/asc_mul_add.md) and [asc_sync](../SIMD-API/c_api/sync/intra_core_sync/asc_sync.md) instead.

- [asc_mul_cast_half2int8_sync (deprecated)](../SIMD-API/c_api/deprecated_interface/asc_mul_cast_half2int8_sync_deprecated.md) API

    `asc_mul_cast_half2int8_sync()` is deprecated. Use the [corresponding non-synchronous API](../SIMD-API/c_api/vector_compute/vector_fused/asc_mul_cast_half2int8.md) and [asc_sync](../SIMD-API/c_api/sync/intra_core_sync/asc_sync.md) instead.

- [asc_mul_cast_half2uint8_sync (deprecated)](../SIMD-API/c_api/deprecated_interface/asc_mul_cast_half2uint8_sync_deprecated.md) API

    `asc_mul_cast_half2uint8_sync()` is deprecated. Please use the [corresponding non-synchronous API](../SIMD-API/c_api/vector_compute/vector_fused/asc_mul_cast_half2uint8.md) and the [asc_sync](../SIMD-API/c_api/sync/intra_core_sync/asc_sync.md) API instead.

- [asc_relu_sync (deprecated)](../SIMD-API/c_api/deprecated_interface/asc_relu_sync_deprecated.md) API

    `asc_relu_sync()` is deprecated. Please use the [corresponding non-synchronous API](../SIMD-API/c_api/vector_compute/vector_fused/asc_relu.md) and the [asc_sync](../SIMD-API/c_api/sync/intra_core_sync/asc_sync.md) API instead.

- [asc_sub_relu_sync (deprecated)](../SIMD-API/c_api/deprecated_interface/asc_sub_relu_sync_deprecated.md) API

    `asc_sub_relu_sync()` is deprecated. Please use the [corresponding non-synchronous API](../SIMD-API/c_api/vector_compute/vector_fused/asc_sub_relu.md) and the [asc_sync](../SIMD-API/c_api/sync/intra_core_sync/asc_sync.md) API instead.

- [asc_vdeq_int162b8_sync (deprecated)](../SIMD-API/c_api/deprecated_interface/asc_vdeq_int162b8_sync_deprecated.md) API

    `asc_vdeq_int162b8_h_sync()` and `asc_vdeq_int162b8_l_sync()` are deprecated. Please use the [corresponding non-synchronous API](../SIMD-API/c_api/vector_compute/vector_fused/asc_vdeq_int162b8.md) and the [asc_sync](../SIMD-API/c_api/sync/intra_core_sync/asc_sync.md) API instead.

#### Aggregate Operations

- [asc_gather_datablock_sync (deprecated)](../SIMD-API/c_api/deprecated_interface/asc_gather_datablock_sync_deprecated.md) API

    `asc_gather_datablock_sync()` is deprecated. Please use the [corresponding non-synchronous API](../SIMD-API/c_api/vector_compute/vector_gather/asc_gather_datablock.md) and the [asc_sync](../SIMD-API/c_api/sync/intra_core_sync/asc_sync.md) API instead.

- [asc_gather_sync (deprecated)](../SIMD-API/c_api/deprecated_interface/asc_gather_sync_deprecated.md) API

    `asc_gather_sync()` is deprecated. Please use the [corresponding non-synchronous API](../SIMD-API/c_api/vector_compute/vector_gather/asc_gather.md) and the [asc_sync](../SIMD-API/c_api/sync/intra_core_sync/asc_sync.md) API instead.

#### Logical Calculation

- [asc_and_sync (deprecated)](../SIMD-API/c_api/deprecated_interface/asc_and_sync_deprecated.md) API

    `asc_and_sync()` is deprecated. Please use [the corresponding non-synchronous API](../SIMD-API/c_api/vector_compute/vector_logic/asc_and.md) and [asc_sync](../SIMD-API/c_api/sync/intra_core_sync/asc_sync.md) as the replacement.

- [asc_not_sync (deprecated)](../SIMD-API/c_api/deprecated_interface/asc_not_sync_deprecated.md) API

    `asc_not_sync()` is deprecated. Please use [the corresponding non-synchronous API](../SIMD-API/c_api/vector_compute/vector_logic/asc_not.md) and [asc_sync](../SIMD-API/c_api/sync/intra_core_sync/asc_sync.md) as the replacement.

- [asc_or_sync (deprecated)](../SIMD-API/c_api/deprecated_interface/asc_or_sync_deprecated.md) API

    `asc_or_sync()` is deprecated. Please use [the corresponding non-synchronous API](../SIMD-API/c_api/vector_compute/vector_logic/asc_or.md) and [asc_sync](../SIMD-API/c_api/sync/intra_core_sync/asc_sync.md) as the replacement.

- [asc_shiftleft_sync (deprecated)](../SIMD-API/c_api/deprecated_interface/asc_shiftleft_sync_deprecated.md) API

    `asc_shiftleft_sync()` is deprecated. Please use [the corresponding non-synchronous API](../SIMD-API/c_api/vector_compute/vector_logic/asc_shiftleft.md) and [asc_sync](../SIMD-API/c_api/sync/intra_core_sync/asc_sync.md) as the replacement.

- [asc_shiftright_sync (deprecated)](../SIMD-API/c_api/deprecated_interface/asc_shiftright_sync_deprecated.md) API

    `asc_shiftright_sync()` and `asc_shiftright_round_sync()` are deprecated. Please use the [corresponding non-synchronous API](../SIMD-API/c_api/vector_compute/vector_logic/asc_shiftright.md) and the [asc_sync](../SIMD-API/c_api/sync/intra_core_sync/asc_sync.md) API instead.

#### Layout Transformation

- [asc_transto5hd_sync (deprecated)](../SIMD-API/c_api/deprecated_interface/asc_transto5hd_sync_deprecated.md) API

    This API has been deprecated. Please use the [asc_transto5hd](../SIMD-API/c_api/vector_compute/vector_permute_sel/asc_transto5hd.md) API and the [asc_sync](../SIMD-API/c_api/sync/intra_core_sync/asc_sync.md) API.

- [asc_select_sync (deprecated)](../SIMD-API/c_api/deprecated_interface/asc_select_sync_deprecated.md) API

    `asc_select_sync()` has been deprecated. Please use the [corresponding non-synchronous API](../SIMD-API/c_api/vector_compute/vector_permute_sel/asc_select.md) and the [asc_sync](../SIMD-API/c_api/sync/intra_core_sync/asc_sync.md) API as replacements.

- [asc_transpose_sync (deprecated)](../SIMD-API/c_api/deprecated_interface/asc_transpose_sync_deprecated.md) API

    `asc_transpose_sync()` has been deprecated. Please use the [corresponding non-synchronous API](../SIMD-API/c_api/vector_compute/vector_permute_sel/asc_transpose.md) and the [asc_sync](../SIMD-API/c_api/sync/intra_core_sync/asc_sync.md) API as replacements.

#### Reduction Computation

- [asc_datablock_reduce_max_sync (deprecated)](../SIMD-API/c_api/deprecated_interface/asc_datablock_reduce_max_sync_deprecated.md) API

    `asc_datablock_reduce_max_sync()` is deprecated. Please use the [corresponding non-synchronous API](../SIMD-API/c_api/vector_compute/vector_reduce/asc_datablock_reduce_max.md) and the [asc_sync](../SIMD-API/c_api/sync/intra_core_sync/asc_sync.md) API instead.

- [asc_datablock_reduce_min_sync (deprecated)](../SIMD-API/c_api/deprecated_interface/asc_datablock_reduce_min_sync_deprecated.md) API

    `asc_datablock_reduce_min_sync()` is deprecated. Please use the [corresponding non-synchronous API](../SIMD-API/c_api/vector_compute/vector_reduce/asc_datablock_reduce_min.md) and the [asc_sync](../SIMD-API/c_api/sync/intra_core_sync/asc_sync.md) API instead.

- [asc_datablock_reduce_sum_sync (deprecated)](../SIMD-API/c_api/deprecated_interface/asc_datablock_reduce_sum_sync_deprecated.md) API

    `asc_datablock_reduce_sum_sync()` is deprecated. Please use the [corresponding non-synchronous API](../SIMD-API/c_api/vector_compute/vector_reduce/asc_datablock_reduce_sum.md) and the [asc_sync](../SIMD-API/c_api/sync/intra_core_sync/asc_sync.md) API instead.

- [asc_pair_reduce_sum_sync (deprecated)](../SIMD-API/c_api/deprecated_interface/asc_pair_reduce_sum_sync_deprecated.md) API

    `asc_pair_reduce_sum_sync()` is deprecated. Please use the [corresponding non-synchronous API](../SIMD-API/c_api/vector_compute/vector_reduce/asc_pair_reduce_sum.md) and the [asc_sync](../SIMD-API/c_api/sync/intra_core_sync/asc_sync.md) API instead.

- [asc_reduce_sync (deprecated)](../SIMD-API/c_api/deprecated_interface/asc_reduce_sync_deprecated.md) API

    `asc_reduce_sync()` is deprecated. Please use the [corresponding non-synchronous API](../SIMD-API/c_api/vector_compute/vector_reduce/asc_reduce.md) and [asc_sync](../SIMD-API/c_api/sync/intra_core_sync/asc_sync.md) instead.

- [asc_repeat_reduce_max_sync (deprecated)](../SIMD-API/c_api/deprecated_interface/asc_repeat_reduce_max_sync_deprecated.md) API

    `asc_repeat_reduce_max_index_value_sync()`, `asc_repeat_reduce_max_value_index_sync()`, `asc_repeat_reduce_max_only_value_sync()`, and `asc_repeat_reduce_max_only_index_sync()` are deprecated. Please use the [corresponding non-synchronous API](../SIMD-API/c_api/vector_compute/vector_reduce/asc_repeat_reduce_max.md) and [asc_sync](../SIMD-API/c_api/sync/intra_core_sync/asc_sync.md) instead.

- [asc_repeat_reduce_min_sync (deprecated)](../SIMD-API/c_api/deprecated_interface/asc_repeat_reduce_min_sync_deprecated.md) API

    `asc_repeat_reduce_min_index_value_sync()`, `asc_repeat_reduce_min_value_index_sync()`, `asc_repeat_reduce_min_only_value_sync()`, and `asc_repeat_reduce_min_only_index_sync()` are deprecated. Please use the [corresponding non-synchronous API](../SIMD-API/c_api/vector_compute/vector_reduce/asc_repeat_reduce_min.md) and [asc_sync](../SIMD-API/c_api/sync/intra_core_sync/asc_sync.md) instead.

- [asc_repeat_reduce_sum_sync (deprecated)](../SIMD-API/c_api/deprecated_interface/asc_repeat_reduce_sum_sync_deprecated.md) API

    `asc_repeat_reduce_sum_sync()` is deprecated. Please use the [corresponding non-synchronous API](../SIMD-API/c_api/vector_compute/vector_reduce/asc_repeat_reduce_sum.md) and [asc_sync](../SIMD-API/c_api/sync/intra_core_sync/asc_sync.md) instead.

#### Sorting Calculation

- [asc_bitsort_sync (deprecated)](../SIMD-API/c_api/deprecated_interface/asc_bitsort_sync_deprecated.md) API

    `asc_bitsort_sync()` is deprecated. Please use the [corresponding non-synchronous API](../SIMD-API/c_api/vector_compute/vector_sort/asc_bitsort.md) and the [asc_sync](../SIMD-API/c_api/sync/intra_core_sync/asc_sync.md) API instead.

- [asc_mrgsort4_sync (deprecated)](../SIMD-API/c_api/deprecated_interface/asc_mrgsort4_sync_deprecated.md) API

    `asc_mrgsort4_sync()` is deprecated. Please use the [corresponding non-synchronous API](../SIMD-API/c_api/vector_compute/vector_sort/asc_mrgsort4.md) and the [asc_sync](../SIMD-API/c_api/sync/intra_core_sync/asc_sync.md) API instead.
