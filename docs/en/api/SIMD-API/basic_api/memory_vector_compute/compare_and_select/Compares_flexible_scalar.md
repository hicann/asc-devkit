# Compares (Flexible Scalar Position)<a name="ZH-CN_TOPIC_0000002136591178"></a>

<!-- md-trans-meta sourceCommit=0364dcce9ab16d714270f168ec9546fd7620f90f translatedAt=2026-09-09T07:58:53.690Z pushedAt=2026-09-10T03:14:37.438Z -->

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

Provides APIs for flexible scalar positions, supporting two scenarios: scalar first and scalar last. The scalar input supports configuring a single-point element of a LocalTensor. The calculation formula is as follows, where idx indicates the position coefficient of the single-point element of the LocalTensor.

![](../../../../figures/zh-cn_formulaimage_0000002516303034.png)

Supports multiple comparison modes:

-   LT: less than
-   GT: greater than

-   GE: greater than or equal to
-   EQ: equal to
-   NE: not equal to
-   LE: less than or equal to

## Prototype<a name="section620mcpsimp"></a>

-   Calculation of the first n data of a tensor

    ```
    template <typename T0 = BinaryDefaultType, typename T1 = BinaryDefaultType, bool isSetMask = true, const BinaryConfig &config = DEFAULT_BINARY_CONFIG, typename T2, typename T3, typename T4>
    __aicore__ inline void Compares(const T2& dst, const T3& src0, const T4& src1, CMPMODE cmpMode, uint32_t count)
    ```

-   High-dimensional slicing calculation of a tensor
    -   Bitwise mask mode

        ```
        template <typename T0 = BinaryDefaultType, typename T1 = BinaryDefaultType, bool isSetMask = true, const BinaryConfig &config = DEFAULT_BINARY_CONFIG, typename T2, typename T3, typename T4>
        __aicore__ inline void Compares(const T2& dst, const T3& src0, const T4& src1, CMPMODE cmpMode, const uint64_t mask[], uint8_t repeatTime, const UnaryRepeatParams& repeatParams)
        ```

    -   Continuous mask mode

        ```
        template <typename T0 = BinaryDefaultType, typename T1 = BinaryDefaultType, bool isSetMask = true, const BinaryConfig &config = DEFAULT_BINARY_CONFIG, typename T2, typename T3, typename T4>
        __aicore__ inline void Compares(const T2& dst, const T3& src0, const T4& src1, CMPMODE cmpMode, const uint64_t mask, uint8_t repeatTime, const UnaryRepeatParams& repeatParams)
        ```

## Parameters<a name="section622mcpsimp"></a>

**Table 1** Template parameters

