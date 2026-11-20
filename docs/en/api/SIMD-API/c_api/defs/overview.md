# Overview

<!-- md-trans-meta sourceCommit=1398d4baddf82d4b412922bb4a32a05abf2ada8a translatedAt=2026-11-16T12:05:52.359Z pushedAt=2026-11-18T08:15:34.379Z nl=2 -->

## API List

### Union

**Table 1** Union

| Name | Description | Finest-grained Common Header File |
| --- | --- | --- |
| [asc_fill_value_config](union/asc_fill_value_config.md) | Initialization parameter structure for `fill_value`, containing various initialization parameters that need to be configured for the [asc_fill_l0a](../cube_datamove/cube_load_aux_config/asc_fill_l0a.md)/[asc_fill_l0b](../cube_datamove/cube_load_aux_config/asc_fill_l0b.md)/[asc_fill_l1](../cube_datamove/cube_load_aux_config/asc_fill_l1.md) interfaces. | `c_api/defs/union.h` |
| [asc_l13d_fmatrix_config](union/asc_l13d_fmatrix_config.md) | Used to set the Feature map attribute parameters of the [asc_copy_l12l0a](../cube_datamove/cube_compute_load/asc_copy_l12l0a.md)/[asc_copy_l12l0b](../cube_datamove/cube_compute_load/asc_copy_l12l0b.md) 3D-format copy interfaces. | `c_api/defs/union.h` |
| [asc_load3d_v2_config](union/asc_load3d_v2_config.md) | The `repeat` parameter of the Load3Dv2 interface. | `c_api/defs/union.h` |
| [asc_ndim_pad_count_config](union/asc_ndim_pad_count_config.md) | Used in the [asc_set_ndim_pad_count](../vector_datamove/asc_set_ndim_pad_count.md) interface to set the number of padding elements on the left and right sides of each dimension of the [asc_ndim_copy_gm2ub](../vector_datamove/asc_ndim_copy_gm2ub.md) interface. | `c_api/defs/union.h` |
| [asc_store_atomic_config](union/asc_store_atomic_config.md) | The value of the atomic operation enable bit and the atomic operation type. | `c_api/defs/union.h` |

### Enums

**Table 2** Enums

| Name | Description | Finest-grained Common Header File |
| --- | --- | --- |
| [asc_channel_pad_mode](enum/asc_channel_pad_mode.md) | Indicates the padding or compression mode for each 32-byte channel block when moving data from GM to the L1 Buffer. | `c_api/defs/enum.h` |
| [asc_dual_dst_mode](enum/asc_dual_dst_mode.md) | Indicates the dual-destination split mode when evicting L0C Buffer data. | `c_api/defs/enum.h` |
| [asc_hf32_round_mode](enum/asc_hf32_round_mode.md) | Indicates the rounding mode management policy for rounding from FP32 to HF32 when HF32 mode is enabled for Mmad computation. | `c_api/defs/enum.h` |
| [asc_l13d_repeat_direction](enum/asc_l13d_repeat_direction.md) | Indicates the `repeat` iteration direction for 3D img2col data movement. | `c_api/defs/enum.h` |
| [asc_load_l2_cache_mode](enum/asc_load_l2_cache_mode.md) | Indicates the L2 Cache management policy for data movement from GM to UB or the L1 Buffer and for the scalar load path. | `c_api/defs/enum.h` |
| [asc_mutex_execute_mode](enum/asc_mutex_execute_mode.md) | Indicates the blocking or non-blocking execution mode of a mutex. | `c_api/defs/enum.h` |
| [asc_override_strategy](enum/asc_override_strategy.md) | Indicates the saturation control strategy. | `c_api/defs/enum.h` |
| [asc_position_mode](enum/asc_position_mode.md) | Indicates the even or odd position of the source or destination operand in a register. | `c_api/defs/enum.h` |
| [asc_position_quarter_mode](enum/asc_position_quarter_mode.md) | Indicates the quarter position of the source or destination operand in a register. | `c_api/defs/enum.h` |
| [asc_quant_mode](enum/asc_quant_mode.md) | Indicates the type conversion or quantization mode when evicting L0C Buffer data. | `c_api/defs/enum.h` |
| [asc_relu_pre_mode](enum/asc_relu_pre_mode.md) | Indicates the accompanying ReLU mode when evicting L0C Buffer data. | `c_api/defs/enum.h` |
| [asc_saturation_mode](enum/asc_saturation_mode.md) | Indicates the saturation control mode. | `c_api/defs/enum.h` |
| [asc_store_l2_cache_mode](enum/asc_store_l2_cache_mode.md) | Indicates the L2 Cache management policy for data movement from UB or the L0C Buffer to GM and for the scalar store path. | `c_api/defs/enum.h` |
| [asc_unit_flag_mode](enum/asc_unit_flag_mode.md) | Indicates the UnitFlag behavior after a cube computation or cube eviction interface is executed. | `c_api/defs/enum.h` |

### Register Type

**Table 3** Register type

| Name | Description | Finest-grained Common Header File |
| --- | --- | --- |
| [Register Type Overview](type/reg_data_types.md) | Introduces the C API register types, their purposes, and usage entry points. | `c_api/defs/type.h` |
| [Register Type Definition](type/data_type_definition.md) | Introduces the vector data register, mask register, unaligned register, and address register data types of the C API. | `c_api/defs/type.h` |

### Macros

**Table 4** Macros

| Name | Description | Finest-grained Common Header File |
| --- | --- | --- |
| [ASC_DEPRECATED](macro/asc_deprecation.md) | Adds a deprecation attribute and compile warning information to C API declarations. | `c_api/defs/macro.h` |
| [ASC_IS_AIC/ASC_IS_AIV](macro/asc_is_aic-asc_is_aiv.md) | Determines whether the currently compiled code path is the Cube Core (AIC) or Vector Core (AIV) code path. | `c_api/defs/macro.h` |

### Constants

**Table 5** Constants

| Name | Description | Finest-grained Common Header File |
| --- | --- | --- |
| [ASC_C_API_MRGSORT_ELEMENT_LEN](constant/asc_c_api_mrgsort_element_len.md) | Number of queues supported by the MrgSort4 operation. | `c_api/defs/constant.h` |
| [ASC_VDEQ_SIZE](constant/asc_vdeq_size.md) | Number of elements in the quantization parameter array of the `asc_set_deq_scale` interface. | `c_api/defs/constant.h` |
