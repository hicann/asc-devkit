# GetVecRegLen<a name="ZH-CN_TOPIC_0000002556048947"></a>

<!-- md-trans-meta sourceCommit=b3e2e715646b9fecf3e353f980743f9a959a7286 translatedAt=2026-09-06T09:38:55.762Z pushedAt=2026-09-11T02:23:35.338Z -->

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
-   For the non-RegBase hardware platform chip architecture, 0 is returned.

## Constraints<a name="zh-cn_topic_0000001664705472_zh-cn_topic_0000001442758437_section19165124931511"></a>

None

## Example<a name="zh-cn_topic_0000001664705472_zh-cn_topic_0000001442758437_section320753512363"></a>

```
void GetLayerNormMaxMinTmpSize(...)
{
       platform_ascendc::PlatformAscendC* platform = platform_ascendc::PlatformAscendCManager::GetInstance();
        ...
        const uint32_t vecLenB32 = platform->GetVecRegLen() / LAYERNORM_SIZEOF_FLOAT;
        const uint32_t vecLenB16 = platform->GetVecRegLen() / LAYERNORM_SIZEOF_HALF;
        ...
}
```

