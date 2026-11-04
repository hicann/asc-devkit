# GmFree<a name="ZH-CN_TOPIC_0000002080803569"></a>

<!-- md-trans-meta sourceCommit=754fb95859dead826727a591e674261c6a829a08 translatedAt=2026-09-23T15:07:53.541Z pushedAt=2026-10-26T07:41:44.371Z nl=2 -->

## Applicable Products

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
- Atlas 200I/500 A2 inference products: supported
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
<!-- @ref: asc-devkit/res/docs/zh/api/SIMD-API/basic_api/debug_interface/cpu_twin_debug/GmFree_res.md#id1 -->

## Description<a name="zh-cn_topic_0000002000199397_zh-cn_topic_0000001592243993_section259105813316"></a>

The header file path is `"tools/cpudebug/include/stub_def.h"`.

Used to release the shared memory allocated through [GmAlloc](GmAlloc.md) when performing CPU-side execution verification of a kernel function (**Kernel**).

## Prototype<a name="zh-cn_topic_0000002000199397_zh-cn_topic_0000001592243993_section2067518173415"></a>

```cpp
void GmFree(void *ptr)
```

## Parameters<a name="zh-cn_topic_0000002000199397_zh-cn_topic_0000001592243993_section158061867342"></a>

**Table 1**  Parameters

| Parameter | Input/Output | Description |
| ------ | ------ | ------ |
| ptr | Input | Pointer to the shared memory to be released. |

## Return Value<a name="zh-cn_topic_0000002000199397_zh-cn_topic_0000001592243993_section640mcpsimp"></a>

None

## Constraints<a name="zh-cn_topic_0000002000199397_zh-cn_topic_0000001592243993_section794123819592"></a>

- To preserve API compatibility, it is recommended to compile with [<<<>>>](../../../../../guide/programming_guide/debug_and_tuning/functional_debug/cpu_twin_debug.md).
- The pointer passed in must be a pointer to shared memory previously allocated through **GmAlloc**.

## Example<a name="zh-cn_topic_0000002000199397_zh-cn_topic_0000001592243993_section82241477610"></a>

```cpp
AscendC::GmFree((void*)x);
```
