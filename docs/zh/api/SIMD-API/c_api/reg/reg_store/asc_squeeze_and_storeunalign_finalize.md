# asc_squeeze_and_storeunalign_finalize

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

reg计算数据搬运接口，适用于从矢量数据寄存器中**筛选出有效元素后**按照连续非32B对齐的起始地址连续搬出到Unified Buffer（UB）的尾块场景。

搬运原理如下：
记目的操作数的起始地址为dst_start，结束地址为dst_end，尾块元素个数为unalign_count = (dst_end - dst_end / 32 * 32) / sizeof(T)。则搬运的数据会分为两部分，分别是：
- 32B对齐的主块部分UB[dst_start, dst_end - unalign_count * sizeof(T)]。
- 非32B对齐的尾块部分UB[dst_end - unalign_count * sizeof(T), dst_end]。

[asc_squeeze_and_storeunalign](asc_squeeze_and_storeunalign.md)接口执行时，会将主块搬出至UB，尾块暂存至非对齐寄存器[0, unalign_count]。本接口执行时，会将尾块从非对齐寄存器搬出至UB。因此需要先调用[asc_squeeze_and_storeunalign](asc_squeeze_and_storeunalign.md)，再调用本接口。

## 函数原型

```cpp
__simd_callee__ inline void asc_squeeze_and_storeunalign_finalize(__ubuf__ int8_t* dst, vector_store_unalign src)
__simd_callee__ inline void asc_squeeze_and_storeunalign_finalize(__ubuf__ uint8_t* dst, vector_store_unalign src)
__simd_callee__ inline void asc_squeeze_and_storeunalign_finalize(__ubuf__ fp4x2_e2m1_t* dst, vector_store_unalign src)
__simd_callee__ inline void asc_squeeze_and_storeunalign_finalize(__ubuf__ fp4x2_e1m2_t* dst, vector_store_unalign src)
__simd_callee__ inline void asc_squeeze_and_storeunalign_finalize(__ubuf__ int4b_t* dst, vector_store_unalign src)
__simd_callee__ inline void asc_squeeze_and_storeunalign_finalize(__ubuf__ fp8_e8m0_t* dst, vector_store_unalign src)
__simd_callee__ inline void asc_squeeze_and_storeunalign_finalize(__ubuf__ fp8_e5m2_t* dst, vector_store_unalign src)
__simd_callee__ inline void asc_squeeze_and_storeunalign_finalize(__ubuf__ fp8_e4m3fn_t* dst, vector_store_unalign src)
__simd_callee__ inline void asc_squeeze_and_storeunalign_finalize(__ubuf__ int16_t* dst, vector_store_unalign src)
__simd_callee__ inline void asc_squeeze_and_storeunalign_finalize(__ubuf__ uint16_t* dst, vector_store_unalign src)
__simd_callee__ inline void asc_squeeze_and_storeunalign_finalize(__ubuf__ half* dst, vector_store_unalign src)
__simd_callee__ inline void asc_squeeze_and_storeunalign_finalize(__ubuf__ bfloat16_t* dst, vector_store_unalign src)
__simd_callee__ inline void asc_squeeze_and_storeunalign_finalize(__ubuf__ int32_t* dst, vector_store_unalign src)
__simd_callee__ inline void asc_squeeze_and_storeunalign_finalize(__ubuf__ uint32_t* dst, vector_store_unalign src)
__simd_callee__ inline void asc_squeeze_and_storeunalign_finalize(__ubuf__ float* dst, vector_store_unalign src)
```

## 参数说明

**表1** 参数说明

| 参数名  | 输入/输出 | 描述 |
| :----- | :------- | :------- |
| dst | 输出 | 目的操作数（矢量）的起始地址。 |
| src | 输入 | 非对齐寄存器，用于保存非对齐数据，长度32B。 |

非对齐寄存器和地址寄存器的详细说明请参见[reg数据类型定义](../reg_data_types/data_type_definition.md)。

## 返回值说明

无

## 流水类型

PIPE_V

## 约束说明

- 该接口中的dst不需要32B对齐，需要和[asc_squeeze_and_storeunalign](asc_squeeze_and_storeunalign.md)配置的目的操作数地址及非对齐寄存器保持一致。
- 需要保证目的操作数的地址写入非对齐寄存器中的数据后不越过UB可用容量上限，否则触发写越界异常。

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
