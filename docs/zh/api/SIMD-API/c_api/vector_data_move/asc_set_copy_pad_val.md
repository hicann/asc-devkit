# asc_set_copy_pad_val

## 产品支持情况

<!-- npu="950" id1 -->
- Ascend 950PR/Ascend 950DT：支持
<!-- end id1 -->
<!-- npu="A3" id2 -->
- Atlas A3 训练系列产品/Atlas A3 推理系列产品：支持
<!-- end id2 -->
<!-- npu="910b" id3 -->
- Atlas A2 训练系列产品/Atlas A2 推理系列产品：支持
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

和asc_copy_gm2ub_align接口配合使用，设置连续搬运数据块左右两侧需要填补的数据值。

<!-- npu="950" id10 -->

对于Ascend 950PR/Ascend 950DT产品：
- 该接口仅对asc_copy_gm2ub_align接口有效。
- fp8_e8m0_t、fp8_e5m2_t、fp8_e4m3fn_t、fp4x2_e2m1_t、fp4x2_e1m2_t、hifloat8_t类型的数据需转换成int8_t类型后再调用本接口。
<!-- end id10 -->

## 函数原型

```cpp
__aicore__ inline void asc_set_copy_pad_val(int8_t pad_value)
__aicore__ inline void asc_set_copy_pad_val(uint8_t pad_value)
__aicore__ inline void asc_set_copy_pad_val(int16_t pad_value)
__aicore__ inline void asc_set_copy_pad_val(uint16_t pad_value)
__aicore__ inline void asc_set_copy_pad_val(half pad_value)
__aicore__ inline void asc_set_copy_pad_val(bfloat16_t pad_value)
__aicore__ inline void asc_set_copy_pad_val(int32_t pad_value)
__aicore__ inline void asc_set_copy_pad_val(uint32_t pad_value)
__aicore__ inline void asc_set_copy_pad_val(float pad_value)
```

## 参数说明

**表1** 参数说明

|参数名|输入/输出|描述|
|------------|------------|-----------|
| pad_value     | 输入     | 需要填补的数据值。|

## 返回值说明

无

## 流水类型

PIPE_S

## 约束说明

无

## 调用示例

```cpp
// src是外部输入的int8_t类型的GM地址。
__ubuf__ int8_t dst[128];

asc_set_copy_pad_val(0);
// 在搬运过程中，每个数据块左右两侧各补充8个int8_t的数据，补充的数据值为0。
asc_copy_gm2ub_align(dst, src, 2, 48 * sizeof(int8_t), 8, 8, true, asc_load_l2_cache_mode::NORMAL_FIRST_VICTIM, 48 * sizeof(int8_t), 64 * sizeof(int8_t));
```
