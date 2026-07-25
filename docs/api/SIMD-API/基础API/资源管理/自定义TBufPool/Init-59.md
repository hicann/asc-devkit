# Init<a name="ZH-CN_TOPIC_0000002051724894"></a>

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
- Atlas 200I/500 A2 推理产品：不支持
<!-- end id4 -->
<!-- npu="310p" id5 -->
- Atlas 推理系列产品AI Core：支持
<!-- end id5 -->
<!-- npu="310p" id6 -->
- Atlas 推理系列产品Vector Core：不支持
<!-- end id6 -->
<!-- npu="910" id7 -->
- Atlas 训练系列产品：支持
<!-- end id7 -->


## 功能说明<a name="section618mcpsimp"></a>

完成自定义TbufPool资源与eventId等变量的初始化操作。

## 函数原型<a name="section620mcpsimp"></a>

```
__aicore__ inline void Init()
```

## 约束说明<a name="section633mcpsimp"></a>

用户可在自定义的构造函数中调用该接口，也可自行实现。

## 返回值说明<a name="section640mcpsimp"></a>

无

## 调用示例<a name="section1234017553610"></a>

请参考[调用示例](EXTERN_IMPL_BUFPOOL宏.md#section1234017553610)。

