# asc_squeeze_and_storeunalign

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

reg计算数据搬运接口，适用于从矢量数据寄存器中**筛选出有效元素后**按照连续非32B对齐的起始地址连续搬出到Unified Buffer（UB）的场景。

本接口将输入矢量数据寄存器src1中被掩码squeeze_mask选择的有效元素依次复制到矢量数据寄存器dst，有效元素在dst上连续排列，dst中后续非有效元素置0。若AR寄存器（用于累加已搬出的有效数据量）中累加了前序asc_squeeze_and_storeunalign指令的有效元素总数据量，则将dst中有效数据写入dst_addr + AR寄存器数据量的地址，并将本次执行的有效数据量大小（以字节为单位）累加进AR寄存器。如[调用示例](#调用示例)所示，多次连续调用本接口时，若不清空AR寄存器，可以实现自动地址偏移，不需要手动管理地址。**调用本接口后，需要调用[asc_squeeze_and_storeunalign_finalize](asc_squeeze_and_storeunalign_finalize.md)接口进行收尾。**

首次调用本接口前或不想基于AR寄存器偏移时，需要调用[asc_squeeze_and_storeunalign_init](asc_squeeze_and_storeunalign_init.md)接口清空AR寄存器中的数据。AR寄存器中的数值可以通过[asc_get_squeeze_status](../../spr/asc_get_squeeze_status.md)接口获取。

## 函数原型

```cpp
__simd_callee__ inline void asc_squeeze_and_storeunalign(__ubuf__ int8_t* dst_addr, vector_int8_t dst, vector_store_unalign& src0, vector_int8_t src1, vector_bool squeeze_mask)
__simd_callee__ inline void asc_squeeze_and_storeunalign(__ubuf__ uint8_t* dst_addr, vector_uint8_t dst, vector_store_unalign& src0, vector_uint8_t src1, vector_bool squeeze_mask)
__simd_callee__ inline void asc_squeeze_and_storeunalign(__ubuf__ int16_t* dst_addr, vector_int16_t dst, vector_store_unalign& src0, vector_int16_t src1, vector_bool squeeze_mask)
__simd_callee__ inline void asc_squeeze_and_storeunalign(__ubuf__ uint16_t* dst_addr, vector_uint16_t dst, vector_store_unalign& src0, vector_uint16_t src1, vector_bool squeeze_mask)
__simd_callee__ inline void asc_squeeze_and_storeunalign(__ubuf__ int32_t* dst_addr, vector_int32_t dst, vector_store_unalign& src0, vector_int32_t src1, vector_bool squeeze_mask)
__simd_callee__ inline void asc_squeeze_and_storeunalign(__ubuf__ uint32_t* dst_addr, vector_uint32_t dst, vector_store_unalign& src0, vector_uint32_t src1, vector_bool squeeze_mask)
__simd_callee__ inline void asc_squeeze_and_storeunalign(__ubuf__ half* dst_addr, vector_half dst, vector_store_unalign& src0, vector_half src1, vector_bool squeeze_mask)
__simd_callee__ inline void asc_squeeze_and_storeunalign(__ubuf__ float* dst_addr, vector_float dst, vector_store_unalign& src0, vector_float src1, vector_bool squeeze_mask)
__simd_callee__ inline void asc_squeeze_and_storeunalign(__ubuf__ bfloat16_t* dst_addr, vector_bfloat16_t dst, vector_store_unalign& src0, vector_bfloat16_t src1, vector_bool squeeze_mask)
__simd_callee__ inline void asc_squeeze_and_storeunalign(__ubuf__ fp8_e4m3fn_t* dst_addr, vector_fp8_e4m3fn_t dst, vector_store_unalign& src0, vector_fp8_e4m3fn_t src1, vector_bool squeeze_mask)
__simd_callee__ inline void asc_squeeze_and_storeunalign(__ubuf__ fp8_e5m2_t* dst_addr, vector_fp8_e5m2_t dst, vector_store_unalign& src0, vector_fp8_e5m2_t src1, vector_bool squeeze_mask)
__simd_callee__ inline void asc_squeeze_and_storeunalign(__ubuf__ fp8_e8m0_t* dst_addr, vector_fp8_e8m0_t dst, vector_store_unalign& src0, vector_fp8_e8m0_t src1, vector_bool squeeze_mask)
__simd_callee__ inline void asc_squeeze_and_storeunalign(__ubuf__ fp4x2_e2m1_t* dst_addr, vector_fp4x2_e2m1_t dst, vector_store_unalign& src0, vector_fp4x2_e2m1_t src1, vector_bool squeeze_mask)
__simd_callee__ inline void asc_squeeze_and_storeunalign(__ubuf__ fp4x2_e1m2_t* dst_addr, vector_fp4x2_e1m2_t dst, vector_store_unalign& src0, vector_fp4x2_e1m2_t src1, vector_bool squeeze_mask)
__simd_callee__ inline void asc_squeeze_and_storeunalign(__ubuf__ int4b_t* dst_addr, vector_int4x2_t dst, vector_store_unalign& src0, vector_int4x2_t src1, vector_bool squeeze_mask)
```

## 参数说明

**表1** 参数说明

| 参数名       | 输入/输出 | 描述        |
| --------- | ----- | ----------------- |
| dst_addr  | 输出    | 目的操作数（矢量）的起始地址。 |
| dst       | 输出    | 目的操作数（矢量数据寄存器）。 |
| src0      | 输入、输出    | 非对齐寄存器，用于保存非对齐数据，长度32B。 |
| src1      | 输入    | 源操作数（矢量数据寄存器）。 |
| squeeze_mask      | 输入    | 源操作数掩码（掩码寄存器），用于指示在计算过程中哪些元素参与计算。对应位置为1时参与计算，为0时不参与计算。 |

矢量数据寄存器和掩码寄存器的详细说明请参见[reg数据类型定义](../../defs/type/data_type_definition.md)。

## 返回值说明

无

## 流水类型

PIPE_V

## 约束说明

- 该接口中的dst_addr不需要32B对齐。
- **调用本接口后，需要调用[asc_squeeze_and_storeunalign_finalize](asc_squeeze_and_storeunalign_finalize.md)接口进行收尾。**
- 首次调用本接口前或不想基于AR寄存器偏移时，需要调用[asc_squeeze_and_storeunalign_init](asc_squeeze_and_storeunalign_init.md)接口清空AR寄存器中的数据。
- 本接口搬出长度取决于掩码寄存器配置的有效数据量，用户需要保证不越过UB可用容量上限，否则触发写越界异常。

## 调用示例

```cpp
__simd_vf__ inline void vf_squeeze_and_storeunalign(__ubuf__ half* dst_addr, __ubuf__ half* src_addr, uint32_t one_repeat_size, uint16_t repeat_time)
{
    vector_half dst;
    vector_half src;
    vector_store_unalign ureg;
    vector_bool mask = asc_create_mask_b16(PAT_M4);
    asc_squeeze_and_storeunalign_init();
    for (uint16_t i = 0; i < repeat_time; ++i) {
        asc_loadalign_postupdate(src, src_addr, one_repeat_size);
        asc_squeeze_and_storeunalign(dst_addr, dst, ureg, src, mask);
    }
    asc_squeeze_and_storeunalign_finalize(dst_addr, ureg);
}
```
