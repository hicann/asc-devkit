# TQue Overview<a name="ZH-CN_TOPIC_0000001617811650"></a>

<!-- md-trans-meta sourceCommit=2bc2b38d5648dc61f632186b01ba7f36c2295b13 translatedAt=2026-08-27T14:47:26.125Z -->

Pipeline tasks communicate and synchronize with each other through queues. **TQue** is a data structure used to perform queue-related operations and manage related resources. **TQue** inherits from the **TQueBind** parent class, with the inheritance relationship as follows:

![](../../../../figures/enque.png)

## Template Parameters<a name="section18341144185913"></a>

```
template <TPosition pos, int32_t depth, auto mask = 0> class TQue{...};
```

**Table 1** TQue template parameters

<a name="table1550165916920"></a>
<table><thead align="left"><tr id="row115015591391"><th class="cellrowborder" valign="top" width="14.099999999999998%" id="mcps1.2.3.1.1"><p id="p12501159099"><a name="p12501159099"></a><a name="p12501159099"></a>Parameter</p></th>
<th class="cellrowborder" valign="top" width="85.9%" id="mcps1.2.3.1.2"><p id="p85019592918"><a name="p85019592918"></a><a name="p85019592918"></a>Description</p></th>
</tr>
</thead>
<tbody><tr id="row1550117591914"><td class="cellrowborder" valign="top" width="14.099999999999998%" headers="mcps1.2.3.1.1 "><p id="p185019592913"><a name="p185019592913"></a><a name="p185019592913"></a>**pos**</p></td>
<td class="cellrowborder" valign="top" width="85.9%" headers="mcps1.2.3.1.2 "><p id="p35011591693"><a name="p35011591693"></a><a name="p35011591693"></a>Logical position of the queue, which can be <span>VECIN, VECOUT,</span> A1<span>, </span>A2<span>, </span>B1<span>, </span>B2<span>, </span>CO1<span>, </span>CO2. <span>For details about TPosition, see</span> <a href="../../aux_data_structures/TPosition.md">TPosition</a>.</p></td>
</tr>
<tr id="row12501859799"><td class="cellrowborder" valign="top" width="14.099999999999998%" headers="mcps1.2.3.1.1 "><p id="p1650113599915"><a name="p1650113599915"></a><a name="p1650113599915"></a>**depth**</p></td>
<td class="cellrowborder" valign="top" width="85.9%" headers="mcps1.2.3.1.2 "><p id="p8510214185418"><a name="p8510214185418"></a><a name="p8510214185418"></a>Queue depth indicates the number of consecutive enqueue/dequeue operations that the queue can perform. At runtime, if there are n consecutive EnQue operations on the same queue (with no DeQue in between), the queue depth must be set to n.</p>
<p id="p7674135716528"><a name="p7674135716528"></a><a name="p7674135716528"></a>Note that the queue depth here is unrelated to double buffer. The queue mechanism is used to implement pipeline parallelism, and double buffer further improves pipeline utilization on this basis. Even if the queue depth is 1, double buffer can still be enabled.</p>
<p id="p1334591855410"><a name="p1334591855410"></a><a name="p1334591855410"></a>In non-Tensor in-place operation scenarios, when the queue depth is set to 1, the compiler applies special optimization to this scenario, which usually yields better performance. <strong id="b0597122135416"><a name="b0597122135416"></a><a name="b0597122135416"></a>It is recommended to set it to 1</strong>.</p>
<p id="p1167810565476"><a name="p1167810565476"></a><a name="p1167810565476"></a>In <span id="ph280915815592"><a name="ph280915815592"></a><a name="ph280915815592"></a><a href="https://gitcode.com/cann/asc-devkit/blob/9.1.0/docs/en/guide/programming_guide/appendix/common_operations/how_to_use_tensor_in_place_operations_to_improve_operator_performance.md" target="_blank" rel="noopener noreferrer">Tensor in-place operation</a></span> scenarios, it must be set to 0.</p>
<a name="ul26757572527"></a><a name="ul26757572527"></a><ul id="ul26757572527"><li>In the following example, the queue has no consecutive enqueue operations, so the queue depth is set to 1.<a name="screen17724335125812"></a><a name="screen17724335125812"></a><pre class="screen" codetype="Cpp" id="screen17724335125812">a1 = que.AllocTensor(); 
que.EnQue(a1);
a1 = que.DeQue();
que.FreeTensor(a1);</pre>
</li></ul>
<a name="ul12675135785214"></a><a name="ul12675135785214"></a><ul id="ul12675135785214"><li>In the following example, the queue performs two consecutive enqueue operations, so the queue depth should be set to 2. This may be used only in very rare preload scenarios (for example, loading two batches of data consecutively, processing one batch, loading another batch after completion, and then processing the batch loaded in advance, and so on). In all other cases, depth &gt;= 2 is not recommended.<a name="screen11973229582"></a><a name="screen11973229582"></a><pre class="screen" codetype="Cpp" id="screen11973229582">a1 = que.AllocTensor(); 
a2 = que.AllocTensor();
que.EnQue(a1);
que.EnQue(a2);
a1 = que.DeQue();
a2 = que.DeQue(); 
que.FreeTensor(a1);
que.FreeTensor(a2);</pre>
</li></ul></td>
</tr>
<tr id="row3501135910920"><td class="cellrowborder" valign="top" width="14.099999999999998%" headers="mcps1.2.3.1.1 "><p id="p6501175912914"><a name="p6501175912914"></a><a name="p6501175912914"></a>**mask**</p></td>
<td class="cellrowborder" valign="top" width="85.9%" headers="mcps1.2.3.1.2 "><a name="ul4317543497"></a><a name="ul4317543497"></a><ul id="ul4317543497"><li>When mask is of the int type, bit positions are used to express information:<a name="ul196262325012"></a><a name="ul196262325012"></a><ul id="ul196262325012"><li>When bit 0 is 1, the data format is converted from ND to NZ, and TPosition only supports A1 or B1;</li><li>When bit 1 is 1, the data format is converted from NZ to ND, and TPosition only supports CO2.</li></ul>
<p id="p17324473167"><a name="p17324473167"></a><a name="p17324473167"></a>The supported models are as follows:</p>
<p id="p9732447121610"><a name="p9732447121610"></a><a name="p9732447121610"></a><span id="ph1429142166"><a name="ph1429142166"></a><a name="ph1429142166"></a><term id="zh-cn_topic_0000001312391781_term1964153212227"><a name="zh-cn_topic_0000001312391781_term1964153212227"></a><a name="zh-cn_topic_0000001312391781_term1964153212227"></a>Atlas inference products</term>AI Core</span></p>
</li><li>When mask is of the const TQueConfig* type, the TQueConfig structure definition and parameter description are as follows. For an example, see <a href="#section45805354920">Example</a>:<a name="screen13896155731115"></a><a name="screen13896155731115"></a><pre class="screen" codetype="Cpp" id="screen13896155731115">struct TQueConfig {
    bool nd2nz = false;  // true indicates that the data format is converted from ND to NZ. Only supports TPosition A1 or B1. Default: false.
    bool nz2nd = false;  // true indicates that the data format is converted from NZ to ND. Only supports TPosition CO2. Default: false.
    bool scmBlockGroup = false;  // TSCM-related parameter. Reserved Parameter. Default: false.
    uint32_t bufferLen = 0;  // Keep consistent with the len parameter input during InitBuffer. This enables compile-time performance optimization. <strong id="b1084715241371"><a name="b1084715241371"></a><a name="b1084715241371"></a>Passing 0 indicates that resource allocation is performed during InitBuffer</strong>.
    uint32_t bufferNumber = 0;  // Keep consistent with the num parameter input during InitBuffer. This enables compile-time performance optimization. <strong id="b132591319977"><a name="b132591319977"></a><a name="b132591319977"></a>Passing 0 indicates that resource allocation is performed during InitBuffer</strong>.
    uint32_t consumerSize = 0;  // Reserved Parameter.
    TPosition consumer[8] = {}; // Reserved Parameter.
    bool enableStaticEvtId = false; // Reserved Parameter.
    bool enableLoopQueue = false;   // Reserved Parameter.
};</pre>
<p id="p8601939151319"><a name="p8601939151319"></a><a name="p8601939151319"></a>The models that support the ND and NZ format conversion parameters described above are as follows:</p>
<p id="p11601163917138"><a name="p11601163917138"></a><a name="p11601163917138"></a><span id="ph760153981316"><a name="ph760153981316"></a><a name="ph760153981316"></a><term id="zh-cn_topic_0000001312391781_term1964153212227_1"><a name="zh-cn_topic_0000001312391781_term1964153212227_1"></a><a name="zh-cn_topic_0000001312391781_term1964153212227_1"></a>Atlas inference products</term>AI Core</span></p>
</li></ul></td>
</tr>
</tbody>
</table>

