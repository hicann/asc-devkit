# SetMaskNorm<a name="ZH-CN_TOPIC_0000001836766205"></a>

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
- Atlas 推理系列产品 AI Core：支持
<!-- end id5 -->
<!-- npu="310p" id6 -->
- Atlas 推理系列产品 Vector Core：不支持
<!-- end id6 -->
<!-- npu="910" id7 -->
- Atlas 训练系列产品：不支持
<!-- end id7 -->
<!-- npu="x90" id8 -->
- Kirin X90：支持
<!-- end id8 -->
<!-- npu="9030" id9 -->
- Kirin 9030：支持
<!-- end id9 -->

## 功能说明<a name="section618mcpsimp"></a>

头文件路径为：`"basic_api/kernel_common.h"`。

设置Mask模式为Normal模式。该模式为系统默认模式，支持开发者配置迭代次数。本接口推荐配合API中isSetMask模板参数使用，当isSetMask为false时，支持用户调用本接口手动管理Normal模式，并通过[SetVectorMask](SetVectorMask.md)设置Normal模式下的掩码。

## 函数原型<a name="section620mcpsimp"></a>

```cpp
__aicore__ inline void SetMaskNorm()
```

## 参数说明<a name="section622mcpsimp"></a>

无

## 返回值说明<a name="section640mcpsimp"></a>

无

## 约束说明<a name="section633mcpsimp"></a>

- 本接口推荐配合API中IsSetMask模板参数使用，当isSetMask为false时，支持用户调用本接口手动管理Normal模式并通过[SetVectorMask](SetVectorMask.md)设置Normal模式下的掩码。

## 调用示例<a name="section837496171220"></a>

请参考[Normal模式调用示例](SetVectorMask.md#section837496171220)。
