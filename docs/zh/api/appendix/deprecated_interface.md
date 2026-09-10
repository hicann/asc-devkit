# 废弃接口<a name="ZH-CN_TOPIC_0000001724165365"></a>

## 基础API

- [WriteGmByPassDCache\(ISASI\)](../SIMD-API/basic_api/deprecated_interface/WriteGmByPassDCache_ISASI_deprecated.md)接口

    此接口后续版本会废弃，请使用[WriteGmBypassDCache\(ISASI\)](../SIMD-API/basic_api/scalar_compute/WriteGmBypassDCache_ISASI.md)接口。

- [ReadGmByPassDCache\(ISASI\)](../SIMD-API/basic_api/deprecated_interface/ReadGmByPassDCache_ISASI_deprecated.md)接口

    此接口后续版本会废弃，请使用[ReadGmBypassDCache\(ISASI\)](../SIMD-API/basic_api/scalar_compute/ReadGmBypassDCache_ISASI.md)接口。

## C API

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

### 特殊寄存器访问

- [asc_get_ar_spr](../SIMD-API/c_api/deprecated_interface/asc_get_ar_spr_deprecated.md)接口

    此接口后续版本会废弃，请使用[asc_get_squeeze_status](../SIMD-API/c_api/spr/asc_get_squeeze_status.md)接口。

<!-- end id6 -->

<!-- npu="950" id7 -->
### Reg矢量计算

#### Reg数据搬入

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

#### Reg数据搬出

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
#### 逻辑计算

- [asc_and](../SIMD-API/c_api/deprecated_interface/asc_and_deprecated.md)接口（浮点类型重载）

    `asc_and`的浮点类型重载已废弃，请按位宽选择对应整数重载替代，详情请参见[asc_and](../SIMD-API/c_api/reg_compute/reg_logic/asc_and.md)。

- [asc_or](../SIMD-API/c_api/deprecated_interface/asc_or_deprecated.md)接口（浮点类型重载）

    `asc_or`的浮点类型重载已废弃，请按位宽选择对应整数重载替代，详情请参见[asc_or](../SIMD-API/c_api/reg_compute/reg_logic/asc_or.md)。

#### 类型转换

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

- [asc_bfloat162e2m1x2](../SIMD-API/c_api/deprecated_interface/asc_bfloat162e2m1x2_deprecated.md)接口

    旧原型已废弃，请使用[asc_bfloat162e2m1x2](../SIMD-API/c_api/reg_compute/reg_convert/asc_bfloat162e2m1x2.md)的显式位置参数重载。

- [asc_bfloat162e1m2x2](../SIMD-API/c_api/deprecated_interface/asc_bfloat162e1m2x2_deprecated.md)接口

    旧原型已废弃，请使用[asc_bfloat162e1m2x2](../SIMD-API/c_api/reg_compute/reg_convert/asc_bfloat162e1m2x2.md)的显式位置参数重载。

- [asc_e2m1x22bfloat16](../SIMD-API/c_api/deprecated_interface/asc_e2m1x22bfloat16_deprecated.md)接口

    旧原型已废弃，请使用[asc_e2m1x22bfloat16](../SIMD-API/c_api/reg_compute/reg_convert/asc_e2m1x22bfloat16.md)的显式位置参数重载。

- [asc_e1m2x22bfloat16](../SIMD-API/c_api/deprecated_interface/asc_e1m2x22bfloat16_deprecated.md)接口

    旧原型已废弃，请使用[asc_e1m2x22bfloat16](../SIMD-API/c_api/reg_compute/reg_convert/asc_e1m2x22bfloat16.md)的显式位置参数重载。

- [asc_float2e5m2](../SIMD-API/c_api/deprecated_interface/asc_float2e5m2_deprecated.md)接口

    旧原型已废弃，请使用[asc_float2e5m2](../SIMD-API/c_api/reg_compute/reg_convert/asc_float2e5m2.md)的显式位置参数重载。

- [asc_e5m22float](../SIMD-API/c_api/deprecated_interface/asc_e5m22float_deprecated.md)接口

    旧原型已废弃，请使用[asc_e5m22float](../SIMD-API/c_api/reg_compute/reg_convert/asc_e5m22float.md)的显式位置参数重载。

- [asc_float2e4m3](../SIMD-API/c_api/deprecated_interface/asc_float2e4m3_deprecated.md)接口

    旧原型已废弃，请使用[asc_float2e4m3](../SIMD-API/c_api/reg_compute/reg_convert/asc_float2e4m3.md)的显式位置参数重载。

- [asc_e4m32float](../SIMD-API/c_api/deprecated_interface/asc_e4m32float_deprecated.md)接口

    旧原型已废弃，请使用[asc_e4m32float](../SIMD-API/c_api/reg_compute/reg_convert/asc_e4m32float.md)的显式位置参数重载。

- [asc_float2hif8](../SIMD-API/c_api/deprecated_interface/asc_float2hif8_deprecated.md)接口

    旧原型已废弃，请使用[asc_float2hif8](../SIMD-API/c_api/reg_compute/reg_convert/asc_float2hif8.md)的显式位置参数重载。

- [asc_hif82float](../SIMD-API/c_api/deprecated_interface/asc_hif82float_deprecated.md)接口

    旧原型已废弃，请使用[asc_hif82float](../SIMD-API/c_api/reg_compute/reg_convert/asc_hif82float.md)的显式位置参数重载。

- [asc_half2int4x2](../SIMD-API/c_api/deprecated_interface/asc_half2int4x2_deprecated.md)接口

    此接口后续版本会废弃。

- [asc_int4x22int16](../SIMD-API/c_api/deprecated_interface/asc_int4x22int16_deprecated.md)接口

    此接口后续版本会废弃。

- [asc_int4x22half](../SIMD-API/c_api/deprecated_interface/asc_int4x22half_deprecated.md)接口

    此接口后续版本会废弃。

- [asc_int4x22bfloat16](../SIMD-API/c_api/deprecated_interface/asc_int4x22bfloat16_deprecated.md)接口

    此接口后续版本会废弃。

- [asc_int82int32](../SIMD-API/c_api/deprecated_interface/asc_int82int32_deprecated.md)接口

    旧原型已废弃，请使用[asc_int82int32](../SIMD-API/c_api/reg_compute/reg_convert/asc_int82int32.md)的显式位置参数重载。

- [asc_int322uint8](../SIMD-API/c_api/deprecated_interface/asc_int322uint8_deprecated.md)接口

    旧原型已废弃，请使用[asc_int322uint8](../SIMD-API/c_api/reg_compute/reg_convert/asc_int322uint8.md)的显式位置参数重载。

- [asc_uint82uint32](../SIMD-API/c_api/deprecated_interface/asc_uint82uint32_deprecated.md)接口

    旧原型已废弃，请使用[asc_uint82uint32](../SIMD-API/c_api/reg_compute/reg_convert/asc_uint82uint32.md)的显式位置参数重载。

- [asc_uint322uint8](../SIMD-API/c_api/deprecated_interface/asc_uint322uint8_deprecated.md)接口

    旧原型已废弃，请使用[asc_uint322uint8](../SIMD-API/c_api/reg_compute/reg_convert/asc_uint322uint8.md)的显式位置参数重载。

#### 复合计算

- [asc_muls](../SIMD-API/c_api/deprecated_interface/asc_muls_deprecated.md)接口

    此接口后续版本会废弃。

- [asc_exp_sub](../SIMD-API/c_api/deprecated_interface/asc_exp_sub_deprecated.md)接口

    `half`输入且不带`src_pos`参数的`asc_exp_sub`原型以及`asc_exp_sub_v2`原型已废弃。`float`输入请使用`asc_exp_sub`原型，`half`输入请使用带显式`src_pos`参数的`asc_exp_sub_half2float`原型。

#### 排布变换

- [asc_squeeze_v2](../SIMD-API/c_api/deprecated_interface/asc_squeeze_deprecated.md)接口

    此接口后续版本会废弃，请使用[asc_squeeze_and_storeunalign](../SIMD-API/c_api/reg_compute/reg_permute_sel/asc_squeeze_and_storeunalign.md)接口。

- [asc_squeeze_with_status](../SIMD-API/c_api/deprecated_interface/asc_squeeze_with_status_deprecated.md)接口

    此接口后续版本会废弃，请使用[asc_squeeze_and_storeunalign](../SIMD-API/c_api/reg_compute/reg_permute_sel/asc_squeeze_and_storeunalign.md)接口。

- [asc_clear_ar_spr](../SIMD-API/c_api/deprecated_interface/asc_clear_ar_spr_deprecated.md)接口

    此接口后续版本会废弃，请使用[asc_squeeze_and_storeunalign_init](../SIMD-API/c_api/reg_compute/reg_permute_sel/asc_squeeze_and_storeunalign_init.md)接口。

- [asc_pack](../SIMD-API/c_api/deprecated_interface/asc_pack_deprecated.md)接口

    此接口后续版本会废弃，请使用[asc_pack_to_low](../SIMD-API/c_api/reg_compute/reg_permute_sel/asc_pack_to_low.md)接口。

