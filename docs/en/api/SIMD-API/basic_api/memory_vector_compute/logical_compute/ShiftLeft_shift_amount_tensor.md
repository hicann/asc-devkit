# ShiftLeft (Shift Amount as Tensor)<a name="ZH-CN_TOPIC_0000002158548560"></a>

<!-- md-trans-meta sourceCommit=0364dcce9ab16d714270f168ec9546fd7620f90f translatedAt=2026-09-09T12:46:55.044Z -->

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

Performs a left shift operation on each element in the source operand. Depending on the data type of the source operand, the left shift operation is classified into the following two cases:

-   The data type is an unsigned type: a logical left shift is performed. A logical left shift moves the entire binary number to the left by the specified number of bits, discards the most significant bit, and fills the least significant bit with 0. For example, after the binary number 1010101010101010 (uint16\_t type) is logically shifted left by 1 bit, the result is 0101010101010100.
-   The data type is a signed type: an arithmetic left shift is performed. An arithmetic left shift moves the entire binary number to the left by the specified number of bits, discards the second most significant bit, and fills the least significant bit with 0. For example, after the binary number 1010101010101010 (int16\_t type) is arithmetically shifted left by 1 bit, the result is 1101010101010100; after it is arithmetically shifted left by 3 bits, the result is 1101010101010000.

## Prototype<a name="section620mcpsimp"></a>

```
template <typename T, typename U>
__aicore__ inline void ShiftLeft(const LocalTensor<T>& dst, const LocalTensor<T>& src0, const LocalTensor<U>& src1, const int32_t& count)
```

## Parameters<a name="section622mcpsimp"></a>

**Table 1** Template parameters

<a name="table4835205712588"></a>
<table><thead align="left"><tr id="zh-cn_topic_0000001429830437_row118356578583"><th class="cellrowborder" valign="top" width="14.469999999999999%" id="mcps1.2.3.1.1"><p id="zh-cn_topic_0000001429830437_p48354572582"><a name="zh-cn_topic_0000001429830437_p48354572582"></a><a name="zh-cn_topic_0000001429830437_p48354572582"></a>Parameter</p></th>
<th class="cellrowborder" valign="top" width="85.53%" id="mcps1.2.3.1.2"><p id="zh-cn_topic_0000001429830437_p583535795817"><a name="zh-cn_topic_0000001429830437_p583535795817"></a><a name="zh-cn_topic_0000001429830437_p583535795817"></a>Description</p></th>
</tr>
</thead>
<tbody><tr id="zh-cn_topic_0000001429830437_row1835857145817"><td class="cellrowborder" valign="top" width="14.469999999999999%" headers="mcps1.2.3.1.1 "><p id="zh-cn_topic_0000001429830437_p5835457165816"><a name="zh-cn_topic_0000001429830437_p5835457165816"></a><a name="zh-cn_topic_0000001429830437_p5835457165816"></a>T</p></td>
<td class="cellrowborder" valign="top" width="85.53%" headers="mcps1.2.3.1.2 "><p id="zh-cn_topic_0000001429830437_p168351657155818"><a name="zh-cn_topic_0000001429830437_p168351657155818"></a><a name="zh-cn_topic_0000001429830437_p168351657155818"></a>Data type of the source/destination operand.</p>
<p id="p722214293126"><a name="p722214293126"></a><a name="p722214293126"></a><span id="ph6222129101217"><a name="ph6222129101217"></a><a name="ph6222129101217"></a>Ascend 950PR/Ascend 950DT</span>, the supported data types are: int8_t, uint8_t, int16_t, uint16_t, int32_t, uint32_t, int64_t, uint64_t.</p></td>
</tr>
<tr id="row1775882311432"><td class="cellrowborder" valign="top" width="14.469999999999999%" headers="mcps1.2.3.1.1 "><p id="p248833374113"><a name="p248833374113"></a><a name="p248833374113"></a>U</p></td>
<td class="cellrowborder" valign="top" width="85.53%" headers="mcps1.2.3.1.2 "><p id="p399424911454"><a name="p399424911454"></a><a name="p399424911454"></a>Data type of the source operand.</p>
<p id="p1928341811617"><a name="p1928341811617"></a><a name="p1928341811617"></a><span id="ph1728461819161"><a name="ph1728461819161"></a><a name="ph1728461819161"></a>Ascend 950PR/Ascend 950DT</span>, the supported data types are: int8_t, int16_t, int32_t, int64_t.</p></td>
</tr>
</tbody>
</table>

