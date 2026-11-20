# GetResGroupBarrierWorkSpaceSize<a name="ZH-CN_TOPIC_0000002078653646"></a>

<!-- md-trans-meta sourceCommit=22a41aa8a74d7a2c3f454ea4bd5f0f8fb27aee29 translatedAt=2026-11-16T08:11:07.824Z pushedAt=2026-11-16T08:13:21.613Z nl=2 -->

## Description<a name="zh-cn_topic_0000001969805546_zh-cn_topic_0000001391767420_section36583473819"></a>

Obtains the workspace size required by [GroupBarrier](../../../SIMD-API/basic_api/sync_control/inter_core_sync/GroupBarrier/GroupBarrier.md).

## Prototype<a name="zh-cn_topic_0000001969805546_zh-cn_topic_0000001391767420_section13230182415108"></a>

```
uint32_t GetResGroupBarrierWorkSpaceSize(void) const
```

## Parameters<a name="zh-cn_topic_0000001969805546_zh-cn_topic_0000001391767420_section189014013619"></a>

None

## Return Value<a name="zh-cn_topic_0000001969805546_zh-cn_topic_0000001391767420_section25791320141317"></a>

The workspace size required by the current GroupBarrier. The API returns the workspace size on success. If `std::numeric_limits<uint32_t>::max()` (that is, `UINT32_MAX`) is returned, the API has failed.

## Constraints<a name="zh-cn_topic_0000001969805546_zh-cn_topic_0000001391767420_section19165124931511"></a>

None

## Examples<a name="zh-cn_topic_0000001969805546_zh-cn_topic_0000001391767420_section320753512363"></a>

### Custom Operator Project

```cpp
// User-defined tiling function.
static ge::graphStatus TilingFunc(gert::TilingContext* context)
{
    AddApiTiling tiling;
    ...
    // If system workspace is required, call GetLibApiWorkSpaceSize to obtain the size of the system workspace.
    auto ascendcPlatform = platform_ascendc:: PlatformAscendC(context->GetPlatformInfo());
    uint32_t sysWorkspaceSize = ascendcPlatform.GetLibApiWorkSpaceSize();
    if (sysWorkspaceSize == std::numeric_limits<uint32_t>::max()) {
        return ge::GRAPH_FAILED;
    }
    // Set the sum of the workspace required by the user and the size required by GroupBarrier as the total size of usrWorkspace.
    uint32_t groupBarrierWorkspaceSize = ascendcPlatform.GetResGroupBarrierWorkSpaceSize();
    if (groupBarrierWorkspaceSize == std::numeric_limits<uint32_t>::max()) {
        return ge::GRAPH_FAILED;
    }
    size_t usrSize = 256 + groupBarrierWorkspaceSize; // Set the workspace size required by the user.
    size_t *currentWorkspace = context->GetWorkspaceSizes(1); // Obtain the workspace pointer through the framework. The input parameter of GetWorkspaceSizes is the number of workspace blocks required. Currently, only one block is supported.
    currentWorkspace[0] = usrSize + sysWorkspaceSize; // Set the total workspace size. The total workspace space is requested and managed by the framework.
    ...
}
```

### Kernel Direct Invocation

```cpp
uint32_t GetGroupBarrierWorkspaceSize()
{
    auto* ascendcPlatform = platform_ascendc::PlatformAscendCManager::GetInstance();
    if (ascendcPlatform == nullptr) {
        return std::numeric_limits<uint32_t>::max();
    }
    return ascendcPlatform->GetResGroupBarrierWorkSpaceSize();
}
```
