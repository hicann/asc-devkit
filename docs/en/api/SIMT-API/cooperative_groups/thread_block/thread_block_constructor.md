# thread_block Constructor

<!-- md-trans-meta sourceCommit=22e1e8c65e425d4c0fb07294311f2eac049ab0b3 translatedAt=2026-10-23T12:44:27.301Z pushedAt=2026-10-26T06:23:22.674Z nl=2 -->

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
<!-- npu="310b" id4 -->
- Atlas 200I/500 A2 inference products: Not supported
<!-- end id4 -->
<!-- npu="310p" id5 -->
- Atlas inference products AI Core: Not supported
- Atlas inference products Vector Core: Not supported
<!-- end id5 -->
<!-- npu="910" id6 -->
- Atlas training products: Not supported
<!-- end id6 -->
<!-- @ref: asc-devkit/res/docs/zh/api/SIMT-API/cooperative_groups/thread_block/thread_block_constructor_res.md#id1 -->

## Description

The header file path is `"simt_api/cooperative_groups.h"`.

`thread_block` does not provide a default constructor. Users obtain the `thread_block` object to which the current thread belongs by using the `this_thread_block` function.

## Prototype

```cpp
// Use this API to create a thread_block in the default scenario.
thread_block this_thread_block()
```

```cpp
// Use this API to create a thread_block when a cross-warp thread_block_tile (size > 32) needs to be created later.
template <unsigned int MaxBlockSize>
thread_block this_thread_block(block_tile_memory<MaxBlockSize>& scratch)
```

## Parameters

### Interface Parameters

**Table 1**  Parameters

| Name | Input/Output | Description |
| --- | --- | --- |
| scratch | Input | [block_tile_memory](#block_tile_memory-description) object in Global Memory or UB, used to carry cross-Warp temporary storage for `thread_block`. |

### block_tile_memory Description

`block_tile_memory` provides the temporary storage required for intra-group synchronization and cross-Warp communication of `thread_block_tile` across Warps. The size of the structure can be obtained through the `sizeof()` API.

```cpp
template <unsigned int MaxBlockSize = 1024>
struct block_tile_memory
```

**Table 2**  `block_tile_memory` template parameter description

| Name | Input/Output | Description |
| --- | --- | --- |
| MaxBlockSize | Input | Template parameter that specifies the maximum number of threads to be supported in the current thread block. It must be greater than 0 and less than or equal to 2048, and must be an integer multiple of the Warp size 32. The default value is 1024. |

> [!CAUTION] Caution
> The actual number of threads launched in a thread block must not exceed `MaxBlockSize`; otherwise, the behavior is undefined.

## Return Value

Returns the `thread_block` object to which the current thread belongs.

## Constraints

- The actual number of threads launched in a thread block must not exceed `MaxBlockSize`.
- The [block_tile_memory](#block_tile_memory-description) object corresponding to `scratch` must be located in Global Memory or Unified Buffer (UB), and must not be an object created in stack space.

## Examples

See [Example](sync.md#examples)
