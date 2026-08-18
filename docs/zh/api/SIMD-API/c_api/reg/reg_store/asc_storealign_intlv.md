# asc_storealign_intlv

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

从矢量数据寄存器交错搬出到Unified Buffer（UB）的接口，忽略mask，将src0、src1中的元素交错存储于dst中，存储长度为VL*2。支持数据类型为b8、b16、b32。

## 函数原型

- 偏移固定传入0，由用户自行更新目的操作数的地址
  ```cpp
  __simd_callee__ inline void asc_storealign_intlv(__ubuf__ int8_t* dst_align32b, vector_int8_t src0, vector_int8_t src1)
  __simd_callee__ inline void asc_storealign_intlv(__ubuf__ uint8_t* dst_align32b, vector_uint8_t src0, vector_uint8_t src1)
  __simd_callee__ inline void asc_storealign_intlv(__ubuf__ int16_t* dst_align32b, vector_int16_t src0, vector_int16_t src1)
  __simd_callee__ inline void asc_storealign_intlv(__ubuf__ uint16_t* dst_align32b, vector_uint16_t src0, vector_uint16_t src1)
  __simd_callee__ inline void asc_storealign_intlv(__ubuf__ int32_t* dst_align32b, vector_int32_t src0, vector_int32_t src1)
  __simd_callee__ inline void asc_storealign_intlv(__ubuf__ uint32_t* dst_align32b, vector_uint32_t src0, vector_uint32_t src1)
  __simd_callee__ inline void asc_storealign_intlv(__ubuf__ half* dst_align32b, vector_half src0, vector_half src1)
  __simd_callee__ inline void asc_storealign_intlv(__ubuf__ bfloat16_t* dst_align32b, vector_bfloat16_t src0, vector_bfloat16_t src1)
  __simd_callee__ inline void asc_storealign_intlv(__ubuf__ fp8_e4m3fn_t* dst_align32b, vector_fp8_e4m3fn_t src0, vector_fp8_e4m3fn_t src1)
  __simd_callee__ inline void asc_storealign_intlv(__ubuf__ hifloat8_t* dst_align32b, vector_hifloat8_t src0, vector_hifloat8_t src1)
  __simd_callee__ inline void asc_storealign_intlv(__ubuf__ fp8_e5m2_t* dst_align32b, vector_fp8_e5m2_t src0, vector_fp8_e5m2_t src1)
  __simd_callee__ inline void asc_storealign_intlv(__ubuf__ fp8_e8m0_t* dst_align32b, vector_fp8_e8m0_t src0, vector_fp8_e8m0_t src1)
  __simd_callee__ inline void asc_storealign_intlv(__ubuf__ fp4x2_e2m1_t* dst_align32b, vector_fp4x2_e2m1_t src0, vector_fp4x2_e2m1_t src1)
  __simd_callee__ inline void asc_storealign_intlv(__ubuf__ fp4x2_e1m2_t* dst_align32b, vector_fp4x2_e1m2_t src0, vector_fp4x2_e1m2_t src1)
  __simd_callee__ inline void asc_storealign_intlv(__ubuf__ int4b_t* dst_align32b, vector_int4x2_t src0, vector_int4x2_t src1)
  ```
- 通过int32_t传入偏移，用户可以选择更新偏移或者更新目的操作数的地址
  ```cpp
  __simd_callee__ inline void asc_storealign_intlv(__ubuf__ int8_t* dst_align32b, vector_int8_t src0, vector_int8_t src1, int32_t offset)
  __simd_callee__ inline void asc_storealign_intlv(__ubuf__ uint8_t* dst_align32b, vector_uint8_t src0, vector_uint8_t src1, int32_t offset)
  __simd_callee__ inline void asc_storealign_intlv(__ubuf__ int16_t* dst_align32b, vector_int16_t src0, vector_int16_t src1, int32_t offset)
  __simd_callee__ inline void asc_storealign_intlv(__ubuf__ uint16_t* dst_align32b, vector_uint16_t src0, vector_uint16_t src1, int32_t offset)
  __simd_callee__ inline void asc_storealign_intlv(__ubuf__ int32_t* dst_align32b, vector_int32_t src0, vector_int32_t src1, int32_t offset)
  __simd_callee__ inline void asc_storealign_intlv(__ubuf__ uint32_t* dst_align32b, vector_uint32_t src0, vector_uint32_t src1, int32_t offset)
  __simd_callee__ inline void asc_storealign_intlv(__ubuf__ half* dst_align32b, vector_half src0, vector_half src1, int32_t offset)
  __simd_callee__ inline void asc_storealign_intlv(__ubuf__ bfloat16_t* dst_align32b, vector_bfloat16_t src0, vector_bfloat16_t src1, int32_t offset)
  __simd_callee__ inline void asc_storealign_intlv(__ubuf__ fp8_e4m3fn_t* dst_align32b, vector_fp8_e4m3fn_t src0, vector_fp8_e4m3fn_t src1, int32_t offset)
  __simd_callee__ inline void asc_storealign_intlv(__ubuf__ hifloat8_t* dst_align32b, vector_hifloat8_t src0, vector_hifloat8_t src1, int32_t offset)
  __simd_callee__ inline void asc_storealign_intlv(__ubuf__ fp8_e5m2_t* dst_align32b, vector_fp8_e5m2_t src0, vector_fp8_e5m2_t src1, int32_t offset)
  __simd_callee__ inline void asc_storealign_intlv(__ubuf__ fp8_e8m0_t* dst_align32b, vector_fp8_e8m0_t src0, vector_fp8_e8m0_t src1, int32_t offset)
  __simd_callee__ inline void asc_storealign_intlv(__ubuf__ fp4x2_e2m1_t* dst_align32b, vector_fp4x2_e2m1_t src0, vector_fp4x2_e2m1_t src1, int32_t offset)
  __simd_callee__ inline void asc_storealign_intlv(__ubuf__ fp4x2_e1m2_t* dst_align32b, vector_fp4x2_e1m2_t src0, vector_fp4x2_e1m2_t src1, int32_t offset)
  __simd_callee__ inline void asc_storealign_intlv(__ubuf__ int4b_t* dst_align32b, vector_int4x2_t src0, vector_int4x2_t src1, int32_t offset)
  ```
