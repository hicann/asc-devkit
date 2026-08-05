# asc_loadalign_deintlv

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

`asc_loadalign_deintlv`用于从UB中读取以32B对齐地址为起始位置的连续`2 x VL`长度数据。数据按元素类型宽度进行解释，在Load过程中完成解交织后，分别写入两个目的矢量数据寄存器。

以`float`数据类型、Ascend 950PR/Ascend 950DT产品为例，若`VL = 256B`，则单次操作覆盖64个`float`元素。调用`asc_loadalign_deintlv`后，数据从UB到Reg的排布如下图所示。

![](../../figures/asc_loadalign_deintlv.png)

> 提示：
>
> 使用本接口需要包含头文件`reg_load.h`。

## 函数原型

`asc_loadalign_deintlv`按源地址的寻址方式分为以下三类接口：[基址寻址接口](#base-addressing-api)、[标量偏移寻址接口](#base-scalar-offset-api)、[地址寄存器偏移寻址接口](#base-register-indexed-api)。支持的数据类型请参见[数据类型](#data-type)章节。

### 基址寻址接口<a name="base-addressing-api"></a>

```cpp
__simd_callee__ inline void asc_loadalign_deintlv(vector_int4x2_t& dst0, vector_int4x2_t& dst1, __ubuf__ int4b_t* src)
__simd_callee__ inline void asc_loadalign_deintlv(vector_int8_t& dst0, vector_int8_t& dst1, __ubuf__ int8_t* src)
__simd_callee__ inline void asc_loadalign_deintlv(vector_uint8_t& dst0, vector_uint8_t& dst1, __ubuf__ uint8_t* src)
__simd_callee__ inline void asc_loadalign_deintlv(vector_fp4x2_e2m1_t& dst0, vector_fp4x2_e2m1_t& dst1, __ubuf__ fp4x2_e2m1_t* src)
__simd_callee__ inline void asc_loadalign_deintlv(vector_fp4x2_e1m2_t& dst0, vector_fp4x2_e1m2_t& dst1, __ubuf__ fp4x2_e1m2_t* src)
__simd_callee__ inline void asc_loadalign_deintlv(vector_hifloat8_t& dst0, vector_hifloat8_t& dst1, __ubuf__ hifloat8_t* src)
__simd_callee__ inline void asc_loadalign_deintlv(vector_fp8_e8m0_t& dst0, vector_fp8_e8m0_t& dst1, __ubuf__ fp8_e8m0_t* src)
__simd_callee__ inline void asc_loadalign_deintlv(vector_fp8_e5m2_t& dst0, vector_fp8_e5m2_t& dst1, __ubuf__ fp8_e5m2_t* src)
__simd_callee__ inline void asc_loadalign_deintlv(vector_fp8_e4m3fn_t& dst0, vector_fp8_e4m3fn_t& dst1, __ubuf__ fp8_e4m3fn_t* src)
__simd_callee__ inline void asc_loadalign_deintlv(vector_int16_t& dst0, vector_int16_t& dst1, __ubuf__ int16_t* src)
__simd_callee__ inline void asc_loadalign_deintlv(vector_uint16_t& dst0, vector_uint16_t& dst1, __ubuf__ uint16_t* src)
__simd_callee__ inline void asc_loadalign_deintlv(vector_half& dst0, vector_half& dst1, __ubuf__ half* src)
__simd_callee__ inline void asc_loadalign_deintlv(vector_bfloat16_t& dst0, vector_bfloat16_t& dst1, __ubuf__ bfloat16_t* src)
__simd_callee__ inline void asc_loadalign_deintlv(vector_int32_t& dst0, vector_int32_t& dst1, __ubuf__ int32_t* src)
__simd_callee__ inline void asc_loadalign_deintlv(vector_uint32_t& dst0, vector_uint32_t& dst1, __ubuf__ uint32_t* src)
__simd_callee__ inline void asc_loadalign_deintlv(vector_float& dst0, vector_float& dst1, __ubuf__ float* src)
```

### 标量偏移寻址接口<a name="base-scalar-offset-api"></a>

源地址通过`src + offset`计算得到，且计算结果必须满足32B对齐要求。`offset`的单位为元素个数，实际地址偏移字节数为：`offset * sizeof(data_type)`。

```cpp
__simd_callee__ inline void asc_loadalign_deintlv(vector_int8_t& dst0, vector_int8_t& dst1, __ubuf__ int8_t* src, int32_t offset)
__simd_callee__ inline void asc_loadalign_deintlv(vector_uint8_t& dst0, vector_uint8_t& dst1, __ubuf__ uint8_t* src, int32_t offset)
__simd_callee__ inline void asc_loadalign_deintlv(vector_fp4x2_e2m1_t& dst0, vector_fp4x2_e2m1_t& dst1, __ubuf__ fp4x2_e2m1_t* src, int32_t offset)
__simd_callee__ inline void asc_loadalign_deintlv(vector_fp4x2_e1m2_t& dst0, vector_fp4x2_e1m2_t& dst1, __ubuf__ fp4x2_e1m2_t* src, int32_t offset)
__simd_callee__ inline void asc_loadalign_deintlv(vector_fp8_e8m0_t& dst0, vector_fp8_e8m0_t& dst1, __ubuf__ fp8_e8m0_t* src, int32_t offset)
__simd_callee__ inline void asc_loadalign_deintlv(vector_fp8_e5m2_t& dst0, vector_fp8_e5m2_t& dst1, __ubuf__ fp8_e5m2_t* src, int32_t offset)
__simd_callee__ inline void asc_loadalign_deintlv(vector_fp8_e4m3fn_t& dst0, vector_fp8_e4m3fn_t& dst1, __ubuf__ fp8_e4m3fn_t* src, int32_t offset)
__simd_callee__ inline void asc_loadalign_deintlv(vector_hifloat8_t& dst0, vector_hifloat8_t& dst1, __ubuf__ hifloat8_t* src, int32_t offset)
__simd_callee__ inline void asc_loadalign_deintlv(vector_int16_t& dst0, vector_int16_t& dst1, __ubuf__ int16_t* src, int32_t offset)
__simd_callee__ inline void asc_loadalign_deintlv(vector_uint16_t& dst0, vector_uint16_t& dst1, __ubuf__ uint16_t* src, int32_t offset)
__simd_callee__ inline void asc_loadalign_deintlv(vector_half& dst0, vector_half& dst1, __ubuf__ half* src, int32_t offset)
__simd_callee__ inline void asc_loadalign_deintlv(vector_bfloat16_t& dst0, vector_bfloat16_t& dst1, __ubuf__ bfloat16_t* src, int32_t offset)
__simd_callee__ inline void asc_loadalign_deintlv(vector_int32_t& dst0, vector_int32_t& dst1, __ubuf__ int32_t* src, int32_t offset)
__simd_callee__ inline void asc_loadalign_deintlv(vector_uint32_t& dst0, vector_uint32_t& dst1, __ubuf__ uint32_t* src, int32_t offset)
__simd_callee__ inline void asc_loadalign_deintlv(vector_float& dst0, vector_float& dst1, __ubuf__ float* src, int32_t offset)
__simd_callee__ inline void asc_loadalign_deintlv(vector_int4x2_t& dst0, vector_int4x2_t& dst1, __ubuf__ int4b_t* src, int32_t offset)
```

### 地址寄存器偏移寻址接口<a name="base-register-indexed-api"></a>

该类接口通过地址寄存器`addr_reg offset`生成实际访问地址。地址寄存器通常在循环场景中使用，需配合[`asc_update_addr_reg`](../reg_data_types/asc_update_addr_reg.md)完成初始化。实际访问地址必须满足32B对齐要求。

```cpp
__simd_callee__ inline void asc_loadalign_deintlv(vector_fp4x2_e1m2_t& dst0, vector_fp4x2_e1m2_t& dst1, __ubuf__ fp4x2_e1m2_t* src, addr_reg offset)
__simd_callee__ inline void asc_loadalign_deintlv(vector_fp4x2_e2m1_t& dst0, vector_fp4x2_e2m1_t& dst1, __ubuf__ fp4x2_e2m1_t* src, addr_reg offset)
__simd_callee__ inline void asc_loadalign_deintlv(vector_uint8_t& dst0, vector_uint8_t& dst1, __ubuf__ uint8_t* src, addr_reg offset)
__simd_callee__ inline void asc_loadalign_deintlv(vector_int8_t& dst0, vector_int8_t& dst1, __ubuf__ int8_t* src, addr_reg offset)
__simd_callee__ inline void asc_loadalign_deintlv(vector_fp8_e4m3fn_t& dst0, vector_fp8_e4m3fn_t& dst1, __ubuf__ fp8_e4m3fn_t* src, addr_reg offset)
__simd_callee__ inline void asc_loadalign_deintlv(vector_fp8_e5m2_t& dst0, vector_fp8_e5m2_t& dst1, __ubuf__ fp8_e5m2_t* src, addr_reg offset)
__simd_callee__ inline void asc_loadalign_deintlv(vector_fp8_e8m0_t& dst0, vector_fp8_e8m0_t& dst1, __ubuf__ fp8_e8m0_t* src, addr_reg offset)
__simd_callee__ inline void asc_loadalign_deintlv(vector_hifloat8_t& dst0, vector_hifloat8_t& dst1, __ubuf__ hifloat8_t* src, addr_reg offset)
__simd_callee__ inline void asc_loadalign_deintlv(vector_uint16_t& dst0, vector_uint16_t& dst1, __ubuf__ uint16_t* src, addr_reg offset)
__simd_callee__ inline void asc_loadalign_deintlv(vector_int16_t& dst0, vector_int16_t& dst1, __ubuf__ int16_t* src, addr_reg offset)
__simd_callee__ inline void asc_loadalign_deintlv(vector_bfloat16_t& dst0, vector_bfloat16_t& dst1, __ubuf__ bfloat16_t* src, addr_reg offset)
__simd_callee__ inline void asc_loadalign_deintlv(vector_half& dst0, vector_half& dst1, __ubuf__ half* src, addr_reg offset)
__simd_callee__ inline void asc_loadalign_deintlv(vector_uint32_t& dst0, vector_uint32_t& dst1, __ubuf__ uint32_t* src, addr_reg offset)
__simd_callee__ inline void asc_loadalign_deintlv(vector_int32_t& dst0, vector_int32_t& dst1, __ubuf__ int32_t* src, addr_reg offset)
__simd_callee__ inline void asc_loadalign_deintlv(vector_float& dst0, vector_float& dst1, __ubuf__ float* src, addr_reg offset)
__simd_callee__ inline void asc_loadalign_deintlv(vector_int4x2_t& dst0, vector_int4x2_t& dst1, __ubuf__ int4b_t* src, addr_reg offset)
```

## 参数说明

**表1** 参数说明

| 参数名 | 输入/输出 | 说明 |
| :--- | :--- | :--- |
| dst0 | 输出 | 第一个目的向量寄存器。 |
| dst1 | 输出 | 第二个目的向量寄存器。 |
| src | 输入 | 源数据在UB中的起始地址。对于不同寻址接口，`src`分别表示基址或参与地址计算的基地址。 |
| offset | 输入 | 地址偏移量。对于标量偏移寻址接口，单位为元素个数；对于地址寄存器偏移寻址接口，表示地址寄存器。 |

寄存器类型的详细说明请参见[reg数据类型定义](../reg_data_types/data_type_definition.md)。

## 数据类型<a name="data-type"></a>

`asc_loadalign_deintlv`支持的数据类型如下表所示。三类寻址接口均支持表中所列数据类型。

**表2** 数据类型

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

- 对于[基址寻址接口](#base-addressing-api)，`src`必须为32B对齐地址，且访问范围不能超过UB有效地址上界减去`2 x VL`。
- 对于[标量偏移寻址接口](#base-scalar-offset-api)和[地址寄存器偏移寻址接口](#base-register-indexed-api)，实际访问地址必须为32B对齐，且访问范围不能超过UB有效地址上界减去`2 x VL`。
- 使用地址寄存器偏移寻址接口时，需要先通过[`asc_update_addr_reg`](../reg_data_types/asc_update_addr_reg.md)完成地址寄存器初始化。

## 调用示例

从UB中连续读取以32B对齐地址`src`为起始位置的`float`数据。

### 基址寻址接口

```cpp
for (uint16_t i = 0; i < repeat_times; i++) {
    asc_loadalign_deintlv(reg0, reg1, src + i * one_repeat_size);
    ...
}
```

### 标量偏移寻址接口

```cpp
// 方式1：通过offset控制偏移
for (uint16_t i = 0; i < repeat_times; i++) {
    asc_loadalign_deintlv(reg0, reg1, src, i * one_repeat_size);
    ...
}

// 方式2：通过src控制偏移
for (uint16_t i = 0; i < repeat_times; i++) {
    asc_loadalign_deintlv(reg0, reg1, src + i * one_repeat_size, 0);
    ...
}
```

### 地址寄存器偏移寻址接口

```cpp
for (uint16_t i = 0; i < repeat_times; i++) {
    addr_reg addr_reg = asc_update_addr_reg_b32(one_repeat_size);
    asc_loadalign_deintlv(reg0, reg1, src, addr_reg);
    ...
}
```
