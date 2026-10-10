# GM与L1/L0数据搬运

NPU架构版本3510提供GM到L1 Buffer和L0C Buffer到GM两类直达通路。矩阵输入从GM进入L0A Buffer/L0B Buffer时，需要先通过PIPE_MTE2搬到L1 Buffer，再通过PIPE_MTE1装载到L0A Buffer/L0B Buffer；矩阵计算结果可通过PIPE_FIX从L0C Buffer直接搬回GM。NPU架构版本3510不提供GM到L0A Buffer/L0B Buffer或L1 Buffer到GM的直达搬运。

## 总体说明

| 方向 | 搬运模式 | 接口 | 流水 |
| --- | --- | --- | --- |
| GM到L1 | 连续搬运，支持通道Padding | [asc_copy_gm2l1](../cube_datamove/cube_compute_load/asc_copy_gm2l1_highdim_split_arch_3510.md) | PIPE_MTE2 |
| GM到L1 | 高维切分搬运，支持通道Padding | [asc_copy_gm2l1](../cube_datamove/cube_compute_load/asc_copy_gm2l1_highdim_split_arch_3510.md) | PIPE_MTE2 |
| GM到L1 | 二维分形矩阵搬运 | [asc_copy_gm2l1](../cube_datamove/cube_compute_load/asc_copy_gm2l1_2d_arch_3510.md) | PIPE_MTE2 |
| GM到L1 | ND到Nz格式转换 | [asc_copy_gm2l1_nd2nz](../cube_datamove/cube_compute_load/asc_copy_gm2l1_nd2nz_arch_3510.md) | PIPE_MTE2 |
| GM到L1 | DN到Nz格式转换 | [asc_copy_gm2l1_dn2nz](../cube_datamove/cube_compute_load/asc_copy_gm2l1_dn2nz.md) | PIPE_MTE2 |
| GM到L1 | 非对齐搬运和Padding | [asc_copy_gm2l1_align](../cube_datamove/cube_compute_load/asc_copy_gm2l1_align.md) | PIPE_MTE2 |
| L0C到GM | 矩阵结果搬出，可组合量化、激活和格式转换 | [asc_copy_l0c2gm](../cube_datamove/cube_compute_store/asc_copy_l0c2gm_arch_3510.md) | PIPE_FIX |

## asc_copy_gm2l1（GM到L1连续或高维切分数据搬运）

该接口按照`n_burst`指定的数据块个数执行搬运。每个数据块包含`len_burst`个32字节单元，`src_stride`和`dst_stride`表示源、目的相邻数据块起始地址之间的距离，单位同样为32字节。只搬运一段连续数据时，可将`n_burst`设为1，并将两个步长设为0。

```c
__aicore__ inline void asc_copy_gm2l1(__cbuf__ void* dst,
                                      __gm__ void* src,
                                      uint32_t n_burst,
                                      uint32_t len_burst,
                                      asc_channel_pad_mode pad_mode,
                                      uint64_t src_stride,
                                      uint32_t dst_stride)
```

- `dst`需要32字节对齐，`src`需要1字节对齐。
- `pad_mode`用于选择不处理、按通道插入Padding或删除Padding。使用插入Padding模式时，先通过[asc_set_gm2l1_padding](../cube_datamove/cube_load_aux_config/asc_set_gm2l1_padding.md)配置填充值；该配置接口不设置Padding模式。
- 插入Padding时，每个数据块的`len_burst`需要按接口要求设置，实际写入L1的数据量可能与GM读取量不同。
- 删除Padding时，目的端实际写入量由删除模式决定，计算L1空间时不能只使用源端读取量。

## asc_copy_gm2l1（GM到L1二维分形矩阵搬运）

二维模式从GM矩阵中按行列起始位置和步长选取完整分形，并写入L1。每个分形固定为512字节，搬运过程中不执行格式转换。

