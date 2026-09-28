# L1到BiasTable数据搬运

BiasTable Buffer保存Mmad使用的bias。NPU架构版本3510通过[asc_copy_l12bt](../cube_datamove/asc_copy_l12bt/asc_copy_l12bt_arch_3510.md)把L1中的偏置数据搬入BiasTable，接口运行在PIPE_MTE1，仅在AIC生效，支持连续和高维切分两种模式。

## 数据类型与随路转换

源数据类型支持`half`、`bfloat16_t`、`int32_t`和`float`，写入BiasTable后的处理方式如下：

| 源类型 | 写入方式 |
| --- | --- |
| `bfloat16_t` | 自动转换为`float`。 |
| `half` | `conv_control`为1时转换为`float`；为0时每个元素扩展为32 bit，高16 bit为占位数据。 |
| `int32_t`、`float` | 保持32位数据格式，`conv_control`需要设为0。 |

## 连续数据搬运

连续模式通过`size`指定一段连续bias数据：

```c
__aicore__ inline void asc_copy_l12bt(uint64_t dst,
                                      __cbuf__ <dtype>* src,
                                      uint32_t size)
```

- `size`单位为字节，需要32字节对齐；未对齐时，实际搬运量向下取整到32字节。
- 源类型为`float`或`int32_t`时，`size / 32`需要是偶数，即实际搬运量需要64字节对齐。
- 连续模式不传入`conv_control`。需要控制`half`转换方式时，应选择高维切分原型，并把数据配置为一个连续数据块。

## 高维切分数据搬运

高维切分模式通过数据块个数、每块长度以及源、目的块间间隔描述bias排布：

```c
__aicore__ inline void asc_copy_l12bt(uint64_t dst,
                                      __cbuf__ <dtype>* src,
                                      uint16_t conv_control,
                                      uint16_t n_burst,
                                      uint16_t len_burst,
                                      uint16_t source_gap,
                                      uint16_t dst_gap)
```

- `len_burst`以32字节为单位，取值范围为[1, 65535]。
- `source_gap`和`dst_gap`表示前一数据块结束地址到下一数据块起始地址的间隔，单位为32字节。
- `n_burst`为1时，两个间隔可设为0；`n_burst`为0时不执行搬运。
- 源类型为`float`或`int32_t`时，`len_burst`和`dst_gap`需要是2的倍数。
- `conv_control`仅对`half`有效，其他数据类型需要设为0。

## 地址、容量与同步

- L1源地址需要32字节对齐，BiasTable目的地址需要64字节对齐。
- L1容量上限为512KB，BiasTable容量上限为4KB；高维切分场景需要把最后一个数据块之前的所有间隔计入地址跨度。
- L1由其他流水写入时，在`asc_copy_l12bt`读取前建立到PIPE_MTE1的依赖。
- BiasTable写入完成后，Mmad通过PIPE_M读取bias前建立PIPE_MTE1到PIPE_M的依赖。
- 多次搬运写入重叠BiasTable区域时，使用同流水同步保证前一条指令完成后再覆盖。
