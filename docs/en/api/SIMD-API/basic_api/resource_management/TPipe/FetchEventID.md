# FetchEventID<a name="ZH-CN_TOPIC_0000001791531573"></a>

<!-- md-trans-meta sourceCommit=754fb95859dead826727a591e674261c6a829a08 translatedAt=2026-09-23T17:41:18.677Z pushedAt=2026-10-26T07:41:44.675Z nl=2 -->

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
- Atlas 200I/500 A2 inference product: not supported
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
<!-- @ref: asc-devkit/res/docs/zh/api/SIMD-API/basic_api/resource_management/TPipe/FetchEventID_res.md#id1 -->

## Description<a name="section618mcpsimp"></a>

The header file path is `"basic_api/kernel_tpipe.h"`.

Obtains the corresponding available **TEventID** based on **HardEvent** (a hardware-type synchronization event). This API does not apply for a **TEventID**; it only provides an available **TEventID**.

## Prototype<a name="section620mcpsimp"></a>

```cpp
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
<tbody><tr id="row11660173845017"><td class="cellrowborder" valign="top" width="12.36%" headers="mcps1.1.4.1.1 "><p id="p143161011192917"><a name="p143161011192917"></a><a name="p143161011192917"></a>evt</p></td>
<td class="cellrowborder" valign="top" width="12.379999999999999%" headers="mcps1.1.4.1.2 "><p id="p9331531865"><a name="p9331531865"></a><a name="p9331531865"></a>Input</p></td>
<td class="cellrowborder" valign="top" width="75.26%" headers="mcps1.1.4.1.3 "><p id="p143308311967"><a name="p143308311967"></a><a name="p143308311967"></a>HardEvent type, which is a hardware synchronization type.</p>
<p id="p15571184910124"><a name="p15571184910124"></a><a name="p15571184910124"></a>For details about this type, see the synchronization type description in <a href="../../sync_control/intra_core_sync/SetFlag_WaitFlag_ISASI.md">SetFlag/WaitFlag(ISASI)</a>.</p></td>
</tr>
</tbody>
</table>

## Constraints<a name="section633mcpsimp"></a>

Compared with [AllocEventID](AllocEventID.md), **FetchEventID** is suitable for scenarios where an ID is used temporarily. After the ID is obtained, it is not occupied. In some complex usage scenarios, developers need to ensure correct usage on their own. For example, when **SetFlag**/**WaitFlag** are called consecutively in the same pipeline, if the IDs passed in both calls are obtained using **FetchEventID**, the two IDs are the same, which may cause undefined behaviors such as program hang. In this case, it is recommended to use **AllocEventID**.

## Return Value<a name="section640mcpsimp"></a>

TEventID

## Example<a name="section6191129670"></a>

```cpp
AscendC::TEventID eventIdVToS = GetTPipePtr()->FetchEventID(AscendC::HardEvent::V_S); //Apply for the ID of the corresponding HardEvent to insert synchronization such as scalar waiting for vector.
AscendC::SetFlag<AscendC::HardEvent::V_S>(eventIdVToS);
AscendC::WaitFlag<AscendC::HardEvent::V_S>(eventIdVToS);
```
