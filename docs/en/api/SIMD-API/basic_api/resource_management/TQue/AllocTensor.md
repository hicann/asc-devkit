# AllocTensor<a name="ZH-CN_TOPIC_0000001475776125"></a>

<!-- md-trans-meta sourceCommit=f6490d1e030d257ee55f3f1600bffece5a8bd3f7 translatedAt=2026-08-27T14:41:31.803Z -->

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

Allocates a **Tensor** from the **Que**. The size of the **Tensor** is the length of each memory block set when calling **InitBuffer**.

## Prototype<a name="section620mcpsimp"></a>

-   non-inplace API: Constructs a new **Tensor** as the object for memory management.

    ```
    template <typename T>
    __aicore__ inline LocalTensor<T> AllocTensor()
    ```

-   inplace API: Directly uses the passed-in **Tensor** as the object for memory management, reducing the overhead of repeatedly creating Tensors. For usage guidance, see [Tensor in-place operations](https://gitcode.com/cann/asc-devkit/blob/9.1.0/docs/en/guide/programming_guide/appendix/common_operations/how_to_use_tensor_in_place_operations_to_improve_operator_performance.md).

    ```
    template <typename T>
    __aicore__ inline void AllocTensor(LocalTensor<T>& tensor)
    ```

## Parameters<a name="section622mcpsimp"></a>

**Table 1**  Template parameter description

<a name="table1550165916920"></a>
<table><thead align="left"><tr id="row115015591391"><th class="cellrowborder" valign="top" width="12.139999999999999%" id="mcps1.2.3.1.1"><p id="p12501159099"><a name="p12501159099"></a><a name="p12501159099"></a>Parameter</p></th>
<th class="cellrowborder" valign="top" width="87.86%" id="mcps1.2.3.1.2"><p id="p85019592918"><a name="p85019592918"></a><a name="p85019592918"></a>Description</p></th>
</tr>
</thead>
<tbody><tr id="row1550117591914"><td class="cellrowborder" valign="top" width="12.139999999999999%" headers="mcps1.2.3.1.1 "><p id="p185019592913"><a name="p185019592913"></a><a name="p185019592913"></a>**T**</p></td>
<td class="cellrowborder" valign="top" width="87.86%" headers="mcps1.2.3.1.2 "><p id="p12101541625"><a name="p12101541625"></a><a name="p12101541625"></a><span>Data type of the Tensor.</span></p></td>
</tr>
</tbody>
</table>

**Table 2**  Parameter description

<a name="table181221135162517"></a>
<table><thead align="left"><tr id="row151221135112520"><th class="cellrowborder" valign="top" width="12.471247124712471%" id="mcps1.2.4.1.1"><p id="p1353754532512"><a name="p1353754532512"></a><a name="p1353754532512"></a>Parameter</p></th>
<th class="cellrowborder" valign="top" width="12.591259125912593%" id="mcps1.2.4.1.2"><p id="p1253774516259"><a name="p1253774516259"></a><a name="p1253774516259"></a>Input/Output</p></th>
<th class="cellrowborder" valign="top" width="74.93749374937492%" id="mcps1.2.4.1.3"><p id="p1653710452259"><a name="p1653710452259"></a><a name="p1653710452259"></a>Description</p></th>
</tr>
</thead>
<tbody><tr id="row12122235102511"><td class="cellrowborder" valign="top" width="12.471247124712471%" headers="mcps1.2.4.1.1 "><p id="p1537164502512"><a name="p1537164502512"></a><a name="p1537164502512"></a>**tensor**</p></td>
<td class="cellrowborder" valign="top" width="12.591259125912593%" headers="mcps1.2.4.1.2 "><p id="p1653714592515"><a name="p1653714592515"></a><a name="p1653714592515"></a>Input</p></td>
<td class="cellrowborder" valign="top" width="74.93749374937492%" headers="mcps1.2.4.1.3 "><p id="p165371945142512"><a name="p165371945142512"></a><a name="p165371945142512"></a>The inplace API requires passing <a href="../../data_structures/LocalTensor/LocalTensor.md">LocalTensor</a> as the object for memory management.</p></td>
</tr>
</tbody>
</table>

## Constraints<a name="section633mcpsimp"></a>

-   For all **Queue**s on the same **TPosition**, the number of **Tensor**s applied for by consecutively calling the **AllocTensor** API is subject to quantity constraints that vary by AI processor model. When applying for a **Buffer**, this constraint must be satisfied.

    Atlas training products: no more than 4.

    Atlas inference products AI Core: no more than 8.

    Atlas inference products Vector Core: no more than 8.

    Atlas A2 training products/Atlas A2 inference products: no more than 8.

    Atlas A3 training products/Atlas A3 inference products: no more than 8.

    Atlas 200I/500 A2 inference products: no more than 8.



-   The **Tensor** content allocated by the non-inplace API may contain random values.
-   For the non-inplace API, set the **depth** template parameter of **TQueBind** to a non-zero value; for the inplace API, set the **depth** template parameter of **TQueBind** to 0.

## Return Value<a name="section640mcpsimp"></a>

The non-inplace API returns a **LocalTensor** object, while the inplace API has no return value.

## Example<a name="section642mcpsimp"></a>

-   Example 1

    ```
    // Use AllocTensor to allocate a Tensor.
    AscendC::TPipe pipe;
    AscendC::TQue<AscendC::TPosition::VECOUT, 2> que;
    int num = 2;
    int len = 1024;
    pipe.InitBuffer(que, num, len); // InitBuffer allocates 2 memory blocks, each 1024 bytes in size.
    AscendC::LocalTensor<half> tensor1 = que.AllocTensor<half>(); // AllocTensor allocates a Tensor of 1024 bytes in length.
    ```

-   Example 2

    ```
    // The following is an example of the restriction scenario for consecutive use of AllocTensor:
    AscendC::TQue<AscendC::TPosition::VECIN, 1> que0;
    AscendC::TQue<AscendC::TPosition::VECIN, 1> que1;
    AscendC::TQue<AscendC::TPosition::VECIN, 1> que2;
    AscendC::TQue<AscendC::TPosition::VECIN, 1> que3;
    AscendC::TQue<AscendC::TPosition::VECIN, 1> que4;
    AscendC::TQue<AscendC::TPosition::VECIN, 1> que5;
    // Not recommended:
    // For example, an operator has 6 inputs and needs to apply for 6 buffers.
    // Allocate memory through six queues, que0 to que5, with one block per queue, for a total of six buffers on the VECIN TPosition.
    // Assume that the number of consecutively allocated buffers on the same TPosition is limited to four. Exceeding this limit causes resource allocation failures when using AllocTensor/FreeTensor.
    // On the NPU, this may manifest as abnormal behavior such as a hang, while in the CPU Debug scenario an error is reported.
    pipe.InitBuffer(que0, 1, len);
    pipe.InitBuffer(que1, 1, len);
    pipe.InitBuffer(que2, 1, len);
    pipe.InitBuffer(que3, 1, len);
    pipe.InitBuffer(que4, 1, len);
    pipe.InitBuffer(que5, 1, len);
    
    AscendC::LocalTensor<T> local1 = que0.AllocTensor<T>();
    AscendC::LocalTensor<T> local2 = que1.AllocTensor<T>();
    AscendC::LocalTensor<T> local3 = que2.AllocTensor<T>();
    AscendC::LocalTensor<T> local4 = que3.AllocTensor<T>();
    // The fifth AllocTensor causes a resource allocation failure because the number of tensors simultaneously allocated on the same TPosition exceeds the limit of four.
    AscendC::LocalTensor<T> local5 = que4.AllocTensor<T>();
    
    // In this case, the following solutions are recommended:
    // If multiple buffers are indeed required, merge them into a single buffer and use offsets to access them.
    pipe.InitBuffer(que0, 1, len * 3);
    pipe.InitBuffer(que1, 1, len * 3);
    /*
     * Allocate a LocalTensor of 3 memory blocks. The address of local1 is the start address of the buffer in que0, 
     * the address of local2 is the address of local1 offset by len, and the address of local3 is the address of local1 offset by 
     * len * 2. 
     */
    int32_t offset1 = len;
    int32_t offset2 = len * 2;
    AscendC::LocalTensor<T> local1 = que0.AllocTensor<T>();
    AscendC::LocalTensor<T> local2 = local1[offset1];
    AscendC::LocalTensor<T> local3 = local1[offset2];
    ```

-   Example 3: inplace API

    ```
    AscendC::TPipe pipe;
    AscendC::TQue<AscendC::TPosition::VECIN, 0> que;
    int num = 2;
    int len = 1024;
    pipe.InitBuffer(que, num, len); // InitBuffer allocates two memory blocks, each 1024 bytes in size.
    AscendC::LocalTensor<half> tensor1;
    que.AllocTensor<half>(tensor1); // AllocTensor allocates a tensor of 1024 bytes.
    ```
