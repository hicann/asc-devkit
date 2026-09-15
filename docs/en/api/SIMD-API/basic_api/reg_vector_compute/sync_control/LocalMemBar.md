# LocalMemBar<a name="ZH-CN_TOPIC_0000002222134221"></a>

<!-- md-trans-meta sourceCommit=unknown translatedAt=2026-08-31T12:57:18.408Z -->

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

A synchronization instruction between different pipelines within a Reg vector computation macro function. This synchronization instruction specifies the **src** source pipeline and the **dst** destination pipeline. As shown in the following figure, the destination pipeline waits until all instructions on the source pipeline are complete before execution. In a read/write scenario, when the register used by a read instruction is the same as the register used by a write instruction, register ordering is triggered, and the instructions are executed in code order without inserting a synchronization instruction. When the registers used are different, a synchronization instruction must be inserted to ensure that the read and write instructions are executed in order. The same applies to a write/write scenario.

**Figure 1**  Pipeline waiting diagram<a name="fig2635167165614"></a>  
![](../../../../figures/pipeline_wait_diagram.png "Pipeline waiting diagram")

## Prototype<a name="section620mcpsimp"></a>

```
template <MemType src, MemType dst> 
__simd_callee__ inline void LocalMemBar()
```

## Parameters<a name="section622mcpsimp"></a>

**Table 1** Template parameter description

<a name="table4835205712588"></a>
<table><thead align="left"><tr id="row118356578583"><th class="cellrowborder" valign="top" width="18.27%" id="mcps1.2.3.1.1"><p id="p48354572582"><a name="p48354572582"></a><a name="p48354572582"></a>Parameter</p></th>
<th class="cellrowborder" valign="top" width="81.73%" id="mcps1.2.3.1.2"><p id="p583535795817"><a name="p583535795817"></a><a name="p583535795817"></a>Description</p></th>
</tr>
</thead>
<tbody><tr id="row1835857145817"><td class="cellrowborder" valign="top" width="18.27%" headers="mcps1.2.3.1.1 "><p id="p5835457165816"><a name="p5835457165816"></a><a name="p5835457165816"></a>**src**</p></td>
<td class="cellrowborder" valign="top" width="81.73%" headers="mcps1.2.3.1.2 "><p id="p16653174755317"><a name="p16653174755317"></a><a name="p16653174755317"></a>Source pipeline, of the **MemType** type. For details, see <a href="#zh-cn_topic_0235751031_table33761356">Table 2 MemType value description</a>.</p></td>
</tr>
<tr id="row18835145716587"><td class="cellrowborder" valign="top" width="18.27%" headers="mcps1.2.3.1.1 "><p id="p1756419170189"><a name="p1756419170189"></a><a name="p1756419170189"></a>**dst**</p></td>
<td class="cellrowborder" valign="top" width="81.73%" headers="mcps1.2.3.1.2 "><p id="p6881103675215"><a name="p6881103675215"></a><a name="p6881103675215"></a>Destination pipeline, of the **MemType** type. For details, see <a href="#zh-cn_topic_0235751031_table33761356">Table 2 MemType value description</a>.</p></td>
</tr>
</tbody>
</table>

**Table 2** MemType value description

