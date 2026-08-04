# asc_reduce_max_datablock

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

根据mask将每个DataBlock(32B)中的最大值依次保存在dst中的最低位。

## 函数原型

```cpp
__simd_callee__ inline void asc_reduce_max_datablock(vector_half& dst, vector_half src, vector_bool mask)
__simd_callee__ inline void asc_reduce_max_datablock(vector_float& dst, vector_float src, vector_bool mask)
__simd_callee__ inline void asc_reduce_max_datablock(vector_uint16_t& dst, vector_uint16_t src, vector_bool mask)
__simd_callee__ inline void asc_reduce_max_datablock(vector_int16_t& dst, vector_int16_t src, vector_bool mask)
__simd_callee__ inline void asc_reduce_max_datablock(vector_uint32_t& dst, vector_uint32_t src, vector_bool mask)
__simd_callee__ inline void asc_reduce_max_datablock(vector_int32_t& dst, vector_int32_t src, vector_bool mask)
```

## 参数说明

**表1** 参数说明
| 参数名  | 输入/输出 | 描述                             |
|:-----| :--- |:-------------------------------|
| dst | 输出 | 目的操作数（矢量数据寄存器）。 |
| src | 输入 | 源操作数（矢量数据寄存器）。 |
| mask | 输入 | 源操作数掩码（掩码寄存器），用于指示在计算过程中哪些元素参与计算。对应位置为1时参与计算，为0时不参与计算。 |

矢量数据寄存器和掩码寄存器的详细说明请参见[reg数据类型定义](../reg_data_types/data_type_definition.md)。

## 返回值说明

无

## 约束说明

- 在比较中，遵循max(-0, +0) = +0。
- 找到源操作数中每个DataBlock内的最大值，连续写入到目的操作数，目的操作数中的其它元素置0。
- 未被mask选中的元素被视为最小值（浮点数为-inf），如果一个DataBlock中所有元素都未被mask选中，-inf将被填充到目的操作数的指定位置。
- 仅输出最值，不输出索引下标。

## 调用示例

```cpp
__simd_vf__ inline void reduce_max_datablock_vf(__ubuf__ half* src_addr, __ubuf__ half* dst_addr, uint32_t count, uint16_t one_repeat_size, uint16_t repeat_time)
{
    vector_half src, dst;
    vector_bool mask;
    for (uint16_t i = 0; i < repeat_time; ++i) {
        mask = asc_update_mask_b16(count);
        asc_loadalign_postupdate(src, src_addr, one_repeat_size);
        asc_reduce_max_datablock(dst, src, mask);
        asc_storealign_postupdate(dst_addr, dst, one_repeat_size, mask);
    }
}
```
