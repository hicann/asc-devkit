# GetCoreNumAiv<a name="ZH-CN_TOPIC_0000002078498906"></a>

<!-- md-trans-meta sourceCommit=754fb95859dead826727a591e674261c6a829a08 translatedAt=2026-10-23T03:06:03.996Z pushedAt=2026-10-23T07:01:40.536Z nl=2 -->

## Description<a name="zh-cn_topic_0000001675101185_zh-cn_topic_0000001442758437_section36583473819"></a>

Obtains the number of Vector cores in the AI Core on the current hardware platform. If the AI Core architecture uses Cube and Vector Separation Mode, the number of Vector cores is returned; in Coupling Mode, the number of AI cores is returned.

## Prototype<a name="zh-cn_topic_0000001675101185_zh-cn_topic_0000001442758437_section13230182415108"></a>

```
uint32_t GetCoreNumAiv(void) const
```

## Parameters<a name="zh-cn_topic_0000001675101185_zh-cn_topic_0000001442758437_section189014013619"></a>

None

## Return Value<a name="zh-cn_topic_0000001675101185_zh-cn_topic_0000001442758437_section25791320141317"></a>

<!-- npu="910" id1 -->
Atlas training products, in Coupling Mode, return the number of AI Cores.
<!-- end id1 -->
<!-- npu="310p" id2 -->
Atlas inference products, in Coupling Mode, return the number of AI Cores.
<!-- end id2 -->
<!-- npu="910b" id3 -->
Atlas A2 products, in Separation Mode, return the number of Vector Cores.
<!-- end id3 -->
<!-- npu="A3" id4 -->
Atlas A3 products, in Separation Mode, return the number of Vector Cores.
<!-- end id4 -->
<!-- npu="950" id5 -->
Ascend 950PR&950DT products, in Separation Mode, return the number of Vector Cores.
<!-- end id5 -->

## Constraints<a name="zh-cn_topic_0000001675101185_zh-cn_topic_0000001442758437_section19165124931511"></a>

None

## Examples<a name="zh-cn_topic_0000001675101185_zh-cn_topic_0000001442758437_section320753512363"></a>

### Custom Operator Project

```cpp
ge::graphStatus TilingXXX(gert::TilingContext* context) {
    auto ascendcPlatform = platform_ascendc::PlatformAscendC(context->GetPlatformInfo());
    auto aicNum = ascendcPlatform.GetCoreNumAic();
    auto aivNum = ascendcPlatform.GetCoreNumAiv();
    // Tile by aivNum.
    context->SetSimdNumBlocks(ascendcPlatform.CalcTschNumBlocks(aivNum, aicNum, aivNum));
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
    auto aicNum = ascendcPlatform->GetCoreNumAic();
    auto aivNum = ascendcPlatform->GetCoreNumAiv();
    // ... Design the tiling strategy based on aicNum and aivNum as needed.
}
```
