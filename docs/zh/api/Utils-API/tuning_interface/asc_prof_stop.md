# asc\_prof\_stop

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
- Atlas 推理系列产品Vector Core：不支持
<!-- end id5 -->
<!-- npu="910" id6 -->
- Atlas 训练系列产品：不支持
<!-- end id6 -->

## 功能说明

SIMD场景下，设置性能数据采集信号停止，和asc\_prof\_start配合使用。使用msOpProf工具进行算子上板调优时，可在kernel侧代码段前后分别调用asc\_prof\_start和asc\_prof\_stop来指定需要调优的代码段范围。

## 函数原型

```
__aicore__ inline void asc_prof_stop()
```

## 参数说明

无

## 返回值说明

无

## 约束说明

无

## 调用示例

```
asc_prof_stop();
```
