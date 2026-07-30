# asc_storealign_1st_postupdate

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

从矢量数据寄存器搬出第一个元素到UB的接口，忽略mask，向dst_align32b中搬运src第一个元素，接口调用后自动更新目的操作数地址。支持数据类型为b8、b16、b32。

## 函数原型

```cpp
__simd_callee__ inline void asc_storealign_1st_postupdate(__ubuf__ int8_t*& dst_align32b, vector_int8_t src, int32_t offset)
__simd_callee__ inline void asc_storealign_1st_postupdate(__ubuf__ uint8_t*& dst_align32b, vector_uint8_t src, int32_t offset)
__simd_callee__ inline void asc_storealign_1st_postupdate(__ubuf__ int16_t*& dst_align32b, vector_int16_t src, int32_t offset)
__simd_callee__ inline void asc_storealign_1st_postupdate(__ubuf__ uint16_t*& dst_align32b, vector_uint16_t src, int32_t offset)
__simd_callee__ inline void asc_storealign_1st_postupdate(__ubuf__ int32_t*& dst_align32b, vector_int32_t src, int32_t offset)
__simd_callee__ inline void asc_storealign_1st_postupdate(__ubuf__ uint32_t*& dst_align32b, vector_uint32_t src, int32_t offset)
__simd_callee__ inline void asc_storealign_1st_postupdate(__ubuf__ half*& dst_align32b, vector_half src, int32_t offset)
__simd_callee__ inline void asc_storealign_1st_postupdate(__ubuf__ float*& dst_align32b, vector_float src, int32_t offset)
__simd_callee__ inline void asc_storealign_1st_postupdate(__ubuf__ bfloat16_t*& dst_align32b, vector_bfloat16_t src, int32_t offset)
```

## 参数说明

| 参数名  | 输入/输出 | 描述 |
| :----- | :------- | :------- |
| dst_align32b | 输入/输出 | 目的操作数（矢量）的起始地址，搬运完成后该地址会被硬件自动Post Update。 |
| src | 输入 | 源操作数（矢量数据寄存器）。 |
| offset | 输入 | 地址偏移量。 |

矢量数据寄存器的详细说明请参见[data_type_definition.md](../reg_data_types/data_type_definition.md)。

## 返回值说明

无

## 流水类型

PIPE_V

## 约束说明

无

## 调用示例

```cpp
__ubuf__ half* dst_align32b = (__ubuf__ half*)asc_get_phy_buf_addr(0);
vector_half src;
int32_t offset = 128;
asc_storealign_1st_postupdate(dst_align32b, src, offset);
```
