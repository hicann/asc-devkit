# DeInterleave<a name="ZH-CN_TOPIC_0000002131264492"></a>

<!-- md-trans-meta sourceCommit=0364dcce9ab16d714270f168ec9546fd7620f90f translatedAt=2026-09-09T14:49:44.388Z -->

## Applicable Products<a name="section1550532418810"></a>

<!-- npu="950" id1 -->
- Ascend 950PR/Ascend 950DT: Supported
<!-- end id1 -->
<!-- npu="A3" id2 -->
- Atlas A3 training products/Atlas A3 inference products: Not supported
<!-- end id2 -->
<!-- npu="910b" id3 -->
- Atlas A2 training products/Atlas A2 inference products: Not supported
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


## Description<a name="section618mcpsimp"></a>

Given the source operands src0 and src1, deinterleaves the elements in src0 and src1 and stores them into the destination operands dst0 and dst1. The deinterleaving arrangement is shown in the following figure, where each cell represents an element.

![](../../../../figures/deinterleave1.png)

## Prototype<a name="section620mcpsimp"></a>

-   Two inputs

    ```
    template <typename T>
    __aicore__ inline void DeInterleave(const LocalTensor<T>& dst0, const LocalTensor<T>& dst1, const LocalTensor<T>& src0, const LocalTensor<T>& src1, const int32_t count)
    ```

-   One input

    ```
    template <typename T>
    __aicore__ inline void DeInterleave(const LocalTensor<T>& dst0, const LocalTensor<T>& dst1, const LocalTensor<T>& src, const int32_t srcCount)
    ```

## Parameters<a name="section176711403104"></a>

**Table 1** Template parameters

<a name="table4835205712588"></a>
<table><thead align="left"><tr id="zh-cn_topic_0000001429830437_row118356578583"><th class="cellrowborder" valign="top" width="16.509999999999998%" id="mcps1.2.3.1.1"><p id="zh-cn_topic_0000001429830437_p48354572582"><a name="zh-cn_topic_0000001429830437_p48354572582"></a><a name="zh-cn_topic_0000001429830437_p48354572582"></a>Parameter</p></th>
<th class="cellrowborder" valign="top" width="83.49%" id="mcps1.2.3.1.2"><p id="zh-cn_topic_0000001429830437_p583535795817"><a name="zh-cn_topic_0000001429830437_p583535795817"></a><a name="zh-cn_topic_0000001429830437_p583535795817"></a>Description</p></th>
</tr>
</thead>
<tbody><tr id="zh-cn_topic_0000001429830437_row1835857145817"><td class="cellrowborder" valign="top" width="16.509999999999998%" headers="mcps1.2.3.1.1 "><p id="zh-cn_topic_0000001429830437_p5835457165816"><a name="zh-cn_topic_0000001429830437_p5835457165816"></a><a name="zh-cn_topic_0000001429830437_p5835457165816"></a>T</p></td>
<td class="cellrowborder" valign="top" width="83.49%" headers="mcps1.2.3.1.2 "><p id="zh-cn_topic_0000001429830437_p168351657155818"><a name="zh-cn_topic_0000001429830437_p168351657155818"></a><a name="zh-cn_topic_0000001429830437_p168351657155818"></a>Data type of the operand.</p></td>
</tr>
</tbody>
</table>

**Table 2** Parameters

