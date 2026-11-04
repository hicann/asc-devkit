# GetCoreNum<a name="ZH-CN_TOPIC_0000002114097973"></a>

<!-- md-trans-meta sourceCommit=754fb95859dead826727a591e674261c6a829a08 translatedAt=2026-10-23T03:05:00.113Z pushedAt=2026-10-23T07:01:40.533Z nl=2 -->

## Description<a name="zh-cn_topic_0000001647201621_zh-cn_topic_0000001442758437_section36583473819"></a>

Obtains the core count of the current hardware platform. If the AI Core architecture is in Cube/Vector Separation Mode, returns the core count of Vector Core; in Coupling Mode, returns the core count of AI Core.

## Prototype<a name="zh-cn_topic_0000001647201621_zh-cn_topic_0000001442758437_section13230182415108"></a>

```
uint32_t GetCoreNum(void) const
```

## Parameters<a name="zh-cn_topic_0000001647201621_zh-cn_topic_0000001442758437_section189014013619"></a>

None

## Return Value<a name="zh-cn_topic_0000001647201621_zh-cn_topic_0000001442758437_section25791320141317"></a>

<!-- npu="910" id1 -->
Atlas training products, Coupled Mode, returns the number of AI Cores
<!-- end id1 -->
<!-- npu="310p" id2 -->
Atlas inference products, Coupling Mode, returns the core count of AI Core
<!-- end id2 -->
<!-- npu="910b" id3 -->
Atlas A2 products, Separation Mode, returns the core count of Vector Core
<!-- end id3 -->
<!-- npu="A3" id4 -->
Atlas A3 products, Separation Mode, returns the core count of Vector Core
<!-- end id4 -->
<!-- npu="950" id5 -->
Ascend 950PR&950DT products, Separation Mode, returns the core count of Vector Core
<!-- end id5 -->

## Constraints<a name="zh-cn_topic_0000001647201621_zh-cn_topic_0000001442758437_section19165124931511"></a>

None

## Examples<a name="zh-cn_topic_0000001647201621_zh-cn_topic_0000001442758437_section320753512363"></a>

### Custom Operator Project

```cpp
ge::graphStatus TilingXXX(gert::TilingContext* context) {
    auto ascendcPlatform = platform_ascendc::PlatformAscendC(context->GetPlatformInfo());
    auto coreNum = ascendcPlatform.GetCoreNum();
    // Design the tiling strategy based on the core count.
    context->SetSimdNumBlocks(coreNum);
    return ret;
}
```

### Kernel Direct Invocation

```cpp
void GetInfoFun()
{
    auto* ascendcPlatform = platform_ascendc::PlatformAscendCManager::GetInstance();
    if (ascendcPlatform == nullptr) {
        return;
    }
    auto coreNum = ascendcPlatform->GetCoreNum();
    // ... Design the splitting strategy based on the core count.
}
```
