# GetVecRegLen<a name="ZH-CN_TOPIC_0000002556048947"></a>

<!-- md-trans-meta sourceCommit=ec5513f20d06e4751ee8b926efe401753e00bd6b translatedAt=2026-11-10T10:37:30.057Z pushedAt=2026-11-13T08:46:49.291Z nl=2 -->

## Description<a name="zh-cn_topic_0000001664705472_zh-cn_topic_0000001442758437_section36583473819"></a>

Obtains the bit width of the Vector compute unit of the current hardware platform chip architecture.

## Prototype<a name="zh-cn_topic_0000001664705472_zh-cn_topic_0000001442758437_section13230182415108"></a>

```
uint32_t GetVecRegLen(void) const
```

## Parameters<a name="zh-cn_topic_0000001664705472_zh-cn_topic_0000001442758437_section189014013619"></a>

None

## Return Value<a name="zh-cn_topic_0000001664705472_zh-cn_topic_0000001442758437_section25791320141317"></a>

-   For the RegBase hardware platform chip architecture, returns the bit width of the Vector compute unit on the current hardware platform.
-   For the non-RegBase hardware platform chip architecture, return 0.

## Constraints<a name="zh-cn_topic_0000001664705472_zh-cn_topic_0000001442758437_section19165124931511"></a>

None

## Examples<a name="zh-cn_topic_0000001664705472_zh-cn_topic_0000001442758437_section320753512363"></a>

### Custom Operator Project

```cpp
ge::graphStatus TilingXXX(gert::TilingContext* context)
{
    auto ascendcPlatform = platform_ascendc::PlatformAscendC(context->GetPlatformInfo());
    const uint32_t vecLenB32 = ascendcPlatform.GetVecRegLen() / LAYERNORM_SIZEOF_FLOAT;
    const uint32_t vecLenB16 = ascendcPlatform.GetVecRegLen() / LAYERNORM_SIZEOF_HALF;
    // ... Use vecLenB32 and vecLenB16 to design the tiling strategy.
    return ret;
}
```

### Kernel Direct Invocation

```cpp
void GetLayerNormMaxMinTmpSize(...)
{
    auto* ascendcPlatform = platform_ascendc::PlatformAscendCManager::GetInstance();
    if (ascendcPlatform == nullptr) {
        return;
    }
    const uint32_t vecLenB32 = ascendcPlatform->GetVecRegLen() / LAYERNORM_SIZEOF_FLOAT;
    const uint32_t vecLenB16 = ascendcPlatform->GetVecRegLen() / LAYERNORM_SIZEOF_HALF;
    // ... Use vecLenB32 and vecLenB16 to design the splitting strategy.
}
```
