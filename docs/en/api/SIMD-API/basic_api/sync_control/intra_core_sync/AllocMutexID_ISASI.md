# AllocMutexID \(ISASI\)<a name="ZH-CN_TOPIC_0000002352639064"></a>

<!-- md-trans-meta sourceCommit=754fb95859dead826727a591e674261c6a829a08 translatedAt=2026-09-23T18:55:55.545Z pushedAt=2026-10-26T07:41:44.817Z nl=2 -->

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
<!-- @ref: asc-devkit/res/docs/zh/api/SIMD-API/basic_api/sync_control/intra_core_sync/AllocMutexID_ISASI_res.md#id1 -->

## Description<a name="section618mcpsimp"></a>

The header file path is `"basic_api/kernel_common.h"`.

Obtains and occupies a MutexID from the framework. Use it together with [ReleaseMutexID](ReleaseMutexID_ISASI.md) to manage the acquisition and release of MutexIDs. The obtained MutexID can be passed to the [Mutex::Lock](Lock.md)/[Mutex::Unlock](Unlock.md) APIs for intra-core asynchronous pipeline synchronization. A MutexID obtained through this API is recorded in the framework's MutexID allocation records, and is suitable for scenarios where `Mutex::Lock`/`Mutex::Unlock` are used together with resource management APIs such as TQue. For the constraints on using a self-managed MutexID with resource management APIs, see [Lock Constraints](Lock.md#mutexid_resource_management_constraint).

## Function Prototype<a name="section620mcpsimp"></a>

```cpp
__aicore__ inline MutexID AllocMutexID()
```

## Parameters<a name="section622mcpsimp"></a>

None

## Return Value<a name="section640mcpsimp"></a>

Returns a **MutexID**, whose type is defined as follows. Each ID represents a Mutex lock.

```cpp
using MutexID = uint8_t;
```

## Constraints<a name="section633mcpsimp"></a>

The number of **MutexID** is limited. After use, call [ReleaseMutexID](ReleaseMutexID_ISASI.md) immediately to release it to prevent **MutexID** exhaustion.

## Example<a name="section837496171220"></a>

For an example, see [Lock example](Lock.md#section123275308128).
