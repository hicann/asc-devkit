# asc\_mark\_stamp

<!-- md-trans-meta sourceCommit=482b56785b936fd20f7354a81c8239038750acee translatedAt=2026-09-06T09:01:04.159Z pushedAt=2026-09-11T01:46:03.351Z -->

## Applicable Products

- Ascend 950PR/Ascend 950DT: supported
- Atlas A3 training products/Atlas A3 inference products: not supported
- Atlas A2 training products/Atlas A2 inference products: not supported
- Atlas 200I/500 A2 inference products: not supported
- Atlas inference products AI Core: not supported
- Atlas inference products Vector Core: not supported
- Atlas training products: not supported

## Description

In SIMD scenarios, users call the API to mark specific positions during operator execution, facilitating later analysis of code execution paths and performance hotspots through the pipeline graph.

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
| pipe | Pipeline type where the logging point is located. |
| idx | Unique identifier of the logging point set by the user. |

## Return Value

None

## Constraints

-   The value range of **idx** is \[0,4095\]. To make it easier to locate the corresponding code in the logging graph, avoid reusing the same **idx**.
-   If an **asc_mark_stamp** instruction is added inside a loop, a logging point is output each time the instruction is executed, and the **idx** is the same.
-   If the developer places logging points in two adjacent VFs, the compiler may fuse **VF A** and **VF B**, causing **asc_mark_stamp1** and **asc_mark_stamp2** to be optimized away, so no logging point is output.

## Example

```
mte2_opt();
// Log at the start of operator execution.
asc_mark_stamp<PIPE_V, 0>();
// Execute the core computation.
vector_opt();
// Log at the end of operator execution.
asc_mark_stamp<PIPE_V, 1>();
mte3_opt();
```
