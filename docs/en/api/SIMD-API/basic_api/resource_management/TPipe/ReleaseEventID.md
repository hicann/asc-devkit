# ReleaseEventID<a name="ZH-CN_TOPIC_0000001744571864"></a>

<!-- md-trans-meta sourceCommit=f6490d1e030d257ee55f3f1600bffece5a8bd3f7 translatedAt=2026-08-27T14:36:58.653Z -->

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

Releases the **TEventID** of a **HardEvent** (hardware-type synchronization event), typically used together with [AllocEventID](AllocEventID.md).

## Prototype<a name="section620mcpsimp"></a>

```
template <HardEvent evt>
__aicore__ inline void ReleaseEventID(TEventID id)
```

## Parameters<a name="section622mcpsimp"></a>

**Table 1** Template parameter description

<a name="table58993346461"></a>
<table><thead align="left"><tr id="row88991534124617"><th class="cellrowborder" valign="top" width="12.23%" id="mcps1.2.3.1.1"><p id="p5899163484616"><a name="p5899163484616"></a><a name="p5899163484616"></a>Parameter</p></th>
<th class="cellrowborder" valign="top" width="87.77000000000001%" id="mcps1.2.3.1.2"><p id="p1889953419469"><a name="p1889953419469"></a><a name="p1889953419469"></a>Description</p></th>
</tr>
</thead>
<tbody><tr id="row18991634144619"><td class="cellrowborder" valign="top" width="12.23%" headers="mcps1.2.3.1.1 "><p id="p118996346463"><a name="p118996346463"></a><a name="p118996346463"></a>evt</p></td>
<td class="cellrowborder" valign="top" width="87.77000000000001%" headers="mcps1.2.3.1.2 "><p id="p1389943411468"><a name="p1389943411468"></a><a name="p1389943411468"></a>Hardware synchronization type of **HardEvent**. For details about this type, see the synchronization type description in <a href="../../sync_control/intra_core_sync/SetFlag_WaitFlag_ISASI.md">SetFlag/WaitFlag(ISASI)</a>.</p></td>
</tr>
</tbody>
</table>

**Table 2** Parameter description

<a name="table193329316393"></a>
<table><thead align="left"><tr id="row123331131153919"><th class="cellrowborder" valign="top" width="12.36%" id="mcps1.2.4.1.1"><p id="p8333133153913"><a name="p8333133153913"></a><a name="p8333133153913"></a>Parameter</p></th>
<th class="cellrowborder" valign="top" width="12.379999999999999%" id="mcps1.2.4.1.2"><p id="p518118718459"><a name="p518118718459"></a><a name="p518118718459"></a>Input/Output</p></th>
<th class="cellrowborder" valign="top" width="75.26%" id="mcps1.2.4.1.3"><p id="p833353113393"><a name="p833353113393"></a><a name="p833353113393"></a>Description</p></th>
</tr>
</thead>
<tbody><tr id="row11660173845017"><td class="cellrowborder" valign="top" width="12.36%" headers="mcps1.2.4.1.1 "><p id="p18393187172718"><a name="p18393187172718"></a><a name="p18393187172718"></a>**id**</p></td>
<td class="cellrowborder" valign="top" width="12.379999999999999%" headers="mcps1.2.4.1.2 "><p id="p1461163910269"><a name="p1461163910269"></a><a name="p1461163910269"></a>Input</p></td>
<td class="cellrowborder" valign="top" width="75.26%" headers="mcps1.2.4.1.3 "><p id="p143308311967"><a name="p143308311967"></a><a name="p143308311967"></a>**TEventID** type, which is the **TEventID** obtained by calling <a href="AllocEventID.md">AllocEventID</a>.</p></td>
</tr>
</tbody>
</table>

## Constraints<a name="section633mcpsimp"></a>

**AllocEventID** and **ReleaseEventID** must be used in pairs. The **TEventID** passed to **ReleaseEventID** must be obtained from the corresponding **AllocEventID**.

## Return Value<a name="section640mcpsimp"></a>

None

## Example<a name="section882216131071"></a>

```
AscendC::TEventID eventID = GetTPipePtr()->AllocEventID<AscendC::HardEvent::V_S>(); //Apply for the corresponding HardEvent ID for synchronization of vectors such as scalar.
AscendC::SetFlag<AscendC::HardEvent::V_S>(eventID);
......
......
......
AscendC::WaitFlag<AscendC::HardEvent::V_S>(eventID);
GetTPipePtr()->ReleaseEventID<AscendC::HardEvent::V_S>(eventID); //Release the HardEvent ID for synchronization of vectors such as scalar.
......
```

