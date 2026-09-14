# How to Use the Mask Operation API<a name="ZH-CN_TOPIC_0000002305974105"></a>

<!-- md-trans-meta sourceCommit=e3c8c70b3f5d27f3770063ae4b535bc75d904e3e translatedAt=2026-08-26T12:24:24.733Z pushedAt=2026-09-03T12:28:28.831Z -->

Mask is used to control the number of elements involved in vector computation. It supports the following working modes and configuration methods:

**Table 1** Mask working modes

<a name="zh-cn_topic_0000002267504584_table414483923116"></a>
<table><thead align="left"><tr id="zh-cn_topic_0000002267504584_row51452039163114"><th class="cellrowborder" valign="top" width="14.14%" id="mcps1.2.3.1.1"><p id="zh-cn_topic_0000002267504584_p814513911319"><a name="zh-cn_topic_0000002267504584_p814513911319"></a><a name="zh-cn_topic_0000002267504584_p814513911319"></a>Working Mode</p></th>
<th class="cellrowborder" valign="top" width="85.86%" id="mcps1.2.3.1.2"><p id="zh-cn_topic_0000002267504584_p14145239103115"><a name="zh-cn_topic_0000002267504584_p14145239103115"></a><a name="zh-cn_topic_0000002267504584_p14145239103115"></a>Description</p></th>
</tr>
</thead>
<tbody><tr id="zh-cn_topic_0000002267504584_row11145143963110"><td class="cellrowborder" valign="top" width="14.14%" headers="mcps1.2.3.1.1 "><p id="zh-cn_topic_0000002267504584_p614518396314"><a name="zh-cn_topic_0000002267504584_p614518396314"></a><a name="zh-cn_topic_0000002267504584_p614518396314"></a>Normal mode</p></td>
<td class="cellrowborder" valign="top" width="85.86%" headers="mcps1.2.3.1.2 "><p id="zh-cn_topic_0000002267504584_p770453072014"><a name="zh-cn_topic_0000002267504584_p770453072014"></a><a name="zh-cn_topic_0000002267504584_p770453072014"></a>The default mode. It supports the Mask capability within a single iteration. Developers need to configure the number of iterations and additionally compute the tail block.</p>
<p id="zh-cn_topic_0000002267504584_p1584693592814"><a name="zh-cn_topic_0000002267504584_p1584693592814"></a><a name="zh-cn_topic_0000002267504584_p1584693592814"></a><strong id="zh-cn_topic_0000002267504584_b277175602011"><a name="zh-cn_topic_0000002267504584_b277175602011"></a><a name="zh-cn_topic_0000002267504584_b277175602011"></a>In Normal mode, Mask is used to control the number of elements involved in computation within a single iteration.</strong></p>
<p id="zh-cn_topic_0000002267504584_p172203193391"><a name="zh-cn_topic_0000002267504584_p172203193391"></a><a name="zh-cn_topic_0000002267504584_p172203193391"></a>Call SetMaskNorm to set the Normal mode.</p></td>
</tr>
<tr id="zh-cn_topic_0000002267504584_row61458393313"><td class="cellrowborder" valign="top" width="14.14%" headers="mcps1.2.3.1.1 "><p id="zh-cn_topic_0000002267504584_p914513918311"><a name="zh-cn_topic_0000002267504584_p914513918311"></a><a name="zh-cn_topic_0000002267504584_p914513918311"></a>Counter mode</p></td>
<td class="cellrowborder" valign="top" width="85.86%" headers="mcps1.2.3.1.2 "><p id="zh-cn_topic_0000002267504584_p376310417215"><a name="zh-cn_topic_0000002267504584_p376310417215"></a><a name="zh-cn_topic_0000002267504584_p376310417215"></a>A simplified mode. It directly passes the amount of data to be computed, <span>automatically infers the number of iterations,</span> and does not require developers to perceive the number of iterations or handle non-aligned tail blocks. However, it does not support the Mask capability within a single iteration.</p>
<p id="zh-cn_topic_0000002267504584_p714520394314"><a name="zh-cn_topic_0000002267504584_p714520394314"></a><a name="zh-cn_topic_0000002267504584_p714520394314"></a><strong id="zh-cn_topic_0000002267504584_b113012902110"><a name="zh-cn_topic_0000002267504584_b113012902110"></a><a name="zh-cn_topic_0000002267504584_b113012902110"></a>In Counter mode, Mask indicates the number of elements involved in the entire vector computation.</strong></p>
<p id="zh-cn_topic_0000002267504584_p17014342110"><a name="zh-cn_topic_0000002267504584_p17014342110"></a><a name="zh-cn_topic_0000002267504584_p17014342110"></a>Call SetMaskCount to set the Counter mode.</p></td>
</tr>
</tbody>
</table>

