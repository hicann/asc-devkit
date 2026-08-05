# asc_unpack

## 产品支持情况

<!-- npu="950" id1 -->
- Ascend 950PR/Ascend 950DT：支持
<!-- end id1 -->
<!-- npu="A3" id2 -->
- Atlas A3 训练系列产品/Atlas A3 推理系列产品：不支持
<!-- end id2 -->
<!-- npu="910b" id3 -->
- Atlas A2 训练系列产品/Atlas A2 推理系列产品：不支持
<!-- end id3 -->
<!-- npu="310b" id4 -->
- Atlas 200I/500 A2 推理产品：不支持
<!-- end id4 -->
<!-- npu="310p" id5 -->
- Atlas 推理系列产品AI Core：不支持
<!-- end id5 -->
<!-- npu="310p" id6 -->
- Atlas 推理系列产品Vector Core：不支持
<!-- end id6 -->
<!-- npu="910" id7 -->
- Atlas 训练系列产品：不支持
<!-- end id7 -->

## 功能说明

将src中低半部分或高半部分的元素以扩充位宽的方式写入dst，支持：

- asc_unpack_lower：取src的低半部分（LOWER），每个元素高位填0扩充位宽后写入dst。
- asc_unpack_upper：取src的高半部分（HIGHER），每个元素高位填0扩充位宽后写入dst。

不同数据类型的扩充方式：

- 无符号整型：高位填0扩充。例如vector_uint8_t到vector_uint16_t，src中每个uint8_t元素高位补0扩展为uint16_t。
- 有符号整型：保持符号位扩充。例如vector_int8_t到vector_int16_t，src中每个int8_t元素按符号位扩展为int16_t。
- bool类型（掩码寄存器）：将每bit展开为2bit，高位填0。

矢量数据寄存器unpack流程如图1所示：

**图1** 矢量数据寄存器unpack流程

![矢量数据寄存器unpack流程](../../figures/capi_unpack_reg.png)

掩码数据寄存器unpack流程如图2所示：

**图2** 掩码数据寄存器unpack流程

![掩码数据寄存器unpack流程](../../figures/capi_unpack_mask.png)

## 函数原型

- 操作数为矢量数据寄存器

    ```cpp
    // 解压缩低位
    __simd_callee__ inline void asc_unpack_lower(vector_uint16_t& dst, vector_uint8_t src)
    __simd_callee__ inline void asc_unpack_lower(vector_int16_t& dst, vector_int8_t src)
    __simd_callee__ inline void asc_unpack_lower(vector_uint32_t& dst, vector_uint16_t src)
    __simd_callee__ inline void asc_unpack_lower(vector_int32_t& dst, vector_int16_t src)
    // 解压缩高位
    __simd_callee__ inline void asc_unpack_upper(vector_uint16_t& dst, vector_uint8_t src)
    __simd_callee__ inline void asc_unpack_upper(vector_int16_t& dst, vector_int8_t src)
    __simd_callee__ inline void asc_unpack_upper(vector_uint32_t& dst, vector_uint16_t src)
    __simd_callee__ inline void asc_unpack_upper(vector_int32_t& dst, vector_int16_t src)
    ```

- 操作数为掩码寄存器

    ```cpp
    // 解压缩低位
    __simd_callee__ inline void asc_unpack_lower(vector_bool& dst, vector_bool src)
    // 解压缩高位
    __simd_callee__ inline void asc_unpack_upper(vector_bool& dst, vector_bool src)
    ```

## 参数说明

**表1** 参数说明（操作数为矢量数据寄存器）

| 参数名 | 输入/输出 | 描述 |
| --- | --- | --- |
| dst | 输出 | 目的操作数（矢量数据寄存器）。 |
| src | 输入 | 源操作数（矢量数据寄存器）。 |

**表2** 参数说明（操作数为掩码寄存器）

| 参数名 | 输入/输出 | 描述 |
| --- | --- | --- |
| dst | 输出 | 目的操作数（掩码寄存器）。 |
| src | 输入 | 源操作数（掩码寄存器）。 |

矢量数据寄存器和掩码寄存器的详细说明请参见[reg数据类型定义](../reg_data_types/data_type_definition.md)。

## 返回值说明

无

## 约束说明

无

## 调用示例

- 对矢量数据寄存器操作

    ```cpp
    __simd_vf__ inline void unpack_vf(__ubuf__ uint32_t* dst_addr, __ubuf__ uint16_t* src_addr, uint16_t one_src_repeat_size, uint16_t one_dst_repeat_size, uint16_t repeat_time)
    {
        vector_uint32_t dst;
        vector_uint16_t src;
        vector_bool mask = asc_create_mask_b32(PAT_ALL);
        for (uint16_t i = 0; i < repeat_time; ++i) {
            asc_loadalign(src, src_addr + i * one_src_repeat_size);
            asc_unpack_lower(dst, src);
            asc_storealign(dst_addr + i * one_dst_repeat_size, dst, mask);
        }
    }
    ```

- 对掩码寄存器操作

    ```cpp
    __simd_vf__ inline void unpack_vf(__ubuf__ uint8_t* dst_addr, __ubuf__ uint8_t* src_addr, uint32_t count, uint32_t one_repeat_size, uint16_t repeat_time)
    {
        vector_uint8_t src;
        vector_bool mask0 = asc_create_mask_b8(PAT_ALL);
        vector_bool mask1;
        asc_unpack_lower(mask1, mask0);
        for (uint16_t i = 0; i < repeat_time; ++i) {
            asc_loadalign(src, src_addr + i * one_repeat_size);
            asc_add_scalar(src, src, 0, mask1);
            asc_storealign(dst_addr + i * one_repeat_size, src, mask1);
        }
    }
    ```
