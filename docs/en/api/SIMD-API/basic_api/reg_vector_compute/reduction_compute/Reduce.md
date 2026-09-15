# Reduce<a name="ZH-CN_TOPIC_0000001938595870"></a>

<!-- md-trans-meta sourceCommit=87fcc89e932521312a6dc152579c1037d8e11853 translatedAt=2026-08-27T21:37:53.278Z -->

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

Supports reduction sum, reduction max, and reduction min.

## Prototype<a name="section620mcpsimp"></a>

```
template <ReduceType type = ReduceType::SUM, typename T = DefaultType, typename U = DefaultType, MaskMergeMode mode = MaskMergeMode::ZEROING, typename S, typename V>
__simd_callee__ inline void Reduce(S& dstReg, V srcReg, MaskReg mask)
```

## Parameters<a name="section622mcpsimp"></a>

**Table 1** Template parameter description

<a name="table4835205712588"></a>
<table><thead align="left"><tr id="row118356578583"><th class="cellrowborder" valign="top" width="13.3%" id="mcps1.2.3.1.1"><p id="p48354572582"><a name="p48354572582"></a><a name="p48354572582"></a>Parameter</p></th>
<th class="cellrowborder" valign="top" width="86.7%" id="mcps1.2.3.1.2"><p id="p583535795817"><a name="p583535795817"></a><a name="p583535795817"></a>Description</p></th>
</tr>
</thead>
<tbody><tr id="row137168314214"><td class="cellrowborder" valign="top" width="13.3%" headers="mcps1.2.3.1.1 "><p id="p197161530217"><a name="p197161530217"></a><a name="p197161530217"></a>**type**</p></td>
<td class="cellrowborder" valign="top" width="86.7%" headers="mcps1.2.3.1.2 "><p id="p47161338218"><a name="p47161338218"></a><a name="p47161338218"></a>**ReduceType** type, supporting **SUM**, **MAX**, and **MIN**.</p>
<pre class="screen" id="screen15472194923914"><a name="screen15472194923914"></a><a name="screen15472194923914"></a>enum class ReduceType {
    SUM = 0,
    MAX,
    MIN,
};</pre></td>
</tr>
<tr id="row1835857145817"><td class="cellrowborder" valign="top" width="13.3%" headers="mcps1.2.3.1.1 "><p id="p5835457165816"><a name="p5835457165816"></a><a name="p5835457165816"></a>**T**</p></td>
<td class="cellrowborder" valign="top" width="86.7%" headers="mcps1.2.3.1.2 "><p id="p10587154162715"><a name="p10587154162715"></a><a name="p10587154162715"></a>Data type of the destination operation **dstReg**.</p>
<a name="ul177171057447"></a><a name="ul177171057447"></a><ul id="ul177171057447"><li>When **type** = **ReduceType::SUM**, the supported data types must match those of the source operand **srcReg**. The matching relationships are as follows, and the data type matching relationships below are arranged in the order of &lt;**dstReg**, **srcReg**&gt;:<p id="p143901768563"><a name="p143901768563"></a><a name="p143901768563"></a><span id="ph13901568568"><a name="ph13901568568"></a><a name="ph13901568568"></a>Ascend 950PR/Ascend 950DT</span>, the supported data types are: &lt;int32_t, int16_t&gt;, &lt;int32_t, int32_t&gt;, &lt;half, half&gt;, &lt;uint32_t, uint16_t&gt;, &lt;uint32_t, uint32_t&gt;, &lt;float, float&gt;, &lt;uint64_t, uint64_t&gt;, &lt;int64_t, int64_t&gt;</p>
</li></ul>
<a name="ul1768984611616"></a><a name="ul1768984611616"></a><ul id="ul1768984611616"><li>When **type** = **ReduceType::MAX** or **type** = **ReduceType::MIN**:<p id="p81921422102412"><a name="p81921422102412"></a><a name="p81921422102412"></a><span id="ph81921522122414"><a name="ph81921522122414"></a><a name="ph81921522122414"></a>Ascend 950PR/Ascend 950DT</span>, the supported data types are: int16_t/half/int32_t/float/uint16_t/uint32_t/uint64_t/int64_t</p>
</li></ul></td>
</tr>
<tr id="row137401528960"><td class="cellrowborder" valign="top" width="13.3%" headers="mcps1.2.3.1.1 "><p id="p07403281766"><a name="p07403281766"></a><a name="p07403281766"></a>**U**</p></td>
<td class="cellrowborder" valign="top" width="86.7%" headers="mcps1.2.3.1.2 "><p id="p1095520521693"><a name="p1095520521693"></a><a name="p1095520521693"></a>Data type of the source operand **srcReg**.</p>
<a name="ul61091217191015"></a><a name="ul61091217191015"></a><ul id="ul61091217191015"><li>When **type** = **ReduceType::SUM**, the supported data type must match that of the destination operand **dstReg**.</li><li>When **type** = **ReduceType::MAX** or **type** = **ReduceType::MIN**, the data type of the source operand is the same as that of the destination operand.</li></ul>
<p id="p13317181094510"><a name="p13317181094510"></a><a name="p13317181094510"></a><span id="ph931731012451"><a name="ph931731012451"></a><a name="ph931731012451"></a>Ascend 950PR/Ascend 950DT</span>, the supported data types are: int16_t/half/int32_t/float/uint16_t/uint32_t/uint64_t/int64_t</p></td>
</tr>
<tr id="row3742113618507"><td class="cellrowborder" valign="top" width="13.3%" headers="mcps1.2.3.1.1 "><p id="p1383515717581"><a name="p1383515717581"></a><a name="p1383515717581"></a>mode</p></td>
<td class="cellrowborder" valign="top" width="86.7%" headers="mcps1.2.3.1.2 "><p id="p77520541653"><a name="p77520541653"></a><a name="p77520541653"></a>Selects the MERGING mode or the ZEROING mode. Currently, only the ZEROING mode is supported.</p>
<a name="ul1163765616511"></a><a name="ul1163765616511"></a><ul id="ul1163765616511"><li>ZEROING, elements not filtered by the mask are set to zero in dst.</li></ul></td>
</tr>
<tr id="row156557341370"><td class="cellrowborder" valign="top" width="13.3%" headers="mcps1.2.3.1.1 "><p id="p916243141912"><a name="p916243141912"></a><a name="p916243141912"></a>S</p></td>
<td class="cellrowborder" valign="top" width="86.7%" headers="mcps1.2.3.1.2 "><p id="p131047476612"><a name="p131047476612"></a><a name="p131047476612"></a><span id="ph19851723182011"><a name="ph19851723182011"></a><a name="ph19851723182011"></a>RegTensor type of the destination operand, for example, RegTensor&lt;half&gt;, which is automatically deduced by the compiler and does not need to be specified by the user.</span></p></td>
</tr>
<tr id="row1577483812718"><td class="cellrowborder" valign="top" width="13.3%" headers="mcps1.2.3.1.1 "><p id="p1658710511977"><a name="p1658710511977"></a><a name="p1658710511977"></a>V</p></td>
<td class="cellrowborder" valign="top" width="86.7%" headers="mcps1.2.3.1.2 "><p id="p839055684515"><a name="p839055684515"></a><a name="p839055684515"></a><span id="ph11390135684517"><a name="ph11390135684517"></a><a name="ph11390135684517"></a>RegTensor type of the source operand, for example, RegTensor&lt;half&gt;, which is automatically deduced by the compiler and does not need to be specified by the user.</span></p></td>
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
<tbody><tr id="row42461942101815"><td class="cellrowborder" valign="top" width="18.54%" headers="mcps1.2.4.1.1 "><p id="p284425844311"><a name="p284425844311"></a><a name="p284425844311"></a>dstReg</p></td>
<td class="cellrowborder" valign="top" width="10.05%" headers="mcps1.2.4.1.2 "><p id="p158449584436"><a name="p158449584436"></a><a name="p158449584436"></a>Output</p></td>
<td class="cellrowborder" valign="top" width="71.41%" headers="mcps1.2.4.1.3 "><p id="p6389114616514"><a name="p6389114616514"></a><a name="p6389114616514"></a>Destination operand.</p>
<p id="p1818424418593"><a name="p1818424418593"></a><a name="p1818424418593"></a>The type is <a href="../register_data_types/RegTensor.md">RegTensor</a>.</p></td>
</tr>
<tr id="row198393197176"><td class="cellrowborder" valign="top" width="18.54%" headers="mcps1.2.4.1.1 "><p id="p1664684418171"><a name="p1664684418171"></a><a name="p1664684418171"></a>srcReg</p></td>
<td class="cellrowborder" valign="top" width="10.05%" headers="mcps1.2.4.1.2 "><p id="p9839171971716"><a name="p9839171971716"></a><a name="p9839171971716"></a>Input</p></td>
<td class="cellrowborder" valign="top" width="71.41%" headers="mcps1.2.4.1.3 "><p id="p779095312179"><a name="p779095312179"></a><a name="p779095312179"></a>Source operand.</p>
<p id="p97903531171"><a name="p97903531171"></a><a name="p97903531171"></a>The type is <a href="../register_data_types/RegTensor.md">RegTensor</a>.</p></td>
</tr>
<tr id="row17332152321719"><td class="cellrowborder" valign="top" width="18.54%" headers="mcps1.2.4.1.1 "><p id="p1538745616196"><a name="p1538745616196"></a><a name="p1538745616196"></a>mask</p></td>
<td class="cellrowborder" valign="top" width="10.05%" headers="mcps1.2.4.1.2 "><p id="p73331523101710"><a name="p73331523101710"></a><a name="p73331523101710"></a>Input</p></td>
<td class="cellrowborder" valign="top" width="71.41%" headers="mcps1.2.4.1.3 "><p id="p11541143920"><a name="p11541143920"></a><a name="p11541143920"></a><span id="ph15776181222"><a name="ph15776181222"></a><a name="ph15776181222"></a>Valid indication of source operand element operations. For details, see <a href="../register_data_types/MaskReg.md">MaskReg</a>.</span></p></td>
</tr>
</tbody>
</table>

