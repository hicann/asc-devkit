# SetAntiQuantVector

## 产品支持情况

<!-- npu="950" id3 -->
- Ascend 950PR&950DT系列产品：不支持
<!-- end id3 -->
<!-- npu="A3" id4 -->
- Atlas A3系列产品：不支持
<!-- end id4 -->
<!-- npu="910b" id5 -->
- Atlas A2系列产品：不支持
<!-- end id5 -->
<!-- npu="310b" id6 -->
- Atlas 200I/500 A2推理产品：不支持
<!-- end id6 -->
<!-- npu="310p" id7 -->
- Atlas推理系列产品AI Core：支持
- Atlas推理系列产品Vector Core：不支持
<!-- end id7 -->
<!-- npu="910" id8 -->
- Atlas训练系列产品：不支持
<!-- end id8 -->
<!-- npu="x90" id1 -->
- Kirin X90：不支持
<!-- end id1 -->
<!-- npu="9030" id2 -->
- Kirin 9030：不支持
<!-- end id2 -->
<!-- @ref: asc-devkit/res/docs/zh/api/SIMD-API/adv_api/cube_compute/Matmul_Kernel/SetAntiQuantVector_res.md#id1 -->

## 功能说明

在Matmul计算时支持A矩阵half类型输入，B矩阵int8类型输入，该场景下，需要调用伪量化接口进行伪量化。调用伪量化接口后，将数据从GM搬出到L1时，会执行伪量化操作，将B矩阵转化为half类型。本节的伪量化接口提供一个量化参数向量，该向量的shape为\[1, N\]，N值为Matmul矩阵计算时M/N/K中的N值。对B矩阵的每一列都采用该向量中对应列的伪量化系数进行伪量化。

请在[Iterate](Iterate.md)或者[IterateAll](IterateAll.md)之前调用该接口。

## 函数原型

```
__aicore__ inline void SetAntiQuantVector(const LocalTensor<SrcT>& offsetTensor, const LocalTensor<SrcT>& scaleTensor)
```

## 参数说明

| 参数名 | 输入/输出 | 描述 |
| --- | --- | --- |
| offsetTensor | 输入 | 伪量化运算时的参数向量，用于加。SrcT为A_TYPE中对应的数据类型。 |
| scaleTensor | 输入 | 伪量化运算时的参数向量，用于乘。SrcT为A_TYPE中对应的数据类型。 |

## 返回值说明

无

## 约束说明

无