**Table 2** Mask configuration methods

<a name="zh-cn_topic_0000002267504584_table642464733119"></a>
<table><thead align="left"><tr id="zh-cn_topic_0000002267504584_row8425134710316"><th class="cellrowborder" valign="top" width="14.12%" id="mcps1.2.3.1.1"><p id="zh-cn_topic_0000002267504584_p194258470311"><a name="zh-cn_topic_0000002267504584_p194258470311"></a><a name="zh-cn_topic_0000002267504584_p194258470311"></a>Configuration Method</p></th>
<th class="cellrowborder" valign="top" width="85.88%" id="mcps1.2.3.1.2"><p id="zh-cn_topic_0000002267504584_p442513473313"><a name="zh-cn_topic_0000002267504584_p442513473313"></a><a name="zh-cn_topic_0000002267504584_p442513473313"></a>Description</p></th>
</tr>
</thead>
<tbody><tr id="zh-cn_topic_0000002267504584_row9425114710316"><td class="cellrowborder" valign="top" width="14.12%" headers="mcps1.2.3.1.1 "><p id="zh-cn_topic_0000002267504584_p2425847103118"><a name="zh-cn_topic_0000002267504584_p2425847103118"></a><a name="zh-cn_topic_0000002267504584_p2425847103118"></a>API parameter passing (default)</p></td>
<td class="cellrowborder" valign="top" width="85.88%" headers="mcps1.2.3.1.2 "><p id="zh-cn_topic_0000002267504584_p1342514712318"><a name="zh-cn_topic_0000002267504584_p1342514712318"></a><a name="zh-cn_topic_0000002267504584_p1342514712318"></a><span>The Mask value is passed directly through the input parameter of the vector computation API. The</span> template parameter isSetMask (supported by only some APIs) of the vector computation API controls whether to use API parameter passing or external API configuration. The default value is true, indicating API parameter passing. <span>The Mask corresponds to the mask/mask[] parameter in the high-dimensional split computation API or the calCount parameter in the tensor first-n data computation API.</span></p></td>
</tr>
<tr id="zh-cn_topic_0000002267504584_row1425124783112"><td class="cellrowborder" valign="top" width="14.12%" headers="mcps1.2.3.1.1 "><p id="zh-cn_topic_0000002267504584_p114251471310"><a name="zh-cn_topic_0000002267504584_p114251471310"></a><a name="zh-cn_topic_0000002267504584_p114251471310"></a>External API configuration</p></td>
<td class="cellrowborder" valign="top" width="85.88%" headers="mcps1.2.3.1.2 "><p id="zh-cn_topic_0000002267504584_p84251647103119"><a name="zh-cn_topic_0000002267504584_p84251647103119"></a><a name="zh-cn_topic_0000002267504584_p84251647103119"></a><span>Call the</span> SetVectorMask <span>API to set the Mask value. Set the template parameter isSetMask of the vector computation API to false. The Mask parameter in the API input parameter (corresponding to the mask/mask[] parameter in the high-dimensional sharding computation API or the calCount parameter in the tensor first-n data computation API)</span> does not take effect. This method applies to scenarios where the Mask parameter is the same and is reused multiple times. It eliminates the need to repeatedly set the Mask inside the vector computation API, providing a certain performance advantage.</p></td>
</tr>
</tbody>
</table>

