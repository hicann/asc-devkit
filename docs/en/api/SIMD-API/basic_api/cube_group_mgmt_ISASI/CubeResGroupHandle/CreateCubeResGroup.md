# CreateCubeResGroup<a name="ZH-CN_TOPIC_0000001996865761"></a>

<!-- md-trans-meta sourceCommit=754fb95859dead826727a591e674261c6a829a08 translatedAt=2026-09-23T13:41:42.303Z pushedAt=2026-10-26T07:41:44.217Z nl=2 -->

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
<!-- @ref: asc-devkit/res/docs/zh/api/SIMD-API/basic_api/cube_group_mgmt_ISASI/CubeResGroupHandle/CreateCubeResGroup_res.md#id1 -->


## Description<a name="zh-cn_topic_0000001526206862_section212607105720"></a>

Quickly creates a **CubeResGroupHandle** object, internally completing message queue space and synchronization event allocation. It is recommended to use this API instead of the **CubeResGroupHandle** constructor to create objects, so as to avoid message queue space conflicts and synchronization event errors between different objects.

## Prototype<a name="section765814724715"></a>

```
template <int groupID, class MatmulApiType, template <class, class> class CallBack, typename CubeMsgType>
__aicore__ inline CubeResGroupHandle<CubeMsgType> CreateCubeResGroup(KfcWorkspace& desc, uint8_t blockStart, uint8_t blockSize, uint8_t msgQueueSize, GM_ADDR tiling)
```

## Parameters<a name="zh-cn_topic_0000001526206862_section129451113125413"></a>

**Table 1** Template parameter description

<a name="table775915013250"></a>
<table><thead align="left"><tr id="row18759203252"><th class="cellrowborder" valign="top" width="13.270000000000001%" id="mcps1.2.3.1.1"><p id="p175919042514"><a name="p175919042514"></a><a name="p175919042514"></a>Parameter</p></th>
<th class="cellrowborder" valign="top" width="86.72999999999999%" id="mcps1.2.3.1.2"><p id="p1759150132516"><a name="p1759150132516"></a><a name="p1759150132516"></a>Description</p></th>
</tr>
</thead>
<tbody><tr id="row187601006257"><td class="cellrowborder" valign="top" width="13.270000000000001%" headers="mcps1.2.3.1.1 "><p id="p147601503258"><a name="p147601503258"></a><a name="p147601503258"></a>**groupID**</p></td>
<td class="cellrowborder" valign="top" width="86.72999999999999%" headers="mcps1.2.3.1.2 "><p id="p157601405251"><a name="p157601405251"></a><a name="p157601405251"></a>Used to represent the group number, with an int32 value range.</p></td>
</tr>
<tr id="row67608072518"><td class="cellrowborder" valign="top" width="13.270000000000001%" headers="mcps1.2.3.1.1 "><p id="p177607013255"><a name="p177607013255"></a><a name="p177607013255"></a>**MatmulApiType**</p></td>
<td class="cellrowborder" valign="top" width="86.72999999999999%" headers="mcps1.2.3.1.2 "><p id="p3760604250"><a name="p3760604250"></a><a name="p3760604250"></a>Defined AIC computation object type.</p></td>
</tr>
<tr id="row11760306250"><td class="cellrowborder" valign="top" width="13.270000000000001%" headers="mcps1.2.3.1.1 "><p id="p167601302256"><a name="p167601302256"></a><a name="p167601302256"></a>**CallBack**</p></td>
<td class="cellrowborder" valign="top" width="86.72999999999999%" headers="mcps1.2.3.1.2 "><p id="p167608022510"><a name="p167608022510"></a><a name="p167608022510"></a>Callback function class that must implement the **Init** and **Call** APIs.</p></td>
</tr>
<tr id="row187601209251"><td class="cellrowborder" valign="top" width="13.270000000000001%" headers="mcps1.2.3.1.1 "><p id="p1776020012258"><a name="p1776020012258"></a><a name="p1776020012258"></a>**CubeMsgType**</p></td>
<td class="cellrowborder" valign="top" width="86.72999999999999%" headers="mcps1.2.3.1.2 "><p id="p18760160162512"><a name="p18760160162512"></a><a name="p18760160162512"></a>User-defined message structure.</p></td>
</tr>
</tbody>
</table>

