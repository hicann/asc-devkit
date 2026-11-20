# cast_layout

<!-- md-trans-meta sourceCommit=dc9f009438d2a4868c62d066ed76526a6e83e20c translatedAt=2026-11-19T02:42:44.514Z pushedAt=2026-11-19T05:54:06.651Z nl=2 -->

## Function Description

Header file path: `"tensor_api/experimental/utils/reg_enum.h"`.

`cast_layout` specifies the position of the narrow bit-width element in each group of data when [cast](../type_conversion/cast.md) performs a bit width change.

## Type Definition

```cpp
enum class cast_layout {
    unknown = -1,
    zero,
    one,
    two,
    three,
};
```

## Enum Value Description

| Enum Value | Description |
| :-- | :-- |
| unknown | No data layout specified. Reserved value, not used by the current `cast` API. |
| zero | For a narrowing conversion, the conversion result is placed at the 0th narrow bit-width position in the destination operand group. For a widening conversion, the 0th narrow bit-width element in the source operand group is selected. |
| one | For a narrowing conversion, the conversion result is placed at the 1st narrow bit-width position in the destination operand group. For a widening conversion, the 1st narrow bit-width element in the source operand group is selected. |
| two | For a narrowing conversion, the conversion result is placed at the 2nd narrow bit-width position in the destination operand group. For a widening conversion, the 2nd narrow bit-width element in the source operand group is selected. |
| three | For a narrowing conversion, the conversion result is placed at the 3rd narrow bit-width position in the destination operand group. For a widening conversion, the 3rd narrow bit-width element in the source operand group is selected. |

The data layouts when the ratio of source operand bit width to destination operand bit width is 1:2, 2:1, 1:4, and 4:1 are shown in the following figures. For the supported `cast_layout` values, see [cast](../type_conversion/cast.md).

![](../../../../figures/src_dst_bitwidth_1to2.png "ratio of source operand bit width to destination operand bit width is 1:2")

![](../../../../figures/src_dst_bitwidth_2to1.png "ratio of source operand bit width to destination operand bit width is 2:1")

![](../../../../figures/src_dst_bitwidth_1to4.png "ratio of source operand bit width to destination operand bit width is 1:4")

![](../../../../figures/src_dst_bitwidth_4to1.png "ratio of source operand bit width to destination operand bit width is 4:1")
