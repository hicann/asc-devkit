# asc_reduce_max

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

根据mask对源操作数src进行归约最大值操作：将src中的最大值写入目的操作数dst的0位置，最大值在src中的索引写入目的操作数dst的1位置，dst的其他位置将被置为0。

对src中mask为1的元素求最大值，结果广播写入dst的所有有效元素；mask为0的位置在输出中置零。

$$
\text{result} = \max\{src_i \mid mask_i = 1\}
$$

## 函数原型

```cpp
__simd_callee__ inline void asc_reduce_max(vector_uint16_t& dst, vector_uint16_t src, vector_bool mask)
__simd_callee__ inline void asc_reduce_max(vector_int16_t& dst, vector_int16_t src, vector_bool mask)
__simd_callee__ inline void asc_reduce_max(vector_half& dst, vector_half src, vector_bool mask)
__simd_callee__ inline void asc_reduce_max(vector_uint32_t& dst, vector_uint32_t src, vector_bool mask)
__simd_callee__ inline void asc_reduce_max(vector_int32_t& dst, vector_int32_t src, vector_bool mask)
__simd_callee__ inline void asc_reduce_max(vector_float& dst, vector_float src, vector_bool mask)
```

## 参数说明

**表1** 参数说明
| 参数名  | 输入/输出 | 描述 |
| :----- | :------- | :------- |
| dst | 输出 | 目的操作数（矢量数据寄存器）。 |
| src | 输入 | 源操作数（矢量数据寄存器）。 |
| mask | 输入 | 源操作数掩码（掩码寄存器），用于指示在计算过程中哪些元素参与计算。对应位置为1时参与计算，为0时不参与计算。 |

矢量数据寄存器和掩码寄存器的详细说明请参见[reg数据类型定义](../reg_data_types/data_type_definition.md)。

## 返回值说明

无

## 约束说明

- 将最大值写入目的操作数的第一个数，将最大值的下标写入目的操作数的第二个数， 如果有多个最大值，第二个数为最小的下标，其余值都将被填充为0。
- 未被mask选中的元素被视为最小值（浮点数为-inf），如果一个DataBlock中所有元素都未被mask选中，-inf将被填充到目的操作数的第一个数，其余值都将被填充为0（包括下标）。
- 在比较中，遵循max(-0, +0) = +0。
- 如果输入数据存在nan，将nan写入目的操作数的第一个数，将第一个nan值的下标写入目的操作数的第二个数。

## 关键特性

**索引值需要强制类型转换**：

dst的索引按照dst的数据类型存储，比如dst为half类型时，索引按照half类型存储，因此读取索引需要使用  reinterpret\_cast方法转换到整数类型。若数据类型是half，需要使用reinterpret\_cast\<uint16_t\*\>；若数据类型是float，需要使用reinterpret\_cast\<uint32\_t\*\>。

**提取结果（值+索引）：**

归约产生两个有效结果，需要分别提取。

## 调用示例

```cpp
__simd_vf__ inline void reduce_max_vf(__ubuf__ half* src_addr, __ubuf__ half* dst_addr, uint32_t count, uint16_t one_repeat_size, uint16_t repeat_time)
{
    vector_half src, dst;
    vector_bool mask;
    for (uint16_t i = 0; i < repeat_time; ++i) {
        mask = asc_update_mask_b16(count);
        asc_loadalign_postupdate(src, src_addr, one_repeat_size);
        asc_reduce_max(dst, src, mask);
        asc_storealign_postupdate(dst_addr, dst, one_repeat_size, mask);
    }
}
```
