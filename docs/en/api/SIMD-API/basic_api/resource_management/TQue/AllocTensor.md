# AllocTensor<a name="ZH-CN_TOPIC_0000001475776125"></a>

<!-- md-trans-meta sourceCommit=8fa2928a9ba79777fe5da4f7c9987e59a88f273e translatedAt=2026-11-05T04:21:52.858Z pushedAt=2026-11-18T01:25:31.043Z nl=2 -->

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
- Atlas 200I/500 A2 inference products: supported
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
<!-- npu="x90" id8 -->
- Kirin X90 processor products: Supported
<!-- end id8 -->
<!-- npu="9030" id9 -->
- Kirin 9030 processor products: Supported
<!-- end id9 -->
<!-- @ref: asc-devkit/res/docs/zh/api/SIMD-API/basic_api/resource_management/TQue/AllocTensor_res.md#id1 -->

## Description<a name="section618mcpsimp"></a>

The header file path is: `"basic_api/kernel_tpipe.h"`.

Allocates a Tensor from the Que. The size occupied by the Tensor is the per-block memory length set during InitBuffer.

## Prototype<a name="section620mcpsimp"></a>

-   non-inplace API: Constructs a new Tensor as the memory-managed object.

    ```cpp
    template <typename T>
    __aicore__ inline LocalTensor<T> AllocTensor()
    ```

-   inplace API: Directly uses the passed-in Tensor as the memory-managed object, reducing the overhead of repeatedly creating tensors. For usage guidance, see [Tensor in-place operations](../../../../../guide/programming_guide/appendix/common_operations/tensor_inplace_performance.md).

    ```cpp
    template <typename T>
    __aicore__ inline void AllocTensor(LocalTensor<T>& tensor)
    ```

## Parameters<a name="section622mcpsimp"></a>

**Table 1** Template parameter description

<a name="table1550165916920"></a>
<table><thead align="left"><tr id="row115015591391"><th class="cellrowborder" valign="top" width="12.139999999999999%" id="mcps1.2.3.1.1"><p id="p12501159099"><a name="p12501159099"></a><a name="p12501159099"></a>Parameter</p>
</th>
<th class="cellrowborder" valign="top" width="87.86%" id="mcps1.2.3.1.2"><p id="p85019592918"><a name="p85019592918"></a><a name="p85019592918"></a>Description</p>
</th>
</tr>
</thead>
<tbody><tr id="row1550117591914"><td class="cellrowborder" valign="top" width="12.139999999999999%" headers="mcps1.2.3.1.1 "><p id="p185019592913"><a name="p185019592913"></a><a name="p185019592913"></a>T</p>
</td>
<td class="cellrowborder" valign="top" width="87.86%" headers="mcps1.2.3.1.2 "><p id="p12101541625"><a name="p12101541625"></a><a name="p12101541625"></a><span>Data type of the tensor. For supported types, see <a href="../../data_structures/LocalTensor/LocalTensor.md">LocalTensor</a>.</span></p>
</td>
</tr>
</tbody>
</table>

**Table 2**  Parameters

<a name="table181221135162517"></a>
<table><thead align="left"><tr id="row151221135112520"><th class="cellrowborder" valign="top" width="12.471247124712471%" id="mcps1.2.4.1.1"><p id="p1353754532512"><a name="p1353754532512"></a><a name="p1353754532512"></a>Parameter</p>
</th>
<th class="cellrowborder" valign="top" width="12.591259125912593%" id="mcps1.2.4.1.2"><p id="p1253774516259"><a name="p1253774516259"></a><a name="p1253774516259"></a>Input/Output</p>
</th>
<th class="cellrowborder" valign="top" width="74.93749374937492%" id="mcps1.2.4.1.3"><p id="p1653710452259"><a name="p1653710452259"></a><a name="p1653710452259"></a>Description</p>
</th>
</tr>
</thead>
<tbody><tr id="row12122235102511"><td class="cellrowborder" valign="top" width="12.471247124712471%" headers="mcps1.2.4.1.1 "><p id="p1537164502512"><a name="p1537164502512"></a><a name="p1537164502512"></a>tensor</p>
</td>
<td class="cellrowborder" valign="top" width="12.591259125912593%" headers="mcps1.2.4.1.2 "><p id="p1653714592515"><a name="p1653714592515"></a><a name="p1653714592515"></a>Input</p>
</td>
<td class="cellrowborder" valign="top" width="74.93749374937492%" headers="mcps1.2.4.1.3 "><p id="p165371945142512"><a name="p165371945142512"></a><a name="p165371945142512"></a>The inplace API requires passing a <a href="../../data_structures/LocalTensor/LocalTensor.md">LocalTensor</a> as the object for memory management.</p>
</td>
</tr>
</tbody>
</table>

## Constraints<a name="section633mcpsimp"></a>

