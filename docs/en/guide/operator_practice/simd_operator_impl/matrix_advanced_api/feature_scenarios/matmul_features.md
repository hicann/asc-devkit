# Matmul Feature Introduction<a name="ZH-CN_TOPIC_0000002500468238"></a>

<!-- md-trans-meta sourceCommit=7c6b4a9f1bae21994540d7d2fa2801ed0b239247 translatedAt=2026-11-09T10:30:23.324Z pushedAt=2026-11-19T06:32:19.737Z nl=2 -->

In addition to the basic computation capabilities described in [Basic Knowledge](../basics.md) and [Operator Implementation](../operator_impl.md), Matmul cube programming also provides processing capabilities and various features for different scenarios. The specific scenarios and features are listed in the following table. For details, see the subsequent sections.

**Table 1** Matmul features<a name="zh-cn_topic_0000002298078557_table1220612410208"></a>

<table><thead align="left"><tr id="zh-cn_topic_0000002298078557_row19206141132015"><th class="cellrowborder" valign="top" width="23.64%" id="mcps1.2.3.1.1"><p id="zh-cn_topic_0000002298078557_p52061441122017"><a name="zh-cn_topic_0000002298078557_p52061441122017"></a><a name="zh-cn_topic_0000002298078557_p52061441122017"></a>Feature</p>
</th>
<th class="cellrowborder" valign="top" width="76.36%" id="mcps1.2.3.1.2"><p id="zh-cn_topic_0000002298078557_p1206124122015"><a name="zh-cn_topic_0000002298078557_p1206124122015"></a><a name="zh-cn_topic_0000002298078557_p1206124122015"></a>Description</p>
</th>
</tr>
</thead>
<tbody><tr id="zh-cn_topic_0000002298078557_row8206941192015"><td class="cellrowborder" valign="top" width="23.64%" headers="mcps1.2.3.1.1 "><p id="zh-cn_topic_0000002298078557_p108541234142315"><a name="zh-cn_topic_0000002298078557_p108541234142315"></a><a name="zh-cn_topic_0000002298078557_p108541234142315"></a><a href="multi_core_aligned_split.md">Multi-core Aligned Split</a></p>
</td>
<td class="cellrowborder" valign="top" width="76.36%" headers="mcps1.2.3.1.2 "><p id="zh-cn_topic_0000002298078557_p8702111310525"><a name="zh-cn_topic_0000002298078557_p8702111310525"></a><a name="zh-cn_topic_0000002298078557_p8702111310525"></a>In multi-core scenarios, cube data can be split along the M, N, and K axes. This is the processing method for aligned scenarios where M is divisible by singleCoreM, N is divisible by singleCoreN, and K is divisible by singleCoreK, thereby implementing multi-core parallel cube multiplication computation.</p>
</td>
</tr>
<tr id="zh-cn_topic_0000002298078557_row12206184142016"><td class="cellrowborder" valign="top" width="23.64%" headers="mcps1.2.3.1.1 "><p id="zh-cn_topic_0000002298078557_p954132121113"><a name="zh-cn_topic_0000002298078557_p954132121113"></a><a name="zh-cn_topic_0000002298078557_p954132121113"></a><a href="multi_core_unaligned_split.md">Multi-core Unaligned Split</a></p>
</td>
<td class="cellrowborder" valign="top" width="76.36%" headers="mcps1.2.3.1.2 "><p id="zh-cn_topic_0000002298078557_p17648104611560"><a name="zh-cn_topic_0000002298078557_p17648104611560"></a><a name="zh-cn_topic_0000002298078557_p17648104611560"></a>In multi-core scenarios, cube data can be split along the M, N, and K axes. This is the processing method for unaligned scenarios (that is, tail-block scenarios) where M is not divisible by singleCoreM, or N is not divisible by singleCoreN, or K is not divisible by singleCoreK.</p>
</td>
</tr>
<tr id="zh-cn_topic_0000002298078557_row12207144114201"><td class="cellrowborder" valign="top" width="23.64%" headers="mcps1.2.3.1.1 "><p id="zh-cn_topic_0000002298078557_p5853634122310"><a name="zh-cn_topic_0000002298078557_p5853634122310"></a><a name="zh-cn_topic_0000002298078557_p5853634122310"></a><a href="async_scenario.md">Asynchronous Scenario Handling</a></p>
</td>
<td class="cellrowborder" valign="top" width="76.36%" headers="mcps1.2.3.1.2 "><p id="zh-cn_topic_0000002298078557_p182077411209"><a name="zh-cn_topic_0000002298078557_p182077411209"></a><a name="zh-cn_topic_0000002298078557_p182077411209"></a>In MIX scenarios (including cube computation and vector computation), other computations can be executed first without waiting for the cube multiplication computation to complete.</p>
</td>
</tr>
<tr id="row5682043577"><td class="cellrowborder" valign="top" width="23.64%" headers="mcps1.2.3.1.1 "><p id="zh-cn_topic_0000002298078557_p2779724105810"><a name="zh-cn_topic_0000002298078557_p2779724105810"></a><a name="zh-cn_topic_0000002298078557_p2779724105810"></a><a href="matmul_output_quant_dequant.md">Quantization/Dequantization of Cube Multiplication Output</a></p>
</td>
<td class="cellrowborder" valign="top" width="76.36%" headers="mcps1.2.3.1.2 "><p id="zh-cn_topic_0000002298078557_p8779024175813"><a name="zh-cn_topic_0000002298078557_p8779024175813"></a><a name="zh-cn_topic_0000002298078557_p8779024175813"></a>When moving the cube multiplication computation result from the L0C Buffer (CO1) to Global Memory, performs data quantization or dequantization on the cube elements.</p>
</td>
</tr>
<tr id="zh-cn_topic_0000002298078557_row1320716413201"><td class="cellrowborder" valign="top" width="23.64%" headers="mcps1.2.3.1.1 "><p id="zh-cn_topic_0000002298078557_p1854719415227"><a name="zh-cn_topic_0000002298078557_p1854719415227"></a><a name="zh-cn_topic_0000002298078557_p1854719415227"></a><a href="../../../../../api/SIMD-API/adv_api/cube_compute/Matmul_Kernel/MatmulCallBackFunc.md">Custom Data Move-In and Move-Out</a></p>
</td>
<td class="cellrowborder" valign="top" width="76.36%" headers="mcps1.2.3.1.2 "><p id="zh-cn_topic_0000002298078557_p1820704113201"><a name="zh-cn_topic_0000002298078557_p1820704113201"></a><a name="zh-cn_topic_0000002298078557_p1820704113201"></a>Customizes the data transfer functions before and after cube multiplication computation. This feature supports user-defined processes for moving the left cube A and right cube B from Global Memory to L1 Buffer (A1) and L1 Buffer (B1), respectively, and for moving the output C cube from the L0C Buffer (CO1) to Global Memory.</p>
</td>
</tr>
<tr id="zh-cn_topic_0000002298078557_row142071416207"><td class="cellrowborder" valign="top" width="23.64%" headers="mcps1.2.3.1.1 "><p id="zh-cn_topic_0000002298078557_p94242715239"><a name="zh-cn_topic_0000002298078557_p94242715239"></a><a name="zh-cn_topic_0000002298078557_p94242715239"></a><a href="matmul_output_channel_split.md">Channel Split of Cube Multiplication Output</a></p>
</td>
<td class="cellrowborder" valign="top" width="76.36%" headers="mcps1.2.3.1.2 "><p id="zh-cn_topic_0000002298078557_p320711418202"><a name="zh-cn_topic_0000002298078557_p320711418202"></a><a name="zh-cn_topic_0000002298078557_p320711418202"></a>Channel split of the cube multiplication output, also known as ChannelSplit. It means that the output C cube of the float data type in NZ data format is stored in 16*8 fractal blocks.</p>
</td>
</tr>
<tr id="zh-cn_topic_0000002298078557_row20207741152014"><td class="cellrowborder" valign="top" width="23.64%" headers="mcps1.2.3.1.1 "><p id="zh-cn_topic_0000002298078557_p5546154142210"><a name="zh-cn_topic_0000002298078557_p5546154142210"></a><a name="zh-cn_topic_0000002298078557_p5546154142210"></a><a href="matrix_vector_multiply.md">Cube-Vector Multiplication</a></p>
</td>
<td class="cellrowborder" valign="top" width="76.36%" headers="mcps1.2.3.1.2 "><p id="zh-cn_topic_0000002298078557_p120764172019"><a name="zh-cn_topic_0000002298078557_p120764172019"></a><a name="zh-cn_topic_0000002298078557_p120764172019"></a>Cube-vector multiplication, that is, GEMV, refers to the scenario where M=1 and K&gt;1 in cube multiplication computation, that is, cube multiplication computation is performed on the left cube A with the shape (1, K).</p>
</td>
</tr>
<tr id="row1892083116812"><td class="cellrowborder" valign="top" width="23.64%" headers="mcps1.2.3.1.1 "><p id="zh-cn_topic_0000002298078557_p87791524145813"><a name="zh-cn_topic_0000002298078557_p87791524145813"></a><a name="zh-cn_topic_0000002298078557_p87791524145813"></a><a href="4_2_sparse_matmul.md">4:2 Sparse Cube Multiplication</a></p>
</td>
<td class="cellrowborder" valign="top" width="76.36%" headers="mcps1.2.3.1.2 "><p id="zh-cn_topic_0000002298078557_p97799246588"><a name="zh-cn_topic_0000002298078557_p97799246588"></a><a name="zh-cn_topic_0000002298078557_p97799246588"></a>4:2 sparse cube multiplication, also known as Sparse Matmul. It refers to cube multiplication computation performed on the sparse left cube A and the 4:2 densified right cube B.</p>
</td>
</tr>
<tr id="zh-cn_topic_0000002298078557_row16577182820235"><td class="cellrowborder" valign="top" width="23.64%" headers="mcps1.2.3.1.1 "><p id="zh-cn_topic_0000002298078557_p13858194593417"><a name="zh-cn_topic_0000002298078557_p13858194593417"></a><a name="zh-cn_topic_0000002298078557_p13858194593417"></a><a href="../../../../../api/SIMD-API/adv_api/cube_compute/Matmul_Kernel/MatmulPolicy.md">Upper/Lower Triangular Cube Multiplication</a></p>
</td>
<td class="cellrowborder" valign="top" width="76.36%" headers="mcps1.2.3.1.2 "><p id="zh-cn_topic_0000002298078557_p457718285237"><a name="zh-cn_topic_0000002298078557_p457718285237"></a><a name="zh-cn_topic_0000002298078557_p457718285237"></a>Ignores the computation of elements in the lower or upper triangular positions of the cube, and implements cube multiplication computation for the elements in the upper or lower triangular positions of the cube.</p>
</td>
</tr>
<tr id="zh-cn_topic_0000002298078557_row2748530162319"><td class="cellrowborder" valign="top" width="23.64%" headers="mcps1.2.3.1.1 "><p id="zh-cn_topic_0000002298078557_p141909471896"><a name="zh-cn_topic_0000002298078557_p141909471896"></a><a name="zh-cn_topic_0000002298078557_p141909471896"></a><a href="tscm_input_matmul.md">Cube Multiplication with TSCM Input</a></p>
</td>
<td class="cellrowborder" valign="top" width="76.36%" headers="mcps1.2.3.1.2 "><p id="zh-cn_topic_0000002298078557_p1748143002320"><a name="zh-cn_topic_0000002298078557_p1748143002320"></a><a name="zh-cn_topic_0000002298078557_p1748143002320"></a>Implements cube multiplication computation for left cube A or right cube B whose memory logical position is <a href="../../../../technical_appendix/concepts_and_terms/glossary.md#p1392229512">TSCM</a>.</p>
</td>
</tr>
<tr id="zh-cn_topic_0000002298078557_row1737123113238"><td class="cellrowborder" valign="top" width="23.64%" headers="mcps1.2.3.1.1 "><p id="zh-cn_topic_0000002298078557_p1218910471691"><a name="zh-cn_topic_0000002298078557_p1218910471691"></a><a name="zh-cn_topic_0000002298078557_p1218910471691"></a><a href="matmul_output_n_alignment.md">N-Direction Alignment of Cube Multiplication Output</a></p>
</td>
<td class="cellrowborder" valign="top" width="76.36%" headers="mcps1.2.3.1.2 "><p id="zh-cn_topic_0000002298078557_p18372153102310"><a name="zh-cn_topic_0000002298078557_p18372153102310"></a><a name="zh-cn_topic_0000002298078557_p18372153102310"></a>N-direction alignment of cube multiplication output, also known as ND_ALIGN format output. It refers to automatic padding and output of the output cube C in ND_ALIGN data format with 32-byte alignment in the N direction.</p>
</td>
</tr>
<tr id="zh-cn_topic_0000002298078557_row191001232122314"><td class="cellrowborder" valign="top" width="23.64%" headers="mcps1.2.3.1.1 "><p id="zh-cn_topic_0000002298078557_p81883473913"><a name="zh-cn_topic_0000002298078557_p81883473913"></a><a name="zh-cn_topic_0000002298078557_p81883473913"></a><a href="single_matmul_local_output.md">Partial Output of a Single Cube Multiplication</a></p>
</td>
<td class="cellrowborder" valign="top" width="76.36%" headers="mcps1.2.3.1.2 "><p id="zh-cn_topic_0000002298078557_p610015321237"><a name="zh-cn_topic_0000002298078557_p610015321237"></a><a name="zh-cn_topic_0000002298078557_p610015321237"></a>Partial output of a single cube multiplication, also known as Partial Output. It refers to directly outputting the computation result without accumulating the computation results in the K direction of a single core during cube multiplication computation.</p>
</td>
</tr>
<tr id="zh-cn_topic_0000002298078557_row1661618291916"><td class="cellrowborder" valign="top" width="23.64%" headers="mcps1.2.3.1.1 "><p id="zh-cn_topic_0000002298078557_p17187847894"><a name="zh-cn_topic_0000002298078557_p17187847894"></a><a name="zh-cn_topic_0000002298078557_p17187847894"></a><a href="aic_aiv_independent_execution.md">Independent Execution Mechanism of AIC and AIV</a></p>
</td>
<td class="cellrowborder" valign="top" width="76.36%" headers="mcps1.2.3.1.2 "><p id="zh-cn_topic_0000002298078557_p116161329690"><a name="zh-cn_topic_0000002298078557_p116161329690"></a><a name="zh-cn_topic_0000002298078557_p116161329690"></a>Independent execution mechanism of AIC and AIV, also known as dual-master mode. In the MIX scenario (including cube computation and vector computation), the AIC core and AIV core run code independently without relying on message driving.</p>
</td>
</tr>
<tr id="row185123919917"><td class="cellrowborder" valign="top" width="23.64%" headers="mcps1.2.3.1.1 "><p id="zh-cn_topic_0000002298078557_p14741614191313"><a name="zh-cn_topic_0000002298078557_p14741614191313"></a><a name="zh-cn_topic_0000002298078557_p14741614191313"></a><a href="mx_matmul_scenario.md">MxMatmul Scenario</a></p>
</td>
<td class="cellrowborder" valign="top" width="76.36%" headers="mcps1.2.3.1.2 "><p id="zh-cn_topic_0000002298078557_p107411814141310"><a name="zh-cn_topic_0000002298078557_p107411814141310"></a><a name="zh-cn_topic_0000002298078557_p107411814141310"></a>Matrix multiplication with quantization coefficients, that is, both the left cube and the right cube have corresponding quantization coefficient cubes. The left cube and the right cube are quantized separately before the cube multiplication computation is performed.</p>
</td>
</tr>
</tbody>
</table>

