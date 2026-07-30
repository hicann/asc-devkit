# asc_set_l0c_copy_prequant

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

数据搬运过程中进行随路量化时，通过调用该接口设置量化流程中的标量量化参数。

## 函数原型

```cpp
__aicore__ inline void asc_set_l0c_copy_prequant(uint64_t config)
```

## 参数说明

|参数名|输入/输出|描述|
|------------|------------|-----------|
| config     | 输入     | 量化过程中使用到的标量量化参数。|

## 返回值说明

无

## 流水类型

PIPE_S

## 约束说明

无

## 调用示例

```cpp
uint64_t config = 2;
asc_set_l0c_copy_prequant(config);
```