# asc_gather_datablock

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

给定源操作数在Unified Buffer（UB）中的基地址和索引，根据索引位置将源操作数按DataBlock收集到目的操作数中。每个DataBlock长度为32B。

![](../../figures/asc_gather_datablock.png)

其中，index中仅前8个元素有效，每个元素对应一个DataBlock。例如，第一个元素为96（3 * 32），表示选取DataBlock3写入dstReg中对应的位置。

## 函数原型

- 支持掩码控制，允许选择性收集数据

    ```cpp
    __simd_callee__ inline void asc_gather_datablock(vector_int4x2_t& dst, __ubuf__ int4b_t* src, vector_uint32_t index, vector_bool mask)
    __simd_callee__ inline void asc_gather_datablock(vector_int8_t& dst, __ubuf__ int8_t* src, vector_uint32_t index, vector_bool mask)
    __simd_callee__ inline void asc_gather_datablock(vector_uint8_t& dst, __ubuf__ uint8_t* src, vector_uint32_t index, vector_bool mask)
    __simd_callee__ inline void asc_gather_datablock(vector_fp4x2_e2m1_t& dst, __ubuf__ fp4x2_e2m1_t* src, vector_uint32_t index, vector_bool mask)
    __simd_callee__ inline void asc_gather_datablock(vector_fp4x2_e1m2_t& dst, __ubuf__ fp4x2_e1m2_t* src, vector_uint32_t index, vector_bool mask)
    __simd_callee__ inline void asc_gather_datablock(vector_hifloat8_t& dst, __ubuf__ hifloat8_t* src, vector_uint32_t index, vector_bool mask)
    __simd_callee__ inline void asc_gather_datablock(vector_fp8_e8m0_t& dst, __ubuf__ fp8_e8m0_t* src, vector_uint32_t index, vector_bool mask)
    __simd_callee__ inline void asc_gather_datablock(vector_fp8_e5m2_t& dst, __ubuf__ fp8_e5m2_t* src, vector_uint32_t index, vector_bool mask)
    __simd_callee__ inline void asc_gather_datablock(vector_fp8_e4m3fn_t& dst, __ubuf__ fp8_e4m3fn_t* src, vector_uint32_t index, vector_bool mask)
    __simd_callee__ inline void asc_gather_datablock(vector_int16_t& dst, __ubuf__ int16_t* src, vector_uint32_t index, vector_bool mask)
    __simd_callee__ inline void asc_gather_datablock(vector_uint16_t& dst, __ubuf__ uint16_t* src, vector_uint32_t index, vector_bool mask)
    __simd_callee__ inline void asc_gather_datablock(vector_half& dst, __ubuf__ half* src, vector_uint32_t index, vector_bool mask)
    __simd_callee__ inline void asc_gather_datablock(vector_bfloat16_t& dst, __ubuf__ bfloat16_t* src, vector_uint32_t index, vector_bool mask)
    __simd_callee__ inline void asc_gather_datablock(vector_int32_t& dst, __ubuf__ int32_t* src, vector_uint32_t index, vector_bool mask)
    __simd_callee__ inline void asc_gather_datablock(vector_uint32_t& dst, __ubuf__ uint32_t* src, vector_uint32_t index, vector_bool mask)
    __simd_callee__ inline void asc_gather_datablock(vector_float& dst, __ubuf__ float* src, vector_uint32_t index, vector_bool mask)
    __simd_callee__ inline void asc_gather_datablock(vector_int64_t& dst, __ubuf__ int64_t* src, vector_uint32_t index, vector_bool mask)
    __simd_callee__ inline void asc_gather_datablock(vector_uint64_t& dst, __ubuf__ uint64_t* src, vector_uint32_t index, vector_bool mask)
    ```
- 无掩码控制，默认收集所有索引对应的数据块
    ```cpp
    __simd_callee__ inline void asc_gather_datablock(vector_int4x2_t& dst, __ubuf__ int4b_t* src, vector_uint32_t index)
    __simd_callee__ inline void asc_gather_datablock(vector_int8_t& dst, __ubuf__ int8_t* src, vector_uint32_t index)
    __simd_callee__ inline void asc_gather_datablock(vector_uint8_t& dst, __ubuf__ uint8_t* src, vector_uint32_t index)
    __simd_callee__ inline void asc_gather_datablock(vector_fp4x2_e2m1_t& dst, __ubuf__ fp4x2_e2m1_t* src, vector_uint32_t index)
    __simd_callee__ inline void asc_gather_datablock(vector_fp4x2_e1m2_t& dst, __ubuf__ fp4x2_e1m2_t* src, vector_uint32_t index)
    __simd_callee__ inline void asc_gather_datablock(vector_hifloat8_t& dst, __ubuf__ hifloat8_t* src, vector_uint32_t index)
    __simd_callee__ inline void asc_gather_datablock(vector_fp8_e8m0_t& dst, __ubuf__ fp8_e8m0_t* src, vector_uint32_t index)
    __simd_callee__ inline void asc_gather_datablock(vector_fp8_e5m2_t& dst, __ubuf__ fp8_e5m2_t* src, vector_uint32_t index)
    __simd_callee__ inline void asc_gather_datablock(vector_fp8_e4m3fn_t& dst, __ubuf__ fp8_e4m3fn_t* src, vector_uint32_t index)
    __simd_callee__ inline void asc_gather_datablock(vector_int16_t& dst, __ubuf__ int16_t* src, vector_uint32_t index)
    __simd_callee__ inline void asc_gather_datablock(vector_uint16_t& dst, __ubuf__ uint16_t* src, vector_uint32_t index)
    __simd_callee__ inline void asc_gather_datablock(vector_half& dst, __ubuf__ half* src, vector_uint32_t index)
    __simd_callee__ inline void asc_gather_datablock(vector_bfloat16_t& dst, __ubuf__ bfloat16_t* src, vector_uint32_t index)
    __simd_callee__ inline void asc_gather_datablock(vector_int32_t& dst, __ubuf__ int32_t* src, vector_uint32_t index)
    __simd_callee__ inline void asc_gather_datablock(vector_uint32_t& dst, __ubuf__ uint32_t* src, vector_uint32_t index)
    __simd_callee__ inline void asc_gather_datablock(vector_float& dst, __ubuf__ float* src, vector_uint32_t index)
    __simd_callee__ inline void asc_gather_datablock(vector_int64_t& dst, __ubuf__ int64_t* src, vector_uint32_t index)
    __simd_callee__ inline void asc_gather_datablock(vector_uint64_t& dst, __ubuf__ uint64_t* src, vector_uint32_t index)
    ```

