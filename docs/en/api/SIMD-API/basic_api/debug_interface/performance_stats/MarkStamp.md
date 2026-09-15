# MarkStamp<a name="ZH-CN_TOPIC_0000002491669476"></a>

<!-- md-trans-meta sourceCommit=1de5b49f75a2c8f83a238b6ccbb54bf6cf8fb65c translatedAt=2026-08-27T11:24:31.346Z -->

## Applicable Products<a name="section7433822121115"></a>

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


## Description<a name="section165477552317"></a>

It is recommended to use the [asc\_mark\_stamp](../../../../Utils-API/tuning_interface/asc_mark_stamp.md) API to mark specific positions. This API applies to both C and C++ programming.

By calling the API, users can mark specific positions during operator execution, facilitating later analysis of code execution paths and performance hotspots through the pipeline graph.

## Prototype<a name="section054795512320"></a>

```
template<pipe_t pipe, uint16_t index>
__aicore__ inline void MarkStamp()

template<pipe_t pipe>
__aicore__ inline void MarkStamp(uint16_t index)
```

## Parameters<a name="section2054811551732"></a>

<a name="table18824164416323"></a>
<table><thead align="left"><tr id="row4824124418325"><th class="cellrowborder" valign="top" width="50%" id="mcps1.1.3.1.1"><p id="p58240441320"><a name="p58240441320"></a><a name="p58240441320"></a>Parameter</p></th>
<th class="cellrowborder" valign="top" width="50%" id="mcps1.1.3.1.2"><p id="p2824114412328"><a name="p2824114412328"></a><a name="p2824114412328"></a>Description</p></th>
</tr>
</thead>
<tbody><tr id="row1882512449324"><td class="cellrowborder" valign="top" width="50%" headers="mcps1.1.3.1.1 "><p id="p88251944153215"><a name="p88251944153215"></a><a name="p88251944153215"></a>pipe</p></td>
<td class="cellrowborder" valign="top" width="50%" headers="mcps1.1.3.1.2 "><p id="p148291124113616"><a name="p148291124113616"></a><a name="p148291124113616"></a>Specifies the pipeline type where the mark is located.</p></td>
</tr>
<tr id="row182624413210"><td class="cellrowborder" valign="top" width="50%" headers="mcps1.1.3.1.1 "><p id="p11826104433211"><a name="p11826104433211"></a><a name="p11826104433211"></a>index</p></td>
<td class="cellrowborder" valign="top" width="50%" headers="mcps1.1.3.1.2 "><p id="p1826184413328"><a name="p1826184413328"></a><a name="p1826184413328"></a>Unique identifier ID of the mark set by the user.</p></td>
</tr>
</tbody>
</table>

## Return Value<a name="section15548145517311"></a>

None

## Constraints<a name="section654818551936"></a>

-   The chip reserves 12 bits for **index**, with a valid value range of \[0,256\). To facilitate locating the corresponding code in the mark graph, avoid reusing the same **index**.
-   If a **MarkStamp** instruction is added inside a loop, a mark is output each time the instruction is executed, and the **index** remains the same.
-   If the developer places marks in two adjacent VFs, the compiler may fuse VF A and VF B, causing **MarkStamp1** and **MarkStamp2** to be optimized out, so no marks are output.

## Example<a name="section354885515310"></a>

```
mte2_opt();
//Mark the position at the start of operator execution.
MarkStamp<PIPE_V, 0>();
//Execute the core computation.
vector_opt();
//Mark the position at the end of operator execution.
MarkStamp<PIPE_V, 1>();
mte3_opt();
```
