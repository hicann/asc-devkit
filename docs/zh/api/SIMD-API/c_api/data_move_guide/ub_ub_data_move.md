# UB与UB数据搬运

NPU架构版本3510通过[asc_copy_ub2ub](../vector_datamove/asc_copy_ub2ub.md)在UB内部复制数据。接口运行在PIPE_V，仅在AIV生效，支持连续和高维切分两种模式，数据格式和内容保持不变。

## 总体说明

| 搬运模式 | 适用场景 | 关键参数单位 |
| --- | --- | --- |
| 连续 | 源、目的均为一段连续UB数据 | `size`：字节 |
| 高维切分 | 源或目的由多个等长数据块组成，块间存在固定间隔 | 块长、源/目的间隔：32字节DataBlock |

需要在UB中按掩码选择元素、广播或完成数据类型相关的重排时，可在Vector Function中使用Reg加载、计算和存储接口，参见[Reg与UB数据搬运](reg_ub_data_move.md)。

## 连续数据搬运

连续模式从`src`开始复制`size`字节到`dst`：

```c
__aicore__ inline void asc_copy_ub2ub(__ubuf__ void* dst,
                                      __ubuf__ void* src,
                                      uint32_t size)
```

- 源、目的地址均需要32字节对齐。
- `size`取值范围为[32, 65535×32]，并且需要是32的整数倍。
- `size`不对齐时，硬件按`size / 32`向下截断，尾部不足32字节的数据不会复制。
- 连续模式的源、目的有效范围不能重叠。

## 高维切分数据搬运

高维切分模式搬运`burst_count`个数据块，每个数据块包含`burst_len`个DataBlock：

```c
__aicore__ inline void asc_copy_ub2ub(__ubuf__ void* dst,
                                      __ubuf__ void* src,
                                      uint16_t burst_count,
                                      uint16_t burst_len,
                                      uint16_t src_gap,
                                      uint16_t dst_gap)
```

- `burst_len`以32字节DataBlock为单位。
- `src_gap`和`dst_gap`表示前一数据块结束地址到下一数据块起始地址的间隔，同样以DataBlock为单位。
- 源访问跨度为`(burst_count - 1) * (burst_len + src_gap) + burst_len`个DataBlock，目的访问跨度使用`dst_gap`按相同方式计算。
- 各个实际参与搬运的源DataBlock和目的DataBlock不能重叠。如果块间间隔使有效DataBlock互不相交，则源、目的整体地址跨度可以交叠。

## 地址重叠示例

假设每个数据块长度为2个DataBlock，共搬运2块：

- `src_gap=1`时，源端有效块为DataBlock 0、1和3、4。
- `dst_gap=2`时，目的端有效块为DataBlock 0、1和4、5。

判断是否支持搬运时，应逐块比较源、目的有效DataBlock，而不是只比较两段整体跨度。接口不提供类似`memmove`的重叠复制语义；需要移动重叠区域时，应使用不重叠的临时UB空间分阶段处理，并在复用空间前完成同步。

## 容量与同步

- 源、目的地址加实际访问跨度均不得超过已分配UB范围。UB名义容量为256KB，实际可用容量还会受SIMD VF栈、预留空间和SIMT Data Cache影响。
- `asc_copy_ub2ub`运行在PIPE_V。在NPU架构版本3510上，PIPE_V内部的执行顺序由硬件保证，多条`asc_copy_ub2ub`之间不需要额外插入事件同步。
- 目的UB随后由PIPE_MTE3或其他流水读取时，使用[asc_sync_notify](../sync/intra_core_sync/asc_sync_notify.md)和[asc_sync_wait](../sync/intra_core_sync/asc_sync_wait.md)建立PIPE_V到消费者流水的依赖。
