# MulsCast\(ISASI\)<a name="ZH-CN_TOPIC_0000002158325172"></a>

<!-- md-trans-meta sourceCommit=0364dcce9ab16d714270f168ec9546fd7620f90f translatedAt=2026-09-09T09:10:49.028Z -->

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

Multiplies the first count elements of the vector source operand by a scalar and then converts the result to the half type in CAST\_ROUND mode, and writes the calculation result to dst. This API supports two scenarios: scalar first and scalar last. The calculation formula is as follows.

![](../../../../figures/zh-cn_formulaimage_0000002339181076.png)

The scalar input also supports configuring a LocalTensor single-point element. The calculation formula is as follows, where idx indicates the position coefficient of the LocalTensor single-point element.

![](../../../../figures/zh-cn_formulaimage_0000002339021404.png)

## Prototype<a name="section620mcpsimp"></a>

```
template <typename T0 = BinaryDefaultType, typename T1 = BinaryDefaultType, const BinaryConfig &config = DEFAULT_BINARY_CONFIG, typename T2, typename T3, typename T4>
__aicore__ inline void MulsCast(const T2 &dst, const T3 &src0, const T4 &src1, const uint32_t count)
```

## Parameters<a name="section622mcpsimp"></a>

**Table 1** Template parameters

<a name="table4835205712588"></a>
<table><thead align="left"><tr id="row118356578583"><th class="cellrowborder" valign="top" width="18.61%" id="mcps1.2.3.1.1"><p id="p48354572582"><a name="p48354572582"></a><a name="p48354572582"></a>Parameter</p></th>
<th class="cellrowborder" valign="top" width="81.39%" id="mcps1.2.3.1.2"><p id="p583535795817"><a name="p583535795817"></a><a name="p583535795817"></a>Description</p></th>
</tr>
</thead>
<tbody><tr id="row1835857145817"><td class="cellrowborder" valign="top" width="18.61%" headers="mcps1.2.3.1.1 "><p id="p5835457165816"><a name="p5835457165816"></a><a name="p5835457165816"></a>T0</p></td>
<td class="cellrowborder" valign="top" width="81.39%" headers="mcps1.2.3.1.2 "><p id="p168351657155818"><a name="p168351657155818"></a><a name="p168351657155818"></a>Data type of the destination operand.</p>
<p id="p17344122173213"><a name="p17344122173213"></a><a name="p17344122173213"></a><span id="ph117921554193319"><a name="ph117921554193319"></a><a name="ph117921554193319"></a>Reserved parameter, not enabled yet, reserved for future function extension. When it needs to be specified, pass the default value BinaryDefaultType.</span></p></td>
</tr>
<tr id="row1448571211232"><td class="cellrowborder" valign="top" width="18.61%" headers="mcps1.2.3.1.1 "><p id="p20119515162311"><a name="p20119515162311"></a><a name="p20119515162311"></a>T1</p></td>
<td class="cellrowborder" valign="top" width="81.39%" headers="mcps1.2.3.1.2 "><p id="p7484161220233"><a name="p7484161220233"></a><a name="p7484161220233"></a>Data type of the source operand.</p>
<p id="p8484121215238"><a name="p8484121215238"></a><a name="p8484121215238"></a><span id="ph1648417126239"><a name="ph1648417126239"></a><a name="ph1648417126239"></a>Reserved parameter, not enabled yet. It is reserved for future function extension. When it needs to be specified, pass the default value BinaryDefaultType.</span></p></td>
</tr>
<tr id="row211319191271"><td class="cellrowborder" valign="top" width="18.61%" headers="mcps1.2.3.1.1 "><p id="p4208135520361"><a name="p4208135520361"></a><a name="p4208135520361"></a>config</p></td>
<td class="cellrowborder" valign="top" width="81.39%" headers="mcps1.2.3.1.2 "><p id="p82081255153612"><a name="p82081255153612"></a><a name="p82081255153612"></a>Type: BinaryConfig. It takes effect when the scalar is a single-point element of the LocalTensor type, and is used to specify the position of the single-point element operand. The default value DEFAULT_BINARY_CONFIG indicates that the right operand is a scalar.</p>
<a name="screen420895553620"></a><a name="screen420895553620"></a><pre class="screen" codetype="Cpp" id="screen420895553620">struct BinaryConfig {
    int8_t scalarTensorIndex = 1; // Specify the position of the scalar when the scalar is a single-point element of the LocalTensor type. 0 indicates the left operand, and 1 indicates the right operand.
};
constexpr BinaryConfig DEFAULT_BINARY_CONFIG = {1};</pre></td>
</tr>
<tr id="row9113219132715"><td class="cellrowborder" valign="top" width="18.61%" headers="mcps1.2.3.1.1 "><p id="p1617215407362"><a name="p1617215407362"></a><a name="p1617215407362"></a>T2</p></td>
<td class="cellrowborder" valign="top" width="81.39%" headers="mcps1.2.3.1.2 "><p id="p3172440173612"><a name="p3172440173612"></a><a name="p3172440173612"></a>LocalTensor type. The corresponding data type is automatically derived based on the input parameter dst. Developers do not need to configure this parameter, but must ensure that dst meets the data type constraints.</p></td>
</tr>
<tr id="row71131019152712"><td class="cellrowborder" valign="top" width="18.61%" headers="mcps1.2.3.1.1 "><p id="p191723408365"><a name="p191723408365"></a><a name="p191723408365"></a>T3</p></td>
<td class="cellrowborder" valign="top" width="81.39%" headers="mcps1.2.3.1.2 "><p id="p151721640153610"><a name="p151721640153610"></a><a name="p151721640153610"></a>LocalTensor type or scalar type. The corresponding data type is automatically derived based on the input parameter src0. Developers do not need to configure this parameter, but must ensure that src0 meets the data type constraints.</p></td>
</tr>
<tr id="row71131919202715"><td class="cellrowborder" valign="top" width="18.61%" headers="mcps1.2.3.1.1 "><p id="p1717216409367"><a name="p1717216409367"></a><a name="p1717216409367"></a>T4</p></td>
<td class="cellrowborder" valign="top" width="81.39%" headers="mcps1.2.3.1.2 "><p id="p11721940193620"><a name="p11721940193620"></a><a name="p11721940193620"></a>LocalTensor type or scalar type. The corresponding data type is automatically derived based on the input parameter src1. Developers do not need to configure this parameter, but must ensure that src1 meets the data type constraints.</p></td>
</tr>
</tbody>
</table>