## Constraints<a name="section177921451558"></a>

-   For reduction max, when all elements are excluded from computation, the minimum value of the data type is written to **dstReg**. When multiple maximum values exist, the index of the first maximum value is saved in **dstReg**.
-   For reduction min, when all elements are excluded from computation, the maximum value of the data type is written to **dstReg**. When multiple minimum values exist, the index of the first minimum value is saved in **dstReg**.
-   For reduction min or reduction max, the source operand and the destination operation have the same data type.

## Example<a name="section642mcpsimp"></a>

-   Reduction sum:

    ```
    template<typename T, typename U>
    __simd_vf__ inline void ReduceVF(__ubuf__ T* dstAddr, __ubuf__ U* srcAddr, uint32_t count, 
     uint32_t srcRepeatSize, uint32_t dstRepeatSize, uint16_t repeatTimes)
    {
        AscendC::Reg::RegTensor<U> srcReg;
        AscendC::Reg::RegTensor<T> dsrReg;
        AscendC::Reg::MaskReg mask;
        for (uint16_t i = 0; i < repeatTimes; i++) {
            AscendC::Reg::LoadAlign(srcReg, srcAddr + i * srcRepeatSize);
            mask = AscendC::Reg::UpdateMask<U>(count);
            AscendC::Reg::Reduce<AscendC::Reg::ReduceType::SUM>(dsrReg, srcReg, mask);
            AscendC::Reg::StoreAlign(dstAddr + i * dstRepeatSize, dsrReg, mask);
        }
    }
    ```

-   Reduction max or min

    ```
    template<typename T>
    __aicore__ inline void ReduceVF(__ubuf__ T* dstAddr, __ubuf__ T* srcAddr, uint32_t count, uint32_t oneRepeatSize, uint16_t repeatTimes)
    {
        AscendC::Reg::RegTensor<T> srcReg;
        AscendC::Reg::RegTensor<T> dstReg;
        AscendC::Reg::MaskReg mask;
        for (uint16_t i = 0; i < repeatTimes; i++) {
            AscendC::Reg::LoadAlign(srcReg, srcAddr + i * oneRepeatSize);
            mask = AscendC::Reg::UpdateMask<T>(count);
            // type = ReduceType::MAX
            AscendC::Reg::Reduce<AscendC::Reg::ReduceType::MAX>(dstReg, srcReg, mask);
            // type = ReduceType::MIN
            // AscendC::Reg::Reduce<AscendC::Reg::ReduceType::MIN>(dstReg, srcReg, mask);
            AscendC::Reg::StoreAlign(dstAddr + i * oneRepeatSize, dstReg, maskReg);
        }
    }
    ```

