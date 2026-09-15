# GmAlloc<a name="ZH-CN_TOPIC_0000002044684566"></a>

<!-- md-trans-meta sourceCommit=ab2c72ba23cbc7229f8b141e516d96812390984a translatedAt=2026-08-27T11:33:02.369Z -->

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


## Description<a name="zh-cn_topic_0000001963639306_zh-cn_topic_0000001541764188_section259105813316"></a>

The header file path is `"tools/cpudebug/include/stub_def.h"`.

When performing CPU-side execution verification of a kernel function, this function is used to create shared memory: it creates a shared file in the /tmp directory and returns the mapped pointer to that file.

## Prototype<a name="zh-cn_topic_0000001963639306_zh-cn_topic_0000001541764188_section2067518173415"></a>

```cpp
void *GmAlloc(size_t size)
```

## Parameters<a name="zh-cn_topic_0000001963639306_zh-cn_topic_0000001541764188_section158061867342"></a>

**Table 1**  Parameters

| Parameter | Input/Output | Description |
| ------ | ------ | ------ |
| size | Input | Size of the shared memory that the user wants to allocate. |

## Return Value<a name="zh-cn_topic_0000001963639306_zh-cn_topic_0000001541764188_section640mcpsimp"></a>

Returns the first address of the shared memory space.

## Constraints<a name="zh-cn_topic_0000001963639306_zh-cn_topic_0000001541764188_section794123819592"></a>

- To preserve API compatibility, it is recommended to compile and use it with [<<<>>>](https://gitcode.com/cann/asc-devkit/blob/9.1.0/docs/en/guide/programming_guide/debug_and_tuning/functional_debug/cpu_domain_twin_debugging.md).
- This API generates a temporary file in the /tmp directory of the system, so sufficient disk space is required to create the shared memory normally.
- It must be released together with [GmFree](GmFree.md).

## Example<a name="zh-cn_topic_0000001963639306_zh-cn_topic_0000001541764188_section82241477610"></a>

```cpp
uint32_t numBlocks = 8;    // Total number of cores.
uint32_t blockLength = 2048;    // Memory size allocated to each core.
size_t len = numBlocks * blockLength * sizeof(uint16_t);    // Shared memory size.
uint8_t* x = (uint8_t*)AscendC::GmAlloc(len);
```
