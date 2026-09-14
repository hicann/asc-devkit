# asc_set_ffts_base_addr

<!-- md-trans-meta sourceCommit=0ece502ce5e30f12c91b14fc87b28b1b051cb796 translatedAt=2026-09-05T13:23:43.795Z pushedAt=2026-09-08T03:45:05.434Z -->

## Applicable Products

<!-- npu="950" id1 -->
- Ascend 950PR/Ascend 950DT: Not supported
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

Used before [asc_sync_block_wait](../sync/asc_sync_block_wait.md) and [asc_sync_block_arrive](../sync/asc_sync_block_arrive.md) to set the base address of the inter-core synchronization register. It must be obtained on the host side by calling the API aclrtGetHardwareSyncAddr and passed in as a parameter.

```cpp
aclError aclrtGetHardwareSyncAddr(void **addr);
```

## Prototype

```cpp
__aicore__ inline void asc_set_ffts_base_addr(uint64_t config)
```

## Parameters

| Parameter  | Input/Output | Description |
| :----- | :------- | :------- |
| config | Input | Base address of inter-core synchronization. Value range: [0, 2^48-1]. |

## Return Value

None

## Pipeline Type

PIPE_S

## Constraints

- Before using [asc_sync_block_wait](../sync/asc_sync_block_wait.md) and [asc_sync_block_arrive](../sync/asc_sync_block_arrive.md), this API must be used to set the base address.

## Example

```cpp
// Call the API aclrtGetHardwareSyncAddr on the host side to obtain the inter-core synchronization base address ffts_addr.
uint64_t config = *(__gm__ uint64_t*)ffts_addr;
asc_set_ffts_base_addr(config);
```
