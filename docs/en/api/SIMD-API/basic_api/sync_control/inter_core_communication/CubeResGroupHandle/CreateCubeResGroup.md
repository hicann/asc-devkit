# CreateCubeResGroup<a name="ZH-CN_TOPIC_0000001996865761"></a>

<!-- md-trans-meta sourceCommit=22a41aa8a74d7a2c3f454ea4bd5f0f8fb27aee29 translatedAt=2026-11-19T02:41:48.558Z pushedAt=2026-11-19T05:58:49.102Z nl=2 -->

## Supported Products<a name="section1550532418810"></a>

<!-- npu="950" id1 -->
- Ascend 950PR&950DT products: Supported
<!-- end id1 -->
<!-- npu="A3" id2 -->
- Atlas A3 products: Not supported
<!-- end id2 -->
<!-- npu="910b" id3 -->
- Atlas A2 products: Supported
<!-- end id3 -->
<!-- npu="310b" id4 -->
- Atlas 200I/500 A2 inference products: Not supported
<!-- end id4 -->
<!-- npu="310p" id5 -->
- Atlas inference products AI Core: Not Supported
<!-- end id5 -->
<!-- npu="310p" id6 -->
- Atlas inference products Vector Core: Not Supported
<!-- end id6 -->
<!-- npu="910" id7 -->
- Atlas training products: Not supported
<!-- end id7 -->
<!-- @ref: asc-devkit/res/docs/zh/api/SIMD-API/basic_api/sync_control/inter_core_communication/CubeResGroupHandle/CreateCubeResGroup_res.md#id1 -->


## Function Description<a name="zh-cn_topic_0000001526206862_section212607105720"></a>

Quickly creates a CubeResGroupHandle object, internally completing the Message Queue Space and synchronization event allocation. Using the API is recommended to avoid using the CubeResGroupHandle constructor to create objects, which may lead to Message Queue Space conflicts and synchronization event errors, etc. between different objects.

## Prototype<a name="section765814724715"></a>

```
template <int groupID, class MatmulApiType, template <class, class> class CallBack, typename CubeMsgType>
__aicore__ inline CubeResGroupHandle<CubeMsgType> CreateCubeResGroup(KfcWorkspace& desc, uint8_t blockStart, uint8_t blockSize, uint8_t msgQueueSize, GM_ADDR tiling)
```

## Parameters<a name="zh-cn_topic_0000001526206862_section129451113125413"></a>

**Table 1** Template parameters

<a name="table775915013250"></a>
<table><thead align="left"><tr id="row18759203252"><th class="cellrowborder" valign="top" width="13.270000000000001%" id="mcps1.2.3.1.1"><p id="p175919042514"><a name="p175919042514"></a><a name="p175919042514"></a>Parameter</p>
</th>
<th class="cellrowborder" valign="top" width="86.72999999999999%" id="mcps1.2.3.1.2"><p id="p1759150132516"><a name="p1759150132516"></a><a name="p1759150132516"></a>Description</p>
</th>
</tr>
</thead>
<tbody><tr id="row187601006257"><td class="cellrowborder" valign="top" width="13.270000000000001%" headers="mcps1.2.3.1.1 "><p id="p147601503258"><a name="p147601503258"></a><a name="p147601503258"></a>groupID</p>
</td>
<td class="cellrowborder" valign="top" width="86.72999999999999%" headers="mcps1.2.3.1.2 "><p id="p157601405251"><a name="p157601405251"></a><a name="p157601405251"></a>Number used to represent the Group, with an int32 value range.</p>
</td>
</tr>
<tr id="row67608072518"><td class="cellrowborder" valign="top" width="13.270000000000001%" headers="mcps1.2.3.1.1 "><p id="p177607013255"><a name="p177607013255"></a><a name="p177607013255"></a>MatmulApiType</p>
</td>
<td class="cellrowborder" valign="top" width="86.72999999999999%" headers="mcps1.2.3.1.2 "><p id="p3760604250"><a name="p3760604250"></a><a name="p3760604250"></a>Defined AIC computation object type.</p>
</td>
</tr>
<tr id="row11760306250"><td class="cellrowborder" valign="top" width="13.270000000000001%" headers="mcps1.2.3.1.1 "><p id="p167601302256"><a name="p167601302256"></a><a name="p167601302256"></a>CallBack</p>
</td>
<td class="cellrowborder" valign="top" width="86.72999999999999%" headers="mcps1.2.3.1.2 "><p id="p167608022510"><a name="p167608022510"></a><a name="p167608022510"></a>Callback function class, which needs to implement the Init and Call APIs.</p>
</td>
</tr>
<tr id="row187601209251"><td class="cellrowborder" valign="top" width="13.270000000000001%" headers="mcps1.2.3.1.1 "><p id="p1776020012258"><a name="p1776020012258"></a><a name="p1776020012258"></a>CubeMsgType</p>
</td>
<td class="cellrowborder" valign="top" width="86.72999999999999%" headers="mcps1.2.3.1.2 "><p id="p18760160162512"><a name="p18760160162512"></a><a name="p18760160162512"></a>User-Defined Message structure.</p>
</td>
</tr>
</tbody>
</table>