## 参数说明

**表1** 参数说明

| 参数名  | 输入/输出 | 描述 |
| :----- | :------- | :------- |
| dst | 输出 | 目的操作数（矢量数据寄存器）。 |
| src | 输入 | 源操作数（矢量）的起始地址。 |
| index | 输入 | 源操作数（矢量数据寄存器），表示dst中每个DataBlock在UB中相对于src的索引位置。索引位置要大于等于0且32B对齐，索引可以存在相同的值。**index仅前8个数有效，单位是字节。** 例如：<br>src: [DataBlock0, DataBlock1, DataBlock2, DataBlock3, DataBlock4, DataBlock5, DataBlock6, DataBlock7, ... , DataBlock32, ...]。<br>index: [0\*32, 1\*32, 2\*32, 3\*32, 4\*32, 5\*32, 6\*32, 32\*32]<br>dst: [DataBlock0, DataBlock1, DataBlock2, DataBlock3, DataBlock4, DataBlock5, DataBlock6, DataBlock32]。 |
| mask | 输入 | 源操作数掩码（掩码寄存器）。**DataBlock搬运的有效指示，按b32格式解释。一个DataBlock对应4bit，仅每4bit中的最低位有效。由于index仅前8个元素有效，因此mask仅使用前8个b32元素对应的bit 0、4、8、12、16、20、24、28，分别控制dstReg中DataBlock0至DataBlock7是否更新，其余bit无效。** |

矢量数据寄存器和掩码寄存器的详细说明请参见[reg数据类型定义](../reg_data_types/data_type_definition.md)。

## 返回值说明

无

## 约束说明

- 源操作数在UB中的起始地址需要32B对齐。
- 索引位置要大于等于0且32B对齐，即一个索引值对应一个DataBlock。
- 索引可以存在相同的值，即可以多次读取源操作数中同一个DataBlock的数据。
- 索引值对应的数据必须在UB有效地址范围内。
- index仅前8个元素有效。

## 调用示例

- 支持掩码控制，允许选择性收集数据
    ```cpp
    __simd_vf__ inline void gather_datablock_vf(__ubuf__ int8_t* dst_addr, __ubuf__ int8_t* src_addr, __ubuf__ uint32_t* index_addr, int32_t index_repeat_size, int32_t dst_repeat_size, uint16_t repeat_time)
    {
        vector_int8_t dst;
        vector_uint32_t index;
        vector_bool cal_mask = asc_create_mask_b32(PAT_VL4); // PAT_VL4表示仅前4个DataBlock更新。
        vector_bool store_mask = asc_create_mask_b8(PAT_H); // PAT_H表示仅前一半的元素搬出。
        for (uint16_t i = 0; i < repeat_time; ++i) {
            asc_loadalign_postupdate(index, index_addr, index_repeat_size);
            asc_gather_datablock(dst, src_addr, index, cal_mask);
            asc_storealign_postupdate(dst_addr, dst, dst_repeat_size, store_mask);
        }
    }
    ```
- 无掩码控制，默认收集所有索引对应的数据块
    ```cpp
    __simd_vf__ inline void gather_datablock_vf(__ubuf__ int8_t* dst_addr, __ubuf__ int8_t* src_addr, __ubuf__ uint32_t* index_addr, int32_t index_repeat_size, int32_t dst_repeat_size, uint16_t repeat_time)
    {
        vector_int8_t dst;
        vector_uint32_t index;
        vector_bool mask = asc_create_mask_b8(PAT_ALL);
        for (uint16_t i = 0; i < repeat_time; ++i) {
            asc_loadalign_postupdate(index, index_addr, index_repeat_size);
            asc_gather_datablock(dst, src_addr, index);
            asc_storealign_postupdate(dst_addr, dst, dst_repeat_size, mask);
        }
    }
    ```