- [asc_pack_v2](../SIMD-API/c_api/deprecated_interface/asc_pack_deprecated.md)接口

    此接口后续版本会废弃，请使用[asc_pack_to_high](../SIMD-API/c_api/reg_compute/reg_permute_sel/asc_pack_to_high.md)接口。

#### 地址寄存器操作

- [asc_create_iter_reg](../SIMD-API/c_api/deprecated_interface/asc_create_iter_reg_deprecated.md)接口

    此接口后续版本会废弃，请使用[asc_update_addr_reg](../SIMD-API/c_api/reg_compute/reg_addr_reg/asc_update_addr_reg.md)接口。

<!-- end id8 -->

### 同步控制

- [asc_sync_vec（无入参原型）](../SIMD-API/c_api/sync/asc_sync_vec.md)接口

    `asc_sync_vec()`接口已废弃，请使用[asc_sync](../SIMD-API/c_api/sync/asc_sync.md)执行针对所有流水线的同步操作。

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
### 矩阵计算搬运

- [asc_copy_gm2l0a_sync（废弃）](../SIMD-API/c_api/deprecated_interface/asc_copy_gm2l0a_sync_deprecated.md)接口

    `asc_copy_gm2l0a_sync()`已废弃，请使用[对应的非同步接口](../SIMD-API/c_api/cube_datamove/asc_copy_gm2l0a.md)和[asc_sync](../SIMD-API/c_api/sync/asc_sync.md)接口替代。

- [asc_copy_gm2l0b_sync（废弃）](../SIMD-API/c_api/deprecated_interface/asc_copy_gm2l0b_sync_deprecated.md)接口

    `asc_copy_gm2l0b_sync()`已废弃，请使用[对应的非同步接口](../SIMD-API/c_api/cube_datamove/asc_copy_gm2l0b.md)和[asc_sync](../SIMD-API/c_api/sync/asc_sync.md)接口替代。

- [asc_copy_gm2l1_align_sync（废弃）](../SIMD-API/c_api/deprecated_interface/asc_copy_gm2l1_align_sync_deprecated.md)接口

    `asc_copy_gm2l1_align_sync()`已废弃，请使用[对应的非同步接口](../SIMD-API/c_api/cube_datamove/asc_copy_gm2l1_align.md)和[asc_sync](../SIMD-API/c_api/sync/asc_sync.md)接口替代。

- [asc_copy_gm2l1_dn2nz_sync（废弃）](../SIMD-API/c_api/deprecated_interface/asc_copy_gm2l1_dn2nz_sync_deprecated.md)接口

    `asc_copy_gm2l1_dn2nz_sync()`已废弃，请使用[对应的非同步接口](../SIMD-API/c_api/cube_datamove/asc_copy_gm2l1_dn2nz.md)和[asc_sync](../SIMD-API/c_api/sync/asc_sync.md)接口替代。

- [asc_copy_gm2l1_nd2nz_sync_arch_2201（废弃）](../SIMD-API/c_api/deprecated_interface/asc_copy_gm2l1_nd2nz_sync_arch_2201_deprecated.md)接口

    `asc_copy_gm2l1_nd2nz_sync()`已废弃，请使用[对应的非同步接口](../SIMD-API/c_api/cube_datamove/asc_copy_gm2l1_nd2nz/asc_copy_gm2l1_nd2nz_arch_2201.md)和[asc_sync](../SIMD-API/c_api/sync/asc_sync.md)接口替代。

- [asc_copy_gm2l1_nd2nz_sync_arch_3510（废弃）](../SIMD-API/c_api/deprecated_interface/asc_copy_gm2l1_nd2nz_sync_arch_3510_deprecated.md)接口

    `asc_copy_gm2l1_nd2nz_sync()`已废弃，请使用[对应的非同步接口](../SIMD-API/c_api/cube_datamove/asc_copy_gm2l1_nd2nz/asc_copy_gm2l1_nd2nz_arch_3510.md)和[asc_sync](../SIMD-API/c_api/sync/asc_sync.md)接口替代。

- [asc_copy_gm2l1_sync_2d_arch_3510（废弃）](../SIMD-API/c_api/deprecated_interface/asc_copy_gm2l1_sync_2d_arch_3510_deprecated.md)接口

    `asc_copy_gm2l1_sync()`已废弃，请使用[对应的非同步接口](../SIMD-API/c_api/cube_datamove/asc_copy_gm2l1/asc_copy_gm2l1_2d_arch_3510.md)和[asc_sync](../SIMD-API/c_api/sync/asc_sync.md)接口替代。

- [asc_copy_gm2l1_sync_arch_2201（废弃）](../SIMD-API/c_api/deprecated_interface/asc_copy_gm2l1_sync_arch_2201_deprecated.md)接口

    `asc_copy_gm2l1_sync()`、`asc_copy_gm2l1_pad1_sync()`、`asc_copy_gm2l1_pad2_sync()`、`asc_copy_gm2l1_pad3_sync()`、`asc_copy_gm2l1_pad4_sync()`、`asc_copy_gm2l1_pad5_sync()`、`asc_copy_gm2l1_pad6_sync()`、`asc_copy_gm2l1_pad7_sync()`、`asc_copy_gm2l1_pad8_sync()`已废弃，请使用[对应的非同步接口](../SIMD-API/c_api/cube_datamove/asc_copy_gm2l1/asc_copy_gm2l1_arch_2201.md)和[asc_sync](../SIMD-API/c_api/sync/asc_sync.md)接口替代。

- [asc_copy_gm2l1_sync_highdim_split_arch_3510（废弃）](../SIMD-API/c_api/deprecated_interface/asc_copy_gm2l1_sync_highdim_split_arch_3510_deprecated.md)接口

    `asc_copy_gm2l1_sync()`已废弃，请使用[对应的非同步接口](../SIMD-API/c_api/cube_datamove/asc_copy_gm2l1/asc_copy_gm2l1_highdim_split_arch_3510.md)和[asc_sync](../SIMD-API/c_api/sync/asc_sync.md)接口替代。

- [asc_copy_l0c2gm_sync_arch_2201（废弃）](../SIMD-API/c_api/deprecated_interface/asc_copy_l0c2gm_sync_arch_2201_deprecated.md)接口

    `asc_copy_l0c2gm_sync()`已废弃，请使用[对应的非同步接口](../SIMD-API/c_api/cube_datamove/asc_copy_l0c2gm/asc_copy_l0c2gm_arch_2201.md)和[asc_sync](../SIMD-API/c_api/sync/asc_sync.md)接口替代。

- [asc_copy_l0c2gm_sync_arch_3510（废弃）](../SIMD-API/c_api/deprecated_interface/asc_copy_l0c2gm_sync_arch_3510_deprecated.md)接口

    `asc_copy_l0c2gm_sync()`已废弃，请使用[对应的非同步接口](../SIMD-API/c_api/cube_datamove/asc_copy_l0c2gm/asc_copy_l0c2gm_arch_3510.md)和[asc_sync](../SIMD-API/c_api/sync/asc_sync.md)接口替代。

- [asc_copy_l0c2l1_sync_arch_2201（废弃）](../SIMD-API/c_api/deprecated_interface/asc_copy_l0c2l1_sync_arch_2201_deprecated.md)接口

    `asc_copy_l0c2l1_sync()`已废弃，请使用[对应的非同步接口](../SIMD-API/c_api/cube_datamove/asc_copy_l0c2l1/asc_copy_l0c2l1_arch_2201.md)和[asc_sync](../SIMD-API/c_api/sync/asc_sync.md)接口替代。

- [asc_copy_l0c2l1_sync_arch_3510（废弃）](../SIMD-API/c_api/deprecated_interface/asc_copy_l0c2l1_sync_arch_3510_deprecated.md)接口

    `asc_copy_l0c2l1_sync()`已废弃，请使用[对应的非同步接口](../SIMD-API/c_api/cube_datamove/asc_copy_l0c2l1/asc_copy_l0c2l1_arch_3510.md)和[asc_sync](../SIMD-API/c_api/sync/asc_sync.md)接口替代。

- [asc_copy_l0c2ub_sync（废弃）](../SIMD-API/c_api/deprecated_interface/asc_copy_l0c2ub_sync_deprecated.md)接口

    `asc_copy_l0c2ub_sync()`已废弃，请使用[对应的非同步接口](../SIMD-API/c_api/cube_datamove/asc_copy_l0c2ub.md)和[asc_sync](../SIMD-API/c_api/sync/asc_sync.md)接口替代。

- [asc_copy_l12bt_sync_arch_2201（废弃）](../SIMD-API/c_api/deprecated_interface/asc_copy_l12bt_sync_arch_2201_deprecated.md)接口

    `asc_copy_l12bt_sync()`已废弃，请使用[对应的非同步接口](../SIMD-API/c_api/cube_datamove/asc_copy_l12bt/asc_copy_l12bt_arch_2201.md)和[asc_sync](../SIMD-API/c_api/sync/asc_sync.md)接口替代。