**Table 2** Parameters

<a name="zh-cn_topic_0235751031_table33761356"></a>
<table><thead align="left"><tr id="zh-cn_topic_0235751031_row27598891"><th class="cellrowborder" valign="top" width="18.54%" id="mcps1.2.4.1.1"><p id="zh-cn_topic_0235751031_p20917673"><a name="zh-cn_topic_0235751031_p20917673"></a><a name="zh-cn_topic_0235751031_p20917673"></a>Parameter</p></th>
<th class="cellrowborder" valign="top" width="10.05%" id="mcps1.2.4.1.2"><p id="zh-cn_topic_0235751031_p16609919"><a name="zh-cn_topic_0235751031_p16609919"></a><a name="zh-cn_topic_0235751031_p16609919"></a>Input/Output</p></th>
<th class="cellrowborder" valign="top" width="71.41%" id="mcps1.2.4.1.3"><p id="zh-cn_topic_0235751031_p59995477"><a name="zh-cn_topic_0235751031_p59995477"></a><a name="zh-cn_topic_0235751031_p59995477"></a>Description</p></th>
</tr>
</thead>
<tbody><tr id="row42461942101815"><td class="cellrowborder" valign="top" width="18.54%" headers="mcps1.2.4.1.1 "><p id="p284425844311"><a name="p284425844311"></a><a name="p284425844311"></a>dst</p></td>
<td class="cellrowborder" valign="top" width="10.05%" headers="mcps1.2.4.1.2 "><p id="p158449584436"><a name="p158449584436"></a><a name="p158449584436"></a>Output</p></td>
<td class="cellrowborder" valign="top" width="71.41%" headers="mcps1.2.4.1.3 "><p id="p6389114616514"><a name="p6389114616514"></a><a name="p6389114616514"></a>Destination operand.</p>
<p id="p5945720195112"><a name="p5945720195112"></a><a name="p5945720195112"></a><span id="zh-cn_topic_0000001530181537_ph173308471594"><a name="zh-cn_topic_0000001530181537_ph173308471594"></a><a name="zh-cn_topic_0000001530181537_ph173308471594"></a><span id="zh-cn_topic_0000001530181537_ph9902231466"><a name="zh-cn_topic_0000001530181537_ph9902231466"></a><a name="zh-cn_topic_0000001530181537_ph9902231466"></a><span id="zh-cn_topic_0000001530181537_ph1782115034816"><a name="zh-cn_topic_0000001530181537_ph1782115034816"></a><a name="zh-cn_topic_0000001530181537_ph1782115034816"></a>The type is <a href="../../data_structures/LocalTensor/LocalTensor.md">LocalTensor</a>, and the supported TPosition values are VECIN/VECCALC/VECOUT.</span></span></span></p>
<p id="p2811183544"><a name="p2811183544"></a><a name="p2811183544"></a><span id="ph1479701815419"><a name="ph1479701815419"></a><a name="ph1479701815419"></a>The start address of LocalTensor must be 32-byte aligned.</span></p>
<p id="p468305719192"><a name="p468305719192"></a><a name="p468305719192"></a><span id="ph126252025205"><a name="ph126252025205"></a><a name="ph126252025205"></a>Ascend 950PR/Ascend 950DT</span>, the supported data type is: half</p>
<p id="p84471955113219"><a name="p84471955113219"></a><a name="p84471955113219"></a>For the precision conversion rules corresponding to different data types, see <a href="#table235404962912">Table 3</a>.</p></td>
</tr>
<tr id="row2137145181815"><td class="cellrowborder" valign="top" width="18.54%" headers="mcps1.2.4.1.1 "><p id="p943725919217"><a name="p943725919217"></a><a name="p943725919217"></a>src0/src1</p></td>
<td class="cellrowborder" valign="top" width="10.05%" headers="mcps1.2.4.1.2 "><p id="p34371759625"><a name="p34371759625"></a><a name="p34371759625"></a>Input</p></td>
<td class="cellrowborder" valign="top" width="71.41%" headers="mcps1.2.4.1.3 "><p id="p88022033103718"><a name="p88022033103718"></a><a name="p88022033103718"></a>Source operand.</p>
<a name="ul1343573724314"></a><a name="ul1343573724314"></a><ul id="ul1343573724314"><li>When the type is LocalTensor, it can be used as a vector operand or a scalar single-point element. The supported TPosition values are VECIN/VECCALC/VECOUT.<p id="p9802163310371"><a name="p9802163310371"></a><a name="p9802163310371"></a><span id="ph12803633203712"><a name="ph12803633203712"></a><a name="ph12803633203712"></a>The start address of LocalTensor must be 32-byte aligned.</span></p>
<p id="p2803143314373"><a name="p2803143314373"></a><a name="p2803143314373"></a><span id="ph28036339376"><a name="ph28036339376"></a><a name="ph28036339376"></a>Ascend 950PR/Ascend 950DT</span>, the supported data type is: float</p>
</li><li>When the type is scalar:<p id="p157482021194415"><a name="p157482021194415"></a><a name="p157482021194415"></a><span id="ph1180353323718"><a name="ph1180353323718"></a><a name="ph1180353323718"></a>Ascend 950PR/Ascend 950DT</span>, the supported data type is: float</p>
</li></ul></td>
</tr>
<tr id="row891912431168"><td class="cellrowborder" valign="top" width="18.54%" headers="mcps1.2.4.1.1 "><p id="p195756503168"><a name="p195756503168"></a><a name="p195756503168"></a>count</p></td>
<td class="cellrowborder" valign="top" width="10.05%" headers="mcps1.2.4.1.2 "><p id="p457515071618"><a name="p457515071618"></a><a name="p457515071618"></a>Input</p></td>
<td class="cellrowborder" valign="top" width="71.41%" headers="mcps1.2.4.1.3 "><p id="p044121644612"><a name="p044121644612"></a><a name="p044121644612"></a>Number of elements involved in the computation.</p></td>
</tr>
</tbody>
</table>

