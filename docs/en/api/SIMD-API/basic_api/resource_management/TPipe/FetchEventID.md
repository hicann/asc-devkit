# FetchEventID<a name="ZH-CN_TOPIC_0000001791531573"></a>

<!-- md-trans-meta sourceCommit=f6490d1e030d257ee55f3f1600bffece5a8bd3f7 translatedAt=2026-08-27T14:32:43.105Z -->

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

Obtains an available **TEventID** based on **HardEvent** (a hardware-type synchronization event). This API does not apply for a **TEventID**; it only provides an available **TEventID**.

## Prototype<a name="section620mcpsimp"></a>

```
template <HardEvent evt>
__aicore__ inline TEventID FetchEventID()
__aicore__ inline TEventID FetchEventID(HardEvent evt)
```

## Parameters<a name="section622mcpsimp"></a>

<a name="table193329316393"></a>
<table><thead align="left"><tr id="row123331131153919"><th class="cellrowborder" valign="top" width="12.36%" id="mcps1.1.4.1.1"><p id="p8333133153913"><a name="p8333133153913"></a><a name="p8333133153913"></a>Parameter</p></th>
<th class="cellrowborder" valign="top" width="12.379999999999999%" id="mcps1.1.4.1.2"><p id="p518118718459"><a name="p518118718459"></a><a name="p518118718459"></a>Input/Output</p></th>
<th class="cellrowborder" valign="top" width="75.26%" id="mcps1.1.4.1.3"><p id="p833353113393"><a name="p833353113393"></a><a name="p833353113393"></a>Description</p></th>
</tr>
</thead>
<tbody><tr id="row11660173845017"><td class="cellrowborder" valign="top" width="12.36%" headers="mcps1.1.4.1.1 "><p id="p143161011192917"><a name="p143161011192917"></a><a name="p143161011192917"></a>**evt**</p></td>
<td class="cellrowborder" valign="top" width="12.379999999999999%" headers="mcps1.1.4.1.2 "><p id="p9331531865"><a name="p9331531865"></a><a name="p9331531865"></a>Input</p></td>
<td class="cellrowborder" valign="top" width="75.26%" headers="mcps1.1.4.1.3 "><p id="p143308311967"><a name="p143308311967"></a><a name="p143308311967"></a>**HardEvent** type, indicating the hardware synchronization type.</p>
<p id="p15571184910124"><a name="p15571184910124"></a><a name="p15571184910124"></a>For details about this type, see the synchronization type description in <a href="../../sync_control/intra_core_sync/SetFlag_WaitFlag_ISASI.md">SetFlag/WaitFlag(ISASI)</a>.</p></td>
</tr>
</tbody>
</table>

## Constraints<a name="section633mcpsimp"></a>

Compared with [AllocEventID](AllocEventID.md), **FetchEventID** is suitable for scenarios where an ID is used temporarily. After the ID is obtained, it is not occupied. In some complex usage scenarios, developers must ensure correct usage on their own. For example, when **SetFlag**/**WaitFlag** are called consecutively in the same pipeline, if the IDs passed in both calls are obtained using **FetchEventID**, the two IDs are the same, which may cause undefined behavior such as program deadlock. In this case, it is recommended that users use **AllocEventID**.

## Return Value<a name="section640mcpsimp"></a>

TEventID

## Example<a name="section6191129670"></a>

```
AscendC::TEventID eventIdVToS = GetTPipePtr()->FetchEventID(AscendC::HardEvent::V_S); //Synchronization of vectors such as scalar needs to be inserted, and the ID of the corresponding HardEvent needs to be applied for.
AscendC::SetFlag<AscendC::HardEvent::V_S>(eventIdVToS);
AscendC::WaitFlag<AscendC::HardEvent::V_S>(eventIdVToS);
```