- [asc_copy_l12bt_sync_arch_3510（废弃）](../SIMD-API/c_api/deprecated_interface/asc_copy_l12bt_sync_arch_3510_deprecated.md)接口

    `asc_copy_l12bt_sync()`已废弃，请使用[对应的非同步接口](../SIMD-API/c_api/cube_datamove/asc_copy_l12bt/asc_copy_l12bt_arch_3510.md)和[asc_sync](../SIMD-API/c_api/sync/asc_sync.md)接口替代。

- [asc_copy_l12fb_sync_arch_2201（废弃）](../SIMD-API/c_api/deprecated_interface/asc_copy_l12fb_sync_arch_2201_deprecated.md)接口

    `asc_copy_l12fb_sync()`已废弃，请使用[对应的非同步接口](../SIMD-API/c_api/cube_datamove/asc_copy_l12fb/asc_copy_l12fb_arch_2201.md)和[asc_sync](../SIMD-API/c_api/sync/asc_sync.md)接口替代。

- [asc_copy_l12fb_sync_arch_3510（废弃）](../SIMD-API/c_api/deprecated_interface/asc_copy_l12fb_sync_arch_3510_deprecated.md)接口

    `asc_copy_l12fb_sync()`已废弃，请使用[对应的非同步接口](../SIMD-API/c_api/cube_datamove/asc_copy_l12fb/asc_copy_l12fb_arch_3510.md)和[asc_sync](../SIMD-API/c_api/sync/asc_sync.md)接口替代。

- [asc_copy_l12gm_sync（废弃）](../SIMD-API/c_api/deprecated_interface/asc_copy_l12gm_sync_deprecated.md)接口

    `asc_copy_l12gm_sync()`已废弃，请使用[对应的非同步接口](../SIMD-API/c_api/cube_datamove/asc_copy_l12gm.md)和[asc_sync](../SIMD-API/c_api/sync/asc_sync.md)接口替代。

- [asc_copy_l12l0a_mx_sync（废弃）](../SIMD-API/c_api/deprecated_interface/asc_copy_l12l0a_mx_sync_deprecated.md)接口

    `asc_copy_l12l0a_mx_sync()`已废弃，请使用[对应的非同步接口](../SIMD-API/c_api/cube_datamove/asc_copy_l12l0a_mx.md)和[asc_sync](../SIMD-API/c_api/sync/asc_sync.md)接口替代。

- [asc_copy_l12l0a_sync_2d_arch_3510（废弃）](../SIMD-API/c_api/deprecated_interface/asc_copy_l12l0a_sync_2d_arch_3510_deprecated.md)接口

    `asc_copy_l12l0a_sync()`、`asc_copy_l12l0a_transpose_sync()`已废弃，请使用[对应的非同步接口](../SIMD-API/c_api/cube_datamove/asc_copy_l12l0a/asc_copy_l12l0a_2d_arch_3510.md)和[asc_sync](../SIMD-API/c_api/sync/asc_sync.md)接口替代。

- [asc_copy_l12l0a_sync_3d_arch_3510（废弃）](../SIMD-API/c_api/deprecated_interface/asc_copy_l12l0a_sync_3d_arch_3510_deprecated.md)接口

    `asc_copy_l12l0a_sync()`已废弃，请使用[对应的非同步接口](../SIMD-API/c_api/cube_datamove/asc_copy_l12l0a/asc_copy_l12l0a_3d_arch_3510.md)和[asc_sync](../SIMD-API/c_api/sync/asc_sync.md)接口替代。

- [asc_copy_l12l0a_sync_arch_2201（废弃）](../SIMD-API/c_api/deprecated_interface/asc_copy_l12l0a_sync_arch_2201_deprecated.md)接口

    `asc_copy_l12l0a_sync()`已废弃，请使用[对应的非同步接口](../SIMD-API/c_api/cube_datamove/asc_copy_l12l0a/asc_copy_l12l0a_arch_2201.md)和[asc_sync](../SIMD-API/c_api/sync/asc_sync.md)接口替代。

- [asc_copy_l12l0a_trans_sync（废弃）](../SIMD-API/c_api/deprecated_interface/asc_copy_l12l0a_trans_sync_deprecated.md)接口

    `asc_copy_l12l0a_trans_sync()`已废弃，请使用[对应的非同步接口](../SIMD-API/c_api/cube_datamove/asc_copy_l12l0a_trans.md)和[asc_sync](../SIMD-API/c_api/sync/asc_sync.md)接口替代。

- [asc_copy_l12l0b_mx_sync（废弃）](../SIMD-API/c_api/deprecated_interface/asc_copy_l12l0b_mx_sync_deprecated.md)接口

    `asc_copy_l12l0b_mx_sync()`已废弃，请使用[对应的非同步接口](../SIMD-API/c_api/cube_datamove/asc_copy_l12l0b_mx.md)和[asc_sync](../SIMD-API/c_api/sync/asc_sync.md)接口替代。

- [asc_copy_l12l0b_sparse_sync（废弃）](../SIMD-API/c_api/deprecated_interface/asc_copy_l12l0b_sparse_sync_deprecated.md)接口

    `asc_copy_l12l0b_sparse_sync()`已废弃，请使用[对应的非同步接口](../SIMD-API/c_api/cube_datamove/asc_copy_l12l0b_sparse.md)和[asc_sync](../SIMD-API/c_api/sync/asc_sync.md)接口替代。

- [asc_copy_l12l0b_sync_2d_arch_3510（废弃）](../SIMD-API/c_api/deprecated_interface/asc_copy_l12l0b_sync_2d_arch_3510_deprecated.md)接口

    `asc_copy_l12l0b_sync()`、`asc_copy_l12l0b_transpose_sync()`已废弃，请使用[对应的非同步接口](../SIMD-API/c_api/cube_datamove/asc_copy_l12l0b/asc_copy_l12l0b_2d_arch_3510.md)和[asc_sync](../SIMD-API/c_api/sync/asc_sync.md)接口替代。

- [asc_copy_l12l0b_sync_3d_arch_3510（废弃）](../SIMD-API/c_api/deprecated_interface/asc_copy_l12l0b_sync_3d_arch_3510_deprecated.md)接口

    `asc_copy_l12l0b_sync()`已废弃，请使用[对应的非同步接口](../SIMD-API/c_api/cube_datamove/asc_copy_l12l0b/asc_copy_l12l0b_3d_arch_3510.md)和[asc_sync](../SIMD-API/c_api/sync/asc_sync.md)接口替代。

- [asc_copy_l12l0b_sync_arch_2201（废弃）](../SIMD-API/c_api/deprecated_interface/asc_copy_l12l0b_sync_arch_2201_deprecated.md)接口

    `asc_copy_l12l0b_sync()`已废弃，请使用[对应的非同步接口](../SIMD-API/c_api/cube_datamove/asc_copy_l12l0b/asc_copy_l12l0b_arch_2201.md)和[asc_sync](../SIMD-API/c_api/sync/asc_sync.md)接口替代。

- [asc_copy_l12l0b_trans_sync_arch_2201（废弃）](../SIMD-API/c_api/deprecated_interface/asc_copy_l12l0b_trans_sync_arch_2201_deprecated.md)接口

    `asc_copy_l12l0b_trans_sync()`已废弃，请使用[对应的非同步接口](../SIMD-API/c_api/cube_datamove/asc_copy_l12l0b_trans/asc_copy_l12l0b_trans_arch_2201.md)和[asc_sync](../SIMD-API/c_api/sync/asc_sync.md)接口替代。

- [asc_copy_l12l0b_trans_sync_arch_3510（废弃）](../SIMD-API/c_api/deprecated_interface/asc_copy_l12l0b_trans_sync_arch_3510_deprecated.md)接口

    `asc_copy_l12l0b_trans_sync()`已废弃，请使用[对应的非同步接口](../SIMD-API/c_api/cube_datamove/asc_copy_l12l0b_trans/asc_copy_l12l0b_trans_arch_3510.md)和[asc_sync](../SIMD-API/c_api/sync/asc_sync.md)接口替代。

- [asc_copy_l12l0c_sync（废弃）](../SIMD-API/c_api/deprecated_interface/asc_copy_l12l0c_sync_deprecated.md)接口

    `asc_copy_l12l0c_sync()`已废弃，请使用[对应的非同步接口](../SIMD-API/c_api/cube_datamove/asc_copy_l12l0c.md)和[asc_sync](../SIMD-API/c_api/sync/asc_sync.md)接口替代。

- [asc_copy_l12ub_sync（废弃）](../SIMD-API/c_api/deprecated_interface/asc_copy_l12ub_sync_deprecated.md)接口

    `asc_copy_l12ub_sync()`已废弃，请使用[对应的非同步接口](../SIMD-API/c_api/cube_datamove/asc_copy_l12ub.md)和[asc_sync](../SIMD-API/c_api/sync/asc_sync.md)接口替代。

- [asc_fill_l0a_sync（废弃）](../SIMD-API/c_api/deprecated_interface/asc_fill_l0a_sync_deprecated.md)接口

    `asc_fill_l0a_sync()`已废弃，请使用[对应的非同步接口](../SIMD-API/c_api/cube_datamove/asc_fill_l0a.md)和[asc_sync](../SIMD-API/c_api/sync/asc_sync.md)接口替代。

