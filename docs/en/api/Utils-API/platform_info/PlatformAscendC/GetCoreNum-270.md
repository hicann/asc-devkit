# GetCoreNum<a name="ZH-CN_TOPIC_0000002114097973"></a>

<!-- md-trans-meta sourceCommit=134c8a713e94913719724061b56ccd2fc3a577d7 translatedAt=2026-09-06T09:23:16.699Z pushedAt=2026-09-11T02:05:24.763Z -->

## Description<a name="zh-cn_topic_0000001647201621_zh-cn_topic_0000001442758437_section36583473819"></a>

Obtains the number of cores on the current hardware platform. If the AI Core architecture is in Cube and Vector Core separation mode, returns the number of Vector Cores; in coupled mode, returns the number of AI Cores.

## Prototype<a name="zh-cn_topic_0000001647201621_zh-cn_topic_0000001442758437_section13230182415108"></a>

```
uint32_t GetCoreNum(void) const
```

## Parameters<a name="zh-cn_topic_0000001647201621_zh-cn_topic_0000001442758437_section189014013619"></a>

None

## Return Value<a name="zh-cn_topic_0000001647201621_zh-cn_topic_0000001442758437_section25791320141317"></a>

<!-- npu="910" id1 -->
Atlas training products, coupled mode: returns the number of AI Cores.
<!-- end id1 -->
<!-- npu="310p" id2 -->
Atlas inference products, coupled mode: returns the number of AI Cores.
<!-- end id2 -->
<!-- npu="910b" id3 -->
Atlas A2 training products/Atlas A2 inference products, separation mode: returns the number of Vector Cores.
<!-- end id3 -->
<!-- npu="A3" id4 -->
Atlas A3 training products/Atlas A3 inference products, separation mode: returns the number of Vector Cores.
<!-- end id4 -->
<!-- npu="950" id5 -->
Ascend 950PR/Ascend 950DT, separation mode: returns the number of Vector Cores.
<!-- end id5 -->

## Constraints<a name="zh-cn_topic_0000001647201621_zh-cn_topic_0000001442758437_section19165124931511"></a>

None

## Example<a name="zh-cn_topic_0000001647201621_zh-cn_topic_0000001442758437_section320753512363"></a>

```
ge::graphStatus TilingXXX(gert::TilingContext* context) {
    auto ascendcPlatform = platform_ascendc::PlatformAscendC(context->GetPlatformInfo());
    auto coreNum = ascendcPlatform.GetCoreNum();
    // Design the tiling strategy based on the number of cores.
    context->SetBlockDim(coreNum);
    return ret;
}
```

