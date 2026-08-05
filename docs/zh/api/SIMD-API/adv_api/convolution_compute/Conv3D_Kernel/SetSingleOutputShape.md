# SetSingleOutputShape

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

设置单核上结果矩阵Output的形状。

Conv3D高阶API目前支持M合轴模式的输出方式。在M合轴模式下，Conv3D API内部将Wout和Hout视为同一轴处理，输出时先沿Wout方向输出，完成一整行Wout输出后，再进行下一行的Wout输出。

**图1**  M合轴模式示意图  
![](../../../../figures/M合轴模式示意图.png "M合轴模式示意图")

## 函数原型

```
__aicore__ inline void SetSingleOutputShape(uint64_t singleCo, uint64_t singleDo, uint64_t singleM)
```

## 参数说明

| 参数名 | 输入/输出 | 描述 |
| --- | --- | --- |
| singleCo | 输入 | 单核上Output的C维度大小。 |
| singleDo | 输入 | 单核上Output的D维度大小。 |
| singleM | 输入 | 单核上Output的M维度大小，即H维度大小与W维度大小的乘积。 |

## 返回值说明

无

## 约束说明

本接口当前仅支持设置Output的C维度、D维度和M维度（即H轴、W轴合并后的维度），不支持设置原始Output的大小。

## 调用示例

```
conv3dApi.SetSingleOutputShape(singleCoreCout, singleCoreDout, singleCoreM);
```
