# asc_div

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

该接口根据mask，对源操作数src0、src1进行按元素求商操作，将结果写入目的操作数dst。

计算公式如下：

$$
dst_i = src0_i \div src1_i
$$

## 函数原型

```cpp
__simd_callee__ inline void asc_div(vector_half& dst, vector_half src0, vector_half src1, vector_bool mask)
__simd_callee__ inline void asc_div(vector_float& dst, vector_float src0, vector_float src1, vector_bool mask)
__simd_callee__ inline void asc_div(vector_int16_t& dst, vector_int16_t src0, vector_int16_t src1, vector_bool mask)
__simd_callee__ inline void asc_div(vector_uint16_t& dst, vector_uint16_t src0, vector_uint16_t src1, vector_bool mask)
__simd_callee__ inline void asc_div(vector_int32_t& dst, vector_int32_t src0, vector_int32_t src1, vector_bool mask)
__simd_callee__ inline void asc_div(vector_uint32_t& dst, vector_uint32_t src0, vector_uint32_t src1, vector_bool mask)
```
## 参数说明

**表1** 参数说明

| 参数名 | 输入/输出 | 描述 |
| :--- | :--- | :--- |
| dst | 输出 | 目的操作数（矢量数据寄存器）。 |
| src0 | 输入 | 源操作数（矢量数据寄存器）。 |
| src1 | 输入 |源操作数（矢量数据寄存器）。 |
| mask | 输入 | 源操作数掩码（掩码寄存器）。用于指示在计算过程中哪些元素参与计算。对应位置为1时参与计算，为0时不参与计算。mask未筛选的元素在输出中置零。 |

矢量数据寄存器和掩码寄存器的详细说明请参见[reg数据类型定义](../reg_data_types/data_type_definition.md)。

## 返回值说明

无

## 约束说明

- mask控制源操作数是否参与计算，源操作数不参与计算的元素在输出对应位置置零。

- 注意除零错误，结果请参考[Div特殊值/边界值输入的计算结果说明](../../../../appendix/reg_vector_compute_interface_boundary_value_summary.md#table1325925915217)。

## 调用示例

```cpp
__simd_vf__ inline void div_vf(__ubuf__ half* dst_addr, __ubuf__ half* src0_addr, __ubuf__ half* src1_addr, uint32_t count, int32_t one_repeat_size, uint16_t repeat_time)
{
    vector_half src0;
    vector_half src1;
    vector_half dst;
    vector_bool mask;
    for (uint16_t i = 0; i < repeat_time; ++i) {
        mask = asc_update_mask_b16(count);
        asc_loadalign_postupdate(src0, src0_addr, one_repeat_size);
        asc_loadalign_postupdate(src1, src1_addr, one_repeat_size);
        asc_div(dst, src0, src1, mask);
        asc_storealign_postupdate(dst_addr, dst, one_repeat_size, mask);
    }
}
```
