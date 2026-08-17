# asc_axpy

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

根据`mask`将源操作数`src`中的每个元素与标量`value`求积后，与目的操作数`dst`中的对应元素相加，将结果写入`dst`。其中，`dst`既作为输入累加器参与计算，又用于存储最终结果。计算公式如下：

$$
dst_i = src_i \times value + dst_i
$$

## 函数原型

```cpp
__simd_callee__ inline void asc_axpy(vector_half& dst, vector_half src, half value, vector_bool mask)
__simd_callee__ inline void asc_axpy(vector_float& dst, vector_float src, float value, vector_bool mask)
```

## 参数说明

**表1** 参数说明
| 参数名  | 输入/输出 | 描述                                                                                                               |
| ------- | --------- | ------------------------------------------------------------------------------------------------------------------ |
| dst     | 输入/输出 | 目的操作数（矢量数据寄存器）。                                                                                     |
| src     | 输入      | 源操作数（矢量数据寄存器）。                                                                                       |
| value   | 输入      | 源操作数（标量）。                                                                                                 |
| mask    | 输入      | 掩码寄存器，用于控制各元素是否参与计算。`mask`中与元素对应的比特位为1时，该元素参与计算；为0时，该元素不参与计算。 |

矢量数据寄存器和掩码寄存器的详细说明请参见[reg数据类型定义](../reg_data_types/data_type_definition.md)。

## 返回值说明

无

## 约束说明

`mask`未筛选的元素在输出中置零。

## 调用示例

```cpp
__simd_vf__ inline void axpy_vf(__ubuf__ half* src0_addr, __ubuf__ half* dst_addr, half scalar, uint32_t count, uint16_t one_repeat_size, uint16_t repeat_time)
{
    vector_half src0, dst;
    vector_bool mask;
    for (uint16_t i = 0; i < repeat_time; ++i) {
        mask = asc_update_mask_b16(count);
        asc_loadalign_postupdate(src0, src0_addr, one_repeat_size);
        asc_loadalign(dst, dst_addr);
        asc_axpy(dst, src0, scalar, mask);
        asc_storealign_postupdate(dst_addr, dst, one_repeat_size, mask);
    }
}
```
