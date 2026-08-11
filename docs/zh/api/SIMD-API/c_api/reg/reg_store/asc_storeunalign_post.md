# asc_storeunalign_post

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

reg计算数据搬运接口，适用于从矢量数据寄存器连续非32B对齐的起始地址连续搬出到UB的尾块场景。

需要先调用下列接口之一后，再调用本接口。
- [asc_storeunalign](asc_storeunalign.md)接口。
- [asc_storeunalign_postupdate](asc_storeunalign_postupdate.md)接口。

搬运原理如下：
记目的操作数的起始地址为dst_start，结束地址为dst_end，尾块元素个数为unalign_count = (dst_end - dst_end / 32 * 32) / sizeof(T)。
则搬运的数据会分为两部分，分别是：
- 32B对齐的主块部分UB[dst_start, dst_end - unalign_count * sizeof(T)]。
- 非32B对齐的尾块部分UB[dst_end - unalign_count * sizeof(T), dst_end]。
store_unalign或store_unalign_postupdate接口执行时，会将主块搬出至UB，尾块暂存至非对齐寄存器[0, unalign_count]。
本接口执行时，会将尾块从非对齐寄存器搬出至UB。

## 函数原型

- 使用int32_t作为偏移量，配合[asc_storeunalign](asc_storeunalign.md)接口。

    ```cpp
    __simd_callee__ inline void asc_storeunalign_post(__ubuf__ int8_t* dst, vector_store_unalign src, int32_t offset)
    __simd_callee__ inline void asc_storeunalign_post(__ubuf__ uint8_t* dst, vector_store_unalign src, int32_t offset)
    __simd_callee__ inline void asc_storeunalign_post(__ubuf__ fp4x2_e2m1_t* dst, vector_store_unalign src, int32_t offset)
    __simd_callee__ inline void asc_storeunalign_post(__ubuf__ fp4x2_e1m2_t* dst, vector_store_unalign src, int32_t offset)
    __simd_callee__ inline void asc_storeunalign_post(__ubuf__ int4b_t* dst, vector_store_unalign src, int32_t offset)
    __simd_callee__ inline void asc_storeunalign_post(__ubuf__ fp8_e8m0_t* dst, vector_store_unalign src, int32_t offset)
    __simd_callee__ inline void asc_storeunalign_post(__ubuf__ fp8_e5m2_t* dst, vector_store_unalign src, int32_t offset)
    __simd_callee__ inline void asc_storeunalign_post(__ubuf__ fp8_e4m3fn_t* dst, vector_store_unalign src, int32_t offset)
    __simd_callee__ inline void asc_storeunalign_post(__ubuf__ hifloat8_t* dst, vector_store_unalign src, int32_t offset)
    __simd_callee__ inline void asc_storeunalign_post(__ubuf__ int16_t* dst, vector_store_unalign src, int32_t offset)
    __simd_callee__ inline void asc_storeunalign_post(__ubuf__ uint16_t* dst, vector_store_unalign src, int32_t offset)
    __simd_callee__ inline void asc_storeunalign_post(__ubuf__ half* dst, vector_store_unalign src, int32_t offset)
    __simd_callee__ inline void asc_storeunalign_post(__ubuf__ bfloat16_t* dst, vector_store_unalign src, int32_t offset)
    __simd_callee__ inline void asc_storeunalign_post(__ubuf__ int32_t* dst, vector_store_unalign src, int32_t offset)
    __simd_callee__ inline void asc_storeunalign_post(__ubuf__ uint32_t* dst, vector_store_unalign src, int32_t offset)
    __simd_callee__ inline void asc_storeunalign_post(__ubuf__ float* dst, vector_store_unalign src, int32_t offset)
    __simd_callee__ inline void asc_storeunalign_post(__ubuf__ int64_t* dst, vector_store_unalign src, int32_t offset)
    ```

