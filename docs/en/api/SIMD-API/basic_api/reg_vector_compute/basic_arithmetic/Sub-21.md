# Sub<a name="ZH-CN_TOPIC_0000001929827652"></a>

<!-- md-trans-meta sourceCommit=87fcc89e932521312a6dc152579c1037d8e11853 translatedAt=2026-08-27T21:56:28.092Z -->

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

Performs element-wise subtraction on the source operands **srcReg0** and **srcReg1** according to **mask**, and writes the result to the destination operand **dstReg**. The calculation formula is as follows:

![](../../../../figures/zh-cn_formulaimage_0000001930509058.png)

If the subtraction of **srcReg0** and **srcReg1** produces a borrow, 0 is written to the least significant bit of each 4-bit group at the corresponding position in **MaskReg carry**; otherwise, 1 is written.

A specific example is as follows:

**Table 1**  Example description

<a name="table78091736133120"></a>
<table><thead align="left"><tr id="row480917360316"><th class="cellrowborder" valign="top" width="16.31163116311631%" id="mcps1.2.4.1.1"><p id="p580917368314"><a name="p580917368314"></a><a name="p580917368314"></a>Data Type</p></th>
<th class="cellrowborder" valign="top" width="19.561956195619562%" id="mcps1.2.4.1.2"><p id="p1280912365313"><a name="p1280912365313"></a><a name="p1280912365313"></a>Whether Borrowed</p></th>
<th class="cellrowborder" valign="top" width="64.12641264126412%" id="mcps1.2.4.1.3"><p id="p680913653119"><a name="p680913653119"></a><a name="p680913653119"></a>Example Description</p></th>
</tr>
</thead>
<tbody><tr id="row9650119112310"><td class="cellrowborder" rowspan="2" valign="top" width="16.31163116311631%" headers="mcps1.2.4.1.1 "><p id="p21571440123413"><a name="p21571440123413"></a><a name="p21571440123413"></a>int32_t data type</p>
<p id="p4870121115236"><a name="p4870121115236"></a><a name="p4870121115236"></a></p></td>
<td class="cellrowborder" valign="top" width="19.561956195619562%" headers="mcps1.2.4.1.2 "><p id="p154637020320"><a name="p154637020320"></a><a name="p154637020320"></a>No borrow is generated.</p></td>
<td class="cellrowborder" valign="top" width="64.12641264126412%" headers="mcps1.2.4.1.3 "><p id="p3311243193110"><a name="p3311243193110"></a><a name="p3311243193110"></a>a_i = 5, b_i = 2</p>
<p id="p531113439313"><a name="p531113439313"></a><a name="p531113439313"></a>dst_i = a_i - b_i = 3</p>
<p id="p4311164312316"><a name="p4311164312316"></a><a name="p4311164312316"></a>Write 1 to the least significant bit of each 4-bit group at the corresponding position in carry: carry_i = 1</p></td>
</tr>
<tr id="row2650199102310"><td class="cellrowborder" valign="top" headers="mcps1.2.4.1.1 "><p id="p58101636153112"><a name="p58101636153112"></a><a name="p58101636153112"></a>A borrow is generated.</p></td>
<td class="cellrowborder" valign="top" headers="mcps1.2.4.1.2 "><p id="p1121711024413"><a name="p1121711024413"></a><a name="p1121711024413"></a>a_i = 5, b_i = -7</p>
<p id="p15172134819317"><a name="p15172134819317"></a><a name="p15172134819317"></a>dst_i = a_i - b_i  = 12</p>
<p id="p5172174883118"><a name="p5172174883118"></a><a name="p5172174883118"></a>Write 0 to the least significant bit of each 4-bit group at the corresponding position in carry: carry_i = 0</p></td>
</tr>
<tr id="row381063633114"><td class="cellrowborder" rowspan="2" valign="top" width="16.31163116311631%" headers="mcps1.2.4.1.1 "><p id="p3815204223410"><a name="p3815204223410"></a><a name="p3815204223410"></a>uint32_t data type</p></td>
<td class="cellrowborder" valign="top" width="19.561956195619562%" headers="mcps1.2.4.1.2 "><p id="p11843103015347"><a name="p11843103015347"></a><a name="p11843103015347"></a>No borrow is generated.</p></td>
<td class="cellrowborder" valign="top" width="64.12641264126412%" headers="mcps1.2.4.1.3 "><p id="p570203082917"><a name="p570203082917"></a><a name="p570203082917"></a>a_i = 5, b_i = 2,</p>
<p id="p168941627173112"><a name="p168941627173112"></a><a name="p168941627173112"></a>dst_i = a_i - b_i = 3</p>
<p id="p29562393290"><a name="p29562393290"></a><a name="p29562393290"></a>Write 1 to the least significant bit of each 4-bit group at the corresponding position in carry: carry_i = 1</p></td>
</tr>
<tr id="row1481017368317"><td class="cellrowborder" valign="top" headers="mcps1.2.4.1.1 "><p id="p148435308342"><a name="p148435308342"></a><a name="p148435308342"></a>A borrow is generated.</p></td>
<td class="cellrowborder" valign="top" headers="mcps1.2.4.1.2 "><p id="p686855917304"><a name="p686855917304"></a><a name="p686855917304"></a>a_i = 5, b_i = 7</p>
<p id="p34696716311"><a name="p34696716311"></a><a name="p34696716311"></a>dst_i = a_i - b_i = -2</p>
<p id="p8868185973017"><a name="p8868185973017"></a><a name="p8868185973017"></a>Write 0 to the least significant bit of each 4-bit group at the corresponding position in carry: carry_i = 0</p></td>
</tr>
</tbody>
</table>

