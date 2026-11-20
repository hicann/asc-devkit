---
okftype: reference.enum
description: "Enum of padding or compression modes that take effect per 32-byte channel block when moving data from GM to L1 Buffer, serving as the typed value of the pad_func_mode parameter of the asc_copy_gm2l1 high-dimensional split data movement interface. The EXPAND series inserts padding values, and the COMPACT series retains only the low-order valid region."
tag:
  - roles: [data_movement_configuration]
  - traits: [datablock_granularity, padding]
dependencies:
  - path: docs/zh/api/SIMD-API/c_api/cube_datamove/cube_compute_load/asc_copy_gm2l1_highdim_split_arch_3510.md
    kind: related
    condition: "This high-dimensional split data movement interface uses this enum as the pad_func_mode parameter"
  - path: docs/zh/api/SIMD-API/c_api/cube_datamove/cube_load_aux_config/asc_set_gm2l1_pad.md
    kind: related
    condition: "The left/right padding configured by this interface and the in-channel-block padding of this enum belong to different levels"
  - path: docs/zh/api/SIMD-API/c_api/cube_datamove/cube_load_aux_config/asc_set_gm2l1_padding.md
    kind: related
    condition: "The channel padding values in EXPAND mode are set by this interface"
version: ">=9.2.0"
version_changes: []
---
# asc_channel_pad_mode

<!-- md-trans-meta sourceCommit=54f1d966cfcc22290ddc2c6312bbe4c6ac2a9232 translatedAt=2026-11-16T12:08:54.706Z pushedAt=2026-11-18T08:15:34.387Z nl=2 -->

The header file path is: `"c_api/defs/enum.h"`.

asc_channel_pad_mode is used to represent the padding or compaction mode of each 32-byte channel block when moving data from GM to L1 Buffer, and can be used for the [asc_copy_gm2l1](../../cube_datamove/cube_compute_load/asc_copy_gm2l1_highdim_split_arch_3510.md) high-dimensional split data movement interface. This mode is a typed definition of the hardware `pad_func_mode`, takes effect on each 32-byte channel data block, and its value range is [0, 8].

`EXPAND_*` is the padding mode, which inserts padding values into the destination data. The channel padding values are set through [asc_set_gm2l1_padding](../../cube_datamove/cube_load_aux_config/asc_set_gm2l1_padding.md).

`COMPACT_*` is the deletion mode, which removes the high-order portion of data from each 32-byte source data block and retains only the low-order valid region.

This enum controls the padding or deletion inside a channel block, which is different from the left and right padding configured through [asc_set_gm2l1_pad](../../cube_datamove/cube_load_aux_config/asc_set_gm2l1_pad.md).

## Enum Class Definition

```cpp
enum class asc_channel_pad_mode : uint8_t {
    NONE = 0,
    EXPAND_1B = 1,
    EXPAND_2B = 2,
    EXPAND_4B = 3,
    EXPAND_8B = 4,
    EXPAND_16B = 5,
    COMPACT_4B = 6,
    COMPACT_8B = 7,
    COMPACT_16B = 8
};
```

## Enum Value Details

**Table 1** Enum value description

| Enum Value | Value | Description |
| --- | --- | --- |
| NONE | 0 | Does not perform channel padding or compaction. |
| EXPAND_1B | 1 | Inserts 31 bytes of padding values after each 1 byte of source data, where the bit width of each inserted padding is b8. |
| EXPAND_2B | 2 | Inserts 15 padding values after each 2 bytes of source data, where the bit width of each inserted padding is b16. |
| EXPAND_4B | 3 | Inserts 14 padding values after each 4 bytes of source data, where the bit width of each inserted padding is b16. |
| EXPAND_8B | 4 | Inserts 12 padding values after each 8 bytes of source data, where the bit width of each inserted padding is b16. |
| EXPAND_16B | 5 | Inserts 8 padding values after each 16 bytes of source data, where the bit width of each inserted padding is b16. |
| COMPACT_4B | 6 | For each 32 bytes of source data, retains the low 4 bytes, discards the high 28 bytes, and retains only the 4 least significant effective bit bytes. |
| COMPACT_8B | 7 | For each 32 bytes of source data, retains the low 8 bytes, discards the high 24 bytes, and retains only the 8 least significant effective bit bytes. |
| COMPACT_16B | 8 | For each 32 bytes of source data, retains the low 16 bytes, discards the high 16 bytes, and retains only the 16 least significant effective bit bytes. |