<a name="zh-cn_topic_0235751031_table33761356"></a>
<table><thead align="left"><tr id="zh-cn_topic_0235751031_row27598891"><th class="cellrowborder" valign="top" width="18.21%" id="mcps1.2.3.1.1"><p id="zh-cn_topic_0235751031_p20917673"><a name="zh-cn_topic_0235751031_p20917673"></a><a name="zh-cn_topic_0235751031_p20917673"></a>MemType Value</p></th>
<th class="cellrowborder" valign="top" width="81.78999999999999%" id="mcps1.2.3.1.2"><p id="zh-cn_topic_0235751031_p59995477"><a name="zh-cn_topic_0235751031_p59995477"></a><a name="zh-cn_topic_0235751031_p59995477"></a>Description</p></th>
</tr>
</thead>
<tbody><tr id="row42461942101815"><td class="cellrowborder" valign="top" width="18.21%" headers="mcps1.2.3.1.1 "><p id="p108051250181214"><a name="p108051250181214"></a><a name="p108051250181214"></a>VEC_STORE</p></td>
<td class="cellrowborder" valign="top" width="81.78999999999999%" headers="mcps1.2.3.1.2 "><p id="p167614410315"><a name="p167614410315"></a><a name="p167614410315"></a>Vector write UB pipeline in the SIMD_VF function.</p>
<p id="p45581654185211"><a name="p45581654185211"></a><a name="p45581654185211"></a>Corresponds to register-to-UB transfer instructions, such as StoreAlign, StoreUnAlign, and Store.</p></td>
</tr>
<tr id="row2137145181815"><td class="cellrowborder" valign="top" width="18.21%" headers="mcps1.2.3.1.1 "><p id="p19574165615129"><a name="p19574165615129"></a><a name="p19574165615129"></a>VEC_LOAD</p></td>
<td class="cellrowborder" valign="top" width="81.78999999999999%" headers="mcps1.2.3.1.2 "><p id="p97591441635"><a name="p97591441635"></a><a name="p97591441635"></a>Vector read UB pipeline in the SIMD_VF function.</p>
<p id="p18760948145415"><a name="p18760948145415"></a><a name="p18760948145415"></a>Corresponds to UB-to-register transfer instructions, such as LoadAlign, LoadUnAlign, and Load.</p></td>
</tr>
<tr id="row11773440341"><td class="cellrowborder" valign="top" width="18.21%" headers="mcps1.2.3.1.1 "><p id="p10132448173420"><a name="p10132448173420"></a><a name="p10132448173420"></a>SCALAR_STORE</p></td>
<td class="cellrowborder" valign="top" width="81.78999999999999%" headers="mcps1.2.3.1.2 "><p id="p462923115592"><a name="p462923115592"></a><a name="p462923115592"></a>Scalar write UB pipeline in the SIMD_VF function.</p>
<p id="p1012020395557"><a name="p1012020395557"></a><a name="p1012020395557"></a>Corresponds to scalar write-to-UB instructions, such as Duplicate.</p></td>
</tr>
<tr id="row19615183817191"><td class="cellrowborder" valign="top" width="18.21%" headers="mcps1.2.3.1.1 "><p id="p1484519586432"><a name="p1484519586432"></a><a name="p1484519586432"></a>SCALAR_LOAD</p></td>
<td class="cellrowborder" valign="top" width="81.78999999999999%" headers="mcps1.2.3.1.2 "><p id="p1415113416592"><a name="p1415113416592"></a><a name="p1415113416592"></a>Scalar read UB pipeline within the SIMD_VF function.</p>
<p id="p1539022412570"><a name="p1539022412570"></a><a name="p1539022412570"></a>Corresponds to instructions that read scalars from UB, such as <a href="../../data_structures/LocalTensor/GetValue.md#section618mcpsimp">GetValue</a>.</p></td>
</tr>
<tr id="row114116587598"><td class="cellrowborder" valign="top" width="18.21%" headers="mcps1.2.3.1.1 "><p id="p141415587592"><a name="p141415587592"></a><a name="p141415587592"></a>VEC_ALL</p></td>
<td class="cellrowborder" valign="top" width="81.78999999999999%" headers="mcps1.2.3.1.2 "><p id="p12141195817592"><a name="p12141195817592"></a><a name="p12141195817592"></a>All vector read/write UB pipelines within the SIMD_VF function.</p></td>
</tr>
<tr id="row3313711801"><td class="cellrowborder" valign="top" width="18.21%" headers="mcps1.2.3.1.1 "><p id="p153131014015"><a name="p153131014015"></a><a name="p153131014015"></a>SCALAR_ALL</p></td>
<td class="cellrowborder" valign="top" width="81.78999999999999%" headers="mcps1.2.3.1.2 "><p id="p12313911005"><a name="p12313911005"></a><a name="p12313911005"></a>All scalar read/write UB pipelines within the SIMD_VF function.</p></td>
</tr>
</tbody>
</table>

**Table 3** Value description of src and dst combinations

