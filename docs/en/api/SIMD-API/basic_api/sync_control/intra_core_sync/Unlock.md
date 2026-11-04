# Unlock<a name="ZH-CN_TOPIC_0000002406681009"></a>

<!-- md-trans-meta sourceCommit=754fb95859dead826727a591e674261c6a829a08 translatedAt=2026-09-23T19:09:27.512Z pushedAt=2026-10-26T07:41:44.836Z nl=2 -->

## Applicable Products<a name="section73648168211"></a>

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
<!-- @ref: asc-devkit/res/docs/zh/api/SIMD-API/basic_api/sync_control/intra_core_sync/Unlock_res.md#id1 -->

## Description<a name="section54681522111017"></a>

The header file path is `"basic_api/kernel_common.h"`.

After the preceding instructions of the specified pipeline are executed, the corresponding Mutex is released based on the MutexID.

> [!NOTE]
> The Unlock API must be used together with the [Lock](Lock.md) API. For the same MutexID, Lock and Unlock must appear strictly in pairs. For detailed constraints, see [Lock Constraints](Lock.md#section184751024101111).

## Prototype<a name="section1568410468104"></a>

```cpp
template <pipe_t pipe>
static __aicore__ inline void Unlock(MutexID id)
```

## Parameters<a name="section74061251191017"></a>

**Table 1** Template parameter description

| Parameter | Description |
| --- | --- |
| pipe | Template parameter, indicating the pipeline type. Supports **PIPE_S**, **PIPE_M**, **PIPE_V**, **PIPE_MTE1**, **PIPE_MTE2**, **PIPE_MTE3**, and **PIPE_FIX**. For the meaning of each pipeline type, see [Hardware Pipeline Type](intra_core_sync_overview.md#hardware-pipeline-type). |

**Table 2** Parameter description

| Parameter | Input/Output | Description |
| --- | --- | --- |
| id | Input | **MutexID** used for pipeline synchronization management. In different programming paradigms, the acquisition and release of this ID differ. For details, see [Lock Constraints](Lock.md#section184751024101111). |

## Return Value<a name="section640mcpsimp"></a>

None

## Constraints<a name="section184751024101111"></a>

See [Lock Constraints](Lock.md#section184751024101111).

## Example<a name="section123275308128"></a>

**Lock** and **Unlock** must be used strictly in pairs. For a detailed example, see [Lock Example](Lock.md#section123275308128).
