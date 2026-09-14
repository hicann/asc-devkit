# asc_store_dev

<!-- md-trans-meta sourceCommit=0ece502ce5e30f12c91b14fc87b28b1b051cb796 translatedAt=2026-09-05T12:35:53.881Z pushedAt=2026-09-08T03:45:05.358Z -->

## Applicable Products

<!-- npu="950" id1 -->
- Ascend 950PR/Ascend 950DT: Supported
<!-- end id1 -->
<!-- npu="A3" id2 -->
- Atlas A3 training products/Atlas A3 inference products: Not supported
<!-- end id2 -->
<!-- npu="910b" id3 -->
- Atlas A2 training products/Atlas A2 inference products: Not supported
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

Writes data to a GM address without going through the DCache.
When multiple cores operate on a GM address, if the data cannot be aligned to a Cache Line, the DCache-based approach reads and writes by Cache Line size, which causes random data overwriting among multiple cores. In this case, you can read from and write to the GM address directly without going through the DCache, thereby avoiding the random overwriting problem described above.

## Prototype

```cpp
__aicore__ inline void asc_store_dev(__gm__ int8_t* addr, int8_t value)

__aicore__ inline void asc_store_dev(__gm__ uint8_t* addr, uint8_t value)

__aicore__ inline void asc_store_dev(__gm__ int16_t* addr, int16_t value)

__aicore__ inline void asc_store_dev(__gm__ uint16_t* addr, uint16_t value)

__aicore__ inline void asc_store_dev(__gm__ int32_t* addr, int32_t value)

__aicore__ inline void asc_store_dev(__gm__ uint32_t* addr, uint32_t value)

__aicore__ inline void asc_store_dev(__gm__ int64_t* addr, int64_t value)

__aicore__ inline void asc_store_dev(__gm__ uint64_t* addr, uint64_t value)
```

## Parameters

|Parameter|Input/Output|Description|
|------------|------------|-----------|
| addr     | Output     | Target GM address.|
| value     | Input     | Data to be written to the target.|

## Return Value

None

## Pipeline Type

PIPE_S

## Constraints

None

## Example

```c++
// addr is the externally input GM address, and value is the data to be written to the GM memory, of type int32_t.
int32_t value = 2;
asc_store_dev(addr, value);
```
