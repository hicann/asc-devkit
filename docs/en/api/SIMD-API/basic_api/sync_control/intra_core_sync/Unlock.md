# Unlock<a name="ZH-CN_TOPIC_0000002406681009"></a>

<!-- md-trans-meta sourceCommit=f6490d1e030d257ee55f3f1600bffece5a8bd3f7 translatedAt=2026-08-27T13:58:15.669Z -->

## Applicable Products<a name="section73648168211"></a>

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

## Description<a name="section54681522111017"></a>

The header file path is `"basic_api/kernel_common.h"`.

After the preceding instructions of the current pipeline are executed, the corresponding Mutex is released based on **MutexID**.

> [!NOTE]Note
> The Unlock API must be used together with the [Lock](Lock.md) API. For the same **MutexID**, Lock and Unlock must strictly appear in pairs. For detailed constraints, see [Lock Constraint Description](Lock.md#section184751024101111).

## Prototype<a name="section1568410468104"></a>

```cpp
template <pipe_t pipe>
static __aicore__ inline void Unlock(MutexID id)
```

## Parameters<a name="section74061251191017"></a>

**Table 1** Template parameter description

| Parameter | Description |
| --- | --- |
| pipe | Template parameter, indicating the pipeline type. The supported pipeline types are **PIPE_S**/**PIPE_M**/**PIPE_V**/**PIPE_MTE1**/**PIPE_MTE2**/**PIPE_MTE3**/**PIPE_FIX**. |

**Table 2** Parameter description

| Parameter | Input/Output | Description |
| --- | --- | --- |
| id | Input | **MutexID** used for pipeline synchronization management. In different programming paradigms, the acquisition and release of this ID differ. For details, see [Lock Constraint Description](Lock.md#section184751024101111). |

## Return Value<a name="section640mcpsimp"></a>

None

## Constraints<a name="section184751024101111"></a>

See [Lock Constraints](Lock.md#section184751024101111).

## Example<a name="section123275308128"></a>

**Lock** and **Unlock** must be used strictly in pairs. For a detailed example, see [Lock Example](Lock.md#section123275308128).
