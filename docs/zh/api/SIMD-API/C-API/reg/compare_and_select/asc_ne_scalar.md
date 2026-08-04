# asc_ne_scalar

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

`src` 中的每个元素逐个与标量 `value` 比较是否不相等，如果 `src_i != value`，则输出结果 `dst` 对应比特位为1，否则为0，每个元素的比较结果占一个bit。`dst` 为掩码寄存器，按掩码 `mask` 指示参与计算的元素。

$$
dst_i = (src_i \neq value)
$$

## 函数原型

```cpp
__simd_callee__ inline void asc_ne_scalar(vector_bool& dst, vector_uint8_t src, uint8_t value, vector_bool mask)
__simd_callee__ inline void asc_ne_scalar(vector_bool& dst, vector_int8_t src, int8_t value, vector_bool mask)
__simd_callee__ inline void asc_ne_scalar(vector_bool& dst, vector_uint16_t src, uint16_t value, vector_bool mask)
__simd_callee__ inline void asc_ne_scalar(vector_bool& dst, vector_int16_t src, int16_t value, vector_bool mask)
__simd_callee__ inline void asc_ne_scalar(vector_bool& dst, vector_uint32_t src, uint32_t value, vector_bool mask)
__simd_callee__ inline void asc_ne_scalar(vector_bool& dst, vector_int32_t src, int32_t value, vector_bool mask)
__simd_callee__ inline void asc_ne_scalar(vector_bool& dst, vector_half src, half value, vector_bool mask)
__simd_callee__ inline void asc_ne_scalar(vector_bool& dst, vector_float src, float value, vector_bool mask)
__simd_callee__ inline void asc_ne_scalar(vector_bool& dst, vector_bfloat16_t src, bfloat16_t value, vector_bool mask)
```
## 参数说明

**表1** 参数说明
| 参数名  | 输入/输出 | 描述 |
| :----- | :------- | :------- |
| dst | 输出 | 目的操作数（掩码寄存器）。 |
| src | 输入 | 源操作数（矢量数据寄存器）。 |
| value | 输入 | 源操作数（标量）。 |
| mask | 输入 | 源操作数掩码（掩码寄存器），用于指示在计算过程中哪些元素参与计算。对应位置为1时参与计算，为0时不参与计算。mask未筛选的元素在输出中置零。 |

矢量数据寄存器和掩码寄存器的详细说明请参见[reg数据类型定义](../reg_data_types/data_type_definition.md)。

## 返回值说明

无

## 约束说明

- mask未筛选的元素在输出中置零。
- 浮点数据类型比较，涉及`nan`时比较结果为1。

## 调用示例

```cpp
__simd_vf__ inline void ne_scalar_vf(__ubuf__ uint8_t* dst_addr, __ubuf__ half* src_addr, half scalar, uint32_t count, uint16_t one_repeat_size, uint16_t one_block_size, uint16_t repeat_time)
{
    vector_half src;
    vector_bool dst;
    vector_bool mask;
    for (uint16_t i = 0; i < repeat_time; ++i) {
        mask = asc_update_mask_b16(count);
        asc_loadalign_postupdate(src, src_addr, one_repeat_size);
        asc_ne_scalar(dst, src, scalar, mask);
        asc_storealign_postupdate(dst_addr, dst, one_block_size);
    }
}
```