- [asc_fill_l0b_sync（废弃）](../SIMD-API/c_api/deprecated_interface/asc_fill_l0b_sync_deprecated.md)接口

    `asc_fill_l0b_sync()`已废弃，请使用[对应的非同步接口](../SIMD-API/c_api/cube_datamove/asc_fill_l0b.md)和[asc_sync](../SIMD-API/c_api/sync/asc_sync.md)接口替代。

- [asc_fill_l1_sync（废弃）](../SIMD-API/c_api/deprecated_interface/asc_fill_l1_sync_deprecated.md)接口

    `asc_fill_l1_sync()`已废弃，请使用[对应的非同步接口](../SIMD-API/c_api/cube_datamove/asc_fill_l1.md)和[asc_sync](../SIMD-API/c_api/sync/asc_sync.md)接口替代。

- [asc_load_image_to_cbuf_sync（废弃）](../SIMD-API/c_api/deprecated_interface/asc_load_image_to_cbuf_sync_deprecated.md)接口

    `asc_load_image_to_cbuf_sync()`已废弃，请使用[对应的非同步接口](../SIMD-API/c_api/cube_datamove/asc_load_image_to_cbuf.md)和[asc_sync](../SIMD-API/c_api/sync/asc_sync.md)接口替代。

### 矢量数据搬运

<!-- npu="950" id1 -->
- [asc_set_gm2ub_pad](../SIMD-API/c_api/deprecated_interface/asc_set_gm2ub_pad_deprecated.md)接口

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

- [asc_copy_gm2ub_align_sync_arch_2201（废弃）](../SIMD-API/c_api/deprecated_interface/asc_copy_gm2ub_align_sync_arch_2201_deprecated.md)接口

    `asc_copy_gm2ub_align_sync()`已废弃，请使用[对应的非同步接口](../SIMD-API/c_api/vector_datamove/asc_copy_gm2ub_align/asc_copy_gm2ub_align_arch_2201.md)和[asc_sync](../SIMD-API/c_api/sync/asc_sync.md)接口替代。

- [asc_copy_gm2ub_sync_arch_2201（废弃）](../SIMD-API/c_api/deprecated_interface/asc_copy_gm2ub_sync_arch_2201_deprecated.md)接口

    `asc_copy_gm2ub_sync()`已废弃，请使用[对应的非同步接口](../SIMD-API/c_api/vector_datamove/asc_copy_gm2ub/asc_copy_gm2ub_arch_2201.md)和[asc_sync](../SIMD-API/c_api/sync/asc_sync.md)接口替代。

- [asc_copy_gm2ub_sync_arch_3510（废弃）](../SIMD-API/c_api/deprecated_interface/asc_copy_gm2ub_sync_arch_3510_deprecated.md)接口

    `asc_copy_gm2ub_sync()`已废弃，请使用[对应的非同步接口](../SIMD-API/c_api/vector_datamove/asc_copy_gm2ub/asc_copy_gm2ub_arch_3510.md)和[asc_sync](../SIMD-API/c_api/sync/asc_sync.md)接口替代。

- [asc_copy_sync（废弃）](../SIMD-API/c_api/deprecated_interface/asc_copy_sync_deprecated.md)接口

    `asc_copy_sync()`已废弃，请使用[对应的非同步接口](../SIMD-API/c_api/vector_datamove/asc_copy.md)和[asc_sync](../SIMD-API/c_api/sync/asc_sync.md)接口替代。

- [asc_copy_ub2gm_align_sync_arch_2201（废弃）](../SIMD-API/c_api/deprecated_interface/asc_copy_ub2gm_align_sync_arch_2201_deprecated.md)接口

    `asc_copy_ub2gm_align_sync()`已废弃，请使用[对应的非同步接口](../SIMD-API/c_api/vector_datamove/asc_copy_ub2gm_align/asc_copy_ub2gm_align_arch_2201.md)和[asc_sync](../SIMD-API/c_api/sync/asc_sync.md)接口替代。

- [asc_copy_ub2gm_sync_arch_2201（废弃）](../SIMD-API/c_api/deprecated_interface/asc_copy_ub2gm_sync_arch_2201_deprecated.md)接口

    `asc_copy_ub2gm_sync()`已废弃，请使用[对应的非同步接口](../SIMD-API/c_api/vector_datamove/asc_copy_ub2gm/asc_copy_ub2gm_arch_2201.md)和[asc_sync](../SIMD-API/c_api/sync/asc_sync.md)接口替代。

- [asc_copy_ub2gm_sync_arch_3510（废弃）](../SIMD-API/c_api/deprecated_interface/asc_copy_ub2gm_sync_arch_3510_deprecated.md)接口

    `asc_copy_ub2gm_sync()`已废弃，请使用[对应的非同步接口](../SIMD-API/c_api/vector_datamove/asc_copy_ub2gm/asc_copy_ub2gm_arch_3510.md)和[asc_sync](../SIMD-API/c_api/sync/asc_sync.md)接口替代。

- [asc_copy_ub2l1_sync（废弃）](../SIMD-API/c_api/deprecated_interface/asc_copy_ub2l1_sync_deprecated.md)接口

    `asc_copy_ub2l1_sync()`已废弃，请使用[asc_copy_ub2l1](../SIMD-API/c_api/vector_datamove/asc_copy_ub2l1.md)和[asc_sync](../SIMD-API/c_api/sync/asc_sync.md)接口替代。

- [asc_copy_ub2ub_sync（废弃）](../SIMD-API/c_api/deprecated_interface/asc_copy_ub2ub_sync_deprecated.md)接口

    `asc_copy_ub2ub_sync()`已废弃，请使用[对应的非同步接口](../SIMD-API/c_api/vector_datamove/asc_copy_ub2ub.md)和[asc_sync](../SIMD-API/c_api/sync/asc_sync.md)接口替代。

### Memory矢量计算

#### 基础算术

- [asc_abs_sync](../SIMD-API/c_api/deprecated_interface/asc_abs_sync_deprecated.md)接口

    `asc_abs_sync()`接口已废弃。请使用[asc_abs](../SIMD-API/c_api/vector_compute/vector_arith/asc_abs.md)接口和[asc_sync](../SIMD-API/c_api/sync/asc_sync.md)接口。

- [asc_add_scalar_sync（废弃）](../SIMD-API/c_api/deprecated_interface/asc_add_scalar_sync_deprecated.md)接口

    `asc_add_scalar_sync()`已废弃，请使用[对应的非同步接口](../SIMD-API/c_api/vector_compute/vector_arith/asc_add_scalar.md)和[asc_sync](../SIMD-API/c_api/sync/asc_sync.md)接口替代。

- [asc_add_sync（废弃）](../SIMD-API/c_api/deprecated_interface/asc_add_sync_deprecated.md)接口

    `asc_add_sync()`已废弃，请使用[对应的非同步接口](../SIMD-API/c_api/vector_compute/vector_arith/asc_add.md)和[asc_sync](../SIMD-API/c_api/sync/asc_sync.md)接口替代。

- [asc_div_sync（废弃）](../SIMD-API/c_api/deprecated_interface/asc_div_sync_deprecated.md)接口

    `asc_div_sync()`已废弃，请使用[对应的非同步接口](../SIMD-API/c_api/vector_compute/vector_arith/asc_div.md)和[asc_sync](../SIMD-API/c_api/sync/asc_sync.md)接口替代。

- [asc_exp_sync（废弃）](../SIMD-API/c_api/deprecated_interface/asc_exp_sync_deprecated.md)接口

    `asc_exp_sync()`已废弃，请使用[对应的非同步接口](../SIMD-API/c_api/vector_compute/vector_arith/asc_exp.md)和[asc_sync](../SIMD-API/c_api/sync/asc_sync.md)接口替代。

- [asc_log_sync（废弃）](../SIMD-API/c_api/deprecated_interface/asc_log_sync_deprecated.md)接口

    `asc_log_sync()`已废弃，请使用[对应的非同步接口](../SIMD-API/c_api/vector_compute/vector_arith/asc_log.md)和[asc_sync](../SIMD-API/c_api/sync/asc_sync.md)接口替代。

- [asc_max_scalar_sync（废弃）](../SIMD-API/c_api/deprecated_interface/asc_max_scalar_sync_deprecated.md)接口

    `asc_max_scalar_sync()`已废弃，请使用[对应的非同步接口](../SIMD-API/c_api/vector_compute/vector_arith/asc_max_scalar.md)和[asc_sync](../SIMD-API/c_api/sync/asc_sync.md)接口替代。

- [asc_max_sync（废弃）](../SIMD-API/c_api/deprecated_interface/asc_max_sync_deprecated.md)接口

    `asc_max_sync()`已废弃，请使用[对应的非同步接口](../SIMD-API/c_api/vector_compute/vector_arith/asc_max.md)和[asc_sync](../SIMD-API/c_api/sync/asc_sync.md)接口替代。

