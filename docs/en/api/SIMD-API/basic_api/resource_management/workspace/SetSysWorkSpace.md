# SetSysWorkSpace<a name="ZH-CN_TOPIC_0000001714391393"></a>

<!-- md-trans-meta sourceCommit=f6490d1e030d257ee55f3f1600bffece5a8bd3f7 translatedAt=2026-08-27T15:04:19.799Z -->

## Applicable Products<a name="section1550532418810"></a>

<!-- npu="950" id1 -->
- Ascend 950PR/Ascend 950DT: Supported
<!-- end id1 -->
<!-- npu="A3" id2 -->
- Atlas A3 training products/Atlas A3 inference products: Supported
<!-- end id2 -->
<!-- npu="910b" id3 -->
- Atlas A2 training products/Atlas A2 inference products: Supported
<!-- end id3 -->
<!-- npu="310b" id4 -->
- Atlas 200I/500 A2 inference products: Not supported
<!-- end id4 -->
<!-- npu="310p" id5 -->
- Atlas inference products AI Core: Supported
<!-- end id5 -->
<!-- npu="310p" id6 -->
- Atlas inference products Vector Core: Not supported
<!-- end id6 -->
<!-- npu="910" id7 -->
- Atlas training products: Supported
<!-- end id7 -->


## Description<a name="section618mcpsimp"></a>

The workspace required by the framework is called the system workspace. High-level APIs such as the [Matmul Kernel side API](../../../advanced_api/matrix_compute/Matmul-Kernel/matmul_kernel_api.md) require the system workspace. Therefore, when using such APIs, you need to call this API to set the pointer to the system workspace. When using the engineering-based operator development method or the kernel direct invocation method (with the HAVE\_WORKSPACE compilation option enabled), developers do not need to set it manually because the framework sets it automatically. In other scenarios, developers need to call **SetSysWorkSpace** to set it.

Before calling this API on the kernel side, you need to call **GetLibApiWorkSpaceSize** on the host side to obtain the size of the system workspace, and set the workspace size on the host side. The following is an example:

```
// User-defined tiling function.
static ge::graphStatus TilingFunc(gert::TilingContext* context)
{
    AddApiTiling tiling;
    ...
    size_t usrSize = 256; // Set the workspace size required by the user.
    // If the system workspace is required, call GetLibApiWorkSpaceSize to obtain the size of the system workspace.
    auto ascendcPlatform = platform_ascendc:: PlatformAscendC(context->GetPlatformInfo());
    uint32_t sysWorkspaceSize = ascendcPlatform.GetLibApiWorkSpaceSize();
    size_t *currentWorkspace = context->GetWorkspaceSizes(1); // Obtain the workspace pointer through the framework. The input parameter of GetWorkspaceSizes is the number of workspace blocks required. Currently, only one block is supported.
    currentWorkspace[0] = usrSize + sysWorkspaceSize; // Set the total workspace size. The total workspace space is applied for and managed by the framework.
    ...
}
```

## Prototype<a name="section620mcpsimp"></a>

```
__aicore__ inline void SetSysWorkspace(GM_ADDR workspace)
```

## Parameters<a name="section622mcpsimp"></a>

**Table 1** API parameter description

<a name="table1055216132132"></a>
<table><thead align="left"><tr id="row105531513121315"><th class="cellrowborder" valign="top" width="16.49%" id="mcps1.2.4.1.1"><p id="p5553171319138"><a name="p5553171319138"></a><a name="p5553171319138"></a>Parameter</p></th>
<th class="cellrowborder" valign="top" width="11.93%" id="mcps1.2.4.1.2"><p id="p5553151313131"><a name="p5553151313131"></a><a name="p5553151313131"></a>Input/Output</p></th>
<th class="cellrowborder" valign="top" width="71.58%" id="mcps1.2.4.1.3"><p id="p655316136139"><a name="p655316136139"></a><a name="p655316136139"></a>Description</p></th>
</tr>
</thead>
<tbody><tr id="row5553201314135"><td class="cellrowborder" valign="top" width="16.49%" headers="mcps1.2.4.1.1 "><p id="p8553813111314"><a name="p8553813111314"></a><a name="p8553813111314"></a>**workspace**</p></td>
<td class="cellrowborder" valign="top" width="11.93%" headers="mcps1.2.4.1.2 "><p id="p755318134134"><a name="p755318134134"></a><a name="p755318134134"></a>Input</p></td>
<td class="cellrowborder" valign="top" width="71.58%" headers="mcps1.2.4.1.3 "><p id="p1185064715302"><a name="p1185064715302"></a><a name="p1185064715302"></a>Pointer to the workspace passed in by the kernel function, including the system workspace and the workspace used by the user.</p></td>
</tr>
</tbody>
</table>

## Constraints<a name="section633mcpsimp"></a>

None

## Return Value<a name="section640mcpsimp"></a>

None

## Example<a name="section642mcpsimp"></a>

```
template<typename aType, typename bType, typename cType, typename biasType>
__aicore__ inline void MatmulLeakyKernel<aType, bType, cType, biasType>::Init(
    GM_ADDR a, GM_ADDR b, GM_ADDR bias, GM_ADDR c, GM_ADDR workspace, const TCubeTiling& tiling, float alpha)
{
    // Initialize the fusion operator.
    // ...
    // workspace is the GM pointer passed to the kernel function, used to set the system workspace.
    AscendC::SetSysWorkspace(workspace);
    if (GetSysWorkSpacePtr() == nullptr) {
        return;
    }
    // Subsequent Matmul instructions.
    ...
}
```

