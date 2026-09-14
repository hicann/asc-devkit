# GetResGroupBarrierWorkSpaceSize<a name="ZH-CN_TOPIC_0000002078653646"></a>

<!-- md-trans-meta sourceCommit=e3c8c70b3f5d27f3770063ae4b535bc75d904e3e translatedAt=2026-09-06T09:33:11.471Z pushedAt=2026-09-11T02:21:46.814Z -->

## Description<a name="zh-cn_topic_0000001969805546_zh-cn_topic_0000001391767420_section36583473819"></a>

Obtains the workspace size required by [GroupBarrier](../../../SIMD-API/basic_api/cube_group_mgmt_ISASI/GroupBarrier/GroupBarrier.md).

## Prototype<a name="zh-cn_topic_0000001969805546_zh-cn_topic_0000001391767420_section13230182415108"></a>

```
uint32_t GetResGroupBarrierWorkSpaceSize(void) const
```

## Parameters<a name="zh-cn_topic_0000001969805546_zh-cn_topic_0000001391767420_section189014013619"></a>

None

## Return Value<a name="zh-cn_topic_0000001969805546_zh-cn_topic_0000001391767420_section25791320141317"></a>

Workspace size required by the current GroupBarrier.

## Constraints<a name="zh-cn_topic_0000001969805546_zh-cn_topic_0000001391767420_section19165124931511"></a>

None

## Example<a name="zh-cn_topic_0000001969805546_zh-cn_topic_0000001391767420_section320753512363"></a>

```
// User-defined tiling function.
static ge::graphStatus TilingFunc(gert::TilingContext* context)
{
    AddApiTiling tiling;
    ...
    // If the system workspace is required, call GetLibApiWorkSpaceSize to obtain the system workspace size.
    auto ascendcPlatform = platform_ascendc:: PlatformAscendC(context->GetPlatformInfo());
    uint32_t sysWorkspaceSize = ascendcPlatform.GetLibApiWorkSpaceSize();
    // Set the workspace required by the user and the size required by GroupBarrier as the total size of usrWorkspace.
    size_t usrSize = 256 + ascendcPlatform.GetResGroupBarrierWorkSpaceSize(); // Set the workspace size required by the user.
    size_t *currentWorkspace = context->GetWorkspaceSizes(1); // Obtain the workspace pointer through the framework. The input parameter of GetWorkspaceSizes is the number of workspace blocks required. Currently, only one block is supported.
    currentWorkspace[0] = usrSize + sysWorkspaceSize; // Set the total workspace size. The total workspace space is applied for and managed by the framework.
    ...
}
```

