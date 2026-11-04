# AllocEventID<a name="ZH-CN_TOPIC_0000001791611817"></a>

<!-- md-trans-meta sourceCommit=754fb95859dead826727a591e674261c6a829a08 translatedAt=2026-09-23T17:39:31.739Z pushedAt=2026-10-26T07:41:44.671Z nl=2 -->

## Applicable Products<a name="section1550532418810"></a>

<!-- npu="950" id1 -->
- Ascend 950PR&950DT products: supported
<!-- end id1 -->
<!-- npu="A3" id2 -->
- Atlas A3 products: supported
<!-- end id2 -->
<!-- npu="910b" id3 -->
- Atlas A2 products: supported
<!-- end id3 -->
<!-- npu="310b" id4 -->
- Atlas 200I/500 A2 inference products: not supported
<!-- end id4 -->
<!-- npu="310p" id5 -->
- Atlas inference products AI Core: supported
<!-- end id5 -->
<!-- npu="310p" id6 -->
- Atlas inference products Vector Core: not supported
<!-- end id6 -->
<!-- npu="910" id7 -->
- Atlas training products: supported
<!-- end id7 -->
<!-- @ref: asc-devkit/res/docs/zh/api/SIMD-API/basic_api/resource_management/TPipe/AllocEventID_res.md#id1 -->

## Description<a name="section618mcpsimp"></a>

The header file path is `"basic_api/kernel_tpipe.h"`.

Applies for the **TEventID** of a **HardEvent** (hardware-type synchronization event). It must be used together with [ReleaseEventID](ReleaseEventID.md). After this API is called, the applied **TEventID** is occupied until **ReleaseEventID** is called to release it.

## Function Prototype<a name="section620mcpsimp"></a>

```cpp
template <HardEvent evt>
__aicore__ inline TEventID AllocEventID()
```

## Parameters<a name="section622mcpsimp"></a>

**Table 1**  Template parameter description

<a name="table193329316393"></a>
<table><thead align="left"><tr id="row123331131153919"><th class="cellrowborder" valign="top" width="16.29%" id="mcps1.2.3.1.1"><p id="p8333133153913"><a name="p8333133153913"></a><a name="p8333133153913"></a>Parameter</p></th>
<th class="cellrowborder" valign="top" width="83.71%" id="mcps1.2.3.1.2"><p id="p833353113393"><a name="p833353113393"></a><a name="p833353113393"></a>Description</p></th>
</tr>
</thead>
<tbody><tr id="row11660173845017"><td class="cellrowborder" valign="top" width="16.29%" headers="mcps1.2.3.1.1 "><p id="p4571114971214"><a name="p4571114971214"></a><a name="p4571114971214"></a>evt</p></td>
<td class="cellrowborder" valign="top" width="83.71%" headers="mcps1.2.3.1.2 "><p id="p15571184910124"><a name="p15571184910124"></a><a name="p15571184910124"></a>HardEvent hardware synchronization type. For details about this type, see the description of synchronization types in <a href="../../sync_control/intra_core_sync/SetFlag_WaitFlag_ISASI.md">SetFlag/WaitFlag(ISASI)</a>.</p></td>
</tr>
</tbody>
</table>

## Constraints<a name="section633mcpsimp"></a>

**TEventID** has a limited quantity. After use, call **ReleaseEventID** immediately to release it to prevent **TEventID** from being exhausted.

## Return Value<a name="section640mcpsimp"></a>

TEventID

## Example<a name="section642mcpsimp"></a>

```cpp
AscendC::TEventID eventID = GetTPipePtr()->AllocEventID<AscendC::HardEvent::V_S>(); // Insert synchronization for scalar and other vectors, and apply for the corresponding HardEvent ID.
AscendC::SetFlag<AscendC::HardEvent::V_S>(eventID);
......
......
......
AscendC::WaitFlag<AscendC::HardEvent::V_S>(eventID);
GetTPipePtr()->ReleaseEventID<AscendC::HardEvent::V_S>(eventID); // Release the HardEvent ID for scalar and other vector synchronization.
......
```
