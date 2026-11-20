---
okftype: reference.api
description: "Reads integer scalar data directly from global memory without going through DCache. It is used in scenarios where multiple cores access GM addresses that cannot be aligned to cache lines, avoiding cache-line-granularity overwrites."
tag:
  - roles: [data_movement]
  - operations: [load]
  - traits: [scalar_operand, unaligned_transfer]
dependencies:
  - path: docs/zh/api/SIMD-API/c_api/cache_ctrl/asc_dcci.md
    kind: related
    condition: "Clean/invalidate cache lines when sharing GM addresses with other paths"
version: ">=9.2.0"
version_changes: []
---
# asc_load_dev

<!-- md-trans-meta sourceCommit=54f1d966cfcc22290ddc2c6312bbe4c6ac2a9232 translatedAt=2026-11-16T01:40:03.457778+00:00 pushedAt=2026-11-16T02:10:57.520Z nl=2 -->

## Applicable Products

<!-- npu="950" id1 -->
- Ascend 950PR&950DT products: Supported
<!-- end id1 -->
<!-- npu="A3" id2 -->
- Atlas A3 products: Not supported
<!-- end id2 -->
<!-- npu="910b" id3 -->
- Atlas A2 products: Not supported
<!-- end id3 -->
<!-- npu="910" id4 -->
- Atlas training products: Not supported
<!-- end id4 -->
<!-- npu="310p" id5 -->
- Atlas inference products AI Core: Not supported
<!-- end id5 -->
<!-- npu="310p" id6 -->
- Atlas inference products Vector Core: Not supported
<!-- end id6 -->
<!-- npu="310b" id7 -->
- Atlas 200I/500 A2 inference products: Not supported
<!-- end id7 -->

## Description

The header file path is `"c_api/scalar_compute/scalar_load.h"`.

Reads integer data directly from a GM address without going through the DCache.
When multiple cores operate on GM addresses and the data cannot be aligned to a Cache Line, DCache reads and writes may introduce data overwrites at Cache Line granularity. In this case, this API can be used to bypass DCache and read GM data.

## Prototype

```cpp
__aicore__ inline int8_t asc_load_dev(__gm__ int8_t* addr)

__aicore__ inline uint8_t asc_load_dev(__gm__ uint8_t* addr)

__aicore__ inline int16_t asc_load_dev(__gm__ int16_t* addr)

__aicore__ inline uint16_t asc_load_dev(__gm__ uint16_t* addr)

__aicore__ inline int32_t asc_load_dev(__gm__ int32_t* addr)

__aicore__ inline uint32_t asc_load_dev(__gm__ uint32_t* addr)

__aicore__ inline int64_t asc_load_dev(__gm__ int64_t* addr)

__aicore__ inline uint64_t asc_load_dev(__gm__ uint64_t* addr)
```

## Parameters

**Table 1** Parameters

| Parameter | Input/Output | Description |
| --- | --- | --- |
| addr | Input | Source GM address. Supported data types are `int8_t`, `uint8_t`, `int16_t`, `uint16_t`, `int32_t`, `uint32_t`, `int64_t`, `uint64_t`. |

## Return Value

Data read from GM. The data type of the return value is consistent with the data type pointed to by `addr`.

## Pipeline Type

PIPE_S

## Constraints

Only integer data is supported; floating-point types are not supported.

## Examples

```cpp
// addr is an externally input GM address of type __gm__ int32_t*.
int32_t value = asc_load_dev(addr);
```