<a name="table4835205712588"></a>
<table><thead align="left"><tr id="row118356578583"><th class="cellrowborder" valign="top" width="13.56%" id="mcps1.2.3.1.1"><p id="p48354572582"><a name="p48354572582"></a><a name="p48354572582"></a>Parameter</p></th>
<th class="cellrowborder" valign="top" width="86.44%" id="mcps1.2.3.1.2"><p id="p583535795817"><a name="p583535795817"></a><a name="p583535795817"></a>Description</p></th>
</tr>
</thead>
<tbody><tr id="row11492616168"><td class="cellrowborder" valign="top" width="13.56%" headers="mcps1.2.3.1.1 "><p id="p19933113132715"><a name="p19933113132715"></a><a name="p19933113132715"></a>T0</p></td>
<td class="cellrowborder" valign="top" width="86.44%" headers="mcps1.2.3.1.2 "><p id="p593343122716"><a name="p593343122716"></a><a name="p593343122716"></a>For the fixed scalar position API, indicates the data type of the source operand.</p>
<p id="p1284851132213"><a name="p1284851132213"></a><a name="p1284851132213"></a>In particular, for the flexible scalar position API, this is a reserved parameter that is not yet enabled and is retained for future feature extension. When it needs to be specified, pass the default value BinaryDefaultType.</p></td>
</tr>
<tr id="row1835857145817"><td class="cellrowborder" valign="top" width="13.56%" headers="mcps1.2.3.1.1 "><p id="p1826944532610"><a name="p1826944532610"></a><a name="p1826944532610"></a>T1</p></td>
<td class="cellrowborder" valign="top" width="86.44%" headers="mcps1.2.3.1.2 "><p id="p1526974512618"><a name="p1526974512618"></a><a name="p1526974512618"></a>For the fixed scalar position API, indicates the data type of the destination operand.</p>
<p id="p2094981215419"><a name="p2094981215419"></a><a name="p2094981215419"></a>In particular, for the flexible scalar position API, this is a reserved parameter that is not yet enabled and is retained for future feature extension. When it needs to be specified, pass the default value BinaryDefaultType.</p></td>
</tr>
<tr id="row18835145716587"><td class="cellrowborder" valign="top" width="13.56%" headers="mcps1.2.3.1.1 "><p id="p1383515717581"><a name="p1383515717581"></a><a name="p1383515717581"></a>isSetMask</p></td>
<td class="cellrowborder" valign="top" width="86.44%" headers="mcps1.2.3.1.2 "><p id="zh-cn_topic_0000001429830437_p77520541653"><a name="zh-cn_topic_0000001429830437_p77520541653"></a><a name="zh-cn_topic_0000001429830437_p77520541653"></a>Whether to set the mask inside the API.</p>
<a name="zh-cn_topic_0000001429830437_ul1163765616511"></a><a name="zh-cn_topic_0000001429830437_ul1163765616511"></a><ul id="zh-cn_topic_0000001429830437_ul1163765616511"><li>true: the mask is set inside the API.</li><li>false: the mask is set outside the API. The developer needs to use the <a href="../mask_operations/SetVectorMask.md">SetVectorMask</a> API to set the mask value. In this mode, the mask value in the API input parameters is set to the placeholder MASK_PLACEHOLDER, which is used only as a placeholder and has no actual meaning.</li></ul></td>
</tr>
<tr id="row18191732132111"><td class="cellrowborder" valign="top" width="13.56%" headers="mcps1.2.3.1.1 "><p id="p914318523429"><a name="p914318523429"></a><a name="p914318523429"></a>config</p></td>
<td class="cellrowborder" valign="top" width="86.44%" headers="mcps1.2.3.1.2 "><p id="p914313523426"><a name="p914313523426"></a><a name="p914313523426"></a>Type: BinaryConfig. This parameter takes effect when the scalar is a LocalTensor single-point element, and is used to specify the position of the single-point element operand. The default value is DEFAULT_BINARY_CONFIG, indicating that the right operand is the scalar.</p>
<a name="screen13143195284213"></a><a name="screen13143195284213"></a><pre class="screen" codetype="Cpp" id="screen13143195284213">struct BinaryConfig {
    int8_t scalarTensorIndex = 1; // Specify the position of the scalar when the scalar is a LocalTensor single-point element. 0 indicates the left operand, and 1 indicates the right operand.
};
constexpr BinaryConfig DEFAULT_BINARY_CONFIG = {1};</pre></td>
</tr>
<tr id="row2525035132114"><td class="cellrowborder" valign="top" width="13.56%" headers="mcps1.2.3.1.1 "><p id="p7144195244214"><a name="p7144195244214"></a><a name="p7144195244214"></a>T2</p></td>
<td class="cellrowborder" valign="top" width="86.44%" headers="mcps1.2.3.1.2 "><p id="p3172440173612"><a name="p3172440173612"></a><a name="p3172440173612"></a>LocalTensor type. The corresponding data type is automatically inferred based on the input parameter dst. Developers do not need to configure this parameter, but only need to ensure that dst meets the data type constraints.</p></td>
</tr>
<tr id="row1479163913213"><td class="cellrowborder" valign="top" width="13.56%" headers="mcps1.2.3.1.1 "><p id="p51440526428"><a name="p51440526428"></a><a name="p51440526428"></a>T3</p></td>
<td class="cellrowborder" valign="top" width="86.44%" headers="mcps1.2.3.1.2 "><p id="p151721640153610"><a name="p151721640153610"></a><a name="p151721640153610"></a>LocalTensor type or scalar type. The corresponding data type is automatically inferred based on the input parameter src0. Developers do not need to configure this parameter, but must ensure that src0 meets the data type constraints.</p></td>
</tr>
<tr id="row43501542122117"><td class="cellrowborder" valign="top" width="13.56%" headers="mcps1.2.3.1.1 "><p id="p7144352114214"><a name="p7144352114214"></a><a name="p7144352114214"></a>T4</p></td>
<td class="cellrowborder" valign="top" width="86.44%" headers="mcps1.2.3.1.2 "><p id="p11721940193620"><a name="p11721940193620"></a><a name="p11721940193620"></a>LocalTensor type or scalar type. The corresponding data type is automatically inferred based on the input parameter src1. Developers do not need to configure this parameter, but must ensure that src1 meets the data type constraints.</p></td>
</tr>
</tbody>
</table>

