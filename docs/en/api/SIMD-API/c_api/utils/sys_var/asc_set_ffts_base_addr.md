---
okftype: reference.api
description: "Configuration API that sets the base address of the inter-core synchronization register. The input parameter is the address obtained by aclrtGetHardwareSyncAddr on the host, with a value range from 0 to 2 to the power of 48 minus 1. It must be called before asc_sync_block_wait and asc_sync_block_arrive."
tag:
  - roles: [configuration]
dependencies:
  - path: docs/zh/api/SIMD-API/c_api/sync/inter_core_sync/asc_sync_block_arrive.md
    kind: requires
    condition: "The base address must be set before using this synchronization API"
  - path: docs/zh/api/SIMD-API/c_api/sync/inter_core_sync/asc_sync_block_wait.md
    kind: requires
    condition: "The base address must be set before using this synchronization API"
version: ">=9.0.0"
version_changes: []
---
# asc_set_ffts_base_addr

<!-- md-trans-meta sourceCommit=54f1d966cfcc22290ddc2c6312bbe4c6ac2a9232 translatedAt=2026-11-16T01:40:03.648707+00:00 pushedAt=2026-11-16T02:10:58.060Z nl=2 -->

## Applicable Products

<!-- npu="950" id1 -->
- Ascend 950PR&950DT products: Not supported
<!-- end id1 -->
<!-- npu="A3" id2 -->
- Atlas A3 products: Supported
<!-- end id2 -->
<!-- npu="910b" id3 -->
- Atlas A2 products: Supported
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

The header file path is `"c_api/utils/sys_var.h"`.

Used before [asc_sync_block_wait](../../sync/inter_core_sync/asc_sync_block_wait.md) and [asc_sync_block_arrive](../../sync/inter_core_sync/asc_sync_block_arrive.md) to set the base address of the inter-core synchronization register. It must be obtained on the host side by calling the API aclrtGetHardwareSyncAddr and passed in as a parameter.

```cpp
aclError aclrtGetHardwareSyncAddr(void **addr);
```

## Prototype

```cpp
__aicore__ inline void asc_set_ffts_base_addr(uint64_t config)
```

## Parameters

**Table 1** Parameters

| Parameter  | Input/Output | Description |
| :----- | :------- | :------- |
| config | Input | Base address of inter-core synchronization. Value range: [0, 2^48-1]. |

## Return Value

None

## Pipeline Type

PIPE_S

## Constraints

- Before using [asc_sync_block_wait](../../sync/inter_core_sync/asc_sync_block_wait.md) and [asc_sync_block_arrive](../../sync/inter_core_sync/asc_sync_block_arrive.md), this API must be used to set the base address.

## Examples

```cpp
// Call the API aclrtGetHardwareSyncAddr on the host to obtain the inter-core synchronization base address ffts_addr.
uint64_t config = *(__gm__ uint64_t*)ffts_addr;
asc_set_ffts_base_addr(config);
```
