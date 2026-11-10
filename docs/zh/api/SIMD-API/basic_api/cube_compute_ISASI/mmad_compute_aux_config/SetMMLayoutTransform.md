# SetMMLayoutTransform

## 产品支持情况

<!-- npu="950" id1 -->
- Ascend 950PR&950DT系列产品：支持
<!-- end id1 -->
<!-- npu="A3" id2 -->
- Atlas A3系列产品：支持
<!-- end id2 -->
<!-- npu="910b" id3 -->
- Atlas A2系列产品：支持
<!-- end id3 -->
<!-- npu="310b" id4 -->
- Atlas 200I/500 A2推理产品：支持
<!-- end id4 -->
<!-- npu="310p" id5 -->
- Atlas推理系列产品AI Core：不支持
<!-- end id5 -->
<!-- npu="310p" id6 -->
- Atlas推理系列产品Vector Core：不支持
<!-- end id6 -->
<!-- npu="910" id7 -->
- Atlas训练系列产品：不支持
<!-- end id7 -->

## 功能说明

控制Mmad/MmadWithSparse计算结果的优先方向（M或N）。该接口已废弃，请使用[SetMMRowMajor/SetMMColumnMajor](SetMMColumnMajor-SetMMRowMajor.md)替代。

## 函数原型

```cpp
__aicore__ inline void SetMMLayoutTransform(bool mmLayoutMode)
```

## 参数说明

**表1** 参数说明

| 参数名 | 输入/输出 | 描述 |
| --- | --- | --- |
| mmLayoutMode | 输入 | Mmad/MmadWithSparse优先方向控制参数，bool类型。<br>&nbsp;&nbsp;&bull; true：CUBE优先通过N方向，再通过M方向生成结果（Column Major）；<br>&nbsp;&nbsp;&bull; false：CUBE优先通过M方向，再通过N方向生成结果（Row Major）。 |

## 返回值说明

无

## 约束说明

- 该接口已废弃，建议使用[SetMMRowMajor](SetMMColumnMajor-SetMMRowMajor.md)/[SetMMColumnMajor](SetMMColumnMajor-SetMMRowMajor.md)替代。

## 调用示例

```cpp
AscendC::SetMMLayoutTransform(true); // CUBE优先通过N方向，再通过M方向生成结果
// 建议使用如下方式替代：
// AscendC::SetMMColumnMajor();
```
