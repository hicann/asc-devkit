# Duplicate<a name="ZH-CN_TOPIC_0000001937477638"></a>

<!-- md-trans-meta sourceCommit=530b71156971d14c92740e2950af29fc469bddf1 translatedAt=2026-08-27T22:17:28.539Z -->

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
- Atlas 200I/500 A2 inference product: Not supported
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

Both **Scalar** and **Tensor** modes are supported:

-   **Scalar** mode: Broadcasts **scalarValue** to the register and stores it in **dstReg** (if a **mask** is present, stores it in the positions of **dstReg** selected by the **mask**).
-   **Tensor** mode: Broadcasts the lowest-order element of **srcReg** to the register and stores it in the positions of **dstReg** selected by the **mask**.

## Prototype<a name="section620mcpsimp"></a>

-   Broadcast **scalarValue** to the register. Mask is not supported.

    ```
    template <typename T = DefaultType, typename U, typename S>
    __simd_callee__ inline void Duplicate(S& dstReg, U scalarValue);
    ```

-   Broadcast **scalarValue** to the register. Mask is supported.

    ```
    template <typename T = DefaultType, MaskMergeMode mode = MaskMergeMode::ZEROING, typename U, typename S>
    __simd_callee__ inline void Duplicate(S& dstReg, U scalarValue, MaskReg& mask);
    ```

-   Broadcast the lowest element of **srcReg** to the register.

    ```
    template <typename T = DefaultType, HighLowPart pos = HighLowPart::LOWEST, MaskMergeMode mode = MaskMergeMode::ZEROING, typename S>
    __simd_callee__ inline void Duplicate(S& dstReg, S& srcReg, MaskReg& mask)
    ```

## Parameters<a name="section622mcpsimp"></a>

**Table 1**  Template parameter description

