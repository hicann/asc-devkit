# asc_disable_dma_atomic

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

头文件路径为：`"c_api/atomic/atomic.h"`。

与[asc_set_atomic_add](asc_set_atomic_add.md)、[asc_set_atomic_max](asc_set_atomic_max.md)、[asc_set_atomic_min](asc_set_atomic_min.md)接口配合使用，用于在完成原子操作后关闭原子操作，避免影响后续搬运。

## 函数原型

```c
__aicore__ inline void asc_disable_dma_atomic()
```

## 参数说明

无

## 返回值说明

无

## 流水类型

`PIPE_S`

## 约束说明

无

## 调用示例

asc_disable_dma_atomic与[asc_set_atomic_add](asc_set_atomic_add.md)等原子操作接口配合使用，调用示例请参见[asc_set_atomic_add的调用示例](asc_set_atomic_add.md#调用示例)。
