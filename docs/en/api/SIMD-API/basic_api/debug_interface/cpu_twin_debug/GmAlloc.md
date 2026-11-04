# GmAlloc<a name="ZH-CN_TOPIC_0000002044684566"></a>

<!-- md-trans-meta sourceCommit=754fb95859dead826727a591e674261c6a829a08 translatedAt=2026-09-23T15:07:35.741Z pushedAt=2026-10-26T07:41:44.369Z nl=2 -->

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
- Atlas 200I/500 A2 inference product: supported
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
<!-- @ref: asc-devkit/res/docs/zh/api/SIMD-API/basic_api/debug_interface/cpu_twin_debug/GmAlloc_res.md#id1 -->

## Description<a name="zh-cn_topic_0000001963639306_zh-cn_topic_0000001541764188_section259105813316"></a>

The header file path is `"tools/cpudebug/include/stub_def.h"`.

When performing CPU-side execution verification of a kernel function, this API is used to create shared memory: it creates a shared file in the /tmp directory and returns the mapped pointer to the file.

## Prototype<a name="zh-cn_topic_0000001963639306_zh-cn_topic_0000001541764188_section2067518173415"></a>

```cpp
void *GmAlloc(size_t size)
```

## Parameters<a name="zh-cn_topic_0000001963639306_zh-cn_topic_0000001541764188_section158061867342"></a>

**Table 1**  Parameter Description

| Parameter | Input/Output | Description |
| ------ | ------ | ------ |
| size | Input | Size of the shared memory that the user wants to apply for. |

## Return Value<a name="zh-cn_topic_0000001963639306_zh-cn_topic_0000001541764188_section640mcpsimp"></a>

Returns the base address of the shared memory space.

## Constraints<a name="zh-cn_topic_0000001963639306_zh-cn_topic_0000001541764188_section794123819592"></a>

- To preserve API compatibility, it is recommended to compile with [<<<>>>](../../../../../guide/programming_guide/debug_and_tuning/functional_debug/cpu_twin_debug.md).
- This API generates temporary files in the /tmp directory of the system, so sufficient disk space is required to generate shared memory properly.
- It must be used together with [GmFree](GmFree.md) for release.

## Example<a name="zh-cn_topic_0000001963639306_zh-cn_topic_0000001541764188_section82241477610"></a>

```cpp
uint32_t numBlocks = 8;    // Total number of cores.
uint32_t blockLength = 2048;    // Memory size allocated for each core.
size_t len = numBlocks * blockLength * sizeof(uint16_t);    // Shared memory size.
uint8_t* x = (uint8_t*)AscendC::GmAlloc(len);
```