<a name="table4835205712588"></a>
<table><thead align="left"><tr id="row118356578583"><th class="cellrowborder" valign="top" width="18.52%" id="mcps1.2.3.1.1"><p id="p48354572582"><a name="p48354572582"></a><a name="p48354572582"></a>Parameter</p></th>
<th class="cellrowborder" valign="top" width="81.47999999999999%" id="mcps1.2.3.1.2"><p id="p583535795817"><a name="p583535795817"></a><a name="p583535795817"></a>Description</p></th>
</tr>
</thead>
<tbody><tr id="row1835857145817"><td class="cellrowborder" valign="top" width="18.52%" headers="mcps1.2.3.1.1 "><p id="p5835457165816"><a name="p5835457165816"></a><a name="p5835457165816"></a>T</p></td>
<td class="cellrowborder" valign="top" width="81.47999999999999%" headers="mcps1.2.3.1.2 "><p id="p67818381089"><a name="p67818381089"></a><a name="p67818381089"></a>Operand data type.</p>
<p id="p1997765116286"><a name="p1997765116286"></a><a name="p1997765116286"></a><span id="ph18632195815284"><a name="ph18632195815284"></a><a name="ph18632195815284"></a>Ascend 950PR/Ascend 950DT</span>, supported data types: bool, int8_t, uint8_t, fp4x2_e2m1_t, fp4x2_e1m2_t, hifloat8_t, fp8_e5m2_t, fp8_e4m3fn_t, fp8_e8m0_t, int16_t, uint16_t, half, bfloat16_t, int32_t, uint32_t, float, complex32, int64_t, uint64_t, complex64.</p></td>
</tr>
<tr id="row137401528960"><td class="cellrowborder" valign="top" width="18.52%" headers="mcps1.2.3.1.1 "><p id="p07403281766"><a name="p07403281766"></a><a name="p07403281766"></a>U</p></td>
<td class="cellrowborder" valign="top" width="81.47999999999999%" headers="mcps1.2.3.1.2 "><p id="p18151938469"><a name="p18151938469"></a><a name="p18151938469"></a>Data type of scalar.</p>
<p id="p6804731173014"><a name="p6804731173014"></a><a name="p6804731173014"></a><span id="ph1380483112304"><a name="ph1380483112304"></a><a name="ph1380483112304"></a>Ascend 950PR/Ascend 950DT</span>, supported data types: bool, int8_t, uint8_t, int16_t, uint16_t, half, bfloat16_t, int32_t, uint32_t, float, complex32, int64_t, uint64_t, complex64.</p></td>
</tr>
<tr id="row3742113618507"><td class="cellrowborder" valign="top" width="18.52%" headers="mcps1.2.3.1.1 "><p id="p1383515717581"><a name="p1383515717581"></a><a name="p1383515717581"></a>mode</p></td>
<td class="cellrowborder" valign="top" width="81.47999999999999%" headers="mcps1.2.3.1.2 "><p id="p77520541653"><a name="p77520541653"></a><a name="p77520541653"></a>Selects the **MERGING** mode or the **ZEROING** mode.</p>
<a name="ul1163765616511"></a><a name="ul1163765616511"></a><ul id="ul1163765616511"><li>**MERGING**: Elements not filtered by **mask** retain their original values in **dst**.</li><li>**ZEROING**: Elements not filtered by **mask** are set to zero in **dst**.</li></ul></td>
</tr>
<tr id="row7678105918116"><td class="cellrowborder" valign="top" width="18.52%" headers="mcps1.2.3.1.1 "><p id="p38495211519"><a name="p38495211519"></a><a name="p38495211519"></a>**pos**</p></td>
<td class="cellrowborder" valign="top" width="81.47999999999999%" headers="mcps1.2.3.1.2 "><p id="p0678155913118"><a name="p0678155913118"></a><a name="p0678155913118"></a>Defaults to **LOWEST**, which broadcasts the lowest bit in **srcReg** to **dstReg** (other values are not supported).</p></td>
</tr>
<tr id="row16521114783914"><td class="cellrowborder" valign="top" width="18.52%" headers="mcps1.2.3.1.1 "><p id="p916243141912"><a name="p916243141912"></a><a name="p916243141912"></a>**S**</p></td>
<td class="cellrowborder" valign="top" width="81.47999999999999%" headers="mcps1.2.3.1.2 "><p id="p131047476612"><a name="p131047476612"></a><a name="p131047476612"></a>**RegTensor** type of the destination operand, for example, **RegTensor&lt;half&gt;**. It is automatically deduced by the compiler, and the user does not need to specify it.</p></td>
</tr>
</tbody>
</table>

**Table 2**  Function parameter description

