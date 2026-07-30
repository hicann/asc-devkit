# asc_set_ndim_pad_value

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

当[asc_ndim_copy_gm2ub](./asc_ndim_copy_gm2ub.md)接口的padding_mode参数为true时，该值将设置为Padding的填充固定值。fp8_e8m0_t，fp8_e5m2_t，fp8_e4m3fn_t类型的数据需转换成int8_t类型后再调用本接口。

## 函数原型

```c++
__aicore__ inline void asc_set_ndim_pad_value(int8_t pad_value)
__aicore__ inline void asc_set_ndim_pad_value(uint8_t pad_value)
__aicore__ inline void asc_set_ndim_pad_value(int16_t pad_value)
__aicore__ inline void asc_set_ndim_pad_value(uint16_t pad_value)
__aicore__ inline void asc_set_ndim_pad_value(half pad_value)
__aicore__ inline void asc_set_ndim_pad_value(bfloat16_t pad_value)
__aicore__ inline void asc_set_ndim_pad_value(int32_t pad_value)
__aicore__ inline void asc_set_ndim_pad_value(uint32_t pad_value)
__aicore__ inline void asc_set_ndim_pad_value(float pad_value)
```

## 参数说明

| 参数名  | 输入/输出 | 描述 |
| :----- | :------- | :------- |
| pad_value | 输入 | 用于设置asc_ndim_copy_gm2ub接口Padding的填充固定值。数据类型需要与asc_ndim_copy_gm2ub接口中dst和src的数据类型保持一致。 |

## 返回值说明

无

## 流水类型

PIPE_S

## 约束说明

需配合[asc_ndim_copy_gm2ub](./asc_ndim_copy_gm2ub.md)使用。

## 调用示例

```c++
half pad_value = 2.0;
asc_set_ndim_pad_value(pad_value); 
```