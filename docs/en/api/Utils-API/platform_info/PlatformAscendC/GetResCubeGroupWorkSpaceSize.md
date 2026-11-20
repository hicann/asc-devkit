# GetResCubeGroupWorkSpaceSize<a name="ZH-CN_TOPIC_0000002114052985"></a>

<!-- md-trans-meta sourceCommit=22a41aa8a74d7a2c3f454ea4bd5f0f8fb27aee29 translatedAt=2026-11-16T08:11:10.192Z pushedAt=2026-11-16T08:13:19.309Z nl=2 -->

## Description<a name="zh-cn_topic_0000001969965338_zh-cn_topic_0000001391767420_section36583473819"></a>

AI Core group computation based on [CreateCubeResGroup](../../../SIMD-API/basic_api/sync_control/inter_core_communication/CubeResGroupHandle/CreateCubeResGroup.md) requires a workspace for message communication. This API is provided on the host side to obtain the workspace size required by CreateCubeResGroup.

## Prototype<a name="zh-cn_topic_0000001969965338_zh-cn_topic_0000001391767420_section13230182415108"></a>

```
uint32_t GetResCubeGroupWorkSpaceSize(void) const
```

## Parameters<a name="zh-cn_topic_0000001969965338_zh-cn_topic_0000001391767420_section189014013619"></a>

None

## Return Value<a name="zh-cn_topic_0000001969965338_zh-cn_topic_0000001391767420_section25791320141317"></a>

Returns the workspace size required by CreateCubeResGroup. The API returns the workspace size on success. If `std::numeric_limits<uint32_t>::max()` (that is, `UINT32_MAX`) is returned, the API has failed.

## Constraints<a name="zh-cn_topic_0000001969965338_zh-cn_topic_0000001391767420_section19165124931511"></a>

None

## Examples<a name="zh-cn_topic_0000001969965338_zh-cn_topic_0000001391767420_section320753512363"></a>

### Custom Operator Project

```cpp
// User-defined tiling function.
static ge::graphStatus TilingFunc(gert::TilingContext* context)
{
    AddApiTiling tiling;
    ...
    // To use the system workspace, call GetLibApiWorkSpaceSize to obtain the size of the system workspace.
    auto ascendcPlatform = platform_ascendc:: PlatformAscendC(context->GetPlatformInfo());
    uint32_t sysWorkspaceSize = ascendcPlatform.GetLibApiWorkSpaceSize();
    if (sysWorkspaceSize == std::numeric_limits<uint32_t>::max()) {
        return ge::GRAPH_FAILED;
    }
    // Set the sum of the user workspace size and the size required by CreateCubeResGroup as the total size of usrWorkspace.
    uint32_t cubeGroupWorkspaceSize = ascendcPlatform.GetResCubeGroupWorkSpaceSize();
    if (cubeGroupWorkspaceSize == std::numeric_limits<uint32_t>::max()) {
        return ge::GRAPH_FAILED;
    }
    size_t usrSize = 256 + cubeGroupWorkspaceSize;
    size_t *currentWorkspace = context->GetWorkspaceSizes(1); // Obtain the workspace pointer through the framework. The input of GetWorkspaceSizes is the number of workspace blocks required. Currently, only one block is supported.
    currentWorkspace[0] = usrSize + sysWorkspaceSize; // Set the total workspace size. The total workspace space is applied for and managed by the framework.
    ...
}
```

### Kernel Direct Invocation

```cpp
uint32_t GetCubeGroupWorkspaceSize()
{
    auto* ascendcPlatform = platform_ascendc::PlatformAscendCManager::GetInstance();
    if (ascendcPlatform == nullptr) {
        return std::numeric_limits<uint32_t>::max();
    }
    return ascendcPlatform->GetResCubeGroupWorkSpaceSize();
}
```