**Table 2** API parameters

<a name="table8955841508"></a>
<table><thead align="left"><tr id="row15956194105014"><th class="cellrowborder" valign="top" width="13.661366136613662%" id="mcps1.2.4.1.1"><p id="p7956144195014"><a name="p7956144195014"></a><a name="p7956144195014"></a>Parameter</p></th>
<th class="cellrowborder" valign="top" width="12.591259125912593%" id="mcps1.2.4.1.2"><p id="p1295624145013"><a name="p1295624145013"></a><a name="p1295624145013"></a>Input/Output</p></th>
<th class="cellrowborder" valign="top" width="73.74737473747375%" id="mcps1.2.4.1.3"><p id="p16956144145011"><a name="p16956144145011"></a><a name="p16956144145011"></a>Description</p></th>
</tr>
</thead>
<tbody><tr id="row5956546509"><td class="cellrowborder" valign="top" width="13.661366136613662%" headers="mcps1.2.4.1.1 "><p id="p19576531173410"><a name="p19576531173410"></a><a name="p19576531173410"></a>dst</p></td>
<td class="cellrowborder" valign="top" width="12.591259125912593%" headers="mcps1.2.4.1.2 "><p id="p16576163119347"><a name="p16576163119347"></a><a name="p16576163119347"></a>Output</p></td>
<td class="cellrowborder" valign="top" width="73.74737473747375%" headers="mcps1.2.4.1.3 "><p id="p6948101892510"><a name="p6948101892510"></a><a name="p6948101892510"></a>Destination operand.</p>
<p id="p5945720195112"><a name="p5945720195112"></a><a name="p5945720195112"></a><span id="zh-cn_topic_0000001530181537_ph173308471594"><a name="zh-cn_topic_0000001530181537_ph173308471594"></a><a name="zh-cn_topic_0000001530181537_ph173308471594"></a><span id="zh-cn_topic_0000001530181537_ph9902231466"><a name="zh-cn_topic_0000001530181537_ph9902231466"></a><a name="zh-cn_topic_0000001530181537_ph9902231466"></a><span id="zh-cn_topic_0000001530181537_ph1782115034816"><a name="zh-cn_topic_0000001530181537_ph1782115034816"></a><a name="zh-cn_topic_0000001530181537_ph1782115034816"></a>The type is <a href="../../data_structures/LocalTensor/LocalTensor.md">LocalTensor</a>, and the supported TPosition values are VECIN/VECCALC/VECOUT.</span></span></span></p>
<p id="p19153175153018"><a name="p19153175153018"></a><a name="p19153175153018"></a><span id="ph1479701815419"><a name="ph1479701815419"></a><a name="ph1479701815419"></a>The start address of LocalTensor must be 32-byte aligned.</span></p>
<p id="p547031144015"><a name="p547031144015"></a><a name="p547031144015"></a>dst is used to store the comparison results. The uint8_t data in dst is expanded bit by bit, and from left to right each bit represents the comparison result of src0 and src1 at the corresponding position. If the comparison result is true, the corresponding bit is 1; otherwise, it is 0.</p>
<p id="p0416193054814"><a name="p0416193054814"></a><a name="p0416193054814"></a><span id="ph19416153010482"><a name="ph19416153010482"></a><a name="ph19416153010482"></a>Ascend 950PR/Ascend 950DT</span>, the supported data type is: uint8_t</p></td>
</tr>
<tr id="row03241135142313"><td class="cellrowborder" valign="top" width="13.661366136613662%" headers="mcps1.2.4.1.1 "><p id="p943725919217"><a name="p943725919217"></a><a name="p943725919217"></a>src0/src1</p></td>
<td class="cellrowborder" valign="top" width="12.591259125912593%" headers="mcps1.2.4.1.2 "><p id="p34371759625"><a name="p34371759625"></a><a name="p34371759625"></a>Input</p></td>
<td class="cellrowborder" valign="top" width="73.74737473747375%" headers="mcps1.2.4.1.3 "><p id="p88022033103718"><a name="p88022033103718"></a><a name="p88022033103718"></a>Source operands in the flexible scalar position API.</p>
<a name="ul1343573724314"></a><a name="ul1343573724314"></a><ul id="ul1343573724314"><li>When the type is LocalTensor, it can be used as a vector operand or a scalar single-point element, and the supported TPosition values are VECIN/VECCALC/VECOUT.<p id="p9802163310371"><a name="p9802163310371"></a><a name="p9802163310371"></a><span id="ph12803633203712"><a name="ph12803633203712"></a><a name="ph12803633203712"></a>The start address of LocalTensor must be 32-byte aligned.</span></p>
<p id="p2803143314373"><a name="p2803143314373"></a><a name="p2803143314373"></a><span id="ph28036339376"><a name="ph28036339376"></a><a name="ph28036339376"></a>Ascend 950PR/Ascend 950DT</span>, the supported data types are: int8_t/uint8_t/int16_t/uint16_t/half/bfloat16_t/float/int32_t/uint32_t/int64_t/uint64_t/double (double only supports CMPMODE::EQ)</p>
</li><li>When the type is scalar:<p id="p157482021194415"><a name="p157482021194415"></a><a name="p157482021194415"></a><span id="ph1180353323718"><a name="ph1180353323718"></a><a name="ph1180353323718"></a>Ascend 950PR/Ascend 950DT</span>, the supported data types are: int8_t/uint8_t/int16_t/uint16_t/half/bfloat16_t/float/int32_t/uint32_t/int64_t/uint64_t/double (double only supports CMPMODE::EQ)</p>
</li></ul>
<p id="p380313383720"><a name="p380313383720"></a><a name="p380313383720"></a>The data type must be consistent with that of the destination operand.</p></td>
</tr>
<tr id="row103306116356"><td class="cellrowborder" valign="top" width="13.661366136613662%" headers="mcps1.2.4.1.1 "><p id="p10974181411356"><a name="p10974181411356"></a><a name="p10974181411356"></a>cmpMode</p></td>
<td class="cellrowborder" valign="top" width="12.591259125912593%" headers="mcps1.2.4.1.2 "><p id="p1797491412352"><a name="p1797491412352"></a><a name="p1797491412352"></a>Input</p></td>
<td class="cellrowborder" valign="top" width="73.74737473747375%" headers="mcps1.2.4.1.3 "><p id="p5974614143512"><a name="p5974614143512"></a><a name="p5974614143512"></a>CMPMODE type, indicating the comparison mode, including EQ, NE, GE, LE, GT, and LT.</p>
<a name="ul1714312547446"></a><a name="ul1714312547446"></a><ul id="ul1714312547446"><li>LT: src0 is less than src1</li><li>GT: src0 is greater than src1</li><li>GE: src0 is greater than or equal to src1</li><li>EQ: src0 is equal to src1</li><li>NE: src0 is not equal to src1</li><li>LE: src0 is less than or equal to src1</li></ul></td>
</tr>
<tr id="row6301859135119"><td class="cellrowborder" valign="top" width="13.661366136613662%" headers="mcps1.2.4.1.1 "><p id="p2554141321313"><a name="p2554141321313"></a><a name="p2554141321313"></a>mask/mask[]</p></td>
<td class="cellrowborder" valign="top" width="12.591259125912593%" headers="mcps1.2.4.1.2 "><p id="p10535746191515"><a name="p10535746191515"></a><a name="p10535746191515"></a>Input</p></td>
<td class="cellrowborder" valign="top" width="73.74737473747375%" headers="mcps1.2.4.1.3 "><p id="zh-cn_topic_0000001530181537_p0554313181312"><a name="zh-cn_topic_0000001530181537_p0554313181312"></a><a name="zh-cn_topic_0000001530181537_p0554313181312"></a><span id="ph42341681148"><a name="ph42341681148"></a><a name="ph42341681148"></a><span id="zh-cn_topic_0000001530181537_ph199431319161515"><a name="zh-cn_topic_0000001530181537_ph199431319161515"></a><a name="zh-cn_topic_0000001530181537_ph199431319161515"></a><a href="../SIMD_compute/high_dimension_slicing.md" target="_blank" rel="noopener noreferrer">mask</a></span> is used to control the elements that participate in the calculation in each iteration.</span></p>
<a name="zh-cn_topic_0000001530181537_ul1255411133132"></a><a name="zh-cn_topic_0000001530181537_ul1255411133132"></a><ul id="zh-cn_topic_0000001530181537_ul1255411133132"><li>Continuous mode: indicates how many consecutive elements at the front participate in the calculation. The value range is related to the data type of the operand. Different data types have different maximum numbers of elements that can be processed in each iteration. When the operand is 16-bit, mask∈[1, 128]; when the operand is 32-bit, mask∈[1, 64].</li></ul>
<a name="zh-cn_topic_0000001530181537_ul18554121313135"></a><a name="zh-cn_topic_0000001530181537_ul18554121313135"></a><ul id="zh-cn_topic_0000001530181537_ul18554121313135"><li>Bitwise mode: controls which elements participate in the calculation on a bit-by-bit basis. A bit value of 1 indicates participation in the calculation, and 0 indicates non-participation. The parameter type is a uint64_t array of length 2 or 4.<p id="zh-cn_topic_0000001530181537_p45540136131"><a name="zh-cn_topic_0000001530181537_p45540136131"></a><a name="zh-cn_topic_0000001530181537_p45540136131"></a>For example, mask=[8, 0], 8=0b1000, indicates that only the 4th element participates in the calculation.</p>
<p id="zh-cn_topic_0000001530181537_p955461317139"><a name="zh-cn_topic_0000001530181537_p955461317139"></a><a name="zh-cn_topic_0000001530181537_p955461317139"></a>The parameter value range is related to the data type of the operand. Different data types have different maximum numbers of elements that can be processed in each iteration. When the operand is 16-bit, mask[0] and mask[1]∈[0, 2<sup id="zh-cn_topic_0000001530181537_sup1955414135136"><a name="zh-cn_topic_0000001530181537_sup1955414135136"></a><a name="zh-cn_topic_0000001530181537_sup1955414135136"></a>64</sup>-1] and are not both 0; when the operand is 32-bit, mask[1] is 0, and mask[0]∈(0, 2<sup id="zh-cn_topic_0000001530181537_sup5554111316132"><a name="zh-cn_topic_0000001530181537_sup5554111316132"></a><a name="zh-cn_topic_0000001530181537_sup5554111316132"></a>64</sup>-1].</p>
</li></ul></td>
</tr>
<tr id="row0863135810539"><td class="cellrowborder" valign="top" width="13.661366136613662%" headers="mcps1.2.4.1.1 "><p id="p557663119345"><a name="p557663119345"></a><a name="p557663119345"></a>repeatTime</p></td>
<td class="cellrowborder" valign="top" width="12.591259125912593%" headers="mcps1.2.4.1.2 "><p id="p195761631163416"><a name="p195761631163416"></a><a name="p195761631163416"></a>Input</p></td>
<td class="cellrowborder" valign="top" width="73.74737473747375%" headers="mcps1.2.4.1.3 "><p id="p11994173311150"><a name="p11994173311150"></a><a name="p11994173311150"></a>Number of repeated iterations. The vector calculation unit reads 256 consecutive bytes of data for calculation each time. To complete the processing of the input data, multiple iterations (repeat) are required to finish reading and calculating all the data. repeatTime indicates the number of iterations.</p>
<p id="p9554151321320"><a name="p9554151321320"></a><a name="p9554151321320"></a>For details about this parameter, see <span id="zh-cn_topic_0000001530181537_ph43961850144019"><a name="zh-cn_topic_0000001530181537_ph43961850144019"></a><a name="zh-cn_topic_0000001530181537_ph43961850144019"></a><a href="../SIMD_compute/high_dimension_slicing.md" target="_blank" rel="noopener noreferrer">High-Dimensional Slicing API</a></span>.</p></td>
</tr>
<tr id="row5250192917342"><td class="cellrowborder" valign="top" width="13.661366136613662%" headers="mcps1.2.4.1.1 "><p id="p1325595674818"><a name="p1325595674818"></a><a name="p1325595674818"></a>repeatParams</p></td>
<td class="cellrowborder" valign="top" width="12.591259125912593%" headers="mcps1.2.4.1.2 "><p id="p172551556134814"><a name="p172551556134814"></a><a name="p172551556134814"></a>Input</p></td>
<td class="cellrowborder" valign="top" width="73.74737473747375%" headers="mcps1.2.4.1.3 "><p id="zh-cn_topic_0000001530181537_p455461351319"><a name="zh-cn_topic_0000001530181537_p455461351319"></a><a name="zh-cn_topic_0000001530181537_p455461351319"></a>Parameter that controls the address stride of the operands. It is of the <a href="../../aux_data_structures/UnaryRepeatParams.md">UnaryRepeatParams</a> type, and contains parameters such as the address stride of the same <span id="zh-cn_topic_0000001530181537_ph1256166185416"><a name="zh-cn_topic_0000001530181537_ph1256166185416"></a><a name="zh-cn_topic_0000001530181537_ph1256166185416"></a>DataBlock</span> between adjacent iterations of an operand, and the address stride of different <span id="zh-cn_topic_0000001530181537_ph131833567170"><a name="zh-cn_topic_0000001530181537_ph131833567170"></a><a name="zh-cn_topic_0000001530181537_ph131833567170"></a>DataBlock</span> within the same iteration of an operand.</p>
<p id="zh-cn_topic_0000001530181537_p1156819418442"><a name="zh-cn_topic_0000001530181537_p1156819418442"></a><a name="zh-cn_topic_0000001530181537_p1156819418442"></a>For details about the address stride parameter between adjacent iterations, see <span id="zh-cn_topic_0000001530181537_zh-cn_topic_0000001487959374_ph18976413195617"><a name="zh-cn_topic_0000001530181537_zh-cn_topic_0000001487959374_ph18976413195617"></a><a name="zh-cn_topic_0000001530181537_zh-cn_topic_0000001487959374_ph18976413195617"></a><a href="../SIMD_compute/high_dimension_slicing.md" target="_blank" rel="noopener noreferrer">repeatStride</a></span>; for details about the address stride parameter of DataBlock within the same iteration, see <span id="zh-cn_topic_0000001530181537_zh-cn_topic_0000001487959374_ph19976131385613"><a name="zh-cn_topic_0000001530181537_zh-cn_topic_0000001487959374_ph19976131385613"></a><a name="zh-cn_topic_0000001530181537_zh-cn_topic_0000001487959374_ph19976131385613"></a><a href="../SIMD_compute/high_dimension_slicing.md" target="_blank" rel="noopener noreferrer">dataBlockStride</a></span>.</p></td>
</tr>
<tr id="row1234319235496"><td class="cellrowborder" valign="top" width="13.661366136613662%" headers="mcps1.2.4.1.1 "><p id="p573202454917"><a name="p573202454917"></a><a name="p573202454917"></a>count</p></td>
<td class="cellrowborder" valign="top" width="12.591259125912593%" headers="mcps1.2.4.1.2 "><p id="p14732152414498"><a name="p14732152414498"></a><a name="p14732152414498"></a>Input</p></td>
<td class="cellrowborder" valign="top" width="73.74737473747375%" headers="mcps1.2.4.1.3 "><p id="p1289219753217"><a name="p1289219753217"></a><a name="p1289219753217"></a>Number of elements involved in the calculation.<strong id="b895117893820"><a name="b895117893820"></a><a name="b895117893820"></a>When setting count, ensure that the space occupied by the count elements is 256-byte aligned.</strong></p></td>
</tr>
</tbody>
</table>

