# asc\_mark\_stamp

<!-- md-trans-meta sourceCommit=754fb95859dead826727a591e674261c6a829a08 translatedAt=2026-10-23T07:45:56.498Z pushedAt=2026-10-26T11:12:09.543Z nl=2 -->

## Applicable Products

<!-- npu="950" id1 -->
- Ascend 950PR&950DT products: Supported
<!-- end id1 -->
<!-- npu="A3" id2 -->
- Atlas A3 products: Not supported
<!-- end id2 -->
<!-- npu="910b" id3 -->
- Atlas A2 products: Not supported
<!-- end id3 -->
<!-- npu="310b" id4 -->
- Atlas 200I/500 A2 inference products: Not supported
<!-- end id4 -->
<!-- npu="310p" id5 -->
- Atlas inference products AI Core: Not supported
- Atlas inference products Vector Core: Not supported
<!-- end id5 -->
<!-- npu="910" id6 -->
- Atlas training products: Not supported
<!-- end id6 -->
<!-- @ref: asc-devkit/res/docs/zh/api/Utils-API/tuning_interface/asc_mark_stamp_res.md#id1 -->

## Description

In SIMD scenarios, you can call the API to mark specific positions during operator execution, facilitating later analysis of code execution paths and performance hotspots through the pipeline graph.

## Prototype

```
template<pipe_t pipe = PIPE_S>
__aicore__ inline void asc_mark_stamp(uint16_t idx)

template<pipe_t pipe = PIPE_S, uint16_t idx>
__aicore__ inline void asc_mark_stamp()
```

## Parameters

| Parameter | Description |
| --- | --- |
| pipe | Specifies the pipeline type where the mark is placed. |
| idx | Unique ID of the mark set by the user. |

## Return Value

None

## Constraints

-   The value range of idx is \[0,4095\]. To make it easier to locate the corresponding code in the mark graph, avoid reusing the same idx.
-   On the AIV side, only the PIPE_S, PIPE_V, PIPE_MTE2, and PIPE_MTE3 pipeline types are supported. On the AIC side, only the PIPE_S, PIPE_MTE1, PIPE_MTE2, PIPE_FIX, and PIPE_M pipeline types are supported.
-   If an asc_mark_stamp instruction is added inside a loop, a mark is output each time the instruction is executed, and the idx remains the same.
-   If you place marks in two adjacent VFs, the compiler may fuse VF A and VF B, causing asc_mark_stamp1 and asc_mark_stamp2 to be optimized out, and no marks will be output.

## Examples

```
mte2_opt();
// Place a mark at the start of operator execution.
asc_mark_stamp<PIPE_V, 0>();
// Perform the core computation.
vector_opt();
// Place a mark at the end of operator execution.
asc_mark_stamp<PIPE_V, 1>();
mte3_opt();
```
