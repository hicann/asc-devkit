# MxMatmul Scenario<a name="ZH-CN_TOPIC_0000002500468246"></a>

<!-- md-trans-meta sourceCommit=e3c8c70b3f5d27f3770063ae4b535bc75d904e3e translatedAt=2026-08-26T13:45:16.566Z -->

## Background<a name="zh-cn_topic_0000002270097206_section182549454374"></a>

Floating-point numbers are widely used in scientific computing, image processing, neural networks, and other fields. Taking AI training as an example, existing floating-point formats either have an insufficient value range or low precision, which affects the convergence speed and performance of models. Meeting both the value range and precision requirements at the same time would lead to excessive memory usage, thereby increasing the cost of data storage and transmission. To address this, the industry has proposed a new floating-point format, the Microscaling (MX) format. Floating-point numbers in the MX format can support AI training and inference at lower bit widths while occupying less memory. Data formats that comply with the MX standard can achieve robust AI training and inference model precision when using 8-bit or lower bit widths.

The MX format is a block data format. Several data elements can form a block (or a group), and data is organized in units of blocks. Data in the MX format consists of three parts:

-   A shared scaling factor X with a bit width of w bits;
-   Private elements P<sub>i</sub> with a bit width of d bits;
-   A block size k, which indicates how many low-bit data elements form a block;

All k elements P<sub>i</sub> have the same bit width and data type, and share one scaling factor X. Each block containing k elements can be encoded using (w+k*d) bits. The data type of the elements and the scaling factor can be selected independently.

The following figure shows the data structure of floating-point numbers in the MX format. S, E, and M represent the sign, exponent, and mantissa field values of a floating-point number, respectively. The shared scaling factor X is a scaling factor used for the entire data block, and it determines the dynamic range of all elements in the data block. By introducing a shared scaling factor, data in the MX format can flexibly represent data in different ranges while maintaining a low bit width. The block size k refers to the number of low-bit data elements that form a data block (or group). Private elements P<sub>i</sub> refer to each low-bit data element in the data block. After being adjusted by the scaling factor X, these elements jointly represent a high-precision floating-point number or integer.

**Figure 1**  MX format composition schematic diagram<a name="zh-cn_topic_0000002270097206_fig11888194743220"></a>  
![](../../../../figures/mx_format_composition_diagram.png "MX format composition schematic diagram")

