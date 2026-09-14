# GetCoreNumAiv<a name="ZH-CN_TOPIC_0000002078498906"></a>

<!-- md-trans-meta sourceCommit=134c8a713e94913719724061b56ccd2fc3a577d7 translatedAt=2026-09-06T09:25:53.943Z pushedAt=2026-09-11T02:07:22.302Z -->

## Description<a name="zh-cn_topic_0000001675101185_zh-cn_topic_0000001442758437_section36583473819"></a>

Obtains the number of Vector cores in the AI Core of the current hardware platform. If the AI Core architecture uses the Cube/Vector separation mode, the number of Vector cores is returned; in coupled mode, the number of AI Cores is returned.

## Prototype<a name="zh-cn_topic_0000001675101185_zh-cn_topic_0000001442758437_section13230182415108"></a>

```
uint32_t GetCoreNumAiv(void) const
```

## Parameters<a name="zh-cn_topic_0000001675101185_zh-cn_topic_0000001442758437_section189014013619"></a>

None

## Return Value<a name="zh-cn_topic_0000001675101185_zh-cn_topic_0000001442758437_section25791320141317"></a>

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

## Constraints<a name="zh-cn_topic_0000001675101185_zh-cn_topic_0000001442758437_section19165124931511"></a>

None

## Example<a name="zh-cn_topic_0000001675101185_zh-cn_topic_0000001442758437_section320753512363"></a>

```
ge::graphStatus TilingXXX(gert::TilingContext* context) {
    auto ascendcPlatform = platform_ascendc::PlatformAscendC(context->GetPlatformInfo());
    auto aicNum = ascendcPlatform.GetCoreNumAic();
    auto aivNum = ascendcPlatform.GetCoreNumAiv();
    // Split by aivNum.
    context->SetBlockDim(ascendcPlatform.CalcTschNumBlocks(aivNum, aicNum, aivNum));
    return ret;
}
```

