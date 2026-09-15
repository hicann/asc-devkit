# GmFree<a name="ZH-CN_TOPIC_0000002080803569"></a>

<!-- md-trans-meta sourceCommit=ab2c72ba23cbc7229f8b141e516d96812390984a translatedAt=2026-08-27T11:34:02.699Z -->

## Applicable Products<a name="section1550532418810"></a>

<!-- npu="950" id1 -->
- Ascend 950PR/Ascend 950DT: Supported
<!-- end id1 -->
<!-- npu="A3" id2 -->
- Atlas A3 training products/Atlas A3 inference products: Supported
<!-- end id2 -->
<!-- npu="910b" id3 -->
- Atlas A2 training products/Atlas A2 inference products: Supported
<!-- end id3 -->
<!-- npu="310b" id4 -->
- Atlas 200I/500 A2 inference products: Supported
<!-- end id4 -->
<!-- npu="310p" id5 -->
- Atlas inference products AI Core: Supported
<!-- end id5 -->
<!-- npu="310p" id6 -->
- Atlas inference products Vector Core: Not supported
<!-- end id6 -->
<!-- npu="910" id7 -->
- Atlas training products: Supported
<!-- end id7 -->


## Description<a name="zh-cn_topic_0000002000199397_zh-cn_topic_0000001592243993_section259105813316"></a>

The header file path is `"tools/cpudebug/include/stub_def.h"`.

During CPU-side runtime verification of a kernel function, this function is used to release the shared memory allocated by [GmAlloc](GmAlloc.md).

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

- To preserve API compatibility, it is recommended to compile and use it with [CPU Domain Twin Debugging](https://gitcode.com/cann/asc-devkit/blob/9.1.0/docs/en/guide/programming_guide/debug_and_tuning/functional_debug/cpu_domain_twin_debugging.md).
- The pointer passed in must be a pointer to shared memory previously allocated through **GmAlloc**.

## Example<a name="zh-cn_topic_0000002000199397_zh-cn_topic_0000001592243993_section82241477610"></a>

```cpp
AscendC::GmFree((void*)x);
```
