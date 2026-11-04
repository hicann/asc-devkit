# GetUserWorkspace<a name="ZH-CN_TOPIC_0000001666932721"></a>

<!-- md-trans-meta sourceCommit=754fb95859dead826727a591e674261c6a829a08 translatedAt=2026-09-23T18:15:29.816Z pushedAt=2026-10-26T07:41:44.748Z nl=2 -->

## Applicable Products<a name="section1550532418810"></a>

<!-- npu="950" id1 -->
- Ascend 950PR&950DT products: supported
<!-- end id1 -->
<!-- npu="A3" id2 -->
- Atlas A3 products: supported
<!-- end id2 -->
<!-- npu="910b" id3 -->
- Atlas A2 products: supported
<!-- end id3 -->
<!-- npu="310b" id4 -->
- Atlas 200I/500 A2 inference product: not supported
<!-- end id4 -->
<!-- npu="310p" id5 -->
- Atlas inference products AI Core: supported
<!-- end id5 -->
<!-- npu="310p" id6 -->
- Atlas inference products Vector Core: not supported
<!-- end id6 -->
<!-- npu="910" id7 -->
- Atlas training products: supported
<!-- end id7 -->
<!-- @ref: asc-devkit/res/docs/zh/api/SIMD-API/basic_api/resource_management/workspace/GetUserWorkspace_res.md#id1 -->


## Description<a name="section618mcpsimp"></a>

Obtains the pointer to the user workspace. In the kernel launch development mode of the kernel function (**Kernel**), if the HAVE\_WORKSPACE compilation option is not enabled, the framework does not automatically set the system workspace. If high-level APIs that require the system workspace, such as the [Matmul kernel function (Kernel) side API](../../../adv_api/cube_compute/Matmul_Kernel/Matmul_Kernel.md), are used, the kernel side must set the system workspace through [SetSysWorkSpace](SetSysWorkSpace.md). In this case, the user workspace must be obtained through this API.

## Prototype<a name="section620mcpsimp"></a>

```
__aicore__ inline GM_ADDR GetUserWorkspace(GM_ADDR workspace)
```

## Parameters<a name="section622mcpsimp"></a>

**Table 1**  API parameter description

<a name="table1055216132132"></a>
<table><thead align="left"><tr id="row105531513121315"><th class="cellrowborder" valign="top" width="16.49%" id="mcps1.2.4.1.1"><p id="p5553171319138"><a name="p5553171319138"></a><a name="p5553171319138"></a>Parameter</p></th>
<th class="cellrowborder" valign="top" width="11.93%" id="mcps1.2.4.1.2"><p id="p5553151313131"><a name="p5553151313131"></a><a name="p5553151313131"></a>Input/Output</p></th>
<th class="cellrowborder" valign="top" width="71.58%" id="mcps1.2.4.1.3"><p id="p655316136139"><a name="p655316136139"></a><a name="p655316136139"></a>Description</p></th>
</tr>
</thead>
<tbody><tr id="row5553201314135"><td class="cellrowborder" valign="top" width="16.49%" headers="mcps1.2.4.1.1 "><p id="p8553813111314"><a name="p8553813111314"></a><a name="p8553813111314"></a>workspace</p></td>
<td class="cellrowborder" valign="top" width="11.93%" headers="mcps1.2.4.1.2 "><p id="p755318134134"><a name="p755318134134"></a><a name="p755318134134"></a>Input</p></td>
<td class="cellrowborder" valign="top" width="71.58%" headers="mcps1.2.4.1.3 "><p id="p1185064715302"><a name="p1185064715302"></a><a name="p1185064715302"></a>Pointer to the workspace, including the system workspace and the user workspace.</p></td>
</tr>
</tbody>
</table>

## Constraints<a name="section633mcpsimp"></a>

None

## Return Value<a name="section640mcpsimp"></a>

Returns the **workspace** pointer for the user.
