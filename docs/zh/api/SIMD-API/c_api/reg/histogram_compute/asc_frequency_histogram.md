# asc_frequency_histogram

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

对输入数据进行频率统计，生成直方图。统计结果在dst原有数据基础上累加。

**工作原理：**
- 统计src中每个数值的出现频率。
- dst的第n个元素存储数值n在src中出现的次数。
- 结果累加到dst原有数据中，支持多次统计的累加计算。

**分区间统计：**
由于src数据类型为uint8_t（取值范围0~255），而dst每个元素为uint16_t，且一个Vector Length可存储128个uint16_t数据，因此将统计范围分为两个区间：
- **低位模式（BIN0）**：统计src中数值在[0, 127]范围内的出现频率。
- **高位模式（BIN1）**：统计src中数值在[128, 255]范围内的出现频率，统计时将数值减去128，映射到dst的索引位置。

**统计方式如下图所示：**
- **低位模式（BIN0）**：低位模式统计结果，dst[0]表示数值0的出现次数，dst[127]表示数值127的出现次数。
- **高位模式（BIN1）**：高位模式统计结果，dst[0]表示数值128的出现次数，dst[127]表示数值255的出现次数。

![频率统计](../../figures/asc_frequency_histogram.png)

## 函数原型

  ```cpp
  // 低位模式
  __simd_callee__ inline void asc_frequency_histogram_bin0(vector_uint16_t& dst, vector_uint8_t src, vector_bool mask)
  // 高位模式
  __simd_callee__ inline void asc_frequency_histogram_bin1(vector_uint16_t& dst, vector_uint8_t src, vector_bool mask)
  ```

## 参数说明

**表1** 参数说明

| 参数名 | 输入/输出 | 描述 |
| --- | --- | --- |
| dst | 输入/输出 | 目的操作数（矢量数据寄存器）。|
| src | 输入 | 源操作数（矢量数据寄存器）。|
| mask | 输入 | 源操作数掩码（掩码寄存器），用于指示在计算过程中哪些元素参与计算。对应位置为1时参与计算，为0时不参与计算。 |

矢量数据寄存器和掩码寄存器的详细说明请参见[reg数据类型定义](../reg_data_types/data_type_definition.md)。

## 返回值说明

无

## 约束说明

- 该接口中mask用于筛选源操作数，不影响dst的写入行为，当mask位数为0时，源操作数src对应位置的数值将被忽略，dst对应位置数值为忽略该位置src后计算得到的值。
- dst的数据类型为uint16_t，最大值为65535，使用时需注意累加溢出问题。

## 调用示例

```cpp
vector_uint16_t dst0, dst1;  // dst0用于低位统计，dst1用于高位统计
vector_uint8_t src;
vector_bool mask = asc_create_mask_b8(PAT_ALL);

// 初始化dst为0，从零开始统计频率
asc_duplicate_scalar(dst0, (uint16_t)0, mask);
asc_duplicate_scalar(dst1, (uint16_t)0, mask);

// 加载待统计的数据
asc_loadalign(src, src_addr);  // src_addr是外部输入的Unified Buffer（UB）内存空间地址

// 分别统计低位和高位区间
asc_frequency_histogram_bin0(dst0, src, mask);  // 统计[0,127]
asc_frequency_histogram_bin1(dst1, src, mask);  // 统计[128,255]

// dst0[0]表示数值0的出现次数，dst0[127]表示数值127的出现次数
// dst1[0]表示数值128的出现次数，dst1[127]表示数值255的出现次数

// 支持对多批数据进行累加统计
asc_loadalign(src, src_addr2);  // 加载第二批数据, src_addr2是外部输入的UB内存空间地址
asc_frequency_histogram_bin0(dst0, src, mask);  // 累加到dst0
asc_frequency_histogram_bin1(dst1, src, mask);  // 累加到dst1
// 此时dst0和dst1包含两批数据的累积统计结果
```
