# asc_reduce_min

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

根据`mask`对源操作数`src`进行归约最小值操作，将最小值写入目的操作数`dst`的第0个元素，将最小值在`src`中的索引原始位模式写入`dst`的第1个元素，并将`dst`中的其他元素置0。如果存在多个最小值，则写入最小的索引。计算公式如下：

$$
dst_0 = \min\{src_i \mid mask_i = 1\} \\
dst_1 = \argmin\{src_i \mid mask_i = 1\}
$$

## 函数原型

```cpp
__simd_callee__ inline void asc_reduce_min(vector_uint16_t& dst, vector_uint16_t src, vector_bool mask)
__simd_callee__ inline void asc_reduce_min(vector_int16_t& dst, vector_int16_t src, vector_bool mask)
__simd_callee__ inline void asc_reduce_min(vector_half& dst, vector_half src, vector_bool mask)
__simd_callee__ inline void asc_reduce_min(vector_uint32_t& dst, vector_uint32_t src, vector_bool mask)
__simd_callee__ inline void asc_reduce_min(vector_int32_t& dst, vector_int32_t src, vector_bool mask)
__simd_callee__ inline void asc_reduce_min(vector_float& dst, vector_float src, vector_bool mask)
```

## 参数说明

**表1** 参数说明
| 参数名  | 输入/输出 | 描述 |
| :----- | :------- | :------- |
| dst | 输出 | 目的操作数（矢量数据寄存器）。 |
| src | 输入 | 源操作数（矢量数据寄存器）。 |
| mask | 输入 | 源操作数掩码（掩码寄存器），用于指示在计算过程中哪些元素参与计算。对应位置为1时参与计算，为0时不参与计算。 |

矢量数据寄存器和掩码寄存器的详细说明请参见[reg数据类型定义](../../defs/type/data_type_definition.md)。

## 返回值说明

无

## 约束说明

- 未被`mask`选中的元素被视为对应数据类型的最大值，浮点数类型的最大值为`+inf`。如果`src`中的所有元素均未被`mask`选中，则将该最大值写入`dst`的第0个元素，并将其余元素置0。
- 比较时遵循$min(-0, +0) = -0$。
- 如果输入数据中存在nan，则将nan写入`dst`的第0个元素，并将第一个nan的索引写入`dst`的第1个元素。

## 关键特性

**索引值需要强制类型转换**：

`dst`的索引按照`dst`的数据类型存储，比如`dst`为half类型时，索引按照half类型存储，因此读取索引需要使用  reinterpret\_cast方法转换到整数类型。若数据类型是half，需要使用reinterpret\_cast\<uint16_t\*\>；若数据类型是float，需要使用reinterpret\_cast\<uint32\_t\*\>。

**提取结果（值+索引）：**

归约产生两个有效结果，需要分别提取。

## 调用示例

```cpp
__simd_vf__ inline void reduce_min_vf(__ubuf__ half* src_addr, __ubuf__ half* dst_addr, uint32_t count, uint16_t one_repeat_size, uint16_t repeat_time)
{
    vector_half src, dst;
    vector_bool mask;
    for (uint16_t i = 0; i < repeat_time; ++i) {
        mask = asc_update_mask_b16(count);
        asc_loadalign_postupdate(src, src_addr, one_repeat_size);
        asc_reduce_min(dst, src, mask);
        asc_storealign_postupdate(dst_addr, dst, one_repeat_size, mask);
    }
}
```