<a name="zh-cn_topic_0235751031_table33761356"></a>
<table><thead align="left"><tr id="zh-cn_topic_0235751031_row27598891"><th class="cellrowborder" valign="top" width="18.54%" id="mcps1.2.4.1.1"><p id="zh-cn_topic_0235751031_p20917673"><a name="zh-cn_topic_0235751031_p20917673"></a><a name="zh-cn_topic_0235751031_p20917673"></a>Parameter</p></th>
<th class="cellrowborder" valign="top" width="10.05%" id="mcps1.2.4.1.2"><p id="zh-cn_topic_0235751031_p16609919"><a name="zh-cn_topic_0235751031_p16609919"></a><a name="zh-cn_topic_0235751031_p16609919"></a>Input/Output</p></th>
<th class="cellrowborder" valign="top" width="71.41%" id="mcps1.2.4.1.3"><p id="zh-cn_topic_0235751031_p59995477"><a name="zh-cn_topic_0235751031_p59995477"></a><a name="zh-cn_topic_0235751031_p59995477"></a>Description</p></th>
</tr>
</thead>
<tbody><tr id="row42461942101815"><td class="cellrowborder" valign="top" width="18.54%" headers="mcps1.2.4.1.1 "><p id="p284425844311"><a name="p284425844311"></a><a name="p284425844311"></a>**dstReg**</p></td>
<td class="cellrowborder" valign="top" width="10.05%" headers="mcps1.2.4.1.2 "><p id="p158449584436"><a name="p158449584436"></a><a name="p158449584436"></a>Output</p></td>
<td class="cellrowborder" valign="top" width="71.41%" headers="mcps1.2.4.1.3 "><p id="p6389114616514"><a name="p6389114616514"></a><a name="p6389114616514"></a>Destination operand.</p>
<p id="p37015312238"><a name="p37015312238"></a><a name="p37015312238"></a>Type: <a href="../register_data_types/RegTensor.md">RegTensor</a>.</p></td>
</tr>
<tr id="row2137145181815"><td class="cellrowborder" valign="top" width="18.54%" headers="mcps1.2.4.1.1 "><p id="p6844125874315"><a name="p6844125874315"></a><a name="p6844125874315"></a>**scalarValue**</p></td>
<td class="cellrowborder" valign="top" width="10.05%" headers="mcps1.2.4.1.2 "><p id="p128442058144312"><a name="p128442058144312"></a><a name="p128442058144312"></a>Input</p></td>
<td class="cellrowborder" valign="top" width="71.41%" headers="mcps1.2.4.1.3 "><p id="p9761550745"><a name="p9761550745"></a><a name="p9761550745"></a>Source operand.</p>
<p id="p1976165012411"><a name="p1976165012411"></a><a name="p1976165012411"></a>Type: scalar.</p></td>
</tr>
<tr id="row198393197176"><td class="cellrowborder" valign="top" width="18.54%" headers="mcps1.2.4.1.1 "><p id="p1664684418171"><a name="p1664684418171"></a><a name="p1664684418171"></a>**srcReg**</p></td>
<td class="cellrowborder" valign="top" width="10.05%" headers="mcps1.2.4.1.2 "><p id="p9839171971716"><a name="p9839171971716"></a><a name="p9839171971716"></a>Input</p></td>
<td class="cellrowborder" valign="top" width="71.41%" headers="mcps1.2.4.1.3 "><p id="p779095312179"><a name="p779095312179"></a><a name="p779095312179"></a>Source operand.</p>
<p id="p97903531171"><a name="p97903531171"></a><a name="p97903531171"></a>Type: <a href="../register_data_types/RegTensor.md">RegTensor</a>.</p></td>
</tr>
<tr id="row17332152321719"><td class="cellrowborder" valign="top" width="18.54%" headers="mcps1.2.4.1.1 "><p id="p1538745616196"><a name="p1538745616196"></a><a name="p1538745616196"></a>**mask**</p></td>
<td class="cellrowborder" valign="top" width="10.05%" headers="mcps1.2.4.1.2 "><p id="p73331523101710"><a name="p73331523101710"></a><a name="p73331523101710"></a>Input</p></td>
<td class="cellrowborder" valign="top" width="71.41%" headers="mcps1.2.4.1.3 "><p id="p11541143920"><a name="p11541143920"></a><a name="p11541143920"></a><span id="ph15776181222"><a name="ph15776181222"></a><a name="ph15776181222"></a>Valid indication of the operation on source operand elements. For details, see <a href="../register_data_types/MaskReg.md">MaskReg</a>.</span></p></td>
</tr>
</tbody>
</table>

**Table 3** Data type constraints on source operands and destination operands

