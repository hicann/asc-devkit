# GetCoreNumAic<a name="ZH-CN_TOPIC_0000002114052977"></a>

<!-- md-trans-meta sourceCommit=dacba287b3c94417af1e532d9cbd29c398889dea translatedAt=2026-11-10T10:33:09.802Z pushedAt=2026-11-13T08:46:49.276Z nl=2 -->

## Description<a name="zh-cn_topic_0000001675181281_zh-cn_topic_0000001442758437_section36583473819"></a>

Obtains the number of Cube cores in the AI Core on the current hardware platform. If the AI Core architecture uses the Cube/Vector Separated Mode, the number of Cube cores is returned; in Coupled Mode, the number of AI Cores is returned.

## Prototype<a name="zh-cn_topic_0000001675181281_zh-cn_topic_0000001442758437_section13230182415108"></a>

```
uint32_t GetCoreNumAic(void) const
```

## Parameters<a name="zh-cn_topic_0000001675181281_zh-cn_topic_0000001442758437_section189014013619"></a>

None

## Return Value<a name="zh-cn_topic_0000001675181281_zh-cn_topic_0000001442758437_section25791320141317"></a>

<!-- npu="910" id1 -->
Atlas training products, in coupled mode, return the number of AI Cores.
<!-- end id1 -->
<!-- npu="310p" id2 -->
Atlas inference products, in coupled mode, return the number of AI Cores.
<!-- end id2 -->
<!-- npu="910b" id3 -->
Atlas A2 products, in decoupled mode: Returns the number of Cube Cores.
<!-- end id3 -->
<!-- npu="A3" id4 -->
Atlas A3 products, in decoupled mode: Returns the number of Cube Cores.
<!-- end id4 -->
<!-- npu="950" id5 -->
Ascend 950PR&950DT products, in decoupled mode: Returns the number of Cube Cores.
<!-- end id5 -->

## Constraints<a name="zh-cn_topic_0000001675181281_zh-cn_topic_0000001442758437_section19165124931511"></a>

None

## Examples<a name="zh-cn_topic_0000001675181281_zh-cn_topic_0000001442758437_section320753512363"></a>

### Custom Operator Project

```cpp
ge::graphStatus TilingXXX(gert::TilingContext* context) {
    auto ascendcPlatform = platform_ascendc::PlatformAscendC(context->GetPlatformInfo());
    auto aicNum = ascendcPlatform.GetCoreNumAic();
    auto aivNum = ascendcPlatform.GetCoreNumAiv();
    // ...Split by aivNum.
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
    // ... Design a splitting strategy based on aicNum and aivNum.
}
```