## TQue Buffer Limitations<a name="section466543213575"></a>

Because the Buffers allocated by **TQue** store synchronization event IDs, the quantity of **TQue** Buffers at the same **TPosition** is related to the hardware synchronization event IDs.

For Atlas training products, the quantity of eventIDs is 4.

For the AI Core of Atlas inference products, the quantity of eventIDs is 8.

For the Vector Core of Atlas inference products, the quantity of eventIDs is 8.

For Atlas A2 training products/Atlas A2 inference products, the quantity of eventIDs is 8.

For Atlas A3 training products/Atlas A3 inference products, the quantity of eventIDs is 8.

For Atlas 200I/500 A2 inference products, the quantity of eventIDs is 8.

The maximum quantity of **TQue** Buffers is also 8 or 4, respectively, meaning that the number of synchronization events that can be inserted is 8 or 4. When applying for **TQue** using **TPipe**'s **InitBuffer**, the quantity of Buffers is limited, and the maximum number of **TQue** that can be applied for is 8 or 4, respectively.

If the **TQue** Buffers used simultaneously exceed the limit, no further **TQue** can be applied for. To continue applying, you can call the **FreeAllEvent** API to release some temporarily unused **TQue**. After finishing using the corresponding **TQue**, call this API to release all events in the corresponding queue, after which **TQue** can be applied for again. The following is an example:

