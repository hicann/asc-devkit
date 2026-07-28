# asc_arange

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

以传入的value为起始值，生成递增/递减的索引，并将生成的索引保存在dst中。算法逻辑表示如下：
```cpp
// 递增
{value, value + 1, value + 2, ... value + VL_T - 2, value + VL_T - 1}
// 递减
{value + VL_T - 1, value + VL_T - 2, value + VL_T - 3, ... value + 1, value}
```

以int16_t数据类型，起始值value=10为例：
递增索引为{10, 11, 12, 13, ... 135, 136, 137}，递减索引为{137, 136, 135, 134, ... 12, 11, 10}。

## 函数原型

- 递增模式
    ```cpp
    __simd_callee__ inline void asc_arange(vector_int8_t& dst, int8_t value)
    __simd_callee__ inline void asc_arange(vector_int16_t& dst, int16_t value)
    __simd_callee__ inline void asc_arange(vector_half& dst, half value)
    __simd_callee__ inline void asc_arange(vector_int32_t& dst, int32_t value)
    __simd_callee__ inline void asc_arange(vector_float& dst, float value)
    ```

- 递减模式
    ```cpp
    __simd_callee__ inline void asc_arange_descend(vector_int8_t& dst, int8_t value)
    __simd_callee__ inline void asc_arange_descend(vector_int16_t& dst, int16_t value)
    __simd_callee__ inline void asc_arange_descend(vector_half& dst, half value)
    __simd_callee__ inline void asc_arange_descend(vector_int32_t& dst, int32_t value)
    __simd_callee__ inline void asc_arange_descend(vector_float& dst, float value)
    ```

## 参数说明

**表1** 参数说明

| 参数名       | 输入/输出 | 描述                |
| --------- | ----- | ----------------- |
| dst       | 输出    | 目的操作数（矢量数据寄存器）。 |
| value     | 输入    | 源操作数（标量）。 |

矢量数据寄存器的详细说明请参见[reg数据类型定义](../reg_data_types/data_type_definition.md)。

## 返回值说明

无

## 约束说明

对于整型数据类型，如果生成的索引发生溢出，结果将环绕。

## 调用示例

```cpp
__simd_vf__ inline void arange_vf(__ubuf__ int8_t* dst_addr, int8_t value, uint32_t count, uint16_t one_block_size, uint16_t repeat_time)
{
    vector_int8_t dst;
    vector_bool mask;
    for (uint16_t i = 0; i < repeat_time; ++i) {
        mask = asc_update_mask_b8(count);
        asc_arange(dst, value);
        asc_storealign(dst_addr, dst, one_block_size, mask);
    }
}
```
