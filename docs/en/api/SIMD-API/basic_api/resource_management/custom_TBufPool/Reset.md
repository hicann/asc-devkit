# Reset<a name="ZH-CN_TOPIC_0000002087883721"></a>

<!-- md-trans-meta sourceCommit=754fb95859dead826727a591e674261c6a829a08 translatedAt=2026-09-23T17:25:05.603Z pushedAt=2026-10-26T07:41:44.635Z nl=2 -->

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
<!-- @ref: asc-devkit/res/docs/zh/api/SIMD-API/basic_api/resource_management/custom_TBufPool/Reset_res.md#id1 -->

## Description<a name="section618mcpsimp"></a>

The header file path is `"basic_api/kernel_tpipe.h"`.

Releases the custom **TbufPool** resources and resets variables such as **eventId** for cancellation.

## Prototype<a name="section620mcpsimp"></a>

```cpp
__aicore__ inline void Reset()
```

## Constraints<a name="section633mcpsimp"></a>

Call this API when switching a custom **TBufPool** resource pool. After the call, the corresponding resource pool and the **Buffer** allocated from it can no longer be used.

## Return Value<a name="section640mcpsimp"></a>

None

## Example<a name="section1234017553610"></a>

See [Example](EXTERN_IMPL_BUFPOOL.md#section1234017553610).
