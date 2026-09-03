# asc_bfloat162e2m1x2 (废弃)

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

头文件路径为：`"c_api/reg_compute/reg_convert.h"`。

将bfloat16_t类型转化为fp4x2_e2m1_t类型，并支持多种舍入模式。

关于舍入模式的详细说明，请参见[舍入模式](rounding_mode.md)。

由于源操作数与目的操作数类型位宽比为4:1，写入数据时需要将一个VL大小的数据分为四部分，根据不同接口选取索引0、索引1、索引2或者索引3。

## 函数原型

```cpp
// RINT舍入模式，数据写入索引为0的位置
__simd_callee__ inline void asc_bfloat162e2m1x2_rn(vector_fp4x2_e2m1_t& dst, vector_bfloat16_t src, vector_bool mask)
// RINT舍入模式，数据写入索引为1的位置
__simd_callee__ inline void asc_bfloat162e2m1x2_rn_v2(vector_fp4x2_e2m1_t& dst, vector_bfloat16_t src, vector_bool mask)
// RINT舍入模式，数据写入索引为2的位置
__simd_callee__ inline void asc_bfloat162e2m1x2_rn_v3(vector_fp4x2_e2m1_t& dst, vector_bfloat16_t src, vector_bool mask)
// RINT舍入模式，数据写入索引为3的位置
__simd_callee__ inline void asc_bfloat162e2m1x2_rn_v4(vector_fp4x2_e2m1_t& dst, vector_bfloat16_t src, vector_bool mask)
// ROUND舍入模式，数据写入索引为0的位置
__simd_callee__ inline void asc_bfloat162e2m1x2_rna(vector_fp4x2_e2m1_t& dst, vector_bfloat16_t src, vector_bool mask)
// ROUND舍入模式，数据写入索引为1的位置
__simd_callee__ inline void asc_bfloat162e2m1x2_rna_v2(vector_fp4x2_e2m1_t& dst, vector_bfloat16_t src, vector_bool mask)
// ROUND舍入模式，数据写入索引为2的位置
__simd_callee__ inline void asc_bfloat162e2m1x2_rna_v3(vector_fp4x2_e2m1_t& dst, vector_bfloat16_t src, vector_bool mask)
// ROUND舍入模式，数据写入索引为3的位置
__simd_callee__ inline void asc_bfloat162e2m1x2_rna_v4(vector_fp4x2_e2m1_t& dst, vector_bfloat16_t src, vector_bool mask)
// FLOOR舍入模式，数据写入索引为0的位置
__simd_callee__ inline void asc_bfloat162e2m1x2_rd(vector_fp4x2_e2m1_t& dst, vector_bfloat16_t src, vector_bool mask)
// FLOOR舍入模式，数据写入索引为1的位置
__simd_callee__ inline void asc_bfloat162e2m1x2_rd_v2(vector_fp4x2_e2m1_t& dst, vector_bfloat16_t src, vector_bool mask)
// FLOOR舍入模式，数据写入索引为2的位置
__simd_callee__ inline void asc_bfloat162e2m1x2_rd_v3(vector_fp4x2_e2m1_t& dst, vector_bfloat16_t src, vector_bool mask)
// FLOOR舍入模式，数据写入索引为3的位置
__simd_callee__ inline void asc_bfloat162e2m1x2_rd_v4(vector_fp4x2_e2m1_t& dst, vector_bfloat16_t src, vector_bool mask)
// CEIL舍入模式，数据写入索引为0的位置
__simd_callee__ inline void asc_bfloat162e2m1x2_ru(vector_fp4x2_e2m1_t& dst, vector_bfloat16_t src, vector_bool mask)
// CEIL舍入模式，数据写入索引为1的位置
__simd_callee__ inline void asc_bfloat162e2m1x2_ru_v2(vector_fp4x2_e2m1_t& dst, vector_bfloat16_t src, vector_bool mask)
// CEIL舍入模式，数据写入索引为2的位置
__simd_callee__ inline void asc_bfloat162e2m1x2_ru_v3(vector_fp4x2_e2m1_t& dst, vector_bfloat16_t src, vector_bool mask)
// CEIL舍入模式，数据写入索引为3的位置
__simd_callee__ inline void asc_bfloat162e2m1x2_ru_v4(vector_fp4x2_e2m1_t& dst, vector_bfloat16_t src, vector_bool mask)
// TRUNC舍入模式，数据写入索引为0的位置
__simd_callee__ inline void asc_bfloat162e2m1x2_rz(vector_fp4x2_e2m1_t& dst, vector_bfloat16_t src, vector_bool mask)
// TRUNC舍入模式，数据写入索引为1的位置
__simd_callee__ inline void asc_bfloat162e2m1x2_rz_v2(vector_fp4x2_e2m1_t& dst, vector_bfloat16_t src, vector_bool mask)
// TRUNC舍入模式，数据写入索引为2的位置
__simd_callee__ inline void asc_bfloat162e2m1x2_rz_v3(vector_fp4x2_e2m1_t& dst, vector_bfloat16_t src, vector_bool mask)
// TRUNC舍入模式，数据写入索引为3的位置
__simd_callee__ inline void asc_bfloat162e2m1x2_rz_v4(vector_fp4x2_e2m1_t& dst, vector_bfloat16_t src, vector_bool mask)
```

## 参数说明

**表1** 参数说明

| 参数名  | 输入/输出 | 描述 |
| :----- | :------- | :------- |
| dst | 输出 | 目的操作数（矢量数据寄存器）。 |
| src | 输入 | 源操作数（矢量数据寄存器）。 |
| mask | 输入 | 源操作数掩码（掩码寄存器），用于指示在计算过程中哪些元素参与计算。对应位置为1时参与计算，为0时不参与计算。mask未筛选的元素在输出中置零。 |

矢量数据寄存器和掩码寄存器的详细说明请参见[reg数据类型定义](../../defs/type/data_type_definition.md)。

## 返回值说明

无

## 流水类型

PIPE_V

## 约束说明

无

## 调用示例

```cpp
vector_bfloat16_t src;
vector_fp4x2_e2m1_t dst;
vector_bool mask;
asc_bfloat162e2m1x2_rn(dst, src, mask);
```