<a name="table913981754616"></a>
<table><thead align="left"><tr id="row817651712466"><th class="cellrowborder" valign="top" width="48.42%" id="mcps1.2.3.1.1"><p id="p1017614172466"><a name="p1017614172466"></a><a name="p1017614172466"></a>src</p></th>
<th class="cellrowborder" valign="top" width="51.580000000000005%" id="mcps1.2.3.1.2"><p id="p522719341678"><a name="p522719341678"></a><a name="p522719341678"></a>dst</p></th>
</tr>
</thead>
<tbody><tr id="row121761817164615"><td class="cellrowborder" valign="top" width="48.42%" headers="mcps1.2.3.1.1 "><p id="p15176181784620"><a name="p15176181784620"></a><a name="p15176181784620"></a>VEC_STORE</p></td>
<td class="cellrowborder" valign="top" width="51.580000000000005%" headers="mcps1.2.3.1.2 "><p id="p9227034075"><a name="p9227034075"></a><a name="p9227034075"></a>VEC_LOAD</p></td>
</tr>
<tr id="row15176917194614"><td class="cellrowborder" valign="top" width="48.42%" headers="mcps1.2.3.1.1 "><p id="p817611744614"><a name="p817611744614"></a><a name="p817611744614"></a>VEC_LOAD</p></td>
<td class="cellrowborder" valign="top" width="51.580000000000005%" headers="mcps1.2.3.1.2 "><p id="p15227163416718"><a name="p15227163416718"></a><a name="p15227163416718"></a>VEC_STORE</p></td>
</tr>
<tr id="row417616179465"><td class="cellrowborder" valign="top" width="48.42%" headers="mcps1.2.3.1.1 "><p id="p101765179461"><a name="p101765179461"></a><a name="p101765179461"></a>VEC_STORE</p></td>
<td class="cellrowborder" valign="top" width="51.580000000000005%" headers="mcps1.2.3.1.2 "><p id="p22275341710"><a name="p22275341710"></a><a name="p22275341710"></a>VEC_STORE</p></td>
</tr>
<tr id="row717714175468"><td class="cellrowborder" valign="top" width="48.42%" headers="mcps1.2.3.1.1 "><p id="p817716171466"><a name="p817716171466"></a><a name="p817716171466"></a>VEC_STORE</p></td>
<td class="cellrowborder" valign="top" width="51.580000000000005%" headers="mcps1.2.3.1.2 "><p id="p1352711201015"><a name="p1352711201015"></a><a name="p1352711201015"></a>SCALAR_LOAD</p></td>
</tr>
<tr id="row417718174461"><td class="cellrowborder" valign="top" width="48.42%" headers="mcps1.2.3.1.1 "><p id="p71771217204611"><a name="p71771217204611"></a><a name="p71771217204611"></a>VEC_STORE</p></td>
<td class="cellrowborder" valign="top" width="51.580000000000005%" headers="mcps1.2.3.1.2 "><p id="p17227103411714"><a name="p17227103411714"></a><a name="p17227103411714"></a>SCALAR_STORE</p></td>
</tr>
<tr id="row10177101774614"><td class="cellrowborder" valign="top" width="48.42%" headers="mcps1.2.3.1.1 "><p id="p0117501496"><a name="p0117501496"></a><a name="p0117501496"></a>VEC_LOAD</p></td>
<td class="cellrowborder" valign="top" width="51.580000000000005%" headers="mcps1.2.3.1.2 "><p id="p10227113416712"><a name="p10227113416712"></a><a name="p10227113416712"></a>SCALAR_STORE</p></td>
</tr>
<tr id="row8177181704613"><td class="cellrowborder" valign="top" width="48.42%" headers="mcps1.2.3.1.1 "><p id="p9177121764614"><a name="p9177121764614"></a><a name="p9177121764614"></a>SCALAR_STORE</p></td>
<td class="cellrowborder" valign="top" width="51.580000000000005%" headers="mcps1.2.3.1.2 "><p id="p19227173413720"><a name="p19227173413720"></a><a name="p19227173413720"></a>VEC_LOAD</p></td>
</tr>
<tr id="row617721711469"><td class="cellrowborder" valign="top" width="48.42%" headers="mcps1.2.3.1.1 "><p id="p181771817134614"><a name="p181771817134614"></a><a name="p181771817134614"></a>SCALAR_STORE</p></td>
<td class="cellrowborder" valign="top" width="51.580000000000005%" headers="mcps1.2.3.1.2 "><p id="p12227234671"><a name="p12227234671"></a><a name="p12227234671"></a>VEC_STORE</p></td>
</tr>
<tr id="row417721720462"><td class="cellrowborder" valign="top" width="48.42%" headers="mcps1.2.3.1.1 "><p id="p171772017114610"><a name="p171772017114610"></a><a name="p171772017114610"></a>SCALAR_LOAD</p></td>
<td class="cellrowborder" valign="top" width="51.580000000000005%" headers="mcps1.2.3.1.2 "><p id="p12277343713"><a name="p12277343713"></a><a name="p12277343713"></a>VEC_STORE</p></td>
</tr>
<tr id="row51771517144617"><td class="cellrowborder" valign="top" width="48.42%" headers="mcps1.2.3.1.1 "><p id="p617751711465"><a name="p617751711465"></a><a name="p617751711465"></a>VEC_ALL</p></td>
<td class="cellrowborder" valign="top" width="51.580000000000005%" headers="mcps1.2.3.1.2 "><p id="p182276341710"><a name="p182276341710"></a><a name="p182276341710"></a>VEC_ALL</p></td>
</tr>
<tr id="row12177717134614"><td class="cellrowborder" valign="top" width="48.42%" headers="mcps1.2.3.1.1 "><p id="p9177191719460"><a name="p9177191719460"></a><a name="p9177191719460"></a>VEC_ALL</p></td>
<td class="cellrowborder" valign="top" width="51.580000000000005%" headers="mcps1.2.3.1.2 "><p id="p322793420715"><a name="p322793420715"></a><a name="p322793420715"></a>SCALAR_ALL</p></td>
</tr>
<tr id="row31781317194619"><td class="cellrowborder" valign="top" width="48.42%" headers="mcps1.2.3.1.1 "><p id="p12178617194619"><a name="p12178617194619"></a><a name="p12178617194619"></a>SCALAR_ALL</p></td>
<td class="cellrowborder" valign="top" width="51.580000000000005%" headers="mcps1.2.3.1.2 "><p id="p2022715341479"><a name="p2022715341479"></a><a name="p2022715341479"></a>VEC_ALL</p></td>
</tr>
</tbody>
</table>