**Table 2** API parameters

<a name="zh-cn_topic_0000001526206862_zh-cn_topic_0000001389783361_table111938719446"></a>
<table><thead align="left"><tr id="zh-cn_topic_0000001526206862_zh-cn_topic_0000001389783361_row6223476444"><th class="cellrowborder" valign="top" width="12.97%" id="mcps1.2.4.1.1"><p id="zh-cn_topic_0000001526206862_zh-cn_topic_0000001389783361_p10223674448"><a name="zh-cn_topic_0000001526206862_zh-cn_topic_0000001389783361_p10223674448"></a><a name="zh-cn_topic_0000001526206862_zh-cn_topic_0000001389783361_p10223674448"></a>Parameter</p>
</th>
<th class="cellrowborder" valign="top" width="8.110000000000001%" id="mcps1.2.4.1.2"><p id="zh-cn_topic_0000001526206862_zh-cn_topic_0000001389783361_p645511218169"><a name="zh-cn_topic_0000001526206862_zh-cn_topic_0000001389783361_p645511218169"></a><a name="zh-cn_topic_0000001526206862_zh-cn_topic_0000001389783361_p645511218169"></a>Input/Output</p>
</th>
<th class="cellrowborder" valign="top" width="78.92%" id="mcps1.2.4.1.3"><p id="zh-cn_topic_0000001526206862_zh-cn_topic_0000001389783361_p1922337124411"><a name="zh-cn_topic_0000001526206862_zh-cn_topic_0000001389783361_p1922337124411"></a><a name="zh-cn_topic_0000001526206862_zh-cn_topic_0000001389783361_p1922337124411"></a>Description</p>
</th>
</tr>
</thead>
<tbody><tr id="zh-cn_topic_0000001526206862_zh-cn_topic_0000001389783361_row152234713443"><td class="cellrowborder" valign="top" width="12.97%" headers="mcps1.2.4.1.1 "><p id="p1060615396919"><a name="p1060615396919"></a><a name="p1060615396919"></a>desc</p>
</td>
<td class="cellrowborder" valign="top" width="8.110000000000001%" headers="mcps1.2.4.1.2 "><p id="p13606163918914"><a name="p13606163918914"></a><a name="p13606163918914"></a>Input</p>
</td>
<td class="cellrowborder" valign="top" width="78.92%" headers="mcps1.2.4.1.3 "><p id="p1560613391196"><a name="p1560613391196"></a><a name="p1560613391196"></a><a href="../../inter_core_communication/KfcWorkspace/KfcWorkspace.md">KfcWorkspace</a>, used to maintain the Message Queue Space.</p>
</td>
</tr>
<tr id="row519512376588"><td class="cellrowborder" valign="top" width="12.97%" headers="mcps1.2.4.1.1 "><p id="zh-cn_topic_0000001526206862_p223953193015"><a name="zh-cn_topic_0000001526206862_p223953193015"></a><a name="zh-cn_topic_0000001526206862_p223953193015"></a>blockStart</p>
</td>
<td class="cellrowborder" valign="top" width="8.110000000000001%" headers="mcps1.2.4.1.2 "><p id="zh-cn_topic_0000001526206862_p7239938308"><a name="zh-cn_topic_0000001526206862_p7239938308"></a><a name="zh-cn_topic_0000001526206862_p7239938308"></a>Input</p>
</td>
<td class="cellrowborder" valign="top" width="78.92%" headers="mcps1.2.4.1.3 "><p id="p1117515154210"><a name="p1117515154210"></a><a name="p1117515154210"></a>The sequence number of the starting AIC corresponding to this CubeResGroupHandle From the Perspective of the AIV, that is, the AIC starting sequence number * 2. For example, if the AIC starting sequence number is 0, fill in 0 * 2; if it is 1, fill in 1 * 2.</p>
</td>
</tr>
<tr id="row11725173915582"><td class="cellrowborder" valign="top" width="12.97%" headers="mcps1.2.4.1.1 "><p id="p11374343181311"><a name="p11374343181311"></a><a name="p11374343181311"></a>blockSize</p>
</td>
<td class="cellrowborder" valign="top" width="8.110000000000001%" headers="mcps1.2.4.1.2 "><p id="p146153901420"><a name="p146153901420"></a><a name="p146153901420"></a>Input</p>
</td>
<td class="cellrowborder" valign="top" width="78.92%" headers="mcps1.2.4.1.3 "><p id="p517518151222"><a name="p517518151222"></a><a name="p517518151222"></a>The number of Blocks Allocated for this CubeResGroupHandle From the Perspective of the AIV, that is, the actual number of AICs * 2.</p>
</td>
</tr>
<tr id="row41174317586"><td class="cellrowborder" valign="top" width="12.97%" headers="mcps1.2.4.1.1 "><p id="p15285194613135"><a name="p15285194613135"></a><a name="p15285194613135"></a>msgQueueSize</p>
</td>
<td class="cellrowborder" valign="top" width="8.110000000000001%" headers="mcps1.2.4.1.2 "><p id="p9285104615133"><a name="p9285104615133"></a><a name="p9285104615133"></a>Input</p>
</td>
<td class="cellrowborder" valign="top" width="78.92%" headers="mcps1.2.4.1.3 "><p id="p101756155210"><a name="p101756155210"></a><a name="p101756155210"></a>The total number of Message Queues Allocated for this CubeResGroupHandle.</p>
</td>
</tr>
<tr id="row20135231122916"><td class="cellrowborder" valign="top" width="12.97%" headers="mcps1.2.4.1.1 "><p id="p176065399919"><a name="p176065399919"></a><a name="p176065399919"></a>tiling</p>
</td>
<td class="cellrowborder" valign="top" width="8.110000000000001%" headers="mcps1.2.4.1.2 "><p id="p96061039494"><a name="p96061039494"></a><a name="p96061039494"></a>Input</p>
</td>
<td class="cellrowborder" valign="top" width="78.92%" headers="mcps1.2.4.1.3 "><p id="p5606039191"><a name="p5606039191"></a><a name="p5606039191"></a>The address of the tiling information required for AIC core Computation.</p>
</td>
</tr>
</tbody>
</table>

