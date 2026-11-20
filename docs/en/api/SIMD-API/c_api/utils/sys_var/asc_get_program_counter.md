---
okftype: reference.api
description: "Obtains the program counter pointer to locate the current execution position, and can also be used as the start address for instruction prefetching together with the cache prefetch API."
tag:
  - roles: [query]
dependencies:
  - path: docs/zh/api/SIMD-API/c_api/cache_ctrl/asc_icache_preload.md
    kind: related
    condition: "The current instruction address can be used as the prefetch start address"
version: ">=9.0.0"
version_changes: []
---
# asc_get_program_counter

<!-- md-trans-meta sourceCommit=54f1d966cfcc22290ddc2c6312bbe4c6ac2a9232 translatedAt=2026-11-16T01:40:03.619095+00:00 pushedAt=2026-11-16T02:10:57.967Z nl=2 -->

## Applicable Products

<!-- npu="950" id1 -->
- Ascend 950PR&950DT products: Supported
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

Obtains the pointer to the program counter. The program counter records the current program execution position. It is commonly used in debugging to locate the current execution position, and can also be used with APIs such as [asc_icache_preload](../../cache_ctrl/asc_icache_preload.md) to use the current instruction address as the start address for prefetching.

## Prototype

```c
__aicore__ inline int64_t asc_get_program_counter()
```

## Parameters

None

## Return Value

Returns the program counter pointer of type int64_t.

## Pipeline Type

PIPE_S

## Constraints

This API returns a 64-bit program counter value, where the valid address bits are bits 0 to 47 and the upper 16 bits are invalid. As shown in the following code snippet, when using it, you need to clear the invalid upper bits with the mask `0xFFFFFFFFFFFF` (lower 48 bits all 1) to obtain the valid 48-bit address of the current instruction in GM.

```cpp
int64_t prefetch_length = 6;
// Obtain the current program counter (PC) value through asc_get_program_counter.
// PC is a 64-bit return value. The valid address bits are bits 0 to 47. Clear the high invalid bits with the mask 0xFFFFFFFFFFFF (all lower 48 bits set to 1).
// Obtain the valid 48-bit address of the current instruction in GM as the preload start address.
int64_t pc = asc_get_program_counter() & 0xFFFFFFFFFFFF;
asc_icache_preload(reinterpret_cast<void *>(pc), prefetch_length);
```

## Examples

After obtaining the PC, it can be used as the start address for instruction prefetching. For the complete prefetching call method, see [asc_icache_preload Example](../../cache_ctrl/asc_icache_preload.md#examples).
