# InitSpmBuffer<a name="ZH-CN_TOPIC_0000001560400104"></a>

<!-- md-trans-meta sourceCommit=f6490d1e030d257ee55f3f1600bffece5a8bd3f7 translatedAt=2026-08-27T14:35:37.083Z -->

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

Initializes the **SPM Buffer**.

## Prototype<a name="section620mcpsimp"></a>

-   Stage to workspace for initialization, which requires specifying the GM address as the SPM Buffer:

    ```
    template <typename T>
    __aicore__ inline void InitSpmBuffer(const GlobalTensor<T>& workspace, const int32_t bufferSize)
    ```

-   Stage to L1 Buffer for initialization, which does not require specifying an address. It stages to the L1 Buffer by default and only requires the desired SPM Buffer size:

    ```
    __aicore__ inline void InitSpmBuffer(const int32_t  bufferSize)
    ```

    Atlas A2 training products/Atlas A2 inference products do not support the API for staging to the L1 Buffer for initialization.

    Atlas A3 training products/Atlas A3 inference products do not support the API for staging to the L1 Buffer for initialization.

## Parameters<a name="section622mcpsimp"></a>

<a name="table1794522316251"></a>
<table><thead align="left"><tr id="row19456238252"><th class="cellrowborder" valign="top" width="12.36%" id="mcps1.1.4.1.1"><p id="p119458239258"><a name="p119458239258"></a><a name="p119458239258"></a>Parameter</p></th>
<th class="cellrowborder" valign="top" width="12.379999999999999%" id="mcps1.1.4.1.2"><p id="p9945152332514"><a name="p9945152332514"></a><a name="p9945152332514"></a>Input/Output</p></th>
<th class="cellrowborder" valign="top" width="75.26%" id="mcps1.1.4.1.3"><p id="p1594552312513"><a name="p1594552312513"></a><a name="p1594552312513"></a>Description</p></th>
</tr>
</thead>
<tbody><tr id="row1694552372511"><td class="cellrowborder" valign="top" width="12.36%" headers="mcps1.1.4.1.1 "><p id="p1094516239250"><a name="p1094516239250"></a><a name="p1094516239250"></a>**workspace**</p></td>
<td class="cellrowborder" valign="top" width="12.379999999999999%" headers="mcps1.1.4.1.2 "><p id="p8945112312514"><a name="p8945112312514"></a><a name="p8945112312514"></a>Input</p></td>
<td class="cellrowborder" valign="top" width="75.26%" headers="mcps1.1.4.1.3 "><p id="p169454239253"><a name="p169454239253"></a><a name="p169454239253"></a>Workspace address.</p></td>
</tr>
<tr id="row524916295111"><td class="cellrowborder" valign="top" width="12.36%" headers="mcps1.1.4.1.1 "><p id="p92491829141111"><a name="p92491829141111"></a><a name="p92491829141111"></a>**bufferSize**</p></td>
<td class="cellrowborder" valign="top" width="12.379999999999999%" headers="mcps1.1.4.1.2 "><p id="p6249142931114"><a name="p6249142931114"></a><a name="p6249142931114"></a>Input</p></td>
<td class="cellrowborder" valign="top" width="75.26%" headers="mcps1.1.4.1.3 "><p id="p17249929131117"><a name="p17249929131117"></a><a name="p17249929131117"></a>Size of **SPM Buffer**, in bytes.</p></td>
</tr>
</tbody>
</table>

## Constraints<a name="section633mcpsimp"></a>

None

## Return Value<a name="section640mcpsimp"></a>

None

## Example<a name="section642mcpsimp"></a>

-   Stage to **workspace** for initialization

    ```
    AscendC::TPipe pipe;
    int len = 1024; // Set the SPM buffer to 1024 data elements of type T.
    workspace_gm.SetGlobalBuffer((__gm__ T *)usrWorkspace, len);  // Here, usrWorkspace is a user-defined workspace.
    auto gm = workspace_gm[AscendC::GetBlockIdx() * len];
    pipe.InitSpmBuffer(gm, len * sizeof(T));
    ```

-   Stage to **L1 Buffer** for initialization

    ```
    AscendC::TPipe pipe;
    int len = 1024; // Set the SPM buffer to 1024 data elements of type T.
    pipe.InitSpmBuffer(len * sizeof(T));
    ```

