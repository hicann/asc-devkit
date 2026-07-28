# asc_half2float

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

头文件路径：`"c_api/reg_compute/reg_convert.h"`。

将half类型数据转换为float类型，无舍入模式。

由于源操作数与目的操作数类型位宽比为1:2，读取数据时需要将一个VL大小的数据分为两部分，根据不同接口选择输入数据索引为奇数的位置或偶数的位置。

## 函数原型

```cpp
// 数据读取索引为偶数的位置
__simd_callee__ inline void asc_half2float(vector_float& dst, vector_half src, vector_bool mask)
// 数据读取索引为奇数的位置
__simd_callee__ inline void asc_half2float_v2(vector_float& dst, vector_half src, vector_bool mask)
```

## 参数说明

**表1** 参数说明

| 参数名 | 输入/输出 | 描述 |
| :--- | :--- | :--- |
| dst | 输出 | 目的操作数（矢量数据寄存器）。 |
| src | 输入 | 源操作数（矢量数据寄存器）。 |
| mask | 输入 | 源操作数掩码（掩码寄存器）。用于指示在计算过程中哪些元素参与计算。对应位置为1时参与计算，为0时不参与计算。mask未筛选的元素在输出中置零。 |

矢量数据寄存器和掩码寄存器的详细说明请参见[reg数据类型定义](../reg_data_types/data_type_definition.md)。

## 返回值说明

无

## 约束说明

mask控制源操作数是否参与计算，源操作数不参与计算的元素在输出对应位置置零。

## 调用示例

 ```cpp
__simd_vf__ inline void half2float_vf(__ubuf__ float* dst_addr, __ubuf__ half* src_addr, uint32_t one_repeat_size, uint16_t one_block_size, uint16_t repeat_time)
{
    vector_half src;
    vector_float dst;
    vector_bool mask = asc_create_mask_b16(PAT_ALL);
    for (uint16_t i = 0; i < repeat_time; ++i) {
        asc_loadalign(src, src_addr, one_repeat_size);
        asc_half2float(dst, src, mask);
        asc_storealign(dst_addr, dst, one_block_size, mask);
    }
}
```
