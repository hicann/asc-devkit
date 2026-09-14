# Advanced API Migration Guide<a name="ZH-CN_TOPIC_0000002503588125"></a>

<!-- md-trans-meta sourceCommit=198cbfdf29f72f78c8de735d88a5244dc258f2e7 translatedAt=2026-08-26T12:48:05.599Z -->

Ascend C advanced APIs are basically compatible with [NPU architecture version 3510](../../../programming_guide/language_extension/SIMD-BuiltIn_keyword.md#table65291052154114) and [NPU architecture version 2201](../../../programming_guide/language_extension/SIMD-BuiltIn_keyword.md#table65291052154114), and some APIs have been extended. The current 3510 architecture does not support advanced APIs for convolution computation.

## Matmul Advanced API<a name="section6925172655117"></a>

-   The supported data types have changed.

    **Table 1**  Data type compatibility

    <a name="table1534819819338"></a>
    <table><thead align="left"><tr id="row1234720819332"><th class="cellrowborder" valign="top" width="20.849999999999998%" id="mcps1.2.6.1.1"><p id="p43471486333"><a name="p43471486333"></a><a name="p43471486333"></a>A matrix</p>
    </th>
    <th class="cellrowborder" valign="top" width="19.05%" id="mcps1.2.6.1.2"><p id="p14347686337"><a name="p14347686337"></a><a name="p14347686337"></a>B matrix</p>
    </th>
    <th class="cellrowborder" valign="top" width="22.68%" id="mcps1.2.6.1.3"><p id="p13347188143313"><a name="p13347188143313"></a><a name="p13347188143313"></a>Bias matrix</p>
    </th>
    <th class="cellrowborder" valign="top" width="21.47%" id="mcps1.2.6.1.4"><p id="p03471788331"><a name="p03471788331"></a><a name="p03471788331"></a>C matrix</p>
    </th>
    <th class="cellrowborder" valign="top" width="15.950000000000001%" id="mcps1.2.6.1.5"><p id="p1934720814334"><a name="p1934720814334"></a><a name="p1934720814334"></a>Description</p>
    </th>
    </tr>
    </thead>
    <tbody><tr id="row1534748113317"><td class="cellrowborder" valign="top" width="20.849999999999998%" headers="mcps1.2.6.1.1 "><p id="p19347168103314"><a name="p19347168103314"></a><a name="p19347168103314"></a>int4b_t</p>
    </td>
    <td class="cellrowborder" valign="top" width="19.05%" headers="mcps1.2.6.1.2 "><p id="p16347168193319"><a name="p16347168193319"></a><a name="p16347168193319"></a>int4b_t</p>
    </td>
    <td class="cellrowborder" valign="top" width="22.68%" headers="mcps1.2.6.1.3 "><p id="p0347985333"><a name="p0347985333"></a><a name="p0347985333"></a>int32_t</p>
    </td>
    <td class="cellrowborder" valign="top" width="21.47%" headers="mcps1.2.6.1.4 "><p id="p1634717893315"><a name="p1634717893315"></a><a name="p1634717893315"></a>int32_t, half</p>
    </td>
    <td class="cellrowborder" valign="top" width="15.950000000000001%" headers="mcps1.2.6.1.5 "><p id="p1034788163317"><a name="p1034788163317"></a><a name="p1034788163317"></a>Not supported by the 3510 architecture.</p>
    </td>
    </tr>
    <tr id="row2348108183316"><td class="cellrowborder" valign="top" width="20.849999999999998%" headers="mcps1.2.6.1.1 "><p id="p1534728183316"><a name="p1534728183316"></a><a name="p1534728183316"></a>fp8_e4m3fn_t, fp8_e5m2_t</p>
    </td>
    <td class="cellrowborder" valign="top" width="19.05%" headers="mcps1.2.6.1.2 "><p id="p17347138103316"><a name="p17347138103316"></a><a name="p17347138103316"></a>fp8_e4m3fn_t, fp8_e5m2_t</p>
    </td>
    <td class="cellrowborder" valign="top" width="22.68%" headers="mcps1.2.6.1.3 "><p id="p434819893316"><a name="p434819893316"></a><a name="p434819893316"></a>float, half, bfloat16_t</p>
    </td>
    <td class="cellrowborder" valign="top" width="21.47%" headers="mcps1.2.6.1.4 "><p id="p9348108163320"><a name="p9348108163320"></a><a name="p9348108163320"></a>fp8_e4m3fn_t, half, bfloat16_t, float</p>
    </td>
    <td class="cellrowborder" valign="top" width="15.950000000000001%" headers="mcps1.2.6.1.5 "><p id="p133481873315"><a name="p133481873315"></a><a name="p133481873315"></a>New in the 3510 architecture.</p>
    </td>
    </tr>
    <tr id="row93481388339"><td class="cellrowborder" valign="top" width="20.849999999999998%" headers="mcps1.2.6.1.1 "><p id="p334811853313"><a name="p334811853313"></a><a name="p334811853313"></a>hifloat8_t</p>
    </td>
    <td class="cellrowborder" valign="top" width="19.05%" headers="mcps1.2.6.1.2 "><p id="p03481387332"><a name="p03481387332"></a><a name="p03481387332"></a>hifloat8_t</p>
    </td>
    <td class="cellrowborder" valign="top" width="22.68%" headers="mcps1.2.6.1.3 "><p id="p734813873310"><a name="p734813873310"></a><a name="p734813873310"></a>float, half, bfloat16_t</p>
    </td>
    <td class="cellrowborder" valign="top" width="21.47%" headers="mcps1.2.6.1.4 "><p id="p4348480335"><a name="p4348480335"></a><a name="p4348480335"></a>hifloat8_t, half, bfloat16_t, float</p>
    </td>
    <td class="cellrowborder" valign="top" width="15.950000000000001%" headers="mcps1.2.6.1.5 "><p id="p1334828163311"><a name="p1334828163311"></a><a name="p1334828163311"></a>3510 architecture addition.</p>
    </td>
    </tr>
    <tr id="row23485883317"><td class="cellrowborder" valign="top" width="20.849999999999998%" headers="mcps1.2.6.1.1 "><p id="p134878103315"><a name="p134878103315"></a><a name="p134878103315"></a>float</p>
    </td>
    <td class="cellrowborder" valign="top" width="19.05%" headers="mcps1.2.6.1.2 "><p id="p934813833317"><a name="p934813833317"></a><a name="p934813833317"></a>float</p>
    </td>
    <td class="cellrowborder" valign="top" width="22.68%" headers="mcps1.2.6.1.3 "><p id="p3348389334"><a name="p3348389334"></a><a name="p3348389334"></a>bfloat16_t</p>
    </td>
    <td class="cellrowborder" valign="top" width="21.47%" headers="mcps1.2.6.1.4 "><p id="p73486819337"><a name="p73486819337"></a><a name="p73486819337"></a>float, half, bfloat16_t</p>
    </td>
    <td class="cellrowborder" valign="top" width="15.950000000000001%" headers="mcps1.2.6.1.5 "><p id="p14348387336"><a name="p14348387336"></a><a name="p14348387336"></a>3510 architecture addition.</p>
    </td>
    </tr>
    <tr id="row113481180333"><td class="cellrowborder" valign="top" width="20.849999999999998%" headers="mcps1.2.6.1.1 "><p id="p123481853316"><a name="p123481853316"></a><a name="p123481853316"></a>bfloat16_t</p>
    </td>
    <td class="cellrowborder" valign="top" width="19.05%" headers="mcps1.2.6.1.2 "><p id="p123486853310"><a name="p123486853310"></a><a name="p123486853310"></a>bfloat16_t</p>
    </td>
    <td class="cellrowborder" valign="top" width="22.68%" headers="mcps1.2.6.1.3 "><p id="p834828183310"><a name="p834828183310"></a><a name="p834828183310"></a>bfloat16_t</p>
    </td>
    <td class="cellrowborder" valign="top" width="21.47%" headers="mcps1.2.6.1.4 "><p id="p17348118193320"><a name="p17348118193320"></a><a name="p17348118193320"></a>float, half, bfloat16_t</p>
    </td>
    <td class="cellrowborder" valign="top" width="15.950000000000001%" headers="mcps1.2.6.1.5 "><p id="p173488812336"><a name="p173488812336"></a><a name="p173488812336"></a>Architecture addition in 3510.</p>
    </td>
    </tr>
    <tr id="row3348128153315"><td class="cellrowborder" valign="top" width="20.849999999999998%" headers="mcps1.2.6.1.1 "><p id="p13486803312"><a name="p13486803312"></a><a name="p13486803312"></a>half</p>
    </td>
    <td class="cellrowborder" valign="top" width="19.05%" headers="mcps1.2.6.1.2 "><p id="p23482893314"><a name="p23482893314"></a><a name="p23482893314"></a>half</p>
    </td>
    <td class="cellrowborder" valign="top" width="22.68%" headers="mcps1.2.6.1.3 "><p id="p33489816333"><a name="p33489816333"></a><a name="p33489816333"></a>bfloat16_t</p>
    </td>
    <td class="cellrowborder" valign="top" width="21.47%" headers="mcps1.2.6.1.4 "><p id="p113481815337"><a name="p113481815337"></a><a name="p113481815337"></a>float, half, bfloat16_t</p>
    </td>
    <td class="cellrowborder" valign="top" width="15.950000000000001%" headers="mcps1.2.6.1.5 "><p id="p134848153312"><a name="p134848153312"></a><a name="p134848153312"></a>Architecture addition in 3510.</p>
    </td>
    </tr>
    <tr id="row1634817819339"><td class="cellrowborder" valign="top" width="20.849999999999998%" headers="mcps1.2.6.1.1 "><p id="p03487883313"><a name="p03487883313"></a><a name="p03487883313"></a>int8_t</p>
    </td>
    <td class="cellrowborder" valign="top" width="19.05%" headers="mcps1.2.6.1.2 "><p id="p33481786334"><a name="p33481786334"></a><a name="p33481786334"></a>int8_t</p>
    </td>
    <td class="cellrowborder" valign="top" width="22.68%" headers="mcps1.2.6.1.3 "><p id="p1134812819331"><a name="p1134812819331"></a><a name="p1134812819331"></a>int32_t</p>
    </td>
    <td class="cellrowborder" valign="top" width="21.47%" headers="mcps1.2.6.1.4 "><p id="p1334898193311"><a name="p1334898193311"></a><a name="p1334898193311"></a>bfloat16_t</p>
    </td>
    <td class="cellrowborder" valign="top" width="15.950000000000001%" headers="mcps1.2.6.1.5 "><p id="p1334820816331"><a name="p1334820816331"></a><a name="p1334820816331"></a>3510 architecture addition.</p>
    </td>
    </tr>
    </tbody>
    </table>

-   The 4:2 sparsity feature is not supported. For the specific compatibility solution, see [4:2 Structured Sparsity](basic_api_migration_guide.md#li69092585134).

## Other Advanced APIs<a name="section14195124865717"></a>

**Table 2** Mathematical computation

<a name="table1011733113422"></a>
<table><thead align="left"><tr id="row3118531154217"><th class="cellrowborder" valign="top" width="50%" id="mcps1.2.3.1.1"><p id="p0118031104211"><a name="p0118031104211"></a><a name="p0118031104211"></a>AscendC Advanced API</p></th>
<th class="cellrowborder" valign="top" width="50%" id="mcps1.2.3.1.2"><p id="p8118143124210"><a name="p8118143124210"></a><a name="p8118143124210"></a>Compatibility Description</p></th>
</tr>
</thead>
<tbody><tr id="row14118113119422"><td class="cellrowborder" valign="top" width="50%" headers="mcps1.2.3.1.1 "><p id="p9188171317366"><a name="p9188171317366"></a><a name="p9188171317366"></a>Tanh, Asin, Sin, Acos, Cos, Log, Atan, Fmod</p>
<p id="p950434551915"><a name="p950434551915"></a><a name="p950434551915"></a></p></td>
<td class="cellrowborder" valign="top" width="50%" headers="mcps1.2.3.1.2 "><p id="p15508125252615"><a name="p15508125252615"></a><a name="p15508125252615"></a>Compatible with the 2201 architecture.</p>
<p id="p550845202613"><a name="p550845202613"></a><a name="p550845202613"></a>Extended support for algorithm configuration, configuring the algorithm used by the API via template parameters to provide high-precision and high-performance algorithm options.</p></td>
</tr>
<tr id="row16667103375216"><td class="cellrowborder" valign="top" width="50%" headers="mcps1.2.3.1.1 "><p id="p1935705813247"><a name="p1935705813247"></a><a name="p1935705813247"></a>Sinh, Cosh, Tan, Trunc, Frac, Erf, Erfc, Atanh, Asinh,</p>
<p id="p1907642152910"><a name="p1907642152910"></a><a name="p1907642152910"></a>Acosh, Floor, Ceil, Round, Axpy, Exp, Lgamma, Digamma, Xor, Cumsum</p></td>
<td class="cellrowborder" valign="top" width="50%" headers="mcps1.2.3.1.2 "><p id="p137711542268"><a name="p137711542268"></a><a name="p137711542268"></a>Compatible with the 2201 architecture.</p></td>
</tr>
<tr id="row1868813347468"><td class="cellrowborder" valign="top" width="50%" headers="mcps1.2.3.1.1 "><p id="p9688434144610"><a name="p9688434144610"></a><a name="p9688434144610"></a>Power</p></td>
<td class="cellrowborder" valign="top" width="50%" headers="mcps1.2.3.1.2 "><p id="p46431157182619"><a name="p46431157182619"></a><a name="p46431157182619"></a>Compatible with the 2201 architecture.</p>
<p id="p15643457182613"><a name="p15643457182613"></a><a name="p15643457182613"></a>Extended support for the uint8_t, int8_t, uint16_t, int16_t, uint32_t, and bfloat16_t data types.</p></td>
</tr>
<tr id="row69291914164916"><td class="cellrowborder" valign="top" width="50%" headers="mcps1.2.3.1.1 "><p id="p1392941424911"><a name="p1392941424911"></a><a name="p1392941424911"></a>Sign</p></td>
<td class="cellrowborder" valign="top" width="50%" headers="mcps1.2.3.1.2 "><p id="p1870316072712"><a name="p1870316072712"></a><a name="p1870316072712"></a>Compatible with the 2201 architecture.</p>
<p id="p1670412092711"><a name="p1670412092711"></a><a name="p1670412092711"></a>Extended support for the int64_t data type.</p></td>
</tr>
</tbody>
</table>

**Table 3** Activation functions

<a name="table192231726173811"></a>
<table><thead align="left"><tr id="row1422362623814"><th class="cellrowborder" valign="top" width="50%" id="mcps1.2.3.1.1"><p id="p1041413536446"><a name="p1041413536446"></a><a name="p1041413536446"></a>AscendC Advanced API</p></th>
<th class="cellrowborder" valign="top" width="50%" id="mcps1.2.3.1.2"><p id="p9414175314412"><a name="p9414175314412"></a><a name="p9414175314412"></a>Compatibility Description</p></th>
</tr>
</thead>
<tbody><tr id="row522452616384"><td class="cellrowborder" valign="top" width="50%" headers="mcps1.2.3.1.1 "><p id="p925911352511"><a name="p925911352511"></a><a name="p925911352511"></a>SoftMax, SimpleSoftMax, SoftmaxFlash, SoftmaxGrad, SoftmaxFlashV2, SoftmaxFlashV3, SoftmaxGradFront, AdjustSoftMaxRes, LogSoftMax, FasterGelu, FasterGeluV2, Gelu, SwiGLU, Silu, Swish, GeGLU, ReFlu, Sigmoid</p></td>
<td class="cellrowborder" valign="top" width="50%" headers="mcps1.2.3.1.2 "><p id="p151696450316"><a name="p151696450316"></a><a name="p151696450316"></a>Compatible with the 2201 architecture.</p></td>
</tr>
</tbody>
</table>

**Table 4** Data normalization

<a name="table1488319115396"></a>
<table><thead align="left"><tr id="row138831103911"><th class="cellrowborder" valign="top" width="49.980000000000004%" id="mcps1.2.3.1.1"><p id="p8665135824416"><a name="p8665135824416"></a><a name="p8665135824416"></a>AscendC Advanced API</p></th>
<th class="cellrowborder" valign="top" width="50.019999999999996%" id="mcps1.2.3.1.2"><p id="p1466513582447"><a name="p1466513582447"></a><a name="p1466513582447"></a>Compatibility Description</p></th>
</tr>
</thead>
<tbody><tr id="row1688319112397"><td class="cellrowborder" valign="top" width="49.980000000000004%" headers="mcps1.2.3.1.1 "><p id="p1088313113916"><a name="p1088313113916"></a><a name="p1088313113916"></a>LayerNormGrad, LayerNormGradBeta, RmsNorm, BatchNorm, DeepNorm, GroupNorm</p></td>
<td class="cellrowborder" valign="top" width="50.019999999999996%" headers="mcps1.2.3.1.2 "><p id="p1867013611278"><a name="p1867013611278"></a><a name="p1867013611278"></a>Compatible with the 2201 architecture.</p></td>
</tr>
<tr id="row112943474533"><td class="cellrowborder" valign="top" width="49.980000000000004%" headers="mcps1.2.3.1.1 "><p id="p429474712536"><a name="p429474712536"></a><a name="p429474712536"></a>Normalize, WelfordUpdate</p></td>
<td class="cellrowborder" valign="top" width="50.019999999999996%" headers="mcps1.2.3.1.2 "><p id="p38904912278"><a name="p38904912278"></a><a name="p38904912278"></a>Compatible with the 2201 architecture.</p>
<p id="p889110902716"><a name="p889110902716"></a><a name="p889110902716"></a>Extended support for the bfloat16_t data type.</p></td>
</tr>
<tr id="row15971925165311"><td class="cellrowborder" valign="top" width="49.980000000000004%" headers="mcps1.2.3.1.1 "><p id="p1059711257536"><a name="p1059711257536"></a><a name="p1059711257536"></a>LayerNorm</p></td>
<td class="cellrowborder" valign="top" width="50.019999999999996%" headers="mcps1.2.3.1.2 "><p id="p552213132278"><a name="p552213132278"></a><a name="p552213132278"></a>Compatible with the 2201 architecture.</p>
<p id="p14522151322717"><a name="p14522151322717"></a><a name="p14522151322717"></a>Extended support for variance calculation.</p></td>
</tr>
<tr id="row2090423941218"><td class="cellrowborder" valign="top" width="49.980000000000004%" headers="mcps1.2.3.1.1 "><p id="p49051539141214"><a name="p49051539141214"></a><a name="p49051539141214"></a>WelfordFinalize</p></td>
<td class="cellrowborder" valign="top" width="50.019999999999996%" headers="mcps1.2.3.1.2 "><p id="p107681713273"><a name="p107681713273"></a><a name="p107681713273"></a>Compatible with the 2201 architecture.</p>
<p id="p37631712716"><a name="p37631712716"></a><a name="p37631712716"></a>Extended support for algorithm configuration, specifying via template parameters whether to use a correction coefficient when computing variance.</p></td>
</tr>
</tbody>
</table>

**Table 5**  Quantization operations

<a name="table432310426394"></a>
<table><thead align="left"><tr id="row18324164212392"><th class="cellrowborder" valign="top" width="33.95%" id="mcps1.2.3.1.1"><p id="p1247120164516"><a name="p1247120164516"></a><a name="p1247120164516"></a>AscendC Advanced API</p></th>
<th class="cellrowborder" valign="top" width="66.05%" id="mcps1.2.3.1.2"><p id="p1247130184515"><a name="p1247130184515"></a><a name="p1247130184515"></a>Compatibility Description</p></th>
</tr>
</thead>
<tbody><tr id="row832474216394"><td class="cellrowborder" valign="top" width="33.95%" headers="mcps1.2.3.1.1 "><p id="p3324342193912"><a name="p3324342193912"></a><a name="p3324342193912"></a>AscendQuant</p></td>
<td class="cellrowborder" valign="top" width="66.05%" headers="mcps1.2.3.1.2 "><p id="p1857162972713"><a name="p1857162972713"></a><a name="p1857162972713"></a>Compatible with the 2201 architecture.</p>
<p id="p3571729132712"><a name="p3571729132712"></a><a name="p3571729132712"></a>Extended support for PRE_TOKEN quantization and PRE_GROUP quantization.</p>
<p id="p55711294275"><a name="p55711294275"></a><a name="p55711294275"></a>Extended support for quantization from half, bfloat16_t, and float types to fp8_e5m2_t, fp8_e4m3fn_t, hifloat8_t, and int8_t types.</p>
<p id="p35712291278"><a name="p35712291278"></a><a name="p35712291278"></a>Extended support for quantization from half and bfloat16_t types to fp4x2_e1m2_t and fp4x2_e2m1_t types.</p></td>
</tr>
<tr id="row5805358162412"><td class="cellrowborder" valign="top" width="33.95%" headers="mcps1.2.3.1.1 "><p id="p1580511586248"><a name="p1580511586248"></a><a name="p1580511586248"></a>AscendDequant</p></td>
<td class="cellrowborder" valign="top" width="66.05%" headers="mcps1.2.3.1.2 "><p id="p183091632112712"><a name="p183091632112712"></a><a name="p183091632112712"></a>Compatible with the 2201 architecture.</p>
<p id="p130963212719"><a name="p130963212719"></a><a name="p130963212719"></a>Extended support for PRE_TOKEN quantization and PRE_GROUP quantization.</p>
<p id="p1309832162718"><a name="p1309832162718"></a><a name="p1309832162718"></a>Extended support for dequantization from the int32_t type to half, bfloat16_t, and float types.</p>
<p id="p13309532182716"><a name="p13309532182716"></a><a name="p13309532182716"></a>Extended support for dequantization from the float type to half, bfloat16_t, and float types.</p></td>
</tr>
<tr id="row7298212142918"><td class="cellrowborder" valign="top" width="33.95%" headers="mcps1.2.3.1.1 "><p id="p32981612142912"><a name="p32981612142912"></a><a name="p32981612142912"></a>AscendAntiQuant</p></td>
<td class="cellrowborder" valign="top" width="66.05%" headers="mcps1.2.3.1.2 "><p id="p568413356278"><a name="p568413356278"></a><a name="p568413356278"></a>Compatible with the 2201 architecture.</p>
<p id="p4684435112711"><a name="p4684435112711"></a><a name="p4684435112711"></a>Extended support for PRE_TOKEN quantization and PRE_GROUP quantization.</p>
<p id="p268443516276"><a name="p268443516276"></a><a name="p268443516276"></a>Extended support for pseudo-quantization from the int8_t, hifloat8_t, fp8_e5m2_t, and fp8_e4m3fn_t types to the half, bfloat16_t, float, and half types.</p>
<p id="p9684153518276"><a name="p9684153518276"></a><a name="p9684153518276"></a>Extended support for pseudo-quantization from the fp4x2_e1m2_t and fp4x2_e2m1_t types to the half and bfloat16_t types.</p></td>
</tr>
</tbody>
</table>

**Table 6**  Reduction operations

<a name="table17561457193915"></a>
<table><thead align="left"><tr id="row8756357163920"><th class="cellrowborder" valign="top" width="60.709999999999994%" id="mcps1.2.3.1.1"><p id="p102601213455"><a name="p102601213455"></a><a name="p102601213455"></a>AscendC Advanced API</p></th>
<th class="cellrowborder" valign="top" width="39.290000000000006%" id="mcps1.2.3.1.2"><p id="p726010113457"><a name="p726010113457"></a><a name="p726010113457"></a>Compatibility Description</p></th>
</tr>
</thead>
<tbody><tr id="row17756175716395"><td class="cellrowborder" valign="top" width="60.709999999999994%" headers="mcps1.2.3.1.1 "><p id="p14756125703918"><a name="p14756125703918"></a><a name="p14756125703918"></a>Sum, Mean, ReduceXorSum, ReduceMean, ReduceAny, ReduceAll, ReduceProd</p></td>
<td class="cellrowborder" valign="top" width="39.290000000000006%" headers="mcps1.2.3.1.2 "><p id="p137995122818"><a name="p137995122818"></a><a name="p137995122818"></a>Compatible with the 2201 architecture.</p></td>
</tr>
<tr id="row1987916211103"><td class="cellrowborder" valign="top" width="60.709999999999994%" headers="mcps1.2.3.1.1 "><p id="p58799211109"><a name="p58799211109"></a><a name="p58799211109"></a>ReduceSum</p></td>
<td class="cellrowborder" valign="top" width="39.290000000000006%" headers="mcps1.2.3.1.2 "><p id="p20285132815"><a name="p20285132815"></a><a name="p20285132815"></a>Compatible with the 2201 architecture.</p>
<p id="p1620515280"><a name="p1620515280"></a><a name="p1620515280"></a>Extended support for the int32_t, uint32_t, int64_t, and uint64_t data types.</p></td>
</tr>
<tr id="row788583213012"><td class="cellrowborder" valign="top" width="60.709999999999994%" headers="mcps1.2.3.1.1 "><p id="p12886532504"><a name="p12886532504"></a><a name="p12886532504"></a>ReduceMax, ReduceMin</p></td>
<td class="cellrowborder" valign="top" width="39.290000000000006%" headers="mcps1.2.3.1.2 "><p id="p172162050162817"><a name="p172162050162817"></a><a name="p172162050162817"></a>Compatible with the 2201 architecture.</p>
<p id="p1321615018283"><a name="p1321615018283"></a><a name="p1321615018283"></a>Extended support for the int8_t, uint8_t, int16_t, uint16_t, bfloat16_t, int32_t, uint32_t, int64_t, and uint64_t data types.</p></td>
</tr>
</tbody>
</table>

**Table 7**  Sorting operations

<a name="table4855685408"></a>
<table><thead align="left"><tr id="row78561987407"><th class="cellrowborder" valign="top" width="50.33%" id="mcps1.2.3.1.1"><p id="p18599528455"><a name="p18599528455"></a><a name="p18599528455"></a>AscendC Advanced API</p></th>
<th class="cellrowborder" valign="top" width="49.669999999999995%" id="mcps1.2.3.1.2"><p id="p115995284510"><a name="p115995284510"></a><a name="p115995284510"></a>Compatibility Description</p></th>
</tr>
</thead>
<tbody><tr id="row1588902418535"><td class="cellrowborder" valign="top" width="50.33%" headers="mcps1.2.3.1.1 "><p id="p1477514835513"><a name="p1477514835513"></a><a name="p1477514835513"></a>Concat, Extract, GetSortOffset, GetSortLen, MrgSort</p></td>
<td class="cellrowborder" valign="top" width="49.669999999999995%" headers="mcps1.2.3.1.2 "><p id="p149522418295"><a name="p149522418295"></a><a name="p149522418295"></a>Compatible with the 2201 architecture.</p></td>
</tr>
<tr id="row1685613814404"><td class="cellrowborder" valign="top" width="50.33%" headers="mcps1.2.3.1.1 "><p id="p188564818400"><a name="p188564818400"></a><a name="p188564818400"></a>TopK</p></td>
<td class="cellrowborder" valign="top" width="49.669999999999995%" headers="mcps1.2.3.1.2 "><p id="p159901844132918"><a name="p159901844132918"></a><a name="p159901844132918"></a>Compatible with the 2201 architecture.</p>
<p id="p2990544112919"><a name="p2990544112919"></a><a name="p2990544112919"></a>When the RADIX_SELECT algorithm is used, extended support for the uint8_t, int8_t, uint16_t, int16_t, uint32_t, int32_t, bfloat16_t, uint64_t, and int64_t data types.</p></td>
</tr>
<tr id="row13201552343"><td class="cellrowborder" valign="top" width="50.33%" headers="mcps1.2.3.1.1 "><p id="p232075219413"><a name="p232075219413"></a><a name="p232075219413"></a>Sort</p></td>
<td class="cellrowborder" valign="top" width="49.669999999999995%" headers="mcps1.2.3.1.2 "><p id="p1561748172917"><a name="p1561748172917"></a><a name="p1561748172917"></a>Compatible with the 2201 architecture.</p>
<p id="p261144822913"><a name="p261144822913"></a><a name="p261144822913"></a>Extended support for algorithm configuration, specifying the sorting algorithm and descending or ascending order via template parameters.</p></td>
</tr>
</tbody>
</table>

**Table 8**  Index computation

<a name="table748063194016"></a>
<table><thead align="left"><tr id="row8480531134011"><th class="cellrowborder" valign="top" width="50%" id="mcps1.2.3.1.1"><p id="p1944815184518"><a name="p1944815184518"></a><a name="p1944815184518"></a>AscendC Advanced API</p></th>
<th class="cellrowborder" valign="top" width="50%" id="mcps1.2.3.1.2"><p id="p1244845124510"><a name="p1244845124510"></a><a name="p1244845124510"></a>Compatibility Description</p></th>
</tr>
</thead>
<tbody><tr id="row2037971217519"><td class="cellrowborder" valign="top" width="50%" headers="mcps1.2.3.1.1 "><p id="p10379171220517"><a name="p10379171220517"></a><a name="p10379171220517"></a>Arange</p></td>
<td class="cellrowborder" valign="top" width="50%" headers="mcps1.2.3.1.2 "><p id="p296915535299"><a name="p296915535299"></a><a name="p296915535299"></a>Compatible with the 2201 architecture.</p>
<p id="p18969653152915"><a name="p18969653152915"></a><a name="p18969653152915"></a>Extended support for the int64_t data type.</p></td>
</tr>
</tbody>
</table>

**Table 9**  Data filtering

<a name="table1937505494013"></a>
<table><thead align="left"><tr id="row03769549406"><th class="cellrowborder" valign="top" width="50%" id="mcps1.2.3.1.1"><p id="p182236817450"><a name="p182236817450"></a><a name="p182236817450"></a>AscendC Advanced API</p></th>
<th class="cellrowborder" valign="top" width="50%" id="mcps1.2.3.1.2"><p id="p422388174510"><a name="p422388174510"></a><a name="p422388174510"></a>Compatibility Description</p></th>
</tr>
</thead>
<tbody><tr id="row1453117371552"><td class="cellrowborder" valign="top" width="50%" headers="mcps1.2.3.1.1 "><p id="p107967415400"><a name="p107967415400"></a><a name="p107967415400"></a>Select</p></td>
<td class="cellrowborder" valign="top" width="50%" headers="mcps1.2.3.1.2 "><p id="p9351156202918"><a name="p9351156202918"></a><a name="p9351156202918"></a>Compatible with the 2201 architecture.</p></td>
</tr>
<tr id="row737675474012"><td class="cellrowborder" valign="top" width="50%" headers="mcps1.2.3.1.1 "><p id="p337645474010"><a name="p337645474010"></a><a name="p337645474010"></a>DropOut</p></td>
<td class="cellrowborder" valign="top" width="50%" headers="mcps1.2.3.1.2 "><p id="p15264155902914"><a name="p15264155902914"></a><a name="p15264155902914"></a>Compatible with the 2201 architecture.</p>
<p id="p72651859162914"><a name="p72651859162914"></a><a name="p72651859162914"></a>Extended support for the bfloat16_t data type.</p></td>
</tr>
</tbody>
</table>

**Table 10**  Tensor transformation

<a name="table68071919174115"></a>
<table><thead align="left"><tr id="row1880713195411"><th class="cellrowborder" valign="top" width="50%" id="mcps1.2.3.1.1"><p id="p2655161904519"><a name="p2655161904519"></a><a name="p2655161904519"></a>AscendC Advanced API</p></th>
<th class="cellrowborder" valign="top" width="50%" id="mcps1.2.3.1.2"><p id="p14655719124519"><a name="p14655719124519"></a><a name="p14655719124519"></a>Compatibility Description</p></th>
</tr>
</thead>
<tbody><tr id="row118071194413"><td class="cellrowborder" valign="top" width="50%" headers="mcps1.2.3.1.1 "><p id="p230212473585"><a name="p230212473585"></a><a name="p230212473585"></a>Transpose</p></td>
<td class="cellrowborder" valign="top" width="50%" headers="mcps1.2.3.1.2 "><p id="p183391064304"><a name="p183391064304"></a><a name="p183391064304"></a>Compatible with the 2201 architecture.</p>
<div class="p" id="p9339126143016"><a name="p9339126143016"></a><a name="p9339126143016"></a>Newly supports data layout transformation scenarios:<a name="ul19819193610813"></a><a name="ul19819193610813"></a><ul id="ul19819193610813"><li>2D transpose or transpose of the last two dimensions of 3D.</li><li>Swapping the first and second dimensions of 3D.</li><li>Swapping the first and third dimensions of 3D.</li><li>Transposing 2D ND2NZ using interleaving instructions.</li></ul>
</div></td>
</tr>
<tr id="row8745113972818"><td class="cellrowborder" valign="top" width="50%" headers="mcps1.2.3.1.1 "><p id="p47451139182817"><a name="p47451139182817"></a><a name="p47451139182817"></a>TransData, Pad, UnPad</p></td>
<td class="cellrowborder" valign="top" width="50%" headers="mcps1.2.3.1.2 "><p id="p69934108302"><a name="p69934108302"></a><a name="p69934108302"></a>Compatible with the 2201 architecture.</p></td>
</tr>
<tr id="row2715221"><td class="cellrowborder" valign="top" width="50%" headers="mcps1.2.3.1.1 "><p id="p72101361211"><a name="p72101361211"></a><a name="p72101361211"></a>BroadCast</p></td>
<td class="cellrowborder" valign="top" width="50%" headers="mcps1.2.3.1.2 "><p id="p17188014143015"><a name="p17188014143015"></a><a name="p17188014143015"></a>Compatible with the 2201 architecture.</p>
<p id="p1918821433011"><a name="p1918821433011"></a><a name="p1918821433011"></a>Extended support for dynamic Shape.</p>
<p id="p121881814143019"><a name="p121881814143019"></a><a name="p121881814143019"></a>Extended support for the int16_t, uint16_t, bfloat16_t, int32_t, and uint32_t data types.</p></td>
</tr>
<tr id="row386815398281"><td class="cellrowborder" valign="top" width="50%" headers="mcps1.2.3.1.1 "><p id="p2942213619"><a name="p2942213619"></a><a name="p2942213619"></a>Fill</p></td>
<td class="cellrowborder" valign="top" width="50%" headers="mcps1.2.3.1.2 "><p id="p1758771711301"><a name="p1758771711301"></a><a name="p1758771711301"></a>Compatible with the 2201 architecture.</p>
<p id="p12588191716305"><a name="p12588191716305"></a><a name="p12588191716305"></a>Extended support for the uint8_t, int8_t, bfloat16_t, uint64_t, and int64_t data types.</p></td>
</tr>
</tbody>
</table>

**Table 11**  Hccl

<a name="table12265143614414"></a>
<table><thead align="left"><tr id="row1126583611410"><th class="cellrowborder" valign="top" width="43.35%" id="mcps1.2.3.1.1"><p id="p16138821104518"><a name="p16138821104518"></a><a name="p16138821104518"></a>AscendC Advanced API</p></th>
<th class="cellrowborder" valign="top" width="56.65%" id="mcps1.2.3.1.2"><p id="p61381215456"><a name="p61381215456"></a><a name="p61381215456"></a>Compatibility Description</p></th>
</tr>
</thead>
<tbody><tr id="row1326563616413"><td class="cellrowborder" valign="top" width="43.35%" headers="mcps1.2.3.1.1 "><p id="p52651936104112"><a name="p52651936104112"></a><a name="p52651936104112"></a>Hccl template parameters</p></td>
<td class="cellrowborder" valign="top" width="56.65%" headers="mcps1.2.3.1.2 "><p id="p1947910612319"><a name="p1947910612319"></a><a name="p1947910612319"></a>Supports the HCCL_SERVER_TYPE_CCU server type.</p></td>
</tr>
<tr id="row1667811121053"><td class="cellrowborder" valign="top" width="43.35%" headers="mcps1.2.3.1.1 "><p id="p1886335314511"><a name="p1886335314511"></a><a name="p1886335314511"></a>InitV2, SetCcTilingV2, AllReduce, AllGather, ReduceScatter, AlltoAll, AlltoAllV, Commit, Wait, Finalize</p>
<p id="p1939872751318"><a name="p1939872751318"></a><a name="p1939872751318"></a></p></td>
<td class="cellrowborder" valign="top" width="56.65%" headers="mcps1.2.3.1.2 "><p id="p235608173114"><a name="p235608173114"></a><a name="p235608173114"></a>Compatible with the 2201 architecture.</p></td>
</tr>
<tr id="row18338107564"><td class="cellrowborder" valign="top" width="43.35%" headers="mcps1.2.3.1.1 "><p id="p63391571160"><a name="p63391571160"></a><a name="p63391571160"></a>BatchWrite, iterate, Query, InterHcclGroupSync, GetWindowsInAddr, GetWindowsOutAddr, GetRankId, GetRankDim, QueueBarrier, GetQueueNum</p></td>
<td class="cellrowborder" valign="top" width="56.65%" headers="mcps1.2.3.1.2 "><p id="p116241033113"><a name="p116241033113"></a><a name="p116241033113"></a>Not supported by the 3510 architecture yet.</p></td>
</tr>
<tr id="row1135194142018"><td class="cellrowborder" valign="top" width="43.35%" headers="mcps1.2.3.1.1 "><p id="p15351141142016"><a name="p15351141142016"></a><a name="p15351141142016"></a>SetReduceType, AlltoAllvWrite</p></td>
<td class="cellrowborder" valign="top" width="56.65%" headers="mcps1.2.3.1.2 "><p id="p23911012153113"><a name="p23911012153113"></a><a name="p23911012153113"></a>Architecture addition in 3510.</p></td>
</tr>
</tbody>
</table>