## Prototype<a name="section620mcpsimp"></a>

-   The calculation result does not retain the carry.

    ```
    template <typename T = DefaultType, MaskMergeMode mode = MaskMergeMode::ZEROING, typename U>
    __simd_callee__ inline void Sub(U& dstReg, U& srcReg0, U& srcReg1, MaskReg& mask)
    ```

-   The calculation result retains the carry.

    ```
    template <typename T = DefaultType, typename U>
    __simd_callee__ inline void Sub(MaskReg& carry, U& dstReg, U& srcReg0, U& srcReg1, MaskReg& mask)
    ```

## Parameters<a name="section622mcpsimp"></a>

**Table 2** Template parameter description

<a name="table4835205712588"></a>
<table><thead align="left"><tr id="row118356578583"><th class="cellrowborder" valign="top" width="18.67%" id="mcps1.2.3.1.1"><p id="p48354572582"><a name="p48354572582"></a><a name="p48354572582"></a>Parameter</p></th>
<th class="cellrowborder" valign="top" width="81.33%" id="mcps1.2.3.1.2"><p id="p583535795817"><a name="p583535795817"></a><a name="p583535795817"></a>Description</p></th>
</tr>
</thead>
<tbody><tr id="row1835857145817"><td class="cellrowborder" valign="top" width="18.67%" headers="mcps1.2.3.1.1 "><p id="p5835457165816"><a name="p5835457165816"></a><a name="p5835457165816"></a>T</p></td>
<td class="cellrowborder" valign="top" width="81.33%" headers="mcps1.2.3.1.2 "><p id="p168351657155818"><a name="p168351657155818"></a><a name="p168351657155818"></a>Data type of the operand.</p>
<p id="p126551392111"><a name="p126551392111"></a><a name="p126551392111"></a><span id="ph116551791911"><a name="ph116551791911"></a><a name="ph116551791911"></a>Ascend 950PR/Ascend 950DT</span>, supported data types are: uint8_t/int8_t/uint16_t/int16_t/uint32_t/int32_t/half/float/bfloat16_t/uint64_t/int64_t/complex32/complex64</p></td>
</tr>
<tr id="row18835145716587"><td class="cellrowborder" valign="top" width="18.67%" headers="mcps1.2.3.1.1 "><p id="p1756419170189"><a name="p1756419170189"></a><a name="p1756419170189"></a>mode</p></td>
<td class="cellrowborder" valign="top" width="81.33%" headers="mcps1.2.3.1.2 "><p id="p77520541653"><a name="p77520541653"></a><a name="p77520541653"></a>Selects the MERGING mode or the ZEROING mode.</p>
<a name="ul1163765616511"></a><a name="ul1163765616511"></a><ul id="ul1163765616511"><li>ZEROING, elements not filtered by the mask are set to zero in dstReg.</li><li>MERGING, currently not supported.</li></ul></td>
</tr>
<tr id="row134574435319"><td class="cellrowborder" valign="top" width="18.67%" headers="mcps1.2.3.1.1 "><p id="p916243141912"><a name="p916243141912"></a><a name="p916243141912"></a>U</p></td>
<td class="cellrowborder" valign="top" width="81.33%" headers="mcps1.2.3.1.2 "><p id="p15901115914145"><a name="p15901115914145"></a><a name="p15901115914145"></a><span id="ph19851723182011"><a name="ph19851723182011"></a><a name="ph19851723182011"></a>RegTensor type of the destination operand, for example, RegTensor&lt;half&gt;, which is automatically deduced by the compiler and does not need to be specified by the user.</span></p></td>
</tr>
</tbody>
</table>

