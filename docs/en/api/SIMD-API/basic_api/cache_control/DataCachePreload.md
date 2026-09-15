# DataCachePreload<a name="ZH-CN_TOPIC_0000001788032974"></a>

<!-- md-trans-meta sourceCommit=1de5b49f75a2c8f83a238b6ccbb54bf6cf8fb65c translatedAt=2026-08-27T12:24:27.346Z -->

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
- Atlas training products: Not supported
<!-- end id7 -->

## Description<a name="section618mcpsimp"></a>

The header file path is `"basic_api/kernel_operator_cache_intf.h"`.

Preloads data from the specific GM address where the source address resides into the DCache. Each call preloads only one Cache Line of data.

## Function Prototype<a name="section620mcpsimp"></a>

```cpp
template <typename T>
__aicore__ inline void DataCachePreload(const GlobalTensor<uint64_t>& src, const T cacheOffset)
```

## Parameters<a name="section622mcpsimp"></a>

**Table 1**  Parameters

| Parameter | Input/Output | Description |
|--------|-----------|------|
| src | Input | Source operand, of type **GlobalTensor**. Supported data type: **uint64_t**. |
| cacheOffset | Input | Loads data starting from the source operand offset by **cacheOffset** bytes. Unit: byte. Supported data types: **int16_t**/**int64_t**. |

## Return Value<a name="section640mcpsimp"></a>

None

## Constraints<a name="section633mcpsimp"></a>

Frequently calling this API may cause congestion in the reservation station (a hardware queue used to temporarily store instructions pending execution). In this case, the instruction is treated as a **NOP** instruction, which in turn blocks the **Scalar** pipeline. Therefore, it is recommended that this API be called no more than four times consecutively.

## Example<a name="section837496171220"></a>

```cpp
AscendC::GlobalTensor<uint64_t> srcGlobal;
int64_t cacheOffset = 0;
AscendC::DataCachePreload(srcGlobal, cacheOffset);
