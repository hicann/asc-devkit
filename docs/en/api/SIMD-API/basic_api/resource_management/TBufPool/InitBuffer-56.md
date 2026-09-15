# InitBuffer<a name="ZH-CN_TOPIC_0000001866643004"></a>

<!-- md-trans-meta sourceCommit=f6490d1e030d257ee55f3f1600bffece5a8bd3f7 translatedAt=2026-08-27T14:28:44.342Z -->

## Applicable Products<a name="section1550532418810"></a>

<!-- npu="950" id1 -->
- Ascend 950PR/Ascend 950DT: Supported
<!-- end id1 -->
<!-- npu="A3" id2 -->
- Atlas A3 training products/Atlas A3 inference products: Supported
<!-- end id2 -->
<!-- npu="910b" id3 -->
- Atlas A2 training products/Atlas A2 inference products: Supported
<!-- end id3 -->
<!-- npu="310b" id4 -->
- Atlas 200I/500 A2 inference products: Not supported
<!-- end id4 -->
<!-- npu="310p" id5 -->
- Atlas inference products AI Core: Supported
<!-- end id5 -->
<!-- npu="310p" id6 -->
- Atlas inference products Vector Core: Not supported
<!-- end id6 -->
<!-- npu="910" id7 -->
- Atlas training products: Supported
<!-- end id7 -->


## Description<a name="section618mcpsimp"></a>

