# L1到Fixpipe Buffer数据搬运

Fixpipe Buffer保存L0C搬出时使用的随路Vector量化参数和随路ReLU参数。NPU架构版本3510通过[asc_copy_l12fb](../cube_datamove/cube_compute_load/asc_copy_l12fb_arch_3510.md)从L1装载参数，接口运行在PIPE_FIX，仅在AIC生效，支持连续和高维切分两种模式。

## 参数地址空间

量化参数和ReLU参数使用相互独立的Fixpipe Buffer地址空间，由目的地址`dst[19:16]`区分；`dst[15:0]`表示对应地址空间内的偏移。

| 参数空间 | 地址选择 | 目的对齐 | 容量上限 |
| --- | --- | --- | --- |
| 随路Vector量化参数 | `dst[16]=0` | 128字节 | 4KB |
| 随路ReLU参数 | `dst[16]=1` | 64字节 | 2KB |

两类参数的地址偏移需要分别计算，不能把一个空间的末尾地址作为另一个空间的起点。

## 连续数据搬运

连续模式通过`size`装载前n个字节的参数：

```c
__aicore__ inline void asc_copy_l12fb(__fbuf__ void* dst,
                                      __cbuf__ void* src,
                                      uint32_t size)
```

- `size`单位为字节，需要32字节对齐；未对齐时，实际搬运量向下取整到32字节。
- `size`为0时不执行搬运。
- 源L1地址需要32字节对齐，目的地址按参数空间满足64字节或128字节对齐。

## 高维切分数据搬运

高维切分模式可从L1中的多个数据块装载参数，并在源、目的端保留间隔：

```c
__aicore__ inline void asc_copy_l12fb(__fbuf__ void* dst,
                                      __cbuf__ void* src,
                                      uint16_t n_burst,
                                      uint16_t len_burst,
                                      uint16_t src_gap_size,
                                      uint16_t dst_gap_size)
```

- `len_burst`以64字节为单位。
- `src_gap_size`和`dst_gap_size`表示前一数据块结束地址到下一数据块起始地址的间隔，以32字节为单位。
- 随路量化参数场景下，`len_burst`和`dst_gap_size`需要是2的倍数；随路ReLU参数场景没有该偶数约束。
- `n_burst`为0时不执行搬运；多块搬运时，目的跨度不得超过对应参数空间容量。

## 使用与同步

- L1总容量为512KB，源地址偏移与高维切分访问跨度不得越界。
- Fixpipe Buffer越界时硬件可能截断写入并产生错误结果，不能依赖截断保护。
- 参数装载和L0C结果搬出都运行在PIPE_FIX。参数必须在消费它的L0C搬出指令之前准备完成，不得在对应搬出仍执行时覆盖。
- 参数由其他流水写入L1时，需要先建立到PIPE_FIX的依赖；多条`asc_copy_l12fb`写入重叠区域时，使用[asc_sync_pipe](../sync/intra_core_sync/asc_sync_pipe.md)保证先后顺序。
- 量化、激活模式以及参数在Fixpipe Buffer中的布局由L0C搬出接口决定，装载前应先根据[asc_copy_l0c2gm](../cube_datamove/cube_compute_store/asc_copy_l0c2gm_arch_3510.md)、[asc_copy_l0c2l1](../cube_datamove/cube_compute_store/asc_copy_l0c2l1_arch_3510.md)或[asc_copy_l0c2ub](../cube_datamove/cube_compute_store/asc_copy_l0c2ub.md)的功能组合准备参数。