- 通过addr_reg地址寄存器传入偏移
  ```cpp
  __simd_callee__ inline void asc_storealign_intlv(__ubuf__ int8_t* dst_align32b, vector_int8_t src0, vector_int8_t src1, addr_reg offset)
  __simd_callee__ inline void asc_storealign_intlv(__ubuf__ uint8_t* dst_align32b, vector_uint8_t src0, vector_uint8_t src1, addr_reg offset)
  __simd_callee__ inline void asc_storealign_intlv(__ubuf__ int16_t* dst_align32b, vector_int16_t src0, vector_int16_t src1, addr_reg offset)
  __simd_callee__ inline void asc_storealign_intlv(__ubuf__ uint16_t* dst_align32b, vector_uint16_t src0, vector_uint16_t src1, addr_reg offset)
  __simd_callee__ inline void asc_storealign_intlv(__ubuf__ int32_t* dst_align32b, vector_int32_t src0, vector_int32_t src1, addr_reg offset)
  __simd_callee__ inline void asc_storealign_intlv(__ubuf__ uint32_t* dst_align32b, vector_uint32_t src0, vector_uint32_t src1, addr_reg offset)
  __simd_callee__ inline void asc_storealign_intlv(__ubuf__ half* dst_align32b, vector_half src0, vector_half src1, addr_reg offset)
  __simd_callee__ inline void asc_storealign_intlv(__ubuf__ bfloat16_t* dst_align32b, vector_bfloat16_t src0, vector_bfloat16_t src1, addr_reg offset)
  __simd_callee__ inline void asc_storealign_intlv(__ubuf__ fp8_e4m3fn_t* dst_align32b, vector_fp8_e4m3fn_t src0, vector_fp8_e4m3fn_t src1, addr_reg offset)
  __simd_callee__ inline void asc_storealign_intlv(__ubuf__ hifloat8_t* dst_align32b, vector_hifloat8_t src0, vector_hifloat8_t src1, addr_reg offset)
  __simd_callee__ inline void asc_storealign_intlv(__ubuf__ fp8_e5m2_t* dst_align32b, vector_fp8_e5m2_t src0, vector_fp8_e5m2_t src1, addr_reg offset)
  __simd_callee__ inline void asc_storealign_intlv(__ubuf__ fp8_e8m0_t* dst_align32b, vector_fp8_e8m0_t src0, vector_fp8_e8m0_t src1, addr_reg offset)
  __simd_callee__ inline void asc_storealign_intlv(__ubuf__ fp4x2_e2m1_t* dst_align32b, vector_fp4x2_e2m1_t src0, vector_fp4x2_e2m1_t src1, addr_reg offset)
  __simd_callee__ inline void asc_storealign_intlv(__ubuf__ fp4x2_e1m2_t* dst_align32b, vector_fp4x2_e1m2_t src0, vector_fp4x2_e1m2_t src1, addr_reg offset)
  __simd_callee__ inline void asc_storealign_intlv(__ubuf__ int4b_t* dst_align32b, vector_int4x2_t src0, vector_int4x2_t src1, addr_reg offset)
  ```

## 参数说明

**表1** 参数说明

| 参数名  | 输入/输出 | 描述 |
| :----- | :------- | :------- |
| dst_align32b | 输出 | 目的操作数（矢量）的起始地址。 |
| src0 | 输入 | 源操作数0（矢量数据寄存器）。 |
| src1 | 输入 | 源操作数1（矢量数据寄存器）。 |
| offset | 输入 | 偏移量。 |

矢量数据寄存器的详细说明请参见[reg数据类型定义](../reg_data_types/data_type_definition.md)。

## 返回值说明

无

## 流水类型

PIPE_V

## 约束说明

无

## 调用示例

```cpp
__ubuf__ half* dst_align32b = (__ubuf__ half*)asc_get_phy_buf_addr(0);
vector_half src0, src1;
asc_storealign_intlv(dst_align32b, src0, src1);
```
