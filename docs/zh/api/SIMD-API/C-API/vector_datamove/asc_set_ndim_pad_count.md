# asc_set_ndim_pad_count

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

用于设置[asc_ndim_copy_gm2ub](./asc_ndim_copy_gm2ub.md)接口的各个维度左右侧的padding元素个数。

## 函数原型

```c++
__aicore__ inline void asc_set_ndim_pad_count(asc_ndim_pad_count_config& config)
```

## 参数说明

| 参数名  | 输入/输出 | 描述 |
| :----- | :------- | :------- |
| config | 输入 | 用于设置asc_ndim_copy_gm2ub接口的各个维度左右侧的padding元素个数，详细说明请参考[asc_ndim_pad_count_config.md](../struct/asc_ndim_pad_count_config.md)。 |

## 返回值说明

无

## 流水类型

PIPE_S

## 约束说明

需配合[asc_ndim_copy_gm2ub](./asc_ndim_copy_gm2ub.md)使用。

## 调用示例

```c++
asc_ndim_pad_count_config config;  // config默认初始值为0。
asc_set_ndim_pad_count(config); 
```