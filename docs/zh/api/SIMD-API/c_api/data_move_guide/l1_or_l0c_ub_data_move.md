# L1/L0C与UB数据搬运

NPU架构版本3510支持L1到UB、UB到L1以及L0C到UB三条片上通路，用于在Cube侧和Vector侧之间交换输入或中间结果。三条通路均为单向通路，分别属于PIPE_MTE1、PIPE_MTE3和PIPE_FIX；NPU架构版本3510不提供UB到L0C的通用搬运。

## 总体说明

| 方向 | 搬运模式 | 接口 | 流水 | 执行范围 |
| --- | --- | --- | --- | --- |
| UB到L1 | 连续 | [asc_copy_ub2l1](../vector_datamove/asc_copy_ub2l1.md) | PIPE_MTE3 | AIV |
| UB到L1 | 高维切分 | [asc_copy_ub2l1](../vector_datamove/asc_copy_ub2l1.md) | PIPE_MTE3 | AIV |
| L1到UB | 连续 | [asc_copy_l12ub](../cube_datamove/cube_compute_store/asc_copy_l12ub.md) | PIPE_MTE1 | AIC |
| L1到UB | 高维切分 | [asc_copy_l12ub](../cube_datamove/cube_compute_store/asc_copy_l12ub.md) | PIPE_MTE1 | AIC |
| L0C到UB | 搬出矩阵结果，可组合量化、激活、格式转换或双目标写入 | [asc_copy_l0c2ub](../cube_datamove/cube_compute_store/asc_copy_l0c2ub.md) | PIPE_FIX | AIC |

## asc_copy_ub2l1（UB到L1连续数据搬运）

连续模式按`size`指定的字节数从UB搬到L1，数据格式和内容保持不变。

```c
__aicore__ inline void asc_copy_ub2l1(__cbuf__ void* dst,
                                      __ubuf__ void* src,
                                      uint32_t size)
```

- 源UB和目的L1地址均需要32字节对齐。
- `size`单位为字节，取值范围为[32, 4095×32]，并且需要是32的整数倍。
- `size`不对齐时，硬件按`size / 32`截断，多出的字节不会搬运。

## asc_copy_ub2l1（UB到L1高维切分数据搬运）

高维切分模式通过数据块个数、每块长度以及源、目的块间间隔描述非连续排布。

```c
__aicore__ inline void asc_copy_ub2l1(__cbuf__ void* dst,
                                      __ubuf__ void* src,
                                      uint16_t burst_count,
                                      uint16_t burst_len,
                                      uint16_t src_gap,
                                      uint16_t dst_gap)
```

- `burst_len`以32字节DataBlock为单位。
- `src_gap`和`dst_gap`表示前一数据块结束地址到下一数据块起始地址之间的间隔，也以DataBlock为单位。
- `burst_count`为1时，两个间隔可设为0；多块搬运时，源、目的最大访问跨度均需要计入块间间隔。

## asc_copy_l12ub（L1到UB数据搬运）

该接口从AIC发射，把L1数据写入指定AIV的UB。通过`burst_count`、`burst_len`、`src_gap`和`dst_gap`描述高维切分排布；连续场景可配置一个数据块，或令多个数据块的间隔为0。

```c
__aicore__ inline void asc_copy_l12ub(__ubuf__ void* dst_addr,
                                      __cbuf__ void* src_addr,
                                      int8_t sub_blockid,
                                      uint16_t burst_count,
                                      uint16_t burst_len,
                                      uint16_t src_gap,
                                      uint16_t dst_gap)
```

- `src_addr`和`dst_addr`均需要32字节对齐。
- `sub_blockid`为0时写入AIV0的UB，为1时写入AIV1的UB。
- `burst_len`、`src_gap`和`dst_gap`均以32字节为单位；`src_gap`和`dst_gap`表示数据块之间的空隙，而不是相邻块首地址间距。
- L1容量上限为512KB；UB名义容量为256KB，但实际可用空间需要扣除SIMD VF栈、预留空间和可能启用的SIMT Data Cache。

## asc_copy_l0c2ub（L0C到UB随路处理搬运）

该接口将Cube计算结果直接写入一个或两个AIV的UB，可在PIPE_FIX中组合类型转换、量化、激活和格式转换。