<a name="table15348612514"></a>
<table><thead align="left"><tr id="row3341061250"><th class="cellrowborder" valign="top" width="50%" id="mcps1.2.3.1.1"><p id="p10342614250"><a name="p10342614250"></a><a name="p10342614250"></a>**dstReg** data type</p></th>
<th class="cellrowborder" valign="top" width="50%" id="mcps1.2.3.1.2"><p id="p2034968252"><a name="p2034968252"></a><a name="p2034968252"></a>**scalar/srcReg** data type</p></th>
</tr>
</thead>
<tbody><tr id="row10341367252"><td class="cellrowborder" valign="top" width="50%" headers="mcps1.2.3.1.1 "><p id="p51901218260"><a name="p51901218260"></a><a name="p51901218260"></a>bool</p></td>
<td class="cellrowborder" valign="top" width="50%" headers="mcps1.2.3.1.2 "><p id="p1328659182611"><a name="p1328659182611"></a><a name="p1328659182611"></a>bool</p></td>
</tr>
<tr id="row5341466253"><td class="cellrowborder" valign="top" width="50%" headers="mcps1.2.3.1.1 "><p id="p819052102614"><a name="p819052102614"></a><a name="p819052102614"></a>int8_t</p></td>
<td class="cellrowborder" valign="top" width="50%" headers="mcps1.2.3.1.2 "><p id="p122869913269"><a name="p122869913269"></a><a name="p122869913269"></a>int8_t</p></td>
</tr>
<tr id="row163413612519"><td class="cellrowborder" valign="top" width="50%" headers="mcps1.2.3.1.1 "><p id="p61908212262"><a name="p61908212262"></a><a name="p61908212262"></a>uint8_t</p></td>
<td class="cellrowborder" valign="top" width="50%" headers="mcps1.2.3.1.2 "><p id="p6286109202619"><a name="p6286109202619"></a><a name="p6286109202619"></a>uint8_t</p></td>
</tr>
<tr id="row4532510164711"><td class="cellrowborder" valign="top" width="50%" headers="mcps1.2.3.1.1 "><p id="p204451180503"><a name="p204451180503"></a><a name="p204451180503"></a>fp4x2_e2m1_t</p></td>
<td class="cellrowborder" valign="top" width="50%" headers="mcps1.2.3.1.2 "><p id="p1144501845010"><a name="p1144501845010"></a><a name="p1144501845010"></a>int8_t</p></td>
</tr>
<tr id="row03411632513"><td class="cellrowborder" valign="top" width="50%" headers="mcps1.2.3.1.1 "><p id="p1319018218263"><a name="p1319018218263"></a><a name="p1319018218263"></a>fp4x2_e1m2_t</p></td>
<td class="cellrowborder" valign="top" width="50%" headers="mcps1.2.3.1.2 "><p id="p1228615912614"><a name="p1228615912614"></a><a name="p1228615912614"></a>int8_t</p></td>
</tr>
<tr id="row203417618256"><td class="cellrowborder" valign="top" width="50%" headers="mcps1.2.3.1.1 "><p id="p51904232620"><a name="p51904232620"></a><a name="p51904232620"></a>hifloat8_t</p></td>
<td class="cellrowborder" valign="top" width="50%" headers="mcps1.2.3.1.2 "><p id="p128629122611"><a name="p128629122611"></a><a name="p128629122611"></a>int8_t</p></td>
</tr>
<tr id="row1022415972519"><td class="cellrowborder" valign="top" width="50%" headers="mcps1.2.3.1.1 "><p id="p1919013222612"><a name="p1919013222612"></a><a name="p1919013222612"></a>fp8_e5m2_t</p></td>
<td class="cellrowborder" valign="top" width="50%" headers="mcps1.2.3.1.2 "><p id="p628615962615"><a name="p628615962615"></a><a name="p628615962615"></a>int8_t</p></td>
</tr>
<tr id="row18107145118466"><td class="cellrowborder" valign="top" width="50%" headers="mcps1.2.3.1.1 "><p id="p125401256184615"><a name="p125401256184615"></a><a name="p125401256184615"></a>fp8_e4m3fn_t</p></td>
<td class="cellrowborder" valign="top" width="50%" headers="mcps1.2.3.1.2 "><p id="p654025674614"><a name="p654025674614"></a><a name="p654025674614"></a>int8_t</p></td>
</tr>
<tr id="row15844141654119"><td class="cellrowborder" valign="top" width="50%" headers="mcps1.2.3.1.1 "><p id="p1084521614412"><a name="p1084521614412"></a><a name="p1084521614412"></a>fp8_e8m0_t</p></td>
<td class="cellrowborder" valign="top" width="50%" headers="mcps1.2.3.1.2 "><p id="p12181115114417"><a name="p12181115114417"></a><a name="p12181115114417"></a>int8_t</p></td>
</tr>
<tr id="row14153324144116"><td class="cellrowborder" valign="top" width="50%" headers="mcps1.2.3.1.1 "><p id="p101531524154114"><a name="p101531524154114"></a><a name="p101531524154114"></a>int16_t</p></td>
<td class="cellrowborder" valign="top" width="50%" headers="mcps1.2.3.1.2 "><p id="p015318248417"><a name="p015318248417"></a><a name="p015318248417"></a>int16_t</p></td>
</tr>
<tr id="row88226332418"><td class="cellrowborder" valign="top" width="50%" headers="mcps1.2.3.1.1 "><p id="p20822333194110"><a name="p20822333194110"></a><a name="p20822333194110"></a>uint16_t</p></td>
<td class="cellrowborder" valign="top" width="50%" headers="mcps1.2.3.1.2 "><p id="p188221833104113"><a name="p188221833104113"></a><a name="p188221833104113"></a>uint16_t</p></td>
</tr>
<tr id="row1664043604117"><td class="cellrowborder" valign="top" width="50%" headers="mcps1.2.3.1.1 "><p id="p1864019363413"><a name="p1864019363413"></a><a name="p1864019363413"></a>half</p></td>
<td class="cellrowborder" valign="top" width="50%" headers="mcps1.2.3.1.2 "><p id="p1764053634115"><a name="p1764053634115"></a><a name="p1764053634115"></a>half</p></td>
</tr>
<tr id="row1454873915411"><td class="cellrowborder" valign="top" width="50%" headers="mcps1.2.3.1.1 "><p id="p154863914116"><a name="p154863914116"></a><a name="p154863914116"></a>bfloat16_t</p></td>
<td class="cellrowborder" valign="top" width="50%" headers="mcps1.2.3.1.2 "><p id="p145483397412"><a name="p145483397412"></a><a name="p145483397412"></a>bfloat16_t</p></td>
</tr>
<tr id="row16311164220414"><td class="cellrowborder" valign="top" width="50%" headers="mcps1.2.3.1.1 "><p id="p4311142114110"><a name="p4311142114110"></a><a name="p4311142114110"></a>int32_t</p></td>
<td class="cellrowborder" valign="top" width="50%" headers="mcps1.2.3.1.2 "><p id="p19311164219411"><a name="p19311164219411"></a><a name="p19311164219411"></a>int32_t</p></td>
</tr>
<tr id="row1773594910426"><td class="cellrowborder" valign="top" width="50%" headers="mcps1.2.3.1.1 "><p id="p173544994210"><a name="p173544994210"></a><a name="p173544994210"></a>uint32_t</p></td>
<td class="cellrowborder" valign="top" width="50%" headers="mcps1.2.3.1.2 "><p id="p1173544913422"><a name="p1173544913422"></a><a name="p1173544913422"></a>uint32_t</p></td>
</tr>
<tr id="row16100125334214"><td class="cellrowborder" valign="top" width="50%" headers="mcps1.2.3.1.1 "><p id="p191001953184214"><a name="p191001953184214"></a><a name="p191001953184214"></a>float</p></td>
<td class="cellrowborder" valign="top" width="50%" headers="mcps1.2.3.1.2 "><p id="p141001153184215"><a name="p141001153184215"></a><a name="p141001153184215"></a>float</p></td>
</tr>
<tr id="row16980105512422"><td class="cellrowborder" valign="top" width="50%" headers="mcps1.2.3.1.1 "><p id="p1998185554210"><a name="p1998185554210"></a><a name="p1998185554210"></a>complex32</p></td>
<td class="cellrowborder" valign="top" width="50%" headers="mcps1.2.3.1.2 "><p id="p2981175524212"><a name="p2981175524212"></a><a name="p2981175524212"></a>complex32</p></td>
</tr>
<tr id="row138164155432"><td class="cellrowborder" valign="top" width="50%" headers="mcps1.2.3.1.1 "><p id="p2816715124310"><a name="p2816715124310"></a><a name="p2816715124310"></a>int64_t</p></td>
<td class="cellrowborder" valign="top" width="50%" headers="mcps1.2.3.1.2 "><p id="p2816151513433"><a name="p2816151513433"></a><a name="p2816151513433"></a>int64_t</p></td>
</tr>
<tr id="row1264917185435"><td class="cellrowborder" valign="top" width="50%" headers="mcps1.2.3.1.1 "><p id="p765051811433"><a name="p765051811433"></a><a name="p765051811433"></a>uint64_t</p></td>
<td class="cellrowborder" valign="top" width="50%" headers="mcps1.2.3.1.2 "><p id="p1665018189436"><a name="p1665018189436"></a><a name="p1665018189436"></a>uint64_t</p></td>
</tr>
<tr id="row161799312434"><td class="cellrowborder" valign="top" width="50%" headers="mcps1.2.3.1.1 "><p id="p1317919310437"><a name="p1317919310437"></a><a name="p1317919310437"></a>complex64</p></td>
<td class="cellrowborder" valign="top" width="50%" headers="mcps1.2.3.1.2 "><p id="p1217963114434"><a name="p1217963114434"></a><a name="p1217963114434"></a>complex64</p></td>
</tr>
</tbody>
</table>

