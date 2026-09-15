# ReleaseMutexID \(ISASI\)<a name="ZH-CN_TOPIC_0000002406521145"></a>

<!-- md-trans-meta sourceCommit=f6490d1e030d257ee55f3f1600bffece5a8bd3f7 translatedAt=2026-08-27T13:55:30.068Z -->

## Applicable Products<a name="section1550532418810"></a>

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
| id | Input | **MutexID** type. Pass the **MutexID** obtained by calling **AllocMutexID**. |

## Return Value<a name="section640mcpsimp"></a>

None

## Constraints<a name="section633mcpsimp"></a>

The number of **MutexID**s is limited. Call [ReleaseMutexID](ReleaseMutexID_ISASI.md) immediately after use to release the **MutexID** and prevent it from being exhausted.

## Example<a name="section837496171220"></a>

For an example, see [Lock example](Lock.md#section123275308128).
