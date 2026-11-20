# PRELOAD

<!-- md-trans-meta sourceCommit=dacba287b3c94417af1e532d9cbd29c398889dea translatedAt=2026-11-02T02:45:43.715Z pushedAt=2026-11-18T01:25:06.668Z nl=2 -->

## Applicable Products

<!-- npu="950" id1 -->
- Ascend 950PR&950DT products: supported
<!-- end id1 -->
<!-- npu="A3" id2 -->
- Atlas A3 products: supported
<!-- end id2 -->
<!-- npu="910b" id3 -->
- Atlas A2 products: supported
<!-- end id3 -->
<!-- npu="910" id4 -->
- Atlas training products: not supported
<!-- end id4 -->
<!-- npu="310p" id5 -->
- Atlas inference products AI Core: supported
<!-- end id5 -->
<!-- npu="310p" id6 -->
- Atlas inference products Vector Core: not supported
<!-- end id6 -->
<!-- npu="310b" id7 -->
- Atlas 200I/500 A2 inference products: not supported
<!-- end id7 -->

## Description

The header file path is `"utils/base/sys_macros.h"`.

Starting from the GM address where the current instruction resides, preloads subsequent instructions into the ICache.

## Function Prototype

```cpp
#define PRELOAD(len)
```

## Parameters

**Table 1**  Parameters

| Parameter | Input/Output | Description |
|--------|-----------|------|
| len | Input | Prefetch length. |

<!-- npu="950,A3,910b" id8 -->
For the following models:
<!-- npu="950" id9 -->
- Ascend 950PR&950DT products
<!-- end id9 -->
<!-- npu="A3" id10 -->
- Atlas A3 products
<!-- end id10 -->
<!-- npu="910b" id11 -->
- Atlas A2 products
<!-- end id11 -->

The unit of `len` is 2K Byte, and its value should be less than the ICache size divided by 2K. The ICache sizes of AIC and AIV are 32KB and 16KB, respectively.
<!-- end id8 -->

<!-- npu="310p" id12 -->
- For the AI Core of Atlas inference products, `len` does not control the prefetch length, which is fixed at 128Byte.
<!-- end id12 -->

## Return Value

None

## Constraints

It can be called only on the Kernel side.

## Example

```cpp
// Preload four units of instructions starting from the current instruction address.
PRELOAD(4);
```
