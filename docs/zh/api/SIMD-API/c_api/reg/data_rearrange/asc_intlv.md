# asc_intlv

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

将源操作数src0和src1中的元素交织存入目的操作数dst0和dst1中。按照操作数类型划分，可以分以下场景：

**操作数为掩码寄存器**
- 不同数据类型的接口决定了交织的位宽大小，例如对于b32类型，交织时以4bit为一组，对于b16类型，交织时以2bit为一组。

**操作数为矢量数据寄存器**
- 以元素为单位进行交织。以int8_t矢量数据寄存器为例，交织过程如下图所示。

![asc_intlv示例](../../figures/asc_intlv.png)

## 函数原型

  ```cpp
  // 操作数为掩码寄存器
  __simd_callee__ inline void asc_intlv_b8(vector_bool& dst0, vector_bool& dst1, vector_bool src0, vector_bool src1)
  __simd_callee__ inline void asc_intlv_b16(vector_bool& dst0, vector_bool& dst1, vector_bool src0, vector_bool src1)
  __simd_callee__ inline void asc_intlv_b32(vector_bool& dst0, vector_bool& dst1, vector_bool src0, vector_bool src1)
  // 操作数为矢量数据寄存器
  __simd_callee__ inline void asc_intlv(vector_int8_t& dst0, vector_int8_t& dst1, vector_int8_t src0, vector_int8_t src1)
  __simd_callee__ inline void asc_intlv(vector_uint8_t& dst0, vector_uint8_t& dst1, vector_uint8_t src0, vector_uint8_t src1)
  __simd_callee__ inline void asc_intlv(vector_hifloat8_t& dst0, vector_hifloat8_t& dst1, vector_hifloat8_t src0, vector_hifloat8_t src1)
  __simd_callee__ inline void asc_intlv(vector_fp8_e8m0_t& dst0, vector_fp8_e8m0_t& dst1, vector_fp8_e8m0_t src0, vector_fp8_e8m0_t src1)
  __simd_callee__ inline void asc_intlv(vector_fp8_e5m2_t& dst0, vector_fp8_e5m2_t& dst1, vector_fp8_e5m2_t src0, vector_fp8_e5m2_t src1)
  __simd_callee__ inline void asc_intlv(vector_fp8_e4m3fn_t& dst0, vector_fp8_e4m3fn_t& dst1, vector_fp8_e4m3fn_t src0, vector_fp8_e4m3fn_t src1)
  __simd_callee__ inline void asc_intlv(vector_int16_t& dst0, vector_int16_t& dst1, vector_int16_t src0, vector_int16_t src1)
  __simd_callee__ inline void asc_intlv(vector_uint16_t& dst0, vector_uint16_t& dst1, vector_uint16_t src0, vector_uint16_t src1)
  __simd_callee__ inline void asc_intlv(vector_half& dst0, vector_half& dst1, vector_half src0, vector_half src1)
  __simd_callee__ inline void asc_intlv(vector_bfloat16_t& dst0, vector_bfloat16_t& dst1, vector_bfloat16_t src0, vector_bfloat16_t src1)
  __simd_callee__ inline void asc_intlv(vector_int32_t& dst0, vector_int32_t& dst1, vector_int32_t src0, vector_int32_t src1)
  __simd_callee__ inline void asc_intlv(vector_uint32_t& dst0, vector_uint32_t& dst1, vector_uint32_t src0, vector_uint32_t src1)
  __simd_callee__ inline void asc_intlv(vector_float& dst0, vector_float& dst1, vector_float src0, vector_float src1)
  ```

## 参数说明

**表1** 参数说明

| 参数名 | 输入/输出 | 描述 |
| --- | --- | --- |
| dst0 | 输出 | 目的操作数（矢量数据寄存器或掩码寄存器）。|
| dst1 | 输出 | 目的操作数（矢量数据寄存器或掩码寄存器）。|
| src0 | 输入 | 源操作数（矢量数据寄存器或掩码寄存器）。|
| src1 | 输入 | 源操作数（矢量数据寄存器或掩码寄存器）。|

矢量数据寄存器和掩码寄存器的详细说明请参见[reg数据类型定义](../reg_data_types/data_type_definition.md)。

## 返回值说明

无

## 约束说明

- src0、src1、dst0、dst1的数据类型需要保持一致。
- src0和src1可以为同一个矢量数据寄存器或掩码寄存器。
- dst0和dst1不能为同一个矢量数据寄存器或掩码寄存器。
- 允许源操作数和目的操作数为同一个矢量数据寄存器或掩码寄存器，例如asc_intlv(src0, src1, src0, src1)。

## 调用示例

```cpp
vector_int8_t dst0;
vector_int8_t dst1;
vector_int8_t src0;
vector_int8_t src1;
asc_loadalign(src0, src0_addr); // src0_addr是外部输入的UB内存空间地址。
asc_loadalign(src1, src1_addr); // src1_addr是外部输入的UB内存空间地址。
asc_intlv(dst0, dst1, src0, src1);
```
