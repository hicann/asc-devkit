# 废弃接口<a name="ZH-CN_TOPIC_0000001724165365"></a>

## 基础API

- [WriteGmByPassDCache\(ISASI\)](../SIMD-API/basic_api/deprecated_interface/WriteGmByPassDCache_ISASI_deprecated.md)接口

    此接口后续版本会废弃，请使用[WriteGmBypassDCache\(ISASI\)](../SIMD-API/basic_api/scalar_compute/WriteGmBypassDCache_ISASI.md)接口。

- [ReadGmByPassDCache\(ISASI\)](../SIMD-API/basic_api/deprecated_interface/ReadGmByPassDCache_ISASI_deprecated.md)接口

    此接口后续版本会废弃，请使用[ReadGmBypassDCache\(ISASI\)](../SIMD-API/basic_api/scalar_compute/ReadGmBypassDCache_ISASI.md)接口。

## C API

### 数据搬运

<!-- npu="950" id1 -->
- [asc_set_gm2ub_pad](../SIMD-API/c_api/vector_datamove/asc_set_gm2ub_pad_deprecated.md)接口

    此接口后续版本会废弃，请使用[asc_set_copy_pad_val](../SIMD-API/c_api/vector_datamove/asc_set_copy_pad_val.md)接口。
<!-- end id1 -->

<!-- npu="950" id2 -->
- [asc_copy_gm2ub_align](../SIMD-API/c_api/vector_datamove/asc_copy_gm2ub_align/asc_copy_gm2ub_align_arch_3510.md)接口（仅Ascend 950PR/Ascend 950DT上原高维切分搬运形式函数原型）

    Ascend 950PR/Ascend 950DT上此接口的原高维切分搬运形式函数原型已废弃，请使用[asc_load_l2_cache_mode](../SIMD-API/c_api/defs/enum/asc_load_l2_cache_mode.md)类型枚举值进行L2 Cache管理策略配置。
<!-- end id2 -->

<!-- npu="950" id3 -->
- [asc_copy_ub2gm_align](../SIMD-API/c_api/vector_datamove/asc_copy_ub2gm_align/asc_copy_ub2gm_align_arch_3510.md)接口（仅Ascend 950PR/Ascend 950DT上原高维切分搬运形式函数原型）

    Ascend 950PR/Ascend 950DT上此接口的原高维切分搬运形式函数原型已废弃，请使用[asc_store_l2_cache_mode](../SIMD-API/c_api/defs/enum/asc_store_l2_cache_mode.md)类型枚举值进行L2 Cache管理策略配置。
<!-- end id3 -->

<!-- npu="950" id4 -->
- [asc_ndim_copy_gm2ub](../SIMD-API/c_api/vector_datamove/asc_ndim_copy_gm2ub.md)

    此接口的原函数原型已废弃，请使用[asc_load_l2_cache_mode](../SIMD-API/c_api/defs/enum/asc_load_l2_cache_mode.md)类型枚举值进行L2 Cache管理策略配置。
<!-- end id4 -->


### 原子操作

<!-- npu="950" id5 -->
- [asc_set_store_atomic_config_v2](../SIMD-API/c_api/deprecated_interface/asc_set_store_atomic_config_v2_deprecated.md)接口

    此接口后续版本会废弃，请使用[asc_atomic_add](../SIMD-API/c_api/atomic/scalar_atomic/asc_atomic_add.md)接口。

- [asc_get_store_atomic_config](../SIMD-API/c_api/atomic/scalar_atomic/asc_get_store_atomic_config.md)接口（仅Ascend 950PR/Ascend 950DT）

    Ascend 950PR/Ascend 950DT上该接口已废弃，请使用[asc_atomic_add](../SIMD-API/c_api/atomic/scalar_atomic/asc_atomic_add.md)接口。
<!-- end id5 -->

- [asc_set_atomic_none](../SIMD-API/c_api/deprecated_interface/asc_set_atomic_none_deprecated.md)接口

    此接口后续版本会废弃，请使用[asc_disable_dma_atomic](../SIMD-API/c_api/atomic/datamove_atomic/asc_disable_dma_atomic.md)接口。
- [asc_set_atomic_add_int](../SIMD-API/c_api/deprecated_interface/asc_set_atomic_add_int_deprecated.md)接口

    此接口后续版本会废弃，请使用[asc_set_atomic_add_int32](../SIMD-API/c_api/atomic/datamove_atomic/asc_set_atomic_add.md)接口。

- [asc_set_atomic_max_int](../SIMD-API/c_api/deprecated_interface/asc_set_atomic_max_int_deprecated.md)接口

    此接口后续版本会废弃，请使用[asc_set_atomic_max_int32](../SIMD-API/c_api/atomic/datamove_atomic/asc_set_atomic_max.md)接口。

