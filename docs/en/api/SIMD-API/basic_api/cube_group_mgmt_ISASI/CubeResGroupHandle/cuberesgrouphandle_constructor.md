# CubeResGroupHandle Constructor<a name="ZH-CN_TOPIC_0000001965821852"></a>

<!-- md-trans-meta sourceCommit=unknown translatedAt=2026-08-31T11:54:47.731Z -->

## Applicable Products<a name="section1550532418810"></a>

<!-- npu="950" id1 -->
- Ascend 950PR/Ascend 950DT: Supported
<!-- end id1 -->
<!-- npu="A3" id2 -->
- Atlas A3 training products/Atlas A3 inference products: Not supported
<!-- end id2 -->
<!-- npu="910b" id3 -->
- Atlas A2 training products/Atlas A2 inference products: Supported
<!-- end id3 -->
<!-- npu="310b" id4 -->
- Atlas 200I/500 A2 inference products: Not supported
<!-- end id4 -->
<!-- npu="310p" id5 -->
- Atlas inference products AI Core: Not supported
<!-- end id5 -->
<!-- npu="310p" id6 -->
- Atlas inference products Vector Core: Not supported
<!-- end id6 -->
<!-- npu="910" id7 -->
- Atlas training products: Not supported
<!-- end id7 -->


## Description<a name="zh-cn_topic_0000001526206862_section212607105720"></a>