The mask operation is used as follows:

**Table 3** Usage of mask operations

<a name="zh-cn_topic_0000002267504584_table1957843418427"></a>
<table><thead align="left"><tr id="zh-cn_topic_0000002267504584_row657817341420"><th class="cellrowborder" valign="top" width="8.58%" id="mcps1.2.5.1.1"><p id="zh-cn_topic_0000002267504584_p19578634194213"><a name="zh-cn_topic_0000002267504584_p19578634194213"></a><a name="zh-cn_topic_0000002267504584_p19578634194213"></a>Configuration Method</p>
</th>
<th class="cellrowborder" valign="top" width="8.37%" id="mcps1.2.5.1.2"><p id="zh-cn_topic_0000002267504584_p165791734184211"><a name="zh-cn_topic_0000002267504584_p165791734184211"></a><a name="zh-cn_topic_0000002267504584_p165791734184211"></a>Working Mode</p>
</th>
<th class="cellrowborder" valign="top" width="35.67%" id="mcps1.2.5.1.3"><p id="zh-cn_topic_0000002267504584_p1757923411421"><a name="zh-cn_topic_0000002267504584_p1757923411421"></a><a name="zh-cn_topic_0000002267504584_p1757923411421"></a>First-n data computation API</p>
</th>
<th class="cellrowborder" valign="top" width="47.38%" id="mcps1.2.5.1.4"><p id="zh-cn_topic_0000002267504584_p1257915343423"><a name="zh-cn_topic_0000002267504584_p1257915343423"></a><a name="zh-cn_topic_0000002267504584_p1257915343423"></a>High-dimensional sharding computation API</p>
</th>
</tr>
</thead>
<tbody><tr id="zh-cn_topic_0000002267504584_row15792034144218"><td class="cellrowborder" rowspan="2" valign="top" width="8.58%" headers="mcps1.2.5.1.1 "><p id="zh-cn_topic_0000002267504584_p1457918343421"><a name="zh-cn_topic_0000002267504584_p1457918343421"></a><a name="zh-cn_topic_0000002267504584_p1457918343421"></a>API parameter passing</p>
</td>
<td class="cellrowborder" valign="top" width="8.37%" headers="mcps1.2.5.1.2 "><p id="zh-cn_topic_0000002267504584_p1157917346420"><a name="zh-cn_topic_0000002267504584_p1157917346420"></a><a name="zh-cn_topic_0000002267504584_p1157917346420"></a>Normal mode</p>
</td>
<td class="cellrowborder" valign="top" width="35.67%" headers="mcps1.2.5.1.3 "><p id="zh-cn_topic_0000002267504584_p35791034104218"><a name="zh-cn_topic_0000002267504584_p35791034104218"></a><a name="zh-cn_topic_0000002267504584_p35791034104218"></a>Not involved.</p>
</td>
<td class="cellrowborder" valign="top" width="47.38%" headers="mcps1.2.5.1.4 "><p id="zh-cn_topic_0000002267504584_p057913416421"><a name="zh-cn_topic_0000002267504584_p057913416421"></a><a name="zh-cn_topic_0000002267504584_p057913416421"></a>Set the isSetMask template parameter to true, pass the Mask through the API input parameter, and configure the <span id="zh-cn_topic_0000002267504584_ph1657918349424"><a name="zh-cn_topic_0000002267504584_ph1657918349424"></a><a name="zh-cn_topic_0000002267504584_ph1657918349424"></a>dataBlockStride</span>, <span id="zh-cn_topic_0000002267504584_ph1557911348429"><a name="zh-cn_topic_0000002267504584_ph1557911348429"></a><a name="zh-cn_topic_0000002267504584_ph1557911348429"></a>repeatStride</span><span>, </span><span>repeatTime</span> parameters based on the scenario.</p>
</td>
</tr>
<tr id="zh-cn_topic_0000002267504584_row55792345422"><td class="cellrowborder" valign="top" headers="mcps1.2.5.1.1 "><p id="zh-cn_topic_0000002267504584_p7579534184213"><a name="zh-cn_topic_0000002267504584_p7579534184213"></a><a name="zh-cn_topic_0000002267504584_p7579534184213"></a>Counter mode</p>
</td>
<td class="cellrowborder" valign="top" headers="mcps1.2.5.1.2 "><p id="zh-cn_topic_0000002267504584_p6579534194215"><a name="zh-cn_topic_0000002267504584_p6579534194215"></a><a name="zh-cn_topic_0000002267504584_p6579534194215"></a>Set the isSetMask template parameter to true and pass the Mask through the interface input parameter.</p>
</td>
<td class="cellrowborder" valign="top" headers="mcps1.2.5.1.3 "><a name="zh-cn_topic_0000002267504584_ul257912346428"></a><a name="zh-cn_topic_0000002267504584_ul257912346428"></a><ul id="zh-cn_topic_0000002267504584_ul257912346428"><li>Set the isSetMask template parameter to true and pass the Mask through the interface input parameter.</li><li>Configure the <span id="zh-cn_topic_0000002267504584_ph0579334194210"><a name="zh-cn_topic_0000002267504584_ph0579334194210"></a><a name="zh-cn_topic_0000002267504584_ph0579334194210"></a>dataBlockStride</span> and <span id="zh-cn_topic_0000002267504584_ph135798348424"><a name="zh-cn_topic_0000002267504584_ph135798348424"></a><a name="zh-cn_topic_0000002267504584_ph135798348424"></a>repeatStride</span> parameters based on the usage scenario. Pass a fixed value to <span>repeatTime</span>; it is recommended to set it to 1, as this value does not take effect.</li></ul>
</td>
</tr>
<tr id="zh-cn_topic_0000002267504584_row158013415422"><td class="cellrowborder" rowspan="2" valign="top" width="8.58%" headers="mcps1.2.5.1.1 "><p id="zh-cn_topic_0000002267504584_p258043494218"><a name="zh-cn_topic_0000002267504584_p258043494218"></a><a name="zh-cn_topic_0000002267504584_p258043494218"></a>External API configuration</p>
</td>
<td class="cellrowborder" valign="top" width="8.37%" headers="mcps1.2.5.1.2 "><p id="zh-cn_topic_0000002267504584_p1658013346421"><a name="zh-cn_topic_0000002267504584_p1658013346421"></a><a name="zh-cn_topic_0000002267504584_p1658013346421"></a>Normal mode</p>
</td>
<td class="cellrowborder" valign="top" width="35.67%" headers="mcps1.2.5.1.3 "><p id="zh-cn_topic_0000002267504584_p1958073434215"><a name="zh-cn_topic_0000002267504584_p1958073434215"></a><a name="zh-cn_topic_0000002267504584_p1958073434215"></a>Not involved.</p>
</td>
<td class="cellrowborder" valign="top" width="47.38%" headers="mcps1.2.5.1.4 "><div class="p" id="zh-cn_topic_0000002267504584_p1699153514480"><a name="zh-cn_topic_0000002267504584_p1699153514480"></a><a name="zh-cn_topic_0000002267504584_p1699153514480"></a>Call SetVectorMask to set the Mask, then call the high-dimensional split computation API.<a name="zh-cn_topic_0000002267504584_ul85801834154214"></a><a name="zh-cn_topic_0000002267504584_ul85801834154214"></a><ul id="zh-cn_topic_0000002267504584_ul85801834154214"><li>Set the isSetMask template parameter to false and set the mask value in the interface input parameter to the placeholder MASK_PLACEHOLDER, which is used only as a placeholder and has no actual meaning.</li><li>Configure the <span>repeatTime</span>, <span id="zh-cn_topic_0000002267504584_ph1658013347423"><a name="zh-cn_topic_0000002267504584_ph1658013347423"></a><a name="zh-cn_topic_0000002267504584_ph1658013347423"></a>dataBlockStride</span>, and <span id="zh-cn_topic_0000002267504584_ph2580133410427"><a name="zh-cn_topic_0000002267504584_ph2580133410427"></a><a name="zh-cn_topic_0000002267504584_ph2580133410427"></a>repeatStride</span> parameters based on the scenario.</li></ul>
</div>
</td>
</tr>
<tr id="zh-cn_topic_0000002267504584_row55801234104210"><td class="cellrowborder" valign="top" headers="mcps1.2.5.1.1 "><p id="zh-cn_topic_0000002267504584_p75801034144212"><a name="zh-cn_topic_0000002267504584_p75801034144212"></a><a name="zh-cn_topic_0000002267504584_p75801034144212"></a>Counter mode</p>
</td>
<td class="cellrowborder" valign="top" headers="mcps1.2.5.1.2 "><p id="zh-cn_topic_0000002267504584_p15281754124219"><a name="zh-cn_topic_0000002267504584_p15281754124219"></a><a name="zh-cn_topic_0000002267504584_p15281754124219"></a>Call SetVectorMask to set the Mask, then call the first-n data computation API with the isSetMask template parameter set to false. It is recommended to set calCount in the interface input parameter to 1.</p>
</td>
<td class="cellrowborder" valign="top" headers="mcps1.2.5.1.3 "><div class="p" id="zh-cn_topic_0000002267504584_p13580173410556"><a name="zh-cn_topic_0000002267504584_p13580173410556"></a><a name="zh-cn_topic_0000002267504584_p13580173410556"></a>Call SetVectorMask to set the Mask, then call the high-dimensional split computation API.<a name="zh-cn_topic_0000002267504584_ul11581183434214"></a><a name="zh-cn_topic_0000002267504584_ul11581183434214"></a><ul id="zh-cn_topic_0000002267504584_ul11581183434214"><li>Set the isSetMask template parameter to false and set the mask value in the interface input parameter to MASK_PLACEHOLDER, which is used only as a placeholder and has no actual meaning.</li><li>Configure the <span id="zh-cn_topic_0000002267504584_ph458183417420"><a name="zh-cn_topic_0000002267504584_ph458183417420"></a><a name="zh-cn_topic_0000002267504584_ph458183417420"></a>dataBlockStride</span> and <span id="zh-cn_topic_0000002267504584_ph105811034164217"><a name="zh-cn_topic_0000002267504584_ph105811034164217"></a><a name="zh-cn_topic_0000002267504584_ph105811034164217"></a>repeatStride</span> parameters based on the scenario. Pass a fixed value to <span>repeatTime</span>; it is recommended to set it to 1, as this value does not take effect.</li></ul>
</div>
</td>
</tr>
</tbody>
</table>