## Return Value<a name="section640mcpsimp"></a>

None

## Constraints<a name="section633mcpsimp"></a>

None

## Example<a name="section642mcpsimp"></a>

In the following example, **dstPtr** and **src0Ptr** point to the same UB address. In the second iteration of the for loop, the read UB vector transfer and the write UB vector transfer in the first iteration operate on the same UB address space. Therefore, the read UB vector transfer in the second iteration must wait until the write UB vector transfer in the first iteration completes before it can execute, which requires inserting synchronization for **VEC_LOAD** to wait for **VEC_STORE**.

```
template<typename T>
__simd_vf__ inline void AddVF(__ubuf__ T* dstAddr, __ubuf__ T* src0Addr, __ubuf__ T* src1Addr, uint32_t count, uint32_t oneRepeatSize, uint16_t repeatTimes)
{
    AscendC::Reg::RegTensor<T> srcReg0;
    AscendC::Reg::RegTensor<T> srcReg1;
    AscendC::Reg::RegTensor<T> dstReg;
    AscendC::Reg::MaskReg mask;   
    for (uint16_t i = 0; i < repeatTimes; i++) {
        mask = AscendC::Reg::UpdateMask<T>(count);
        AscendC::Reg::LocalMemBar<AscendC::Reg::MemType::VEC_STORE, AscendC::Reg::MemType::VEC_LOAD>();
        AscendC::Reg::LoadAlign(srcReg0, src0Addr);
        AscendC::Reg::LoadAlign(srcReg1, src1Addr + i * oneRepeatSize);
        AscendC::Reg::Add(dstReg, srcReg0, srcReg1, mask);
        AscendC::Reg::StoreAlign(dstAddr, dstReg, mask);
    }
}
```
