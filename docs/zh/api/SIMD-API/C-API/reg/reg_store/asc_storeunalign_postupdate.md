# asc_storeunalign_postupdate

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

reg计算数据搬运接口，适用于从矢量数据寄存器或掩码寄存器连续非32B对齐的起始地址连续搬出到UB的场景。

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
    __simd_callee__ inline void asc_storeunalign_postupdate(__ubuf__ uint16_t*& dst, vector_store_unalign& src0, vector_bool src1)
    __simd_callee__ inline void asc_storeunalign_postupdate(__ubuf__ uint32_t*& dst, vector_store_unalign& src0, vector_bool src1)
    __simd_callee__ inline void asc_storeunalign_postupdate(__ubuf__ uint8_t*& dst, vector_store_unalign& src0, vector_bool src1)
    ```

- 使用uint32_t作为存储偏移量

    ```cpp
    __simd_callee__ inline void asc_storeunalign_postupdate(__ubuf__ int8_t*& dst, vector_store_unalign& src0, vector_int8_t src1, uint32_t count)
    __simd_callee__ inline void asc_storeunalign_postupdate(__ubuf__ uint8_t*& dst, vector_store_unalign& src0, vector_uint8_t src1, uint32_t count)
    __simd_callee__ inline void asc_storeunalign_postupdate(__ubuf__ int16_t*& dst, vector_store_unalign& src0, vector_int16_t src1, uint32_t count)
    __simd_callee__ inline void asc_storeunalign_postupdate(__ubuf__ uint16_t*& dst, vector_store_unalign& src0, vector_uint16_t src1, uint32_t count)
    __simd_callee__ inline void asc_storeunalign_postupdate(__ubuf__ int32_t*& dst, vector_store_unalign& src0, vector_int32_t src1, uint32_t count)
    __simd_callee__ inline void asc_storeunalign_postupdate(__ubuf__ uint32_t*& dst, vector_store_unalign& src0, vector_uint32_t src1, uint32_t count) 
    __simd_callee__ inline void asc_storeunalign_postupdate(__ubuf__ int64_t*& dst, vector_store_unalign& src0, vector_int64_t src1, uint32_t count)
    __simd_callee__ inline void asc_storeunalign_postupdate(__ubuf__ half*& dst, vector_store_unalign& src0, vector_half src1, uint32_t count)
    __simd_callee__ inline void asc_storeunalign_postupdate(__ubuf__ float*& dst, vector_store_unalign& src0, vector_float src1, uint32_t count)
    __simd_callee__ inline void asc_storeunalign_postupdate(__ubuf__ bfloat16_t*& dst, vector_store_unalign& src0, vector_bfloat16_t src1, uint32_t count)
    __simd_callee__ inline void asc_storeunalign_postupdate(__ubuf__ fp8_e4m3fn_t*& dst, vector_store_unalign& src0, vector_fp8_e4m3fn_t src1, uint32_t count)
    __simd_callee__ inline void asc_storeunalign_postupdate(__ubuf__ hifloat8_t*& dst, vector_store_unalign& src0, vector_hifloat8_t src1, uint32_t count)
    __simd_callee__ inline void asc_storeunalign_postupdate(__ubuf__ fp8_e5m2_t*& dst, vector_store_unalign& src0, vector_fp8_e5m2_t src1, uint32_t count)
    __simd_callee__ inline void asc_storeunalign_postupdate(__ubuf__ fp8_e8m0_t*& dst, vector_store_unalign& src0, vector_fp8_e8m0_t src1, uint32_t count)
    __simd_callee__ inline void asc_storeunalign_postupdate(__ubuf__ fp4x2_e2m1_t*& dst, vector_store_unalign& src0, vector_fp4x2_e2m1_t src1, uint32_t count)
    __simd_callee__ inline void asc_storeunalign_postupdate(__ubuf__ fp4x2_e1m2_t*& dst, vector_store_unalign& src0, vector_fp4x2_e1m2_t src1, uint32_t count)
    __simd_callee__ inline void asc_storeunalign_postupdate(__ubuf__ int4b_t*& dst, vector_store_unalign& src0, vector_int4x2_t src1, uint32_t count)
    ```

- 使用addr_reg作为存储偏移量

    ```cpp
    __simd_callee__ inline void asc_storeunalign_postupdate(__ubuf__ int8_t* dst, vector_store_unalign& src0, vector_int8_t src1, addr_reg& count)
    __simd_callee__ inline void asc_storeunalign_postupdate(__ubuf__ uint8_t* dst, vector_store_unalign& src0, vector_uint8_t src1, addr_reg& count)
    __simd_callee__ inline void asc_storeunalign_postupdate(__ubuf__ int16_t* dst, vector_store_unalign& src0, vector_int16_t src1, addr_reg& count)
    __simd_callee__ inline void asc_storeunalign_postupdate(__ubuf__ uint16_t* dst, vector_store_unalign& src0, vector_uint16_t src1, addr_reg& count)
    __simd_callee__ inline void asc_storeunalign_postupdate(__ubuf__ int32_t* dst, vector_store_unalign& src0, vector_int32_t src1, addr_reg& count)
    __simd_callee__ inline void asc_storeunalign_postupdate(__ubuf__ uint32_t* dst, vector_store_unalign& src0, vector_uint32_t src1, addr_reg& count)
    __simd_callee__ inline void asc_storeunalign_postupdate(__ubuf__ int64_t* dst, vector_store_unalign& src0, vector_int64_t src1, addr_reg& count)
    __simd_callee__ inline void asc_storeunalign_postupdate(__ubuf__ half* dst, vector_store_unalign& src0, vector_half src1, addr_reg& count)
    __simd_callee__ inline void asc_storeunalign_postupdate(__ubuf__ float* dst, vector_store_unalign& src0, vector_float src1, addr_reg& count)
    __simd_callee__ inline void asc_storeunalign_postupdate(__ubuf__ bfloat16_t* dst, vector_store_unalign& src0, vector_bfloat16_t src1, addr_reg& count)
    __simd_callee__ inline void asc_storeunalign_postupdate(__ubuf__ fp8_e4m3fn_t* dst, vector_store_unalign& src0, vector_fp8_e4m3fn_t src1, addr_reg& count)
    __simd_callee__ inline void asc_storeunalign_postupdate(__ubuf__ hifloat8_t* dst, vector_store_unalign& src0, vector_hifloat8_t src1, addr_reg& count)
    __simd_callee__ inline void asc_storeunalign_postupdate(__ubuf__ fp8_e5m2_t* dst, vector_store_unalign& src0, vector_fp8_e5m2_t src1, addr_reg& count)
    __simd_callee__ inline void asc_storeunalign_postupdate(__ubuf__ fp8_e8m0_t* dst, vector_store_unalign& src0, vector_fp8_e8m0_t src1, addr_reg& count)
    __simd_callee__ inline void asc_storeunalign_postupdate(__ubuf__ fp4x2_e2m1_t* dst, vector_store_unalign& src0, vector_fp4x2_e2m1_t src1, addr_reg& count)
    __simd_callee__ inline void asc_storeunalign_postupdate(__ubuf__ fp4x2_e1m2_t* dst, vector_store_unalign& src0, vector_fp4x2_e1m2_t src1, addr_reg& count)
    __simd_callee__ inline void asc_storeunalign_postupdate(__ubuf__ int4b_t* dst, vector_store_unalign& src0, vector_int4x2_t src1, addr_reg& count)
    ```

## 参数说明

- 不指定存储偏移量

    | 参数名  | 输入/输出 | 描述 |
    | :----- | :------- | :------- |
    | dst | 输出 | 目的操作数（矢量）的起始地址。 |
    | src0 | 输入/输出 | 非对齐寄存器，用于保存非对齐数据，长度32B。 |
    | src1 | 输入 | 源操作数（矢量数据寄存器或掩码寄存器）。 |

- 使用uint32_t作为存储偏移量

    | 参数名  | 输入/输出 | 描述 |
    | :----- | :------- | :------- |
    | dst | 输出 | 目的操作数（矢量）的起始地址。 |
    | src0 | 输入/输出 | 非对齐寄存器，用于保存非对齐数据，长度32B。 |
    | src1 | 输入 | 源操作数（矢量数据寄存器）。 |
    | count | 输入 | 存储搬运的个数。 |

- 使用addr_reg作为存储偏移量

    | 参数名  | 输入/输出 | 描述 |
    | :----- | :------- | :------- |
    | dst | 输出 | 目的操作数（矢量）的起始地址。 |
    | src0 | 输入/输出 | 非对齐寄存器，用于保存非对齐数据，长度32B。 |
    | src1 | 输入 | 源操作数（矢量数据寄存器）。 |
    | count | 输入 | 存储搬运的个数（地址寄存器）。 |

矢量数据寄存器、非对齐寄存器和地址寄存器的详细说明请参见[data_type_definition.md](../reg_data_types/data_type_definition.md)。

## 返回值说明

无

## 流水类型

PIPE_V

## 约束说明

- 该接口中的dst不需要32B对齐。
- 使用uint32_t作为存储偏移量时，本接口应与[asc_storeunalign_post_postupdate](asc_storeunalign_post_postupdate.md)接口配合使用。asc_storeunalign_post_postupdate接口用于处理该场景下非对齐搬出的尾块。
- 使用addr_reg作为存储偏移量时，本接口应与[asc_storeunalign_post](asc_storeunalign_post.md)接口配合使用。asc_storeunalign_post接口用于处理该场景下非对齐搬出的尾块。

## 调用示例

- 不指定存储偏移量

    ```cpp
    // dst为ub地址，起始地址为8,非32B对齐
    vector_store_unalign ureg;
    vector_uint32_t src;
    asc_storeunalign_postupdate(dst, ureg, src);
    asc_storeunalign_post(dst, ureg);
    ```

- 掩码寄存器连续非对齐搬出

    ```cpp
    // dst为ub地址，起始地址为8,非32B对齐
    vector_store_unalign ureg;
    vector_bool src;
    uint32_t remain_count = 128;
    uint32_t repeat = 2;
    for (uint32_t i = 0; i < repeat; i++) {
        // 准备本次待搬出的src
        src = asc_update_src_b16(remain_count);
        // 跨32B边界的尾块缓存在ureg中
        asc_storeunalign_postupdate(dst, ureg, src);
    }
    // 循环结束后调用一次post，将ureg中剩余数据刷至UB；dst已自增至结束位置，offset为0
    asc_storeunalign_post(dst, ureg, 0);
    ```

- 使用uint32_t作为存储偏移量

    ```cpp
    // dst为ub地址，起始地址为8,非32B对齐
    vector_store_unalign ureg;
    vector_uint32_t src;
    uint32_t count = 64;
    uint32_t repeat = 2;
    for (uint32_t i = 0; i < repeat; i++) {
        // 其他reg操作

        // 第一次：src[0:62] 写入dst[0:62]（即UB[8:256]）；同时src[62:64] 写入ureg[0:2]；同时dst自增为dst[64]（即UB[264]）
        // 第二次：ureg[0:2] + src[0:62] 写入dst[-2:62]（即UB[256:512]）；同时src[62:64] 写入ureg[0:2]；同时dst自增为dst[128]（即UB[520]）
        asc_storeunalign_postupdate(dst, ureg, src, count);
    }
    // ureg[0:2] 写入dst[-2:0]（即UB[512:520]）；同时dst不自增
    asc_storeunalign_post_postupdate(dst, ureg, 0);
    ```

- 使用addr_reg作为存储偏移量

    ```cpp
    // dst为ub地址，起始地址为8,非32B对齐
    vector_store_unalign ureg;
    vector_uint32_t src;
    uint32_t repeat = 2;
    for (uint32_t i = 0; i < repeat; i++) {
        addr_reg count = asc_update_addr_reg_b32(64);
        // 其他reg操作

        // 第一次：src[0:62] 写入dst[0:62]（即UB[8:256]）；同时src[62:64] 写入ureg[0:2]；同时dst自增为dst[64]（即UB[264]）
        // 第二次：ureg[0:2] + src[0:62] 写入dst[-2:62]（即UB[256:512]）；同时src[62:64] 写入ureg[0:2]；同时dst自增为dst[128]（即UB[520]）
        asc_storeunalign_postupdate(dst, ureg, src, count);
    }
    // dst结束地址为520，但此时dst已经自增到了520，所以offset要配置为0 
    addr_reg offset = asc_update_addr_reg_b32(0);
    // ureg[0:2] 写入dst[-2:0]（即UB[512:520]）
    asc_storeunalign_post(dst, ureg, offset);
    ```