The following shows usage examples of typical scenarios:

-   Scenario 1: Normal mode + external API configuration + high-dimensional split computation API

    ```
    AscendC::LocalTensor<half> dstLocal;
    AscendC::LocalTensor<half> src0Local;
    AscendC::LocalTensor<half> src1Local;
    
    // 1. Set Normal mode
    AscendC::SetMaskNorm();
    // 2. Set Mask
    AscendC::SetVectorMask<half, AscendC::MaskMode::NORMAL>(0xffffffffffffffff, 0xffffffffffffffff);  // bit-by-bit mode
    // SetVectorMask<half, MaskMode::NORMAL>(128);  // continuous mode
    
    // 3. Call the vector computation API multiple times. Set the isSetMask template parameter to false and set the mask value in the interface input parameter to the placeholder MASK_PLACEHOLDER, which is used only as a placeholder and has no actual meaning.
    // Configure the repeatTime, dataBlockStride, and repeatStride parameters based on the scenario.
    // dstBlkStride, src0BlkStride, src1BlkStride = 1, data is read and written continuously within a single iteration.
    // dstRepStride, src0RepStride, src1RepStride = 8, data is read and written continuously between adjacent iterations.
    AscendC::Add<half, false>(dstLocal, src0Local, src1Local, AscendC::MASK_PLACEHOLDER, 1, { 2, 2, 2, 8, 8, 8 });
    AscendC::Sub<half, false>(src0Local, dstLocal, src1Local, AscendC::MASK_PLACEHOLDER, 1, { 2, 2, 2, 8, 8, 8 });
    AscendC::Mul<half, false>(src1Local, dstLocal, src0Local, AscendC::MASK_PLACEHOLDER, 1, { 2, 2, 2, 8, 8, 8 });
    // 4. Restore the Mask value to the default value.
    AscendC::ResetMask();
    ```

