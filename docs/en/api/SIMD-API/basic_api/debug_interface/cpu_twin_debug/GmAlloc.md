# GmAlloc<a name="ZH-CN_TOPIC_0000002044684566"></a>

<!-- md-trans-meta sourceCommit=8fa2928a9ba79777fe5da4f7c9987e59a88f273e translatedAt=2026-11-02T05:44:32.586Z pushedAt=2026-11-18T01:25:06.943Z nl=2 -->

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
<!-- npu="x90" id8 -->
- Kirin X90 processor products: Supported
<!-- end id8 -->
<!-- npu="9030" id9 -->
- Kirin 9030 processor products: Supported
<!-- end id9 -->
<!-- @ref: asc-devkit/res/docs/zh/api/SIMD-API/basic_api/debug_interface/cpu_twin_debug/GmAlloc_res.md#id1 -->

## Description<a name="zh-cn_topic_0000001963639306_zh-cn_topic_0000001541764188_section259105813316"></a>

The header file path is `"tools/cpudebug/include/stub_def.h"`.

Used to create shared memory when performing CPU-side execution verification of the kernel function (Kernel): creates a shared file in the /tmp directory and returns the mapping pointer of the file.

## Prototype<a name="zh-cn_topic_0000001963639306_zh-cn_topic_0000001541764188_section2067518173415"></a>

```cpp
void *GmAlloc(size_t size)
```

## Parameters<a name="zh-cn_topic_0000001963639306_zh-cn_topic_0000001541764188_section158061867342"></a>

**Table 1**  Parameters

| Parameter | Input/Output | Description |
| ------ | ------ | ------ |
| size | Input | Size of the shared memory that the user wants to apply for.|

## Return Value<a name="zh-cn_topic_0000001963639306_zh-cn_topic_0000001541764188_section640mcpsimp"></a>

Returns the base address of the shared memory space.

## Constraints<a name="zh-cn_topic_0000001963639306_zh-cn_topic_0000001541764188_section794123819592"></a>

- To preserve API compatibility, it is recommended to compile with [<<<>>>](../../../../../guide/programming_guide/debug_and_tuning/functional_debug/cpu_twin_debug.md).
- This API generates temporary files in the /tmp directory of the system, so sufficient disk space is required to generate shared memory properly.
- It must be used together with [GmFree](GmFree.md) for release.

## Example<a name="zh-cn_topic_0000001963639306_zh-cn_topic_0000001541764188_section82241477610"></a>

```cpp
uint32_t numBlocks = 8;    // Total number of cores.
uint32_t blockLength = 2048;    // Memory size allocated per core.
size_t len = numBlocks * blockLength * sizeof(uint16_t);    // Shared memory size.
uint8_t* x = (uint8_t*)AscendC::GmAlloc(len);
```
