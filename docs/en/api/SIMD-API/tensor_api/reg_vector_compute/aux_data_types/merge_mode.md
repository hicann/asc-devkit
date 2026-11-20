# merge_mode

<!-- md-trans-meta sourceCommit=dc9f009438d2a4868c62d066ed76526a6e83e20c translatedAt=2026-11-19T02:43:13.780Z pushedAt=2026-11-19T05:54:06.653Z nl=2 -->

## Function Description

Header file path: `"tensor_api/experimental/utils/reg_enum.h"`.

`merge_mode` specifies how elements in the destination register that are not selected by the mask are handled in a register write operation with a mask.

## Type Definition

```cpp
enum class merge_mode {
    zeroing,
    merging,
};
```

## Enum Value Description

| Enum Value | Description |
| :-- | :-- |
| zeroing | Elements not selected by the mask are set to 0 in the destination register. |
| merging | Elements not selected by the mask retain their original values in the destination register. |

## Usage

`merge_mode` is used as a template parameter. Using the [fill](../data_padding/fill.md) API as an example, `merge_mode::zeroing` is used by default. To retain the elements in the destination register that are not selected by the mask, specify `merge_mode::merging` as follows:

```cpp
dst = asc::te::experimental::fill<asc::te::experimental::merge_mode::merging>(dst, value, mask);
```

Using the `fill` API as an example, the zeroing and merging modes are illustrated as follows:

**Figure 1** zeroing/merging comparison<a id="fig1"></a>

![](../../../../figures/zeroing_merging_comparison.png "zeroing/merging comparison")