**Table 3**  Parameters

<a name="zh-cn_topic_0235751031_table33761356"></a>
<table><thead align="left"><tr id="zh-cn_topic_0235751031_row27598891"><th class="cellrowborder" valign="top" width="18.54%" id="mcps1.2.4.1.1"><p id="zh-cn_topic_0235751031_p20917673"><a name="zh-cn_topic_0235751031_p20917673"></a><a name="zh-cn_topic_0235751031_p20917673"></a>Parameter</p></th>
<th class="cellrowborder" valign="top" width="10.05%" id="mcps1.2.4.1.2"><p id="zh-cn_topic_0235751031_p16609919"><a name="zh-cn_topic_0235751031_p16609919"></a><a name="zh-cn_topic_0235751031_p16609919"></a>Input/Output</p></th>
<th class="cellrowborder" valign="top" width="71.41%" id="mcps1.2.4.1.3"><p id="zh-cn_topic_0235751031_p59995477"><a name="zh-cn_topic_0235751031_p59995477"></a><a name="zh-cn_topic_0235751031_p59995477"></a>Description</p></th>
</tr>
</thead>
<tbody><tr id="row42461942101815"><td class="cellrowborder" valign="top" width="18.54%" headers="mcps1.2.4.1.1 "><p id="p108051250181214"><a name="p108051250181214"></a><a name="p108051250181214"></a>dstReg</p></td>
<td class="cellrowborder" valign="top" width="10.05%" headers="mcps1.2.4.1.2 "><p id="p158449584436"><a name="p158449584436"></a><a name="p158449584436"></a>Output</p></td>
<td class="cellrowborder" valign="top" width="71.41%" headers="mcps1.2.4.1.3 "><p id="p6389114616514"><a name="p6389114616514"></a><a name="p6389114616514"></a>Destination operand.</p>
<p id="p66093533169"><a name="p66093533169"></a><a name="p66093533169"></a><span id="ph134278176129"><a name="ph134278176129"></a><a name="ph134278176129"></a>The type is <a href="../register_data_types/RegTensor.md">RegTensor</a>.</span></p></td>
</tr>
<tr id="row2137145181815"><td class="cellrowborder" valign="top" width="18.54%" headers="mcps1.2.4.1.1 "><p id="p19574165615129"><a name="p19574165615129"></a><a name="p19574165615129"></a>srcReg0</p></td>
<td class="cellrowborder" valign="top" width="10.05%" headers="mcps1.2.4.1.2 "><p id="p128442058144312"><a name="p128442058144312"></a><a name="p128442058144312"></a>Input</p></td>
<td class="cellrowborder" valign="top" width="71.41%" headers="mcps1.2.4.1.3 "><p id="p172083541517"><a name="p172083541517"></a><a name="p172083541517"></a>Source operand.</p>
<p id="p12998112012416"><a name="p12998112012416"></a><a name="p12998112012416"></a><span id="ph1998120152416"><a name="ph1998120152416"></a><a name="ph1998120152416"></a>The type is <a href="../register_data_types/RegTensor.md">RegTensor</a>.</span></p>
<p id="p1484485824312"><a name="p1484485824312"></a><a name="p1484485824312"></a>The data type must be consistent with the destination operand.</p></td>
</tr>
<tr id="row11773440341"><td class="cellrowborder" valign="top" width="18.54%" headers="mcps1.2.4.1.1 "><p id="p10132448173420"><a name="p10132448173420"></a><a name="p10132448173420"></a>srcReg1</p></td>
<td class="cellrowborder" valign="top" width="10.05%" headers="mcps1.2.4.1.2 "><p id="p151325484342"><a name="p151325484342"></a><a name="p151325484342"></a>Input</p></td>
<td class="cellrowborder" valign="top" width="71.41%" headers="mcps1.2.4.1.3 "><p id="p181326485341"><a name="p181326485341"></a><a name="p181326485341"></a>Source operand.</p>
<p id="p15657823142410"><a name="p15657823142410"></a><a name="p15657823142410"></a><span id="ph196571123192418"><a name="ph196571123192418"></a><a name="ph196571123192418"></a>The type is <a href="../register_data_types/RegTensor.md">RegTensor</a>.</span></p>
<p id="p12132154853419"><a name="p12132154853419"></a><a name="p12132154853419"></a>The data type must be consistent with the destination operand.</p></td>
</tr>
<tr id="row114531581305"><td class="cellrowborder" valign="top" width="18.54%" headers="mcps1.2.4.1.1 "><p id="p64531758183015"><a name="p64531758183015"></a><a name="p64531758183015"></a>carry</p></td>
<td class="cellrowborder" valign="top" width="10.05%" headers="mcps1.2.4.1.2 "><p id="p34536586309"><a name="p34536586309"></a><a name="p34536586309"></a>Output</p></td>
<td class="cellrowborder" valign="top" width="71.41%" headers="mcps1.2.4.1.3 "><p id="p1142915300561"><a name="p1142915300561"></a><a name="p1142915300561"></a>Destination operand. Outputs the carry value.</p>
<p id="p1742912304566"><a name="p1742912304566"></a><a name="p1742912304566"></a>The type is <a href="../register_data_types/MaskReg.md">MaskReg</a>.</p></td>
</tr>
<tr id="row19615183817191"><td class="cellrowborder" valign="top" width="18.54%" headers="mcps1.2.4.1.1 "><p id="p1484519586432"><a name="p1484519586432"></a><a name="p1484519586432"></a>mask</p></td>
<td class="cellrowborder" valign="top" width="10.05%" headers="mcps1.2.4.1.2 "><p id="p484514581433"><a name="p484514581433"></a><a name="p484514581433"></a>Input</p></td>
<td class="cellrowborder" valign="top" width="71.41%" headers="mcps1.2.4.1.3 "><p id="p11541143920"><a name="p11541143920"></a><a name="p11541143920"></a><span id="ph15776181222"><a name="ph15776181222"></a><a name="ph15776181222"></a>Valid indication of the operation on source operand elements. For details, see <a href="../register_data_types/MaskReg.md">MaskReg</a>.</span></p></td>
</tr>
</tbody>
</table>