```c
__aicore__ inline void asc_copy_l0c2ub(__ubuf__ <dst_dtype>* dst,
                                       __cc__ <src_dtype>* src,
                                       uint16_t n_size,
                                       uint16_t m_size,
                                       uint32_t dst_stride,
                                       uint16_t src_stride,
                                       int8_t sub_blockid,
                                       asc_dual_dst_mode dual_dst_ctrl,
                                       asc_unit_flag_mode unit_flag_mode,
                                       asc_quant_mode quant_pre_mode,
                                       asc_relu_pre_mode relu_pre_mode,
                                       bool enable_channel_split,
                                       bool enable_nz2nd,
                                       bool enable_nz2dn,
                                       bool enable_clip_relu_pre)
```

### 目的AIV选择

- 单目标模式下，`sub_blockid`选择AIV0或AIV1，完整矩阵写入对应UB。
- `asc_dual_dst_mode::DUAL_DST_SPLIT_M`按M维把矩阵一分为二，分别写入两个UB，M需要是2的倍数。
- `asc_dual_dst_mode::DUAL_DST_SPLIT_N`按N维拆分，N需要是32的倍数。
- 双目标仅支持普通Nz2Nz或Nz2ND场景，不能与其他随路能力任意组合。

### 格式、量化与激活

| 功能 | 主接口参数 | 前置配置 |
| --- | --- | --- |
| Nz2ND | `enable_nz2nd` | [asc_set_l0c_copy_nz_para](../cube_datamove/cube_store_aux_config/asc_set_l0c_copy_nz_para.md) |
| Nz2DN | `enable_nz2dn` | `asc_set_l0c_copy_nz_para`、[asc_set_l0c_copy_channel_para](../cube_datamove/cube_store_aux_config/asc_set_l0c_copy_channel_para.md) |
| scalar量化 | `quant_pre_mode` | [asc_set_l0c_copy_prequant](../cube_datamove/cube_store_aux_config/asc_set_l0c_copy_prequant.md) |
| tensor量化 | `quant_pre_mode` | [asc_set_l0c_copy_config](../cube_datamove/cube_store_aux_config/asc_set_l0c_copy_config.md) |
| ReLU/Leaky ReLU | `relu_pre_mode`、`enable_clip_relu_pre` | [asc_set_l0c_copy_relu_alpha](../cube_datamove/cube_store_aux_config/asc_set_l0c_copy_relu_alpha.md)、[asc_set_l0c_copy_lrelu_alpha](../cube_datamove/cube_store_aux_config/asc_set_l0c_copy_lrelu_alpha.md) |

`src_dtype`为`int32_t`或`float`，目的类型由量化或Cast模式决定。源、目的数据类型必须与`quant_pre_mode`匹配，量化系数不能为INF、NAN或非规格化数。

### 步长与边界

- 源L0C地址需要64字节对齐，目的UB地址需要32字节对齐。
- `src_stride`以64字节为单位，表示源Nz矩阵相邻Z排布起始地址的距离。
- `dst_stride`以元素为单位；Nz输出时表示相邻Z排布间距，ND/DN输出时表示一行的元素数，对应字节长度需要32字节对齐。
- L0C和UB访问范围分别不能超过256KB和实际分配的UB空间。双目标模式需要分别检查两个AIV的目的范围。

## 同步要求

- AIV通过PIPE_MTE3把UB写入L1后，AIC读取L1前需要完成AIV到AIC的核间同步并建立后续流水依赖。AIC依赖同一group内全部AIV时，使用模式2的[asc_sync_block_arrive](../sync/inter_core_sync/asc_sync_block_arrive.md)和[asc_sync_block_wait](../sync/inter_core_sync/asc_sync_block_wait.md)，由各AIV发送通知、AIC等待；仅依赖单个AIV时，使用模式4的[asc_sync_intra_arrive](../sync/inter_core_sync/asc_sync_intra_arrive.md)和[asc_sync_intra_wait](../sync/inter_core_sync/asc_sync_intra_wait.md)。
- AIC通过PIPE_MTE1把L1写入UB，或通过PIPE_FIX把L0C写入UB后，AIV读取UB前需要完成AIC到目标AIV的同步。全部AIV均消费数据时使用模式2，由AIC发送通知、各AIV等待；仅指定AIV消费数据时使用模式4。
- L0C由PIPE_M生成，PIPE_FIX搬出前需要同步；满足条件时也可通过UnitFlag让矩阵计算和搬出按分形并行。
- 多条指令写入相同L1或UB范围时，需要按[同步控制](../sync/system_sync_overview.md)要求串行化，不能仅依赖源码顺序。
