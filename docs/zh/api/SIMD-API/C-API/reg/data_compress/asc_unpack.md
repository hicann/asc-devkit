# asc_unpack

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

矢量解包操作。对于无符号整型，将源操作数中低半部分或高半部分的元素以高位填0扩充位宽的方式写入目标操作数，对于有符号整型，将源操作数中低半部分或高半部分元素以保持符号位扩展位宽的方式写入目标操作数，对于布尔矢量，将源操作数中低半部分或高半部分的元素，保持其真值并维持原有的顺序，写入目标操作数。

## 函数原型

```cpp
__simd_callee__ inline void asc_unpack_upper(vector_uint16_t& dst, vector_uint8_t src)
__simd_callee__ inline void asc_unpack_lower(vector_uint16_t& dst, vector_uint8_t src)
__simd_callee__ inline void asc_unpack_upper(vector_int16_t& dst, vector_int8_t src)
__simd_callee__ inline void asc_unpack_lower(vector_int16_t& dst, vector_int8_t src)
__simd_callee__ inline void asc_unpack_upper(vector_uint32_t& dst, vector_uint16_t src)
__simd_callee__ inline void asc_unpack_lower(vector_uint32_t& dst, vector_uint16_t src)
__simd_callee__ inline void asc_unpack_upper(vector_int32_t& dst, vector_int16_t src)
__simd_callee__ inline void asc_unpack_lower(vector_int32_t& dst, vector_int16_t src)
__simd_callee__ inline void asc_unpack_upper(vector_bool& dst, vector_bool src)
__simd_callee__ inline void asc_unpack_lower(vector_bool& dst, vector_bool src)
```

## 参数说明

| 参数名  | 输入/输出 | 描述 |
| :----- | :------- | :------- |
| dst | 输出 | 目的操作数（矢量数据寄存器/掩码寄存器）。 |
| src | 输入 | 源操作数（矢量数据寄存器/掩码寄存器）。 |

矢量数据寄存器和掩码寄存器的详细说明请参见[reg数据类型定义](../reg_data_types/data_type_definition.md)。

## 返回值说明

无

## 流水类型

PIPE_V

## 约束说明

无

## 调用示例

```cpp
vector_uint16_t dst;
vector_uint8_t src;
asc_loadalign(src, src_addr); // src_addr是外部输入的UB内存空间地址。
asc_unpack_upper(dst, src);
```