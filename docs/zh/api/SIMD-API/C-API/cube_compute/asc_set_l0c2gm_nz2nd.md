# asc_set_l0c2gm_nz2nd

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

数据搬运过程中进行随路格式转换（NZ格式转换为ND格式）时，通过调用该接口设置格式转换的相关配置。

## 函数原型

```cpp
__aicore__ inline void asc_set_l0c2gm_nz2nd(uint64_t nd_num, uint64_t src_nd_stride, uint64_t dst_nd_stride)
```

## 参数说明

| 参数名  | 输入/输出 | 描述 |
| :----- | :------- | :------- |
| nd_num | 输入 | ND矩阵的个数，取值范围为[1, 65535]。 |
| src_nd_stride | 输入 | 以分形大小为单位的源步长，源相邻NZ矩阵的偏移。取值范围为[0, 65535]，单位为32B。 |
| dst_nd_stride | 输入 | 目的相邻ND矩阵的偏移。取值范围为[1, $2^{32}$ - 1]，单位为元素。 |

## 返回值说明

无

## 流水类型

PIPE_S

## 约束说明

无

## 调用示例

```cpp
uint64_t nd_num = 2;
uint64_t src_nd_stride = 2;
uint64_t dst_nd_stride = 1;
asc_set_l0c2gm_nz2nd(nd_num, src_nd_stride, dst_nd_stride);
```
