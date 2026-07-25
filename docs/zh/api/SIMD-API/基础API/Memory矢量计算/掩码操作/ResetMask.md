# ResetMask<a name="ZH-CN_TOPIC_0000001857638449"></a>

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

## 功能说明<a name="section618mcpsimp"></a>

头文件路径为:`"basic_api/kernel_common.h"`。

恢复mask的值为默认值（全1），表示矢量计算中每次迭代内的所有元素都将参与运算。

## 函数原型<a name="section620mcpsimp"></a>

```cpp
__aicore__ inline void ResetMask()
```

## 参数说明<a name="section622mcpsimp"></a>

无

## 返回值说明<a name="section640mcpsimp"></a>

无

## 约束说明<a name="section633mcpsimp"></a>

调用SetVectorMask接口之后，如需恢复mask的值为默认值（全1），可调用本接口实现。

## 调用示例<a name="section837496171220"></a>

用[SetVectorMask](SetVectorMask.md)设置mask值并使用完成后，使用ResetMask恢复mask的值为默认值。

```cpp
AscendC::SetVectorMask<half, AscendC::MaskMode::NORMAL>(128);
AscendC::ResetMask();
```

更多示例请参考[接口外设置Mask](../SIMD计算说明/掩码.md#接口外设置mask)。
