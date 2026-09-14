# VectorPadding \(ISASI\)<a name="ZH-CN_TOPIC_0000001870220489"></a>

<!-- md-trans-meta sourceCommit=0364dcce9ab16d714270f168ec9546fd7620f90f translatedAt=2026-09-09T14:36:16.921Z -->

## Applicable Products<a name="section1550532418810"></a>

<!-- npu="950" id1 -->
- Ascend 950PR/Ascend 950DT: Not supported
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
- Atlas inference products AI Core: Supported
<!-- end id5 -->
<!-- npu="310p" id6 -->
- Atlas inference products Vector Core: Not supported
<!-- end id6 -->
<!-- npu="910" id7 -->
- Atlas training products: Not supported
<!-- end id7 -->


## Description<a name="section618mcpsimp"></a>

Performs padding on the source operand by datablock according to padMode (padding mode) and padSide (padding direction).

Assume that a datablock of the source operand has 16 numbers, datablock\[0:15\]=a\~p:

-   padSide==false: padding starts from the left side of the datablock, that is, the direction of the start value of the datablock \(a-\>p\)

-   padSide==true: padding starts from the right side of the datablock, that is, the direction of the end value of the datablock \(p-\>a\)
-   padMode==0: uses the neighboring number as the padding value, for example: aaa|abc\(padSide=false\), nop|ppp\(padSide=true\)
-   padMode==1: performs symmetric padding using the neighboring datablock value, for example: cba|abc\(padSide=false\), nop|pon\(padSide=true\)
-   padMode==2: performs symmetric padding using the neighboring datablock value offset by one number, for example:
    -   padSide=false: xcb|abc, xcb is padded, padding process description: a is discarded, symmetric padding is performed, and 0 is padded at x
    -   padSide=true: nop|onx, onx is padded, padding process description: p is discarded, symmetric padding is performed, and 0 is padded at x

## Prototype<a name="section620mcpsimp"></a>

-   Compute the first n data of the tensor

    ```
    template <typename T>
    __aicore__ inline void VectorPadding(const LocalTensor<T>& dst, const LocalTensor<T>& src, const uint8_t padMode, const bool padSide, const uint32_t count)
    ```

-   Compute the tensor by high-dimensional slicing
    -   Bitwise mask mode

        ```
        template <typename T, bool isSetMask = true>
        __aicore__ inline void VectorPadding(const LocalTensor<T>& dst, const LocalTensor<T>& src, const uint8_t padMode, const bool padSide, const uint64_t mask[], const uint8_t repeatTime, const UnaryRepeatParams& repeatParams)
        ```

    -   Continuous mask mode

        ```
        template <typename T, bool isSetMask = true>
        __aicore__ inline void VectorPadding(const LocalTensor<T>& dst, const LocalTensor<T>& src, const uint8_t padMode, const bool padSide, const uint64_t mask, const uint8_t repeatTime, const UnaryRepeatParams& repeatParams)
        ```

## Parameters<a name="section622mcpsimp"></a>

**Table 1** Template parameters

