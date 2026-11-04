# DataCachePreload<a name="ZH-CN_TOPIC_0000001788032974"></a>

<!-- md-trans-meta sourceCommit=754fb95859dead826727a591e674261c6a829a08 translatedAt=2026-09-23T11:14:01.911Z pushedAt=2026-10-26T07:41:44.041Z nl=2 -->

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
- Atlas 200I/500 A2 inference products: supported
<!-- end id4 -->
<!-- npu="310p" id5 -->
- Atlas inference products AI Core: supported
<!-- end id5 -->
<!-- npu="310p" id6 -->
- Atlas inference products Vector Core: not supported
<!-- end id6 -->
<!-- npu="910" id7 -->
- Atlas training products: not supported
<!-- end id7 -->
<!-- @ref: asc-devkit/res/docs/zh/api/SIMD-API/basic_api/cache_control/DataCachePreload_res.md#id1 -->

## Description<a name="section618mcpsimp"></a>

The header file path is `"basic_api/kernel_operator_cache_intf.h"`.

Prefetches data from the specific GM address where the source address is located into the DCache. Each call prefetches only one Cache Line of data.

## Function Prototype<a name="section620mcpsimp"></a>

```cpp
template <typename T>
__aicore__ inline void DataCachePreload(const GlobalTensor<uint64_t>& src, const T cacheOffset)
```

## Parameters<a name="section622mcpsimp"></a>

**Table 1**  Parameters

| Parameter | Input/Output | Description |
|--------|-----------|------|
| src | Input | Source operand, of type [GlobalTensor](../data_structures/GlobalTensor/GlobalTensor_intro.md). Supported data type: uint64_t. |
| cacheOffset | Input | Starts loading data at an offset of cacheOffset from the source operand, in bytes. Supported data types: int16_t, int64_t. |

## Return Value<a name="section640mcpsimp"></a>

None

## Constraints<a name="section633mcpsimp"></a>

Frequently calling this API may cause congestion in the reservation station (a hardware queue used to temporarily store instructions pending execution). In this case, the instruction is treated as a NOP instruction, which in turn blocks the Scalar pipeline. Therefore, it is recommended that this API be called no more than four times consecutively.

## Example<a name="section837496171220"></a>

```cpp
AscendC::GlobalTensor<uint64_t> srcGlobal;
int64_t cacheOffset = 0;
AscendC::DataCachePreload(srcGlobal, cacheOffset);
```