-   Without double buffer

    ```
    // The maximum number of buffers that can be applied for on the VECIN position is 8. If this limit is exceeded, resource allocation may fail when AllocTensor/FreeTensor is used later. Therefore, when double buffer is not enabled, a maximum of 8 TQues can be applied for.
    AscendC::TPipe pipe;
    int len = 1024;
    AscendC::TQue<AscendC::TPosition::VECIN, 1> que0;
    AscendC::TQue<AscendC::TPosition::VECIN, 1> que1;
    AscendC::TQue<AscendC::TPosition::VECIN, 1> que2;
    AscendC::TQue<AscendC::TPosition::VECIN, 1> que3;
    AscendC::TQue<AscendC::TPosition::VECIN, 1> que4;
    AscendC::TQue<AscendC::TPosition::VECIN, 1> que5;
    AscendC::TQue<AscendC::TPosition::VECIN, 1> que6;
    AscendC::TQue<AscendC::TPosition::VECIN, 1> que7;
     
    pipe.InitBuffer(que0, 1, len);
    pipe.InitBuffer(que1, 1, len);
    pipe.InitBuffer(que2, 1, len);
    pipe.InitBuffer(que3, 1, len);
    pipe.InitBuffer(que4, 1, len);
    pipe.InitBuffer(que5, 1, len);
    pipe.InitBuffer(que6, 1, len);
    pipe.InitBuffer(que7, 1, len);
    ```

-   With double buffer

    ```
    // If double buffer is enabled, each TQue allocates 2 memory blocks, so a maximum of 4 TQues can be applied for.
    AscendC::TPipe pipe;
    int len = 1024;
    AscendC::TQue<AscendC::TPosition::VECIN, 1> que0;
    AscendC::TQue<AscendC::TPosition::VECIN, 1> que1;
    AscendC::TQue<AscendC::TPosition::VECIN, 1> que2;
    AscendC::TQue<AscendC::TPosition::VECIN, 1> que3;
     
    pipe.InitBuffer(que0, 2, len);
    pipe.InitBuffer(que1, 2, len);
    pipe.InitBuffer(que2, 2, len);
    pipe.InitBuffer(que3, 2, len);
    ```

-   Applying for TQue multiple times

    ```
    // If the number of TQues has reached the maximum, call the FreeAllEvent API to continue applying for TQues.
    AscendC::TPipe pipe;
    int len = 1024;
    AscendC::TQue<AscendC::TPosition::VECIN, 1> que0;
    pipe.InitBuffer(que0, 1, len);
    AscendC::LocalTensor<half> tensor1 = que0.AllocTensor<half>();
    que0.EnQue(tensor1);
    tensor1 = que0.DeQue<half>(); // Move the tensor out of the VECOUT queue.
    que0.FreeTensor<half>(tensor1);
    que0.FreeAllEvent(); // Release all synchronization events of que0, after which TQues can be applied for again.
    AscendC::TQue<AscendC::TPosition::VECIN, 1> que1;
    pipe.InitBuffer(que1, 1, len);
    ```

## Example<a name="section45805354920"></a>

The following example enables compile-time calculation of **bufferNumber** by passing in **TQueConfig**. The vector operator does not involve data format conversion, so **nd2nz** and **nz2nd** are **false**.

```
// User-defined metafunction for constructing TQueConfig.
__aicore__ constexpr AscendC::TQueConfig GetMyTQueConfig(bool nd2nzIn, bool nz2ndIn, bool scmBlockGroupIn,
    uint32_t bufferLenIn, uint32_t bufferNumberIn, uint32_t consumerSizeIn, const AscendC::TPosition consumerIn[])
{
    return {
        .nd2nz = nd2nzIn,
        .nz2nd = nz2ndIn,
        .scmBlockGroup = scmBlockGroupIn,
        .bufferLen = bufferLenIn,
        .bufferNumber = bufferNumberIn,
        .consumerSize = consumerSizeIn,
        .consumer = {consumerIn[0], consumerIn[1], consumerIn[2], consumerIn[3],
            consumerIn[4], consumerIn[5], consumerIn[6], consumerIn[7]}
    };
}
static constexpr AscendC::TPosition tp[8] = {AscendC::TPosition::MAX, AscendC::TPosition::MAX, AscendC::TPosition::MAX, AscendC::TPosition::MAX,
            AscendC::TPosition::MAX, AscendC::TPosition::MAX, AscendC::TPosition::MAX, AscendC::TPosition::MAX};
static constexpr AscendC::TQueConfig conf = GetMyTQueConfig(false, false, false, 0, 1, 0, tp);
...
AscendC::TPipe pipe;
AscendC::TQue<AscendC::TPosition::VECIN, 1, &conf> inQueueX;
AscendC::TQue<AscendC::TPosition::VECOUT, 1, &conf> outQueue;
pipe.InitBuffer(inQueueX, 1, dataSize * sizeof(srcType));
pipe.InitBuffer(outQueue, 1, dataSize * sizeof(int8_t));
```
