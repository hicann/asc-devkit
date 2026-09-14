# Scatter \(ISASI\)<a name="ZH-CN_TOPIC_0000001862055393"></a>

<!-- md-trans-meta sourceCommit=e3c8c70b3f5d27f3770063ae4b535bc75d904e3e translatedAt=2026-09-09T12:18:50.320Z -->

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
- Atlas 200I/500 A2 inference products: Supported
<!-- end id4 -->
<!-- npu="310p" id5 -->
- Atlas inference products AI Core: Supported
<!-- end id5 -->
<!-- npu="310p" id6 -->
- Atlas inference products Vector Core: Not supported
<!-- end id6 -->
<!-- npu="910" id7 -->
- Atlas training products: Not supported
<!-- end id7 -->


## Description<a name="section17600329101418"></a>

Given a contiguous input tensor and a destination address offset tensor, the Scatter instruction generates a new result tensor based on the offset addresses and then scatters the input tensor into the result tensor.

Scatters the elements in the source operand src into the destination operand dst at the specified positions (determined jointly by dst\_offset and base\_addr).

## Prototype<a name="section15660625202219"></a>

-   Compute the first n data elements of the tensor

    ```
    template <typename T>
    __aicore__ inline void Scatter(const LocalTensor<T>& dst, const LocalTensor<T>& src, const LocalTensor<uint32_t>& dstOffset, const uint32_t dstBaseAddr, const uint32_t count)
    ```

-   High-dimensional split computation of the tensor
    -   Bitwise mask mode

        ```
        template <typename T>
        __aicore__ inline void Scatter(const LocalTensor<T>& dst, const LocalTensor<T>& src, const LocalTensor<uint32_t>& dstOffset, const uint32_t dstBaseAddr, const uint64_t mask[], const uint8_t repeatTime, const uint8_t srcRepStride)
        ```

    -   Continuous mask mode

        ```
        template <typename T>
        __aicore__ inline void Scatter(const LocalTensor<T>& dst, const LocalTensor<T>& src, const LocalTensor<uint32_t>& dstOffset, const uint32_t dstBaseAddr, const uint64_t mask, const uint8_t repeatTime, const uint8_t srcRepStride)
        ```

## Parameters<a name="section1619484392111"></a>

**Table 1** Template parameters

<a name="table4835205712588"></a>
<table><thead align="left"><tr id="zh-cn_topic_0000001429830437_row118356578583"><th class="cellrowborder" valign="top" width="14.729999999999999%" id="mcps1.2.3.1.1"><p id="zh-cn_topic_0000001429830437_p48354572582"><a name="zh-cn_topic_0000001429830437_p48354572582"></a><a name="zh-cn_topic_0000001429830437_p48354572582"></a>Parameter</p></th>
<th class="cellrowborder" valign="top" width="85.27%" id="mcps1.2.3.1.2"><p id="zh-cn_topic_0000001429830437_p583535795817"><a name="zh-cn_topic_0000001429830437_p583535795817"></a><a name="zh-cn_topic_0000001429830437_p583535795817"></a>Description</p></th>
</tr>
</thead>
<tbody><tr id="zh-cn_topic_0000001429830437_row1835857145817"><td class="cellrowborder" valign="top" width="14.729999999999999%" headers="mcps1.2.3.1.1 "><p id="zh-cn_topic_0000001429830437_p5835457165816"><a name="zh-cn_topic_0000001429830437_p5835457165816"></a><a name="zh-cn_topic_0000001429830437_p5835457165816"></a>T</p></td>
<td class="cellrowborder" valign="top" width="85.27%" headers="mcps1.2.3.1.2 "><p id="zh-cn_topic_0000001429830437_p168351657155818"><a name="zh-cn_topic_0000001429830437_p168351657155818"></a><a name="zh-cn_topic_0000001429830437_p168351657155818"></a>Data type of the operand.</p>
<p id="p1396113348309"><a name="p1396113348309"></a><a name="p1396113348309"></a><span id="ph1396213414302"><a name="ph1396213414302"></a><a name="ph1396213414302"></a>Ascend 950PR/Ascend 950DT</span>, supported data types: uint8_t/int8_t/uint16_t/int16_t/half/bfloat16_t/uint32_t/int32_t/float/uint64_t/int64_t</p>
<p id="p2767195044513"><a name="p2767195044513"></a><a name="p2767195044513"></a><span id="ph14412018583"><a name="ph14412018583"></a><a name="ph14412018583"></a><term id="zh-cn_topic_0000001312391781_term354143892110_1"><a name="zh-cn_topic_0000001312391781_term354143892110_1"></a><a name="zh-cn_topic_0000001312391781_term354143892110_1"></a>Atlas 200I/500 A2 inference product</term></span>, supported data types: uint8_t/int8_t/uint16_t/int16_t/half/uint32_t/int32_t/float</p>
<p id="p85261196445"><a name="p85261196445"></a><a name="p85261196445"></a><span id="ph052613904417"><a name="ph052613904417"></a><a name="ph052613904417"></a><term id="zh-cn_topic_0000001312391781_term1964153212227_1"><a name="zh-cn_topic_0000001312391781_term1964153212227_1"></a><a name="zh-cn_topic_0000001312391781_term1964153212227_1"></a>Atlas inference series product</term>AI Core</span>, supported data types: uint16_t/uint32_t/float/half</p></td>
</tr>
</tbody>
</table>