- 使用addr_reg作为偏移量，配合[asc_storeunalign_postupdate](asc_storeunalign_postupdate.md)使用addr_reg作为存储偏移量的接口。

    ```cpp
    __simd_callee__ inline void asc_storeunalign_post(__ubuf__ int8_t* dst, vector_store_unalign src, addr_reg offset)
    __simd_callee__ inline void asc_storeunalign_post(__ubuf__ uint8_t* dst, vector_store_unalign src, addr_reg offset)
    __simd_callee__ inline void asc_storeunalign_post(__ubuf__ fp4x2_e2m1_t* dst, vector_store_unalign src, addr_reg offset)
    __simd_callee__ inline void asc_storeunalign_post(__ubuf__ fp4x2_e1m2_t* dst, vector_store_unalign src, addr_reg offset)
    __simd_callee__ inline void asc_storeunalign_post(__ubuf__ int4b_t* dst, vector_store_unalign src, addr_reg offset)
    __simd_callee__ inline void asc_storeunalign_post(__ubuf__ fp8_e8m0_t* dst, vector_store_unalign src, addr_reg offset)
    __simd_callee__ inline void asc_storeunalign_post(__ubuf__ fp8_e5m2_t* dst, vector_store_unalign src, addr_reg offset)
    __simd_callee__ inline void asc_storeunalign_post(__ubuf__ fp8_e4m3fn_t* dst, vector_store_unalign src, addr_reg offset)
    __simd_callee__ inline void asc_storeunalign_post(__ubuf__ int16_t* dst, vector_store_unalign src, addr_reg offset)
    __simd_callee__ inline void asc_storeunalign_post(__ubuf__ uint16_t* dst, vector_store_unalign src, addr_reg offset)
    __simd_callee__ inline void asc_storeunalign_post(__ubuf__ half* dst, vector_store_unalign src, addr_reg offset)
    __simd_callee__ inline void asc_storeunalign_post(__ubuf__ bfloat16_t* dst, vector_store_unalign src, addr_reg offset)
    __simd_callee__ inline void asc_storeunalign_post(__ubuf__ int32_t* dst, vector_store_unalign src, addr_reg offset)
    __simd_callee__ inline void asc_storeunalign_post(__ubuf__ uint32_t* dst, vector_store_unalign src, addr_reg offset)
    __simd_callee__ inline void asc_storeunalign_post(__ubuf__ float* dst, vector_store_unalign src, addr_reg offset)
    __simd_callee__ inline void asc_storeunalign_post(__ubuf__ int64_t* dst, vector_store_unalign src, addr_reg offset)
    ```

## 参数说明

- 使用int32_t作为存储偏移量

    **表2** 参数说明

    | 参数名  | 输入/输出 | 描述 |
    | :----- | :------- | :------- |
    | dst | 输出 | 目的操作数（矢量）的起始地址。 |
    | src | 输入 | 非对齐寄存器，用于保存非对齐数据，长度32B。 |
    | offset | 输入 | 目的操作数结束地址的偏移，类型为int32_t，单位为元素个数。 |

- 使用addr_reg作为存储偏移量

    **表3** 参数说明

    | 参数名  | 输入/输出 | 描述 |
    | :----- | :------- | :------- |
    | dst | 输出 | 目的操作数（矢量）的起始地址。 |
    | src | 输入 | 非对齐寄存器，用于保存非对齐数据，长度32B。 |
    | offset | 输入 | 目的操作数结束地址的偏移，类型为地址寄存器，单位为元素个数。 |

非对齐寄存器和地址寄存器的详细说明请参见[reg数据类型定义](../reg_data_types/data_type_definition.md)。

## 返回值说明

无

## 流水类型

PIPE_V

## 约束说明

- 该接口中的dst不需要32B对齐，但数据类型为T的dst需要sizeof(T) Byte对齐。
- 调用本接口之前，需要调用[asc_storeunalign](asc_storeunalign.md)接口，或者[asc_storeunalign_postupdate](asc_storeunalign_postupdate.md)接口，且本接口与前序接口的非对齐寄存器要保持一致。
- 需要保证目的操作数的地址加上offset对应的偏移地址，结果等于数据搬运的结束地址，具体见调用示例。

## 调用示例

- 使用int32_t作为存储偏移量

    ```cpp
    // dst地址为8，非32B对齐，占用的地址为UB[8:520]。
    __ubuf__ uint32_t* dst = (__ubuf__ uint32_t*)asc_get_phy_buf_addr(8);
    vector_store_unalign ureg;
    vector_uint32_t src;
    uint32_t count = 64;
    uint32_t repeat = 2;
    for (uint32_t i = 0; i < repeat; i++) {
        // 其他reg操作

        // 第一次：src[0:62] 写入dst[0:62]（即UB[8:256]）；同时src[62:64] 写入ureg[0:2]
        // 第二次：ureg[0:2] + src[0:62] 写入dst[62:126]（即UB[256:512]）；同时src[62:64] 写入ureg[0:2]
        asc_storeunalign(dst + i*count, ureg, src, count);
    }
    // dst结束地址为520，即dst + repeat * count * sizeof(uint32_t)，所以offset要配置为repeat * count
    int32_t offset = repeat * count;
    // ureg[0:2] 写入dst[126:128]（即UB[512:520]）
    asc_storeunalign_post(dst, ureg, offset);
    ```

- 使用addr_reg作为存储偏移量

    ```cpp
    // dst地址为8，非32B对齐。占用的地址为[8:520]。
    __ubuf__ uint32_t* dst = (__ubuf__ uint32_t*)asc_get_phy_buf_addr(8);
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
