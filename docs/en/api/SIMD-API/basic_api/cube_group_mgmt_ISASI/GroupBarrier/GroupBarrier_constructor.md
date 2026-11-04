# GroupBarrier constructor<a name="ZH-CN_TOPIC_0000002005085181"></a>

<!-- md-trans-meta sourceCommit=754fb95859dead826727a591e674261c6a829a08 translatedAt=2026-09-23T13:50:51.455Z pushedAt=2026-10-26T07:41:44.238Z nl=2 -->

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
- Atlas 200I/500 A2 inference products: not supported
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
<!-- @ref: asc-devkit/res/docs/zh/api/SIMD-API/basic_api/cube_group_mgmt_ISASI/GroupBarrier/GroupBarrier_constructor_res.md#id1 -->


## Description<a name="zh-cn_topic_0000001526206862_section212607105720"></a>

Creates a **GroupBarrier** object and determines the **Block** count of the Arrive group and the **Block** count of the Wait group by setting constructor parameters.

## Prototype<a name="section765814724715"></a>

```
template <PipeMode pipeMode>
class GroupBarrier;
__aicore__ inline GroupBarrier(GM_ADDR groupWorkspace, uint32_t arriveSizeIn, uint32_t waitSizeIn);
```

## Parameters<a name="zh-cn_topic_0000001526206862_section129451113125413"></a>

**Table 1** Template parameter description

<a name="table884518212555"></a>
<table><thead align="left"><tr id="row1584512213553"><th class="cellrowborder" valign="top" width="17.119999999999997%" id="mcps1.2.3.1.1"><p id="p158869811398"><a name="p158869811398"></a><a name="p158869811398"></a>Data Type</p></th>
<th class="cellrowborder" valign="top" width="82.88%" id="mcps1.2.3.1.2"><p id="p158864823917"><a name="p158864823917"></a><a name="p158864823917"></a>Description</p></th>
</tr>
</thead>
<tbody><tr id="row484511235519"><td class="cellrowborder" valign="top" width="17.119999999999997%" headers="mcps1.2.3.1.1 "><p id="p988616883917"><a name="p988616883917"></a><a name="p988616883917"></a>PipeMode</p></td>
<td class="cellrowborder" valign="top" width="82.88%" headers="mcps1.2.3.1.2 "><p id="p112701021123919"><a name="p112701021123919"></a><a name="p112701021123919"></a>Execution unit used by GroupBarrier to send group synchronization messages. Only MTE3_MODE is supported.</p>
<a name="screen1488615823912"></a><a name="screen1488615823912"></a><pre class="screen" codetype="Cpp" id="screen1488615823912">enum class PipeMode : uint8_t { 
  SCALAR_MODE = 0,
  MTE3_MODE = 1,
  MAX 
}</pre></td>
</tr>
</tbody>
</table>

**Table 2** GroupBarrier constructor parameter description

