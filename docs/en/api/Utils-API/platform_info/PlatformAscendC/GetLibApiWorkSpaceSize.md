# GetLibApiWorkSpaceSize<a name="ZH-CN_TOPIC_0000002114097981"></a>

<!-- md-trans-meta sourceCommit=b3e2e715646b9fecf3e353f980743f9a959a7286 translatedAt=2026-09-06T09:30:45.047Z pushedAt=2026-09-11T02:17:47.331Z -->

## Description<a name="zh-cn_topic_0000001663835704_zh-cn_topic_0000001391767420_section36583473819"></a>

Obtains the workspace size required by the AscendC API.

## Prototype<a name="zh-cn_topic_0000001663835704_zh-cn_topic_0000001391767420_section13230182415108"></a>

```
uint32_t GetLibApiWorkSpaceSize(void) const
```

## Parameters<a name="zh-cn_topic_0000001663835704_zh-cn_topic_0000001391767420_section189014013619"></a>

None

## Return Value<a name="zh-cn_topic_0000001663835704_zh-cn_topic_0000001391767420_section25791320141317"></a>

Result of the uint32\_t data type, which represents the size of the current system workspace in bytes.

## Constraints<a name="zh-cn_topic_0000001663835704_zh-cn_topic_0000001391767420_section19165124931511"></a>

None

## Example<a name="zh-cn_topic_0000001663835704_zh-cn_topic_0000001391767420_section320753512363"></a>

```
// User-defined tiling function.
static ge::graphStatus TilingFunc(gert::TilingContext* context)
{
    AddApiTiling tiling;
    ...
    size_t usrSize = 256; // Set the workspace size required by the user.
    // To use the system workspace, call GetLibApiWorkSpaceSize to obtain the system workspace size.
    auto ascendcPlatform = platform_ascendc:: PlatformAscendC(context->GetPlatformInfo());
    uint32_t sysWorkspaceSize = ascendcPlatform.GetLibApiWorkSpaceSize();
    size_t *currentWorkspace = context->GetWorkspaceSizes(1); // Obtain the workspace pointer through the framework. The input parameter of GetWorkspaceSizes is the number of workspace blocks required. Currently, only one block is supported.
    currentWorkspace[0] = usrSize + sysWorkspaceSize; // Set the total workspace size. The total workspace space is requested and managed by the framework.
    ...
}
```

