# PostFakeMsg<a name="ZH-CN_TOPIC_0000001960384800"></a>

<!-- md-trans-meta sourceCommit=e3c8c70b3f5d27f3770063ae4b535bc75d904e3e translatedAt=2026-08-27T17:35:36.328Z -->

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

After obtaining the message space address through the **AllocMessage** API, the AIV sends a fake message and refreshes the message state **msgState** to **FAKE**.

When the message content of multiple AIVs is identical, the AIC needs to read only the first message at the front position once. By setting the user-defined parameter **skipCnt** in the message structure to n, the AIC is notified that the subsequent n messages do not need to be processed and can be skipped directly. The skipped AIVs need to use this API to send fake messages. This is called the message merging mechanism or message merging scenario.

As shown in the following figure, assume that the 0th message of Queue1, 2, and 3 is the same as the 0th message of Queue0. In the message merging scenario, from the perspective of the AIC, the messages of Queue0\(0\) and Queue4\(0\) are processed, and the corresponding computation on the AIC is completed based on the user-defined message content. Because Queue1\(0\), Queue2\(0\), and Queue3\(0\) send fake messages, the AIC does not read the message content for computation and directly releases the messages.

**Figure 1**  PostFakeMessage schematic diagram<a name="fig6289195515216"></a>  
![](../../../../figures/post_fake_message_diagram.png "PostFakeMessage schematic diagram")

## Prototype<a name="section765814724715"></a>

```
__aicore__ inline uint16_t PostFakeMsg(__gm__ CubeMsgType* msg)
```

## Parameters<a name="zh-cn_topic_0000001526206862_section129451113125413"></a>

**Table 1**  API parameter description

<a name="zh-cn_topic_0000001526206862_zh-cn_topic_0000001389783361_table111938719446"></a>
<table><thead align="left"><tr id="zh-cn_topic_0000001526206862_zh-cn_topic_0000001389783361_row6223476444"><th class="cellrowborder" valign="top" width="9.09%" id="mcps1.2.4.1.1"><p id="zh-cn_topic_0000001526206862_zh-cn_topic_0000001389783361_p10223674448"><a name="zh-cn_topic_0000001526206862_zh-cn_topic_0000001389783361_p10223674448"></a><a name="zh-cn_topic_0000001526206862_zh-cn_topic_0000001389783361_p10223674448"></a>Parameter</p></th>
<th class="cellrowborder" valign="top" width="7.93%" id="mcps1.2.4.1.2"><p id="zh-cn_topic_0000001526206862_zh-cn_topic_0000001389783361_p645511218169"><a name="zh-cn_topic_0000001526206862_zh-cn_topic_0000001389783361_p645511218169"></a><a name="zh-cn_topic_0000001526206862_zh-cn_topic_0000001389783361_p645511218169"></a>Input/Output</p></th>
<th class="cellrowborder" valign="top" width="82.98%" id="mcps1.2.4.1.3"><p id="zh-cn_topic_0000001526206862_zh-cn_topic_0000001389783361_p1922337124411"><a name="zh-cn_topic_0000001526206862_zh-cn_topic_0000001389783361_p1922337124411"></a><a name="zh-cn_topic_0000001526206862_zh-cn_topic_0000001389783361_p1922337124411"></a>Description</p></th>
</tr>
</thead>
<tbody><tr id="zh-cn_topic_0000001526206862_zh-cn_topic_0000001389783361_row152234713443"><td class="cellrowborder" valign="top" width="9.09%" headers="mcps1.2.4.1.1 "><p id="zh-cn_topic_0000001526206862_zh-cn_topic_0000001389783361_p2340183613156"><a name="zh-cn_topic_0000001526206862_zh-cn_topic_0000001389783361_p2340183613156"></a><a name="zh-cn_topic_0000001526206862_zh-cn_topic_0000001389783361_p2340183613156"></a>msg</p></td>
<td class="cellrowborder" valign="top" width="7.93%" headers="mcps1.2.4.1.2 "><p id="p19741912147"><a name="p19741912147"></a><a name="p19741912147"></a>Input</p></td>
<td class="cellrowborder" valign="top" width="82.98%" headers="mcps1.2.4.1.3 "><p id="zh-cn_topic_0000001526206862_zh-cn_topic_0000001389783361_p2684123934216"><a name="zh-cn_topic_0000001526206862_zh-cn_topic_0000001389783361_p2684123934216"></a><a name="zh-cn_topic_0000001526206862_zh-cn_topic_0000001389783361_p2684123934216"></a>Message space address of a task in this CubeResGroupHandle.</p></td>
</tr>
</tbody>
</table>

## Return Value<a name="section640mcpsimp"></a>

Returns the address offset between the current message space and the space at the head of the message queue.

## Constraints<a name="zh-cn_topic_0000001526206862_section65498832"></a>

None

## Example<a name="zh-cn_topic_0000001526206862_section97001499599"></a>

```
hanndle.AssignQueue(queIdx);  
auto msgPtr = handle.AllocMessage();        // Obtain the message space pointer msgPtr.
auto offset = handle.PostFakeMsg(msgPtr);           // Send a fake message at the position pointed to by msgPtr.
```

