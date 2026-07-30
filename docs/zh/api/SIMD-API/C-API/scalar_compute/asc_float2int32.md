# asc_float2int32

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

将float类型转化为int32_t类型，并支持多种舍入模式。

舍入模式：
- RINT舍入模式：四舍六入五成双舍入
- ROUND舍入模式：四舍五入舍入
- FLOOR舍入模式：向负无穷舍入
- CEIL舍入模式：向正无穷舍入

## 函数原型

```cpp
// RINT舍入模式
__aicore__ inline int32_t asc_float2int32_rn(float value)
// ROUND舍入模式
__aicore__ inline int32_t asc_float2int32_rna(float value)
// FLOOR舍入模式
__aicore__ inline int32_t asc_float2int32_rd(float value)
// CEIL舍入模式
__aicore__ inline int32_t asc_float2int32_ru(float value)
```

## 参数说明

| 参数名 | 输入/输出 | 描述 |
| :------ | :----- | :----------------- |
| value | 输入 | 源操作数（标量）。 |

## 返回值说明

目的操作数（标量），value精度转换成int32_t的结果。

## 流水类型

PIPE_S

## 约束说明

无

## 调用示例

```cpp
float value = 3.0;
int32_t dst = asc_float2int32_rn(value);
```
