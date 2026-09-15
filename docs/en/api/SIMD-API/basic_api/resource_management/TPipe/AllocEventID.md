# AllocEventID<a name="ZH-CN_TOPIC_0000001791611817"></a>

<!-- md-trans-meta sourceCommit=f6490d1e030d257ee55f3f1600bffece5a8bd3f7 translatedAt=2026-08-27T14:30:58.832Z -->

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

Applies for the **TEventID** of a **HardEvent** (hardware synchronization event). It must be used together with [ReleaseEventID](ReleaseEventID.md). After this API is called, the applied **TEventID** is occupied until **ReleaseEventID** is called to release it.

## Prototype<a name="section620mcpsimp"></a>

```
template <HardEvent evt>
__aicore__ inline TEventID AllocEventID()
```

## Parameters<a name="section622mcpsimp"></a>

**Table 1** Template parameter description

<a name="table193329316393"></a>
<table><thead align="left"><tr id="row123331131153919"><th class="cellrowborder" valign="top" width="16.29%" id="mcps1.2.3.1.1"><p id="p8333133153913"><a name="p8333133153913"></a><a name="p8333133153913"></a>Parameter</p></th>
<th class="cellrowborder" valign="top" width="83.71%" id="mcps1.2.3.1.2"><p id="p833353113393"><a name="p833353113393"></a><a name="p833353113393"></a>Description</p></th>
</tr>
</thead>
<tbody><tr id="row11660173845017"><td class="cellrowborder" valign="top" width="16.29%" headers="mcps1.2.3.1.1 "><p id="p4571114971214"><a name="p4571114971214"></a><a name="p4571114971214"></a>evt</p></td>
<td class="cellrowborder" valign="top" width="83.71%" headers="mcps1.2.3.1.2 "><p id="p15571184910124"><a name="p15571184910124"></a><a name="p15571184910124"></a>**HardEvent** hardware synchronization type. For details about this type, see the synchronization type description in <a href="../../sync_control/intra_core_sync/SetFlag_WaitFlag_ISASI.md">SetFlag/WaitFlag(ISASI)</a>.</p></td>
</tr>
</tbody>
</table>

## Constraints<a name="section633mcpsimp"></a>

The number of **TEventID** instances is limited. After use, call **ReleaseEventID** immediately to release them to prevent **TEventID** exhaustion.

## Return Value<a name="section640mcpsimp"></a>

**TEventID**

## Example<a name="section642mcpsimp"></a>

```
AscendC::TEventID eventID = GetTPipePtr()->AllocEventID<AscendC::HardEvent::V_S>(); //Apply for the ID of the corresponding HardEvent to insert synchronization for vectors such as scalar.
AscendC::SetFlag<AscendC::HardEvent::V_S>(eventID);
......
......
......
AscendC::WaitFlag<AscendC::HardEvent::V_S>(eventID);
GetTPipePtr()->ReleaseEventID<AscendC::HardEvent::V_S>(eventID); //Release the ID of the HardEvent for synchronizing vectors such as scalar.
......
```

