# asc_exp_sub (废弃)

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

将src0与src1相减，差值作为e的指数计算，i为元素索引。计算公式如下：

- 当src0和src1为float类型时：

$$
dst_i = e^{src0_i - src1_i}
$$

- 当src0和src1为half类型时：

$$
dst_i = e^{cast\_f16\_to\_f32(src0_i) - cast\_f16\_to\_f32(src1_i)}
$$

## 函数原型

```cpp
// 从源操作数的偶数位读取元素
__simd_callee__ inline void asc_exp_sub(vector_float& dst, vector_half src0, vector_half src1, vector_bool mask)
__simd_callee__ inline void asc_exp_sub(vector_float& dst, vector_float src0, vector_float src1, vector_bool mask)

// 从源操作数的奇数位读取元素
__simd_callee__ inline void asc_exp_sub_v2(vector_float& dst, vector_half src0, vector_half src1, vector_bool mask)
__simd_callee__ inline void asc_exp_sub_v2(vector_float& dst, vector_float src0, vector_float src1, vector_bool mask)
```

## 参数说明

**表1** 参数说明

| 参数名       | 输入/输出 | 描述               |
| --------- | ----- | ---------------- |
| dst       | 输出    | 目的操作数（矢量数据寄存器）。            |
| src0 | 输入    | 源操作数（矢量数据寄存器）。            |
| src1 | 输入    | 源操作数（矢量数据寄存器）。            |
| mask     | 输入    | 源操作数掩码（掩码寄存器），用于指示在计算过程中哪些元素参与计算。对应位置为1时参与计算，为0时不参与计算。mask未筛选的元素在输出中置零。        |

矢量数据寄存器和掩码寄存器的详细说明请参见[reg数据类型定义](../reg_data_types/data_type_definition.md)。

## 返回值说明

无

## 流水类型

PIPE_V

## 约束说明

无

## 调用示例

```cpp
vector_float dst;
vector_half src0;
vector_half src1;
vector_bool mask;
asc_exp_sub(dst, src0, src1, mask);    // 从源操作数的偶数位读取元素转换成float，再进行计算
asc_exp_sub_v2(dst, src0, src1, mask);    // 从源操作数的奇数位读取元素转换成float，再进行计算
```