## Constraints<a name="section177921451558"></a>

None

## Example<a name="section642mcpsimp"></a>

-   Example 1

    ```
    template<typename T>
    __simd_vf__ inline void DuplicateVF(__ubuf__ T* dstAddr, T scalarValue, uint32_t oneRepeatSize, uint16_t repeatTimes)
    {
        AscendC::Reg::RegTensor<T> dstReg;
        AscendC::Reg::MaskReg mask = AscendC::Reg::CreateMask<T>();    
        for (uint16_t i = 0; i < repeatTimes; i++) {
            AscendC::Reg::Duplicate(dstReg, scalarValue);
            AscendC::Reg::StoreAlign(dstAddr + i * oneRepeatSize, dstReg, mask);
        }
    }
    ```

-   Example 2

    ```
    template<typename T>
    __simd_vf__ inline void DuplicateVF(__ubuf__ T* dstAddr, T scalarValue, uint32_t count, uint32_t oneRepeatSize, uint16_t repeatTimes)
    {
        AscendC::Reg::RegTensor<T> dstReg;
        AscendC::Reg::MaskReg mask;    
        for (uint16_t i = 0; i < repeatTimes; i++) {
            mask = AscendC::Reg::UpdateMask<T>(count);
            AscendC::Reg::Duplicate(dstReg, scalarValue, mask);
            AscendC::Reg::StoreAlign(dstAddr + i * oneRepeatSize, dstReg, mask);
        }
    }
    ```

-   Example 3

    ```
    template<typename T>
    __simd_vf__ inline void DuplicateVF(__ubuf__ T* dstAddr, __ubuf__ T* srcAddr, uint32_t count, uint32_t oneRepeatSize, uint16_t repeatTimes)
    {
        AscendC::Reg::RegTensor<T> srcReg;
        AscendC::Reg::RegTensor<T> dstReg;
        AscendC::Reg::MaskReg mask;
        for (uint16_t i = 0; i < repeatTimes; i++) {
            mask = AscendC::Reg::UpdateMask<T>(count);
            AscendC::Reg::LoadAlign(srcReg, srcAddr + i * oneRepeatSize);
            AscendC::Reg::Duplicate(dstReg, srcReg, mask);
            AscendC::Reg::StoreAlign(dstAddr + i * oneRepeatSize, dstReg, mask);
        }
    }
    ```

