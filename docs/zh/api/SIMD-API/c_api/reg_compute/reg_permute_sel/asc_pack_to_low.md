# asc_pack_to_low

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

头文件路径为：`"c_api/reg_compute/compute/reg_permute_sel.h"`。

将源操作数中的元素选取偶数位bit(bool)、低8位（b16）、低16位（b32）、低32位（b64）写入目的操作数的低半部分。
当源操作数为vector_bool时，其按bit位进行压缩，每2bit将其中的低位bit存储至目的操作数的低半部分。

![](../../../../figures/pack_low_draft.png)

## 函数原型

```cpp
__simd_callee__ inline void asc_pack_to_low(vector_uint8_t& dst, vector_uint16_t src)
__simd_callee__ inline void asc_pack_to_low(vector_uint16_t& dst, vector_uint32_t src)
__simd_callee__ inline void asc_pack_to_low(vector_bool& dst, vector_bool src)
```

## 参数说明

**表1** 参数说明

| 参数名       | 输入/输出 | 描述               |
| --------- | ----- | ---------------- |
| dst       | 输出    | 目的操作数（矢量数据寄存器/掩码寄存器）。            |
| src | 输入    | 源操作数（矢量数据寄存器/掩码寄存器）。            |

矢量数据寄存器和掩码寄存器的详细说明请参见[reg数据类型定义](../../defs/type/data_type_definition.md)。

## 返回值说明

无

## 约束说明

无

## 调用示例

```cpp
vector_uint8_t dst;
vector_uint16_t src;
asc_pack_to_low(dst, src);    // 将src的低8位写入dst的低半部分
```
