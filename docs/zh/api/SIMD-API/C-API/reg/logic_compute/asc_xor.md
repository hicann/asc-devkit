# asc_xor

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

头文件路径：`"c_api/reg_compute/reg_vector.h"`。

asc_xor支持两种接口：

- 对矢量数据寄存器操作：
    根据mask对矢量数据寄存器src0和src1进行按位异或（^）操作，将结果写入dst。未被mask筛选的位置被置为0。
- 对掩码寄存器操作：
    根据mask对两个掩码寄存器src0和src1进行按位异或（^）操作，将结果写入dst。未被mask筛选的位置被置为0。

## 函数原型

- 操作数为矢量数据寄存器

```cpp
__simd_callee__ inline void asc_xor(vector_int8_t& dst, vector_int8_t src0, vector_int8_t src1, vector_bool mask)
__simd_callee__ inline void asc_xor(vector_uint8_t& dst, vector_uint8_t src0, vector_uint8_t src1, vector_bool mask)
__simd_callee__ inline void asc_xor(vector_int16_t& dst, vector_int16_t src0, vector_int16_t src1, vector_bool mask)
__simd_callee__ inline void asc_xor(vector_uint16_t& dst, vector_uint16_t src0, vector_uint16_t src1, vector_bool mask)
__simd_callee__ inline void asc_xor(vector_int32_t& dst, vector_int32_t src0, vector_int32_t src1, vector_bool mask)
__simd_callee__ inline void asc_xor(vector_uint32_t& dst, vector_uint32_t src0, vector_uint32_t src1, vector_bool mask)
```

- 操作数为掩码寄存器

```cpp
__simd_callee__ inline void asc_xor(vector_bool& dst, vector_bool src0, vector_bool src1, vector_bool mask)
```

## 参数说明

**表1** 参数说明（操作数为矢量数据寄存器）

| 参数名 | 输入/输出 | 描述 |
| --- | --- | --- |
| dst | 输出 | 目的操作数（矢量数据寄存器）。 |
| src0 | 输入 | 源操作数（矢量数据寄存器）。 |
| src1 | 输入 | 源操作数（矢量数据寄存器）。 |
| mask | 输入 | 源操作数掩码（掩码寄存器），用于指示在计算过程中哪些元素参与计算。对应位置为1时参与计算，为0时不参与计算。mask未筛选的元素在输出中置零。 |

**表2** 参数说明（操作数为掩码寄存器）

| 参数名 | 输入/输出 | 描述 |
| --- | --- | --- |
| dst | 输出 | 目的操作数（掩码寄存器）。 |
| src0 | 输入 | 源操作数（掩码寄存器）。 |
| src1 | 输入 | 源操作数（掩码寄存器）。 |
| mask | 输入 | 源操作数掩码（掩码寄存器），用于指示在计算过程中哪些元素参与计算。对应位置为1时参与计算，为0时不参与计算。mask未筛选的元素在输出中置零。 |

矢量数据寄存器和掩码寄存器的详细说明请参见[reg数据类型定义](../reg_data_types/data_type_definition.md)。

## 返回值说明

无

## 约束说明

dst中未被mask筛选的位置被置为0。

## 调用示例

- 对矢量数据寄存器操作

    ```cpp
    __simd_vf__ inline void xor_vf(__ubuf__ int8_t* dst_addr, __ubuf__ int8_t* src0_addr, __ubuf__ int8_t* src1_addr, uint32_t count, uint16_t one_repeat_size, uint16_t repeat_time)
    {
        vector_int8_t dst;
        vector_int8_t src0;
        vector_int8_t src1;
        vector_bool mask;
        for (uint16_t i = 0; i < repeat_time; ++i) {
            mask = asc_update_mask_b8(count);
            asc_loadalign(src0, src0_addr + i * one_repeat_size);
            asc_loadalign(src1, src1_addr + i * one_repeat_size);
            asc_xor(dst, src0, src1, mask);
            asc_storealign(dst_addr + i * one_repeat_size, dst, mask);
        }
    }
    ```

- 对掩码寄存器操作

    ```cpp
    __simd_vf__ inline void xor_vf(__ubuf__ int8_t* dst_addr, __ubuf__ int8_t* src_addr, uint32_t count, uint32_t one_repeat_size, uint16_t repeat_time)
    {
        vector_int8_t src;
        vector_bool dst;
        vector_bool mask = asc_create_mask_b8(PAT_ALL);
        vector_bool mask0 = asc_create_mask_b8(PAT_ALL);
        vector_bool mask1 = asc_create_mask_b8(PAT_ALLF);
        asc_xor(dst, mask0, mask1, mask);
        for (uint16_t i = 0; i < repeat_time; ++i) {
            asc_loadalign(src, src_addr + i * one_repeat_size);
            asc_add_scalar(src, src, 0, dst);
            asc_storealign(dst_addr + i * one_repeat_size, src, mask);
        }
    }
    ```
