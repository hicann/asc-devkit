# GetPosition<a name="ZH-CN_TOPIC_0000002364845741"></a>

## 产品支持情况<a name="section1550532418810"></a>

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
- Atlas 200I/500 A2 推理产品：支持
<!-- end id4 -->
<!-- npu="310p" id5 -->
- Atlas 推理系列产品AI Core：支持
<!-- end id5 -->
<!-- npu="310p" id6 -->
- Atlas 推理系列产品Vector Core：支持
<!-- end id6 -->
<!-- npu="910" id7 -->
- Atlas 训练系列产品：支持
<!-- end id7 -->
<!-- @ref: asc-devkit/res/docs/zh/api/SIMD-API/basic_api/data_structures/LocalTensor/GetPosition_res.md#id1 -->


## 功能说明<a name="section618mcpsimp"></a>

获取LocalTensor所在的TPosition逻辑位置，支持TPosition为VECIN、VECOUT、VECCALC、A1、A2、B1、B2、CO1、CO2。

## 函数原型<a name="section620mcpsimp"></a>

```cpp
__aicore__ inline int32_t GetPosition() const
```

## 参数说明<a name="section622mcpsimp"></a>

无

## 返回值说明<a name="section640mcpsimp"></a>

LocalTensor所在的[TPosition](../../aux_data_structures/TPosition.md)逻辑位置。

## 约束说明<a name="section633mcpsimp"></a>

无

## 调用示例<a name="section17531157161314"></a>

```cpp
// 示例
AscendC::TPosition srcPos = (AscendC::TPosition)inputLocal.GetPosition();
if (srcPos == AscendC::TPosition::VECCALC) {
    // 处理逻辑1
} else if (srcPos == AscendC::TPosition::A1) {
    // 处理逻辑2
} else {
    // 处理逻辑3
}
```
