# GetCurBufSize<a name="ZH-CN_TOPIC_0000002051883242"></a>

<!-- md-trans-meta sourceCommit=8fa2928a9ba79777fe5da4f7c9987e59a88f273e translatedAt=2026-11-05T03:57:18.459Z pushedAt=2026-11-18T01:25:30.981Z nl=2 -->

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
<!-- npu="x90" id8 -->
- Kirin X90 processor products: Supported
<!-- end id8 -->
<!-- npu="9030" id9 -->
- Kirin 9030 processor products: Supported
<!-- end id9 -->
<!-- @ref: asc-devkit/res/docs/zh/api/SIMD-API/basic_api/resource_management/custom_TBufPool/GetCurBufSize_res.md#id1 -->

## Description<a name="section618mcpsimp"></a>

The header file path is: `"basic_api/kernel_tpipe.h"`.

Obtains the number of memory blocks that have been allocated by the custom TBufPool.

## Prototype<a name="section620mcpsimp"></a>

```cpp
__aicore__ inline uint8_t GetCurBufSize()
```

## Constraints<a name="section633mcpsimp"></a>

None

## Return Value<a name="section640mcpsimp"></a>

Returns the number of allocated memory blocks.

## Examples<a name="section1234017553610"></a>

See [Examples](EXTERN_IMPL_BUFPOOL.md#section1234017553610).
