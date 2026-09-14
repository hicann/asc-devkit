# GetResCubeGroupWorkSpaceSize<a name="ZH-CN_TOPIC_0000002114052985"></a>

<!-- md-trans-meta sourceCommit=e3c8c70b3f5d27f3770063ae4b535bc75d904e3e translatedAt=2026-09-06T09:31:56.567Z pushedAt=2026-09-11T02:19:34.412Z -->

## Description<a name="zh-cn_topic_0000001969965338_zh-cn_topic_0000001391767420_section36583473819"></a>

To perform AI Core grouping computation based on [CreateCubeResGroup](../../../SIMD-API/basic_api/cube_group_mgmt_ISASI/CubeResGroupHandle/CreateCubeResGroup.md), a workspace is required for message communication. This API is provided on the Host side to obtain the workspace size required by **CreateCubeResGroup**.

## Prototype<a name="zh-cn_topic_0000001969965338_zh-cn_topic_0000001391767420_section13230182415108"></a>

```
uint32_t GetResCubeGroupWorkSpaceSize(void) const
```

## Parameters<a name="zh-cn_topic_0000001969965338_zh-cn_topic_0000001391767420_section189014013619"></a>

None

## Return Value<a name="zh-cn_topic_0000001969965338_zh-cn_topic_0000001391767420_section25791320141317"></a>

Workspace size currently required by **CreateCubeResGroup**.

## Constraints<a name="zh-cn_topic_0000001969965338_zh-cn_topic_0000001391767420_section19165124931511"></a>

None

## Example<a name="zh-cn_topic_0000001969965338_zh-cn_topic_0000001391767420_section320753512363"></a>

```
// User-defined tiling function.
static ge::graphStatus TilingFunc(gert::TilingContext* context)
{
    AddApiTiling tiling;
    ...
    // If system workspace is required, call GetLibApiWorkSpaceSize to obtain the size of the system workspace.
    auto ascendcPlatform = platform_ascendc:: PlatformAscendC(context->GetPlatformInfo());
    uint32_t sysWorkspaceSize = ascendcPlatform.GetLibApiWorkSpaceSize();
    // Set the workspace required by the user plus the size required by CreateCubeResGroup as the total size of usrWorkspace.
    size_t usrSize = 256 + ascendcPlatform.GetResCubeGroupWorkSpaceSize(); 
    size_t *currentWorkspace = context->GetWorkspaceSizes(1); // Obtain the workspace pointer through the framework. The input parameter of GetWorkspaceSizes is the number of workspace blocks required. Currently, only one block is supported.
    currentWorkspace[0] = usrSize + sysWorkspaceSize; // Set the total workspace size. The total workspace space is applied for and managed by the framework.
    ...
}
```