<a name="table1055216132132"></a>
<table><thead align="left"><tr id="row105531513121315"><th class="cellrowborder" valign="top" width="16.49%" id="mcps1.2.4.1.1"><p id="p5553171319138"><a name="p5553171319138"></a><a name="p5553171319138"></a>Parameter</p></th>
<th class="cellrowborder" valign="top" width="11.93%" id="mcps1.2.4.1.2"><p id="p5553151313131"><a name="p5553151313131"></a><a name="p5553151313131"></a>Input/Output</p></th>
<th class="cellrowborder" valign="top" width="71.58%" id="mcps1.2.4.1.3"><p id="p655316136139"><a name="p655316136139"></a><a name="p655316136139"></a>Description</p></th>
</tr>
</thead>
<tbody><tr id="row5553201314135"><td class="cellrowborder" valign="top" width="16.49%" headers="mcps1.2.4.1.1 "><p id="p8553813111314"><a name="p8553813111314"></a><a name="p8553813111314"></a>dst0/dst1</p></td>
<td class="cellrowborder" valign="top" width="11.93%" headers="mcps1.2.4.1.2 "><p id="p755318134134"><a name="p755318134134"></a><a name="p755318134134"></a>Output</p></td>
<td class="cellrowborder" valign="top" width="71.58%" headers="mcps1.2.4.1.3 "><p id="p1515191511407"><a name="p1515191511407"></a><a name="p1515191511407"></a>Destination operand.</p>
<p id="p65530137137"><a name="p65530137137"></a><a name="p65530137137"></a><span id="ph173308471594"><a name="ph173308471594"></a><a name="ph173308471594"></a><span id="ph9902231466"><a name="ph9902231466"></a><a name="ph9902231466"></a><span id="ph1782115034816"><a name="ph1782115034816"></a><a name="ph1782115034816"></a>The type is <a href="../../data_structures/LocalTensor/LocalTensor.md">LocalTensor</a>, and the supported TPosition values are VECIN/VECCALC/VECOUT.</span></span></span></p>
<p id="p37511234195317"><a name="p37511234195317"></a><a name="p37511234195317"></a><span id="ph19174141065411"><a name="ph19174141065411"></a><a name="ph19174141065411"></a>The start address of LocalTensor must be 32-byte aligned.</span></p>
<p id="p468305719192"><a name="p468305719192"></a><a name="p468305719192"></a><span id="ph126252025205"><a name="ph126252025205"></a><a name="ph126252025205"></a>For Ascend 950PR/Ascend 950DT</span>, the supported data types are: uint8_t/int8_t/uint16_t/int16_t/half/bfloat16_t/uint32_t/int32_t/float/uint64_t/int64_t</p></td>
</tr>
<tr id="row6553613191315"><td class="cellrowborder" valign="top" width="16.49%" headers="mcps1.2.4.1.1 "><p id="p195531113161311"><a name="p195531113161311"></a><a name="p195531113161311"></a>src/src0/src1</p></td>
<td class="cellrowborder" valign="top" width="11.93%" headers="mcps1.2.4.1.2 "><p id="p155310135134"><a name="p155310135134"></a><a name="p155310135134"></a>Input</p></td>
<td class="cellrowborder" valign="top" width="71.58%" headers="mcps1.2.4.1.3 "><p id="p7218122944012"><a name="p7218122944012"></a><a name="p7218122944012"></a>Source operand.</p>
<p id="p15422163732418"><a name="p15422163732418"></a><a name="p15422163732418"></a><span id="ph97971326111115"><a name="ph97971326111115"></a><a name="ph97971326111115"></a><span id="zh-cn_topic_0000001530181537_ph9902231466"><a name="zh-cn_topic_0000001530181537_ph9902231466"></a><a name="zh-cn_topic_0000001530181537_ph9902231466"></a><span id="zh-cn_topic_0000001530181537_ph1782115034816"><a name="zh-cn_topic_0000001530181537_ph1782115034816"></a><a name="zh-cn_topic_0000001530181537_ph1782115034816"></a>The type is <a href="../../data_structures/LocalTensor/LocalTensor.md">LocalTensor</a>, and the supported TPosition values are VECIN/VECCALC/VECOUT.</span></span></span></p>
<p id="p2811183544"><a name="p2811183544"></a><a name="p2811183544"></a><span id="ph1479701815419"><a name="ph1479701815419"></a><a name="ph1479701815419"></a>The start address of LocalTensor must be 32-byte aligned.</span></p>
<p id="p2012716431610"><a name="p2012716431610"></a><a name="p2012716431610"></a>The data type of the source operand must be consistent with that of the destination operand.</p>
<p id="p11792551164616"><a name="p11792551164616"></a><a name="p11792551164616"></a><span id="ph11792155174611"><a name="ph11792155174611"></a><a name="ph11792155174611"></a>For Ascend 950PR/Ascend 950DT</span>, the supported data types are: uint8_t/int8_t/uint16_t/int16_t/half/bfloat16_t/uint32_t/int32_t/float/uint64_t/int64_t</p></td>
</tr>
<tr id="row103840207421"><td class="cellrowborder" valign="top" width="16.49%" headers="mcps1.2.4.1.1 "><p id="p20104222104514"><a name="p20104222104514"></a><a name="p20104222104514"></a>count</p></td>
<td class="cellrowborder" valign="top" width="11.93%" headers="mcps1.2.4.1.2 "><p id="p2183122716423"><a name="p2183122716423"></a><a name="p2183122716423"></a>Input</p></td>
<td class="cellrowborder" valign="top" width="71.58%" headers="mcps1.2.4.1.3 "><p id="p62147396317"><a name="p62147396317"></a><a name="p62147396317"></a>Number of input/output data elements. The length of dst0/dst1/src0/src1 is count. count must be an even number.</p></td>
</tr>
<tr id="row825185514307"><td class="cellrowborder" valign="top" width="16.49%" headers="mcps1.2.4.1.1 "><p id="p825165517307"><a name="p825165517307"></a><a name="p825165517307"></a>srcCount</p></td>
<td class="cellrowborder" valign="top" width="11.93%" headers="mcps1.2.4.1.2 "><p id="p18894175913307"><a name="p18894175913307"></a><a name="p18894175913307"></a>Input</p></td>
<td class="cellrowborder" valign="top" width="71.58%" headers="mcps1.2.4.1.3 "><p id="p1025125514300"><a name="p1025125514300"></a><a name="p1025125514300"></a>Number of input data elements. The size of each output is half of the input. srcCount must be an even number.</p></td>
</tr>
</tbody>
</table>

## Return Value<a name="section14483414194"></a>

None

## Constraints<a name="section633mcpsimp"></a>

None

## Example<a name="section176061616102911"></a>

This sample shows only some key code.

-   Two inputs

    ```
    AscendC::DeInterleave(dst0Local, dst1Local, src0Local, src1Local, 512);
    ```

    The result is as follows:

    ```
    Input data src0Local: [1 2 3 ... 512]
    Input data src1Local: [513 514 515 ... 1024]
    Output data dst0Local: [1 3 5 ... 1023]
    Output data dst1Local: [2 4 6 ... 1024]
    ```

-   One input

    ```
    AscendC::DeInterleave(dst0Local, dst1Local, srcLocal, 512);
    ```

    The result is as follows:

    ```
    Input data srcLocal: [1 2 3 ... 512]
    Output data dst0Local: [1 3 5 ... 511]
    Output data dst1Local: [2 4 6 ... 512]
    ```
