# Reset<a name="ZH-CN_TOPIC_0000001912722605"></a>

<!-- md-trans-meta sourceCommit=754fb95859dead826727a591e674261c6a829a08 translatedAt=2026-09-23T17:36:45.888Z pushedAt=2026-10-26T07:41:44.664Z nl=2 -->

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
- Atlas inference products AI Core: supported
<!-- end id5 -->
<!-- npu="310p" id6 -->
- Atlas inference products Vector Core: not supported
<!-- end id6 -->
<!-- npu="910" id7 -->
- Atlas training products: supported
<!-- end id7 -->
<!-- @ref: asc-devkit/res/docs/zh/api/SIMD-API/basic_api/resource_management/TBufPool/Reset_res.md#id1 -->

## Description<a name="section618mcpsimp"></a>

The header file path is `"basic_api/kernel_tpipe.h"`.

Used when switching the **TBufPool** resource pool to end the related events currently being processed by the **TBufPool** resource pool. After the call, the current resource pool and the **Buffer** allocated by the resource pool still exist, but the **Buffer** content may be overwritten. You can switch back to this resource pool and resume using the **Buffer** without allocating it again.

## Prototype<a name="section620mcpsimp"></a>

```cpp
__aicore__ inline void Reset()
```

## Constraints<a name="section633mcpsimp"></a>

None

## Return Value<a name="section640mcpsimp"></a>

None

## Example<a name="section642mcpsimp"></a>

See [InitBufPool](InitBufPool.md).
