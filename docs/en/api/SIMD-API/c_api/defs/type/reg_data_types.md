---
okftype: reference.concept
description: "Overview of the C API register types for Ascend 950PR&950DT, listing the vector data register, vector_bool mask register, unaligned move register, and addr_reg address register by purpose, and explaining how they work with the mask and Reg move-in/move-out interfaces."
---
# Register Types

<!-- md-trans-meta sourceCommit=54f1d966cfcc22290ddc2c6312bbe4c6ac2a9232 translatedAt=2026-11-16T12:16:37.822Z pushedAt=2026-11-18T08:15:34.409Z nl=2 -->

The C API of the Ascend 950PR&950DT products (NPU architecture version 3510) provides vector data register, mask register, unaligned move register, and address register types. Register types are mainly used inside the Vector Function of the AIV (functions marked with `__simd_vf__`), and the finest-grained public header file is `c_api/defs/type.h`.

The purposes of each register type are as follows.

| C API Type | Purpose |
| --- | --- |
| `vector_<dtype>`, for example `vector_int8_t`, `vector_half`, `vector_float` | Stores the data operands and results of vector computation. The vector data register length of NPU architecture version 3510 is 256 bytes. |
| `vector_bool` | Controls whether vector instructions take effect element-wise. The mask register length is 256 bits. |
| `vector_load_unalign` | Stores the intermediate state of UB unaligned load. |
| `vector_store_unalign` | Stores the intermediate state of UB unaligned store. |
| `addr_reg` | Stores the address offset, used together with Reg interfaces that support address registers. |

## Usage Notes

- Vector data register types are divided into b8, b16, b32, and b64 categories based on element bit width. For the complete type list and packed type descriptions, see [Register Type Definition](data_type_definition.md).
- `vector_bool` can be constructed by the mask generation interface, or its mask value can be read from a special register via [asc_get_mask_spr](../../reg_compute/reg_mask/asc_get_mask_spr.md).
- Data exchange between Reg and UB is completed through the [Reg Load](../../reg_compute/load/reg_load_overview.md) and [Reg Store](../../reg_compute/store/reg_store_overview.md) interfaces. Data in GM cannot be loaded directly into Reg; it must first be moved to UB.
- `vector_load_unalign` and `vector_store_unalign` are used only for the corresponding unaligned load and store interface sequences, not for general vector computation.
- `addr_reg` can be updated via [asc_update_addr_reg](../../reg_compute/reg_addr_reg/asc_update_addr_reg.md). The valid range of a register object is determined jointly by the Vector Function and the specific interface constraints.