The MX format includes multiple data types, such as MXFP8, MXFP4, MXFP16, and MXINT4. The following table lists the data types supported by the [MxMatmul scenario](#zh-cn_topic_0000002270097206_section310824820358) (Microscaling Matmul).

**Table 1**  Data types supported by MxMatmul in MX format

<a name="zh-cn_topic_0000002270097206_table5383144710452"></a>
<table><thead align="left"><tr id="zh-cn_topic_0000002270097206_row193837473457"><th class="cellrowborder" valign="top" width="16.666666666666664%" id="mcps1.2.7.1.1"><p id="zh-cn_topic_0000002270097206_p410517278313"><a name="zh-cn_topic_0000002270097206_p410517278313"></a><a name="zh-cn_topic_0000002270097206_p410517278313"></a>Data Type</p></th>
<th class="cellrowborder" valign="top" width="16.666666666666664%" id="mcps1.2.7.1.2"><p id="zh-cn_topic_0000002270097206_p12383184711457"><a name="zh-cn_topic_0000002270097206_p12383184711457"></a><a name="zh-cn_topic_0000002270097206_p12383184711457"></a>Private Element Data Type</p></th>
<th class="cellrowborder" valign="top" width="16.666666666666664%" id="mcps1.2.7.1.3"><p id="zh-cn_topic_0000002270097206_p2383647184516"><a name="zh-cn_topic_0000002270097206_p2383647184516"></a><a name="zh-cn_topic_0000002270097206_p2383647184516"></a>Private Element Bit Width (d)</p></th>
<th class="cellrowborder" valign="top" width="16.666666666666664%" id="mcps1.2.7.1.4"><p id="zh-cn_topic_0000002270097206_p16383847154510"><a name="zh-cn_topic_0000002270097206_p16383847154510"></a><a name="zh-cn_topic_0000002270097206_p16383847154510"></a>Block Size (k)</p></th>
<th class="cellrowborder" valign="top" width="16.666666666666664%" id="mcps1.2.7.1.5"><p id="zh-cn_topic_0000002270097206_p1238334784520"><a name="zh-cn_topic_0000002270097206_p1238334784520"></a><a name="zh-cn_topic_0000002270097206_p1238334784520"></a>Shared Scaling Factor Data Type</p></th>
<th class="cellrowborder" valign="top" width="16.666666666666664%" id="mcps1.2.7.1.6"><p id="zh-cn_topic_0000002270097206_p113831847114514"><a name="zh-cn_topic_0000002270097206_p113831847114514"></a><a name="zh-cn_topic_0000002270097206_p113831847114514"></a>Shared Scaling Factor Bit Width (w)</p></th>
</tr>
</thead>
<tbody><tr id="zh-cn_topic_0000002270097206_row238320470455"><td class="cellrowborder" valign="top" width="16.666666666666664%" headers="mcps1.2.7.1.1 "><p id="zh-cn_topic_0000002270097206_p9383134734518"><a name="zh-cn_topic_0000002270097206_p9383134734518"></a><a name="zh-cn_topic_0000002270097206_p9383134734518"></a>MXFP8</p></td>
<td class="cellrowborder" valign="top" width="16.666666666666664%" headers="mcps1.2.7.1.2 "><p id="zh-cn_topic_0000002270097206_p11383447164519"><a name="zh-cn_topic_0000002270097206_p11383447164519"></a><a name="zh-cn_topic_0000002270097206_p11383447164519"></a>fp8_e5m2_t</p></td>
<td class="cellrowborder" valign="top" width="16.666666666666664%" headers="mcps1.2.7.1.3 "><p id="zh-cn_topic_0000002270097206_p33831947194516"><a name="zh-cn_topic_0000002270097206_p33831947194516"></a><a name="zh-cn_topic_0000002270097206_p33831947194516"></a>8</p></td>
<td class="cellrowborder" valign="top" width="16.666666666666664%" headers="mcps1.2.7.1.4 "><p id="zh-cn_topic_0000002270097206_p1338311471456"><a name="zh-cn_topic_0000002270097206_p1338311471456"></a><a name="zh-cn_topic_0000002270097206_p1338311471456"></a>32</p></td>
<td class="cellrowborder" valign="top" width="16.666666666666664%" headers="mcps1.2.7.1.5 "><p id="zh-cn_topic_0000002270097206_p1661204742410"><a name="zh-cn_topic_0000002270097206_p1661204742410"></a><a name="zh-cn_topic_0000002270097206_p1661204742410"></a>fp8_e8m0_t</p></td>
<td class="cellrowborder" valign="top" width="16.666666666666664%" headers="mcps1.2.7.1.6 "><p id="zh-cn_topic_0000002270097206_p149798459481"><a name="zh-cn_topic_0000002270097206_p149798459481"></a><a name="zh-cn_topic_0000002270097206_p149798459481"></a>8</p></td>
</tr>
<tr id="zh-cn_topic_0000002270097206_row133831647204514"><td class="cellrowborder" valign="top" width="16.666666666666664%" headers="mcps1.2.7.1.1 "><p id="p448511510259"><a name="p448511510259"></a><a name="p448511510259"></a>MXFP8</p></td>
<td class="cellrowborder" valign="top" width="16.666666666666664%" headers="mcps1.2.7.1.2 "><p id="zh-cn_topic_0000002270097206_p09723273916"><a name="zh-cn_topic_0000002270097206_p09723273916"></a><a name="zh-cn_topic_0000002270097206_p09723273916"></a>fp8_e4m3fn_t</p></td>
<td class="cellrowborder" valign="top" width="16.666666666666664%" headers="mcps1.2.7.1.3 "><p id="p175322662417"><a name="p175322662417"></a><a name="p175322662417"></a>8</p></td>
<td class="cellrowborder" valign="top" width="16.666666666666664%" headers="mcps1.2.7.1.4 "><p id="p11711191614244"><a name="p11711191614244"></a><a name="p11711191614244"></a>32</p></td>
<td class="cellrowborder" valign="top" width="16.666666666666664%" headers="mcps1.2.7.1.5 "><p id="p75995345248"><a name="p75995345248"></a><a name="p75995345248"></a>fp8_e8m0_t</p></td>
<td class="cellrowborder" valign="top" width="16.666666666666664%" headers="mcps1.2.7.1.6 "><p id="p11842154562416"><a name="p11842154562416"></a><a name="p11842154562416"></a>8</p></td>
</tr>
<tr id="zh-cn_topic_0000002270097206_row1138444719456"><td class="cellrowborder" valign="top" width="16.666666666666664%" headers="mcps1.2.7.1.1 "><p id="zh-cn_topic_0000002270097206_p173841478457"><a name="zh-cn_topic_0000002270097206_p173841478457"></a><a name="zh-cn_topic_0000002270097206_p173841478457"></a>MXFP4</p></td>
<td class="cellrowborder" valign="top" width="16.666666666666664%" headers="mcps1.2.7.1.2 "><p id="zh-cn_topic_0000002270097206_p158615210394"><a name="zh-cn_topic_0000002270097206_p158615210394"></a><a name="zh-cn_topic_0000002270097206_p158615210394"></a>fp4x2_e1m2_t</p></td>
<td class="cellrowborder" valign="top" width="16.666666666666664%" headers="mcps1.2.7.1.3 "><p id="zh-cn_topic_0000002270097206_p238444704512"><a name="zh-cn_topic_0000002270097206_p238444704512"></a><a name="zh-cn_topic_0000002270097206_p238444704512"></a>4</p></td>
<td class="cellrowborder" valign="top" width="16.666666666666664%" headers="mcps1.2.7.1.4 "><p id="zh-cn_topic_0000002270097206_p2038434716454"><a name="zh-cn_topic_0000002270097206_p2038434716454"></a><a name="zh-cn_topic_0000002270097206_p2038434716454"></a>32</p></td>
<td class="cellrowborder" valign="top" width="16.666666666666664%" headers="mcps1.2.7.1.5 "><p id="p205991534102413"><a name="p205991534102413"></a><a name="p205991534102413"></a>fp8_e8m0_t</p></td>
<td class="cellrowborder" valign="top" width="16.666666666666664%" headers="mcps1.2.7.1.6 "><p id="p1842145182418"><a name="p1842145182418"></a><a name="p1842145182418"></a>8</p></td>
</tr>
<tr id="zh-cn_topic_0000002270097206_row12384154710452"><td class="cellrowborder" valign="top" width="16.666666666666664%" headers="mcps1.2.7.1.1 "><p id="p10647179102513"><a name="p10647179102513"></a><a name="p10647179102513"></a>MXFP4</p></td>
<td class="cellrowborder" valign="top" width="16.666666666666664%" headers="mcps1.2.7.1.2 "><p id="zh-cn_topic_0000002270097206_p8952103819241"><a name="zh-cn_topic_0000002270097206_p8952103819241"></a><a name="zh-cn_topic_0000002270097206_p8952103819241"></a>fp4x2_e2m1_t</p></td>
<td class="cellrowborder" valign="top" width="16.666666666666664%" headers="mcps1.2.7.1.3 "><p id="p69001011132410"><a name="p69001011132410"></a><a name="p69001011132410"></a>4</p></td>
<td class="cellrowborder" valign="top" width="16.666666666666664%" headers="mcps1.2.7.1.4 "><p id="p250011195246"><a name="p250011195246"></a><a name="p250011195246"></a>32</p></td>
<td class="cellrowborder" valign="top" width="16.666666666666664%" headers="mcps1.2.7.1.5 "><p id="p125992348249"><a name="p125992348249"></a><a name="p125992348249"></a>fp8_e8m0_t</p></td>
<td class="cellrowborder" valign="top" width="16.666666666666664%" headers="mcps1.2.7.1.6 "><p id="p284212453244"><a name="p284212453244"></a><a name="p284212453244"></a>8</p></td>
</tr>
</tbody>
</table>

## Description<a name="zh-cn_topic_0000002270097206_section310824820358"></a>

MxMatmul (Microscaling Matmul) is matrix multiplication with quantization coefficients. That is, both the left matrix and the right matrix have corresponding quantization coefficient matrices: the left quantization coefficient matrix scaleA and the right quantization coefficient matrix scaleB. In the MxMatmul scenario, the left quantization coefficient matrix is multiplied by the left matrix, the right quantization coefficient matrix is multiplied by the right matrix, and matrix multiplication is performed on the results of the two products.

The formula of MxMatmul is: C = \(scaleA ⊗ A\) \* \(scaleB ⊗ B\) + Bias, where "⊗" indicates broadcast multiplication. When the left/right matrix is multiplied by the left/right quantization coefficient matrix, every 32 elements in the K direction share one quantization factor, as shown in [Figure 2](#zh-cn_topic_0000002270097206_fig1942919398330).

-   A, scaleA, B, and scaleB are source operands. A is the left matrix with the shape [M, K]; scaleA is the left quantization coefficient matrix with the shape [M, K/32]; B is the right matrix with the shape [K, N]; scaleB is the right quantization coefficient matrix with the shape [K/32, N].
-   C is the destination operand, which is the matrix storing the matrix multiplication result and has the shape [M, N].
-   Bias is the matrix multiplication bias with the shape [1, N]. This Bias is applied to each row of the result matrix of \(scaleA ⊗ A\) \* \(scaleB ⊗ B\).

**Figure 2**  MxMatmul matrix multiplication schematic diagram<a name="zh-cn_topic_0000002270097206_fig1942919398330"></a>  
![](../../../../figures/mxmatmul_matrix_multiplication_diagram.png "MxMatmul matrix multiplication schematic diagram")

The layout formats of matrices A, scaleA, B, and scaleB at different positions are shown in the following figures.

**Figure 3**  Layout format of matrix A at different positions<a name="zh-cn_topic_0000002270097206_fig384531413014"></a>  
![](../../../../figures/layout_format_of_matrix_a_at_different_positions.png "Layout format of matrix A at different positions")

**Figure 4**  Layout format of matrix B at different positions<a name="zh-cn_topic_0000002270097206_fig674419451902"></a>  
![](../../../../figures/layout_format_of_matrix_b_at_different_positions.png "Layout format of matrix B at different positions")

**Figure 5** Layout format of the scaleA matrix at different positions<a name="zh-cn_topic_0000002270097206_fig107863142019"></a>  
![](../../../../figures/layout_format_of_matrix_scalea_at_different_positions.png "Layout format of the scaleA matrix at different positions")

**Figure 6** Layout format of the scaleB matrix at different positions<a name="zh-cn_topic_0000002270097206_fig76682054103416"></a>  
![](../../../../figures/layout_format_of_matrix_scaleb_at_different_positions.png "Layout format of the scaleB matrix at different positions")

## Scenarios<a name="zh-cn_topic_0000002270097206_section118051016163613"></a>

A scenario where matrices A and B need to be quantized before Cube computation. In this scenario, the data types supported by the Matmul input and output matrices are shown in the following table.

**Table 2** Quantization scenarios supported by MxMatmul

<a name="zh-cn_topic_0000002270097206_table2844113219191"></a>
<table><thead align="left"><tr id="zh-cn_topic_0000002270097206_row4844732121915"><th class="cellrowborder" valign="top" width="10.41%" id="mcps1.2.7.1.1"><p id="zh-cn_topic_0000002270097206_p1084417322199"><a name="zh-cn_topic_0000002270097206_p1084417322199"></a><a name="zh-cn_topic_0000002270097206_p1084417322199"></a>A Matrix</p></th>
<th class="cellrowborder" valign="top" width="20.599999999999998%" id="mcps1.2.7.1.2"><p id="zh-cn_topic_0000002270097206_p178441532121916"><a name="zh-cn_topic_0000002270097206_p178441532121916"></a><a name="zh-cn_topic_0000002270097206_p178441532121916"></a>B Matrix</p></th>
<th class="cellrowborder" valign="top" width="15.340000000000002%" id="mcps1.2.7.1.3"><p id="zh-cn_topic_0000002270097206_p9844203201914"><a name="zh-cn_topic_0000002270097206_p9844203201914"></a><a name="zh-cn_topic_0000002270097206_p9844203201914"></a>ScaleA Matrix/ScaleB Matrix</p></th>
<th class="cellrowborder" valign="top" width="18.85%" id="mcps1.2.7.1.4"><p id="zh-cn_topic_0000002270097206_p6532145910546"><a name="zh-cn_topic_0000002270097206_p6532145910546"></a><a name="zh-cn_topic_0000002270097206_p6532145910546"></a>Bias Matrix</p></th>
<th class="cellrowborder" valign="top" width="17.72%" id="mcps1.2.7.1.5"><p id="zh-cn_topic_0000002270097206_p131201522142011"><a name="zh-cn_topic_0000002270097206_p131201522142011"></a><a name="zh-cn_topic_0000002270097206_p131201522142011"></a>C Matrix</p></th>
<th class="cellrowborder" valign="top" width="17.080000000000002%" id="mcps1.2.7.1.6"><p id="zh-cn_topic_0000002270097206_p1012052214207"><a name="zh-cn_topic_0000002270097206_p1012052214207"></a><a name="zh-cn_topic_0000002270097206_p1012052214207"></a>Supported Platform</p></th>
</tr>
</thead>
<tbody><tr id="zh-cn_topic_0000002270097206_row12844932171910"><td class="cellrowborder" valign="top" width="10.41%" headers="mcps1.2.7.1.1 "><p id="zh-cn_topic_0000002270097206_p7777112717207"><a name="zh-cn_topic_0000002270097206_p7777112717207"></a><a name="zh-cn_topic_0000002270097206_p7777112717207"></a>fp4x2_e1m2_t</p></td>
<td class="cellrowborder" valign="top" width="20.599999999999998%" headers="mcps1.2.7.1.2 "><p id="zh-cn_topic_0000002270097206_p57771127152013"><a name="zh-cn_topic_0000002270097206_p57771127152013"></a><a name="zh-cn_topic_0000002270097206_p57771127152013"></a>fp4x2_e1m2_t/fp4x2_e2m1_t</p></td>
<td class="cellrowborder" valign="top" width="15.340000000000002%" headers="mcps1.2.7.1.3 "><p id="zh-cn_topic_0000002270097206_p1377792713202"><a name="zh-cn_topic_0000002270097206_p1377792713202"></a><a name="zh-cn_topic_0000002270097206_p1377792713202"></a>fp8_e8m0_t</p></td>
<td class="cellrowborder" valign="top" width="18.85%" headers="mcps1.2.7.1.4 "><p id="zh-cn_topic_0000002270097206_p4532125918544"><a name="zh-cn_topic_0000002270097206_p4532125918544"></a><a name="zh-cn_topic_0000002270097206_p4532125918544"></a>float/half/bfloat16_t</p></td>
<td class="cellrowborder" valign="top" width="17.72%" headers="mcps1.2.7.1.5 "><p id="zh-cn_topic_0000002270097206_p6777827162018"><a name="zh-cn_topic_0000002270097206_p6777827162018"></a><a name="zh-cn_topic_0000002270097206_p6777827162018"></a>float/half/bfloat16_t</p></td>
<td class="cellrowborder" valign="top" width="17.080000000000002%" headers="mcps1.2.7.1.6 "><p id="p355413382268"><a name="p355413382268"></a><a name="p355413382268"></a><span id="zh-cn_topic_0000002270097206_ph3777122762015"><a name="zh-cn_topic_0000002270097206_ph3777122762015"></a><a name="zh-cn_topic_0000002270097206_ph3777122762015"></a>Ascend 950PR/Ascend 950DT</span></p></td>
</tr>
<tr id="zh-cn_topic_0000002270097206_row11845153220193"><td class="cellrowborder" valign="top" width="10.41%" headers="mcps1.2.7.1.1 "><p id="zh-cn_topic_0000002270097206_p377742710204"><a name="zh-cn_topic_0000002270097206_p377742710204"></a><a name="zh-cn_topic_0000002270097206_p377742710204"></a>fp4x2_e2m1_t</p></td>
<td class="cellrowborder" valign="top" width="20.599999999999998%" headers="mcps1.2.7.1.2 "><p id="zh-cn_topic_0000002270097206_p177771427132012"><a name="zh-cn_topic_0000002270097206_p177771427132012"></a><a name="zh-cn_topic_0000002270097206_p177771427132012"></a>fp4x2_e2m1_t/fp4x2_e1m2_t</p></td>
<td class="cellrowborder" valign="top" width="15.340000000000002%" headers="mcps1.2.7.1.3 "><p id="zh-cn_topic_0000002270097206_p17777162715206"><a name="zh-cn_topic_0000002270097206_p17777162715206"></a><a name="zh-cn_topic_0000002270097206_p17777162715206"></a>fp8_e8m0_t</p></td>
<td class="cellrowborder" valign="top" width="18.85%" headers="mcps1.2.7.1.4 "><p id="zh-cn_topic_0000002270097206_p853295911547"><a name="zh-cn_topic_0000002270097206_p853295911547"></a><a name="zh-cn_topic_0000002270097206_p853295911547"></a>float/half/bfloat16_t</p></td>
<td class="cellrowborder" valign="top" width="17.72%" headers="mcps1.2.7.1.5 "><p id="zh-cn_topic_0000002270097206_p57779278201"><a name="zh-cn_topic_0000002270097206_p57779278201"></a><a name="zh-cn_topic_0000002270097206_p57779278201"></a>float/half/bfloat16_t</p></td>
<td class="cellrowborder" valign="top" width="17.080000000000002%" headers="mcps1.2.7.1.6 "><p id="p15680154020265"><a name="p15680154020265"></a><a name="p15680154020265"></a><span id="zh-cn_topic_0000002270097206_ph1677719278204"><a name="zh-cn_topic_0000002270097206_ph1677719278204"></a><a name="zh-cn_topic_0000002270097206_ph1677719278204"></a>Ascend 950PR/Ascend 950DT</span></p></td>
</tr>
<tr id="zh-cn_topic_0000002270097206_row0845173241915"><td class="cellrowborder" valign="top" width="10.41%" headers="mcps1.2.7.1.1 "><p id="zh-cn_topic_0000002270097206_p1677712716209"><a name="zh-cn_topic_0000002270097206_p1677712716209"></a><a name="zh-cn_topic_0000002270097206_p1677712716209"></a>fp8_e4m3fn_t</p></td>
<td class="cellrowborder" valign="top" width="20.599999999999998%" headers="mcps1.2.7.1.2 "><p id="zh-cn_topic_0000002270097206_p87771927182015"><a name="zh-cn_topic_0000002270097206_p87771927182015"></a><a name="zh-cn_topic_0000002270097206_p87771927182015"></a>fp8_e4m3fn_t/fp8_e5m2_t</p></td>
<td class="cellrowborder" valign="top" width="15.340000000000002%" headers="mcps1.2.7.1.3 "><p id="zh-cn_topic_0000002270097206_p1577832742016"><a name="zh-cn_topic_0000002270097206_p1577832742016"></a><a name="zh-cn_topic_0000002270097206_p1577832742016"></a>fp8_e8m0_t</p></td>
<td class="cellrowborder" valign="top" width="18.85%" headers="mcps1.2.7.1.4 "><p id="zh-cn_topic_0000002270097206_p6532125914546"><a name="zh-cn_topic_0000002270097206_p6532125914546"></a><a name="zh-cn_topic_0000002270097206_p6532125914546"></a>float/half/bfloat16_t</p></td>
<td class="cellrowborder" valign="top" width="17.72%" headers="mcps1.2.7.1.5 "><p id="zh-cn_topic_0000002270097206_p37781627172014"><a name="zh-cn_topic_0000002270097206_p37781627172014"></a><a name="zh-cn_topic_0000002270097206_p37781627172014"></a>float/half/bfloat16_t</p></td>
<td class="cellrowborder" valign="top" width="17.080000000000002%" headers="mcps1.2.7.1.6 "><p id="p16938423262"><a name="p16938423262"></a><a name="p16938423262"></a><span id="zh-cn_topic_0000002270097206_ph77781527182016"><a name="zh-cn_topic_0000002270097206_ph77781527182016"></a><a name="zh-cn_topic_0000002270097206_ph77781527182016"></a>Ascend 950PR/Ascend 950DT</span></p></td>
</tr>
<tr id="zh-cn_topic_0000002270097206_row178451932101918"><td class="cellrowborder" valign="top" width="10.41%" headers="mcps1.2.7.1.1 "><p id="zh-cn_topic_0000002270097206_p3778172722012"><a name="zh-cn_topic_0000002270097206_p3778172722012"></a><a name="zh-cn_topic_0000002270097206_p3778172722012"></a>fp8_e5m2_t</p></td>
<td class="cellrowborder" valign="top" width="20.599999999999998%" headers="mcps1.2.7.1.2 "><p id="zh-cn_topic_0000002270097206_p17778727112016"><a name="zh-cn_topic_0000002270097206_p17778727112016"></a><a name="zh-cn_topic_0000002270097206_p17778727112016"></a>fp8_e4m3fn_t/fp8_e5m2_t</p></td>
<td class="cellrowborder" valign="top" width="15.340000000000002%" headers="mcps1.2.7.1.3 "><p id="zh-cn_topic_0000002270097206_p6778627122015"><a name="zh-cn_topic_0000002270097206_p6778627122015"></a><a name="zh-cn_topic_0000002270097206_p6778627122015"></a>fp8_e8m0_t</p></td>
<td class="cellrowborder" valign="top" width="18.85%" headers="mcps1.2.7.1.4 "><p id="zh-cn_topic_0000002270097206_p7532145915540"><a name="zh-cn_topic_0000002270097206_p7532145915540"></a><a name="zh-cn_topic_0000002270097206_p7532145915540"></a>float/half/bfloat16_t</p></td>
<td class="cellrowborder" valign="top" width="17.72%" headers="mcps1.2.7.1.5 "><p id="zh-cn_topic_0000002270097206_p18778102710208"><a name="zh-cn_topic_0000002270097206_p18778102710208"></a><a name="zh-cn_topic_0000002270097206_p18778102710208"></a>float/half/bfloat16_t</p></td>
<td class="cellrowborder" valign="top" width="17.080000000000002%" headers="mcps1.2.7.1.6 "><p id="p1366354422615"><a name="p1366354422615"></a><a name="p1366354422615"></a><span id="zh-cn_topic_0000002270097206_ph1877832711205"><a name="zh-cn_topic_0000002270097206_ph1877832711205"></a><a name="zh-cn_topic_0000002270097206_ph1877832711205"></a>Ascend 950PR/Ascend 950DT</span></p></td>
</tr>
</tbody>
</table>

## Implementation Process<a name="zh-cn_topic_0000002270097206_section17128015184410"></a>

The key steps for the host side to automatically obtain Tiling parameters are described as follows:

1.  **Create a Tiling object.**

    ```
    auto ascendcPlatform = platform_ascendc::PlatformAscendC(context->GetPlatformInfo());
    matmul_tiling::MatmulApiTiling cubeTiling(ascendcPlatform); 
    ```

    Pass the hardware platform information to create a PlatformAscendC object, and then create a Tiling object. The hardware platform information can be obtained through GetPlatformInfo.

2.  **Set the memory logical positions, formats, data types, and transpose information of A, B, C, and Bias, and set the memory logical positions, formats, and transpose information of scaleA and scaleB.**

    Call the [SetScaleAType](../../../../../api/SIMD-API/advanced_api/matrix_compute/Matmul-Tiling/SetScaleAType.md) and [SetScaleBType](../../../../../api/SIMD-API/advanced_api/matrix_compute/Matmul-Tiling/SetScaleBType.md) APIs to set the memory logical positions, formats, and transpose information of scaleA and scaleB.

    ```
    cubeTiling.SetAType(AscendC::TPosition::GM, CubeFormat::ND, matmul_tiling::DataType::DT_FLOAT8_E5M2, false);
    cubeTiling.SetBType(AscendC::TPosition::GM, CubeFormat::ND, matmul_tiling::DataType::DT_FLOAT8_E5M2, true);
    cubeTiling.SetScaleAType(AscendC::TPosition::GM, CubeFormat::ND, false);
    cubeTiling.SetScaleBType(AscendC::TPosition::GM, CubeFormat::ND, true);
    cubeTiling.SetCType(AscendC::TPosition::GM, CubeFormat::ND, matmul_tiling::DataType::DT_FLOAT);
    cubeTiling.SetBiasType(AscendC::TPosition::GM, CubeFormat::ND, matmul_tiling::DataType::DT_FLOAT);
    ```

3.  **Set the MxMatmul scenario.**

    Call the [SetMadType](../../../../../api/SIMD-API/advanced_api/matrix_compute/Matmul-Tiling/SetMadType.md) API to set the Tiling computation logic to the MxMatmul scenario.

    ```
    cubetiling.SetMadType(MatrixMadType::MXMODE);
    ```

4.  **Set the matrix shape information.**

    ```
    cubeTiling.SetShape(M, N, K);
    cubeTiling.SetOrgShape(M, N, K); // Set the original complete shapes M, N, and K.
    ```

5.  **Set the available space size information.**

    Set the L1 Buffer/L0C Buffer/Unified Buffer space size available for Matmul computation. -1 indicates the size of the corresponding Buffer on the AI processor.

    ```
    cubeTiling.SetBufferSpace(-1, -1, -1);
    ```

6.  **Set other parameters as needed, for example, set bias to participate in the computation.**

    ```
    cubeTiling.EnableBias(true);
    ```

7.  **Obtain the Tiling parameters.**

    ```
    MatmulCustomTilingData tiling;
    if (cubeTiling.GetTiling(tiling.cubeTilingData) == -1){ 
        return ge::GRAPH_FAILED;  
    }
    ```

8.  Perform other operations such as serialization and saving of the Tiling parameters.

The key steps on the Kernel side are described as follows:

1.  **Create a Matmul object.**

    ```
    // In the MxMatmul scenario, MatmulTypeWithScale is used to define the parameter type information of A, scaleA, B, and scaleB.
    typedef AscendC::MatmulTypeWithScale<AscendC::TPosition::GM, AscendC::TPosition::GM, CubeFormat::ND, fp8_e5m2_t, isTransposeA> aType; 
    typedef AscendC::MatmulTypeWithScale<AscendC::TPosition::GM, AscendC::TPosition::GM, CubeFormat::ND, fp8_e5m2_t, isTransposeB> bType;
    typedef AscendC::MatmulType<AscendC::TPosition::GM, CubeFormat::ND, float> cType; 
    typedef AscendC::MatmulType<AscendC::TPosition::GM, CubeFormat::ND, float> biasType; 
    // When defining the matmul object, pass MatmulWithScalePolicy to indicate that the MxMatmul template policy is enabled.
    AscendC::Matmul<aType, bType, cType, biasType, CFG_MDL, MatmulCallBackFunc<nullptr, nullptr, nullptr>, AscendC::Impl::Detail::MatmulWithScalePolicy> mm; 
    ```

    When creating the object, you need to pass in the parameter type information of A, scaleA, B, scaleB, C, and Bias. The type information of A, scaleA, B, and scaleB is defined through [MatmulTypeWithScale](#zh-cn_topic_0000002270097206_table14759942142014), and the type information of C and Bias is defined through [MatmulType](../../../../../api/SIMD-API/advanced_api/matrix_compute/Matmul-Kernel/matmul_usage.md), including: memory logical position, data format, data type, and transpose information. At the same time, pass [MatmulWithScalePolicy](../../../../../api/SIMD-API/advanced_api/matrix_compute/Matmul-Kernel/MatmulPolicy.md) through the template parameter [MatmulPolicy](../../../../../api/SIMD-API/advanced_api/matrix_compute/Matmul-Kernel/MatmulPolicy.md) to indicate that the MxMatmul scenario is enabled.

    ```
    template <TPosition POSITION, TPosition SCALE_POSITION, CubeFormat FORMAT, typename TYPE, bool ISTRANS = false, TPosition SRCPOS = TPosition::GM, CubeFormat SCALE_FORMAT = FORMAT, bool SCALE_ISTRANS = ISTRANS, TPosition SCALE_SRCPOS = SRCPOS>
    struct MatmulTypeWithScale: public MatmulType<POSITION, FORMAT, TYPE, ISTRANS> {
        constexpr static TPosition scalePosition = SCALE_POSITION;
        constexpr static CubeFormat scaleFormat = SCALE_FORMAT;
        constexpr static bool isScaleTrans = SCALE_ISTRANS;
        constexpr static TPosition srcScalePos = SCALE_SRCPOS;
    };
    ```

2.  **Perform initialization.**

    ```
    REGIST_MATMUL_OBJ(&pipe, GetSysWorkSpacePtr(), mm, &tiling); // Initialize
    ```

3.  **Set left matrix A, right matrix B, left quantization coefficient matrix scaleA, right quantization coefficient matrix scaleB, and Bias.**

    Set the left quantization coefficient matrix scaleA and right quantization coefficient matrix scaleB through [SetTensorScaleA](../../../../../api/SIMD-API/advanced_api/matrix_compute/Matmul-Kernel/SetTensorScaleA.md) and [SetTensorScaleB](../../../../../api/SIMD-API/advanced_api/matrix_compute/Matmul-Kernel/SetTensorScaleB.md).

    ```
    mm.SetTensorA(gm_a, isTransposeA);    // Set left matrix A
    mm.SetTensorB(gm_b, isTransposeB);    // Set right matrix B
    mm.SetTensorScaleA(gm_scaleA, isTransposeScaleA);    // Set left quantization coefficient matrix scaleA
    mm.SetTensorScaleB(gm_scaleB, isTransposeScaleB);    // Set right quantization coefficient matrix scaleB
    mm.SetBias(gm_bias);    // Set Bias
    ```

4.  **Complete the matrix multiplication.**
    -   Call [Iterate](../../../../../api/SIMD-API/advanced_api/matrix_compute/Matmul-Kernel/Iterate.md) to complete a single iteration, and use a while loop to complete the computation of all data on a single core. With the Iterate method, you can control the number of iterations and complete the computation of the required amount of data, which is more flexible.

        ```
        while (mm.Iterate()) {   
            mm.GetTensorC(gm_c); 
        }
        ```

    -   Call [IterateAll](../../../../../api/SIMD-API/advanced_api/matrix_compute/Matmul-Kernel/IterateAll.md) to complete the computation of all data on a single core. The [IterateAll](../../../../../api/SIMD-API/advanced_api/matrix_compute/Matmul-Kernel/IterateAll.md) method does not require loop iteration and is simpler to use.

        ```
        mm.IterateAll(gm_c);
        ```

5.  **End the matrix multiplication operation.**

    ```
    mm.End();
    ```

For more complete operator examples, please refer to [MxMatmul example](https://gitcode.com/cann/asc-devkit/tree/9.1.0/examples/01_simd_cpp_api/04_advanced_api/00_matmul/matmul_mx), [MxMatmul example with custom input source](https://gitcode.com/cann/asc-devkit/tree/9.1.0/examples/01_simd_cpp_api/04_advanced_api/00_matmul/matmul_mx_ub_tscm_nz), and [MxMatmul example with multi-buffer scale cache](https://gitcode.com/cann/asc-devkit/tree/9.1.0/examples/01_simd_cpp_api/04_advanced_api/00_matmul/matmul_mx_scale_cache).

## Parameters<a name="zh-cn_topic_0000002270097206_section2756107144914"></a>

**Table 3** MatmulTypeWithScale parameters

<a name="zh-cn_topic_0000002270097206_table14759942142014"></a>
<table><thead align="left"><tr id="zh-cn_topic_0000002270097206_row07595429207"><th class="cellrowborder" valign="top" width="18.11%" id="mcps1.2.3.1.1"><p id="zh-cn_topic_0000002270097206_p1818821912211"><a name="zh-cn_topic_0000002270097206_p1818821912211"></a><a name="zh-cn_topic_0000002270097206_p1818821912211"></a>Parameter</p></th>
<th class="cellrowborder" valign="top" width="81.89%" id="mcps1.2.3.1.2"><p id="zh-cn_topic_0000002270097206_p9188219102113"><a name="zh-cn_topic_0000002270097206_p9188219102113"></a><a name="zh-cn_topic_0000002270097206_p9188219102113"></a>Description</p></th>
</tr>
</thead>
<tbody><tr id="zh-cn_topic_0000002270097206_row207591442162020"><td class="cellrowborder" valign="top" width="18.11%" headers="mcps1.2.3.1.1 "><p id="zh-cn_topic_0000002270097206_p57591442162010"><a name="zh-cn_topic_0000002270097206_p57591442162010"></a><a name="zh-cn_topic_0000002270097206_p57591442162010"></a>POSITION</p></td>
<td class="cellrowborder" valign="top" width="81.89%" headers="mcps1.2.3.1.2 "><p id="zh-cn_topic_0000002270097206_p126441432182115"><a name="zh-cn_topic_0000002270097206_p126441432182115"></a><a name="zh-cn_topic_0000002270097206_p126441432182115"></a>Memory logical position of the left and right matrices.</p>
<p id="zh-cn_topic_0000002270097206_p11644432192117"><a name="zh-cn_topic_0000002270097206_p11644432192117"></a><a name="zh-cn_topic_0000002270097206_p11644432192117"></a>For <span id="zh-cn_topic_0000002270097206_ph498220912720"><a name="zh-cn_topic_0000002270097206_ph498220912720"></a><a name="zh-cn_topic_0000002270097206_ph498220912720"></a>Ascend 950PR/Ascend 950DT</span>:</p>
<p id="p199760149278"><a name="p199760149278"></a><a name="p199760149278"></a>The A matrix can be set to TPosition::GM, TPosition::VECOUT, TPosition::TSCM</p>
<p id="p1297671462717"><a name="p1297671462717"></a><a name="p1297671462717"></a>The B matrix can be set to TPosition::GM, TPosition::VECOUT, TPosition::TSCM</p>
<p id="zh-cn_topic_0000002270097206_p1267723333010"><a name="zh-cn_topic_0000002270097206_p1267723333010"></a><a name="zh-cn_topic_0000002270097206_p1267723333010"></a>Note: When the A and B matrices are set to TPosition::TSCM, the corresponding Format only supports CubeFormat::NZ.</p></td>
</tr>
<tr id="zh-cn_topic_0000002270097206_row13470153112518"><td class="cellrowborder" valign="top" width="18.11%" headers="mcps1.2.3.1.1 "><p id="zh-cn_topic_0000002270097206_p9507104014512"><a name="zh-cn_topic_0000002270097206_p9507104014512"></a><a name="zh-cn_topic_0000002270097206_p9507104014512"></a>SCALE_POSITION</p></td>
<td class="cellrowborder" valign="top" width="81.89%" headers="mcps1.2.3.1.2 "><p id="zh-cn_topic_0000002270097206_p3507174020512"><a name="zh-cn_topic_0000002270097206_p3507174020512"></a><a name="zh-cn_topic_0000002270097206_p3507174020512"></a>Memory logical position of the quantization coefficient matrix.</p>
<p id="zh-cn_topic_0000002270097206_p145073401055"><a name="zh-cn_topic_0000002270097206_p145073401055"></a><a name="zh-cn_topic_0000002270097206_p145073401055"></a>For <span id="zh-cn_topic_0000002270097206_ph185821514192716"><a name="zh-cn_topic_0000002270097206_ph185821514192716"></a><a name="zh-cn_topic_0000002270097206_ph185821514192716"></a>Ascend 950PR/Ascend 950DT</span>:</p>
<p id="p28081820162716"><a name="p28081820162716"></a><a name="p28081820162716"></a>The scaleA matrix can be set to TPosition::GM, TPosition::VECOUT, TPosition::TSCM</p>
<p id="p08081820132714"><a name="p08081820132714"></a><a name="p08081820132714"></a>The scaleB matrix can be set to TPosition::GM, TPosition::VECOUT, TPosition::TSCM</p>
<p id="zh-cn_topic_0000002270097206_p274171193714"><a name="zh-cn_topic_0000002270097206_p274171193714"></a><a name="zh-cn_topic_0000002270097206_p274171193714"></a>Note: When the scaleA and scaleB matrices are set to TPosition::TSCM, the corresponding SCALE_FORMAT parameter only supports CubeFormat::NZ.</p></td>
</tr>
<tr id="zh-cn_topic_0000002270097206_row1175904217207"><td class="cellrowborder" valign="top" width="18.11%" headers="mcps1.2.3.1.1 "><p id="zh-cn_topic_0000002270097206_p5537172115157"><a name="zh-cn_topic_0000002270097206_p5537172115157"></a><a name="zh-cn_topic_0000002270097206_p5537172115157"></a>FORMAT</p></td>
<td class="cellrowborder" valign="top" width="81.89%" headers="mcps1.2.3.1.2 "><p id="zh-cn_topic_0000002270097206_p6310416287"><a name="zh-cn_topic_0000002270097206_p6310416287"></a><a name="zh-cn_topic_0000002270097206_p6310416287"></a>Physical layout format of the data.</p>
<p id="zh-cn_topic_0000002270097206_p1175934213209"><a name="zh-cn_topic_0000002270097206_p1175934213209"></a><a name="zh-cn_topic_0000002270097206_p1175934213209"></a>For <span id="zh-cn_topic_0000002270097206_ph1712219180275"><a name="zh-cn_topic_0000002270097206_ph1712219180275"></a><a name="zh-cn_topic_0000002270097206_ph1712219180275"></a>Ascend 950PR/Ascend 950DT</span>:</p>
<p id="p1638262618270"><a name="p1638262618270"></a><a name="p1638262618270"></a>Matrix A can be set to CubeFormat::ND, CubeFormat::NZ, or CubeFormat::VECTOR.</p>
<p id="p183823268273"><a name="p183823268273"></a><a name="p183823268273"></a>Matrix B can be set to CubeFormat::ND or CubeFormat::NZ.</p>
<p id="zh-cn_topic_0000002270097206_p9185171813367"><a name="zh-cn_topic_0000002270097206_p9185171813367"></a><a name="zh-cn_topic_0000002270097206_p9185171813367"></a>Note: For the NZ layout format, refer to <a href="../basic_knowledge.md#zh-cn_topic_0000001622194138_section1453415011">Data Format</a> for the layout formats of A and B.</p></td>
</tr>
<tr id="zh-cn_topic_0000002270097206_row167591242142019"><td class="cellrowborder" valign="top" width="18.11%" headers="mcps1.2.3.1.1 "><p id="zh-cn_topic_0000002270097206_p675984214203"><a name="zh-cn_topic_0000002270097206_p675984214203"></a><a name="zh-cn_topic_0000002270097206_p675984214203"></a>TYPE</p></td>
<td class="cellrowborder" valign="top" width="81.89%" headers="mcps1.2.3.1.2 "><p id="p33861110112812"><a name="p33861110112812"></a><a name="p33861110112812"></a>Data type.</p>
<p id="zh-cn_topic_0000002270097206_p86091832163315"><a name="zh-cn_topic_0000002270097206_p86091832163315"></a><a name="zh-cn_topic_0000002270097206_p86091832163315"></a>For <span id="zh-cn_topic_0000002270097206_ph11118321102719"><a name="zh-cn_topic_0000002270097206_ph11118321102719"></a><a name="zh-cn_topic_0000002270097206_ph11118321102719"></a>Ascend 950PR/Ascend 950DT</span>:</p>
<p id="p9107163120275"><a name="p9107163120275"></a><a name="p9107163120275"></a>Matrix A can be set to fp4x2_e1m2_t, fp4x2_e2m1_t, fp8_e4m3fn_t, or fp8_e5m2_t.</p>
<p id="p161081731182712"><a name="p161081731182712"></a><a name="p161081731182712"></a>Matrix B can be set to fp4x2_e1m2_t, fp4x2_e2m1_t, fp8_e4m3fn_t, or fp8_e5m2_t.</p>
<p id="zh-cn_topic_0000002270097206_p1732612596387"><a name="zh-cn_topic_0000002270097206_p1732612596387"></a><a name="zh-cn_topic_0000002270097206_p1732612596387"></a><strong id="zh-cn_topic_0000002270097206_b8326205913819"><a name="zh-cn_topic_0000002270097206_b8326205913819"></a><a name="zh-cn_topic_0000002270097206_b8326205913819"></a>Note: For the specific data type combinations, refer to</strong><a href="#zh-cn_topic_0000002270097206_table5383144710452">MxMatmul Supported Data Types</a>.</p></td>
</tr>
<tr id="zh-cn_topic_0000002270097206_row67591429205"><td class="cellrowborder" valign="top" width="18.11%" headers="mcps1.2.3.1.1 "><p id="zh-cn_topic_0000002270097206_p1475904232016"><a name="zh-cn_topic_0000002270097206_p1475904232016"></a><a name="zh-cn_topic_0000002270097206_p1475904232016"></a>ISTRANS</p></td>
<td class="cellrowborder" valign="top" width="81.89%" headers="mcps1.2.3.1.2 "><p id="zh-cn_topic_0000002270097206_p15691340143420"><a name="zh-cn_topic_0000002270097206_p15691340143420"></a><a name="zh-cn_topic_0000002270097206_p15691340143420"></a>Whether to enable the matrix transpose function for matrices A and B. The default value is false. The supported values are as follows:</p>
<p id="p11558102715289"><a name="p11558102715289"></a><a name="p11558102715289"></a>true: Enables the matrix transpose function. After it is enabled, use the isTransposeA and isTransposeB parameters in SetTensorA and SetTensorB to set whether matrices A and B are transposed. If matrices A and B are set to be transposed, Matmul will consider the shape of matrix A to be [K, M] and the shape of matrix B to be [N, K].</p>
<p id="p45581527162811"><a name="p45581527162811"></a><a name="p45581527162811"></a>false: Disables the matrix transpose function. In this case, the transpose status of matrices A and B cannot be set through SetTensorA and SetTensorB. Matmul will consider the shape of matrix A to be [M, K] and the shape of matrix B to be [K, N].</p></td>
</tr>
<tr id="zh-cn_topic_0000002270097206_row68812038163520"><td class="cellrowborder" valign="top" width="18.11%" headers="mcps1.2.3.1.1 "><p id="zh-cn_topic_0000002270097206_p54471329153612"><a name="zh-cn_topic_0000002270097206_p54471329153612"></a><a name="zh-cn_topic_0000002270097206_p54471329153612"></a>SRCPOS</p></td>
<td class="cellrowborder" valign="top" width="81.89%" headers="mcps1.2.3.1.2 "><p id="zh-cn_topic_0000002270097206_p204471629153618"><a name="zh-cn_topic_0000002270097206_p204471629153618"></a><a name="zh-cn_topic_0000002270097206_p204471629153618"></a>When the POSITION parameter of matrices A/B is set to TPosition::TSCM, set the memory logic position of the source of the matrix data in TSCM. The default value is TPosition::GM.</p>
<p id="zh-cn_topic_0000002270097206_p2231172914619"><a name="zh-cn_topic_0000002270097206_p2231172914619"></a><a name="zh-cn_topic_0000002270097206_p2231172914619"></a>For <span id="zh-cn_topic_0000002270097206_ph1231182974620"><a name="zh-cn_topic_0000002270097206_ph1231182974620"></a><a name="zh-cn_topic_0000002270097206_ph1231182974620"></a>Ascend 950PR/Ascend 950DT</span>:</p>
<p id="p13320143342815"><a name="p13320143342815"></a><a name="p13320143342815"></a>Matrix A can be set to TPosition::GM or TPosition::VECOUT.</p>
<p id="p12320163310288"><a name="p12320163310288"></a><a name="p12320163310288"></a>Matrix B can be set to TPosition::GM or TPosition::VECOUT.</p></td>
</tr>
<tr id="zh-cn_topic_0000002270097206_row7921164017113"><td class="cellrowborder" valign="top" width="18.11%" headers="mcps1.2.3.1.1 "><p id="zh-cn_topic_0000002270097206_p11226185115114"><a name="zh-cn_topic_0000002270097206_p11226185115114"></a><a name="zh-cn_topic_0000002270097206_p11226185115114"></a>SCALE_FORMAT</p></td>
<td class="cellrowborder" valign="top" width="81.89%" headers="mcps1.2.3.1.2 "><p id="zh-cn_topic_0000002270097206_p78145661612"><a name="zh-cn_topic_0000002270097206_p78145661612"></a><a name="zh-cn_topic_0000002270097206_p78145661612"></a>Physical layout format of the quantization coefficient matrix. For details, refer to <a href="#zh-cn_topic_0000002270097206_fig107863142019">Data Format</a>. The default value is the value of the FORMAT parameter.</p>
<p id="zh-cn_topic_0000002270097206_p158141465164"><a name="zh-cn_topic_0000002270097206_p158141465164"></a><a name="zh-cn_topic_0000002270097206_p158141465164"></a>For <span id="zh-cn_topic_0000002270097206_ph695882732713"><a name="zh-cn_topic_0000002270097206_ph695882732713"></a><a name="zh-cn_topic_0000002270097206_ph695882732713"></a>Ascend 950PR/Ascend 950DT</span>:</p>
<p id="p15845121173011"><a name="p15845121173011"></a><a name="p15845121173011"></a>The scaleA matrix can be set to CubeFormat::ND, CubeFormat::NZ, or CubeFormat::VECTOR.</p>
<p id="p884571116308"><a name="p884571116308"></a><a name="p884571116308"></a>The scaleB matrix can be set to CubeFormat::ND or CubeFormat::NZ.</p>
<p id="zh-cn_topic_0000002270097206_p7332111273413"><a name="zh-cn_topic_0000002270097206_p7332111273413"></a><a name="zh-cn_topic_0000002270097206_p7332111273413"></a>Note:</p>
<p id="p15352181633019"><a name="p15352181633019"></a><a name="p15352181633019"></a>For the NZ layout format, refer to <a href="../../../../technical_appendix/concepts_and_terms/neural_networks_and_operators/data_layout_format.md#li19960204116136">NZ</a>. In the MxMatmul scenario, the data type of scaleA and scaleB is fp8_e8m0_t, with fractal size H0=16 and W0=2.</p>
<p id="p43523162303"><a name="p43523162303"></a><a name="p43523162303"></a>In the scenario where the Scale matrix is in ND format, when the scaleA matrix transpose is set through the SetTensorScaleA API, the scaleA memory layout format must follow (K/64, M, 2). When the scaleB matrix is set to non-transposed through the SetTensorScaleB API, the scaleB memory layout format must follow (K/64, N, 2). For details, refer to <a href="#zh-cn_topic_0000002270097206_fig76682054103416">Data Format</a>.</p></td>
</tr>
<tr id="zh-cn_topic_0000002270097206_row0325310101910"><td class="cellrowborder" valign="top" width="18.11%" headers="mcps1.2.3.1.1 "><p id="zh-cn_topic_0000002270097206_p19617394191"><a name="zh-cn_topic_0000002270097206_p19617394191"></a><a name="zh-cn_topic_0000002270097206_p19617394191"></a>SCALE_ISTRANS</p></td>
<td class="cellrowborder" valign="top" width="81.89%" headers="mcps1.2.3.1.2 "><p id="zh-cn_topic_0000002270097206_p22682252237"><a name="zh-cn_topic_0000002270097206_p22682252237"></a><a name="zh-cn_topic_0000002270097206_p22682252237"></a>Whether to enable the matrix transpose function for scaleA and scaleB. The default value is the value of the ISTRANS parameter. The supported values of this parameter are as follows:</p>
<p id="p1407842193011"><a name="p1407842193011"></a><a name="p1407842193011"></a>true: Enables the matrix transpose function. After it is enabled, set whether the scaleA and scaleB matrices are transposed through the isTransposeScaleA and isTransposeScaleB parameters in SetTensorScaleA and SetTensorScaleB, respectively. In the scenario where the Scale matrix is in ND format, if the scaleA and scaleB matrices are set to be transposed, Matmul will consider the shape of the scaleA matrix to be [Ceil(K/64), M, 2] and the shape of the scaleB matrix to be [N, Ceil(K/64), 2].</p>
<p id="p19408124217308"><a name="p19408124217308"></a><a name="p19408124217308"></a>false: Does not enable the matrix transpose function. The transpose status of the scaleA and scaleB matrices cannot be set through SetTensorScaleA and SetTensorScaleB. Matmul will consider the shape of the scaleA matrix to be [M, Ceil(K/64), 2] and the shape of the scaleB matrix to be [Ceil(K/64), N, 2].</p></td>
</tr>
<tr id="zh-cn_topic_0000002270097206_row11254846161912"><td class="cellrowborder" valign="top" width="18.11%" headers="mcps1.2.3.1.1 "><p id="zh-cn_topic_0000002270097206_p6158144912199"><a name="zh-cn_topic_0000002270097206_p6158144912199"></a><a name="zh-cn_topic_0000002270097206_p6158144912199"></a>SCALE_SRCPOS</p></td>
<td class="cellrowborder" valign="top" width="81.89%" headers="mcps1.2.3.1.2 "><p id="zh-cn_topic_0000002270097206_p642071011551"><a name="zh-cn_topic_0000002270097206_p642071011551"></a><a name="zh-cn_topic_0000002270097206_p642071011551"></a>When the SCALE_POSITION parameter of the scaleA and scaleB matrices is set to TPosition::TSCM, use this parameter to set the memory logical position of the matrix data source in TSCM. The default value is the value of the SRCPOS parameter.</p>
<p id="zh-cn_topic_0000002270097206_p134201910115519"><a name="zh-cn_topic_0000002270097206_p134201910115519"></a><a name="zh-cn_topic_0000002270097206_p134201910115519"></a>For <span id="zh-cn_topic_0000002270097206_ph2420111019559"><a name="zh-cn_topic_0000002270097206_ph2420111019559"></a><a name="zh-cn_topic_0000002270097206_ph2420111019559"></a>Ascend 950PR/Ascend 950DT</span>:</p>
<p id="p138204923010"><a name="p138204923010"></a><a name="p138204923010"></a>The scaleA matrix can be set to TPosition::GM or TPosition::VECOUT.</p>
<p id="p68316492306"><a name="p68316492306"></a><a name="p68316492306"></a>The scaleB matrix can be set to TPosition::GM or TPosition::VECOUT.</p></td>
</tr>
</tbody>
</table>

## Constraints<a name="zh-cn_topic_0000002270097206_section14160134220363"></a>

-   The MxMatmul scenario supports only the [Norm template](../../../../../api/SIMD-API/advanced_api/matrix_compute/Matmul-Kernel/MatmulConfig.md) and [MDL template](../../../../../api/SIMD-API/advanced_api/matrix_compute/Matmul-Kernel/MatmulConfig.md).

-   In the MxMatmul scenario, if the positions of matrices A and B are both GM, there is no special restriction on singleKIn. In this case, if the K-direction size of scaleA and scaleB (that is, Ceil\(singleKIn, 32\)) is an odd number, you need to pad zeros in the K direction of scaleA and scaleB to make it an even number. For example, when singleKIn is 30, Ceil\(singleKIn, 32\) is 1, and you need to pad zeros in the K direction of scaleA and scaleB to make the K direction an even number. For other combinations of the logical positions of matrices A and B, that is, when the positions of matrices A and B are not both GM, the value obtained by aligning singleKIn upward to 32 elements must be an even multiple of 32.
-   In the MxMatmul scenario, when the input data type is fp4x2\_e2m1\_t/fp4x2\_e1m2\_t, the inner axis must be an even number.
-   In the MxMatmul scenario, set the data format of matrix A and matrix scaleA to VECTOR to enable [GEMV mode](cube_vector_multiplication.md). In this mode, matrices A and scaleA support only the GM memory logical position, and neither supports transpose.
-   When matrices A and B are UB inputs, the inner axis of the matrix needs to be aligned upward to 32 bytes. For example, when the shape of matrix A is \(M, K\), align K to 32 bytes; when the shape of matrix A is \(K, M\), align M to 32 bytes.
-   When matrices scaleA and scaleB are UB inputs, the inner axis of the matrix needs to be aligned upward to 32 bytes. For example, when the shape of matrix scaleA is \(M, K/32\), align K/32 to 32 bytes; when the shape of matrix scaleA is \(K/32, M\), align M to 32 bytes.
-   When matrices scaleA and scaleB are input in the ND format, the high-order API occupies UB temporary space when performing format conversion internally. Developers need to use the [SetLocalWorkspace](../../../../../api/SIMD-API/advanced_api/matrix_compute/Matmul-Kernel/SetLocalWorkspace.md) API to configure the temporary space. The formula for calculating the temporary space size (in bytes) is as follows.

    ```
    int32_t scaleATmpBuf = 0;
    int32_t scaleBTmpBuf = 0;
    if constexpr (A_TYPE::scalePosition == TPosition::VECOUT) {
        if (A_TYPE::isScaleTrans) {
            scaleATmpBuf = CeilAlign(SingleCoreM, 32) * scaleK;
        } else {
            scaleATmpBuf = CeilAlign(scaleK, 32) * SingleCoreM;
        }
    }
    if constexpr (B_TYPE::scalePosition == TPosition::VECOUT) {
        if (B_TYPE::isScaleTrans) {
            scaleBTmpBuf = SingleCoreN * CeilAlign(scaleK, 32);
        } else {
            scaleBTmpBuf = scaleK * CeilAlign(SingleCoreN, 32);
        }
    }
    int32_t totalTmpBuf = scaleATmpBuf + scaleBTmpBuf;
    ```