Calls the **TBufPool::InitBuffer** API to allocate memory for **TQue**/**TBuf**.

## Prototype<a name="section620mcpsimp"></a>

```
template <class T> __aicore__ inline bool InitBuffer(T& que, uint8_t num, uint32_t len)
template <TPosition pos> __aicore__ inline bool InitBuffer(TBuf<pos>& buf, uint32_t len)
```

## Parameters<a name="section622mcpsimp"></a>

**Table 1** Template parameter description

<a name="table1550165916920"></a>
<table><thead align="left"><tr id="row115015591391"><th class="cellrowborder" valign="top" width="12.139999999999999%" id="mcps1.2.3.1.1"><p id="p12501159099"><a name="p12501159099"></a><a name="p12501159099"></a>Parameter</p></th>
<th class="cellrowborder" valign="top" width="87.86%" id="mcps1.2.3.1.2"><p id="p85019592918"><a name="p85019592918"></a><a name="p85019592918"></a>Description</p></th>
</tr>
</thead>
<tbody><tr id="row1550117591914"><td class="cellrowborder" valign="top" width="12.139999999999999%" headers="mcps1.2.3.1.1 "><p id="p185019592913"><a name="p185019592913"></a><a name="p185019592913"></a>T</p></td>
<td class="cellrowborder" valign="top" width="87.86%" headers="mcps1.2.3.1.2 "><p id="p12101541625"><a name="p12101541625"></a><a name="p12101541625"></a><span>Type of the **que** parameter.</span></p></td>
</tr>
<tr id="row758993611212"><td class="cellrowborder" valign="top" width="12.139999999999999%" headers="mcps1.2.3.1.1 "><p id="p12692424332"><a name="p12692424332"></a><a name="p12692424332"></a>pos</p></td>
<td class="cellrowborder" valign="top" width="87.86%" headers="mcps1.2.3.1.2 "><p id="p35011591693"><a name="p35011591693"></a><a name="p35011591693"></a>Logical position of the buffer, which can be <span>**VECIN**, **VECOUT**, </span>**VECCALC**, **A1**<span>, </span>**B1**, or **C1**.<span>For details about **TPosition**, see </span><a href="../../aux_data_structures/TPosition.md">TPosition</a>.</p></td>
</tr>
</tbody>
</table>

**Table 2** Parameter description of the InitBuffer\(T& que, uint8\_t num, uint32\_t len\) prototype

<a name="table193329316393"></a>
<table><thead align="left"><tr id="row123331131153919"><th class="cellrowborder" valign="top" width="12.36%" id="mcps1.2.4.1.1"><p id="p8333133153913"><a name="p8333133153913"></a><a name="p8333133153913"></a>Parameter</p></th>
<th class="cellrowborder" valign="top" width="12.379999999999999%" id="mcps1.2.4.1.2"><p id="p518118718459"><a name="p518118718459"></a><a name="p518118718459"></a>Input/Output</p></th>
<th class="cellrowborder" valign="top" width="75.26%" id="mcps1.2.4.1.3"><p id="p833353113393"><a name="p833353113393"></a><a name="p833353113393"></a>Description</p></th>
</tr>
</thead>
<tbody><tr id="row11660173845017"><td class="cellrowborder" valign="top" width="12.36%" headers="mcps1.2.4.1.1 "><p id="p466053810507"><a name="p466053810507"></a><a name="p466053810507"></a>que</p></td>
<td class="cellrowborder" valign="top" width="12.379999999999999%" headers="mcps1.2.4.1.2 "><p id="p885774605014"><a name="p885774605014"></a><a name="p885774605014"></a>Input</p></td>
<td class="cellrowborder" valign="top" width="75.26%" headers="mcps1.2.4.1.3 "><p id="p0660153818501"><a name="p0660153818501"></a><a name="p0660153818501"></a>**TQue** object for which memory is to be allocated.</p></td>
</tr>
<tr id="row03336319398"><td class="cellrowborder" valign="top" width="12.36%" headers="mcps1.2.4.1.1 "><p id="p11399116193313"><a name="p11399116193313"></a><a name="p11399116193313"></a>num</p></td>
<td class="cellrowborder" valign="top" width="12.379999999999999%" headers="mcps1.2.4.1.2 "><p id="p111819774511"><a name="p111819774511"></a><a name="p111819774511"></a>Input</p></td>
<td class="cellrowborder" valign="top" width="75.26%" headers="mcps1.2.4.1.3 "><p id="p6383173514333"><a name="p6383173514333"></a><a name="p6383173514333"></a>Number of memory blocks to be allocated.</p></td>
</tr>
<tr id="row1430772593316"><td class="cellrowborder" valign="top" width="12.36%" headers="mcps1.2.4.1.1 "><p id="p530752514330"><a name="p530752514330"></a><a name="p530752514330"></a>len</p></td>
<td class="cellrowborder" valign="top" width="12.379999999999999%" headers="mcps1.2.4.1.2 "><p id="p530711252335"><a name="p530711252335"></a><a name="p530711252335"></a>Input</p></td>
<td class="cellrowborder" valign="top" width="75.26%" headers="mcps1.2.4.1.3 "><p id="p183081251336"><a name="p183081251336"></a><a name="p183081251336"></a>Size of each memory block, in bytes. If the size is not aligned to 32 bytes, it is automatically rounded up to 32-byte alignment.</p></td>
</tr>
</tbody>
</table>

**Table 3** Parameter description of the InitBuffer(TBuf<pos\>& buf, uint32_t len) prototype

<a name="table5376122715308"></a>
<table><thead align="left"><tr id="row1337716275309"><th class="cellrowborder" valign="top" width="12.36%" id="mcps1.2.4.1.1"><p id="p1537762711305"><a name="p1537762711305"></a><a name="p1537762711305"></a>Parameter</p></th>
<th class="cellrowborder" valign="top" width="12.370000000000001%" id="mcps1.2.4.1.2"><p id="p153771127123013"><a name="p153771127123013"></a><a name="p153771127123013"></a>Input/Output</p></th>
<th class="cellrowborder" valign="top" width="75.27000000000001%" id="mcps1.2.4.1.3"><p id="p17377162715303"><a name="p17377162715303"></a><a name="p17377162715303"></a>Description</p></th>
</tr>
</thead>
<tbody><tr id="row19377627133012"><td class="cellrowborder" valign="top" width="12.36%" headers="mcps1.2.4.1.1 "><p id="p737710279307"><a name="p737710279307"></a><a name="p737710279307"></a>buf</p></td>
<td class="cellrowborder" valign="top" width="12.370000000000001%" headers="mcps1.2.4.1.2 "><p id="p13377122733010"><a name="p13377122733010"></a><a name="p13377122733010"></a>Input</p></td>
<td class="cellrowborder" valign="top" width="75.27000000000001%" headers="mcps1.2.4.1.3 "><p id="p19377102793016"><a name="p19377102793016"></a><a name="p19377102793016"></a>**TBuf** object for which memory is to be allocated.</p></td>
</tr>
<tr id="row13377162793019"><td class="cellrowborder" valign="top" width="12.36%" headers="mcps1.2.4.1.1 "><p id="p5377527113018"><a name="p5377527113018"></a><a name="p5377527113018"></a>**len**</p></td>
<td class="cellrowborder" valign="top" width="12.370000000000001%" headers="mcps1.2.4.1.2 "><p id="p12377122712304"><a name="p12377122712304"></a><a name="p12377122712304"></a>Input</p></td>
<td class="cellrowborder" valign="top" width="75.27000000000001%" headers="mcps1.2.4.1.3 "><p id="p6514716314"><a name="p6514716314"></a><a name="p6514716314"></a>Memory size allocated for **TBuf**, in bytes. If the size is not 32-byte aligned, it is automatically rounded up to 32-byte alignment.</p></td>
</tr>
</tbody>
</table>

## Constraints<a name="section633mcpsimp"></a>

When declaring **TBufPool**, you can use **bufIDSize** to specify the maximum number of allocatable buffers. The default upper limit is 4, and the maximum is 16. The physical memory of **TQue** or **TBuf** must be consistent with that of **TBufPool**.

## Return Value<a name="section640mcpsimp"></a>

None

## Example<a name="section642mcpsimp"></a>

See [InitBufPool](InitBufPool-55.md).
