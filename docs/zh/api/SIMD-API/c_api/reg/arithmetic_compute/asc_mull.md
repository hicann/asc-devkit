# asc_mull

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

该接口根据mask对输入数据src0、src1执行按元素相乘操作，将乘法结果的低位部分写入dst0，高位（溢出）部分写入dst1。计算公式及图示如下：
  
  $$
  dst0_i = (src0_i \times src1_i) mod 2^{bit}
  $$
  
  $$
  dst1_i = (src0_i \times src1_i) / 2^{bit}
  $$
  
其中，bit表示操作数的位宽。

![asc_mull示例](../../figures/asc_mull.png)

## 函数原型

  ```cpp
  __simd_callee__ inline void asc_mull(vector_int32_t& dst0, vector_int32_t& dst1, vector_int32_t src0, vector_int32_t src1, vector_bool mask)
  __simd_callee__ inline void asc_mull(vector_uint32_t& dst0, vector_uint32_t& dst1, vector_uint32_t src0, vector_uint32_t src1, vector_bool mask)
  ```

## 参数说明

**表1** 参数说明

| 参数名 | 输入/输出 | 描述 |
| --- | --- | --- |
| dst0 | 输出 | 目的操作数（矢量数据寄存器）。|
| dst1 | 输出 | 目的操作数（矢量数据寄存器）。|
| src0 | 输入 | 源操作数（矢量数据寄存器）。|
| src1 | 输入 | 源操作数（矢量数据寄存器）。|
| mask | 输入 | 源操作数掩码（掩码寄存器），用于指示在计算过程中哪些元素参与计算。对应位置为1时参与计算，为0时不参与计算。mask未筛选的元素在输出中置零。 |

矢量数据寄存器和掩码寄存器的详细说明请参见[reg数据类型定义](../reg_data_types/data_type_definition.md)。

## 返回值说明

无

## 约束说明

无

## 调用示例

```cpp
vector_int32_t dst0;
vector_int32_t dst1;
vector_int32_t src0;
vector_int32_t src1;
vector_bool mask = asc_create_mask_b32(PAT_ALL);
asc_loadalign(src0, src0_addr); // src0_addr是外部输入的Unified Buffer（UB）内存空间地址。
asc_loadalign(src1, src1_addr); // src1_addr是外部输入的UB内存空间地址。
asc_mull(dst0, dst1, src0, src1, mask);
```
