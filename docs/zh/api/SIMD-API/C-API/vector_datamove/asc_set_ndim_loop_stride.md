# asc_set_ndim_loop_stride

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

用于设置[asc_ndim_copy_gm2ub](./asc_ndim_copy_gm2ub.md)接口，每个维度内的源操作数与目的操作数的元素之间的间隔，最多设置5个维度。

## 函数原型

```cpp
__aicore__ inline void asc_set_ndim_loop0_stride(uint64_t dst_stride, uint64_t src_stride)
__aicore__ inline void asc_set_ndim_loop1_stride(uint64_t dst_stride, uint64_t src_stride)
__aicore__ inline void asc_set_ndim_loop2_stride(uint64_t dst_stride, uint64_t src_stride)
__aicore__ inline void asc_set_ndim_loop3_stride(uint64_t dst_stride, uint64_t src_stride)
__aicore__ inline void asc_set_ndim_loop4_stride(uint64_t dst_stride, uint64_t src_stride)
```

## 参数说明

| 参数名  | 输入/输出 | 描述 |
| :----- | :------- | :------- |
| dst_stride | 输入 | 目的操作数的元素之间的间隔，单位为元素个数，默认值：0。取值范围：[0, 2^20 - 1]。 |
| src_stride | 输入 | 源操作数的元素之间的间隔，单位为元素个数，默认值：0。取值范围：[0, 2^40 - 1]。 |

## 返回值说明

无

## 流水类型

PIPE_S

## 约束说明

需配合[asc_ndim_copy_gm2ub](./asc_ndim_copy_gm2ub.md)使用。

## 调用示例

```cpp
uint64_t dst_stride = 8;
uint64_t src_stride = 8;
asc_set_ndim_loop0_stride(dst_stride, src_stride);
```