## Return Value<a name="section640mcpsimp"></a>

None

## Constraints<a name="section633mcpsimp"></a>

None

## Example<a name="section642mcpsimp"></a>

-   The calculation result does not retain the carry.

    ```
    template<typename T>
    __simd_vf__ inline void SubVF(__ubuf__ T* dstAddr, __ubuf__ T* src0Addr, __ubuf__ T* src1Addr, uint32_t count, uint32_t oneRepeatSize, uint16_t repeatTimes)
    {
        AscendC::Reg::RegTensor<T> srcReg0;
        AscendC::Reg::RegTensor<T> srcReg1;
        AscendC::Reg::RegTensor<T> dstReg;
        AscendC::Reg::MaskReg mask;    
        for (uint16_t i = 0; i < repeatTimes; i++) {
            mask = AscendC::Reg::UpdateMask<T>(count);
            AscendC::Reg::LoadAlign(srcReg0, src0Addr + i * oneRepeatSize);
            AscendC::Reg::LoadAlign(srcReg1, src1Addr + i * oneRepeatSize);
            AscendC::Reg::Sub(dstReg, srcReg0, srcReg1, mask);
            AscendC::Reg::StoreAlign(dstAddr + i * oneRepeatSize, dstReg, mask);
        }
    }
    ```

-   The calculation result retains the carry.

    ```
    template <typename T>
    __simd_vf__ inline void SubVF(__ubuf__ T* dst0Addr, __ubuf__ T* dst1Addr, __ubuf__ T* src0Addr, __ubuf__ T* src1Addr, uint32_t count, uint32_t repeatTimes, uint16_t oneRepeatSize){
        
        AscendC::Reg::RegTensor<T> srcReg0;
        AscendC::Reg::RegTensor<T> srcReg1;
        AscendC::Reg::RegTensor<T> dstReg0;
        AscendC::Reg::MaskReg mask;   
        AscendC::Reg::MaskReg carry = AscendC::Reg::CreateMask<uint8_t>();
        for (uint16_t i = 0; i < repeatTimes; i++) {
            mask = AscendC::Reg::UpdateMask<T>(count);
            AscendC::Reg::LoadAlign(srcReg0, src0Addr + i * oneRepeatSize);
            AscendC::Reg::LoadAlign(srcReg1, src1Addr + i * oneRepeatSize);
            AscendC::Reg::Sub(carry, dstReg0, srcReg0, srcReg1, mask);
            // 8*4B=32B align
            AscendC::Reg::StoreAlign<uint32_t, AscendC::Reg::MaskDist::DIST_NORM>((__ubuf__ uint32_t*)dst1Addr + i * 8, carry);
            AscendC::Reg::StoreAlign(dst0Addr + i * oneRepeatSize, dstReg0, mask);
        }
    }
    ```