Constructs a **CubeResGroupHandle** object to allocate AICs and message queues within the group. When constructing a **CubeResGroupHandle** object, you need to pass in the template parameter **CubeMsgType**, which is a user-defined message structure. For details, see [Table 1](cuberesgrouphandle_usage_description.md#table189051237164018). This API requires you to manage addresses, synchronization events, and so on by yourself. Therefore, it is recommended to use [CreateCubeResGroup](cuberesgrouphandle_constructor.md) to quickly create a **CubeResGroupHandle** object.

## Prototype<a name="section765814724715"></a>

```
template <typename CubeMsgType>
class CubeResGroupHandle;
__aicore__ inline CubeResGroupHandle() = default
__aicore__ inline CubeResGroupHandle(GM_ADDR workspace, uint8_t blockStart, uint8_t blockSize, uint8_t msgQueueSize, uint8_t evtIDIn)
```

## Parameters<a name="zh-cn_topic_0000001526206862_section129451113125413"></a>

**Table 1**  CubeResGroupHandle parameters

<a name="zh-cn_topic_0000001526206862_zh-cn_topic_0000001389783361_table111938719446"></a>
<table><thead align="left"><tr id="zh-cn_topic_0000001526206862_zh-cn_topic_0000001389783361_row6223476444"><th class="cellrowborder" valign="top" width="17.22%" id="mcps1.2.4.1.1"><p id="zh-cn_topic_0000001526206862_zh-cn_topic_0000001389783361_p10223674448"><a name="zh-cn_topic_0000001526206862_zh-cn_topic_0000001389783361_p10223674448"></a><a name="zh-cn_topic_0000001526206862_zh-cn_topic_0000001389783361_p10223674448"></a>Parameter</p></th>
<th class="cellrowborder" valign="top" width="15.340000000000002%" id="mcps1.2.4.1.2"><p id="zh-cn_topic_0000001526206862_zh-cn_topic_0000001389783361_p645511218169"><a name="zh-cn_topic_0000001526206862_zh-cn_topic_0000001389783361_p645511218169"></a><a name="zh-cn_topic_0000001526206862_zh-cn_topic_0000001389783361_p645511218169"></a>Input/Output</p></th>
<th class="cellrowborder" valign="top" width="67.44%" id="mcps1.2.4.1.3"><p id="zh-cn_topic_0000001526206862_zh-cn_topic_0000001389783361_p1922337124411"><a name="zh-cn_topic_0000001526206862_zh-cn_topic_0000001389783361_p1922337124411"></a><a name="zh-cn_topic_0000001526206862_zh-cn_topic_0000001389783361_p1922337124411"></a>Description</p></th>
</tr>
</thead>
<tbody><tr id="zh-cn_topic_0000001526206862_zh-cn_topic_0000001389783361_row152234713443"><td class="cellrowborder" valign="top" width="17.22%" headers="mcps1.2.4.1.1 "><p id="zh-cn_topic_0000001526206862_zh-cn_topic_0000001389783361_p2340183613156"><a name="zh-cn_topic_0000001526206862_zh-cn_topic_0000001389783361_p2340183613156"></a><a name="zh-cn_topic_0000001526206862_zh-cn_topic_0000001389783361_p2340183613156"></a>**workspace**</p></td>
<td class="cellrowborder" valign="top" width="15.340000000000002%" headers="mcps1.2.4.1.2 "><p id="p19741912147"><a name="p19741912147"></a><a name="p19741912147"></a>Input</p></td>
<td class="cellrowborder" valign="top" width="67.44%" headers="mcps1.2.4.1.3 "><p id="p17175015525"><a name="p17175015525"></a><a name="p17175015525"></a>Start address of the message communication area of this CubeResGroupHandle on GM.</p></td>
</tr>
<tr id="zh-cn_topic_0000001526206862_row1239183183016"><td class="cellrowborder" valign="top" width="17.22%" headers="mcps1.2.4.1.1 "><p id="zh-cn_topic_0000001526206862_p223953193015"><a name="zh-cn_topic_0000001526206862_p223953193015"></a><a name="zh-cn_topic_0000001526206862_p223953193015"></a>**blockStart**</p></td>
<td class="cellrowborder" valign="top" width="15.340000000000002%" headers="mcps1.2.4.1.2 "><p id="zh-cn_topic_0000001526206862_p7239938308"><a name="zh-cn_topic_0000001526206862_p7239938308"></a><a name="zh-cn_topic_0000001526206862_p7239938308"></a>Input</p></td>
<td class="cellrowborder" valign="top" width="67.44%" headers="mcps1.2.4.1.3 "><p id="p1117515154210"><a name="p1117515154210"></a><a name="p1117515154210"></a>Sequence number of the starting AIC corresponding to this CubeResGroupHandle from the AIV perspective, that is, the starting sequence number of the AIC multiplied by 2. For example, if the starting sequence number of the AIC is 0, fill in 0*2; if it is 1, fill in 1*2.</p></td>
</tr>
<tr id="row9374154371313"><td class="cellrowborder" valign="top" width="17.22%" headers="mcps1.2.4.1.1 "><p id="p11374343181311"><a name="p11374343181311"></a><a name="p11374343181311"></a>**blockSize**</p></td>
<td class="cellrowborder" valign="top" width="15.340000000000002%" headers="mcps1.2.4.1.2 "><p id="p146153901420"><a name="p146153901420"></a><a name="p146153901420"></a>Input</p></td>
<td class="cellrowborder" valign="top" width="67.44%" headers="mcps1.2.4.1.3 "><p id="p517518151222"><a name="p517518151222"></a><a name="p517518151222"></a>Number of Blocks allocated to this CubeResGroupHandle from the AIV perspective, that is, the actual number of AICs multiplied by 2.</p></td>
</tr>
<tr id="row15285204611313"><td class="cellrowborder" valign="top" width="17.22%" headers="mcps1.2.4.1.1 "><p id="p15285194613135"><a name="p15285194613135"></a><a name="p15285194613135"></a>**msgQueueSize**</p></td>
<td class="cellrowborder" valign="top" width="15.340000000000002%" headers="mcps1.2.4.1.2 "><p id="p9285104615133"><a name="p9285104615133"></a><a name="p9285104615133"></a>Input</p></td>
<td class="cellrowborder" valign="top" width="67.44%" headers="mcps1.2.4.1.3 "><p id="p101756155210"><a name="p101756155210"></a><a name="p101756155210"></a>Total number of message queues allocated to this CubeResGroupHandle.</p></td>
</tr>
<tr id="row155491219122113"><td class="cellrowborder" valign="top" width="17.22%" headers="mcps1.2.4.1.1 "><p id="p1250414522214"><a name="p1250414522214"></a><a name="p1250414522214"></a>**evtIDIn**</p></td>
<td class="cellrowborder" valign="top" width="15.340000000000002%" headers="mcps1.2.4.1.2 "><p id="p17504115215215"><a name="p17504115215215"></a><a name="p17504115215215"></a>Input</p></td>
<td class="cellrowborder" valign="top" width="67.44%" headers="mcps1.2.4.1.3 "><p id="p155041252152119"><a name="p155041252152119"></a><a name="p155041252152119"></a>Synchronization event used for AIV-side messages within the communication framework.</p></td>
</tr>
</tbody>
</table>

As shown in the following figure, for CubeResGroupHandle1, **blockStart** is 4 and **blockSize** is 4, indicating that the starting AIC number is 2, that is, **blockStart** / 2; the number of AICs is 2, that is, **blockSize** / 2. **msgQueueSize** is 10, indicating that the number of message queues is 10, and the number of message queues allocated to each Block is Ceil\(**msgQueueSize**, **blockSize**/2\). Both Block2 and Block3 are allocated 5 message queues. For CubeResGroupHandle2, **msgQueueSize** is 11, and the last Block can be allocated only 5 message queues.

**Figure 1**  Block and message queue mapping diagram<a name="fig38005710136"></a>  
![](../../../../figures/block_msg_queue_mapping.png "Block and message queue mapping diagram")

## Constraints<a name="zh-cn_topic_0000001526206862_section65498832"></a>

-   Assuming that the number of AIV cores on the chip is x, then blockStart + blockSize <= x - 1, and msgQueueSize <= x.
-   Each AIC is allocated at least one message queue **msgQueue**.
-   **blockStart** and **blockSize** must be even numbers.
-   When this API is used, the last 1600B + sizeof\(CubeMsgType\) of the UB space is occupied.
-   One AIC can belong to only one **CubeGroupHandle**, that is, the \[blockStart / 2, blockStart / 2 + blockSize / 2\] intervals of multiple **CubeGroupHandle** objects must not overlap.
-   This API cannot be used together with [REGIST\_MATMUL\_OBJ](../../../high-level API/cube computation/Matmul-Kernel-side API/REGIST_MATMUL_OBJ.md). When the resource management API is used, the user manages the inter-core communication between AIC and AIV. In **REGIST\_MATMUL\_OBJ**, the framework manages the inter-core communication between AIC and AIV. Using them together may cause exceptions such as incorrect communication messages.

## Example<a name="zh-cn_topic_0000001526206862_section97001499599"></a>

```
uint8_t blockStart = 4;
uint8_t blockSize = 4;
uint8_t msgQueueSize = 10;
uint8_t evtIDIn = 0; //Manage the event ID by yourself.
AscendC::KfcWorkspace desc(workspace); // Workspace pointer managed by the user.
AscendC::CubeResGroupHandle<CubeMsgBody> handle;
handle = AscendC::CubeResGroupHandle<MatmulApiType, MyCallbackFunc, CubeMsgBody>(desc.GetMsgStart(), blockStart, blockSize, msgQueueSize, evtIDIn); 
```

