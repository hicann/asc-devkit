# GetCoreNumVector<a name="ZH-CN_TOPIC_0000002114097977"></a>

<!-- md-trans-meta sourceCommit=134c8a713e94913719724061b56ccd2fc3a577d7 translatedAt=2026-09-06T09:27:24.266Z pushedAt=2026-09-11T02:15:54.832Z -->

## Description<a name="zh-cn_topic_0000001817714666_zh-cn_topic_0000001442758437_section36583473819"></a>

Obtains the number of independent Vector Cores on the hardware platform.

This API is valid only on Atlas inference products; other hardware platform models return 0.

## Prototype<a name="zh-cn_topic_0000001817714666_zh-cn_topic_0000001442758437_section13230182415108"></a>

```
uint32_t GetCoreNumVector(void) const
```

## Parameters<a name="zh-cn_topic_0000001817714666_zh-cn_topic_0000001442758437_section189014013619"></a>

None

## Return Value<a name="zh-cn_topic_0000001817714666_zh-cn_topic_0000001442758437_section25791320141317"></a>

Number of Vector Cores on the hardware platform.

## Constraints<a name="zh-cn_topic_0000001817714666_zh-cn_topic_0000001442758437_section19165124931511"></a>

<!-- npu="910" id1 -->
Atlas training products do not support this API and return 0.
<!-- end id1 -->
<!-- npu="310p" id2 -->
Atlas inference products support this API and return the number of Vector Cores on the hardware platform.
<!-- end id2 -->
<!-- npu="910b" id3 -->
Atlas A2 training products/Atlas A2 inference products do not support this API and return 0.
<!-- end id3 -->
<!-- npu="A3" id4 -->
Atlas A3 training products/Atlas A3 inference products do not support this API and return 0.
<!-- end id4 -->
<!-- npu="310b" id5 -->
Atlas 200I/500 A2 inference products do not support this API and return 0.
<!-- end id5 -->
<!-- npu="950" id6 -->
Ascend 950PR/Ascend 950DT do not support this API and return 0.
<!-- end id6 -->

## Example<a name="zh-cn_topic_0000001817714666_zh-cn_topic_0000001442758437_section320753512363"></a>

```
ge::graphStatus TilingXXX(gert::TilingContext* context) {
    auto ascendcPlatform = platform_ascendc::PlatformAscendC(context->GetPlatformInfo());
    auto aivCoreNum = ascendcPlatform.GetCoreNumAiv();
    auto vectorCoreNum = ascendcPlatform.GetCoreNumVector();
    auto allVecCoreNums = aivCoreNum + vectorCoreNum;
    // Split by allVecCoreNums.
    return ret;
}
```

