# SetSkipMsg<a name="ZH-CN_TOPIC_0000001960384808"></a>

<!-- md-trans-meta sourceCommit=e3c8c70b3f5d27f3770063ae4b535bc75d904e3e translatedAt=2026-08-27T17:37:14.889Z -->

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

The AIC skips the processing of a specified number of fake messages, and this API can be called only in the callback function. In the following figure, Block0 skips three fake messages by calling **SetSkipMsg**.

**Figure 1**  SetSkipMsg schematic diagram<a name="fig1732521102711"></a>  
![](../../../../figures/set_skip_msg_diagram.png "SetSkipMsg schematic diagram")

## Prototype<a name="section765814724715"></a>

```
 __aicore__ inline void SetSkipMsg(uint8_t skipCnt)
```

## Parameters<a name="zh-cn_topic_0000001526206862_section129451113125413"></a>

**Table 1** API parameter description

<a name="zh-cn_topic_0000001526206862_zh-cn_topic_0000001389783361_table111938719446"></a>
<table><thead align="left"><tr id="zh-cn_topic_0000001526206862_zh-cn_topic_0000001389783361_row6223476444"><th class="cellrowborder" valign="top" width="9.09%" id="mcps1.2.4.1.1"><p id="zh-cn_topic_0000001526206862_zh-cn_topic_0000001389783361_p10223674448"><a name="zh-cn_topic_0000001526206862_zh-cn_topic_0000001389783361_p10223674448"></a><a name="zh-cn_topic_0000001526206862_zh-cn_topic_0000001389783361_p10223674448"></a>Parameter</p></th>
<th class="cellrowborder" valign="top" width="7.93%" id="mcps1.2.4.1.2"><p id="zh-cn_topic_0000001526206862_zh-cn_topic_0000001389783361_p645511218169"><a name="zh-cn_topic_0000001526206862_zh-cn_topic_0000001389783361_p645511218169"></a><a name="zh-cn_topic_0000001526206862_zh-cn_topic_0000001389783361_p645511218169"></a>Input/Output</p></th>
<th class="cellrowborder" valign="top" width="82.98%" id="mcps1.2.4.1.3"><p id="zh-cn_topic_0000001526206862_zh-cn_topic_0000001389783361_p1922337124411"><a name="zh-cn_topic_0000001526206862_zh-cn_topic_0000001389783361_p1922337124411"></a><a name="zh-cn_topic_0000001526206862_zh-cn_topic_0000001389783361_p1922337124411"></a>Description</p></th>
</tr>
</thead>
<tbody><tr id="zh-cn_topic_0000001526206862_zh-cn_topic_0000001389783361_row152234713443"><td class="cellrowborder" valign="top" width="9.09%" headers="mcps1.2.4.1.1 "><p id="p0731542025"><a name="p0731542025"></a><a name="p0731542025"></a>**skipCnt**</p></td>
<td class="cellrowborder" valign="top" width="7.93%" headers="mcps1.2.4.1.2 "><p id="p10731044217"><a name="p10731044217"></a><a name="p10731044217"></a>Input</p></td>
<td class="cellrowborder" valign="top" width="82.98%" headers="mcps1.2.4.1.3 "><p id="p373442029"><a name="p373442029"></a><a name="p373442029"></a>Number of messages that the **AIC** needs to skip.</p></td>
</tr>
</tbody>
</table>

## Return Value<a name="section640mcpsimp"></a>

None

## Constraints<a name="zh-cn_topic_0000001526206862_section65498832"></a>

The **skipCnt** message queues following the message space of this task need to send FAKE messages.

## Example<a name="zh-cn_topic_0000001526206862_section97001499599"></a>

```
 __aicore__ inline static void Call(
    MatmulApiCfg &mm, __gm__ CubeMsgBody *rcvMsg, CubeResGroupHandle<CubeMsgBody> &handle)
{
    //  Computation logic on the AIC, implemented by the user.
    auto skipNum = 3;//(rcvMsg->head).skipCnt. The number of fake messages can be defined by the user in the callback computation structure or passed through a custom message structure.
    auto tmpId = handle.FreeMessage(rcvMsg, AscendC::CubeMsgState::VALID);    // After the current message is processed, call FreeMessage to indicate that rcvMsg has been processed.
    for (int i = 1; i < skipNum + 1; i++) {  
         // Since three fake messages are sent subsequently, FreeMessage must also be called to indicate that the fake messages have been processed.
         auto tmpId = handle.FreeMessage(rcvMsg + i, AscendC::CubeMsgState::FAKE);
    }
    // When fake messages exist, call SetSkipMsg to notify the Cube core not to process the following three fake messages.
    handle.SetSkipMsg(skipNum);
};
```