**Table 2** API parameter description

<a name="zh-cn_topic_0000001526206862_zh-cn_topic_0000001389783361_table111938719446"></a>
<table><thead align="left"><tr id="zh-cn_topic_0000001526206862_zh-cn_topic_0000001389783361_row6223476444"><th class="cellrowborder" valign="top" width="12.97%" id="mcps1.2.4.1.1"><p id="zh-cn_topic_0000001526206862_zh-cn_topic_0000001389783361_p10223674448"><a name="zh-cn_topic_0000001526206862_zh-cn_topic_0000001389783361_p10223674448"></a><a name="zh-cn_topic_0000001526206862_zh-cn_topic_0000001389783361_p10223674448"></a>Parameter</p></th>
<th class="cellrowborder" valign="top" width="8.110000000000001%" id="mcps1.2.4.1.2"><p id="zh-cn_topic_0000001526206862_zh-cn_topic_0000001389783361_p645511218169"><a name="zh-cn_topic_0000001526206862_zh-cn_topic_0000001389783361_p645511218169"></a><a name="zh-cn_topic_0000001526206862_zh-cn_topic_0000001389783361_p645511218169"></a>Input/Output</p></th>
<th class="cellrowborder" valign="top" width="78.92%" id="mcps1.2.4.1.3"><p id="zh-cn_topic_0000001526206862_zh-cn_topic_0000001389783361_p1922337124411"><a name="zh-cn_topic_0000001526206862_zh-cn_topic_0000001389783361_p1922337124411"></a><a name="zh-cn_topic_0000001526206862_zh-cn_topic_0000001389783361_p1922337124411"></a>Description</p></th>
</tr>
</thead>
<tbody><tr id="zh-cn_topic_0000001526206862_zh-cn_topic_0000001389783361_row152234713443"><td class="cellrowborder" valign="top" width="12.97%" headers="mcps1.2.4.1.1 "><p id="p1060615396919"><a name="p1060615396919"></a><a name="p1060615396919"></a>**desc**</p></td>
<td class="cellrowborder" valign="top" width="8.110000000000001%" headers="mcps1.2.4.1.2 "><p id="p13606163918914"><a name="p13606163918914"></a><a name="p13606163918914"></a>Input</p></td>
<td class="cellrowborder" valign="top" width="78.92%" headers="mcps1.2.4.1.3 "><p id="p1560613391196"><a name="p1560613391196"></a><a name="p1560613391196"></a><a href="../KfcWorkspace/KfcWorkspace.md">KfcWorkspace</a>, used to maintain the message queue space.</p></td>
</tr>
<tr id="row519512376588"><td class="cellrowborder" valign="top" width="12.97%" headers="mcps1.2.4.1.1 "><p id="zh-cn_topic_0000001526206862_p223953193015"><a name="zh-cn_topic_0000001526206862_p223953193015"></a><a name="zh-cn_topic_0000001526206862_p223953193015"></a>**blockStart**</p></td>
<td class="cellrowborder" valign="top" width="8.110000000000001%" headers="mcps1.2.4.1.2 "><p id="zh-cn_topic_0000001526206862_p7239938308"><a name="zh-cn_topic_0000001526206862_p7239938308"></a><a name="zh-cn_topic_0000001526206862_p7239938308"></a>Input</p></td>
<td class="cellrowborder" valign="top" width="78.92%" headers="mcps1.2.4.1.3 "><p id="p1117515154210"><a name="p1117515154210"></a><a name="p1117515154210"></a>Sequence number of the starting AIC corresponding to this CubeResGroupHandle from the AIV perspective, that is, the AIC starting sequence number * 2. For example, if the AIC starting sequence number is 0, fill in 0 * 2; if it is 1, fill in 1 * 2.</p></td>
</tr>
<tr id="row11725173915582"><td class="cellrowborder" valign="top" width="12.97%" headers="mcps1.2.4.1.1 "><p id="p11374343181311"><a name="p11374343181311"></a><a name="p11374343181311"></a>**blockSize**</p></td>
<td class="cellrowborder" valign="top" width="8.110000000000001%" headers="mcps1.2.4.1.2 "><p id="p146153901420"><a name="p146153901420"></a><a name="p146153901420"></a>Input</p></td>
<td class="cellrowborder" valign="top" width="78.92%" headers="mcps1.2.4.1.3 "><p id="p517518151222"><a name="p517518151222"></a><a name="p517518151222"></a>Number of blocks allocated for this CubeResGroupHandle from the AIV perspective, that is, the actual number of AICs * 2.</p></td>
</tr>
<tr id="row41174317586"><td class="cellrowborder" valign="top" width="12.97%" headers="mcps1.2.4.1.1 "><p id="p15285194613135"><a name="p15285194613135"></a><a name="p15285194613135"></a>**msgQueueSize**</p></td>
<td class="cellrowborder" valign="top" width="8.110000000000001%" headers="mcps1.2.4.1.2 "><p id="p9285104615133"><a name="p9285104615133"></a><a name="p9285104615133"></a>Input</p></td>
<td class="cellrowborder" valign="top" width="78.92%" headers="mcps1.2.4.1.3 "><p id="p101756155210"><a name="p101756155210"></a><a name="p101756155210"></a>Total number of message queues allocated for this CubeResGroupHandle.</p></td>
</tr>
<tr id="row20135231122916"><td class="cellrowborder" valign="top" width="12.97%" headers="mcps1.2.4.1.1 "><p id="p176065399919"><a name="p176065399919"></a><a name="p176065399919"></a>tiling</p></td>
<td class="cellrowborder" valign="top" width="8.110000000000001%" headers="mcps1.2.4.1.2 "><p id="p96061039494"><a name="p96061039494"></a><a name="p96061039494"></a>Input</p></td>
<td class="cellrowborder" valign="top" width="78.92%" headers="mcps1.2.4.1.3 "><p id="p5606039191"><a name="p5606039191"></a><a name="p5606039191"></a>Address of the tiling information required for AIC core computation.</p></td>
</tr>
</tbody>
</table>

