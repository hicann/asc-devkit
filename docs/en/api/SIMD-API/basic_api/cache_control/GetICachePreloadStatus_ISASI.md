# GetICachePreloadStatus\(ISASI\)<a name="ZH-CN_TOPIC_0000001788032978"></a>

<!-- md-trans-meta sourceCommit=754fb95859dead826727a591e674261c6a829a08 translatedAt=2026-09-23T11:14:06.700Z pushedAt=2026-10-26T07:41:44.042Z nl=2 -->

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
<!-- @ref: asc-devkit/res/docs/zh/api/SIMD-API/basic_api/cache_control/GetICachePreloadStatus_ISASI_res.md#id1 -->

## Description<a name="section618mcpsimp"></a>

The header file path is `"basic_api/kernel_operator_cache_intf.h"`.

**GetICachePreloadStatus** is a debugging API. It is called after **ICachePreLoad** to obtain the **PreLoad** status of the ICache: when the return value is 0, the **PreLoad** of the ICache is complete; when the return value is 1, the **PreLoad** of the ICache is not complete.

## Prototype<a name="section620mcpsimp"></a>

```cpp
__aicore__ inline int64_t GetICachePreloadStatus()
```

## Parameters<a name="section622mcpsimp"></a>

None

## Return Value<a name="section640mcpsimp"></a>

int64\_t Type. **0** indicates that the ICache PreLoad is complete, and **1** indicates that the ICache PreLoad is not complete.

## Constraints<a name="section633mcpsimp"></a>

None

## Example<a name="section837496171220"></a>

```cpp
// Obtain the ICache preload status. 0 indicates completed, and 1 indicates not completed.
int64_t cachePreloadStatus = AscendC::GetICachePreloadStatus();
```
