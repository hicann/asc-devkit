# FreeMessage<a name="ZH-CN_TOPIC_0000001996945149"></a>

<!-- md-trans-meta sourceCommit=e3c8c70b3f5d27f3770063ae4b535bc75d904e3e translatedAt=2026-08-27T17:34:31.942Z -->

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

In the custom callback function logic, after message processing is complete, call this API to refresh the message state to **FREE**; or, when the message state reaches the specified state **waitState**, refresh the message state to **FREE**. For details about the message state, see the **msgState** parameter in [Table 2](cuberesgrouphandle_usage_description.md#table77221554135216).

## Prototype<a name="section765814724715"></a>

```
__aicore__ inline uint16_t FreeMessage(__gm__ CubeMsgType *msg);     
__aicore__ inline uint16_t FreeMessage(__gm__ CubeMsgType *msg, CubeMsgState waitState);   
```

## Parameters<a name="zh-cn_topic_0000001526206862_section129451113125413"></a>

**Table 1**  API parameter description

<a name="zh-cn_topic_0000001526206862_zh-cn_topic_0000001389783361_table111938719446"></a>
<table><thead align="left"><tr id="zh-cn_topic_0000001526206862_zh-cn_topic_0000001389783361_row6223476444"><th class="cellrowborder" valign="top" width="9.87%" id="mcps1.2.4.1.1"><p id="zh-cn_topic_0000001526206862_zh-cn_topic_0000001389783361_p10223674448"><a name="zh-cn_topic_0000001526206862_zh-cn_topic_0000001389783361_p10223674448"></a><a name="zh-cn_topic_0000001526206862_zh-cn_topic_0000001389783361_p10223674448"></a>Parameter</p></th>
<th class="cellrowborder" valign="top" width="9.71%" id="mcps1.2.4.1.2"><p id="zh-cn_topic_0000001526206862_zh-cn_topic_0000001389783361_p645511218169"><a name="zh-cn_topic_0000001526206862_zh-cn_topic_0000001389783361_p645511218169"></a><a name="zh-cn_topic_0000001526206862_zh-cn_topic_0000001389783361_p645511218169"></a>Input/Output</p></th>
<th class="cellrowborder" valign="top" width="80.42%" id="mcps1.2.4.1.3"><p id="zh-cn_topic_0000001526206862_zh-cn_topic_0000001389783361_p1922337124411"><a name="zh-cn_topic_0000001526206862_zh-cn_topic_0000001389783361_p1922337124411"></a><a name="zh-cn_topic_0000001526206862_zh-cn_topic_0000001389783361_p1922337124411"></a>Description</p></th>
</tr>
</thead>
<tbody><tr id="zh-cn_topic_0000001526206862_zh-cn_topic_0000001389783361_row152234713443"><td class="cellrowborder" valign="top" width="9.87%" headers="mcps1.2.4.1.1 "><p id="zh-cn_topic_0000001526206862_zh-cn_topic_0000001389783361_p2340183613156"><a name="zh-cn_topic_0000001526206862_zh-cn_topic_0000001389783361_p2340183613156"></a><a name="zh-cn_topic_0000001526206862_zh-cn_topic_0000001389783361_p2340183613156"></a>msg</p></td>
<td class="cellrowborder" valign="top" width="9.71%" headers="mcps1.2.4.1.2 "><p id="p19741912147"><a name="p19741912147"></a><a name="p19741912147"></a>Input</p></td>
<td class="cellrowborder" valign="top" width="80.42%" headers="mcps1.2.4.1.3 "><p id="zh-cn_topic_0000001526206862_zh-cn_topic_0000001389783361_p2684123934216"><a name="zh-cn_topic_0000001526206862_zh-cn_topic_0000001389783361_p2684123934216"></a><a name="zh-cn_topic_0000001526206862_zh-cn_topic_0000001389783361_p2684123934216"></a>Message space address in this **CubeResGroupHandle**.</p></td>
</tr>
<tr id="row6413133313219"><td class="cellrowborder" valign="top" width="9.87%" headers="mcps1.2.4.1.1 "><p id="p23594269491"><a name="p23594269491"></a><a name="p23594269491"></a>waitState</p></td>
<td class="cellrowborder" valign="top" width="9.71%" headers="mcps1.2.4.1.2 "><p id="p1835911267498"><a name="p1835911267498"></a><a name="p1835911267498"></a>Input</p></td>
<td class="cellrowborder" valign="top" width="80.42%" headers="mcps1.2.4.1.3 "><p id="p1335942664919"><a name="p1335942664919"></a><a name="p1335942664919"></a>**msgState** to wait for.</p></td>
</tr>
</tbody>
</table>

## Return Value<a name="section640mcpsimp"></a>

Returns the address offset between the current message space and the head space of the message queue.

## Constraints<a name="zh-cn_topic_0000001526206862_section65498832"></a>

The specified message state **waitState** cannot be **QUIT** or **FREE**.

## Example<a name="zh-cn_topic_0000001526206862_section97001499599"></a>

```
template <int32_t funcId>
__aicore__ inline static typename IsEqual<funcId, 1>::Type CubeGroupCallBack(
    MatmulApiCfg &mm, __gm__ CubeMsgBody *rcvMsg, CubeResGroupHandle<CubeMsgBody> &handle)
{
       // Computation logic on the Cube core, implemented by the user. After all computations are complete, call FreeMessage to indicate that rcvMsg has been processed.
       auto tmpId = handle.FreeMessage(rcvMsg);
};
```

