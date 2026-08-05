# SetSingleShape

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
- Atlas 推理系列产品Vector Core：不支持
<!-- end id5 -->
<!-- npu="910" id6 -->
- Atlas 训练系列产品：不支持
<!-- end id6 -->

## 功能说明

设置Conv3DBackpropInput在单核上计算的形状，单位为元素个数。

## 函数原型

```
__aicore__ inline void SetSingleShape(uint64_t singleShapeM, uint64_t singleShapeK, uint32_t singleShapeN)
```

## 参数说明

**表1**  接口参数说明

| 参数名 | 输入/输出 | 描述 |
| --- | --- | --- |
| singleShapeM | 输入 | 单核上M的大小，单位为元素。 |
| singleShapeK | 输入 | 单核上K的大小，单位为元素。 |
| singleShapeN | 输入 | 单核上N的大小，单位为元素。 |

## 返回值说明

无

## 约束说明

无

## 调用示例

```
const Conv3DBackpropInputTilingData* tilingData;
// ...初始化tilingData
ConvBackpropApi::Conv3DBackpropInput<weightDxType, inputSizeDxType, gradOutputDxType, gradInputDxType> gradInput_;
// ...设置其它参数
gradInput_.SetSingleShape(singleShapeM_, singleShapeK_, singleShapeN_); // 设置单核计算的形状
```
