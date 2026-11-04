# Wait<a name="ZH-CN_TOPIC_0000001960224988"></a>

<!-- md-trans-meta sourceCommit=754fb95859dead826727a591e674261c6a829a08 translatedAt=2026-09-23T13:48:51.905Z pushedAt=2026-10-26T07:41:44.233Z nl=2 -->

## Applicable Products<a name="section1550532418810"></a>

<!-- npu="950" id1 -->
- Ascend 950PR&950DT products: supported
<!-- end id1 -->
<!-- npu="A3" id2 -->
- Atlas A3 products: not supported
<!-- end id2 -->
<!-- npu="910b" id3 -->
- Atlas A2 products: supported
<!-- end id3 -->
<!-- npu="310b" id4 -->
- Atlas 200I/500 A2 inference product: not supported
<!-- end id4 -->
<!-- npu="310p" id5 -->
- Atlas inference products AI Core: not supported
<!-- end id5 -->
<!-- npu="310p" id6 -->
- Atlas inference products Vector Core: not supported
<!-- end id6 -->
<!-- npu="910" id7 -->
- Atlas training products: not supported
<!-- end id7 -->
<!-- @ref: asc-devkit/res/docs/zh/api/SIMD-API/basic_api/cube_group_mgmt_ISASI/CubeResGroupHandle/Wait_res.md#id1 -->


## Description<a name="zh-cn_topic_0000001526206862_section212607105720"></a>

After calling **PostMessage** or **PostFakeMessage**, query whether the message has been processed by the **AIC**.

## Prototype<a name="section765814724715"></a>

```
template <bool sync = true>
__aicore__ inline bool Wait(uint16_t offset)
```

## Parameters<a name="zh-cn_topic_0000001526206862_section129451113125413"></a>

**Table 1** Template parameter description

<a name="table7254433348"></a>
<table><thead align="left"><tr id="row102594383418"><th class="cellrowborder" valign="top" width="9.2%" id="mcps1.2.3.1.1"><p id="p102574316348"><a name="p102574316348"></a><a name="p102574316348"></a>Parameter</p></th>
<th class="cellrowborder" valign="top" width="90.8%" id="mcps1.2.3.1.2"><p id="p325194343417"><a name="p325194343417"></a><a name="p325194343417"></a>Description</p></th>
</tr>
</thead>
<tbody><tr id="row525343163416"><td class="cellrowborder" valign="top" width="9.2%" headers="mcps1.2.3.1.1 "><p id="p2251243143411"><a name="p2251243143411"></a><a name="p2251243143411"></a>sync</p></td>
<td class="cellrowborder" valign="top" width="90.8%" headers="mcps1.2.3.1.2 "><p id="p10936103116363"><a name="p10936103116363"></a><a name="p10936103116363"></a>Whether the program needs to wait when querying messages. The parameter values are as follows:</p>
<a name="ul9461719193513"></a><a name="ul9461719193513"></a><ul id="ul9461719193513"><li>true: The program can continue running only after the AIC finishes processing the message.</li><li>false: Only queries whether the AIC has finished processing the message.</li></ul></td>
</tr>
</tbody>
</table>

**Table 2** API parameter description

<a name="zh-cn_topic_0000001526206862_zh-cn_topic_0000001389783361_table111938719446"></a>
<table><thead align="left"><tr id="zh-cn_topic_0000001526206862_zh-cn_topic_0000001389783361_row6223476444"><th class="cellrowborder" valign="top" width="9.09%" id="mcps1.2.4.1.1"><p id="zh-cn_topic_0000001526206862_zh-cn_topic_0000001389783361_p10223674448"><a name="zh-cn_topic_0000001526206862_zh-cn_topic_0000001389783361_p10223674448"></a><a name="zh-cn_topic_0000001526206862_zh-cn_topic_0000001389783361_p10223674448"></a>Parameter</p></th>
<th class="cellrowborder" valign="top" width="7.93%" id="mcps1.2.4.1.2"><p id="zh-cn_topic_0000001526206862_zh-cn_topic_0000001389783361_p645511218169"><a name="zh-cn_topic_0000001526206862_zh-cn_topic_0000001389783361_p645511218169"></a><a name="zh-cn_topic_0000001526206862_zh-cn_topic_0000001389783361_p645511218169"></a>Input/Output</p></th>
<th class="cellrowborder" valign="top" width="82.98%" id="mcps1.2.4.1.3"><p id="zh-cn_topic_0000001526206862_zh-cn_topic_0000001389783361_p1922337124411"><a name="zh-cn_topic_0000001526206862_zh-cn_topic_0000001389783361_p1922337124411"></a><a name="zh-cn_topic_0000001526206862_zh-cn_topic_0000001389783361_p1922337124411"></a>Description</p></th>
</tr>
</thead>
<tbody><tr id="row6413133313219"><td class="cellrowborder" valign="top" width="9.09%" headers="mcps1.2.4.1.1 "><p id="p7413153317217"><a name="p7413153317217"></a><a name="p7413153317217"></a>offset</p></td>
<td class="cellrowborder" valign="top" width="7.93%" headers="mcps1.2.4.1.2 "><p id="p741317331213"><a name="p741317331213"></a><a name="p741317331213"></a>Input</p></td>
<td class="cellrowborder" valign="top" width="82.98%" headers="mcps1.2.4.1.3 "><p id="p1660161516133"><a name="p1660161516133"></a><a name="p1660161516133"></a>Message space address offset, obtained from the return value of **PostMessage** or **PostFakeMessage**.</p></td>
</tr>
</tbody>
</table>

## Return Value<a name="section640mcpsimp"></a>

-   true: The current message has been processed by the **AIC**.
-   false: The current message has not been processed by the **AIC**.

## Constraints<a name="zh-cn_topic_0000001526206862_section65498832"></a>

None

## Example<a name="zh-cn_topic_0000001526206862_section97001499599"></a>

```
auto msgPtr = handle.AllocMessage();        // A new message can be sent at the position pointed to by msgPtr.
AscendC::CubeGroupMsgHead headA = {AscendC::CubeMsgState::VALID, 0};
AscendC::CubeMsgBody msgA = {headA, 1, 0, 0, false, false, false, false, 0, 0, 0, 0, 0, 0, 0, 0};
auto offset = handle.PostMessage(msgPtr, msgA);           // Fill the user-defined message structure at the position pointed to by msgPtr and send it.
bool waitState = handle.template Wait<true>(offset);      // Wait for the AIC to finish processing msgA.
// Fake message scenario.
auto msgFakePtr = handle.AllocMessage();
offset = handle.PostFakeMsg(msgFakePtr);
bool waitState = handle.template Wait<true>(offset); // Wait for the AIC to finish processing the fake message msgFake.
```
