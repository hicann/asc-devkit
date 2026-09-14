# AddPlatformInfo<a name="ZH-CN_TOPIC_0000002333452901"></a>

<!-- md-trans-meta sourceCommit=134c8a713e94913719724061b56ccd2fc3a577d7 translatedAt=2026-09-06T15:46:27.720Z pushedAt=2026-09-11T06:26:57.049Z -->

## Description<a name="zh-cn_topic_0000001867409741_zh-cn_topic_0000001389787297_section36583473819"></a>

Sets the hardware platform information for use in debugging the operator Tiling function. The following two setting methods are supported:

-   **Automatically obtain the current hardware platform information**: Pass a null pointer to automatically obtain the current hardware information and add it to the ContextBuilder class.
-   **Specify the hardware platform information**: Pass a specific AI processor model to add the corresponding hardware information to the ContextBuilder class.

If the setting fails, an error message is printed. For details about log configuration and viewing, see the log-related environment variables in [Environment Variable Reference](https://www.hiascend.com/document/redirect/CannCommunityEnvRef).

## Prototype<a name="zh-cn_topic_0000001867409741_zh-cn_topic_0000001389787297_section13230182415108"></a>

```
ContextBuilder &AddPlatformInfo(const char* customSocVersion)
```

## Parameters<a name="zh-cn_topic_0000001867409741_zh-cn_topic_0000001389787297_section75395119104"></a>

|Parameter|Input/Output|Description|
|------------|------------|-----------|
| customSocVersion     | Input     | AI processor model.   |

## Return Value<a name="zh-cn_topic_0000001867409741_zh-cn_topic_0000001389787297_section25791320141317"></a>

The current **ContextBuilder** object.

## Constraints<a name="zh-cn_topic_0000001867409741_zh-cn_topic_0000001389787297_section19165124931511"></a>

After **AddPlatformInfo** is called, use [BuildTilingContext](BuildTilingContext.md) to build the Tiling context and pass it to the Tiling function for use.

## Example<a name="zh-cn_topic_0000001867409741_zh-cn_topic_0000001389787297_section320753512363"></a>

```
void AddPlatformInfoDemo(......)
{
    auto holder = context_ascendc::ContextBuilder()
	// ... ... // Add the call to the operator input/output interfaces.
	.AddPlatformInfo("Ascendxxxyy")
	.BuildTilingContext();
    auto tilingContext = holder->GetContext<gert::TilingContext>();
    // ... ...
}
```