**Table 3**  Precision conversion rules

<a name="table235404962912"></a>
<table><thead align="left"><tr id="row935554942920"><th class="cellrowborder" valign="top" width="10.191019101910191%" id="mcps1.2.4.1.1"><p id="p13355144922911"><a name="p13355144922911"></a><a name="p13355144922911"></a>src type</p></th>
<th class="cellrowborder" valign="top" width="9.43094309430943%" id="mcps1.2.4.1.2"><p id="p135514913299"><a name="p135514913299"></a><a name="p135514913299"></a>dst type</p></th>
<th class="cellrowborder" valign="top" width="80.37803780378037%" id="mcps1.2.4.1.3"><p id="p7113121774314"><a name="p7113121774314"></a><a name="p7113121774314"></a>Type conversion mode description</p></th>
</tr>
</thead>
<tbody><tr id="row3355849152915"><td class="cellrowborder" align="left" valign="top" width="10.191019101910191%" headers="mcps1.2.4.1.1 "><p id="p9687163213521"><a name="p9687163213521"></a><a name="p9687163213521"></a>float</p></td>
<td class="cellrowborder" align="left" valign="top" width="9.43094309430943%" headers="mcps1.2.4.1.2 "><p id="p113551749202919"><a name="p113551749202919"></a><a name="p113551749202919"></a>half</p></td>
<td class="cellrowborder" valign="top" width="80.37803780378037%" headers="mcps1.2.4.1.3 "><p id="p43071952153512"><a name="p43071952153512"></a><a name="p43071952153512"></a>Rounds the source operand to a value representable by half in CAST_ROUND mode, and stores the result in dst in half format (overflow is processed by saturation by default).</p></td>
</tr>
</tbody>
</table>

## Return Value<a name="section640mcpsimp"></a>

None

## Constraints<a name="section633mcpsimp"></a>

-   At least one of the left operand and the right operand must be a vector; the case where both the left and right operands are scalars is not supported.
-   When a single-point element of a LocalTensor is passed as a scalar to this API, the idx parameter must be a compile-time constant. If a variable is passed, it must be declared as constexpr.

## Example<a name="section642mcpsimp"></a>

```
// Example with the scalar on the right
AscendC::MulsCast(dstLocal, src0Local, src1Local[0], 512);

// Example with the scalar on the left
static constexpr AscendC::BinaryConfig config = { 0 };
AscendC::MulsCast<BinaryDefaultType, BinaryDefaultType, config>(dstLocal, src0Local[0], src1Local, 512);
```

The result is as follows:

```
Input data src0Local: [6 5 11 ... ]
Input data src1Local: 2
Output data dstLocal: [12 10 22 ... ]
```
