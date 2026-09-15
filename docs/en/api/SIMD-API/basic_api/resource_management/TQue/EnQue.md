# EnQue<a name="ZH-CN_TOPIC_0000001426057533"></a>

<!-- md-trans-meta sourceCommit=f6490d1e030d257ee55f3f1600bffece5a8bd3f7 translatedAt=2026-08-27T14:41:15.501Z -->

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
- Atlas 200I/500 A2 inference products: Supported
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

Pushes a **Tensor** to the queue.

## Prototype<a name="section620mcpsimp"></a>

-   No need to specify the source and destination positions.

    ```
    template <typename T>
    __aicore__ inline bool EnQue(const LocalTensor<T>& tensor)
    ```

-   Need to specify the source and destination positions.

    By binding VECIN and VECOUT through [TQueBind](../TQueBind/TQueBind.md), memory reuse between VECIN and VECOUT can be achieved. The following API is used to implement reuse in scenarios involving Vector computation, where the source and destination positions need to be specified during enqueue. In scenarios without Vector computation, the bool EnQue\(LocalTensor<T\>& tensor\) enqueue API can be called directly.

    ```
    template <TPosition srcUserPos, TPosition dstUserPos, typename T>
    __aicore__ inline bool EnQue(const LocalTensor<T>& tensor)
    ```

## Parameters<a name="section622mcpsimp"></a>

**Table 1**  Template parameter description

<a name="table1550165916920"></a>
<table><thead align="left"><tr id="row115015591391"><th class="cellrowborder" valign="top" width="12.139999999999999%" id="mcps1.2.3.1.1"><p id="p12501159099"><a name="p12501159099"></a><a name="p12501159099"></a>Parameter</p></th>
<th class="cellrowborder" valign="top" width="87.86%" id="mcps1.2.3.1.2"><p id="p85019592918"><a name="p85019592918"></a><a name="p85019592918"></a>Description</p></th>
</tr>
</thead>
<tbody><tr id="row1550117591914"><td class="cellrowborder" valign="top" width="12.139999999999999%" headers="mcps1.2.3.1.1 "><p id="p185019592913"><a name="p185019592913"></a><a name="p185019592913"></a>**T**</p></td>
<td class="cellrowborder" valign="top" width="87.86%" headers="mcps1.2.3.1.2 "><p id="p12101541625"><a name="p12101541625"></a><a name="p12101541625"></a><span>Data type of the **Tensor**.</span></p></td>
</tr>
<tr id="row1629562101416"><td class="cellrowborder" valign="top" width="12.139999999999999%" headers="mcps1.2.3.1.1 "><p id="p174916484213"><a name="p174916484213"></a><a name="p174916484213"></a>**srcUserPos**</p></td>
<td class="cellrowborder" valign="top" width="87.86%" headers="mcps1.2.3.1.2 "><p id="p24912414422"><a name="p24912414422"></a><a name="p24912414422"></a>User-specified src position of the queue. Currently only the following path is supported: GM-&gt;VECIN/VECOUT-&gt;GM.</p></td>
</tr>
<tr id="row1780112318146"><td class="cellrowborder" valign="top" width="12.139999999999999%" headers="mcps1.2.3.1.1 "><p id="p1469019111420"><a name="p1469019111420"></a><a name="p1469019111420"></a>**dstUserPos**</p></td>
<td class="cellrowborder" valign="top" width="87.86%" headers="mcps1.2.3.1.2 "><p id="p96907110428"><a name="p96907110428"></a><a name="p96907110428"></a>The user specifies the dst position of the queue. Currently, only the following path is supported: **GM**-&gt;**VECIN**/**VECOUT**-&gt;**GM**.</p></td>
</tr>
</tbody>
</table>

**Table 2**  Parameters

<a name="table181221135162517"></a>
<table><thead align="left"><tr id="row151221135112520"><th class="cellrowborder" valign="top" width="12.471247124712471%" id="mcps1.2.4.1.1"><p id="p1353754532512"><a name="p1353754532512"></a><a name="p1353754532512"></a>Parameter</p></th>
<th class="cellrowborder" valign="top" width="12.591259125912593%" id="mcps1.2.4.1.2"><p id="p1253774516259"><a name="p1253774516259"></a><a name="p1253774516259"></a>Input/Output</p></th>
<th class="cellrowborder" valign="top" width="74.93749374937492%" id="mcps1.2.4.1.3"><p id="p1653710452259"><a name="p1653710452259"></a><a name="p1653710452259"></a>Description</p></th>
</tr>
</thead>
<tbody><tr id="row12122235102511"><td class="cellrowborder" valign="top" width="12.471247124712471%" headers="mcps1.2.4.1.1 "><p id="p1537164502512"><a name="p1537164502512"></a><a name="p1537164502512"></a>tensor</p></td>
<td class="cellrowborder" valign="top" width="12.591259125912593%" headers="mcps1.2.4.1.2 "><p id="p1653714592515"><a name="p1653714592515"></a><a name="p1653714592515"></a>Input</p></td>
<td class="cellrowborder" valign="top" width="74.93749374937492%" headers="mcps1.2.4.1.3 "><p id="p165371945142512"><a name="p165371945142512"></a><a name="p165371945142512"></a>Specified tensor.</p></td>
</tr>
</tbody>
</table>

## Constraints<a name="section633mcpsimp"></a>

None

## Return Value<a name="section640mcpsimp"></a>

-   **true** - The Tensor is successfully added to the Queue.
-   **false** - The Queue is full, and the enqueue operation fails.

## Example<a name="section642mcpsimp"></a>

```
// API: EnQue Tensor.
AscendC::TPipe pipe;
AscendC::TQue<AscendC::TPosition::VECOUT, 4> que;
int num = 4;
int len = 1024;
pipe.InitBuffer(que, num, len);
AscendC::LocalTensor<half> tensor1 = que.AllocTensor<half>();
que.EnQue(tensor1);// Add the tensor to the VECOUT queue.
// API: EnQue specifies the src/dst position and adds the tensor to the corresponding queue.
// template <TPosition srcUserPos, TPosition dstUserPos> bool EnQue(LocalTensor<T>& tensor)
AscendC::TPipe pipe;
AscendC::TQueBind<AscendC::TPosition::VECIN, AscendC::TPosition::VECOUT, 1> que;
int num = 4;
int len = 1024;
pipe.InitBuffer(que, num, len);
AscendC::LocalTensor<half> tensor1 = que.AllocTensor<half>();
que.EnQue<AscendC::TPosition::GM, AscendC::TPosition::VECIN, half>(tensor1);// Add the tensor to the VECIN queue to implement memory reuse.
```