**Table 2**  BatchMatmul features<a name="zh-cn_topic_0000002298078557_table1558122415575"></a>

<table><thead align="left"><tr id="zh-cn_topic_0000002298078557_row1255852419575"><th class="cellrowborder" valign="top" width="24.04%" id="mcps1.2.3.1.1"><p id="zh-cn_topic_0000002298078557_p4558162445718"><a name="zh-cn_topic_0000002298078557_p4558162445718"></a><a name="zh-cn_topic_0000002298078557_p4558162445718"></a>Feature</p>
</th>
<th class="cellrowborder" valign="top" width="75.96000000000001%" id="mcps1.2.3.1.2"><p id="zh-cn_topic_0000002298078557_p105581124185715"><a name="zh-cn_topic_0000002298078557_p105581124185715"></a><a name="zh-cn_topic_0000002298078557_p105581124185715"></a>Description</p>
</th>
</tr>
</thead>
<tbody><tr id="zh-cn_topic_0000002298078557_row1655816246576"><td class="cellrowborder" valign="top" width="24.04%" headers="mcps1.2.3.1.1 "><p id="zh-cn_topic_0000002298078557_p755882465718"><a name="zh-cn_topic_0000002298078557_p755882465718"></a><a name="zh-cn_topic_0000002298078557_p755882465718"></a><a href="batch_matmul_basics.md">Batch Matmul Basic Functions</a></p>
</td>
<td class="cellrowborder" valign="top" width="75.96000000000001%" headers="mcps1.2.3.1.2 "><p id="zh-cn_topic_0000002298078557_p175581124155711"><a name="zh-cn_topic_0000002298078557_p175581124155711"></a><a name="zh-cn_topic_0000002298078557_p175581124155711"></a>Basic functions of Batch Matmul, which support batch processing of Matmul. By calling the IterateBatch API once, multiple C cubes of the singleCoreM * singleCoreN size are computed.</p>
</td>
</tr>
<tr id="zh-cn_topic_0000002298078557_row1955852414573"><td class="cellrowborder" valign="top" width="24.04%" headers="mcps1.2.3.1.1 "><p id="zh-cn_topic_0000002298078557_p6559172455716"><a name="zh-cn_topic_0000002298078557_p6559172455716"></a><a name="zh-cn_topic_0000002298078557_p6559172455716"></a><a href="batch_matmul_reuse_bias.md">Bias Cube Reuse of Batch Matmul</a></p>
</td>
<td class="cellrowborder" valign="top" width="75.96000000000001%" headers="mcps1.2.3.1.2 "><p id="zh-cn_topic_0000002298078557_p16559162418579"><a name="zh-cn_topic_0000002298078557_p16559162418579"></a><a name="zh-cn_topic_0000002298078557_p16559162418579"></a>The Matmul computation of each batch reuses the same Bias cube without a batch axis.</p>
</td>
</tr>
</tbody>
</table>
