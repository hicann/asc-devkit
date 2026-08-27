# asc_get_l0c2gm_relu

## 产品支持情况

<!-- npu="950" id1 -->
- Ascend 950PR/Ascend 950DT：不支持
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

数据搬运过程中进行随路量化时，通过调用该接口获取ReLU操作前矢量的起始地址。

## 函数原型

```cpp
__aicore__ inline uint64_t asc_get_l0c2gm_relu()
```

## 参数说明

无

## 返回值说明

ReLU操作前矢量的起始地址。

## 流水类型

PIPE_S

## 约束说明

无

## 调用示例

```cpp
uint64_t relu_addr = asc_get_l0c2gm_relu();
```
