# asc_scatter（废弃）

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

**该接口已废弃。请使用[asc_scatter](../reg_compute/scatter/asc_scatter.md)实现此功能。**

该指令会根据索引值index将源操作数src中的元素分散到目的操作数dst中。分散过程如下图所示：

![scatter功能说明](../figures/reg_scatter.png)

## 函数原型

```cpp
__simd_callee__ inline void asc_scatter(vector_int8_t& src, __ubuf__ int8_t* dst, vector_uint16_t index, vector_bool mask)
__simd_callee__ inline void asc_scatter(vector_uint8_t& src, __ubuf__ uint8_t* dst, vector_uint16_t index, vector_bool mask)
__simd_callee__ inline void asc_scatter(vector_int16_t& src, __ubuf__ int16_t* dst, vector_uint16_t index, vector_bool mask)
__simd_callee__ inline void asc_scatter(vector_uint16_t& src, __ubuf__ uint16_t* dst, vector_uint16_t index, vector_bool mask)
__simd_callee__ inline void asc_scatter(vector_int32_t& src, __ubuf__ int32_t* dst, vector_uint32_t index, vector_bool mask)
__simd_callee__ inline void asc_scatter(vector_uint32_t& src, __ubuf__ uint32_t* dst, vector_uint32_t index, vector_bool mask)
__simd_callee__ inline void asc_scatter(vector_bfloat16_t& src, __ubuf__ bfloat16_t* dst, vector_uint16_t index, vector_bool mask)
__simd_callee__ inline void asc_scatter(vector_half& src, __ubuf__ half* dst, vector_uint16_t index, vector_bool mask)
__simd_callee__ inline void asc_scatter(vector_float& src, __ubuf__ float* dst, vector_uint32_t index, vector_bool mask)
```

## 参数说明

**表1** 参数说明

| 参数名   | 输入/输出 | 描述                                                                      |
|:------| :--- |:------------------------------------------------------------------------|
| src   | 输入 | 源操作数（矢量数据寄存器）。                                                           |
| dst   | 输出 | 目的操作数（矢量）的起始地址。                                                          |
| index | 输入 | 数据索引（矢量数据寄存器）。                                                          |
| mask  | 输入 | 源操作数掩码（掩码寄存器），用于指示在计算过程中哪些元素参与计算。对应位置为1时参与计算，为0时不参与计算。 |

掩码寄存器的详细说明请参见[reg数据类型定义](../defs/type/data_type_definition.md)。

## 返回值说明

无

## 流水类型

PIPE_V

## 约束说明

- dst首地址必须32B对齐。
- 当dst为int8_t或者uint8_t数据类型时，源操作数中仅偶数位元素有效。即src中的偶数位置[0, 2, 4, ..., 252, 254]的数据会被分散存储到目的操作数中。
- index中的值必须唯一。若存在重复的index值，系统仅保留其中一个对应的数据，其余将被忽略。无法确定具体保留哪一个，因此必须确保index值不重复。

## 调用示例

```cpp
// dst为外部输入的指向Unified Buffer（UB）内存空间的half类型的指针。
vector_half src;
vector_uint16_t index;
vector_bool mask = asc_create_mask_b16(PAT_ALL);
asc_scatter(src, dst, index, mask);
```