## Return Value<a name="section640mcpsimp"></a>

CubeResGroupHandle object instance.

## Constraints<a name="section84021317123613"></a>

-   Assuming the number of AIV cores on the chip is x, then blockStart + blockSize <= x - 1, and msgQueueSize <= x.
-   Each AIC is Allocated at least 1 msgQueue.
-   blockStart and blockSize must be even numbers.
-   Using the API, the 1600B + sizeof\(CubeMsgType\) at the end of the Unified Buffer (UB) Space will be occupied.
-   1 AIC can only belong to 1 CubeGroupHandle, that is, the \[blockStart / 2, blockStart / 2 + blockSize / 2\] intervals of multiple CubeGroupHandles cannot overlap.
-   Cannot be used simultaneously with the [REGIST\_MATMUL\_OBJ](../../../../adv_api/cube_compute/Matmul_Kernel/REGIST_MATMUL_OBJ.md) API. When using the resource management API, the user manages the inter-core communication between AIC and AIV independently, while REGIST\_MATMUL\_OBJ manages the inter-core communication between AIC and AIV internally by the framework. Using them simultaneously may cause communication Message errors and other exceptions.

## Example<a name="zh-cn_topic_0000001526206862_section97001499599"></a>

```
auto handle = AscendC::CreateCubeResGroup<GROUPID, MatmulApiType, MyCallbackFunc, CubeMsgBody> (desc, BLOCKSTART, BLOCKSIZE, MSGQUEUESIZE, tilingGM);
```