**Table 2** Parameters

<a name="table917mcpsimp"></a>
<table><thead align="left"><tr id="row923mcpsimp"><th class="cellrowborder" valign="top" width="15.02%" id="mcps1.2.4.1.1"><p id="p925mcpsimp"><a name="p925mcpsimp"></a><a name="p925mcpsimp"></a>Parameter</p></th>
<th class="cellrowborder" valign="top" width="10%" id="mcps1.2.4.1.2"><p id="p927mcpsimp"><a name="p927mcpsimp"></a><a name="p927mcpsimp"></a>Input/Output</p></th>
<th class="cellrowborder" valign="top" width="74.98%" id="mcps1.2.4.1.3"><p id="p929mcpsimp"><a name="p929mcpsimp"></a><a name="p929mcpsimp"></a>Description</p></th>
</tr>
</thead>
<tbody><tr id="row930mcpsimp"><td class="cellrowborder" valign="top" width="15.02%" headers="mcps1.2.4.1.1 "><p id="p2925016172518"><a name="p2925016172518"></a><a name="p2925016172518"></a>dst</p></td>
<td class="cellrowborder" valign="top" width="10%" headers="mcps1.2.4.1.2 "><p id="p199251416112517"><a name="p199251416112517"></a><a name="p199251416112517"></a>Output</p></td>
<td class="cellrowborder" valign="top" width="74.98%" headers="mcps1.2.4.1.3 "><p id="p292591672516"><a name="p292591672516"></a><a name="p292591672516"></a>Destination operand, of type LocalTensor.<span id="ph1479701815419"><a name="ph1479701815419"></a><a name="ph1479701815419"></a>The start address of LocalTensor must be 32-byte aligned.</span></p></td>
</tr>
<tr id="row937mcpsimp"><td class="cellrowborder" valign="top" width="15.02%" headers="mcps1.2.4.1.1 "><p id="p3926171610253"><a name="p3926171610253"></a><a name="p3926171610253"></a>src</p></td>
<td class="cellrowborder" valign="top" width="10%" headers="mcps1.2.4.1.2 "><p id="p4926121682518"><a name="p4926121682518"></a><a name="p4926121682518"></a>Input</p></td>
<td class="cellrowborder" valign="top" width="74.98%" headers="mcps1.2.4.1.3 "><p id="p49261616142516"><a name="p49261616142516"></a><a name="p49261616142516"></a>Source operand, of type LocalTensor. The data type must be consistent with that of dst.</p></td>
</tr>
<tr id="row18516194102416"><td class="cellrowborder" valign="top" width="15.02%" headers="mcps1.2.4.1.1 "><p id="p85164422415"><a name="p85164422415"></a><a name="p85164422415"></a>dstOffset</p></td>
<td class="cellrowborder" valign="top" width="10%" headers="mcps1.2.4.1.2 "><p id="p10516104162418"><a name="p10516104162418"></a><a name="p10516104162418"></a>Input</p></td>
<td class="cellrowborder" valign="top" width="74.98%" headers="mcps1.2.4.1.3 "><p id="p22319556114"><a name="p22319556114"></a><a name="p22319556114"></a>Stores the address offset of each element of the source operand in dst. The offset is <span>calculated based on</span> the base address dstBaseAddr of <span>dst</span>, in bytes. The value must be aligned with the bit width of the dst data type; otherwise, unexpected behavior may occur.</p>
<p id="p203229151200"><a name="p203229151200"></a><a name="p203229151200"></a>For the following models, the value range of the address offset only needs to not exceed the range of uint32_t.</p>
<p id="p16677257161910"><a name="p16677257161910"></a><a name="p16677257161910"></a><span id="ph20677145716192"><a name="ph20677145716192"></a><a name="ph20677145716192"></a><term id="zh-cn_topic_0000001312391781_term1964153212227_2"><a name="zh-cn_topic_0000001312391781_term1964153212227_2"></a><a name="zh-cn_topic_0000001312391781_term1964153212227_2"></a>Atlas inference series product</term>AI Core</span></p>
<p id="p774419412217"><a name="p774419412217"></a><a name="p774419412217"></a>For the following models, the value range of the address offset is as follows: when the operand is 8 bits, the value range is [0, 2<sup id="sup3920174615212"><a name="sup3920174615212"></a><a name="sup3920174615212"></a>16</sup>-1]; when the operand is 16 bits, the value range is [0, 2<sup id="sup99207467210"><a name="sup99207467210"></a><a name="sup99207467210"></a>17</sup>-1]; when the operand is 32 bits or 64 bits, the value only needs to not exceed the range of uint32_t. Exceeding the value range may cause unexpected output.</p>
<p id="p1867711577198"><a name="p1867711577198"></a><a name="p1867711577198"></a><span id="ph1067710571195"><a name="ph1067710571195"></a><a name="ph1067710571195"></a><term id="zh-cn_topic_0000001312391781_term354143892110_2"><a name="zh-cn_topic_0000001312391781_term354143892110_2"></a><a name="zh-cn_topic_0000001312391781_term354143892110_2"></a>Atlas 200I/500 A2 inference product</term></span></p>
<p id="p468305719192"><a name="p468305719192"></a><a name="p468305719192"></a><span id="ph126252025205"><a name="ph126252025205"></a><a name="ph126252025205"></a>Ascend 950PR/Ascend 950DT</span></p></td>
</tr>
<tr id="row19460165818515"><td class="cellrowborder" valign="top" width="15.02%" headers="mcps1.2.4.1.1 "><p id="p13461158175117"><a name="p13461158175117"></a><a name="p13461158175117"></a>dstBaseAddr</p></td>
<td class="cellrowborder" valign="top" width="10%" headers="mcps1.2.4.1.2 "><p id="p4461205845119"><a name="p4461205845119"></a><a name="p4461205845119"></a>Input</p></td>
<td class="cellrowborder" valign="top" width="74.98%" headers="mcps1.2.4.1.3 "><p id="p13461145835112"><a name="p13461145835112"></a><a name="p13461145835112"></a>Start address offset of dst, in bytes. The value must be aligned with the bit width of the dst data type; otherwise, unexpected behavior may occur.</p></td>
</tr>
<tr id="row20730549195712"><td class="cellrowborder" valign="top" width="15.02%" headers="mcps1.2.4.1.1 "><p id="p1473034913577"><a name="p1473034913577"></a><a name="p1473034913577"></a>count</p></td>
<td class="cellrowborder" valign="top" width="10%" headers="mcps1.2.4.1.2 "><p id="p9730649125711"><a name="p9730649125711"></a><a name="p9730649125711"></a>Input</p></td>
<td class="cellrowborder" valign="top" width="74.98%" headers="mcps1.2.4.1.3 "><p id="p4148133318548"><a name="p4148133318548"></a><a name="p4148133318548"></a>Number of data elements to be processed.</p></td>
</tr>
<tr id="row69861713087"><td class="cellrowborder" valign="top" width="15.02%" headers="mcps1.2.4.1.1 "><p id="p1728791441620"><a name="p1728791441620"></a><a name="p1728791441620"></a>mask/mask[]</p></td>
<td class="cellrowborder" valign="top" width="10%" headers="mcps1.2.4.1.2 "><p id="p1998614131587"><a name="p1998614131587"></a><a name="p1998614131587"></a>Input</p>
<p id="p19256025212"><a name="p19256025212"></a><a name="p19256025212"></a></p></td>
<td class="cellrowborder" valign="top" width="74.98%" headers="mcps1.2.4.1.3 "><p id="p1490502512373"><a name="p1490502512373"></a><a name="p1490502512373"></a><span id="ph42341681148"><a name="ph42341681148"></a><a name="ph42341681148"></a><span id="zh-cn_topic_0000001530181537_ph199431319161515"><a name="zh-cn_topic_0000001530181537_ph199431319161515"></a><a name="zh-cn_topic_0000001530181537_ph199431319161515"></a><a href="../SIMD_compute/high_dimension_slicing.md" target="_blank" rel="noopener noreferrer">mask</a></span> is used to control the elements that participate in the computation within each iteration.</span></p>
<a name="zh-cn_topic_0000001530181537_ul1255411133132"></a><a name="zh-cn_topic_0000001530181537_ul1255411133132"></a><ul id="zh-cn_topic_0000001530181537_ul1255411133132"><li>Continuous mode: indicates how many consecutive elements at the front participate in the computation. The value range is related to the data type of the operand. Different data types have different maximum numbers of elements that can be processed within each iteration. When the operand is 8-bit or 16-bit, mask∈[1, 128]; when the operand is 32-bit, mask∈[1, 64]; when the operand is 64-bit, mask∈[1, 32].</li></ul>
<a name="zh-cn_topic_0000001530181537_ul18554121313135"></a><a name="zh-cn_topic_0000001530181537_ul18554121313135"></a><ul id="zh-cn_topic_0000001530181537_ul18554121313135"><li>Bit-by-bit mode: controls which elements participate in the computation on a per-bit basis. A bit value of 1 indicates that the element participates in the computation, and 0 indicates that it does not. The parameter type is a uint64_t array of length 2.<p id="zh-cn_topic_0000001530181537_p45540136131"><a name="zh-cn_topic_0000001530181537_p45540136131"></a><a name="zh-cn_topic_0000001530181537_p45540136131"></a>For example, mask=[8, 0], where 8=0b1000, indicates that only the 4th element participates in the computation.</p>
<p id="zh-cn_topic_0000001530181537_p955461317139"><a name="zh-cn_topic_0000001530181537_p955461317139"></a><a name="zh-cn_topic_0000001530181537_p955461317139"></a>The parameter value range is related to the data type of the operand. Different data types have different maximum numbers of elements that can be processed within each iteration. When the operand is 8-bit or 16-bit, mask[0] and mask[1]∈[0, 2<sup id="zh-cn_topic_0000001530181537_sup1955414135136"><a name="zh-cn_topic_0000001530181537_sup1955414135136"></a><a name="zh-cn_topic_0000001530181537_sup1955414135136"></a>64</sup>-1] and are not both 0; when the operand is 32-bit, mask[1] is 0 and mask[0]∈(0, 2<sup id="zh-cn_topic_0000001530181537_sup5554111316132"><a name="zh-cn_topic_0000001530181537_sup5554111316132"></a><a name="zh-cn_topic_0000001530181537_sup5554111316132"></a>64</sup>-1]; when the operand is 64-bit, mask[1] is 0 and mask[0]∈(0, 2<sup id="zh-cn_topic_0000001530181537_sup1555451310138"><a name="zh-cn_topic_0000001530181537_sup1555451310138"></a><a name="zh-cn_topic_0000001530181537_sup1555451310138"></a>32</sup>-1].</p>
</li></ul></td>
</tr>
<tr id="row159263231086"><td class="cellrowborder" valign="top" width="15.02%" headers="mcps1.2.4.1.1 "><p id="p69261923387"><a name="p69261923387"></a><a name="p69261923387"></a>repeatTime</p></td>
<td class="cellrowborder" valign="top" width="10%" headers="mcps1.2.4.1.2 "><p id="p1092602315814"><a name="p1092602315814"></a><a name="p1092602315814"></a>Input</p></td>
<td class="cellrowborder" valign="top" width="74.98%" headers="mcps1.2.4.1.3 "><p id="p1692718231584"><a name="p1692718231584"></a><a name="p1692718231584"></a>Number of instruction iterations. Each iteration collects data of 8 datablocks. Value range: repeatTime∈[0,255].</p>
<div class="p" id="p0618144471917"><a name="p0618144471917"></a><a name="p0618144471917"></a>In particular, for the following models:<a name="ul12780145612209"></a><a name="ul12780145612209"></a><ul id="ul12780145612209"><li><span id="ph6903252152013"><a name="ph6903252152013"></a><a name="ph6903252152013"></a><term id="zh-cn_topic_0000001312391781_term354143892110_3"><a name="zh-cn_topic_0000001312391781_term354143892110_3"></a><a name="zh-cn_topic_0000001312391781_term354143892110_3"></a>Atlas 200I/500 A2 inference product</term></span></li><li><span id="ph14607113171416"><a name="ph14607113171416"></a><a name="ph14607113171416"></a>Ascend 950PR/Ascend 950DT</span></li></ul>
</div>
<p id="p18148356182117"><a name="p18148356182117"></a><a name="p18148356182117"></a>When the operand is <strong id="b84473109468"><a name="b84473109468"></a><a name="b84473109468"></a>8-bit</strong>, each iteration collects data of <strong id="b1247251454616"><a name="b1247251454616"></a><a name="b1247251454616"></a>4 datablocks</strong> (32Bytes).</p></td>
</tr>
<tr id="row05851326989"><td class="cellrowborder" valign="top" width="15.02%" headers="mcps1.2.4.1.1 "><p id="p55852261189"><a name="p55852261189"></a><a name="p55852261189"></a>srcRepStride</p></td>
<td class="cellrowborder" valign="top" width="10%" headers="mcps1.2.4.1.2 "><p id="p1858502610817"><a name="p1858502610817"></a><a name="p1858502610817"></a>Input</p></td>
<td class="cellrowborder" valign="top" width="74.98%" headers="mcps1.2.4.1.3 "><p id="p9585126281"><a name="p9585126281"></a><a name="p9585126281"></a>Address stride between adjacent iterations, in datablocks.</p></td>
</tr>
</tbody>
</table>

