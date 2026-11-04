# InitSpmBuffer<a name="ZH-CN_TOPIC_0000001560400104"></a>

<!-- md-trans-meta sourceCommit=754fb95859dead826727a591e674261c6a829a08 translatedAt=2026-09-23T17:44:44.816Z pushedAt=2026-10-26T07:41:44.685Z nl=2 -->

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
- Atlas 200I/500 A2 inference products: not supported
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
<!-- @ref: asc-devkit/res/docs/zh/api/SIMD-API/basic_api/resource_management/TPipe/InitSpmBuffer_res.md#id1 -->

## Description<a name="section618mcpsimp"></a>

The header file path is `"basic_api/kernel_tpipe.h"`.

Initializes the SPM Buffer. The SPM Buffer is a space in GM where overflow data can be stored when the Unified Buffer (UB) space is insufficient.

## Prototype<a name="section620mcpsimp"></a>

-   Stage to workspace for initialization, which requires specifying the GM address as the SPM Buffer:

    ```cpp
    template <typename T>
    __aicore__ inline void InitSpmBuffer(const GlobalTensor<T>& workspace, const int32_t bufferSize)
    ```

-   Stage to L1 Buffer for initialization, which does not require specifying an address. Data is staged to L1 Buffer by default, and only the required SPM Buffer size needs to be passed in:

    ```cpp
    __aicore__ inline void InitSpmBuffer(const int32_t bufferSize)
    ```

    <!-- npu="950" id10 -->
    Ascend 950PR&950DT products does not support the stage-to-L1-Buffer initialization API.
    <!-- end id10 -->
    <!-- npu="A3" id11 -->
    Atlas A3 products does not support the stage-to-L1-Buffer initialization API.
    <!-- end id11 -->
    <!-- npu="910b" id12 -->
    Atlas A2 products does not support the stage-to-L1-Buffer initialization API.
    <!-- end id12 -->

## Parameters<a name="section622mcpsimp"></a>

**Table 1** Template parameter description

| Parameter | Description |
| -------- | ---- |
| **T** | Data type of the Tensor to be obtained. For supported types, see [LocalTensor](../../data_structures/LocalTensor/LocalTensor.md). |

**Table 2** API parameter description

<a name="table1794522316251"></a>
<table><thead align="left"><tr id="row19456238252"><th class="cellrowborder" valign="top" width="12.36%" id="mcps1.1.4.1.1"><p id="p119458239258"><a name="p119458239258"></a><a name="p119458239258"></a>Parameter</p></th>
<th class="cellrowborder" valign="top" width="12.379999999999999%" id="mcps1.1.4.1.2"><p id="p9945152332514"><a name="p9945152332514"></a><a name="p9945152332514"></a>Input/Output</p></th>
<th class="cellrowborder" valign="top" width="75.26%" id="mcps1.1.4.1.3"><p id="p1594552312513"><a name="p1594552312513"></a><a name="p1594552312513"></a>Description</p></th>
</tr>
</thead>
<tbody><tr id="row1694552372511"><td class="cellrowborder" valign="top" width="12.36%" headers="mcps1.1.4.1.1 "><p id="p1094516239250"><a name="p1094516239250"></a><a name="p1094516239250"></a>**workspace**</p></td>
<td class="cellrowborder" valign="top" width="12.379999999999999%" headers="mcps1.1.4.1.2 "><p id="p8945112312514"><a name="p8945112312514"></a><a name="p8945112312514"></a>Input</p></td>
<td class="cellrowborder" valign="top" width="75.26%" headers="mcps1.1.4.1.3 "><p id="p169454239253"><a name="p169454239253"></a><a name="p169454239253"></a>**workspace** address.</p></td>
</tr>
<tr id="row524916295111"><td class="cellrowborder" valign="top" width="12.36%" headers="mcps1.1.4.1.1 "><p id="p92491829141111"><a name="p92491829141111"></a><a name="p92491829141111"></a>**bufferSize**</p></td>
<td class="cellrowborder" valign="top" width="12.379999999999999%" headers="mcps1.1.4.1.2 "><p id="p6249142931114"><a name="p6249142931114"></a><a name="p6249142931114"></a>Input</p></td>
<td class="cellrowborder" valign="top" width="75.26%" headers="mcps1.1.4.1.3 "><p id="p17249929131117"><a name="p17249929131117"></a><a name="p17249929131117"></a>Size of the **SPM Buffer**, in bytes.</p></td>
</tr>
</tbody>
</table>

## Constraints<a name="section633mcpsimp"></a>

SPM Buffer is intended for special scenarios where UB space is insufficient. It delivers poor performance and is generally not recommended.

## Return Value<a name="section640mcpsimp"></a>

None

## Example<a name="section642mcpsimp"></a>

-   Initialize by staging to workspace

    ```cpp
    AscendC::TPipe pipe;
    int len = 1024; // Set the spm buffer to 1024 data items of type T.
    workspace_gm.SetGlobalBuffer((__gm__ T *)usrWorkspace, len);  // Here usrWorkspace is a user-defined workspace.
    auto gm = workspace_gm[AscendC::GetBlockIdx() * len];
    pipe.InitSpmBuffer(gm, len * sizeof(T));
    ```

-   Stage to L1 Buffer for initialization.

    ```cpp
    AscendC::TPipe pipe;
    int len = 1024; // Set the spm buffer to 1024 data items of type T.
    pipe.InitSpmBuffer(len * sizeof(T));
    ```
