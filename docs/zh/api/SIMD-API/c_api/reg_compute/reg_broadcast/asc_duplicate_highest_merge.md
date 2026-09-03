# asc_duplicate_highest_merge

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
<!-- npu="910" id4 -->
- Atlas 训练系列产品：不支持
<!-- end id4 -->
<!-- npu="310p" id5 -->
- Atlas 推理系列产品AI Core：不支持
<!-- end id5 -->
<!-- npu="310p" id6 -->
- Atlas 推理系列产品Vector Core：不支持
<!-- end id6 -->
<!-- npu="310b" id7 -->
- Atlas 200I/500 A2 推理产品：不支持
<!-- end id7 -->

## 功能说明

头文件路径为：`"c_api/reg_compute/compute/reg_broadcast.h"`。

merge模式下，将src的最高位元素广播到dst中被mask筛选的位置，dst中未被mask筛选的元素保留原值。

## 函数原型

```cpp
__simd_callee__ inline void asc_duplicate_highest_merge(vector_int8_t& dst, vector_int8_t src, vector_bool mask)
__simd_callee__ inline void asc_duplicate_highest_merge(vector_uint8_t& dst, vector_uint8_t src, vector_bool mask)
__simd_callee__ inline void asc_duplicate_highest_merge(vector_fp8_e8m0_t& dst, vector_fp8_e8m0_t src, vector_bool mask)
__simd_callee__ inline void asc_duplicate_highest_merge(vector_fp8_e5m2_t& dst, vector_fp8_e5m2_t src, vector_bool mask)
__simd_callee__ inline void asc_duplicate_highest_merge(vector_fp8_e4m3fn_t& dst, vector_fp8_e4m3fn_t src, vector_bool mask)
__simd_callee__ inline void asc_duplicate_highest_merge(vector_int16_t& dst, vector_int16_t src, vector_bool mask)
__simd_callee__ inline void asc_duplicate_highest_merge(vector_uint16_t& dst, vector_uint16_t src, vector_bool mask)
__simd_callee__ inline void asc_duplicate_highest_merge(vector_half& dst, vector_half src, vector_bool mask)
__simd_callee__ inline void asc_duplicate_highest_merge(vector_bfloat16_t& dst, vector_bfloat16_t src, vector_bool mask)
__simd_callee__ inline void asc_duplicate_highest_merge(vector_int32_t& dst, vector_int32_t src, vector_bool mask)
__simd_callee__ inline void asc_duplicate_highest_merge(vector_uint32_t& dst, vector_uint32_t src, vector_bool mask)
__simd_callee__ inline void asc_duplicate_highest_merge(vector_float& dst, vector_float src, vector_bool mask)
```

## 参数说明

**表1** 参数说明

| 参数名 | 输入/输出 | 描述 |
| --- | --- | --- |
| dst | 输入/输出 | 目的操作数（矢量数据寄存器）。未被mask筛选的元素保留原值。 |
| src | 输入 | 源操作数（矢量数据寄存器）。 |
| mask | 输入 | 源操作数掩码（掩码寄存器），用于指示在计算过程中哪些元素参与计算。对应位置为1时参与计算，为0时不参与计算。 |

矢量数据寄存器和掩码寄存器的详细说明请参见[reg数据类型定义](../../defs/type/data_type_definition.md)。

## 返回值说明

无

## 约束说明

无

## 调用示例

```cpp
__simd_vf__ inline void duplicate_highest_merge_vf(__ubuf__ half* dst_addr, __ubuf__ half* src_addr,
    uint32_t count, uint16_t one_repeat_size, uint16_t repeat_time)
{
    vector_half dst;
    vector_half src;
    vector_bool mask;
    for (uint16_t i = 0; i < repeat_time; ++i) {
        mask = asc_update_mask_b16(count);
        asc_loadalign(dst, dst_addr + i * one_repeat_size);
        asc_loadalign(src, src_addr + i * one_repeat_size);
        asc_duplicate_highest_merge(dst, src, mask);
        asc_storealign(dst_addr + i * one_repeat_size, dst, mask);
    }
}
```