- [asc_set_atomic_min_int](../SIMD-API/c_api/deprecated_interface/asc_set_atomic_min_int_deprecated.md)接口

    此接口后续版本会废弃，请使用[asc_set_atomic_min_int32](../SIMD-API/c_api/atomic/datamove_atomic/asc_set_atomic_min.md)接口。

<!-- npu="950" id6 -->
### 系统变量

- [asc_get_block_idx](../SIMD-API/c_api/deprecated_interface/asc_get_block_idx_deprecated.md)接口

    此接口后续版本会废弃，请使用内置变量[block_idx](../../guide/programming_guide/language_extension/simd_builtin_keywords.md#内置变量)结合[asc_get_sub_block_num](../SIMD-API/c_api/utils/sys_var/asc_get_sub_block_num.md)和[asc_get_sub_block_id](../SIMD-API/c_api/utils/sys_var/asc_get_sub_block_id.md)接口计算逻辑核索引。

- [asc_get_block_num](../SIMD-API/c_api/deprecated_interface/asc_get_block_num_deprecated.md)接口

    此接口后续版本会废弃，请使用[内置变量block_num](../../guide/programming_guide/language_extension/simd_builtin_keywords.md#内置变量)。

- [asc_get_ar_spr](../SIMD-API/c_api/spr/asc_get_ar_spr_deprecated.md)接口

    此接口后续版本会废弃，请使用[asc_get_squeeze_status](../SIMD-API/c_api/spr/asc_get_squeeze_status.md)接口。

<!-- end id6 -->

<!-- npu="950" id7 -->
### Reg数据搬运

- [asc_loadalign_brc_v2](../SIMD-API/c_api/deprecated_interface/asc_loadalign_v1_deprecated.md)接口（无偏移量原型）

    此接口后续版本会废弃，请使用[asc_loadalign_brc_datablock](../SIMD-API/c_api/reg_compute/load/asc_loadalign_brc_datablock.md)接口。

- [asc_loadalign_brc_v3](../SIMD-API/c_api/deprecated_interface/asc_loadalign_v1_deprecated.md)接口（无偏移量原型）

    此接口后续版本会废弃，请使用[asc_loadalign_brc_elem2datablock](../SIMD-API/c_api/reg_compute/load/asc_loadalign_brc_elem2datablock.md)接口。

- [asc_loadalign_unpack_v2](../SIMD-API/c_api/deprecated_interface/asc_loadalign_v1_deprecated.md)接口（无偏移量原型）

    此接口后续版本会废弃，请使用[asc_loadalign_unpack4](../SIMD-API/c_api/reg_compute/load/asc_loadalign_unpack4.md)接口。

- [asc_loadalign_brc_v2](../SIMD-API/c_api/deprecated_interface/asc_loadalign_v2_deprecated.md)接口（addr_reg偏移原型）

    此接口后续版本会废弃，请使用[asc_loadalign_brc_datablock](../SIMD-API/c_api/reg_compute/load/asc_loadalign_brc_datablock.md)接口。

- [asc_loadalign_brc_v3](../SIMD-API/c_api/deprecated_interface/asc_loadalign_v2_deprecated.md)接口（addr_reg偏移原型）

    此接口后续版本会废弃，请使用[asc_loadalign_brc_elem2datablock](../SIMD-API/c_api/reg_compute/load/asc_loadalign_brc_elem2datablock.md)接口。

- [asc_loadalign_unpack_v2](../SIMD-API/c_api/deprecated_interface/asc_loadalign_v2_deprecated.md)接口（addr_reg偏移原型）

    此接口后续版本会废弃，请使用[asc_loadalign_unpack4](../SIMD-API/c_api/reg_compute/load/asc_loadalign_unpack4.md)接口。

- [asc_loadalign_brc_v2](../SIMD-API/c_api/deprecated_interface/asc_loadalign_v3_deprecated.md)接口（int32_t偏移原型）

    此接口后续版本会废弃，请使用[asc_loadalign_brc_datablock](../SIMD-API/c_api/reg_compute/load/asc_loadalign_brc_datablock.md)接口。

- [asc_loadalign_brc_v3](../SIMD-API/c_api/deprecated_interface/asc_loadalign_v3_deprecated.md)接口（int32_t偏移原型）

    此接口后续版本会废弃，请使用[asc_loadalign_brc_elem2datablock](../SIMD-API/c_api/reg_compute/load/asc_loadalign_brc_elem2datablock.md)接口。

- [asc_loadalign_unpack_v2](../SIMD-API/c_api/deprecated_interface/asc_loadalign_v3_deprecated.md)接口（int32_t偏移原型）

    此接口后续版本会废弃，请使用[asc_loadalign_unpack4](../SIMD-API/c_api/reg_compute/load/asc_loadalign_unpack4.md)接口。

- [asc_loadalign_brc_postupdate_v2](../SIMD-API/c_api/deprecated_interface/asc_loadalign_v4_deprecated.md)接口（int32_t偏移Post Update原型）

    此接口后续版本会废弃，请使用[asc_loadalign_brc_datablock_postupdate](../SIMD-API/c_api/reg_compute/load/asc_loadalign_brc_datablock_postupdate.md)接口。

- [asc_loadalign_brc_postupdate_v3](../SIMD-API/c_api/deprecated_interface/asc_loadalign_v4_deprecated.md)接口（int32_t偏移Post Update原型）

    此接口后续版本会废弃，请使用[asc_loadalign_brc_elem2datablock_postupdate](../SIMD-API/c_api/reg_compute/load/asc_loadalign_brc_elem2datablock_postupdate.md)接口。

- [asc_loadalign_unpack_postupdate_v2](../SIMD-API/c_api/deprecated_interface/asc_loadalign_v4_deprecated.md)接口（int32_t偏移Post Update原型）

    此接口后续版本会废弃，请使用[asc_loadalign_unpack4_postupdate](../SIMD-API/c_api/reg_compute/load/asc_loadalign_unpack4_postupdate.md)接口。

- [asc_loadalign (多模式重载)](../SIMD-API/c_api/reg_compute/load/asc_loadalign.md)接口

    原 `asc_loadalign(dst, src)` 通过函数名重载实现NORM/BRC/UP2X/DOWN2X/UNPACK/BRCV2/BRCV3/DEINTLV等多种搬入模式，现收窄为仅NORM模式，其他模式拆分到独立函数。请使用[asc_loadalign](../SIMD-API/c_api/reg_compute/load/asc_loadalign.md)的新版本。

- [asc_loadalign (deintlv模式)](../SIMD-API/c_api/reg_compute/load/asc_loadalign_deintlv.md)接口

    原asc_loadalign重载之一，已拆分为独立函数[asc_loadalign_deintlv](../SIMD-API/c_api/reg_compute/load/asc_loadalign_deintlv.md)。

- [asc_loadalign (deintlv+postupdate模式)](../SIMD-API/c_api/reg_compute/load/asc_loadalign_deintlv_postupdate.md)接口

    原asc_loadalign重载之一，已拆分为独立函数[asc_loadalign_deintlv_postupdate](../SIMD-API/c_api/reg_compute/load/asc_loadalign_deintlv_postupdate.md)。

- [asc_loadalign (brc_datablock模式)](../SIMD-API/c_api/reg_compute/load/asc_loadalign_brc_datablock.md)接口

    原asc_loadalign重载之一，已拆分为独立函数[asc_loadalign_brc_datablock](../SIMD-API/c_api/reg_compute/load/asc_loadalign_brc_datablock.md)。

- [asc_loadalign (brc_elem模式)](../SIMD-API/c_api/reg_compute/load/asc_loadalign_brc_elem.md)接口

    原asc_loadalign重载之一，已拆分为独立函数[asc_loadalign_brc_elem](../SIMD-API/c_api/reg_compute/load/asc_loadalign_brc_elem.md)。

- [asc_loadalign (brc_elem2datablock模式)](../SIMD-API/c_api/reg_compute/load/asc_loadalign_brc_elem2datablock.md)接口

    原asc_loadalign重载之一，已拆分为独立函数[asc_loadalign_brc_elem2datablock](../SIMD-API/c_api/reg_compute/load/asc_loadalign_brc_elem2datablock.md)。

- [asc_loadalign (downsample模式)](../SIMD-API/c_api/reg_compute/load/asc_loadalign_downsample.md)接口

    原asc_loadalign重载之一，已拆分为独立函数[asc_loadalign_downsample](../SIMD-API/c_api/reg_compute/load/asc_loadalign_downsample.md)。

- [asc_loadalign (unpack模式)](../SIMD-API/c_api/reg_compute/load/asc_loadalign_unpack.md)接口

    原asc_loadalign重载之一，已拆分为独立函数[asc_loadalign_unpack](../SIMD-API/c_api/reg_compute/load/asc_loadalign_unpack.md)。

- [asc_loadalign (unpack4模式)](../SIMD-API/c_api/reg_compute/load/asc_loadalign_unpack4.md)接口

    原asc_loadalign重载之一，已拆分为独立函数[asc_loadalign_unpack4](../SIMD-API/c_api/reg_compute/load/asc_loadalign_unpack4.md)。

- [asc_loadalign (upsample模式)](../SIMD-API/c_api/reg_compute/load/asc_loadalign_upsample.md)接口

    原asc_loadalign重载之一，已拆分为独立函数[asc_loadalign_upsample](../SIMD-API/c_api/reg_compute/load/asc_loadalign_upsample.md)。

- [asc_storealign_pack_v2](../SIMD-API/c_api/deprecated_interface/asc_storealign_v1_deprecated.md)接口（无偏移量原型）

    此接口后续版本会废弃，请使用[asc_storealign_pack_quarter](../SIMD-API/c_api/reg_compute/store/asc_storealign_pack_quarter.md)接口。

- [asc_storealign_pack_v2](../SIMD-API/c_api/deprecated_interface/asc_storealign_v2_deprecated.md)接口（addr_reg偏移原型）

    此接口后续版本会废弃，请使用[asc_storealign_pack_quarter](../SIMD-API/c_api/reg_compute/store/asc_storealign_pack_quarter.md)接口。

- [asc_storealign_pack_v2](../SIMD-API/c_api/deprecated_interface/asc_storealign_v3_deprecated.md)接口（int32_t偏移原型）

    此接口后续版本会废弃，请使用[asc_storealign_pack_quarter](../SIMD-API/c_api/reg_compute/store/asc_storealign_pack_quarter.md)接口。

- [asc_storealign_pack_postupdate_v2](../SIMD-API/c_api/deprecated_interface/asc_storealign_v4_deprecated.md)接口

    此接口后续版本会废弃，请使用[asc_storealign_pack_quarter_postupdate](../SIMD-API/c_api/reg_compute/store/asc_storealign_pack_quarter_postupdate.md)接口。

- [asc_storealign (多模式重载)](../SIMD-API/c_api/reg_compute/store/asc_storealign.md)接口

    原 `asc_storealign(dst, src, mask)` 通过函数名重载实现NORM/FIRST/PACK/PACKV2/INTLV等多种搬出模式，现收窄为仅NORM模式，其他模式拆分到独立函数。请使用[asc_storealign](../SIMD-API/c_api/reg_compute/store/asc_storealign.md)的新版本。

- [asc_storealign (1st模式)](../SIMD-API/c_api/reg_compute/store/asc_storealign_1st.md)接口

    原asc_storealign重载之一，已拆分为独立函数[asc_storealign_1st](../SIMD-API/c_api/reg_compute/store/asc_storealign_1st.md)。

- [asc_storealign (intlv模式)](../SIMD-API/c_api/reg_compute/store/asc_storealign_intlv.md)接口

    原asc_storealign重载之一，已拆分为独立函数[asc_storealign_intlv](../SIMD-API/c_api/reg_compute/store/asc_storealign_intlv.md)。

- [asc_storealign (pack模式)](../SIMD-API/c_api/reg_compute/store/asc_storealign_pack.md)接口

    原asc_storealign重载之一，已拆分为独立函数[asc_storealign_pack](../SIMD-API/c_api/reg_compute/store/asc_storealign_pack.md)。

- [asc_storealign (pack_quarter模式)](../SIMD-API/c_api/reg_compute/store/asc_storealign_pack_quarter.md)接口

    原asc_storealign重载之一，已拆分为独立函数[asc_storealign_pack_quarter](../SIMD-API/c_api/reg_compute/store/asc_storealign_pack_quarter.md)。

- [asc_storealign (postupdate模式)](../SIMD-API/c_api/reg_compute/store/asc_storealign_postupdate.md)接口

    原asc_storealign重载之一，已拆分为独立函数[asc_storealign_postupdate](../SIMD-API/c_api/reg_compute/store/asc_storealign_postupdate.md)。

- [asc_scatter (reg_store)](../SIMD-API/c_api/deprecated_interface/asc_scatter_deprecated.md)接口

    此接口后续版本会废弃，请使用[asc_scatter (reg_store)](../SIMD-API/c_api/reg_compute/scatter/asc_scatter.md)接口。

- [asc_storeunalign_post](../SIMD-API/c_api/deprecated_interface/asc_storeunalign_post_deprecated.md)接口（无偏移量原型）

    此接口后续版本会废弃，请使用[asc_squeeze_and_storeunalign_finalize](../SIMD-API/c_api/reg_compute/reg_permute_sel/asc_squeeze_and_storeunalign_finalize.md)接口。

- [asc_storeunalign_postupdate](../SIMD-API/c_api/deprecated_interface/asc_storeunalign_postupdate_deprecated.md)接口（不指定存储偏移量原型）

    此接口后续版本会废弃，请使用[asc_squeeze_and_storeunalign](../SIMD-API/c_api/reg_compute/reg_permute_sel/asc_squeeze_and_storeunalign.md)接口。

- [asc_storeunalign_postupdate](../SIMD-API/c_api/deprecated_interface/asc_storeunalign_postupdate_deprecated.md)接口（掩码寄存器搬出模式的b8数据类型原型）

    此接口后续版本会废弃，请使用b16或b32数据类型原型。

<!-- end id7 -->

<!-- npu="950" id8 -->
### Reg矢量计算

- [asc_and](../SIMD-API/c_api/deprecated_interface/asc_and_deprecated.md)接口（浮点类型重载）

    `asc_and`的浮点类型重载已废弃，请按位宽选择对应整数重载替代，详情请参见[asc_and](../SIMD-API/c_api/reg_compute/reg_logic/asc_and.md)。

- [asc_or](../SIMD-API/c_api/deprecated_interface/asc_or_deprecated.md)接口（浮点类型重载）

    `asc_or`的浮点类型重载已废弃，请按位宽选择对应整数重载替代，详情请参见[asc_or](../SIMD-API/c_api/reg_compute/reg_logic/asc_or.md)。

- [asc_float2bfloat16](../SIMD-API/c_api/deprecated_interface/asc_float2bfloat16_deprecated.md)接口（通过接口后缀来控制目的操作数写入位置的三参数原型）

    旧原型已废弃，请使用[asc_float2bfloat16](../SIMD-API/c_api/reg_compute/reg_convert/asc_float2bfloat16.md)的显式位置参数重载。

- [asc_int322int16](../SIMD-API/c_api/deprecated_interface/asc_int322int16_deprecated.md)接口（通过接口后缀来控制目的操作数写入位置的三参数原型）

    旧原型已废弃，请使用[asc_int322int16](../SIMD-API/c_api/reg_compute/reg_convert/asc_int322int16.md)的显式位置参数重载。

- [asc_int322int64](../SIMD-API/c_api/deprecated_interface/asc_int322int64_deprecated.md)接口（通过接口后缀来控制源操作数读取位置的三参数原型）

    旧原型已废弃，请使用[asc_int322int64](../SIMD-API/c_api/reg_compute/reg_convert/asc_int322int64.md)的显式位置参数重载。

- [asc_int322uint16](../SIMD-API/c_api/deprecated_interface/asc_int322uint16_deprecated.md)接口（通过接口后缀来控制目的操作数写入位置的三参数原型）

    旧原型已废弃，请使用[asc_int322uint16](../SIMD-API/c_api/reg_compute/reg_convert/asc_int322uint16.md)的显式位置参数重载。

- [asc_uint82uint16](../SIMD-API/c_api/deprecated_interface/asc_uint82uint16_deprecated.md)接口（通过接口后缀来控制源操作数读取位置的三参数原型）

    旧原型已废弃，请使用[asc_uint82uint16](../SIMD-API/c_api/reg_compute/reg_convert/asc_uint82uint16.md)的显式位置参数重载。

- [asc_uint82uint16](../SIMD-API/c_api/deprecated_interface/asc_uint82uint16_deprecated.md)接口（通过接口后缀来控制源操作数读取位置的三参数原型）

    旧原型已废弃，请使用[asc_uint82uint16](../SIMD-API/c_api/reg_compute/reg_convert/asc_uint82uint16.md)的显式位置参数重载。

- [asc_float2half](../SIMD-API/c_api/deprecated_interface/asc_float2half_deprecated.md)接口（通过接口后缀来控制目的操作数写入位置的三参数原型）

    旧原型已废弃，请使用[asc_float2half](../SIMD-API/c_api/reg_compute/reg_convert/asc_float2half.md)的显式位置参数重载。

- [asc_float2int16](../SIMD-API/c_api/deprecated_interface/asc_float2int16_deprecated.md)接口（通过接口后缀来控制目的操作数写入位置的三参数原型）

    旧原型已废弃，请使用[asc_float2int16](../SIMD-API/c_api/reg_compute/reg_convert/asc_float2int16.md)的显式位置参数重载。

- [asc_float2int64](../SIMD-API/c_api/deprecated_interface/asc_float2int64_deprecated.md)接口（通过接口后缀来控制目的操作数写入位置的三参数原型）

    旧原型已废弃，请使用[asc_float2int64](../SIMD-API/c_api/reg_compute/reg_convert/asc_float2int64.md)的显式位置参数重载。

- [asc_hif82half](../SIMD-API/c_api/deprecated_interface/asc_hif82half_deprecated.md)接口（通过接口后缀来控制源操作数读取位置的三参数原型）

    旧原型已废弃，请使用[asc_hif82half](../SIMD-API/c_api/reg_compute/reg_convert/asc_hif82half.md)的显式位置参数重载。

- [asc_int162float](../SIMD-API/c_api/deprecated_interface/asc_int162float_deprecated.md)接口（通过接口后缀来控制源操作数读取位置的三参数原型）

    旧原型已废弃，请使用[asc_int162float](../SIMD-API/c_api/reg_compute/reg_convert/asc_int162float.md)的显式位置参数重载。

- [asc_int162int32](../SIMD-API/c_api/deprecated_interface/asc_int162int32_deprecated.md)接口（通过接口后缀来控制源操作数读取位置的三参数原型）

    旧原型已废弃，请使用[asc_int162int32](../SIMD-API/c_api/reg_compute/reg_convert/asc_int162int32.md)的显式位置参数重载。

- [asc_int162uint32](../SIMD-API/c_api/deprecated_interface/asc_int162uint32_deprecated.md)接口（通过接口后缀来控制源操作数读取位置的三参数原型）

    旧原型已废弃，请使用[asc_int162uint32](../SIMD-API/c_api/reg_compute/reg_convert/asc_int162uint32.md)的显式位置参数重载。

- [asc_int162uint8](../SIMD-API/c_api/deprecated_interface/asc_int162uint8_deprecated.md)接口（通过接口后缀来控制目的操作数写入位置的三参数原型）

    旧原型已废弃，请使用[asc_int162uint8](../SIMD-API/c_api/reg_compute/reg_convert/asc_int162uint8.md)的显式位置参数重载。

- [asc_int642float](../SIMD-API/c_api/deprecated_interface/asc_int642float_deprecated.md)接口（通过接口后缀来控制目的操作数写入位置的三参数原型）

    旧原型已废弃，请使用[asc_int642float](../SIMD-API/c_api/reg_compute/reg_convert/asc_int642float.md)的显式位置参数重载。

- [asc_int642int32](../SIMD-API/c_api/deprecated_interface/asc_int642int32_deprecated.md)接口（通过接口后缀来控制目的操作数写入位置的三参数原型）

    旧原型已废弃，请使用[asc_int642int32](../SIMD-API/c_api/reg_compute/reg_convert/asc_int642int32.md)的显式位置参数重载。

- [asc_int82half](../SIMD-API/c_api/deprecated_interface/asc_int82half_deprecated.md)接口（通过接口后缀来控制源操作数读取位置的三参数原型）

    旧原型已废弃，请使用[asc_int82half](../SIMD-API/c_api/reg_compute/reg_convert/asc_int82half.md)的显式位置参数重载。

- [asc_int82int16](../SIMD-API/c_api/deprecated_interface/asc_int82int16_deprecated.md)接口（通过接口后缀来控制源操作数读取位置的三参数原型）

    旧原型已废弃，请使用[asc_int82int16](../SIMD-API/c_api/reg_compute/reg_convert/asc_int82int16.md)的显式位置参数重载。

- [asc_uint162uint32](../SIMD-API/c_api/deprecated_interface/asc_uint162uint32_deprecated.md)接口（通过接口后缀来控制源操作数读取位置的三参数原型）

    旧原型已废弃，请使用[asc_uint162uint32](../SIMD-API/c_api/reg_compute/reg_convert/asc_uint162uint32.md)的显式位置参数重载。

- [asc_uint162uint8](../SIMD-API/c_api/deprecated_interface/asc_uint162uint8_deprecated.md)接口（通过接口后缀来控制目的操作数写入位置的三参数原型）

    旧原型已废弃，请使用[asc_uint162uint8](../SIMD-API/c_api/reg_compute/reg_convert/asc_uint162uint8.md)的显式位置参数重载。

- [asc_uint322int16](../SIMD-API/c_api/deprecated_interface/asc_uint322int16_deprecated.md)接口（通过接口后缀来控制目的操作数写入位置的三参数原型）

    旧原型已废弃，请使用[asc_uint322int16](../SIMD-API/c_api/reg_compute/reg_convert/asc_uint322int16.md)的显式位置参数重载。

- [asc_uint322uint16](../SIMD-API/c_api/deprecated_interface/asc_uint322uint16_deprecated.md)接口（通过接口后缀来控制目的操作数写入位置的三参数原型）

    旧原型已废弃，请使用[asc_uint322uint16](../SIMD-API/c_api/reg_compute/reg_convert/asc_uint322uint16.md)的显式位置参数重载。

- [asc_uint82half](../SIMD-API/c_api/deprecated_interface/asc_uint82half_deprecated.md)接口（通过接口后缀来控制源操作数读取位置的三参数原型）

    旧原型已废弃，请使用[asc_uint82half](../SIMD-API/c_api/reg_compute/reg_convert/asc_uint82half.md)的显式位置参数重载。

- [asc_bfloat162float](../SIMD-API/c_api/deprecated_interface/asc_bfloat162float_deprecated.md)接口（通过接口后缀来控制源操作数读取位置的三参数原型）

    旧原型已废弃，请使用[asc_bfloat162float](../SIMD-API/c_api/reg_compute/reg_convert/asc_bfloat162float.md)的显式位置参数重载。

- [asc_bfloat162int32](../SIMD-API/c_api/deprecated_interface/asc_bfloat162int32_deprecated.md)接口（通过接口后缀来控制源操作数读取位置的三参数原型）

    旧原型已废弃，请使用[asc_bfloat162int32](../SIMD-API/c_api/reg_compute/reg_convert/asc_bfloat162int32.md)的显式位置参数重载。

- [asc_half2float](../SIMD-API/c_api/deprecated_interface/asc_half2float_deprecated.md)接口（通过接口后缀来控制源操作数读取位置的三参数原型）

    旧原型已废弃，请使用[asc_half2float](../SIMD-API/c_api/reg_compute/reg_convert/asc_half2float.md)的显式位置参数重载。

- [asc_half2hif8](../SIMD-API/c_api/deprecated_interface/asc_half2hif8_deprecated.md)接口（通过接口后缀来控制目的操作数写入位置的三参数原型）

    旧原型已废弃，请使用[asc_half2hif8](../SIMD-API/c_api/reg_compute/reg_convert/asc_half2hif8.md)的显式位置参数重载。

- [asc_half2int32](../SIMD-API/c_api/deprecated_interface/asc_half2int32_deprecated.md)接口（通过接口后缀来控制源操作数读取位置的三参数原型）

    旧原型已废弃，请使用[asc_half2int32](../SIMD-API/c_api/reg_compute/reg_convert/asc_half2int32.md)的显式位置参数重载。

- [asc_half2int8](../SIMD-API/c_api/deprecated_interface/asc_half2int8_deprecated.md)接口（通过接口后缀来控制目的操作数写入位置的三参数原型）

    旧原型已废弃，请使用[asc_half2int8](../SIMD-API/c_api/reg_compute/reg_convert/asc_half2int8.md)的显式位置参数重载。

- [asc_half2uint8](../SIMD-API/c_api/deprecated_interface/asc_half2uint8_deprecated.md)接口（通过接口后缀来控制目的操作数写入位置的三参数原型）

    旧原型已废弃，请使用[asc_half2uint8](../SIMD-API/c_api/reg_compute/reg_convert/asc_half2uint8.md)的显式位置参数重载。

- [asc_squeeze_v2](../SIMD-API/c_api/reg_compute/reg_permute_sel/asc_squeeze_deprecated.md)接口

    此接口后续版本会废弃，请使用[asc_squeeze_and_storeunalign](../SIMD-API/c_api/reg_compute/reg_permute_sel/asc_squeeze_and_storeunalign.md)接口。

- [asc_squeeze_with_status](../SIMD-API/c_api/reg_compute/reg_permute_sel/asc_squeeze_with_status_deprecated.md)接口

    此接口后续版本会废弃，请使用[asc_squeeze_and_storeunalign](../SIMD-API/c_api/reg_compute/reg_permute_sel/asc_squeeze_and_storeunalign.md)接口。

- [asc_create_iter_reg](../SIMD-API/c_api/reg_compute/reg_addr_reg/asc_create_iter_reg_deprecated.md)接口

    此接口后续版本会废弃，请使用[asc_update_addr_reg](../SIMD-API/c_api/reg_compute/reg_addr_reg/asc_update_addr_reg.md)接口。

- [asc_bfloat162e2m1x2](../SIMD-API/c_api/reg_compute/reg_convert/asc_bfloat162e2m1x2_deprecated.md)接口

    此接口后续版本会废弃。

- [asc_bfloat162e1m2x2](../SIMD-API/c_api/reg_compute/reg_convert/asc_bfloat162e1m2x2_deprecated.md)接口

    此接口后续版本会废弃。

- [asc_e2m1x22bfloat16](../SIMD-API/c_api/reg_compute/reg_convert/asc_e2m1x22bfloat16_deprecated.md)接口

    此接口后续版本会废弃。

- [asc_e1m2x22bfloat16](../SIMD-API/c_api/reg_compute/reg_convert/asc_e1m2x22bfloat16_deprecated.md)接口

    此接口后续版本会废弃。

- [asc_float2e5m2](../SIMD-API/c_api/reg_compute/reg_convert/asc_float2e5m2_deprecated.md)接口

    此接口后续版本会废弃。

- [asc_e5m22float](../SIMD-API/c_api/reg_compute/reg_convert/asc_e5m22float_deprecated.md)接口

    此接口后续版本会废弃。

- [asc_float2e4m3](../SIMD-API/c_api/reg_compute/reg_convert/asc_float2e4m3_deprecated.md)接口

    此接口后续版本会废弃。

- [asc_e4m32float](../SIMD-API/c_api/reg_compute/reg_convert/asc_e4m32float_deprecated.md)接口

    此接口后续版本会废弃。

- [asc_float2hif8](../SIMD-API/c_api/reg_compute/reg_convert/asc_float2hif8_deprecated.md)接口

    此接口后续版本会废弃。

- [asc_hif82float](../SIMD-API/c_api/reg_compute/reg_convert/asc_hif82float_deprecated.md)接口

    此接口后续版本会废弃。

- [asc_half2int4x2](../SIMD-API/c_api/reg_compute/reg_convert/asc_half2int4x2_deprecated.md)接口

    此接口后续版本会废弃。

- [asc_int4x22int16](../SIMD-API/c_api/reg_compute/reg_convert/asc_int4x22int16_deprecated.md)接口

    此接口后续版本会废弃。

- [asc_int4x22half](../SIMD-API/c_api/reg_compute/reg_convert/asc_int4x22half_deprecated.md)接口

    此接口后续版本会废弃。

- [asc_int4x22bfloat16](../SIMD-API/c_api/reg_compute/reg_convert/asc_int4x22bfloat16_deprecated.md)接口

    此接口后续版本会废弃。

- [asc_int82int32](../SIMD-API/c_api/reg_compute/reg_convert/asc_int82int32_deprecated.md)接口

    此接口后续版本会废弃。

- [asc_int322uint8](../SIMD-API/c_api/reg_compute/reg_convert/asc_int322uint8_deprecated.md)接口

    此接口后续版本会废弃。

- [asc_uint82uint32](../SIMD-API/c_api/reg_compute/reg_convert/asc_uint82uint32_deprecated.md)接口

    此接口后续版本会废弃。

- [asc_uint322uint8](../SIMD-API/c_api/reg_compute/reg_convert/asc_uint322uint8_deprecated.md)接口

    此接口后续版本会废弃。

- [asc_muls](../SIMD-API/c_api/reg_compute/reg_fused/asc_muls_deprecated.md)接口

    此接口后续版本会废弃。

- [asc_exp_sub](../SIMD-API/c_api/deprecated_interface/asc_exp_sub_deprecated.md)接口

    `half`输入且不带`src_pos`参数的`asc_exp_sub`原型以及`asc_exp_sub_v2`原型已废弃。`float`输入请使用`asc_exp_sub`原型，`half`输入请使用带显式`src_pos`参数的`asc_exp_sub_half2float`原型。

- [asc_pack](../SIMD-API/c_api/reg_compute/reg_permute_sel/asc_pack_deprecated.md)接口

    此接口后续版本会废弃，请使用[asc_pack_to_low](../SIMD-API/c_api/reg_compute/reg_permute_sel/asc_pack_to_low.md)接口。

- [asc_pack_v2](../SIMD-API/c_api/reg_compute/reg_permute_sel/asc_pack_deprecated.md)接口

    此接口后续版本会废弃，请使用[asc_pack_to_high](../SIMD-API/c_api/reg_compute/reg_permute_sel/asc_pack_to_high.md)接口。

<!-- end id8 -->

### 同步控制

- [asc_sync_vec（无入参原型）](../SIMD-API/c_api/sync/asc_sync_vec.md)接口

    `asc_sync_vec()`接口已废弃，请使用[asc_sync](../SIMD-API/c_api/sync/asc_sync.md)执行针对所有流水线的同步操作。

### 矢量计算

- [asc_abs_sync](../SIMD-API/c_api/deprecated_interface/asc_abs_sync_deprecated.md)接口

    `asc_abs_sync()`接口已废弃。请使用[asc_abs](../SIMD-API/c_api/vector_compute/vector_arith/asc_abs.md)接口和[asc_sync](../SIMD-API/c_api/sync/asc_sync.md)接口。

### 矩阵计算

- [asc_mmad（废弃）](../SIMD-API/c_api/deprecated_interface/asc_mmad_deprecated.md)接口

    使用`uint8_t unit_flag`或`uint8_t feat_offset`入参的`asc_mmad()`和`asc_mmad_s4()`接口已废弃，请使用[asc_mmad](../SIMD-API/c_api/cube_compute/asc_mmad.md)接口。`asc_mmad_sync()`和`asc_mmad_s4_sync()`接口已废弃，请使用[asc_mmad](../SIMD-API/c_api/cube_compute/asc_mmad.md)接口和[asc_sync](../SIMD-API/c_api/sync/asc_sync.md)接口。

<!-- npu="950" id9 -->
- [asc_mmad_mx（废弃）](../SIMD-API/c_api/deprecated_interface/asc_mmad_mx_deprecated.md)接口

    使用`uint8_t unit_flag`入参的`asc_mmad_mx()`接口已废弃，请使用[asc_mmad_mx](../SIMD-API/c_api/cube_compute/asc_mmad_mx.md)接口。`asc_mmad_mx_sync()`接口已废弃，请使用[asc_mmad_mx](../SIMD-API/c_api/cube_compute/asc_mmad_mx.md)接口和[asc_sync](../SIMD-API/c_api/sync/asc_sync.md)接口。
<!-- end id9 -->

<!-- npu="A3,910b" id10 -->
- [asc_mmad_sparse（废弃）](../SIMD-API/c_api/deprecated_interface/asc_mmad_sparse_deprecated.md)接口

    使用`uint8_t unit_flag`入参的`asc_mmad_sparse()`接口已废弃，请使用[asc_mmad_sparse](../SIMD-API/c_api/cube_compute/asc_mmad_sparse.md)接口。`asc_mmad_sparse_sync()`接口已废弃，请使用[asc_mmad_sparse](../SIMD-API/c_api/cube_compute/asc_mmad_sparse.md)接口和[asc_sync](../SIMD-API/c_api/sync/asc_sync.md)接口。
<!-- end id10 -->
- [asc_transto5hd_sync（废弃）](../SIMD-API/c_api/deprecated_interface/asc_transto5hd_sync_deprecated.md)接口

    此接口已废弃。请使用[asc_transto5hd](../SIMD-API/c_api/vector_compute/vector_permute_sel/asc_transto5hd.md)接口和[asc_sync](../SIMD-API/c_api/sync/asc_sync.md)接口。