## Return Value<a name="section128671456102513"></a>

None

## Constraints<a name="section633mcpsimp"></a>

-   For the operand address alignment requirements, see [General Address Alignment Constraints](../../../general_description_and_constraints.md#section796754519912).
-   When the flexible scalar position API is called and the source operand is a LocalTensor single-point element, overlapping of the source operand and destination operand addresses is not supported.

-   **dst** is sorted into a binary result in little-endian order, corresponding to the comparison result of the data at the corresponding position in **src**.
-   For the API that uses the first n data elements of a tensor for calculation, when setting count, ensure that the space occupied by the count elements is 256-byte aligned.
-   For Ascend 950PR/Ascend 950DT, the int8\_t, uint8\_t, int64\_t, uint64\_t, and double data types only support the API for calculating the first n data elements of a tensor, and double only supports CMPMODE::EQ.
-   At least one of the left operand and the right operand must be a vector. Currently, both the left and right operands being scalars is not supported.
-   When this API passes single-point LocalTensor data as a scalar, the idx parameter must be a compile-time constant. When a variable is passed, it must be declared as constexpr.

## Example<a name="section642mcpsimp"></a>

For the flexible scalar position API, it supports directly passing an immediate value or a single-point LocalTensor as the scalar, and it supports both calling modes with the scalar in the front and in the back.

For the complete example, see scenario 4 of the [Compare class example](https://gitcode.com/cann/asc-devkit/tree/9.1.0/examples/01_simd_cpp_api/03_basic_api/01_memory_vector_compute/compare).

-   Computation on the first n elements of a tensor

    ```
    // Scalar in the back, src1Local[0] as the scalar
    AscendC::Compares(dstLocal, src0Local, src1Local[0], AscendC::CMPMODE::LT, srcDataSize);
    
    // Scalar in the front, src0Local[0] as the scalar
    static constexpr AscendC::BinaryConfig config = { 0 };
    AscendC::Compares<BinaryDefaultType, BinaryDefaultType, true, config>(dstLocal, src0Local[0], src1Local, AscendC::CMPMODE::LT, srcDataSize);
    ```

-   Computation on high-dimensional tensor slices in continuous mask mode

    ```
    uint64_t mask = 256 / sizeof(float); // 256 is the number of bytes processed in each iteration
    int repeat = 4;
    AscendC::UnaryRepeatParams repeatParams = { 1, 1, 8, 8 };
    // repeat = 4, 64 elements one repeat, 256 elements total
    // dstBlkStride, srcBlkStride = 1, no gap between blocks in one repeat
    // dstRepStride, srcRepStride = 8, no gap between repeats
    // Scalar in the back, src1Local[0] serves as the scalar
    AscendC::Compares(dstLocal, src0Local, src1Local[0], AscendC::CMPMODE::LT, mask, repeat, repeatParams);
    
    // Scalar in the front, src0Local[0] serves as the scalar
    static constexpr AscendC::BinaryConfig config = { 0 };
    AscendC::Compares<BinaryDefaultType, BinaryDefaultType, true, config>(dstLocal, src0Local[0], src1Local, AscendC::CMPMODE::LT, mask, repeat, repeatParams);
    ```

-   Computation on high-dimensional tensor slices in bitwise mask mode

    ```
    uint64_t mask[2] = { UINT64_MAX, 0};
    int repeat = 4;
    AscendC::UnaryRepeatParams repeatParams = { 1, 1, 8, 8 };
    // repeat = 4, 64 elements one repeat, 256 elements total
    // srcBlkStride, = 1, no gap between blocks in one repeat
    // dstRepStride, srcRepStride = 8, no gap between repeats
    // Scalar in the back, src1Local[0] serves as the scalar
    AscendC::Compares(dstLocal, src0Local, src1Local[0], AscendC::CMPMODE::LT, mask, repeat, repeatParams);
    
    // Scalar in the front, src0Local[0] serves as the scalar
    static constexpr AscendC::BinaryConfig config = { 0 };
    AscendC::Compares<BinaryDefaultType, BinaryDefaultType, true, config>(dstLocal, src0Local[0], src1Local, AscendC::CMPMODE::LT, mask, repeat, repeatParams);
    ```
