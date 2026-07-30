# asc_loadalign_deintlv_postupdate

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

`asc_loadalign_deintlv_postupdate`用于从UB中读取以32B对齐地址`src`为起始位置的连续`2 x VL`长度数据。数据按元素类型宽度进行解释，在Load过程中完成解交织后，分别写入两个目的向量寄存器`dst0`和`dst1`。

与[`asc_loadalign_deintlv`](asc_loadalign_deintlv.md)不同，该接口在完成数据加载后，会对源地址参数`src`执行后更新操作：

```cpp
src = src + offset
```

其中，`offset`的单位为元素个数，实际地址偏移字节数为：`offset * sizeof(data_type)`。

以`float`数据类型、Ascend 950PR/Ascend 950DT产品为例，若`VL = 256B`、`src = 32`、`offset = 128`，调用`asc_loadalign_deintlv_postupdate`后，数据从UB到Reg的排布如下图所示，同时`src`会更新为`544`，可直接用于后续计算。

![](../../figures/asc_loadalign_deintlv_postupdate.png)

> 提示：
>
> 1. 使用本接口需要包含头文件`reg_load.h`。
> 2. 本接口会修改输入的UB地址参数`src`。若需要保留原始地址，请使用[`asc_loadalign_deintlv`](asc_loadalign_deintlv.md)。

## 函数原型

支持的数据类型请参见[数据类型](#data-type)章节。

```cpp
__simd_callee__ inline void asc_loadalign_deintlv_postupdate(vector_int4x2_t& dst0, vector_int4x2_t& dst1, __ubuf__ int4b_t*& src, int32_t offset)
__simd_callee__ inline void asc_loadalign_deintlv_postupdate(vector_fp4x2_e2m1_t& dst0, vector_fp4x2_e2m1_t& dst1, __ubuf__ fp4x2_e2m1_t*& src, int32_t offset)
__simd_callee__ inline void asc_loadalign_deintlv_postupdate(vector_fp4x2_e1m2_t& dst0, vector_fp4x2_e1m2_t& dst1, __ubuf__ fp4x2_e1m2_t*& src, int32_t offset)
__simd_callee__ inline void asc_loadalign_deintlv_postupdate(vector_int8_t& dst0, vector_int8_t& dst1, __ubuf__ int8_t*& src, int32_t offset)
__simd_callee__ inline void asc_loadalign_deintlv_postupdate(vector_uint8_t& dst0, vector_uint8_t& dst1, __ubuf__ uint8_t*& src, int32_t offset)
__simd_callee__ inline void asc_loadalign_deintlv_postupdate(vector_fp8_e8m0_t& dst0, vector_fp8_e8m0_t& dst1, __ubuf__ fp8_e8m0_t*& src, int32_t offset)
__simd_callee__ inline void asc_loadalign_deintlv_postupdate(vector_fp8_e5m2_t& dst0, vector_fp8_e5m2_t& dst1, __ubuf__ fp8_e5m2_t*& src, int32_t offset)
__simd_callee__ inline void asc_loadalign_deintlv_postupdate(vector_fp8_e4m3fn_t& dst0, vector_fp8_e4m3fn_t& dst1, __ubuf__ fp8_e4m3fn_t*& src, int32_t offset)
__simd_callee__ inline void asc_loadalign_deintlv_postupdate(vector_hifloat8_t& dst0, vector_hifloat8_t& dst1, __ubuf__ hifloat8_t*& src, int32_t offset)
__simd_callee__ inline void asc_loadalign_deintlv_postupdate(vector_int16_t& dst0, vector_int16_t& dst1, __ubuf__ int16_t*& src, int32_t offset)
__simd_callee__ inline void asc_loadalign_deintlv_postupdate(vector_uint16_t& dst0, vector_uint16_t& dst1, __ubuf__ uint16_t*& src, int32_t offset)
__simd_callee__ inline void asc_loadalign_deintlv_postupdate(vector_half& dst0, vector_half& dst1, __ubuf__ half*& src, int32_t offset)
__simd_callee__ inline void asc_loadalign_deintlv_postupdate(vector_bfloat16_t& dst0, vector_bfloat16_t& dst1, __ubuf__ bfloat16_t*& src, int32_t offset)
__simd_callee__ inline void asc_loadalign_deintlv_postupdate(vector_int32_t& dst0, vector_int32_t& dst1, __ubuf__ int32_t*& src, int32_t offset)
__simd_callee__ inline void asc_loadalign_deintlv_postupdate(vector_uint32_t& dst0, vector_uint32_t& dst1, __ubuf__ uint32_t*& src, int32_t offset)
__simd_callee__ inline void asc_loadalign_deintlv_postupdate(vector_float& dst0, vector_float& dst1, __ubuf__ float*& src, int32_t offset)
```

## 参数说明

| 参数名 | 输入/输出 | 说明 |
| :--- | :--- | :--- |
| dst0 | 输出 | 第一个目的向量寄存器。 |
| dst1 | 输出 | 第二个目的向量寄存器。 |
| src | 输入/输出 | 源数据在UB中的起始地址，要求32B对齐。接口执行完成后，`src`会更新为`src + offset`。 |
| offset | 输入 | 地址偏移量，单位为元素个数。 |

向量寄存器类型的详细说明请参见[data_type_definition.md](../reg_data_types/data_type_definition.md)。

## 数据类型<a name="data-type"></a>

`asc_loadalign_deintlv_postupdate`支持的数据类型如下表所示。

| 源数据类型`src` | 目的寄存器类型`dst0/dst1` |
| :--- | :--- |
| `int4b_t` | `vector_int4x2_t` |
| `fp4x2_e1m2_t` | `vector_fp4x2_e1m2_t` |
| `fp4x2_e2m1_t` | `vector_fp4x2_e2m1_t` |
| `int8_t` | `vector_int8_t` |
| `uint8_t` | `vector_uint8_t` |
| `fp8_e4m3fn_t` | `vector_fp8_e4m3fn_t` |
| `fp8_e5m2_t` | `vector_fp8_e5m2_t` |
| `fp8_e8m0_t` | `vector_fp8_e8m0_t` |
| `hifloat8_t` | `vector_hifloat8_t` |
| `int16_t` | `vector_int16_t` |
| `uint16_t` | `vector_uint16_t` |
| `half` | `vector_half` |
| `bfloat16_t` | `vector_bfloat16_t` |
| `int32_t` | `vector_int32_t` |
| `uint32_t` | `vector_uint32_t` |
| `float` | `vector_float` |

## 返回值说明

无。

## 流水类型

`PIPE_V`

## 约束说明

- `src`必须为32B对齐地址，且访问范围不能超过UB有效地址上界减去`2 x VL`，否则可能导致UB越界访问。
- 在循环中使用本接口时，`offset`的取值需要保证更新后的`src`仍满足32B对齐要求。
- `offset`以元素为单位，实际地址步长需要结合源数据类型宽度进行换算。

## 调用示例

从UB中连续读取以32B对齐地址`src`为起始位置的`float`数据，并在每次调用后自动更新地址。

```cpp
for (uint16_t i = 0; i < repeat_times; i++) {
    asc_loadalign_deintlv_postupdate(reg0, reg1, src, one_repeat_size);
    ...
}
```