```c
__aicore__ inline void asc_copy_gm2l1(__cbuf__ <dtype>* dst,
                                      __gm__ <dtype>* src,
                                      uint32_t m_start_position,
                                      uint32_t k_start_position,
                                      uint16_t dst_stride,
                                      uint16_t m_step,
                                      uint16_t k_step,
                                      asc_load_l2_cache_mode l2_cache_mode)
```

- `m_start_position`和`m_step`以16个元素为单位，`k_start_position`和`k_step`以32字节为单位。
- `dst_stride`以512字节为单位，表示目的矩阵列方向相邻分形起始地址的距离。
- `m_step`或`k_step`为0时不执行搬运；接口不支持只搬运分形内的一部分元素。
- 边界不足一个分形时，应在GM侧按后续矩阵计算要求准备完整分形。

## asc_copy_gm2l1_nd2nz（GM到L1随路ND2Nz搬运）

该接口从GM读取ND排布矩阵，在写入L1时转换为Nz分形排布。源矩阵可包含多个ND矩阵，`src_d_value`描述同一矩阵相邻行起始地址间距，`src_nd_matrix_stride`描述相邻ND矩阵起始地址间距。

```c
__aicore__ inline void asc_copy_gm2l1_nd2nz(__cbuf__ <dtype>* dst,
                                            __gm__ <dtype>* src,
                                            uint64_t src_d_value,
                                            asc_load_l2_cache_mode l2_cache_mode,
                                            uint16_t n_value,
                                            uint32_t d_value,
                                            uint64_t src_nd_matrix_stride,
                                            bool enable_small_c0)
```

- 调用前通过[asc_set_gm2l1_nz_para](../cube_datamove/cube_load_aux_config/asc_set_gm2l1_nz_para.md)设置目的Nz矩阵步长和ND矩阵个数。
- `src_d_value`和`src_nd_matrix_stride`单位为字节，`n_value`和`d_value`单位为元素。
- 当每行有效数据不足32字节对齐时，标准模式在目的矩阵补0到32字节边界。
- 仅当`d_value`不大于4时可开启SmallC0模式；此时按4个元素粒度补齐。

## asc_copy_gm2l1_dn2nz（GM到L1随路DN2Nz搬运）

该接口在搬运时把GM中的DN排布转换为L1中的Nz排布，适用于深度方向连续的数据。参数形式与ND2Nz接口相近，但`n_value`表示DN矩阵列数，`d_value`表示DN矩阵行数。

- 调用前同样通过`asc_set_gm2l1_nz_para`设置目的Nz矩阵步长和DN矩阵个数。
- `src_nd_matrix_stride`为0且矩阵个数大于1时，表示重复读取第一个DN矩阵。
- SmallC0的开启条件、地址对齐和L1容量要求与接口文档一致。

## asc_copy_gm2l1_align（GM到L1非对齐数据搬运）

该接口支持有效块长不是32字节整数倍的数据，并可在目的L1中执行补齐。高维切分模式通过数据块个数、有效字节数、左右Padding、填充来源、L2 Cache策略和源/目的步长描述搬运。

```c
__aicore__ inline void asc_copy_gm2l1_align(__cbuf__ <dtype>* dst,
                                            __gm__ <dtype>* src,
                                            uint32_t burst_count,
                                            uint32_t burst_len,
                                            uint8_t left_padding_count,
                                            uint8_t right_padding_count,
                                            bool enable_data_select,
                                            asc_load_l2_cache_mode l2_cache_mode,
                                            uint64_t src_stride,
                                            uint32_t dst_stride)
```

