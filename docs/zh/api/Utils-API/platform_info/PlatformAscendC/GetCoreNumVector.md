# GetCoreNumVector<a name="ZH-CN_TOPIC_0000002114097977"></a>

## 功能说明<a name="zh-cn_topic_0000001817714666_zh-cn_topic_0000001442758437_section36583473819"></a>

用于获取硬件平台独立的Vector Core的核数。

该接口仅在Atlas 推理系列产品有效，其他硬件平台型号均返回0。

## 函数原型<a name="zh-cn_topic_0000001817714666_zh-cn_topic_0000001442758437_section13230182415108"></a>

```
uint32_t GetCoreNumVector(void) const
```

## 参数说明<a name="zh-cn_topic_0000001817714666_zh-cn_topic_0000001442758437_section189014013619"></a>

无

## 返回值说明<a name="zh-cn_topic_0000001817714666_zh-cn_topic_0000001442758437_section25791320141317"></a>

返回硬件平台Vector Core的核数。

## 约束说明<a name="zh-cn_topic_0000001817714666_zh-cn_topic_0000001442758437_section19165124931511"></a>

<!-- npu="910" id1 -->
Atlas 训练系列产品，不支持该接口，返回0
<!-- end id1 -->
<!-- npu="310p" id2 -->
Atlas 推理系列产品，支持该接口，返回硬件平台Vector Core的核数
<!-- end id2 -->
<!-- npu="910b" id3 -->
Atlas A2 训练系列产品/Atlas A2 推理系列产品不支持该接口，返回0
<!-- end id3 -->
<!-- npu="A3" id4 -->
Atlas A3 训练系列产品/Atlas A3 推理系列产品不支持该接口，返回0
<!-- end id4 -->
<!-- npu="310b" id5 -->
Atlas 200I/500 A2 推理产品不支持该接口，返回0
<!-- end id5 -->
<!-- npu="950" id6 -->
Ascend 950PR/Ascend 950DT不支持该接口，返回0
<!-- end id6 -->

## 调用示例<a name="zh-cn_topic_0000001817714666_zh-cn_topic_0000001442758437_section320753512363"></a>

```
ge::graphStatus TilingXXX(gert::TilingContext* context) {
    auto ascendcPlatform = platform_ascendc::PlatformAscendC(context->GetPlatformInfo());
    auto aivCoreNum = ascendcPlatform.GetCoreNumAiv();
    auto vectorCoreNum = ascendcPlatform.GetCoreNumVector();
    auto allVecCoreNums = aivCoreNum + vectorCoreNum;
    // ...按照allVecCoreNums切分
    return ret;
}
```