<a name="table4835205712588"></a>
<table><thead align="left"><tr id="zh-cn_topic_0000001429830437_row118356578583"><th class="cellrowborder" valign="top" width="16.72%" id="mcps1.2.3.1.1"><p id="zh-cn_topic_0000001429830437_p48354572582"><a name="zh-cn_topic_0000001429830437_p48354572582"></a><a name="zh-cn_topic_0000001429830437_p48354572582"></a>Parameter</p></th>
<th class="cellrowborder" valign="top" width="83.28%" id="mcps1.2.3.1.2"><p id="zh-cn_topic_0000001429830437_p583535795817"><a name="zh-cn_topic_0000001429830437_p583535795817"></a><a name="zh-cn_topic_0000001429830437_p583535795817"></a>Description</p></th>
</tr>
</thead>
<tbody><tr id="zh-cn_topic_0000001429830437_row1835857145817"><td class="cellrowborder" valign="top" width="16.72%" headers="mcps1.2.3.1.1 "><p id="zh-cn_topic_0000001429830437_p5835457165816"><a name="zh-cn_topic_0000001429830437_p5835457165816"></a><a name="zh-cn_topic_0000001429830437_p5835457165816"></a>T</p></td>
<td class="cellrowborder" valign="top" width="83.28%" headers="mcps1.2.3.1.2 "><p id="zh-cn_topic_0000001429830437_p168351657155818"><a name="zh-cn_topic_0000001429830437_p168351657155818"></a><a name="zh-cn_topic_0000001429830437_p168351657155818"></a>Data type of the operand.</p>
<p id="p199917715712"><a name="p199917715712"></a><a name="p199917715712"></a><span id="ph20991107105718"><a name="ph20991107105718"></a><a name="ph20991107105718"></a><term id="zh-cn_topic_0000001312391781_term1964153212227_1"><a name="zh-cn_topic_0000001312391781_term1964153212227_1"></a><a name="zh-cn_topic_0000001312391781_term1964153212227_1"></a>Atlas inference series products</term>AI Core</span>, the supported data types are: int16_t/uint16_t/half/int32_t/uint32_t/float</p></td>
</tr>
<tr id="zh-cn_topic_0000001429830437_row18835145716587"><td class="cellrowborder" valign="top" width="16.72%" headers="mcps1.2.3.1.1 "><p id="zh-cn_topic_0000001429830437_p1383515717581"><a name="zh-cn_topic_0000001429830437_p1383515717581"></a><a name="zh-cn_topic_0000001429830437_p1383515717581"></a>isSetMask</p></td>
<td class="cellrowborder" valign="top" width="83.28%" headers="mcps1.2.3.1.2 "><p id="zh-cn_topic_0000001429830437_p77520541653"><a name="zh-cn_topic_0000001429830437_p77520541653"></a><a name="zh-cn_topic_0000001429830437_p77520541653"></a>Whether to set the mask inside the API.</p>
<a name="zh-cn_topic_0000001429830437_ul1163765616511"></a><a name="zh-cn_topic_0000001429830437_ul1163765616511"></a><ul id="zh-cn_topic_0000001429830437_ul1163765616511"><li>true: the mask is set inside the API.</li><li>false: the mask is set outside the API. The developer needs to use the <a href="../mask_operations/SetVectorMask.md">SetVectorMask</a> API to set the mask value. In this mode, the mask value in the API input parameter is set to the placeholder MASK_PLACEHOLDER, which is used only as a placeholder and has no actual meaning.</li></ul></td>
</tr>
</tbody>
</table>

**Table 2** Parameters

