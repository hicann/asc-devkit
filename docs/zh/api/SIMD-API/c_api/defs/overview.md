# 概述

## API列表

### 联合体

**表1** 联合体

| 名称 | 描述 | 最细粒度公共头文件 |
| --- | --- | --- |
| [asc_fill_value_config](union/asc_fill_value_config.md) | `fill_value`的初始化参数结构体，包含[asc_fill_l0a](../cube_datamove/cube_load_aux_config/asc_fill_l0a.md)/[asc_fill_l0b](../cube_datamove/cube_load_aux_config/asc_fill_l0b.md)/[asc_fill_l1](../cube_datamove/cube_load_aux_config/asc_fill_l1.md)接口需要配置的各种初始化参数。 | `c_api/defs/union.h` |
| [asc_l13d_fmatrix_config](union/asc_l13d_fmatrix_config.md) | 用于设置[asc_copy_l12l0a](../cube_datamove/cube_compute_load/asc_copy_l12l0a.md)/[asc_copy_l12l0b](../cube_datamove/cube_compute_load/asc_copy_l12l0b.md)3D格式搬运接口的Feature map属性参数。 | `c_api/defs/union.h` |
| [asc_load3d_v2_config](union/asc_load3d_v2_config.md) | Load3Dv2接口的`repeat`参数。 | `c_api/defs/union.h` |
| [asc_ndim_pad_count_config](union/asc_ndim_pad_count_config.md) | 用于[asc_set_ndim_pad_count](../vector_datamove/asc_set_ndim_pad_count.md)接口中，设置[asc_ndim_copy_gm2ub](../vector_datamove/asc_ndim_copy_gm2ub.md)接口的各个维度左右侧的padding元素个数。 | `c_api/defs/union.h` |
| [asc_store_atomic_config](union/asc_store_atomic_config.md) | 原子操作启用位与原子操作类型的值。 | `c_api/defs/union.h` |

### 枚举

**表2** 枚举

| 名称 | 描述 | 最细粒度公共头文件 |
| --- | --- | --- |
| [asc_channel_pad_mode](enum/asc_channel_pad_mode.md) | 表示GM到L1 Buffer搬运时，每个32字节通道块的填充或压缩模式。 | `c_api/defs/enum.h` |
| [asc_dual_dst_mode](enum/asc_dual_dst_mode.md) | 表示L0C Buffer数据搬出时的双目标拆分模式。 | `c_api/defs/enum.h` |
| [asc_hf32_round_mode](enum/asc_hf32_round_mode.md) | 表示Mmad计算开启HF32模式时由FP32舍入到HF32的舍入模式管理策略。 | `c_api/defs/enum.h` |
| [asc_l13d_repeat_direction](enum/asc_l13d_repeat_direction.md) | 表示3D img2col搬运的`repeat`迭代方向。 | `c_api/defs/enum.h` |
| [asc_load_l2_cache_mode](enum/asc_load_l2_cache_mode.md) | 表示GM到UB或L1 Buffer的数据搬运及标量load路径的L2 Cache管理策略。 | `c_api/defs/enum.h` |
| [asc_mutex_execute_mode](enum/asc_mutex_execute_mode.md) | 表示互斥锁的阻塞或非阻塞执行模式。 | `c_api/defs/enum.h` |
| [asc_override_strategy](enum/asc_override_strategy.md) | 表示饱和控制策略。 | `c_api/defs/enum.h` |
| [asc_position_mode](enum/asc_position_mode.md) | 表示源或目的操作数在寄存器中的奇偶位置。 | `c_api/defs/enum.h` |
| [asc_position_quarter_mode](enum/asc_position_quarter_mode.md) | 表示源或目的操作数在寄存器中的四分之一位置。 | `c_api/defs/enum.h` |
| [asc_quant_mode](enum/asc_quant_mode.md) | 表示L0C Buffer数据搬出时的类型转换或量化模式。 | `c_api/defs/enum.h` |
| [asc_relu_pre_mode](enum/asc_relu_pre_mode.md) | 表示L0C Buffer数据搬出时的随路ReLU模式。 | `c_api/defs/enum.h` |
| [asc_saturation_mode](enum/asc_saturation_mode.md) | 表示饱和控制模式。 | `c_api/defs/enum.h` |
| [asc_store_l2_cache_mode](enum/asc_store_l2_cache_mode.md) | 表示UB或L0C Buffer到GM的数据搬运及标量store路径的L2 Cache管理策略。 | `c_api/defs/enum.h` |
| [asc_unit_flag_mode](enum/asc_unit_flag_mode.md) | 表示矩阵计算或矩阵搬出接口执行后的UnitFlag行为。 | `c_api/defs/enum.h` |

### 寄存器类型

**表3** 寄存器类型

| 名称 | 描述 | 最细粒度公共头文件 |
| --- | --- | --- |
| [寄存器类型总览](type/reg_data_types.md) | 介绍C API寄存器类型、用途及使用入口。 | `c_api/defs/type.h` |
| [寄存器类型定义](type/data_type_definition.md) | 介绍C API的矢量数据寄存器、掩码寄存器、非对齐寄存器和地址寄存器数据类型。 | `c_api/defs/type.h` |

### 宏

**表4** 宏

| 名称 | 描述 | 最细粒度公共头文件 |
| --- | --- | --- |
| [ASC_DEPRECATED](macro/asc_deprecation.md) | 为C API声明添加废弃属性和编译告警信息。 | `c_api/defs/macro.h` |
| [ASC_IS_AIC/ASC_IS_AIV](macro/asc_is_aic-asc_is_aiv.md) | 判断当前编译的代码路径是Cube Core（AIC）还是Vector Core（AIV）代码路径。 | `c_api/defs/macro.h` |

### 常量

**表5** 常量

| 名称 | 描述 | 最细粒度公共头文件 |
| --- | --- | --- |
| [ASC_C_API_MRGSORT_ELEMENT_LEN](constant/asc_c_api_mrgsort_element_len.md) | MrgSort4操作支持的队列数量。 | `c_api/defs/constant.h` |
| [ASC_VDEQ_SIZE](constant/asc_vdeq_size.md) | `asc_set_deq_scale`接口中量化参数数组的元素个数。 | `c_api/defs/constant.h` |
