# MarkStamp<a name="ZH-CN_TOPIC_0000002491669476"></a>

<!-- md-trans-meta sourceCommit=754fb95859dead826727a591e674261c6a829a08 translatedAt=2026-09-23T15:16:47.733Z pushedAt=2026-10-26T07:41:44.390Z nl=2 -->

## Applicable Products

<!-- npu="950" id1 -->
- Ascend 950PR&950DT products: supported
<!-- end id1 -->
<!-- npu="A3" id2 -->
- Atlas A3 products: not supported
<!-- end id2 -->
<!-- npu="910b" id3 -->
- Atlas A2 products: not supported
<!-- end id3 -->
<!-- npu="310b" id4 -->
- Atlas 200I/500 A2 inference product: not supported
<!-- end id4 -->
<!-- npu="310p" id5 -->
- Atlas inference products AI Core: not supported
<!-- end id5 -->
<!-- npu="310p" id6 -->
- Atlas inference products Vector Core: not supported
<!-- end id6 -->
<!-- npu="910" id7 -->
- Atlas training products: not supported
<!-- end id7 -->
<!-- @ref: asc-devkit/res/docs/zh/api/SIMD-API/basic_api/debug_interface/performance_stats/MarkStamp_res.md#id1 -->

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
<tbody><tr id="row1882512449324"><td class="cellrowborder" valign="top" width="50%" headers="mcps1.1.3.1.1 "><p id="p88251944153215"><a name="p88251944153215"></a><a name="p88251944153215"></a>**pipe**</p></td>
<td class="cellrowborder" valign="top" width="50%" headers="mcps1.1.3.1.2 "><p id="p148291124113616"><a name="p148291124113616"></a><a name="p148291124113616"></a>Specifies the pipeline type where the marking is located.</p></td>
</tr>
<tr id="row182624413210"><td class="cellrowborder" valign="top" width="50%" headers="mcps1.1.3.1.1 "><p id="p11826104433211"><a name="p11826104433211"></a><a name="p11826104433211"></a>**index**</p></td>
<td class="cellrowborder" valign="top" width="50%" headers="mcps1.1.3.1.2 "><p id="p1826184413328"><a name="p1826184413328"></a><a name="p1826184413328"></a>Unique identifier of the marking set by the user.</p></td>
</tr>
</tbody>
</table>

## Return Value<a name="section15548145517311"></a>

None

## Constraints<a name="section654818551936"></a>

-   The chip reserves 12 bits for the index, and the available value range is \[0,256\). To easily locate the corresponding code in the marking graph, avoid reusing the same index.
-   If a MarkStamp instruction is added in a loop, a marking is output each time the instruction is executed, and the index is the same.
-   If the developer places marks in two adjacent VFs, the compiler may fuse VF A and VF B, causing MarkStamp1 and MarkStamp2 to be optimized out, and no marks will be output.

## Example<a name="section354885515310"></a>

```
mte2_opt();
// Mark at the start of operator execution.
MarkStamp<PIPE_V, 0>();
// Execute the core computation.
vector_opt();
// Mark at the end of operator execution.
MarkStamp<PIPE_V, 1>();
mte3_opt();
```