<a name="table1055216132132"></a>
<table><thead align="left"><tr id="row105531513121315"><th class="cellrowborder" valign="top" width="16.49%" id="mcps1.2.4.1.1"><p id="p5553171319138"><a name="p5553171319138"></a><a name="p5553171319138"></a>Parameter</p></th>
<th class="cellrowborder" valign="top" width="11.92%" id="mcps1.2.4.1.2"><p id="p5553151313131"><a name="p5553151313131"></a><a name="p5553151313131"></a>Input/Output</p></th>
<th class="cellrowborder" valign="top" width="71.59%" id="mcps1.2.4.1.3"><p id="p655316136139"><a name="p655316136139"></a><a name="p655316136139"></a>Description</p></th>
</tr>
</thead>
<tbody><tr id="row5553201314135"><td class="cellrowborder" valign="top" width="16.49%" headers="mcps1.2.4.1.1 "><p id="p8553813111314"><a name="p8553813111314"></a><a name="p8553813111314"></a>dst</p></td>
<td class="cellrowborder" valign="top" width="11.92%" headers="mcps1.2.4.1.2 "><p id="p755318134134"><a name="p755318134134"></a><a name="p755318134134"></a>Output</p></td>
<td class="cellrowborder" valign="top" width="71.59%" headers="mcps1.2.4.1.3 "><p id="p156168411944"><a name="p156168411944"></a><a name="p156168411944"></a>Destination operand.</p>
<p id="p5945720195112"><a name="p5945720195112"></a><a name="p5945720195112"></a><span id="zh-cn_topic_0000001530181537_ph173308471594"><a name="zh-cn_topic_0000001530181537_ph173308471594"></a><a name="zh-cn_topic_0000001530181537_ph173308471594"></a><span id="zh-cn_topic_0000001530181537_ph9902231466"><a name="zh-cn_topic_0000001530181537_ph9902231466"></a><a name="zh-cn_topic_0000001530181537_ph9902231466"></a><span id="zh-cn_topic_0000001530181537_ph1782115034816"><a name="zh-cn_topic_0000001530181537_ph1782115034816"></a><a name="zh-cn_topic_0000001530181537_ph1782115034816"></a>The type is <a href="../../data_structures/LocalTensor/LocalTensor.md">LocalTensor</a>, and the supported TPosition values are VECIN/VECCALC/VECOUT.</span></span></span></p>
<p id="p98123138213"><a name="p98123138213"></a><a name="p98123138213"></a><span id="ph1479701815419"><a name="ph1479701815419"></a><a name="ph1479701815419"></a>The start address of LocalTensor must be 32-byte aligned.</span></p></td>
</tr>
<tr id="row6553613191315"><td class="cellrowborder" valign="top" width="16.49%" headers="mcps1.2.4.1.1 "><p id="p195531113161311"><a name="p195531113161311"></a><a name="p195531113161311"></a>src</p></td>
<td class="cellrowborder" valign="top" width="11.92%" headers="mcps1.2.4.1.2 "><p id="p155310135134"><a name="p155310135134"></a><a name="p155310135134"></a>Input</p></td>
<td class="cellrowborder" valign="top" width="71.59%" headers="mcps1.2.4.1.3 "><p id="p1426211497413"><a name="p1426211497413"></a><a name="p1426211497413"></a>Source operand.</p>
<p id="p183768531842"><a name="p183768531842"></a><a name="p183768531842"></a><span id="zh-cn_topic_0000001530181537_ph173308471594_1"><a name="zh-cn_topic_0000001530181537_ph173308471594_1"></a><a name="zh-cn_topic_0000001530181537_ph173308471594_1"></a><span id="zh-cn_topic_0000001530181537_ph9902231466_1"><a name="zh-cn_topic_0000001530181537_ph9902231466_1"></a><a name="zh-cn_topic_0000001530181537_ph9902231466_1"></a><span id="zh-cn_topic_0000001530181537_ph1782115034816_1"><a name="zh-cn_topic_0000001530181537_ph1782115034816_1"></a><a name="zh-cn_topic_0000001530181537_ph1782115034816_1"></a>The type is <a href="../../data_structures/LocalTensor/LocalTensor.md">LocalTensor</a>, and the supported TPosition values are VECIN/VECCALC/VECOUT.</span></span></span></p>
<p id="p161831116162113"><a name="p161831116162113"></a><a name="p161831116162113"></a><span id="ph657711168215"><a name="ph657711168215"></a><a name="ph657711168215"></a>The start address of LocalTensor must be 32-byte aligned.</span></p>
<p id="p1955311137135"><a name="p1955311137135"></a><a name="p1955311137135"></a>The data type of the source operand must be the same as that of the destination operand.</p></td>
</tr>
<tr id="row1697704514916"><td class="cellrowborder" valign="top" width="16.49%" headers="mcps1.2.4.1.1 "><p id="p69771145164913"><a name="p69771145164913"></a><a name="p69771145164913"></a>padMode</p></td>
<td class="cellrowborder" valign="top" width="11.92%" headers="mcps1.2.4.1.2 "><p id="p1097714451496"><a name="p1097714451496"></a><a name="p1097714451496"></a>Input</p></td>
<td class="cellrowborder" valign="top" width="71.59%" headers="mcps1.2.4.1.3 "><p id="p89775453498"><a name="p89775453498"></a><a name="p89775453498"></a>Padding mode, of type uint8_t, with a value range of [0,2].</p>
<a name="ul11555455331"></a><a name="ul11555455331"></a><ul id="ul11555455331"><li>0: Use the neighboring value as the padding value.</li><li>1: Use the neighboring datablock value for symmetric padding.</li><li>2: Use the neighboring datablock value for padding, offset by one element, for symmetric padding.</li></ul></td>
</tr>
<tr id="row61804118512"><td class="cellrowborder" valign="top" width="16.49%" headers="mcps1.2.4.1.1 "><p id="p1418017105117"><a name="p1418017105117"></a><a name="p1418017105117"></a>padSide</p></td>
<td class="cellrowborder" valign="top" width="11.92%" headers="mcps1.2.4.1.2 "><p id="p21801419518"><a name="p21801419518"></a><a name="p21801419518"></a>Input</p></td>
<td class="cellrowborder" valign="top" width="71.59%" headers="mcps1.2.4.1.3 "><p id="p201801117514"><a name="p201801117514"></a><a name="p201801117514"></a>Padding direction, of type bool.</p>
<a name="ul1355515512334"></a><a name="ul1355515512334"></a><ul id="ul1355515512334"><li>false: left.</li><li>true: right.</li></ul></td>
</tr>
<tr id="row1873734314149"><td class="cellrowborder" valign="top" width="16.49%" headers="mcps1.2.4.1.1 "><p id="p86771552121414"><a name="p86771552121414"></a><a name="p86771552121414"></a>count</p></td>
<td class="cellrowborder" valign="top" width="11.92%" headers="mcps1.2.4.1.2 "><p id="p1767755217142"><a name="p1767755217142"></a><a name="p1767755217142"></a>Input</p></td>
<td class="cellrowborder" valign="top" width="71.59%" headers="mcps1.2.4.1.3 "><p id="p714120485355"><a name="p714120485355"></a><a name="p714120485355"></a>Number of elements involved in the computation.</p></td>
</tr>
<tr id="row16554713131317"><td class="cellrowborder" valign="top" width="16.49%" headers="mcps1.2.4.1.1 "><p id="p2554141321313"><a name="p2554141321313"></a><a name="p2554141321313"></a>mask[]/mask</p></td>
<td class="cellrowborder" valign="top" width="11.92%" headers="mcps1.2.4.1.2 "><p id="p755431341319"><a name="p755431341319"></a><a name="p755431341319"></a>Input</p></td>
<td class="cellrowborder" valign="top" width="71.59%" headers="mcps1.2.4.1.3 "><p id="zh-cn_topic_0000001530181537_p0554313181312"><a name="zh-cn_topic_0000001530181537_p0554313181312"></a><a name="zh-cn_topic_0000001530181537_p0554313181312"></a><span id="zh-cn_topic_0000001530181537_ph793119540147"><a name="zh-cn_topic_0000001530181537_ph793119540147"></a><a name="zh-cn_topic_0000001530181537_ph793119540147"></a><span id="zh-cn_topic_0000001530181537_ph199431319161515"><a name="zh-cn_topic_0000001530181537_ph199431319161515"></a><a name="zh-cn_topic_0000001530181537_ph199431319161515"></a><a href="../../../../../guide/programming_guide/library_api/basic_api/interface_classification/high_dimensional_sharding_api.md" target="_blank" rel="noopener noreferrer">mask</a></span> controls the elements that participate in the computation in each iteration.</span></p>
<a name="zh-cn_topic_0000001530181537_ul1255411133132"></a><a name="zh-cn_topic_0000001530181537_ul1255411133132"></a><ul id="zh-cn_topic_0000001530181537_ul1255411133132"><li>Bit-by-bit mode: controls which elements participate in the computation on a per-bit basis. A bit value of 1 indicates that the element participates in the computation, and 0 indicates that it does not.<p id="zh-cn_topic_0000001530181537_p121114581013"><a name="zh-cn_topic_0000001530181537_p121114581013"></a><a name="zh-cn_topic_0000001530181537_p121114581013"></a>mask is in array form. The array length and the value range of the array elements depend on the data type of the operand. When the operand is 16-bit, the array length is 2, and mask[0], mask[1]∈[0, 2<sup id="zh-cn_topic_0000001530181537_sup1411059101"><a name="zh-cn_topic_0000001530181537_sup1411059101"></a><a name="zh-cn_topic_0000001530181537_sup1411059101"></a>64</sup>-1], and they are not both 0. When the operand is 32-bit, the array length is 1, and mask[0]∈(0, 2<sup id="zh-cn_topic_0000001530181537_sup1711155161017"><a name="zh-cn_topic_0000001530181537_sup1711155161017"></a><a name="zh-cn_topic_0000001530181537_sup1711155161017"></a>64</sup>-1]. When the operand is 64-bit, the array length is 1, and mask[0]∈(0, 2<sup id="zh-cn_topic_0000001530181537_sup181195111019"><a name="zh-cn_topic_0000001530181537_sup181195111019"></a><a name="zh-cn_topic_0000001530181537_sup181195111019"></a>32</sup>-1].</p>
<p id="zh-cn_topic_0000001530181537_p711354105"><a name="zh-cn_topic_0000001530181537_p711354105"></a><a name="zh-cn_topic_0000001530181537_p711354105"></a>For example, mask=[8, 0], where 8=0b1000, indicates that only the 4th element participates in the computation.</p>
</li></ul>
<a name="zh-cn_topic_0000001530181537_ul18554121313135"></a><a name="zh-cn_topic_0000001530181537_ul18554121313135"></a><ul id="zh-cn_topic_0000001530181537_ul18554121313135"><li>Continuous mode: indicates how many consecutive elements at the front participate in the computation. The value range depends on the data type of the operand. Different data types allow different maximum numbers of elements to be processed in each iteration. When the operand is 16-bit, mask∈[1, 128]. When the operand is 32-bit, mask∈[1, 64]. When the operand is 64-bit, mask∈[1, 32].</li></ul></td>
</tr>
<tr id="row185542138131"><td class="cellrowborder" valign="top" width="16.49%" headers="mcps1.2.4.1.1 "><p id="p755471321311"><a name="p755471321311"></a><a name="p755471321311"></a>repeatTime</p></td>
<td class="cellrowborder" valign="top" width="11.92%" headers="mcps1.2.4.1.2 "><p id="p135541313101314"><a name="p135541313101314"></a><a name="p135541313101314"></a>Input</p></td>
<td class="cellrowborder" valign="top" width="71.59%" headers="mcps1.2.4.1.3 "><p id="p141775514"><a name="p141775514"></a><a name="p141775514"></a>Number of repeated iterations. The vector computation unit reads 256 bytes of consecutive data for computation each time. To complete the processing of the input data, multiple iterations (repeats) are required to finish reading and computing all the data. repeatTime indicates the number of iterations.</p>
<p id="p9554151321320"><a name="p9554151321320"></a><a name="p9554151321320"></a>For details about this parameter, see <span id="zh-cn_topic_0000001530181537_ph43961850144019"><a name="zh-cn_topic_0000001530181537_ph43961850144019"></a><a name="zh-cn_topic_0000001530181537_ph43961850144019"></a><a href="../../../../../guide/programming_guide/library_api/basic_api/interface_classification/high_dimensional_sharding_api.md" target="_blank" rel="noopener noreferrer">high-dimensional slicing API</a></span>.</p></td>
</tr>
<tr id="row195541813181310"><td class="cellrowborder" valign="top" width="16.49%" headers="mcps1.2.4.1.1 "><p id="p15554121320132"><a name="p15554121320132"></a><a name="p15554121320132"></a>repeatParams</p></td>
<td class="cellrowborder" valign="top" width="11.92%" headers="mcps1.2.4.1.2 "><p id="p18554141331317"><a name="p18554141331317"></a><a name="p18554141331317"></a>Input</p></td>
<td class="cellrowborder" valign="top" width="71.59%" headers="mcps1.2.4.1.3 "><p id="zh-cn_topic_0000001530181537_p455461351319"><a name="zh-cn_topic_0000001530181537_p455461351319"></a><a name="zh-cn_topic_0000001530181537_p455461351319"></a>Parameter that controls the address stride of the operand. It is of the <a href="../../aux_data_structures/UnaryRepeatParams.md">UnaryRepeatParams</a> type, and contains parameters such as the address stride of the same <span id="zh-cn_topic_0000001530181537_ph1256166185416"><a name="zh-cn_topic_0000001530181537_ph1256166185416"></a><a name="zh-cn_topic_0000001530181537_ph1256166185416"></a>DataBlock</span> between adjacent iterations of the operand, and the address stride of different <span id="zh-cn_topic_0000001530181537_ph131833567170"><a name="zh-cn_topic_0000001530181537_ph131833567170"></a><a name="zh-cn_topic_0000001530181537_ph131833567170"></a>DataBlock</span>s within the same iteration of the operand.</p>
<p id="zh-cn_topic_0000001530181537_p1156819418442"><a name="zh-cn_topic_0000001530181537_p1156819418442"></a><a name="zh-cn_topic_0000001530181537_p1156819418442"></a>For details about the address stride parameter between adjacent iterations, see <span id="zh-cn_topic_0000001530181537_zh-cn_topic_0000001487959374_ph18976413195617"><a name="zh-cn_topic_0000001530181537_zh-cn_topic_0000001487959374_ph18976413195617"></a><a name="zh-cn_topic_0000001530181537_zh-cn_topic_0000001487959374_ph18976413195617"></a><a href="../../../../../guide/programming_guide/library_api/basic_api/interface_classification/high_dimensional_sharding_api.md" target="_blank" rel="noopener noreferrer">repeatStride</a></span>. For details about the address stride parameter of DataBlock within the same iteration, see <span id="zh-cn_topic_0000001530181537_zh-cn_topic_0000001487959374_ph19976131385613"><a name="zh-cn_topic_0000001530181537_zh-cn_topic_0000001487959374_ph19976131385613"></a><a name="zh-cn_topic_0000001530181537_zh-cn_topic_0000001487959374_ph19976131385613"></a><a href="../../../../../guide/programming_guide/library_api/basic_api/interface_classification/high_dimensional_sharding_api.md" target="_blank" rel="noopener noreferrer">dataBlockStride</a></span>.</p></td>
</tr>
</tbody>
</table>