-   Scenario 2: Counter mode + external API configuration + high-dimensional split computation API

    ```
    AscendC::LocalTensor<half> dstLocal;
    AscendC::LocalTensor<half> src0Local;
    AscendC::LocalTensor<half> src1Local;
    int32_t len = 128;  // number of elements involved in the computation
    // 1. Set Counter mode
    AscendC::SetMaskCount();
    // 2. Set Mask
    AscendC::SetVectorMask<half, AscendC::MaskMode::COUNTER>(len);
    // 3. Call the vector computation API multiple times, set the isSetMask template parameter to false, and set the mask value in the interface input parameter to MASK_PLACEHOLDER, which is used as a placeholder and has no actual meaning.
    // Correctly configure the dataBlockStride and repeatStride parameters based on the usage scenario. A fixed value can be passed to repeatTime. It is recommended to set it to 1, and this value does not take effect.
    AscendC::Add<half, false>(dstLocal, src0Local, src1Local, AscendC::MASK_PLACEHOLDER, 1, { 1, 1, 1, 8, 8, 8 });
    AscendC::Sub<half, false>(src0Local, dstLocal, src1Local, AscendC::MASK_PLACEHOLDER, 1, { 1, 1, 1, 8, 8, 8 });
    AscendC::Mul<half, false>(src1Local, dstLocal, src0Local, AscendC::MASK_PLACEHOLDER, 1, { 1, 1, 1, 8, 8, 8 });
    // 4. Restore the working mode
    AscendC::SetMaskNorm();
    // 5. Restore the Mask value to the default value
    AscendC::ResetMask();
    ```