## Return Value<a name="section640mcpsimp"></a>

CubeResGroupHandle object instance.

## Constraints<a name="section84021317123613"></a>

-   Assuming that the number of AIV cores on the chip is x, then blockStart + blockSize <= x - 1, and msgQueueSize <= x.
-   Each AIC is allocated at least one msgQueue.
-   blockStart and blockSize must be even numbers.
-   When this API is used, the 1600B + sizeof\(CubeMsgType\) at the end of the Unified Buffer (UB) space will be occupied.
-   One AIC can belong to only one CubeGroupHandle, that is, the \[blockStart / 2, blockStart / 2 + blockSize / 2\] intervals of multiple CubeGroupHandles cannot overlap.
-   This API cannot be used together with the [REGIST\_MATMUL\_OBJ](../../../adv_api/cube_compute/Matmul_Kernel/REGIST_MATMUL_OBJ.md) API. When the resource management API is used, users manage the inter-core communication between AIC and AIV by themselves. Inside REGIST\_MATMUL\_OBJ, the inter-core communication between AIC and AIV is managed by the framework. Using them at the same time may cause exceptions such as communication message errors.

## Example<a name="zh-cn_topic_0000001526206862_section97001499599"></a>

```
auto handle = AscendC::CreateCubeResGroup<GROUPID, MatmulApiType, MyCallbackFunc, CubeMsgBody> (desc, BLOCKSTART, BLOCKSIZE, MSGQUEUESIZE, tilingGM);
```