## Constraints<a name="section633mcpsimp"></a>

-   For the operand address alignment requirements, see [General Address Alignment Constraints](../../../general_description_and_constraints.md#section796754519912).
-   For the operand address overlap constraints, see [General Address Overlap Constraints](../../../general_description_and_constraints.md#section668772811100).

-   The offset addresses in dstOffset cannot have the same value. If two or more offsets are duplicated, the behavior is unpredictable.
-   For Ascend 950PR/Ascend 950DT, uint8\_t/int8\_t data types support only the API for computing the first n data of a tensor.

## Calling Example<a name="section11276201527"></a>

For a complete usage example, see [Scatter Compatibility Example](https://gitcode.com/cann/asc-devkit/tree/9.1.0/examples/01_simd_cpp_api/06_compatibility_guide/scatter).

```
uint32_t COUNT = 128;
AscendC::Scatter(dstLocal, srcLocal, dstOffsetLocal, (uint32_t)0, COUNT); // dstOffsetLocal stores the address offset of each source operand element in dst
```

Result example:

```
Input data dstOffsetLocal:
[254 252 250 ... 4 2 0]
Input data srcLocal (128 half-type data):
[0 1 2 ... 125 126 127]
Output data dstGlobal:
[127 126 125 ... 2 1 0]
```