## Return Value<a name="section38228281712"></a>

None

## Constraints<a name="section633mcpsimp"></a>

-   For operand address alignment requirements, see [General Address Alignment Constraints](../../../general_description_and_constraints.md#section796754519912).

-   mask controls only which elements in the destination operand are to be written. The read of the source operand is independent of mask.
-   count indicates the total number of elements written to the destination operand. The read of the source operand is independent of count.

## Example<a name="section642mcpsimp"></a>

The srcLocal and dstLocal in the sample are both of the half type.

For more samples, see [LINK](../basic_arithmetic/more_sample_9.md).

-   Tensor high-dimensional slicing calculation sample - mask continuous mode

    ```
    uint64_t mask = 256 / sizeof(half);
    uint8_t padMode = 0;
    bool padSide = false;
    // repeatTime = 4, 128 elements one repeat, 512 elements total
    // dstBlkStride, srcBlkStride = 1, no gap between blocks in one repeat
    // dstRepStride, srcRepStride = 8, no gap between repeats
    AscendC::VectorPadding(dstLocal, srcLocal, padMode, padSide, mask, 4, { 1, 1, 8, 8 });
    ```

-   Tensor high-dimensional slicing calculation sample - mask bit-by-bit mode

    ```
    uint64_t mask[2] = { UINT64_MAX, UINT64_MAX };
    uint8_t padMode = 0;
    bool padSide = false;
    // repeatTime = 4, 128 elements one repeat, 512 elements total
    // dstBlkStride, srcBlkStride = 1, no gap between blocks in one repeat
    // dstRepStride, srcRepStride = 8, no gap between repeats
    AscendC::VectorPadding(dstLocal, srcLocal, padMode, padSide, mask, 4, { 1, 1, 8, 8 });
    ```

-   Tensor first n data calculation sample

    ```
    uint8_t padMode = 0;
    bool padSide = false;
    AscendC::VectorPadding(dstLocal, srcLocal, padMode, padSide, 512);
    ```

The result is as follows:

```
// Take the value of one datablock of srcLocal as an example, which has 16 numbers.
Input data (srcLocal): [6.938 -8.86 -0.2263 ... 1.971 1.778]
Output data (dstLocal): 
[6.938 6.938 6.938 ... 6.938 6.938]
```
