# GroupBarrier Constructor<a name="ZH-CN_TOPIC_0000002005085181"></a>

<!-- md-trans-meta sourceCommit=e3c8c70b3f5d27f3770063ae4b535bc75d904e3e translatedAt=2026-08-27T17:41:34.207Z -->

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

Creates a **GroupBarrier** object and determines the **Block** count of the Arrive group and the **Block** count of the Wait group by setting the constructor parameters.

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
<td class="cellrowborder" valign="top" width="82.88%" headers="mcps1.2.3.1.2 "><p id="p112701021123919"><a name="p112701021123919"></a><a name="p112701021123919"></a>Execution unit used by **GroupBarrier** to send group synchronization messages. Only **MTE3_MODE** is supported.</p>
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
<td class="cellrowborder" valign="top" width="67.44%" headers="mcps1.2.4.1.3 "><p id="zh-cn_topic_0000001526206862_zh-cn_topic_0000001389783361_p2684123934216"><a name="zh-cn_topic_0000001526206862_zh-cn_topic_0000001389783361_p2684123934216"></a><a name="zh-cn_topic_0000001526206862_zh-cn_topic_0000001389783361_p2684123934216"></a>Starting address of this GroupBarrier in GM, used to store group synchronization messages. It must be 512B aligned. Users manage this part of GlobalMemory themselves, including address alignment and zeroing.</p></td>
</tr>
<tr id="row9374154371313"><td class="cellrowborder" valign="top" width="17.22%" headers="mcps1.2.4.1.1 "><p id="p11374343181311"><a name="p11374343181311"></a><a name="p11374343181311"></a>arriveSizeIn</p></td>
<td class="cellrowborder" valign="top" width="15.340000000000002%" headers="mcps1.2.4.1.2 "><p id="p146153901420"><a name="p146153901420"></a><a name="p146153901420"></a>Input</p></td>
<td class="cellrowborder" valign="top" width="67.44%" headers="mcps1.2.4.1.3 "><p id="p123745437133"><a name="p123745437133"></a><a name="p123745437133"></a>Count of AIVs in the Arrive group.</p></td>
</tr>
<tr id="row15285204611313"><td class="cellrowborder" valign="top" width="17.22%" headers="mcps1.2.4.1.1 "><p id="p15285194613135"><a name="p15285194613135"></a><a name="p15285194613135"></a>waitSizeIn</p></td>
<td class="cellrowborder" valign="top" width="15.340000000000002%" headers="mcps1.2.4.1.2 "><p id="p9285104615133"><a name="p9285104615133"></a><a name="p9285104615133"></a>Input</p></td>
<td class="cellrowborder" valign="top" width="67.44%" headers="mcps1.2.4.1.3 "><p id="p18285114611311"><a name="p18285114611311"></a><a name="p18285114611311"></a>Count of AIVs in the **Wait** group.</p></td>
</tr>
</tbody>
</table>

## Return Value<a name="section640mcpsimp"></a>

Returns a **GroupBarrier** object instance.

## Constraints<a name="zh-cn_topic_0000001526206862_section65498832"></a>

-   When this API is used, the last 1600B of the UB space is occupied.
-   This API cannot be used together with [REGIST\_MATMUL\_OBJ](../../../High-level API/Cube Computation/Matmul-Kernel-side API/REGIST_MATMUL_OBJ.md). When the resource management APIs are used, the user manages the inter-core communication between AIC and AIV. Inside REGIST\_MATMUL\_OBJ, the framework manages the inter-core communication between AIC and AIV. Using both at the same time may cause exceptions such as incorrect communication messages.

## Example<a name="zh-cn_topic_0000001526206862_section97001499599"></a>

```
// Six AIVs wait for three AIVs to Arrive before starting subsequent services. A total of 6*512B address space is required, with the start address being the user-specified startAddr.
AscendC::GroupBarrier<AscendC::PipeMode::MTE3_MODE> barA(startAddr, 3, 6);
```

