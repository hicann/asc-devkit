# asc_float2int64（废弃）

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

**该接口通过接口后缀来控制目的操作数写入位置（仅包括源操作数、目的操作数、掩码寄存器三个参数）的原型已废弃，请使用[asc_float2int64](../reg_compute/reg_convert/asc_float2int64.md)的显式位置参数重载替代。**

将float类型数据转化为int64_t类型，并支持多种舍入模式，饱和/非饱和模式。

关于舍入模式和饱和/非饱和模式的详细说明，请参见[舍入模式与饱和模式](../reg_compute/reg_convert/rounding_mode.md)。

由于源操作数与目的操作数类型位宽比为1:2，读取数据时需要将一个VL大小的数据分为两部分，根据不同接口选择输入数据索引为奇数的位置或偶数的位置。

## 函数原型

```cpp
// RINT舍入模式，非饱和模式，数据读取索引为偶数的位置
__simd_callee__ inline void asc_float2int64_rn(vector_int64_t& dst, vector_float src, vector_bool mask)
// RINT舍入模式，饱和模式，数据读取索引为偶数的位置
__simd_callee__ inline void asc_float2int64_rn_sat(vector_int64_t& dst, vector_float src, vector_bool mask)
// RINT舍入模式，非饱和模式，数据读取索引为奇数的位置
__simd_callee__ inline void asc_float2int64_rn_v2(vector_int64_t& dst, vector_float src, vector_bool mask)
// RINT舍入模式，饱和模式，数据读取索引为奇数的位置
__simd_callee__ inline void asc_float2int64_rn_sat_v2(vector_int64_t& dst, vector_float src, vector_bool mask)
// ROUND舍入模式，非饱和模式，数据读取索引为偶数的位置
__simd_callee__ inline void asc_float2int64_rna(vector_int64_t& dst, vector_float src, vector_bool mask)
// ROUND舍入模式，饱和模式，数据读取索引为偶数的位置
__simd_callee__ inline void asc_float2int64_rna_sat(vector_int64_t& dst, vector_float src, vector_bool mask)
// ROUND舍入模式，非饱和模式，数据读取索引为奇数的位置
__simd_callee__ inline void asc_float2int64_rna_v2(vector_int64_t& dst, vector_float src, vector_bool mask)
// ROUND舍入模式，饱和模式，数据读取索引为奇数的位置
__simd_callee__ inline void asc_float2int64_rna_sat_v2(vector_int64_t& dst, vector_float src, vector_bool mask)
// FLOOR舍入模式，非饱和模式，数据读取索引为偶数的位置
__simd_callee__ inline void asc_float2int64_rd(vector_int64_t& dst, vector_float src, vector_bool mask)
// FLOOR舍入模式，饱和模式，数据读取索引为偶数的位置
__simd_callee__ inline void asc_float2int64_rd_sat(vector_int64_t& dst, vector_float src, vector_bool mask)
// FLOOR舍入模式，非饱和模式，数据读取索引为奇数的位置
__simd_callee__ inline void asc_float2int64_rd_v2(vector_int64_t& dst, vector_float src, vector_bool mask)
// FLOOR舍入模式，饱和模式，数据读取索引为奇数的位置
__simd_callee__ inline void asc_float2int64_rd_sat_v2(vector_int64_t& dst, vector_float src, vector_bool mask)
// CEIL舍入模式，非饱和模式，数据读取索引为偶数的位置
__simd_callee__ inline void asc_float2int64_ru(vector_int64_t& dst, vector_float src, vector_bool mask)
// CEIL舍入模式，饱和模式，数据读取索引为偶数的位置
__simd_callee__ inline void asc_float2int64_ru_sat(vector_int64_t& dst, vector_float src, vector_bool mask)
// CEIL舍入模式，非饱和模式，数据读取索引为奇数的位置
__simd_callee__ inline void asc_float2int64_ru_v2(vector_int64_t& dst, vector_float src, vector_bool mask)
// CEIL舍入模式，饱和模式，数据读取索引为奇数的位置
__simd_callee__ inline void asc_float2int64_ru_sat_v2(vector_int64_t& dst, vector_float src, vector_bool mask)
// TRUNC舍入模式，非饱和模式，数据读取索引为偶数的位置
__simd_callee__ inline void asc_float2int64_rz(vector_int64_t& dst, vector_float src, vector_bool mask)
// TRUNC舍入模式，饱和模式，数据读取索引为偶数的位置
__simd_callee__ inline void asc_float2int64_rz_sat(vector_int64_t& dst, vector_float src, vector_bool mask)
// TRUNC舍入模式，非饱和模式，数据读取索引为奇数的位置
__simd_callee__ inline void asc_float2int64_rz_v2(vector_int64_t& dst, vector_float src, vector_bool mask)
// TRUNC舍入模式，饱和模式，数据读取索引为奇数的位置
__simd_callee__ inline void asc_float2int64_rz_sat_v2(vector_int64_t& dst, vector_float src, vector_bool mask)
```

## 参数说明

**表1** 参数说明

| 参数名 | 输入/输出 | 描述 |
| --- | --- | --- |
| dst | 输出 | 目的操作数（矢量数据寄存器）。|
| src | 输入 | 源操作数（矢量数据寄存器）。|
| mask | 输入 | 源操作数掩码（掩码寄存器），用于指示在计算过程中哪些元素参与计算。对应位置为1时参与计算，为0时不参与计算。mask未筛选的元素在输出中置零。 |

矢量数据寄存器和掩码寄存器的详细说明请参见[reg数据类型定义](../defs/type/data_type_definition.md)。

## 返回值说明

无

## 约束说明

- mask控制源操作数是否参与计算，源操作数不参与计算的元素在输出对应位置置零。
- 开启饱和模式和非饱和模式时，需配置ctrl寄存器，ctrl寄存器的详细说明请参见[asc_set_ctrl.md](../spr/asc_set_ctrl.md)。

## 调用示例

```cpp
vector_float src;
vector_int64_t dst;
vector_bool mask;
mask = asc_create_mask_b32(PAT_ALL);
asc_float2int64_rn(dst, src, mask);
```
