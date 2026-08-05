# SetAntiQuantScalar

## 产品支持情况

<!-- npu="950" id3 -->
- Ascend 950PR/Ascend 950DT：不支持
<!-- end id3 -->
<!-- npu="A3" id4 -->
- Atlas A3 训练系列产品/Atlas A3 推理系列产品：不支持
<!-- end id4 -->
<!-- npu="910b" id5 -->
- Atlas A2 训练系列产品/Atlas A2 推理系列产品：不支持
<!-- end id5 -->
<!-- npu="310b" id6 -->
- Atlas 200I/500 A2 推理产品：不支持
<!-- end id6 -->
<!-- npu="310p" id7 -->
- Atlas 推理系列产品AI Core：支持
- Atlas 推理系列产品Vector Core：不支持
<!-- end id7 -->
<!-- npu="910" id8 -->
- Atlas 训练系列产品：不支持
<!-- end id8 -->
<!-- npu="x90" id1 -->
- Kirin X90：不支持
<!-- end id1 -->
<!-- npu="9030" id2 -->
- Kirin 9030：不支持
<!-- end id2 -->

## 功能说明

在Matmul计算时支持A矩阵half类型输入，B矩阵int8类型输入，该场景下，需要调用伪量化接口进行伪量化。调用伪量化接口后，将数据从GM搬出到L1时，会执行伪量化操作，将B矩阵转化为half类型。本节的伪量化接口提供对B矩阵的所有数据采用同一量化系数进行伪量化的功能。

请在[Iterate](Iterate.md)或者[IterateAll](IterateAll.md)之前调用该接口。

## 函数原型

```
__aicore__ inline void SetAntiQuantScalar(const SrcT offsetScalar, const SrcT scaleScalar)
```

## 参数说明

| 参数名 | 输入/输出 | 描述 |
| --- | --- | --- |
| offsetScalar | 输入 | 伪量化系数，用于加法。SrcT为A_TYPE中对应的数据类型。 |
| scaleScalar | 输入 | 伪量化系数，用于乘法。SrcT为A_TYPE中对应的数据类型。 |

## 返回值说明

无

## 约束说明

无