- [asc_min_scalar_sync（废弃）](../SIMD-API/c_api/deprecated_interface/asc_min_scalar_sync_deprecated.md)接口

    `asc_min_scalar_sync()`已废弃，请使用[对应的非同步接口](../SIMD-API/c_api/vector_compute/vector_arith/asc_min_scalar.md)和[asc_sync](../SIMD-API/c_api/sync/asc_sync.md)接口替代。

- [asc_min_sync（废弃）](../SIMD-API/c_api/deprecated_interface/asc_min_sync_deprecated.md)接口

    `asc_min_sync()`已废弃，请使用[对应的非同步接口](../SIMD-API/c_api/vector_compute/vector_arith/asc_min.md)和[asc_sync](../SIMD-API/c_api/sync/asc_sync.md)接口替代。

- [asc_mul_scalar_sync（废弃）](../SIMD-API/c_api/deprecated_interface/asc_mul_scalar_sync_deprecated.md)接口

    `asc_mul_scalar_sync()`已废弃，请使用[对应的非同步接口](../SIMD-API/c_api/vector_compute/vector_arith/asc_mul_scalar.md)和[asc_sync](../SIMD-API/c_api/sync/asc_sync.md)接口替代。

- [asc_mul_sync（废弃）](../SIMD-API/c_api/deprecated_interface/asc_mul_sync_deprecated.md)接口

    `asc_mul_sync()`已废弃，请使用[对应的非同步接口](../SIMD-API/c_api/vector_compute/vector_arith/asc_mul.md)和[asc_sync](../SIMD-API/c_api/sync/asc_sync.md)接口替代。

- [asc_rcp_sync（废弃）](../SIMD-API/c_api/deprecated_interface/asc_rcp_sync_deprecated.md)接口

    `asc_rcp_sync()`已废弃，请使用[对应的非同步接口](../SIMD-API/c_api/vector_compute/vector_arith/asc_rcp.md)和[asc_sync](../SIMD-API/c_api/sync/asc_sync.md)接口替代。

- [asc_rsqrt_sync（废弃）](../SIMD-API/c_api/deprecated_interface/asc_rsqrt_sync_deprecated.md)接口

    `asc_rsqrt_sync()`已废弃，请使用[对应的非同步接口](../SIMD-API/c_api/vector_compute/vector_arith/asc_rsqrt.md)和[asc_sync](../SIMD-API/c_api/sync/asc_sync.md)接口替代。

- [asc_sqrt_sync（废弃）](../SIMD-API/c_api/deprecated_interface/asc_sqrt_sync_deprecated.md)接口

    `asc_sqrt_sync()`已废弃，请使用[对应的非同步接口](../SIMD-API/c_api/vector_compute/vector_arith/asc_sqrt.md)和[asc_sync](../SIMD-API/c_api/sync/asc_sync.md)接口替代。

- [asc_sub_scalar_sync（废弃）](../SIMD-API/c_api/deprecated_interface/asc_sub_scalar_sync_deprecated.md)接口

    `asc_sub_scalar_sync()`已废弃，请使用[对应的非同步接口](../SIMD-API/c_api/vector_compute/vector_arith/asc_sub_scalar.md)和[asc_sync](../SIMD-API/c_api/sync/asc_sync.md)接口替代。

- [asc_sub_sync（废弃）](../SIMD-API/c_api/deprecated_interface/asc_sub_sync_deprecated.md)接口

    `asc_sub_sync()`已废弃，请使用[对应的非同步接口](../SIMD-API/c_api/vector_compute/vector_arith/asc_sub.md)和[asc_sync](../SIMD-API/c_api/sync/asc_sync.md)接口替代。

#### 广播操作

- [asc_brcb_sync（废弃）](../SIMD-API/c_api/deprecated_interface/asc_brcb_sync_deprecated.md)接口

    `asc_brcb_sync()`已废弃，请使用[对应的非同步接口](../SIMD-API/c_api/vector_compute/vector_broadcast/asc_brcb.md)和[asc_sync](../SIMD-API/c_api/sync/asc_sync.md)接口替代。

- [asc_duplicate_sync（废弃）](../SIMD-API/c_api/deprecated_interface/asc_duplicate_sync_deprecated.md)接口

    `asc_duplicate_sync()`已废弃，请使用[对应的非同步接口](../SIMD-API/c_api/vector_compute/vector_broadcast/asc_duplicate.md)和[asc_sync](../SIMD-API/c_api/sync/asc_sync.md)接口替代。

#### 比较计算

- [asc_eq_scalar_sync（废弃）](../SIMD-API/c_api/deprecated_interface/asc_eq_scalar_sync_deprecated.md)接口

    `asc_eq_scalar_sync()`已废弃，请使用[对应的非同步接口](../SIMD-API/c_api/vector_compute/vector_compare/asc_eq_scalar.md)和[asc_sync](../SIMD-API/c_api/sync/asc_sync.md)接口替代。

- [asc_eq_sync（废弃）](../SIMD-API/c_api/deprecated_interface/asc_eq_sync_deprecated.md)接口

    `asc_eq_sync()`已废弃，请使用[对应的非同步接口](../SIMD-API/c_api/vector_compute/vector_compare/asc_eq.md)和[asc_sync](../SIMD-API/c_api/sync/asc_sync.md)接口替代。

- [asc_ge_scalar_sync（废弃）](../SIMD-API/c_api/deprecated_interface/asc_ge_scalar_sync_deprecated.md)接口

    `asc_ge_scalar_sync()`已废弃，请使用[对应的非同步接口](../SIMD-API/c_api/vector_compute/vector_compare/asc_ge_scalar.md)和[asc_sync](../SIMD-API/c_api/sync/asc_sync.md)接口替代。

- [asc_ge_sync（废弃）](../SIMD-API/c_api/deprecated_interface/asc_ge_sync_deprecated.md)接口

    `asc_ge_sync()`已废弃，请使用[对应的非同步接口](../SIMD-API/c_api/vector_compute/vector_compare/asc_ge.md)和[asc_sync](../SIMD-API/c_api/sync/asc_sync.md)接口替代。

- [asc_gt_scalar_sync（废弃）](../SIMD-API/c_api/deprecated_interface/asc_gt_scalar_sync_deprecated.md)接口

    `asc_gt_scalar_sync()`已废弃，请使用[对应的非同步接口](../SIMD-API/c_api/vector_compute/vector_compare/asc_gt_scalar.md)和[asc_sync](../SIMD-API/c_api/sync/asc_sync.md)接口替代。

- [asc_gt_sync（废弃）](../SIMD-API/c_api/deprecated_interface/asc_gt_sync_deprecated.md)接口

    `asc_gt_sync()`已废弃，请使用[对应的非同步接口](../SIMD-API/c_api/vector_compute/vector_compare/asc_gt.md)和[asc_sync](../SIMD-API/c_api/sync/asc_sync.md)接口替代。

- [asc_le_scalar_sync（废弃）](../SIMD-API/c_api/deprecated_interface/asc_le_scalar_sync_deprecated.md)接口

    `asc_le_scalar_sync()`已废弃，请使用[对应的非同步接口](../SIMD-API/c_api/vector_compute/vector_compare/asc_le_scalar.md)和[asc_sync](../SIMD-API/c_api/sync/asc_sync.md)接口替代。

- [asc_le_sync（废弃）](../SIMD-API/c_api/deprecated_interface/asc_le_sync_deprecated.md)接口

    `asc_le_sync()`已废弃，请使用[对应的非同步接口](../SIMD-API/c_api/vector_compute/vector_compare/asc_le.md)和[asc_sync](../SIMD-API/c_api/sync/asc_sync.md)接口替代。

- [asc_lt_scalar_sync（废弃）](../SIMD-API/c_api/deprecated_interface/asc_lt_scalar_sync_deprecated.md)接口

    `asc_lt_scalar_sync()`已废弃，请使用[对应的非同步接口](../SIMD-API/c_api/vector_compute/vector_compare/asc_lt_scalar.md)和[asc_sync](../SIMD-API/c_api/sync/asc_sync.md)接口替代。

- [asc_lt_sync（废弃）](../SIMD-API/c_api/deprecated_interface/asc_lt_sync_deprecated.md)接口

    `asc_lt_sync()`已废弃，请使用[对应的非同步接口](../SIMD-API/c_api/vector_compute/vector_compare/asc_lt.md)和[asc_sync](../SIMD-API/c_api/sync/asc_sync.md)接口替代。

- [asc_ne_scalar_sync（废弃）](../SIMD-API/c_api/deprecated_interface/asc_ne_scalar_sync_deprecated.md)接口

    `asc_ne_scalar_sync()`已废弃，请使用[对应的非同步接口](../SIMD-API/c_api/vector_compute/vector_compare/asc_ne_scalar.md)和[asc_sync](../SIMD-API/c_api/sync/asc_sync.md)接口替代。