-   For all queues on the same TPosition, the number of tensors applied for by consecutively calling the AllocTensor API is subject to a quantity constraint that varies with the AI processor model. This constraint must be satisfied when applying for buffers.
    <!-- npu="950" id10 -->
    - Ascend 950PR&950DT products: not exceeding 8.
    <!-- end id10 -->
    <!-- npu="A3" id11 -->
    - Atlas A3 products, not exceeding 8.
    <!-- end id11 -->
    <!-- npu="910b" id12 -->
    - Atlas A2 products, not exceeding 8.
    <!-- end id12 -->
    <!-- npu="310b" id13 -->
    - Atlas 200I/500 A2 inference products, not exceeding 8.
    <!-- end id13 -->
    <!-- npu="310p" id14 -->
    - Atlas inference products, not exceeding 8 on the AI Core.
    <!-- end id14 -->
    <!-- npu="310p" id15 -->
    - Atlas inference products, not exceeding 8 on the Vector Core.
    <!-- end id15 -->
    <!-- npu="910" id16 -->
    - Atlas training products, not exceeding 4.
    <!-- end id16 -->
    <!-- npu="x90" id17 -->
    - Kirin X90 processor products, not exceeding 8.
    <!-- end id17 -->
    <!-- npu="9030" id18 -->
    - Kirin 9030 processor products, not exceeding 8.
    <!-- end id18 -->    
-   The Tensor content allocated by the non-inplace API may contain random values.
-   For the non-inplace API, set the depth template parameter of TQueBind to a non-zero value; for the inplace API, set the depth template parameter of TQueBind to 0.

## Return Value<a name="section640mcpsimp"></a>

The non-inplace API returns a LocalTensor object, while the inplace API has no return value.

## Example<a name="section642mcpsimp"></a>

-   Example 1

    ```cpp
    // Allocate a tensor using AllocTensor.
    AscendC::TPipe pipe;
    AscendC::TQue<AscendC::TPosition::VECOUT, 2> que;
    int num = 2;
    int len = 1024;
    pipe.InitBuffer(que, num, len); // InitBuffer allocates 2 memory blocks, each 1024 bytes in size.
    AscendC::LocalTensor<half> tensor1 = que.AllocTensor<half>(); // AllocTensor allocates a tensor of 1024 bytes.
    ```

-   Example 2

    ```cpp
    // Example of a restricted scenario for consecutive use of AllocTensor:
    AscendC::TQue<AscendC::TPosition::VECIN, 1> que0;
    AscendC::TQue<AscendC::TPosition::VECIN, 1> que1;
    AscendC::TQue<AscendC::TPosition::VECIN, 1> que2;
    AscendC::TQue<AscendC::TPosition::VECIN, 1> que3;
    AscendC::TQue<AscendC::TPosition::VECIN, 1> que4;
    AscendC::TQue<AscendC::TPosition::VECIN, 1> que5;
    // Not recommended:
    // For example, an operator has 6 inputs and needs to allocate 6 buffers.
    // Allocate memory for them through 6 queues, que0 to que5, with each queue allocating 1 block. The total number of buffers allocated on the VECIN TPosition is 6.
    // Assume that the limit on the number of buffers consecutively allocated on the same TPosition is 4. Beyond this limit, using AllocTensor/FreeTensor causes resource allocation failure.
    // On the NPU, this may manifest as abnormal behavior such as a hang, and in the CPU Debug scenario, an error is reported.
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
    // The fifth AllocTensor causes resource allocation failure because the number of tensors simultaneously allocated on the same TPosition exceeds the limit of 4.
    AscendC::LocalTensor<T> local5 = que4.AllocTensor<T>();
    
    // In this case, resolve the issue as follows:
    // If multiple buffers are indeed required, merge them into a single buffer and use them through offsets.
    pipe.InitBuffer(que0, 1, len * 3);
    pipe.InitBuffer(que1, 1, len * 3);
    /*
     * Allocate a LocalTensor of three memory blocks. The address of local1 is the start address of the buffer in que0.
     * The address of local2 is the address of local1 offset by len, and the address of local3 is the address of local1 offset by
     * len * 2.
     */
    int32_t offset1 = len;
    int32_t offset2 = len * 2;
    AscendC::LocalTensor<T> local1 = que0.AllocTensor<T>();
    AscendC::LocalTensor<T> local2 = local1[offset1];
    AscendC::LocalTensor<T> local3 = local1[offset2];
    ```

-   Example 3: inplace API

    ```cpp
    AscendC::TPipe pipe;
    AscendC::TQue<AscendC::TPosition::VECIN, 0> que;
    int num = 2;
    int len = 1024;
    pipe.InitBuffer(que, num, len); // InitBuffer allocates 2 memory blocks, each 1024 bytes in size.
    AscendC::LocalTensor<half> tensor1;
    que.AllocTensor<half>(tensor1); // AllocTensor allocates a tensor of 1024 bytes.
    ```
