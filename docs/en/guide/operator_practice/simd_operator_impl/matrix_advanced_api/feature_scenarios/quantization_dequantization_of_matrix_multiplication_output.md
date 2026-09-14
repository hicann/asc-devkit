# Quantization/Dequantization of Matmul Output<a name="ZH-CN_TOPIC_0000002500468242"></a>

<!-- md-trans-meta sourceCommit=e3c8c70b3f5d27f3770063ae4b535bc75d904e3e translatedAt=2026-08-26T13:36:16.941Z -->

## Description<a name="zh-cn_topic_0000002298767893_section310824820358"></a>

For [specific input and output data types](#zh-cn_topic_0000002298767893_table1996113269499), Matmul supports performing data quantization or dequantization on the elements of the output C matrix when moving the computation result from CO1 to Global Memory.

-   Matmul quantization scenario: During Matmul computation, the left matrix A and right matrix B are of the half or bfloat16_t data type, and the output C matrix is of the int8_t data type. In this scenario, when the data of the C matrix is moved from CO1 to Global Memory, a quantization operation is performed to quantize the final result to the int8_t type, as shown in the following figure.

    **Figure 1**  Matmul quantization scenario schematic diagram<a name="zh-cn_topic_0000002298767893_fig14487971312"></a>  
    ![](../../../../figures/matmul_quantization_scenario_diagram.png "Matmul quantization scenario schematic diagram")

-   Matmul dequantization scenario: During Matmul computation, the left matrix A and right matrix B are of the int8_t or int4b_t data type and the output C matrix is of the half data type, or the left matrix A and right matrix B are of the int8_t data type and the output C matrix is of the int8_t data type. In this scenario, when the data of the C matrix is moved from CO1 to Global Memory, a dequantization operation is performed to dequantize the final result to the corresponding half type or int8_t type, as shown in the following figure.

    **Figure 2**  Matmul dequantization scenario schematic diagram<a name="zh-cn_topic_0000002298767893_fig12707172751211"></a>  
    ![](../../../../figures/matmul_dequantization_scenario_diagram.png "Matmul dequantization scenario schematic diagram")

Matmul quantization/dequantization includes two modes: the same-coefficient quantization/dequantization mode and the vector quantization/dequantization mode. The developer calls the [SetDequantType](../../../../../api/SIMD-API/advanced_api/matrix_compute/Matmul-Tiling/SetDequantType.md) API on the operator Tiling side to set the quantization or dequantization mode. The specific differences between the two modes are as follows:

-   Same-coefficient quantization/dequantization mode (PER_TENSOR mode): The entire C matrix corresponds to one quantization parameter, whose shape is [1]. The developer calls the [SetQuantScalar](../../../../../api/SIMD-API/advanced_api/matrix_compute/Matmul-Kernel/SetQuantScalar.md) API on the operator Kernel side to set the quantization parameter.
-   Vector quantization/dequantization mode (PER_CHANNEL mode): The shape of the C matrix is [m, n]. Each channel dimension, that is, each column of the C matrix, corresponds to one quantization parameter, whose shape is [n]. The developer calls the [SetQuantVector](../../../../../api/SIMD-API/advanced_api/matrix_compute/Matmul-Kernel/SetQuantVector.md) API on the operator Kernel side to set the quantization parameter.

**Table 1**  Interface configuration corresponding to the quantization/dequantization modes

<a name="zh-cn_topic_0000002298767893_table6550119507"></a>
<table><thead align="left"><tr id="zh-cn_topic_0000002298767893_row125501219302"><th class="cellrowborder" valign="top" width="19.26192619261926%" id="mcps1.2.4.1.1"><p id="zh-cn_topic_0000002298767893_p35491192010"><a name="zh-cn_topic_0000002298767893_p35491192010"></a><a name="zh-cn_topic_0000002298767893_p35491192010"></a>Mode</p></th>
<th class="cellrowborder" valign="top" width="47.4047404740474%" id="mcps1.2.4.1.2"><p id="zh-cn_topic_0000002298767893_p1550161912012"><a name="zh-cn_topic_0000002298767893_p1550161912012"></a><a name="zh-cn_topic_0000002298767893_p1550161912012"></a>Tiling Side API</p></th>
<th class="cellrowborder" valign="top" width="33.33333333333333%" id="mcps1.2.4.1.3"><p id="zh-cn_topic_0000002298767893_p75502019905"><a name="zh-cn_topic_0000002298767893_p75502019905"></a><a name="zh-cn_topic_0000002298767893_p75502019905"></a>Kernel Side API</p></th>
</tr>
</thead>
<tbody><tr id="zh-cn_topic_0000002298767893_row135501319102"><td class="cellrowborder" valign="top" width="19.26192619261926%" headers="mcps1.2.4.1.1 "><p id="zh-cn_topic_0000002298767893_p165508195019"><a name="zh-cn_topic_0000002298767893_p165508195019"></a><a name="zh-cn_topic_0000002298767893_p165508195019"></a>Quantization/dequantization with the same coefficient</p></td>
<td class="cellrowborder" valign="top" width="47.4047404740474%" headers="mcps1.2.4.1.2 "><p id="zh-cn_topic_0000002298767893_p055018196010"><a name="zh-cn_topic_0000002298767893_p055018196010"></a><a name="zh-cn_topic_0000002298767893_p055018196010"></a>SetDequantType(DequantType::SCALAR)</p></td>
<td class="cellrowborder" valign="top" width="33.33333333333333%" headers="mcps1.2.4.1.3 "><p id="zh-cn_topic_0000002298767893_p1055012191506"><a name="zh-cn_topic_0000002298767893_p1055012191506"></a><a name="zh-cn_topic_0000002298767893_p1055012191506"></a>SetQuantScalar(gmScalar)</p></td>
</tr>
<tr id="zh-cn_topic_0000002298767893_row45507193020"><td class="cellrowborder" valign="top" width="19.26192619261926%" headers="mcps1.2.4.1.1 "><p id="zh-cn_topic_0000002298767893_p1255019191208"><a name="zh-cn_topic_0000002298767893_p1255019191208"></a><a name="zh-cn_topic_0000002298767893_p1255019191208"></a>Vector quantization/dequantization</p></td>
<td class="cellrowborder" valign="top" width="47.4047404740474%" headers="mcps1.2.4.1.2 "><p id="zh-cn_topic_0000002298767893_p155505193017"><a name="zh-cn_topic_0000002298767893_p155505193017"></a><a name="zh-cn_topic_0000002298767893_p155505193017"></a>SetDequantType(DequantType::TENSOR)</p></td>
<td class="cellrowborder" valign="top" width="33.33333333333333%" headers="mcps1.2.4.1.3 "><p id="zh-cn_topic_0000002298767893_p11550819205"><a name="zh-cn_topic_0000002298767893_p11550819205"></a><a name="zh-cn_topic_0000002298767893_p11550819205"></a>SetQuantVector(gmTensor)</p></td>
</tr>
</tbody>
</table>

## Scenarios<a name="zh-cn_topic_0000002298767893_section118051016163613"></a>

Scenarios where the matrix computation result needs to be quantized or dequantized. In these scenarios, the data types supported by the Matmul input and output matrices are listed in the following table.

**Table 2**  Data types supported by Matmul quantization/dequantization

<a name="zh-cn_topic_0000002298767893_table1996113269499"></a>
<table><thead align="left"><tr id="zh-cn_topic_0000002298767893_row14961182654919"><th class="cellrowborder" valign="top" width="20.7%" id="mcps1.2.5.1.1"><p id="zh-cn_topic_0000002298767893_p1696192654916"><a name="zh-cn_topic_0000002298767893_p1696192654916"></a><a name="zh-cn_topic_0000002298767893_p1696192654916"></a>A Matrix</p></th>
<th class="cellrowborder" valign="top" width="18.96%" id="mcps1.2.5.1.2"><p id="zh-cn_topic_0000002298767893_p1796116269498"><a name="zh-cn_topic_0000002298767893_p1796116269498"></a><a name="zh-cn_topic_0000002298767893_p1796116269498"></a>B Matrix</p></th>
<th class="cellrowborder" valign="top" width="16.71%" id="mcps1.2.5.1.3"><p id="zh-cn_topic_0000002298767893_p12961122616491"><a name="zh-cn_topic_0000002298767893_p12961122616491"></a><a name="zh-cn_topic_0000002298767893_p12961122616491"></a>C Matrix</p></th>
<th class="cellrowborder" valign="top" width="43.63%" id="mcps1.2.5.1.4"><p id="zh-cn_topic_0000002298767893_p484471411911"><a name="zh-cn_topic_0000002298767893_p484471411911"></a><a name="zh-cn_topic_0000002298767893_p484471411911"></a>Supported Platform</p></th>
</tr>
</thead>
<tbody><tr id="zh-cn_topic_0000002298767893_row233918527911"><td class="cellrowborder" valign="top" width="20.7%" headers="mcps1.2.5.1.1 "><p id="zh-cn_topic_0000002298767893_p127712332147"><a name="zh-cn_topic_0000002298767893_p127712332147"></a><a name="zh-cn_topic_0000002298767893_p127712332147"></a>half</p></td>
<td class="cellrowborder" valign="top" width="18.96%" headers="mcps1.2.5.1.2 "><p id="zh-cn_topic_0000002298767893_p1454973121514"><a name="zh-cn_topic_0000002298767893_p1454973121514"></a><a name="zh-cn_topic_0000002298767893_p1454973121514"></a>half</p></td>
<td class="cellrowborder" valign="top" width="16.71%" headers="mcps1.2.5.1.3 "><p id="zh-cn_topic_0000002298767893_p47617337148"><a name="zh-cn_topic_0000002298767893_p47617337148"></a><a name="zh-cn_topic_0000002298767893_p47617337148"></a>int8_t</p></td>
<td class="cellrowborder" valign="top" width="43.63%" headers="mcps1.2.5.1.4 "><p id="p10570131022219"><a name="p10570131022219"></a><a name="p10570131022219"></a><span id="zh-cn_topic_0000002270097206_ph3777122762015"><a name="zh-cn_topic_0000002270097206_ph3777122762015"></a><a name="zh-cn_topic_0000002270097206_ph3777122762015"></a>Ascend 950PR/Ascend 950DT</span></p>
<p id="p5933413132214"><a name="p5933413132214"></a><a name="p5933413132214"></a><span id="zh-cn_topic_0000002298767893_ph126654381515"><a name="zh-cn_topic_0000002298767893_ph126654381515"></a><a name="zh-cn_topic_0000002298767893_ph126654381515"></a><term id="zh-cn_topic_0000001312391781_term1253731311225"><a name="zh-cn_topic_0000001312391781_term1253731311225"></a><a name="zh-cn_topic_0000001312391781_term1253731311225"></a>Atlas A3 training products</term>/<term id="zh-cn_topic_0000001312391781_term131434243115"><a name="zh-cn_topic_0000001312391781_term131434243115"></a><a name="zh-cn_topic_0000001312391781_term131434243115"></a>Atlas A3 inference products</term></span></p>
<p id="p67721622219"><a name="p67721622219"></a><a name="p67721622219"></a><span id="zh-cn_topic_0000002298767893_ph3661443121518"><a name="zh-cn_topic_0000002298767893_ph3661443121518"></a><a name="zh-cn_topic_0000002298767893_ph3661443121518"></a><term id="zh-cn_topic_0000001312391781_term11962195213215"><a name="zh-cn_topic_0000001312391781_term11962195213215"></a><a name="zh-cn_topic_0000001312391781_term11962195213215"></a>Atlas A2 training products</term>/<term id="zh-cn_topic_0000001312391781_term184716139811"><a name="zh-cn_topic_0000001312391781_term184716139811"></a><a name="zh-cn_topic_0000001312391781_term184716139811"></a>Atlas A2 inference products</term></span></p></td>
</tr>
<tr id="zh-cn_topic_0000002298767893_row029855420910"><td class="cellrowborder" valign="top" width="20.7%" headers="mcps1.2.5.1.1 "><p id="zh-cn_topic_0000002298767893_p17592161819155"><a name="zh-cn_topic_0000002298767893_p17592161819155"></a><a name="zh-cn_topic_0000002298767893_p17592161819155"></a>bfloat16_t</p></td>
<td class="cellrowborder" valign="top" width="18.96%" headers="mcps1.2.5.1.2 "><p id="zh-cn_topic_0000002298767893_p1059141841511"><a name="zh-cn_topic_0000002298767893_p1059141841511"></a><a name="zh-cn_topic_0000002298767893_p1059141841511"></a>bfloat16_t</p></td>
<td class="cellrowborder" valign="top" width="16.71%" headers="mcps1.2.5.1.3 "><p id="zh-cn_topic_0000002298767893_p155917189154"><a name="zh-cn_topic_0000002298767893_p155917189154"></a><a name="zh-cn_topic_0000002298767893_p155917189154"></a>int8_t</p></td>
<td class="cellrowborder" valign="top" width="43.63%" headers="mcps1.2.5.1.4 "><p id="p19106152016221"><a name="p19106152016221"></a><a name="p19106152016221"></a><span id="ph12577144817315"><a name="ph12577144817315"></a><a name="ph12577144817315"></a>Ascend 950PR/Ascend 950DT</span></p>
<p id="p41061420132214"><a name="p41061420132214"></a><a name="p41061420132214"></a><span id="zh-cn_topic_0000002298767893_ph1686194414153"><a name="zh-cn_topic_0000002298767893_ph1686194414153"></a><a name="zh-cn_topic_0000002298767893_ph1686194414153"></a><term id="zh-cn_topic_0000001312391781_term1253731311225_1"><a name="zh-cn_topic_0000001312391781_term1253731311225_1"></a><a name="zh-cn_topic_0000001312391781_term1253731311225_1"></a>Atlas A3 training products</term>/<term id="zh-cn_topic_0000001312391781_term131434243115_1"><a name="zh-cn_topic_0000001312391781_term131434243115_1"></a><a name="zh-cn_topic_0000001312391781_term131434243115_1"></a>Atlas A3 inference products</term></span></p>
<p id="p1110762092216"><a name="p1110762092216"></a><a name="p1110762092216"></a><span id="zh-cn_topic_0000002298767893_ph1286444111520"><a name="zh-cn_topic_0000002298767893_ph1286444111520"></a><a name="zh-cn_topic_0000002298767893_ph1286444111520"></a><term id="zh-cn_topic_0000001312391781_term11962195213215_1"><a name="zh-cn_topic_0000001312391781_term11962195213215_1"></a><a name="zh-cn_topic_0000001312391781_term11962195213215_1"></a>Atlas A2 training products</term>/<term id="zh-cn_topic_0000001312391781_term184716139811_1"><a name="zh-cn_topic_0000001312391781_term184716139811_1"></a><a name="zh-cn_topic_0000001312391781_term184716139811_1"></a>Atlas A2 inference products</term></span></p></td>
</tr>
<tr id="zh-cn_topic_0000002298767893_row1760512313141"><td class="cellrowborder" valign="top" width="20.7%" headers="mcps1.2.5.1.1 "><p id="zh-cn_topic_0000002298767893_p123398521294"><a name="zh-cn_topic_0000002298767893_p123398521294"></a><a name="zh-cn_topic_0000002298767893_p123398521294"></a>int8_t</p></td>
<td class="cellrowborder" valign="top" width="18.96%" headers="mcps1.2.5.1.2 "><p id="zh-cn_topic_0000002298767893_p92511749151314"><a name="zh-cn_topic_0000002298767893_p92511749151314"></a><a name="zh-cn_topic_0000002298767893_p92511749151314"></a>int8_t</p></td>
<td class="cellrowborder" valign="top" width="16.71%" headers="mcps1.2.5.1.3 "><p id="zh-cn_topic_0000002298767893_p183391252190"><a name="zh-cn_topic_0000002298767893_p183391252190"></a><a name="zh-cn_topic_0000002298767893_p183391252190"></a>half</p></td>
<td class="cellrowborder" valign="top" width="43.63%" headers="mcps1.2.5.1.4 "><p id="p167912472211"><a name="p167912472211"></a><a name="p167912472211"></a><span id="ph1241915516313"><a name="ph1241915516313"></a><a name="ph1241915516313"></a>Ascend 950PR/Ascend 950DT</span></p>
<p id="p197952442213"><a name="p197952442213"></a><a name="p197952442213"></a><span id="zh-cn_topic_0000002298767893_ph1711518201413"><a name="zh-cn_topic_0000002298767893_ph1711518201413"></a><a name="zh-cn_topic_0000002298767893_ph1711518201413"></a><term id="zh-cn_topic_0000001312391781_term1253731311225_2"><a name="zh-cn_topic_0000001312391781_term1253731311225_2"></a><a name="zh-cn_topic_0000001312391781_term1253731311225_2"></a>Atlas A3 training products</term>/<term id="zh-cn_topic_0000001312391781_term131434243115_2"><a name="zh-cn_topic_0000001312391781_term131434243115_2"></a><a name="zh-cn_topic_0000001312391781_term131434243115_2"></a>Atlas A3 inference products</term></span></p>
<p id="p980524132216"><a name="p980524132216"></a><a name="p980524132216"></a><span id="zh-cn_topic_0000002298767893_ph181164212142"><a name="zh-cn_topic_0000002298767893_ph181164212142"></a><a name="zh-cn_topic_0000002298767893_ph181164212142"></a><term id="zh-cn_topic_0000001312391781_term11962195213215_2"><a name="zh-cn_topic_0000001312391781_term11962195213215_2"></a><a name="zh-cn_topic_0000001312391781_term11962195213215_2"></a>Atlas A2 training products</term>/<term id="zh-cn_topic_0000001312391781_term184716139811_2"><a name="zh-cn_topic_0000001312391781_term184716139811_2"></a><a name="zh-cn_topic_0000001312391781_term184716139811_2"></a>Atlas A2 inference products</term></span></p></td>
</tr>
<tr id="zh-cn_topic_0000002298767893_row1167273521411"><td class="cellrowborder" valign="top" width="20.7%" headers="mcps1.2.5.1.1 "><p id="zh-cn_topic_0000002298767893_p1350520556135"><a name="zh-cn_topic_0000002298767893_p1350520556135"></a><a name="zh-cn_topic_0000002298767893_p1350520556135"></a>int4b_t</p></td>
<td class="cellrowborder" valign="top" width="18.96%" headers="mcps1.2.5.1.2 "><p id="zh-cn_topic_0000002298767893_p4211161771817"><a name="zh-cn_topic_0000002298767893_p4211161771817"></a><a name="zh-cn_topic_0000002298767893_p4211161771817"></a>int4b_t</p></td>
<td class="cellrowborder" valign="top" width="16.71%" headers="mcps1.2.5.1.3 "><p id="zh-cn_topic_0000002298767893_p6505175511137"><a name="zh-cn_topic_0000002298767893_p6505175511137"></a><a name="zh-cn_topic_0000002298767893_p6505175511137"></a>half</p></td>
<td class="cellrowborder" valign="top" width="43.63%" headers="mcps1.2.5.1.4 "><p id="p3150112912220"><a name="p3150112912220"></a><a name="p3150112912220"></a><span id="zh-cn_topic_0000002298767893_ph29803213147"><a name="zh-cn_topic_0000002298767893_ph29803213147"></a><a name="zh-cn_topic_0000002298767893_ph29803213147"></a><term id="zh-cn_topic_0000001312391781_term1253731311225_3"><a name="zh-cn_topic_0000001312391781_term1253731311225_3"></a><a name="zh-cn_topic_0000001312391781_term1253731311225_3"></a>Atlas A3 training products</term>/<term id="zh-cn_topic_0000001312391781_term131434243115_3"><a name="zh-cn_topic_0000001312391781_term131434243115_3"></a><a name="zh-cn_topic_0000001312391781_term131434243115_3"></a>Atlas A3 inference products</term></span></p>
<p id="p715012914226"><a name="p715012914226"></a><a name="p715012914226"></a><span id="zh-cn_topic_0000002298767893_ph1980192201415"><a name="zh-cn_topic_0000002298767893_ph1980192201415"></a><a name="zh-cn_topic_0000002298767893_ph1980192201415"></a><term id="zh-cn_topic_0000001312391781_term11962195213215_3"><a name="zh-cn_topic_0000001312391781_term11962195213215_3"></a><a name="zh-cn_topic_0000001312391781_term11962195213215_3"></a>Atlas A2 training products</term>/<term id="zh-cn_topic_0000001312391781_term184716139811_3"><a name="zh-cn_topic_0000001312391781_term184716139811_3"></a><a name="zh-cn_topic_0000001312391781_term184716139811_3"></a>Atlas A2 inference products</term></span></p></td>
</tr>
<tr id="zh-cn_topic_0000002298767893_row199152121811"><td class="cellrowborder" valign="top" width="20.7%" headers="mcps1.2.5.1.1 "><p id="zh-cn_topic_0000002298767893_p134722285187"><a name="zh-cn_topic_0000002298767893_p134722285187"></a><a name="zh-cn_topic_0000002298767893_p134722285187"></a>int8_t</p></td>
<td class="cellrowborder" valign="top" width="18.96%" headers="mcps1.2.5.1.2 "><p id="zh-cn_topic_0000002298767893_p3472132815187"><a name="zh-cn_topic_0000002298767893_p3472132815187"></a><a name="zh-cn_topic_0000002298767893_p3472132815187"></a>int8_t</p></td>
<td class="cellrowborder" valign="top" width="16.71%" headers="mcps1.2.5.1.3 "><p id="zh-cn_topic_0000002298767893_p1472528181812"><a name="zh-cn_topic_0000002298767893_p1472528181812"></a><a name="zh-cn_topic_0000002298767893_p1472528181812"></a>int8_t</p></td>
<td class="cellrowborder" valign="top" width="43.63%" headers="mcps1.2.5.1.4 "><p id="p027033292218"><a name="p027033292218"></a><a name="p027033292218"></a><span id="ph3448115113216"><a name="ph3448115113216"></a><a name="ph3448115113216"></a>Ascend 950PR/Ascend 950DT</span></p>
<p id="p5270132192215"><a name="p5270132192215"></a><a name="p5270132192215"></a><span id="zh-cn_topic_0000002298767893_ph13275143812188"><a name="zh-cn_topic_0000002298767893_ph13275143812188"></a><a name="zh-cn_topic_0000002298767893_ph13275143812188"></a><term id="zh-cn_topic_0000001312391781_term1253731311225_4"><a name="zh-cn_topic_0000001312391781_term1253731311225_4"></a><a name="zh-cn_topic_0000001312391781_term1253731311225_4"></a>Atlas A3 training products</term>/<term id="zh-cn_topic_0000001312391781_term131434243115_4"><a name="zh-cn_topic_0000001312391781_term131434243115_4"></a><a name="zh-cn_topic_0000001312391781_term131434243115_4"></a>Atlas A3 inference products</term></span></p>
<p id="p10270133292215"><a name="p10270133292215"></a><a name="p10270133292215"></a><span id="zh-cn_topic_0000002298767893_ph1327623871813"><a name="zh-cn_topic_0000002298767893_ph1327623871813"></a><a name="zh-cn_topic_0000002298767893_ph1327623871813"></a><term id="zh-cn_topic_0000001312391781_term11962195213215_4"><a name="zh-cn_topic_0000001312391781_term11962195213215_4"></a><a name="zh-cn_topic_0000001312391781_term11962195213215_4"></a>Atlas A2 training products</term>/<term id="zh-cn_topic_0000001312391781_term184716139811_4"><a name="zh-cn_topic_0000001312391781_term184716139811_4"></a><a name="zh-cn_topic_0000001312391781_term184716139811_4"></a>Atlas A2 inference products</term></span></p></td>
</tr>
<tr id="row258119462811"><td class="cellrowborder" valign="top" width="20.7%" headers="mcps1.2.5.1.1 "><p id="p757613460812"><a name="p757613460812"></a><a name="p757613460812"></a>int8_t</p></td>
<td class="cellrowborder" valign="top" width="18.96%" headers="mcps1.2.5.1.2 "><p id="p157644615814"><a name="p157644615814"></a><a name="p157644615814"></a>int8_t</p></td>
<td class="cellrowborder" valign="top" width="16.71%" headers="mcps1.2.5.1.3 "><p id="p1767833425"><a name="p1767833425"></a><a name="p1767833425"></a>bfloat16_t</p></td>
<td class="cellrowborder" valign="top" width="43.63%" headers="mcps1.2.5.1.4 "><p id="p1666113614229"><a name="p1666113614229"></a><a name="p1666113614229"></a><span id="ph95771046482"><a name="ph95771046482"></a><a name="ph95771046482"></a>Ascend 950PR/Ascend 950DT</span></p></td>
</tr>
<tr id="row109291299205"><td class="cellrowborder" valign="top" width="20.7%" headers="mcps1.2.5.1.1 "><p id="p1835141611538"><a name="p1835141611538"></a><a name="p1835141611538"></a>fp8_e4m3fn_t/fp8_e5m2_t</p></td>
<td class="cellrowborder" valign="top" width="18.96%" headers="mcps1.2.5.1.2 "><p id="p10329241249"><a name="p10329241249"></a><a name="p10329241249"></a>fp8_e4m3fn_t/fp8_e5m2_t</p></td>
<td class="cellrowborder" valign="top" width="16.71%" headers="mcps1.2.5.1.3 "><p id="p966315574422"><a name="p966315574422"></a><a name="p966315574422"></a>fp8_e4m3fn_t/half/bfloat16_t/float</p></td>
<td class="cellrowborder" valign="top" width="43.63%" headers="mcps1.2.5.1.4 "><p id="p119371738172216"><a name="p119371738172216"></a><a name="p119371738172216"></a><span id="ph192515299207"><a name="ph192515299207"></a><a name="ph192515299207"></a>Ascend 950PR/Ascend 950DT</span></p></td>
</tr>
<tr id="row2899181383117"><td class="cellrowborder" valign="top" width="20.7%" headers="mcps1.2.5.1.1 "><p id="p1769419324338"><a name="p1769419324338"></a><a name="p1769419324338"></a>hifloat8_t</p></td>
<td class="cellrowborder" valign="top" width="18.96%" headers="mcps1.2.5.1.2 "><p id="p669483217333"><a name="p669483217333"></a><a name="p669483217333"></a>hifloat8_t</p></td>
<td class="cellrowborder" valign="top" width="16.71%" headers="mcps1.2.5.1.3 "><p id="p9694232183318"><a name="p9694232183318"></a><a name="p9694232183318"></a>hifloat8_t/half/bfloat16_t/float</p></td>
<td class="cellrowborder" valign="top" width="43.63%" headers="mcps1.2.5.1.4 "><p id="p1048164352210"><a name="p1048164352210"></a><a name="p1048164352210"></a><span id="ph5896181318319"><a name="ph5896181318319"></a><a name="ph5896181318319"></a>Ascend 950PR/Ascend 950DT</span></p>
<p id="p16562223181910"><a name="p16562223181910"></a><a name="p16562223181910"></a>Note:</p>
<p id="p144994718228"><a name="p144994718228"></a><a name="p144994718228"></a>When the output is hifloat8_t, the Half to Away Round method is used for quantization.</p>
<p id="p2661551202211"><a name="p2661551202211"></a><a name="p2661551202211"></a>When the output of a quantization scenario is of the float type, this quantization mode cannot achieve an accuracy of 2 in 10,000, but can achieve 2 in 1,000. If an accuracy of 2 in 10,000 is required, use the AscendDeQuant high-order API.</p></td>
</tr>
</tbody>
</table>

## Constraints<a name="zh-cn_topic_0000002298767893_section14160134220363"></a>

-   The [SetQuantScalar](../../../../../api/SIMD-API/advanced_api/matrix_compute/Matmul-Kernel/SetQuantScalar.md) and [SetQuantVector](../../../../../api/SIMD-API/advanced_api/matrix_compute/Matmul-Kernel/SetQuantVector.md) APIs must be called before the [Iterate](../../../../../api/SIMD-API/advanced_api/matrix_compute/Matmul-Kernel/Iterate.md) or [IteratAll](../../../../../api/SIMD-API/advanced_api/matrix_compute/Matmul-Kernel/IterateAll.md) API.

-   The quantization/dequantization modes set on the Kernel side and the Tiling side must be consistent:
    -   On the Kernel side, call the SetQuantScalar API to set the quantization/dequantization mode with the same coefficient, and on the Tiling side, call the SetDequantType API to configure the mode as DequantType::SCALAR.
    -   On the Kernel side, call the SetQuantVector API to set the vector quantization/dequantization mode, and on the Tiling side, call the SetDequantType API to configure the mode as DequantType::TENSOR.

-   When matrices A and B are of the int8\_t or int4b\_t type and matrix C is of the half type, the output result of this feature does not support the INF\_NAN mode. If the result needs to be output in the INF\_NAN mode, it is recommended that you output the result to TPosition::VECIN when calling the Matmul API, set the output data type to int32\_t, and then use the high-order API [AscendDequant](../../../../../api/SIMD-API/advanced_api/quantization_operations/AscendDequant.md) based on the AIV core to dequantize the result to the half type.

## Calling Example<a name="zh-cn_topic_0000002298767893_section15486294368"></a>

For the complete operator sample, see [matmul_quant sample](https://gitcode.com/cann/asc-devkit/tree/9.1.0/examples/01_simd_cpp_api/04_advanced_api/00_matmul/matmul_quant).

-   Tiling Implementation

    Call the [SetDequantType](../../../../../api/SIMD-API/high-order API/Cube computation/Matmul-Tiling class/SetDequantType.md) API to set the quantization or dequantization mode. Other implementation details are the same as those in the basic scenario.

    ```
    auto ascendcPlatform = platform_ascendc::PlatformAscendC(context->GetPlatformInfo());
    matmul_tiling::MatmulApiTiling tiling(ascendcPlatform); 
    tiling.SetAType(matmul_tiling::TPosition::GM, matmul_tiling::CubeFormat::ND, matmul_tiling::DataType::DT_INT8);
    tiling.SetBType(matmul_tiling::TPosition::GM, matmul_tiling::CubeFormat::ND, matmul_tiling::DataType::DT_INT8);   
    tiling.SetCType(matmul_tiling::TPosition::GM, matmul_tiling::CubeFormat::ND, matmul_tiling::DataType::DT_FLOAT16);   
    tiling.SetBiasType(matmul_tiling::TPosition::GM, matmul_tiling::CubeFormat::ND, matmul_tiling::DataType::DT_INT32);   
    tiling.SetShape(M, N, K);   
    tiling.SetOrgShape(M, N, K);  
    tiling.EnableBias(true);
    tiling.SetDequantType(DequantType::SCALAR);  // Set the same-coefficient quantization/dequantization mode.
    // tiling.SetDequantType(DequantType::TENSOR);  // Set the vector quantization/dequantization mode.
    ... // Perform other configurations.
    ```

-   Kernel Implementation

    Based on the specific quantization mode scenario, call the [SetQuantScalar](../../../../../api/SIMD-API/high-order API/Cube computation/Matmul-Kernel side interface/SetQuantScalar.md) or [SetQuantVector](../../../../../api/SIMD-API/high-order API/Cube computation/Matmul-Kernel side interface/SetQuantVector.md) API to set the quantization parameters. Other implementation details are the same as those in the basic scenario.

    -   Same-coefficient quantization/dequantization mode

        ```
        REGIST_MATMUL_OBJ(&pipe, GetSysWorkSpacePtr(), mm, &tiling);
        float tmp = 0.1;  // The output is multiplied by 0.1 when written to GM.
        uint64_t ans = static_cast<uint64_t>(*reinterpret_cast<int32_t*>(&tmp)); // Convert the floating-point quantization coefficient to uint64_t for setting.
        mm.SetQuantScalar(ans);
        mm.SetTensorA(gm_a);
        mm.SetTensorB(gm_b);
        mm.SetBias(gm_bias);
        mm.IterateAll(gm_c);
        ```

    -   Vector quantization/dequantization mode

        ```
        GlobalTensor gmQuant;
        ...
        REGIST_MATMUL_OBJ(&pipe, GetSysWorkSpacePtr(), mm, &tiling);
        mm.SetQuantVector(gmQuant);
        mm.SetTensorA(gm_a);
        mm.SetTensorB(gm_b);
        mm.SetBias(gm_bias);
        mm.IterateAll(gm_c);
        ```
