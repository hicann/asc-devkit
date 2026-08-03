# asc_cumulative_histogram

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

该接口用于对直方图数据进行累计统计。

如下图所示，在低位模式（BIN0）下，目的寄存器dst会统计源寄存器src中落在低位区间[0-127]的数据分布情况；在高位模式（BIN1）下，目的寄存器dst则会统计src中落在高位区间[128-255]的数据分布情况。dst中的第n个元素表示src中从0到n的所有数值在对应区间中出现的总频率。最终，统计结果会在目的寄存器原始数据的基础上进行累加。

![累计统计](../../../../figures/asc_cumulative_histogram.png)

## 函数原型

  ```cpp
  // 低位模式
  __simd_callee__ inline void asc_cumulative_histogram_bin0(vector_uint16_t& dst, vector_uint8_t src, vector_bool mask)
  // 高位模式
  __simd_callee__ inline void asc_cumulative_histogram_bin1(vector_uint16_t& dst, vector_uint8_t src, vector_bool mask)
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

- 当mask位数为0时，对应位置的src源操作数的数值被忽略，dst对应位置数值为忽略该位置src后计算得到的值。
- dst的数据类型为uint16_t，最大值为65535，使用时需注意累加溢出问题。

## 调用示例

```cpp
__simd_vf__ inline void cumulative_histogram_vf(__ubuf__ uint16_t* dst_addr, __ubuf__ uint8_t* src_addr, uint32_t count, uint16_t one_repeat_size, uint16_t repeat_time)
{
    vector_uint16_t dst0, dst1;  // dst0用于低位统计，dst1用于高位统计
    vector_uint8_t src;
    vector_bool mask_b8 = asc_create_mask_b8(PAT_ALL);
    vector_bool mask_b16 = asc_create_mask_b16(PAT_ALL);
    
    // 初始化dst为0，从零开始统计频率
    asc_duplicate_scalar(dst0, (uint16_t)0);
    asc_duplicate_scalar(dst1, (uint16_t)0);

    for (uint16_t i = 0; i < repeat_time; ++i) {
        asc_loadalign_postupdate(src, src_addr, one_repeat_size);
        asc_cumulative_histogram_bin0(dst0, src, mask_b8);
        asc_cumulative_histogram_bin1(dst1, src, mask_b8);
    }

    // 此时dst0和dst1包含两批数据的累积统计结果
    asc_storealign_postupdate(dst_addr, dst0, one_repeat_size, mask_b16);
    asc_storealign_postupdate(dst_addr, dst1, one_repeat_size, mask_b16);
}
```