- [asc_ne_sync（废弃）](../SIMD-API/c_api/deprecated_interface/asc_ne_sync_deprecated.md)接口

    `asc_ne_sync()`已废弃，请使用[对应的非同步接口](../SIMD-API/c_api/vector_compute/vector_compare/asc_ne.md)和[asc_sync](../SIMD-API/c_api/sync/asc_sync.md)接口替代。

#### 类型转换

- [asc_bfloat162float_sync（废弃）](../SIMD-API/c_api/deprecated_interface/asc_bfloat162float_sync_deprecated.md)接口

    `asc_bfloat162float_sync()`已废弃，请使用[对应的非同步接口](../SIMD-API/c_api/vector_compute/vector_convert/asc_bfloat162float.md)和[asc_sync](../SIMD-API/c_api/sync/asc_sync.md)接口替代。

- [asc_bfloat162int32_sync（废弃）](../SIMD-API/c_api/deprecated_interface/asc_bfloat162int32_sync_deprecated.md)接口

    `asc_bfloat162int32_rn_sync()`、`asc_bfloat162int32_rna_sync()`、`asc_bfloat162int32_rd_sync()`、`asc_bfloat162int32_ru_sync()`、`asc_bfloat162int32_rz_sync()`已废弃，请使用[对应的非同步接口](../SIMD-API/c_api/vector_compute/vector_convert/asc_bfloat162int32.md)和[asc_sync](../SIMD-API/c_api/sync/asc_sync.md)接口替代。

- [asc_float2bfloat16_sync（废弃）](../SIMD-API/c_api/deprecated_interface/asc_float2bfloat16_sync_deprecated.md)接口

    `asc_float2bfloat16_rn_sync()`、`asc_float2bfloat16_rna_sync()`、`asc_float2bfloat16_rd_sync()`、`asc_float2bfloat16_ru_sync()`、`asc_float2bfloat16_rz_sync()`已废弃，请使用[对应的非同步接口](../SIMD-API/c_api/vector_compute/vector_convert/asc_float2bfloat16.md)和[asc_sync](../SIMD-API/c_api/sync/asc_sync.md)接口替代。

- [asc_float2float_sync（废弃）](../SIMD-API/c_api/deprecated_interface/asc_float2float_sync_deprecated.md)接口

    `asc_float2float_rn_sync()`、`asc_float2float_rd_sync()`、`asc_float2float_ru_sync()`、`asc_float2float_rna_sync()`、`asc_float2float_rz_sync()`已废弃，请使用[对应的非同步接口](../SIMD-API/c_api/vector_compute/vector_convert/asc_float2float.md)和[asc_sync](../SIMD-API/c_api/sync/asc_sync.md)接口替代。

- [asc_float2half_sync（废弃）](../SIMD-API/c_api/deprecated_interface/asc_float2half_sync_deprecated.md)接口

    `asc_float2half_sync()`、`asc_float2half_rn_sync()`、`asc_float2half_rna_sync()`、`asc_float2half_rd_sync()`、`asc_float2half_ru_sync()`、`asc_float2half_rz_sync()`、`asc_float2half_ro_sync()`已废弃，请使用[对应的非同步接口](../SIMD-API/c_api/vector_compute/vector_convert/asc_float2half.md)和[asc_sync](../SIMD-API/c_api/sync/asc_sync.md)接口替代。

- [asc_float2int16_sync（废弃）](../SIMD-API/c_api/deprecated_interface/asc_float2int16_sync_deprecated.md)接口

    `asc_float2int16_rn_sync()`、`asc_float2int16_rna_sync()`、`asc_float2int16_rd_sync()`、`asc_float2int16_ru_sync()`、`asc_float2int16_rz_sync()`已废弃，请使用[对应的非同步接口](../SIMD-API/c_api/vector_compute/vector_convert/asc_float2int16.md)和[asc_sync](../SIMD-API/c_api/sync/asc_sync.md)接口替代。

- [asc_float2int32_sync（废弃）](../SIMD-API/c_api/deprecated_interface/asc_float2int32_sync_deprecated.md)接口

    `asc_float2int32_rn_sync()`、`asc_float2int32_rna_sync()`、`asc_float2int32_rd_sync()`、`asc_float2int32_ru_sync()`、`asc_float2int32_rz_sync()`已废弃，请使用[对应的非同步接口](../SIMD-API/c_api/vector_compute/vector_convert/asc_float2int32.md)和[asc_sync](../SIMD-API/c_api/sync/asc_sync.md)接口替代。

- [asc_float2int64_sync（废弃）](../SIMD-API/c_api/deprecated_interface/asc_float2int64_sync_deprecated.md)接口

    `asc_float2int64_rn_sync()`、`asc_float2int64_rna_sync()`、`asc_float2int64_rd_sync()`、`asc_float2int64_ru_sync()`、`asc_float2int64_rz_sync()`已废弃，请使用[对应的非同步接口](../SIMD-API/c_api/vector_compute/vector_convert/asc_float2int64.md)和[asc_sync](../SIMD-API/c_api/sync/asc_sync.md)接口替代。

- [asc_half2float_sync（废弃）](../SIMD-API/c_api/deprecated_interface/asc_half2float_sync_deprecated.md)接口

    `asc_half2float_sync()`已废弃，请使用[对应的非同步接口](../SIMD-API/c_api/vector_compute/vector_convert/asc_half2float.md)和[asc_sync](../SIMD-API/c_api/sync/asc_sync.md)接口替代。

- [asc_half2int16_sync（废弃）](../SIMD-API/c_api/deprecated_interface/asc_half2int16_sync_deprecated.md)接口

    `asc_half2int16_rn_sync()`、`asc_half2int16_rna_sync()`、`asc_half2int16_rd_sync()`、`asc_half2int16_ru_sync()`、`asc_half2int16_rz_sync()`已废弃，请使用[对应的非同步接口](../SIMD-API/c_api/vector_compute/vector_convert/asc_half2int16.md)和[asc_sync](../SIMD-API/c_api/sync/asc_sync.md)接口替代。

- [asc_half2int32_sync（废弃）](../SIMD-API/c_api/deprecated_interface/asc_half2int32_sync_deprecated.md)接口

    `asc_half2int32_rn_sync()`、`asc_half2int32_rna_sync()`、`asc_half2int32_rd_sync()`、`asc_half2int32_ru_sync()`、`asc_half2int32_rz_sync()`已废弃，请使用[对应的非同步接口](../SIMD-API/c_api/vector_compute/vector_convert/asc_half2int32.md)和[asc_sync](../SIMD-API/c_api/sync/asc_sync.md)接口替代。

- [asc_half2int4_sync（废弃）](../SIMD-API/c_api/deprecated_interface/asc_half2int4_sync_deprecated.md)接口

    `asc_half2int4_sync()`、`asc_half2int4_rn_sync()`、`asc_half2int4_rd_sync()`、`asc_half2int4_rna_sync()`、`asc_half2int4_ru_sync()`、`asc_half2int4_rz_sync()`已废弃，请使用[对应的非同步接口](../SIMD-API/c_api/vector_compute/vector_convert/asc_half2int4.md)和[asc_sync](../SIMD-API/c_api/sync/asc_sync.md)接口替代。

- [asc_half2int8_sync（废弃）](../SIMD-API/c_api/deprecated_interface/asc_half2int8_sync_deprecated.md)接口

    `asc_half2int8_sync()`、`asc_half2int8_rn_sync()`、`asc_half2int8_rna_sync()`、`asc_half2int8_rd_sync()`、`asc_half2int8_ru_sync()`、`asc_half2int8_rz_sync()`已废弃，请使用[对应的非同步接口](../SIMD-API/c_api/vector_compute/vector_convert/asc_half2int8.md)和[asc_sync](../SIMD-API/c_api/sync/asc_sync.md)接口替代。

- [asc_half2uint8_sync（废弃）](../SIMD-API/c_api/deprecated_interface/asc_half2uint8_sync_deprecated.md)接口

    `asc_half2uint8_sync()`、`asc_half2uint8_rn_sync()`、`asc_half2uint8_rd_sync()`、`asc_half2uint8_rna_sync()`、`asc_half2uint8_ru_sync()`、`asc_half2uint8_rz_sync()`已废弃，请使用[对应的非同步接口](../SIMD-API/c_api/vector_compute/vector_convert/asc_half2uint8.md)和[asc_sync](../SIMD-API/c_api/sync/asc_sync.md)接口替代。

- [asc_int162float_sync（废弃）](../SIMD-API/c_api/deprecated_interface/asc_int162float_sync_deprecated.md)接口

    `asc_int162float_sync()`已废弃，请使用[对应的非同步接口](../SIMD-API/c_api/vector_compute/vector_convert/asc_int162float.md)和[asc_sync](../SIMD-API/c_api/sync/asc_sync.md)接口替代。

- [asc_int162half_sync（废弃）](../SIMD-API/c_api/deprecated_interface/asc_int162half_sync_deprecated.md)接口

    `asc_int162half_sync()`、`asc_int162half_rn_sync()`、`asc_int162half_rna_sync()`、`asc_int162half_rd_sync()`、`asc_int162half_ru_sync()`、`asc_int162half_rz_sync()`已废弃，请使用[对应的非同步接口](../SIMD-API/c_api/vector_compute/vector_convert/asc_int162half.md)和[asc_sync](../SIMD-API/c_api/sync/asc_sync.md)接口替代。

