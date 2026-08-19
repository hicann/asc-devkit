# asc_unsqueeze

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

将dst中数据根据mask进行解压缩。解压缩方式：dst中第0个元素置为0，dst中的第i个元素等于mask中从第0个到第(i-1)个元素中1的数量。mask最高位被忽略，不参与统计。

具体算法如图1所示，dst的首位为0。对于后续元素，与dst[i-1]对应的有效mask位为1时，dst[i]的值为dst[i-1] + 1；对应的有效mask位为0时，dst[i]的值为dst[i-1]。

**图1** unsqueeze流程

![unsqueeze流程](../../figures/capi_unsqueeze.png)

## 函数原型

```cpp
__simd_callee__ inline void asc_unsqueeze(vector_int8_t& dst, vector_bool mask)
__simd_callee__ inline void asc_unsqueeze(vector_uint8_t& dst, vector_bool mask)
__simd_callee__ inline void asc_unsqueeze(vector_int16_t& dst, vector_bool mask)
__simd_callee__ inline void asc_unsqueeze(vector_uint16_t& dst, vector_bool mask)
__simd_callee__ inline void asc_unsqueeze(vector_int32_t& dst, vector_bool mask)
__simd_callee__ inline void asc_unsqueeze(vector_uint32_t& dst, vector_bool mask)
```

## 参数说明

**表1** 参数说明

| 参数名 | 输入/输出 | 描述 |
| --- | --- | --- |
| dst | 输入/输出 | 源操作数和目的操作数（矢量数据寄存器）。 |
| mask | 输入 | 源操作数（掩码寄存器），用于提供dst解压缩信息。 |

矢量数据寄存器和掩码寄存器的详细说明请参见[reg数据类型定义](../reg_data_types/data_type_definition.md)。

## 返回值说明

无

## 约束说明

无

## 调用示例

```cpp
__simd_vf__ inline void unsqueeze_vf(__ubuf__ int8_t* dst_addr, uint32_t count, uint16_t one_repeat_size, uint16_t repeat_time)
{
    vector_int8_t dst;
    vector_bool mask;
    for (uint16_t i = 0; i < repeat_time; ++i) {
        mask = asc_update_mask_b8(count);
        asc_unsqueeze(dst, mask);
        asc_storealign(dst_addr + i * one_repeat_size, dst, mask);
    }
}
```
