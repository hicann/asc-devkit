# grid_group_constructor

<!-- md-trans-meta sourceCommit=22e1e8c65e425d4c0fb07294311f2eac049ab0b3 translatedAt=2026-10-23T12:33:49.126Z pushedAt=2026-10-26T06:23:22.642Z nl=2 -->

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
<!-- @ref: asc-devkit/res/docs/zh/api/SIMT-API/cooperative_groups/grid_group/grid_group_constructor_res.md#id1 -->

## Description

The header file path is `"simt_api/cooperative_groups.h"`.

`grid_group` does not provide a default constructor. Users obtain the `grid_group` object corresponding to the current grid by using the `this_grid` function.

## Prototype

```cpp
grid_group this_grid()
```

## Parameters

None

## Return Value

Returns the `grid_group` object corresponding to the current grid.

## Constraints

- Only the SIMT programming scenario is supported. The mixed SIMD and SIMT programming scenario is not supported.

## Examples

See [Example](sync.md#examples)
