# End

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

在Conv3DBackpropFilter卷积反向计算完成后，必须调用一次End，以清除EventID并释放内部申请的临时内存。

## 函数原型

```
__aicore__ inline void End()
```

## 参数说明

无

## 返回值说明

无

## 约束说明

End接口必须在Iterate和GetTensorC接口后调用。

## 调用示例

```
const Conv3DBackpropFilterTilingData* tilingData;
// ...初始化tilingData
ConvBackpropApi::Conv3DBackpropFilter<inputType, weightSizeType, gradOutputType, gradWeightType> gradWeight_;
// ...其它调用
gradWeight_.End();
```