- **Compact模式：** `dst_stride`等于`burst_len`，左右Padding为0。多个数据块在L1中紧密排列，仅在整体末尾补齐到32字节边界。
- **Normal模式：** `dst_stride`不等于`burst_len`且为32字节的整数倍，每个数据块分别补齐。该对齐约束在`burst_count`为1时仍生效。
- **填充值来源：** 左右Padding均为0时，`enable_data_select`为`false`表示使用每个数据块的首元素填充，为`true`表示使用[asc_set_gm2l1_pad](../cube_datamove/cube_load_aux_config/asc_set_gm2l1_pad.md)预先配置的常量。`enable_data_select`为`true`时，即使左右Padding均为0，也必须先配置填充值。
- **左右Padding模式：** `left_padding_count`或`right_padding_count`非0时，左、右数量的单位为元素，`enable_data_select`不生效，硬件强制使用常量填充。此时必须先调用`asc_set_gm2l1_pad`配置填充值。
- **循环填充模式：** 通过[asc_set_gm2l1_loop_size](../cube_datamove/cube_load_aux_config/asc_set_gm2l1_loop_size.md)、[asc_set_gm2l1_loop1_stride](../cube_datamove/cube_load_aux_config/asc_set_gm2l1_loop1_stride.md)和[asc_set_gm2l1_loop2_stride](../cube_datamove/cube_load_aux_config/asc_set_gm2l1_loop2_stride.md)配置两层循环，不能与左右Padding同时开启。

`burst_len`、`src_stride`和`dst_stride`的单位均为字节；`src`需要1字节对齐，`dst`需要32字节对齐。循环填充模式的loop1、loop2目的步长也必须32字节对齐，对应循环次数为1时仍生效。目的L1空间需要包含有效数据、左右Padding、补齐到32字节边界的dummy数据，以及`dst_stride`和loop目的步长产生的最大偏移。

## asc_copy_l0c2gm（L0C到GM随路处理搬运）

该接口将Cube计算结果从L0C搬到GM，可在PIPE_FIX中组合以下能力：

- 输出Nz排布，或执行Nz2ND、Nz2DN格式转换。
- 将`int32_t`或`float`结果转换为4位、8位、16位或32位目的类型。
- 执行scalar或tensor量化。
- 执行Normal、Scalar、Vector或Clip ReLU等激活模式。
- 在满足数据类型和布局约束时执行通道拆分。
- 通过UnitFlag与矩阵计算建立分形粒度的流水并行。

调用前根据功能组合配置以下状态：

| 功能 | 配置接口 |
| --- | --- |
| Nz2ND/Nz2DN | [asc_set_l0c_copy_nz_para](../cube_datamove/cube_store_aux_config/asc_set_l0c_copy_nz_para.md) |
| Nz2DN通道参数 | [asc_set_l0c_copy_channel_para](../cube_datamove/cube_store_aux_config/asc_set_l0c_copy_channel_para.md) |
| scalar量化 | [asc_set_l0c_copy_prequant](../cube_datamove/cube_store_aux_config/asc_set_l0c_copy_prequant.md) |
| tensor量化参数地址 | [asc_set_l0c_copy_config](../cube_datamove/cube_store_aux_config/asc_set_l0c_copy_config.md) |
| ReLU/Leaky ReLU参数 | [asc_set_l0c_copy_relu_alpha](../cube_datamove/cube_store_aux_config/asc_set_l0c_copy_relu_alpha.md)、[asc_set_l0c_copy_lrelu_alpha](../cube_datamove/cube_store_aux_config/asc_set_l0c_copy_lrelu_alpha.md) |

`src`需要64字节对齐，`dst`需要1字节对齐。`n_size`、`m_size`、`dst_stride`和源、目的数据类型必须与格式转换、量化及激活组合匹配；目的GM空间应按转换后的数据类型和排布计算。

## GM到L0A/L0B的两段搬运

NPU架构版本3510的矩阵输入需要按以下顺序准备：

1. 使用本章GM到L1接口把输入数据或分形搬到L1。
2. 在PIPE_MTE2与PIPE_MTE1之间建立数据依赖。
3. 使用[L1与L0数据搬运](l1_l0_data_move.md)中的二维、三维、转置或MX接口把L1数据装载到L0A/L0B。
4. 在PIPE_MTE1与PIPE_M之间建立依赖后发起矩阵计算。

两段搬运的步长单位不同。GM到L1高维切分以32字节描述块长和起始地址间距，L1到L0二维搬运则以16个元素、32字节和512字节分形混合描述矩阵位置，不能直接复用参数。