<a name="zh-cn_topic_0000001526206862_zh-cn_topic_0000001389783361_table111938719446"></a>
<table><thead align="left"><tr id="zh-cn_topic_0000001526206862_zh-cn_topic_0000001389783361_row6223476444"><th class="cellrowborder" valign="top" width="17.22%" id="mcps1.2.4.1.1"><p id="zh-cn_topic_0000001526206862_zh-cn_topic_0000001389783361_p10223674448"><a name="zh-cn_topic_0000001526206862_zh-cn_topic_0000001389783361_p10223674448"></a><a name="zh-cn_topic_0000001526206862_zh-cn_topic_0000001389783361_p10223674448"></a>Parameter</p></th>
<th class="cellrowborder" valign="top" width="15.340000000000002%" id="mcps1.2.4.1.2"><p id="zh-cn_topic_0000001526206862_zh-cn_topic_0000001389783361_p645511218169"><a name="zh-cn_topic_0000001526206862_zh-cn_topic_0000001389783361_p645511218169"></a><a name="zh-cn_topic_0000001526206862_zh-cn_topic_0000001389783361_p645511218169"></a>Input/Output</p></th>
<th class="cellrowborder" valign="top" width="67.44%" id="mcps1.2.4.1.3"><p id="zh-cn_topic_0000001526206862_zh-cn_topic_0000001389783361_p1922337124411"><a name="zh-cn_topic_0000001526206862_zh-cn_topic_0000001389783361_p1922337124411"></a><a name="zh-cn_topic_0000001526206862_zh-cn_topic_0000001389783361_p1922337124411"></a>Description</p></th>
</tr>
</thead>
<tbody><tr id="zh-cn_topic_0000001526206862_zh-cn_topic_0000001389783361_row152234713443"><td class="cellrowborder" valign="top" width="17.22%" headers="mcps1.2.4.1.1 "><p id="zh-cn_topic_0000001526206862_zh-cn_topic_0000001389783361_p2340183613156"><a name="zh-cn_topic_0000001526206862_zh-cn_topic_0000001389783361_p2340183613156"></a><a name="zh-cn_topic_0000001526206862_zh-cn_topic_0000001389783361_p2340183613156"></a>groupWorkspace</p></td>
<td class="cellrowborder" valign="top" width="15.340000000000002%" headers="mcps1.2.4.1.2 "><p id="p19741912147"><a name="p19741912147"></a><a name="p19741912147"></a>Input</p></td>
<td class="cellrowborder" valign="top" width="67.44%" headers="mcps1.2.4.1.3 "><p id="zh-cn_topic_0000001526206862_zh-cn_topic_0000001389783361_p2684123934216"><a name="zh-cn_topic_0000001526206862_zh-cn_topic_0000001389783361_p2684123934216"></a><a name="zh-cn_topic_0000001526206862_zh-cn_topic_0000001389783361_p2684123934216"></a>Start address of this GroupBarrier on GM, used to store group synchronization messages. It must be 512B-aligned. The user manages this part of GlobalMemory, including address alignment and zeroing.</p></td>
</tr>
<tr id="row9374154371313"><td class="cellrowborder" valign="top" width="17.22%" headers="mcps1.2.4.1.1 "><p id="p11374343181311"><a name="p11374343181311"></a><a name="p11374343181311"></a>arriveSizeIn</p></td>
<td class="cellrowborder" valign="top" width="15.340000000000002%" headers="mcps1.2.4.1.2 "><p id="p146153901420"><a name="p146153901420"></a><a name="p146153901420"></a>Input</p></td>
<td class="cellrowborder" valign="top" width="67.44%" headers="mcps1.2.4.1.3 "><p id="p123745437133"><a name="p123745437133"></a><a name="p123745437133"></a>Count of AIVs in the Arrive group.</p></td>
</tr>
<tr id="row15285204611313"><td class="cellrowborder" valign="top" width="17.22%" headers="mcps1.2.4.1.1 "><p id="p15285194613135"><a name="p15285194613135"></a><a name="p15285194613135"></a>waitSizeIn</p></td>
<td class="cellrowborder" valign="top" width="15.340000000000002%" headers="mcps1.2.4.1.2 "><p id="p9285104615133"><a name="p9285104615133"></a><a name="p9285104615133"></a>Input</p></td>
<td class="cellrowborder" valign="top" width="67.44%" headers="mcps1.2.4.1.3 "><p id="p18285114611311"><a name="p18285114611311"></a><a name="p18285114611311"></a>Count of AIVs in the Wait group.</p></td>
</tr>
</tbody>
</table>

## Return Value<a name="section640mcpsimp"></a>

A **GroupBarrier** object instance.

## Constraints<a name="zh-cn_topic_0000001526206862_section65498832"></a>

-   When this API is used, the last 1600B of the Unified Buffer (UB) space is occupied.
-   This API cannot be used together with the [REGIST\_MATMUL\_OBJ](../../../adv_api/cube_compute/Matmul_Kernel/REGIST_MATMUL_OBJ.md) API. When the resource management APIs are used, the user manages the inter-core communication between AIC and AIV independently, whereas the inter-core communication between AIC and AIV is managed by the framework inside REGIST\_MATMUL\_OBJ. Using them at the same time may cause exceptions such as incorrect communication messages.

## Example<a name="zh-cn_topic_0000001526206862_section97001499599"></a>

```
// 6 AIVs wait for 3 AIVs to Arrive before starting subsequent operations. A total of 6*512B address space is required, with the start address being the user-specified startAddr.
AscendC::GroupBarrier<AscendC::PipeMode::MTE3_MODE> barA(startAddr, 3, 6);
```