- [asc_int322float_sync（废弃）](../SIMD-API/c_api/deprecated_interface/asc_int322float_sync_deprecated.md)接口

    `asc_int322float_sync()`、`asc_int322float_rn_sync()`、`asc_int322float_rna_sync()`、`asc_int322float_rd_sync()`、`asc_int322float_ru_sync()`、`asc_int322float_rz_sync()`已废弃，请使用[对应的非同步接口](../SIMD-API/c_api/vector_compute/vector_convert/asc_int322float.md)和[asc_sync](../SIMD-API/c_api/sync/asc_sync.md)接口替代。

- [asc_int322int16_sync（废弃）](../SIMD-API/c_api/deprecated_interface/asc_int322int16_sync_deprecated.md)接口

    `asc_int322int16_sync()`已废弃，请使用[对应的非同步接口](../SIMD-API/c_api/vector_compute/vector_convert/asc_int322int16.md)和[asc_sync](../SIMD-API/c_api/sync/asc_sync.md)接口替代。

- [asc_int322int64_sync（废弃）](../SIMD-API/c_api/deprecated_interface/asc_int322int64_sync_deprecated.md)接口

    `asc_int322int64_sync()`已废弃，请使用[对应的非同步接口](../SIMD-API/c_api/vector_compute/vector_convert/asc_int322int64.md)和[asc_sync](../SIMD-API/c_api/sync/asc_sync.md)接口替代。

- [asc_int42half_sync（废弃）](../SIMD-API/c_api/deprecated_interface/asc_int42half_sync_deprecated.md)接口

    `asc_int42half_sync()`已废弃，请使用[对应的非同步接口](../SIMD-API/c_api/vector_compute/vector_convert/asc_int42half.md)和[asc_sync](../SIMD-API/c_api/sync/asc_sync.md)接口替代。

- [asc_int642float_sync（废弃）](../SIMD-API/c_api/deprecated_interface/asc_int642float_sync_deprecated.md)接口

    `asc_int642float_rd_sync()`、`asc_int642float_rn_sync()`、`asc_int642float_rna_sync()`、`asc_int642float_ru_sync()`、`asc_int642float_rz_sync()`已废弃，请使用[对应的非同步接口](../SIMD-API/c_api/vector_compute/vector_convert/asc_int642float.md)和[asc_sync](../SIMD-API/c_api/sync/asc_sync.md)接口替代。

- [asc_int642int32_sync（废弃）](../SIMD-API/c_api/deprecated_interface/asc_int642int32_sync_deprecated.md)接口

    `asc_int642int32_sync()`已废弃，请使用[对应的非同步接口](../SIMD-API/c_api/vector_compute/vector_convert/asc_int642int32.md)和[asc_sync](../SIMD-API/c_api/sync/asc_sync.md)接口替代。

- [asc_int82half_sync（废弃）](../SIMD-API/c_api/deprecated_interface/asc_int82half_sync_deprecated.md)接口

    `asc_int82half_sync()`已废弃，请使用[对应的非同步接口](../SIMD-API/c_api/vector_compute/vector_convert/asc_int82half.md)和[asc_sync](../SIMD-API/c_api/sync/asc_sync.md)接口替代。

- [asc_uint82half_sync（废弃）](../SIMD-API/c_api/deprecated_interface/asc_uint82half_sync_deprecated.md)接口

    `asc_uint82half_sync()`已废弃，请使用[对应的非同步接口](../SIMD-API/c_api/vector_compute/vector_convert/asc_uint82half.md)和[asc_sync](../SIMD-API/c_api/sync/asc_sync.md)接口替代。

#### 复合计算

- [asc_add_relu_sync（废弃）](../SIMD-API/c_api/deprecated_interface/asc_add_relu_sync_deprecated.md)接口

    `asc_add_relu_sync()`已废弃，请使用[对应的非同步接口](../SIMD-API/c_api/vector_compute/vector_fused/asc_add_relu.md)和[asc_sync](../SIMD-API/c_api/sync/asc_sync.md)接口替代。

- [asc_axpy_sync（废弃）](../SIMD-API/c_api/deprecated_interface/asc_axpy_sync_deprecated.md)接口

    `asc_axpy_sync()`已废弃，请使用[对应的非同步接口](../SIMD-API/c_api/vector_compute/vector_fused/asc_axpy.md)和[asc_sync](../SIMD-API/c_api/sync/asc_sync.md)接口替代。

- [asc_deq_int162b8_sync（废弃）](../SIMD-API/c_api/deprecated_interface/asc_deq_int162b8_sync_deprecated.md)接口

    `asc_deq_int162b8_h_sync()`、`asc_deq_int162b8_l_sync()`已废弃，请使用[对应的非同步接口](../SIMD-API/c_api/vector_compute/vector_fused/asc_deq_int162b8.md)和[asc_sync](../SIMD-API/c_api/sync/asc_sync.md)接口替代。

- [asc_deq_int322half_sync（废弃）](../SIMD-API/c_api/deprecated_interface/asc_deq_int322half_sync_deprecated.md)接口

    `asc_deq_int322half_sync()`已废弃，请使用[对应的非同步接口](../SIMD-API/c_api/vector_compute/vector_fused/asc_deq_int322half.md)和[asc_sync](../SIMD-API/c_api/sync/asc_sync.md)接口替代。

- [asc_fma_sync（废弃）](../SIMD-API/c_api/deprecated_interface/asc_fma_sync_deprecated.md)接口

    `asc_fma_sync()`已废弃，请使用[对应的非同步接口](../SIMD-API/c_api/vector_compute/vector_fused/asc_fma.md)和[asc_sync](../SIMD-API/c_api/sync/asc_sync.md)接口替代。

- [asc_leakyrelu_sync（废弃）](../SIMD-API/c_api/deprecated_interface/asc_leakyrelu_sync_deprecated.md)接口

    `asc_leakyrelu_sync()`已废弃，请使用[对应的非同步接口](../SIMD-API/c_api/vector_compute/vector_fused/asc_leakyrelu.md)和[asc_sync](../SIMD-API/c_api/sync/asc_sync.md)接口替代。

- [asc_mul_add_relu_sync（废弃）](../SIMD-API/c_api/deprecated_interface/asc_mul_add_relu_sync_deprecated.md)接口

    `asc_mul_add_relu_sync()`已废弃，请使用[对应的非同步接口](../SIMD-API/c_api/vector_compute/vector_fused/asc_mul_add_relu.md)和[asc_sync](../SIMD-API/c_api/sync/asc_sync.md)接口替代。

- [asc_mul_add_sync（废弃）](../SIMD-API/c_api/deprecated_interface/asc_mul_add_sync_deprecated.md)接口

    `asc_mul_add_sync()`已废弃，请使用[对应的非同步接口](../SIMD-API/c_api/vector_compute/vector_fused/asc_mul_add.md)和[asc_sync](../SIMD-API/c_api/sync/asc_sync.md)接口替代。

- [asc_mul_cast_half2int8_sync（废弃）](../SIMD-API/c_api/deprecated_interface/asc_mul_cast_half2int8_sync_deprecated.md)接口

    `asc_mul_cast_half2int8_sync()`已废弃，请使用[对应的非同步接口](../SIMD-API/c_api/vector_compute/vector_fused/asc_mul_cast_half2int8.md)和[asc_sync](../SIMD-API/c_api/sync/asc_sync.md)接口替代。

- [asc_mul_cast_half2uint8_sync（废弃）](../SIMD-API/c_api/deprecated_interface/asc_mul_cast_half2uint8_sync_deprecated.md)接口

    `asc_mul_cast_half2uint8_sync()`已废弃，请使用[对应的非同步接口](../SIMD-API/c_api/vector_compute/vector_fused/asc_mul_cast_half2uint8.md)和[asc_sync](../SIMD-API/c_api/sync/asc_sync.md)接口替代。

- [asc_relu_sync（废弃）](../SIMD-API/c_api/deprecated_interface/asc_relu_sync_deprecated.md)接口

    `asc_relu_sync()`已废弃，请使用[对应的非同步接口](../SIMD-API/c_api/vector_compute/vector_fused/asc_relu.md)和[asc_sync](../SIMD-API/c_api/sync/asc_sync.md)接口替代。

- [asc_sub_relu_sync（废弃）](../SIMD-API/c_api/deprecated_interface/asc_sub_relu_sync_deprecated.md)接口

    `asc_sub_relu_sync()`已废弃，请使用[对应的非同步接口](../SIMD-API/c_api/vector_compute/vector_fused/asc_sub_relu.md)和[asc_sync](../SIMD-API/c_api/sync/asc_sync.md)接口替代。

- [asc_vdeq_int162b8_sync（废弃）](../SIMD-API/c_api/deprecated_interface/asc_vdeq_int162b8_sync_deprecated.md)接口

    `asc_vdeq_int162b8_h_sync()`、`asc_vdeq_int162b8_l_sync()`已废弃，请使用[对应的非同步接口](../SIMD-API/c_api/vector_compute/vector_fused/asc_vdeq_int162b8.md)和[asc_sync](../SIMD-API/c_api/sync/asc_sync.md)接口替代。

