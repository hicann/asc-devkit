# asc_storeunalign_postupdate（废弃）

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

头文件路径为：`"c_api/reg_compute/store/storeunalign.h"`。

**该接口不指定存储偏移量的原型已废弃，请使用[asc_squeeze_and_storeunalign](../reg_compute/reg_permute_sel/asc_squeeze_and_storeunalign.md)实现此功能。掩码寄存器搬出模式的b8数据类型原型已废弃，请使用b16或b32数据类型原型。**

reg计算数据搬运接口，适用于从矢量数据寄存器或掩码寄存器连续非32B对齐的起始地址连续搬出到Unified Buffer（UB）的场景。

该接口每调用一次接口会更新目的操作数在UB上的地址。

## 函数原型

- 不指定存储偏移量

    ```cpp
    __simd_callee__ inline void asc_storeunalign_postupdate(__ubuf__ int8_t* dst, vector_store_unalign& src0, vector_int8_t src1)
    __simd_callee__ inline void asc_storeunalign_postupdate(__ubuf__ uint8_t* dst, vector_store_unalign& src0, vector_uint8_t src1)
    __simd_callee__ inline void asc_storeunalign_postupdate(__ubuf__ int16_t* dst, vector_store_unalign& src0, vector_int16_t src1)
    __simd_callee__ inline void asc_storeunalign_postupdate(__ubuf__ uint16_t* dst, vector_store_unalign& src0, vector_uint16_t src1)
    __simd_callee__ inline void asc_storeunalign_postupdate(__ubuf__ int32_t* dst, vector_store_unalign& src0, vector_int32_t src1)
    __simd_callee__ inline void asc_storeunalign_postupdate(__ubuf__ uint32_t* dst, vector_store_unalign& src0, vector_uint32_t src1)
    __simd_callee__ inline void asc_storeunalign_postupdate(__ubuf__ int64_t* dst, vector_store_unalign& src0, vector_int64_t src1)
    __simd_callee__ inline void asc_storeunalign_postupdate(__ubuf__ half* dst, vector_store_unalign& src0, vector_half src1)
    __simd_callee__ inline void asc_storeunalign_postupdate(__ubuf__ float* dst, vector_store_unalign& src0, vector_float src1)
    __simd_callee__ inline void asc_storeunalign_postupdate(__ubuf__ bfloat16_t* dst, vector_store_unalign& src0, vector_bfloat16_t src1)
    __simd_callee__ inline void asc_storeunalign_postupdate(__ubuf__ fp8_e4m3fn_t* dst, vector_store_unalign& src0, vector_fp8_e4m3fn_t src1)
    __simd_callee__ inline void asc_storeunalign_postupdate(__ubuf__ fp8_e5m2_t* dst, vector_store_unalign& src0, vector_fp8_e5m2_t src1)
    __simd_callee__ inline void asc_storeunalign_postupdate(__ubuf__ fp8_e8m0_t* dst, vector_store_unalign& src0, vector_fp8_e8m0_t src1)
    __simd_callee__ inline void asc_storeunalign_postupdate(__ubuf__ fp4x2_e2m1_t* dst, vector_store_unalign& src0, vector_fp4x2_e2m1_t src1)
    __simd_callee__ inline void asc_storeunalign_postupdate(__ubuf__ fp4x2_e1m2_t* dst, vector_store_unalign& src0, vector_fp4x2_e1m2_t src1)
    __simd_callee__ inline void asc_storeunalign_postupdate(__ubuf__ int4b_t* dst, vector_store_unalign& src0, vector_int4x2_t src1)
    ```

- 掩码寄存器搬出模式（b8数据类型）

    ```cpp
    __simd_callee__ inline void asc_storeunalign_postupdate(__ubuf__ uint8_t*& dst, vector_store_unalign& src0, vector_bool src1)
    ```

## 参数说明

- 不指定存储偏移量

    **表1** 参数说明

    | 参数名  | 输入/输出 | 描述 |
    | :----- | :------- | :------- |
    | dst | 输出 | 目的操作数（矢量）的起始地址。 |
    | src0 | 输入/输出 | 非对齐寄存器，用于保存非对齐数据，长度为32B。 |
    | src1 | 输入 | 源操作数（矢量数据寄存器或掩码寄存器）。 |

- 掩码寄存器搬出模式（b8数据类型）

    **表2** 参数说明

    | 参数名 | 输入/输出 | 描述 |
    | :----- | :------- | :------- |
    | dst | 输入/输出 | 目的操作数在UB中的起始地址，数据类型为`uint8_t`。搬出后，`dst`自动更新至本次搬出的结束地址。 |
    | src0 | 输入/输出 | 非对齐寄存器，用于保存非对齐数据，长度为32B。 |
    | src1 | 输入 | 源操作数（掩码寄存器），类型为`vector_bool`。 |

矢量数据寄存器、非对齐寄存器和地址寄存器的详细说明请参见[reg数据类型定义](../defs/type/data_type_definition.md)。

## 返回值说明

无

## 流水类型

PIPE_V

## 约束说明

- 该接口中的dst不需要32B对齐。
- 本接口应该配合[asc_storeunalign_post](asc_storeunalign_post_deprecated.md)接口一起使用。
- 掩码寄存器搬出模式的b8数据类型原型已废弃，请使用b16或b32数据类型原型。

## 调用示例

- 不指定存储偏移量

    ```cpp
    // dst为ub地址，起始地址为8,非32B对齐
    vector_store_unalign ureg;
    vector_uint32_t src;
    asc_storeunalign_postupdate(dst, ureg, src);
    asc_storeunalign_post(dst, ureg);
    ```
