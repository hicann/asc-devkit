# SetQuit<a name="ZH-CN_TOPIC_0000001996865757"></a>

<!-- md-trans-meta sourceCommit=e3c8c70b3f5d27f3770063ae4b535bc75d904e3e translatedAt=2026-08-27T17:36:49.579Z -->

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

After the message space address is obtained through the **AllocMessage** API, an exit message is sent to inform the AIC corresponding to the message queue that it does not need to process messages in this queue. As shown in the following figure, after the AIV corresponding to Queue5 sends an exit message, Block1 no longer processes any messages in Queue5.

**Figure 1**  Message queue exit indication<a name="fig915491163918"></a>  
![](../../../../figures/msg_queue_exit_diagram.png "Message queue exit indication")

## Prototype<a name="section765814724715"></a>

```
__aicore__ inline void SetQuit(__gm__ CubeMsgType* msg)
```

## Parameters<a name="zh-cn_topic_0000001526206862_section129451113125413"></a>

**Table 1** API parameter description

<a name="zh-cn_topic_0000001526206862_zh-cn_topic_0000001389783361_table111938719446"></a>
<table><thead align="left"><tr id="zh-cn_topic_0000001526206862_zh-cn_topic_0000001389783361_row6223476444"><th class="cellrowborder" valign="top" width="9.09%" id="mcps1.2.4.1.1"><p id="zh-cn_topic_0000001526206862_zh-cn_topic_0000001389783361_p10223674448"><a name="zh-cn_topic_0000001526206862_zh-cn_topic_0000001389783361_p10223674448"></a><a name="zh-cn_topic_0000001526206862_zh-cn_topic_0000001389783361_p10223674448"></a>Parameter</p></th>
<th class="cellrowborder" valign="top" width="9.01%" id="mcps1.2.4.1.2"><p id="zh-cn_topic_0000001526206862_zh-cn_topic_0000001389783361_p645511218169"><a name="zh-cn_topic_0000001526206862_zh-cn_topic_0000001389783361_p645511218169"></a><a name="zh-cn_topic_0000001526206862_zh-cn_topic_0000001389783361_p645511218169"></a>Input/Output</p></th>
<th class="cellrowborder" valign="top" width="81.89999999999999%" id="mcps1.2.4.1.3"><p id="zh-cn_topic_0000001526206862_zh-cn_topic_0000001389783361_p1922337124411"><a name="zh-cn_topic_0000001526206862_zh-cn_topic_0000001389783361_p1922337124411"></a><a name="zh-cn_topic_0000001526206862_zh-cn_topic_0000001389783361_p1922337124411"></a>Description</p></th>
</tr>
</thead>
<tbody><tr id="zh-cn_topic_0000001526206862_zh-cn_topic_0000001389783361_row152234713443"><td class="cellrowborder" valign="top" width="9.09%" headers="mcps1.2.4.1.1 "><p id="p153103773911"><a name="p153103773911"></a><a name="p153103773911"></a>**msg**</p></td>
<td class="cellrowborder" valign="top" width="9.01%" headers="mcps1.2.4.1.2 "><p id="p1831010712394"><a name="p1831010712394"></a><a name="p1831010712394"></a>Input</p></td>
<td class="cellrowborder" valign="top" width="81.89999999999999%" headers="mcps1.2.4.1.3 "><p id="p203101270393"><a name="p203101270393"></a><a name="p203101270393"></a>Message space address in this **CubeResGroupHandle**.</p></td>
</tr>
</tbody>
</table>

## Return Value<a name="section640mcpsimp"></a>

None

## Constraints<a name="zh-cn_topic_0000001526206862_section65498832"></a>

None

## Example<a name="zh-cn_topic_0000001526206862_section97001499599"></a>

```
handle.AssignQueue(queIdx);  
auto msgPtr = a.AllocMessage();        // Obtain the message space pointer msgPtr.
handle.SetQuit(msgPtr);              // Send the exit message.
```

