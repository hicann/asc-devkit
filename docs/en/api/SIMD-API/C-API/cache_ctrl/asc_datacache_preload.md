# asc_datacache_preload

<!-- md-trans-meta sourceCommit=0ece502ce5e30f12c91b14fc87b28b1b051cb796 translatedAt=2026-08-26T10:26:39.777Z -->

## Applicable Products

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
- Atlas 200I/500 A2 inference products: Not supported
<!-- end id4 -->
<!-- npu="310p" id5 -->
- Atlas inference products AI Core: Not supported
<!-- end id5 -->
<!-- npu="310p" id6 -->
- Atlas inference products Vector Core: Not supported
<!-- end id6 -->
<!-- npu="910" id7 -->
- Atlas training products: Not supported
<!-- end id7 -->

## Description

Preloads data from the specific GM address where the source address is located into the Data Cache.

## Prototype

```cpp
__aicore__ inline void asc_datacache_preload(__gm__ uint64_t* address, int64_t offset)
```

## Parameters

|Parameter|Input/Output|Description|
|------------|------------|-----------|
| **address**     | Input     | Start address of the source operand.   |
| **offset**     | Input     | Offset from the source operand at which data loading starts, in bytes.|

## Return Value

None

## Pipeline Type

PIPE_S

## Constraints

Frequent calls to this API may cause reservation stack congestion. In this case, the API is treated as a NOP instruction and blocks the Scalar pipeline. Therefore, frequent calls to this API are not recommended.

## Example

```cpp
// Assume that x is an externally passed address, each core computes 16 data elements, and block_idx is a built-in variable. The offset is calculated through block_idx to isolate data across multiple cores.
__gm__ float* x_gm = x + block_idx * 16;
int64_t cacheOffset = 0;
asc_datacache_preload(reinterpret_cast<__gm__ uint64_t*>(x_gm), cacheOffset);
```
