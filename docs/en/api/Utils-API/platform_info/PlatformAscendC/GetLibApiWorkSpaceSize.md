# GetLibApiWorkSpaceSize<a name="ZH-CN_TOPIC_0000002114097981"></a>

<!-- md-trans-meta sourceCommit=ec5513f20d06e4751ee8b926efe401753e00bd6b translatedAt=2026-11-10T10:35:21.194Z pushedAt=2026-11-13T08:46:49.281Z nl=2 -->

## Description<a name="zh-cn_topic_0000001663835704_zh-cn_topic_0000001391767420_section36583473819"></a>

Obtains the workspace size required by the AscendC API.

## Prototype<a name="zh-cn_topic_0000001663835704_zh-cn_topic_0000001391767420_section13230182415108"></a>

```
uint32_t GetLibApiWorkSpaceSize(void) const
```

## Parameters<a name="zh-cn_topic_0000001663835704_zh-cn_topic_0000001391767420_section189014013619"></a>

None

## Return Value<a name="zh-cn_topic_0000001663835704_zh-cn_topic_0000001391767420_section25791320141317"></a>

Returns a result of the uint32_t data type, which represents the size of the current system workspace in bytes. The API returns the workspace size on success. If `std::numeric_limits<uint32_t>::max()` (that is, `UINT32_MAX`) is returned, the API has failed.

## Constraints<a name="zh-cn_topic_0000001663835704_zh-cn_topic_0000001391767420_section19165124931511"></a>

None

## Examples<a name="zh-cn_topic_0000001663835704_zh-cn_topic_0000001391767420_section320753512363"></a>

### Custom Operator Project

```cpp
// User-defined tiling function.
static ge::graphStatus TilingFunc(gert::TilingContext* context)
{
    AddApiTiling tiling;
    ...
    size_t usrSize = 256; // Set the workspace size required by the user.
    // To use the system workspace, call GetLibApiWorkSpaceSize to obtain the system workspace size.
    auto ascendcPlatform = platform_ascendc:: PlatformAscendC(context->GetPlatformInfo());
    uint32_t sysWorkspaceSize = ascendcPlatform.GetLibApiWorkSpaceSize();
    if (sysWorkspaceSize == std::numeric_limits<uint32_t>::max()) {
        return ge::GRAPH_FAILED;
    }
    size_t *currentWorkspace = context->GetWorkspaceSizes(1); // Obtain the workspace pointer through the framework. The input of GetWorkspaceSizes is the number of workspace blocks required. Currently, only one block is supported.
    currentWorkspace[0] = usrSize + sysWorkspaceSize; // Set the total workspace size. The total workspace is applied for and managed by the framework.
    ...
}
```

### Kernel Direct Invocation

```cpp
uint32_t GetSysWorkspaceSize()
{
    auto* ascendcPlatform = platform_ascendc::PlatformAscendCManager::GetInstance();
    if (ascendcPlatform == nullptr) {
        return std::numeric_limits<uint32_t>::max();
    }
    return ascendcPlatform->GetLibApiWorkSpaceSize();
}
```
