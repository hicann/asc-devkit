# asc_get_system_cycle

<!-- md-trans-meta sourceCommit=b89e273df218e4b8c2c6857aeec20f0a35233b01 translatedAt=2026-09-05T13:20:28.644Z pushedAt=2026-09-08T03:45:05.432Z -->

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

<!-- npu="950" id8 -->
- For Ascend 950PR/Ascend 950DT, to convert to time, use a frequency of 1 GHz with the time unit of us. The conversion formula is: time = (cycle count/1000) us.
<!-- end id8 -->

<!-- npu="A3" id9 -->
- For Atlas A3 training products/Atlas A3 inference products, to convert to time, use a frequency of 50 MHz with the time unit of us. The conversion formula is: time = (cycle count/50) us.
<!-- end id9 -->

<!-- npu="910b" id10 -->
- For Atlas A2 training products/Atlas A2 inference products, to convert to time, use a frequency of 50 MHz with the time unit of us. The conversion formula is: time = (cycle count/50) us.
<!-- end id10 --> 

## Prototype

```cpp
__aicore__ inline int64_t asc_get_system_cycle()
```

## Parameters

None

## Return Value

Returns the system cycle count.

## Pipeline Type

PIPE_S

## Constraints

This API runs on the PIPE_S pipeline. To measure the instruction time of other pipelines, insert synchronization for the corresponding pipeline before calling this API.

## Example

```cpp
int64_t system_cycle_before = asc_get_system_cycle(); // Obtain the cycle count before the computation operation instruction.
asc_sync_notify(PIPE_S, PIPE_V, 0);
asc_sync_wait(PIPE_S, PIPE_V, 0);

......// Perform the computation operation on the PIPE_V pipeline.

asc_sync_notify(PIPE_V, PIPE_S, 0);
asc_sync_wait(PIPE_V, PIPE_S, 0);

int64_t system_cycle_after = asc_get_system_cycle(); // Obtain the cycle count after the computation operation instruction.
int64_t total_cycle = system_cycle_after - system_cycle_before; // Number of cycles consumed by executing the Add instruction.
```