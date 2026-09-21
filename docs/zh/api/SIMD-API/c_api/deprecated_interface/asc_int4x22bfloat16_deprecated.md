# asc_int4x22bfloat16（废弃）

## 产品支持情况

<!-- npu="950" id1 -->
- Ascend 950PR&950DT系列产品：支持
<!-- end id1 -->
<!-- npu="A3" id2 -->
- Atlas A3系列产品：不支持
<!-- end id2 -->
<!-- npu="910b" id3 -->
- Atlas A2系列产品：不支持
<!-- end id3 -->
<!-- npu="310b" id4 -->
- Atlas 200I/500 A2推理产品：不支持
<!-- end id4 -->
<!-- npu="310p" id5 -->
- Atlas推理系列产品AI Core：不支持
<!-- end id5 -->
<!-- npu="310p" id6 -->
- Atlas推理系列产品Vector Core：不支持
<!-- end id6 -->
<!-- npu="910" id7 -->
- Atlas训练系列产品：不支持
<!-- end id7 -->

## 功能说明

**该接口的无显式位置参数原型已废弃，请使用[asc_int4x22bfloat16](../reg_compute/reg_convert/asc_int4x22bfloat16.md)的显式位置参数重载替代。**

`int4x2_t`将两个连续的4bit有符号整数打包为一个8bit存储单元。本接口根据`mask`将参与计算的4bit有符号整数转换为`bfloat16_t`类型。

由于每个4bit有符号整数的位宽小于1Byte，实际处理时，同一`int4x2_t`存储单元内的两个4bit有符号整数共同对应`mask`中的一个掩码位。该掩码位为1时，两个4bit有符号整数同时参与转换，转换结果为两个连续存放的`bfloat16_t`类型元素；为0时，对应的两个`bfloat16_t`类型元素均置0。

## 函数原型

```cpp
// 数据读取索引为0的位置
__simd_callee__ inline void asc_int4x22bfloat16(vector_bfloat16_t& dst, vector_int4x2_t src, vector_bool mask)
// 数据读取索引为1的位置
__simd_callee__ inline void asc_int4x22bfloat16_v2(vector_bfloat16_t& dst, vector_int4x2_t src, vector_bool mask)
// 数据读取索引为2的位置
__simd_callee__ inline void asc_int4x22bfloat16_v3(vector_bfloat16_t& dst, vector_int4x2_t src, vector_bool mask)
// 数据读取索引为3的位置
__simd_callee__ inline void asc_int4x22bfloat16_v4(vector_bfloat16_t& dst, vector_int4x2_t src, vector_bool mask)
```

## 参数说明

**表1** 参数说明

| 参数名    | 输入/输出 | 描述                |
| :------ | :----- | :----------------- |
| dst    | 输出    | 目的操作数（矢量数据寄存器）。            |
| src    | 输入    | 源操作数（矢量数据寄存器）。             |
| mask | 输入 | 源操作数掩码（掩码寄存器），用于指示在计算过程中哪些元素参与计算。对应位置为1时参与计算，为0时不参与计算。mask未筛选的元素在输出中置零。 |

矢量数据寄存器和掩码寄存器的详细说明请参见[reg数据类型定义](../defs/type/data_type_definition.md)。

## 返回值说明

无

## 约束说明

无

## 调用示例

```cpp
vector_int4x2_t src;
vector_bfloat16_t dst;
vector_bool mask = asc_create_mask_b8(PAT_ALL);
asc_loadalign(src, src_addr); // src_addr是外部输入的Unified Buffer（UB）内存空间地址。
asc_int4x22bfloat16(dst, src, mask);
```
