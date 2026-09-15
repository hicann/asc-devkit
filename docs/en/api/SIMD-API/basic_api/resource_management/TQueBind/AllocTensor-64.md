# AllocTensor<a name="ZH-CN_TOPIC_0000001811034064"></a>

<!-- md-trans-meta sourceCommit=f6490d1e030d257ee55f3f1600bffece5a8bd3f7 translatedAt=2026-08-27T14:47:13.581Z -->

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

Allocates a **Tensor** from the **Que**. The size of the **Tensor** is the length of each memory block set during **InitBuffer**.

## Prototype<a name="section620mcpsimp"></a>

-   **non-inplace** API: Constructs a new **Tensor** as the object for memory management.

    ```
    template <typename T>
    __aicore__ inline LocalTensor<T> AllocTensor()
    ```

-   **inplace** API: Directly uses the passed-in **Tensor** as the object for memory management, which reduces the overhead of repeatedly creating **Tensor** objects. For usage guidance, see [Tensor In-place Operation](../../../../../guide/programming_guide/appendix/common_operations/how_to_use_tensor_in_place_operations_to_improve_operator_performance.md).

    ```
    template <typename T>
    __aicore__ inline void AllocTensor(LocalTensor<T>& tensor)
    ```

## Parameters<a name="section622mcpsimp"></a>

**Table 1** Template parameter description

<a name="table1550165916920"></a>
<table><thead align="left"><tr id="row115015591391"><th class="cellrowborder" valign="top" width="12.139999999999999%" id="mcps1.2.3.1.1"><p id="p12501159099"><a name="p12501159099"></a><a name="p12501159099"></a>Parameter</p></th>
<th class="cellrowborder" valign="top" width="87.86%" id="mcps1.2.3.1.2"><p id="p85019592918"><a name="p85019592918"></a><a name="p85019592918"></a>Description</p></th>
</tr>
</thead>
<tbody><tr id="row1550117591914"><td class="cellrowborder" valign="top" width="12.139999999999999%" headers="mcps1.2.3.1.1 "><p id="p185019592913"><a name="p185019592913"></a><a name="p185019592913"></a>**T**</p></td>
<td class="cellrowborder" valign="top" width="87.86%" headers="mcps1.2.3.1.2 "><p id="p12101541625"><a name="p12101541625"></a><a name="p12101541625"></a><span>Data type of the tensor.</span></p></td>
</tr>
</tbody>
</table>

**Table 2** Parameter description

<a name="table181221135162517"></a>
<table><thead align="left"><tr id="row151221135112520"><th class="cellrowborder" valign="top" width="12.471247124712471%" id="mcps1.2.4.1.1"><p id="p1353754532512"><a name="p1353754532512"></a><a name="p1353754532512"></a>Parameter</p></th>
<th class="cellrowborder" valign="top" width="12.591259125912593%" id="mcps1.2.4.1.2"><p id="p1253774516259"><a name="p1253774516259"></a><a name="p1253774516259"></a>Input/Output</p></th>
<th class="cellrowborder" valign="top" width="74.93749374937492%" id="mcps1.2.4.1.3"><p id="p1653710452259"><a name="p1653710452259"></a><a name="p1653710452259"></a>Description</p></th>
</tr>
</thead>
<tbody><tr id="row12122235102511"><td class="cellrowborder" valign="top" width="12.471247124712471%" headers="mcps1.2.4.1.1 "><p id="p1537164502512"><a name="p1537164502512"></a><a name="p1537164502512"></a>**tensor**</p></td>
<td class="cellrowborder" valign="top" width="12.591259125912593%" headers="mcps1.2.4.1.2 "><p id="p1653714592515"><a name="p1653714592515"></a><a name="p1653714592515"></a>Input</p></td>
<td class="cellrowborder" valign="top" width="74.93749374937492%" headers="mcps1.2.4.1.3 "><p id="p165371945142512"><a name="p165371945142512"></a><a name="p165371945142512"></a>The inplace API requires a <a href="../../data_structures/LocalTensor/LocalTensor.md">LocalTensor</a> as the object for memory management.</p></td>
</tr>
</tbody>
</table>

## Constraints<a name="section633mcpsimp"></a>

-   The Tensor content allocated by the **non-inplace** API may contain random values.
-   For the **non-inplace** API, set the **depth** template parameter of **TQueBind** to a non-zero value; for the **inplace** API, set the **depth** template parameter of **TQueBind** to 0.

## Return Value<a name="section640mcpsimp"></a>

The non-inplace API returns a **LocalTensor** object, while the inplace API has no return value.

## Example<a name="section642mcpsimp"></a>

-   non-inplace API

    ```
    AscendC::TPipe pipe;
    AscendC::TQueBind<AscendC::TPosition::VECOUT, AscendC::TPosition::GM, 2> que;
    int num = 4;
    int len = 1024;
    pipe.InitBuffer(que, num, len); // InitBuffer allocates 4 memory blocks, each 1024 bytes in size.
    AscendC::LocalTensor<half> tensor1 = que.AllocTensor<half>(); // AllocTensor allocates a Tensor of 1024 bytes in length.
    ```

-   inplace API

    ```
    AscendC::TPipe pipe;
    AscendC::TQueBind<AscendC::TPosition::VECOUT, AscendC::TPosition::GM, 0> que;
    int num = 2;
    int len = 1024;
    pipe.InitBuffer(que, num, len); // InitBuffer allocates 2 memory blocks, each 1024 bytes in size.
    AscendC::LocalTensor<half> tensor1;
    que.AllocTensor<half>(tensor1); // AllocTensor allocates a Tensor of 1024 bytes in length.
    ```