**Table 2** Parameters

<a name="table1549711469155"></a>
<table><thead align="left"><tr id="row12534194619150"><th class="cellrowborder" valign="top" width="14.510000000000002%" id="mcps1.2.4.1.1"><p id="p115341446121510"><a name="p115341446121510"></a><a name="p115341446121510"></a><strong id="b125344463152"><a name="b125344463152"></a><a name="b125344463152"></a>Parameter</strong></p></th>
<th class="cellrowborder" valign="top" width="9.49%" id="mcps1.2.4.1.2"><p id="zh-cn_topic_0235751031_p16609919"><a name="zh-cn_topic_0235751031_p16609919"></a><a name="zh-cn_topic_0235751031_p16609919"></a>Input/Output</p></th>
<th class="cellrowborder" valign="top" width="76%" id="mcps1.2.4.1.3"><p id="p6534046101518"><a name="p6534046101518"></a><a name="p6534046101518"></a><strong id="b105341546101519"><a name="b105341546101519"></a><a name="b105341546101519"></a>Description</strong></p></th>
</tr>
</thead>
<tbody><tr id="row1253413467153"><td class="cellrowborder" valign="top" width="14.510000000000002%" headers="mcps1.2.4.1.1 "><p id="p1534204617157"><a name="p1534204617157"></a><a name="p1534204617157"></a>dst</p></td>
<td class="cellrowborder" valign="top" width="9.49%" headers="mcps1.2.4.1.2 "><p id="p3534104620153"><a name="p3534104620153"></a><a name="p3534104620153"></a>Output</p></td>
<td class="cellrowborder" valign="top" width="76%" headers="mcps1.2.4.1.3 "><p id="p1228234191918"><a name="p1228234191918"></a><a name="p1228234191918"></a>Destination operand.</p>
<p id="p5945720195112"><a name="p5945720195112"></a><a name="p5945720195112"></a><span id="zh-cn_topic_0000001530181537_ph173308471594"><a name="zh-cn_topic_0000001530181537_ph173308471594"></a><a name="zh-cn_topic_0000001530181537_ph173308471594"></a><span id="zh-cn_topic_0000001530181537_ph9902231466"><a name="zh-cn_topic_0000001530181537_ph9902231466"></a><a name="zh-cn_topic_0000001530181537_ph9902231466"></a><span id="zh-cn_topic_0000001530181537_ph1782115034816"><a name="zh-cn_topic_0000001530181537_ph1782115034816"></a><a name="zh-cn_topic_0000001530181537_ph1782115034816"></a>The type is <a href="../../data_structures/LocalTensor/LocalTensor.md">LocalTensor</a>, and the supported TPosition is VECIN/VECCALC/VECOUT.</span></span></span></p>
<p id="p3247968151"><a name="p3247968151"></a><a name="p3247968151"></a><span id="ph1479701815419"><a name="ph1479701815419"></a><a name="ph1479701815419"></a>The start address of LocalTensor must be 32-byte aligned.</span></p></td>
</tr>
<tr id="row3534104617155"><td class="cellrowborder" valign="top" width="14.510000000000002%" headers="mcps1.2.4.1.1 "><p id="p1534946101517"><a name="p1534946101517"></a><a name="p1534946101517"></a>src0</p></td>
<td class="cellrowborder" valign="top" width="9.49%" headers="mcps1.2.4.1.2 "><p id="p14534164616158"><a name="p14534164616158"></a><a name="p14534164616158"></a>Input</p></td>
<td class="cellrowborder" valign="top" width="76%" headers="mcps1.2.4.1.3 "><p id="p824919322011"><a name="p824919322011"></a><a name="p824919322011"></a>Source operand.</p>
<p id="p1931925112017"><a name="p1931925112017"></a><a name="p1931925112017"></a><span id="zh-cn_topic_0000001530181537_ph173308471594_1"><a name="zh-cn_topic_0000001530181537_ph173308471594_1"></a><a name="zh-cn_topic_0000001530181537_ph173308471594_1"></a><span id="zh-cn_topic_0000001530181537_ph9902231466_1"><a name="zh-cn_topic_0000001530181537_ph9902231466_1"></a><a name="zh-cn_topic_0000001530181537_ph9902231466_1"></a><span id="zh-cn_topic_0000001530181537_ph1782115034816_1"><a name="zh-cn_topic_0000001530181537_ph1782115034816_1"></a><a name="zh-cn_topic_0000001530181537_ph1782115034816_1"></a>The type is <a href="../../data_structures/LocalTensor/LocalTensor.md">LocalTensor</a>, and the supported TPosition values are VECIN/VECCALC/VECOUT.</span></span></span></p>
<p id="p967411615235"><a name="p967411615235"></a><a name="p967411615235"></a><span id="ph1067456152317"><a name="ph1067456152317"></a><a name="ph1067456152317"></a>The start address of LocalTensor must be 32-byte aligned.</span></p>
<p id="p6534104601515"><a name="p6534104601515"></a><a name="p6534104601515"></a>The data type must be consistent with that of the destination operand.</p></td>
</tr>
<tr id="row1053417466157"><td class="cellrowborder" valign="top" width="14.510000000000002%" headers="mcps1.2.4.1.1 "><p id="p1253584619151"><a name="p1253584619151"></a><a name="p1253584619151"></a>src1</p></td>
<td class="cellrowborder" valign="top" width="9.49%" headers="mcps1.2.4.1.2 "><p id="p053534691510"><a name="p053534691510"></a><a name="p053534691510"></a>Input</p></td>
<td class="cellrowborder" valign="top" width="76%" headers="mcps1.2.4.1.3 "><p id="p053524613153"><a name="p053524613153"></a><a name="p053524613153"></a>LocalTensor that stores the left shift amount. The number of bytes of the data type must match the number of bytes of the element data type in the source <span>src0</span> operand Tensor. Setting it to a negative number is not supported.</p></td>
</tr>
<tr id="row8124195945818"><td class="cellrowborder" valign="top" width="14.510000000000002%" headers="mcps1.2.4.1.1 "><p id="p45661461598"><a name="p45661461598"></a><a name="p45661461598"></a>count</p></td>
<td class="cellrowborder" valign="top" width="9.49%" headers="mcps1.2.4.1.2 "><p id="p7566469592"><a name="p7566469592"></a><a name="p7566469592"></a>Input</p></td>
<td class="cellrowborder" valign="top" width="76%" headers="mcps1.2.4.1.3 "><p id="p55669635915"><a name="p55669635915"></a><a name="p55669635915"></a>Number of elements involved in the computation.</p></td>
</tr>
</tbody>
</table>

## Return Value<a name="section194321251175110"></a>

None

## Constraints<a name="section15109150164412"></a>

-   For operand address alignment requirements, see [General Address Alignment Constraints](../../../general_description_and_constraints.md#section796754519912).

-   For logical shift (unsigned data type), if the shift amount is greater than the data type bit width, the output is 0.
-   For arithmetic shift (signed data type), if src0 is less than 0, src1 is less than 0, and the shift amount is greater than the data type bit width, the output is -1; if src0 is greater than 0 and the shift amount is greater than the data type bit width, the output is 0.

## Example<a name="section132384819392"></a>

```
AscendC::ShiftLeft(dstLocal, srcLocal0, srcLocal1, 512);
```

The result is as follows:

```
Input data srcLocal0: [1 2 3 ... 512]
Input data srcLocal1: [2 2 2 ... 2]
Output data dstLocal: [4 8 12 ... 2048]
```