-   Scenario 3: Counter mode + external API configuration + first-n data computation API

    ```
    AscendC::LocalTensor<half> dstLocal;
    AscendC::LocalTensor<half> src0Local;
    half num = 2; 
    // 1. Set Mask
    AscendC::SetVectorMask<half, AscendC::MaskMode::COUNTER>(128); // The number of elements involved in the computation is 128
    // 2. Call the first-n data computation API, set the isSetMask template parameter to false, and set calCount in the interface input parameter to 1.
    AscendC::Adds<half, false>(dstLocal, src0Local, num, 1);
    AscendC::Muls<half, false>(dstLocal, src0Local, num, 1);
    // 3. Restore the working mode
    AscendC::SetMaskNorm();
    // 4. Restore the Mask value to the default value
    AscendC::ResetMask();
    ```

> [!NOTE]
>
> - The first-n data computation API internally sets the working mode to Counter mode. Therefore, when the first-n data computation API is used together with Counter mode, you do not need to manually call [SetMaskCount](../../../../../api/SIMD-API/basic_api/memory_vector_compute/mask_operations/SetMaskCount.md) to set Counter mode.
> - In all scenarios where Counter mode is manually used, call [SetMaskNorm](../../../../../api/SIMD-API/basic_api/memory_vector_compute/mask_operations/SetMaskNorm.md) to restore the working mode after use.
> - After calling [SetVectorMask](../../../../../api/SIMD-API/basic_api/memory_vector_compute/mask_operations/SetVectorMask.md) to set the Mask, call [ResetMask](../../../../../api/SIMD-API/basic_api/memory_vector_compute/mask_operations/ResetMask.md) to restore the Mask value to the default value after use.
> - When the high-dimensional sharding computation API is used together with Counter mode, it adds interval-based computation compared with the first-n data computation API, and supports the **dataBlockStride** and **repeatStride** parameters.
