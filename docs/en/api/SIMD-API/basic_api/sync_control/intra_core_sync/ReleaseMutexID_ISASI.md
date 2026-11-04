# ReleaseMutexID \(ISASI\)<a name="ZH-CN_TOPIC_0000002406521145"></a>

<!-- md-trans-meta sourceCommit=754fb95859dead826727a591e674261c6a829a08 translatedAt=2026-09-23T19:07:25.790Z pushedAt=2026-10-26T07:41:44.829Z nl=2 -->

## Applicable Products<a name="section1550532418810"></a>

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
- Atlas 200I/500 A2 inference products: not supported
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
<!-- @ref: asc-devkit/res/docs/zh/api/SIMD-API/basic_api/sync_control/intra_core_sync/ReleaseMutexID_ISASI_res.md#id1 -->

## Description<a name="section1386110162210"></a>

The header file path is `"basic_api/kernel_common.h"`.

Releases a MutexID from the framework, used together with [AllocMutexID](AllocMutexID_ISASI.md).

## Prototype<a name="section620mcpsimp"></a>

```cpp
__aicore__ inline void ReleaseMutexID(MutexID id)
```

## Parameters<a name="section622mcpsimp"></a>

**Table 1**  Parameters

| Parameter | Input/Output | Description |
| --- | --- | --- |
| id | Input | MutexID type. Pass the MutexID obtained by calling AllocMutexID. |

## Return Value<a name="section640mcpsimp"></a>

None

## Constraints<a name="section633mcpsimp"></a>

The number of MutexIDs is limited. After use, call [ReleaseMutexID](ReleaseMutexID_ISASI.md) immediately to release the MutexID and prevent MutexID exhaustion.

## Example<a name="section837496171220"></a>

For an example, see [Lock Example](Lock.md#section123275308128).