#### 聚合操作

- [asc_gather_datablock_sync（废弃）](../SIMD-API/c_api/deprecated_interface/asc_gather_datablock_sync_deprecated.md)接口

    `asc_gather_datablock_sync()`已废弃，请使用[对应的非同步接口](../SIMD-API/c_api/vector_compute/vector_gather/asc_gather_datablock.md)和[asc_sync](../SIMD-API/c_api/sync/asc_sync.md)接口替代。

- [asc_gather_sync（废弃）](../SIMD-API/c_api/deprecated_interface/asc_gather_sync_deprecated.md)接口

    `asc_gather_sync()`已废弃，请使用[对应的非同步接口](../SIMD-API/c_api/vector_compute/vector_gather/asc_gather.md)和[asc_sync](../SIMD-API/c_api/sync/asc_sync.md)接口替代。

#### 逻辑计算

- [asc_and_sync（废弃）](../SIMD-API/c_api/deprecated_interface/asc_and_sync_deprecated.md)接口

    `asc_and_sync()`已废弃，请使用[对应的非同步接口](../SIMD-API/c_api/vector_compute/vector_logic/asc_and.md)和[asc_sync](../SIMD-API/c_api/sync/asc_sync.md)接口替代。

- [asc_not_sync（废弃）](../SIMD-API/c_api/deprecated_interface/asc_not_sync_deprecated.md)接口

    `asc_not_sync()`已废弃，请使用[对应的非同步接口](../SIMD-API/c_api/vector_compute/vector_logic/asc_not.md)和[asc_sync](../SIMD-API/c_api/sync/asc_sync.md)接口替代。

- [asc_or_sync（废弃）](../SIMD-API/c_api/deprecated_interface/asc_or_sync_deprecated.md)接口

    `asc_or_sync()`已废弃，请使用[对应的非同步接口](../SIMD-API/c_api/vector_compute/vector_logic/asc_or.md)和[asc_sync](../SIMD-API/c_api/sync/asc_sync.md)接口替代。

- [asc_shiftleft_sync（废弃）](../SIMD-API/c_api/deprecated_interface/asc_shiftleft_sync_deprecated.md)接口

    `asc_shiftleft_sync()`已废弃，请使用[对应的非同步接口](../SIMD-API/c_api/vector_compute/vector_logic/asc_shiftleft.md)和[asc_sync](../SIMD-API/c_api/sync/asc_sync.md)接口替代。

- [asc_shiftright_sync（废弃）](../SIMD-API/c_api/deprecated_interface/asc_shiftright_sync_deprecated.md)接口

    `asc_shiftright_sync()`、`asc_shiftright_round_sync()`已废弃，请使用[对应的非同步接口](../SIMD-API/c_api/vector_compute/vector_logic/asc_shiftright.md)和[asc_sync](../SIMD-API/c_api/sync/asc_sync.md)接口替代。

#### 排布变换

- [asc_transto5hd_sync（废弃）](../SIMD-API/c_api/deprecated_interface/asc_transto5hd_sync_deprecated.md)接口

    此接口已废弃。请使用[asc_transto5hd](../SIMD-API/c_api/vector_compute/vector_permute_sel/asc_transto5hd.md)接口和[asc_sync](../SIMD-API/c_api/sync/asc_sync.md)接口。

- [asc_select_sync（废弃）](../SIMD-API/c_api/deprecated_interface/asc_select_sync_deprecated.md)接口

    `asc_select_sync()`已废弃，请使用[对应的非同步接口](../SIMD-API/c_api/vector_compute/vector_permute_sel/asc_select.md)和[asc_sync](../SIMD-API/c_api/sync/asc_sync.md)接口替代。

- [asc_transpose_sync（废弃）](../SIMD-API/c_api/deprecated_interface/asc_transpose_sync_deprecated.md)接口

    `asc_transpose_sync()`已废弃，请使用[对应的非同步接口](../SIMD-API/c_api/vector_compute/vector_permute_sel/asc_transpose.md)和[asc_sync](../SIMD-API/c_api/sync/asc_sync.md)接口替代。

#### 归约计算

- [asc_datablock_reduce_max_sync（废弃）](../SIMD-API/c_api/deprecated_interface/asc_datablock_reduce_max_sync_deprecated.md)接口

    `asc_datablock_reduce_max_sync()`已废弃，请使用[对应的非同步接口](../SIMD-API/c_api/vector_compute/vector_reduce/asc_datablock_reduce_max.md)和[asc_sync](../SIMD-API/c_api/sync/asc_sync.md)接口替代。

- [asc_datablock_reduce_min_sync（废弃）](../SIMD-API/c_api/deprecated_interface/asc_datablock_reduce_min_sync_deprecated.md)接口

    `asc_datablock_reduce_min_sync()`已废弃，请使用[对应的非同步接口](../SIMD-API/c_api/vector_compute/vector_reduce/asc_datablock_reduce_min.md)和[asc_sync](../SIMD-API/c_api/sync/asc_sync.md)接口替代。

- [asc_datablock_reduce_sum_sync（废弃）](../SIMD-API/c_api/deprecated_interface/asc_datablock_reduce_sum_sync_deprecated.md)接口

    `asc_datablock_reduce_sum_sync()`已废弃，请使用[对应的非同步接口](../SIMD-API/c_api/vector_compute/vector_reduce/asc_datablock_reduce_sum.md)和[asc_sync](../SIMD-API/c_api/sync/asc_sync.md)接口替代。

- [asc_pair_reduce_sum_sync（废弃）](../SIMD-API/c_api/deprecated_interface/asc_pair_reduce_sum_sync_deprecated.md)接口

    `asc_pair_reduce_sum_sync()`已废弃，请使用[对应的非同步接口](../SIMD-API/c_api/vector_compute/vector_reduce/asc_pair_reduce_sum.md)和[asc_sync](../SIMD-API/c_api/sync/asc_sync.md)接口替代。

- [asc_reduce_sync（废弃）](../SIMD-API/c_api/deprecated_interface/asc_reduce_sync_deprecated.md)接口

    `asc_reduce_sync()`已废弃，请使用[对应的非同步接口](../SIMD-API/c_api/vector_compute/vector_reduce/asc_reduce.md)和[asc_sync](../SIMD-API/c_api/sync/asc_sync.md)接口替代。

- [asc_repeat_reduce_max_sync（废弃）](../SIMD-API/c_api/deprecated_interface/asc_repeat_reduce_max_sync_deprecated.md)接口

    `asc_repeat_reduce_max_index_value_sync()`、`asc_repeat_reduce_max_value_index_sync()`、`asc_repeat_reduce_max_only_value_sync()`、`asc_repeat_reduce_max_only_index_sync()`已废弃，请使用[对应的非同步接口](../SIMD-API/c_api/vector_compute/vector_reduce/asc_repeat_reduce_max.md)和[asc_sync](../SIMD-API/c_api/sync/asc_sync.md)接口替代。

- [asc_repeat_reduce_min_sync（废弃）](../SIMD-API/c_api/deprecated_interface/asc_repeat_reduce_min_sync_deprecated.md)接口

    `asc_repeat_reduce_min_index_value_sync()`、`asc_repeat_reduce_min_value_index_sync()`、`asc_repeat_reduce_min_only_value_sync()`、`asc_repeat_reduce_min_only_index_sync()`已废弃，请使用[对应的非同步接口](../SIMD-API/c_api/vector_compute/vector_reduce/asc_repeat_reduce_min.md)和[asc_sync](../SIMD-API/c_api/sync/asc_sync.md)接口替代。

- [asc_repeat_reduce_sum_sync（废弃）](../SIMD-API/c_api/deprecated_interface/asc_repeat_reduce_sum_sync_deprecated.md)接口

    `asc_repeat_reduce_sum_sync()`已废弃，请使用[对应的非同步接口](../SIMD-API/c_api/vector_compute/vector_reduce/asc_repeat_reduce_sum.md)和[asc_sync](../SIMD-API/c_api/sync/asc_sync.md)接口替代。

#### 排序计算

- [asc_bitsort_sync（废弃）](../SIMD-API/c_api/deprecated_interface/asc_bitsort_sync_deprecated.md)接口

    `asc_bitsort_sync()`已废弃，请使用[对应的非同步接口](../SIMD-API/c_api/vector_compute/vector_sort/asc_bitsort.md)和[asc_sync](../SIMD-API/c_api/sync/asc_sync.md)接口替代。

- [asc_mrgsort4_sync（废弃）](../SIMD-API/c_api/deprecated_interface/asc_mrgsort4_sync_deprecated.md)接口

    `asc_mrgsort4_sync()`已废弃，请使用[对应的非同步接口](../SIMD-API/c_api/vector_compute/vector_sort/asc_mrgsort4.md)和[asc_sync](../SIMD-API/c_api/sync/asc_sync.md)接口替代。
