# 总体约束说明

数据搬运接口共享地址对齐、存储容量、参数单位和同步等通用规则。调用前应先完成本节检查；具体接口文档给出的要求更严格时，以具体接口为准。

## 地址对齐约束

NPU架构版本3510各存储单元的架构基线对齐如下表所示。

**表1** 存储单元地址对齐要求

| 存储单元 | 地址空间或对象 | 架构基线对齐 | 说明 |
| --- | --- | --- | --- |
| Global Memory | `__gm__` | 1字节 | 接口仍可能按数据类型要求2字节或4字节对齐。 |
| UB | `__ubuf__` | 32字节 | 普通DMA搬运通常要求32字节对齐；NDDMA和部分Reg接口的要求不同。 |
| L1 Buffer | `__cbuf__` | 32字节 | 与DataBlock大小一致。 |
| L0A/L0B Buffer | `__ca__`/`__cb__` | 512字节 | 二维、三维矩阵搬运通常以完整分形为粒度。 |
| L0C Buffer | `__cc__` | 64字节 | L0C搬出接口的源地址要求。 |
| BiasTable Buffer | `uint64_t`目的地址 | 64字节 | 容量上限为4KB。 |
| Fixpipe Buffer量化参数空间 | `__fbuf__` | 128字节 | 容量上限为4KB。 |
| Fixpipe Buffer ReLU参数空间 | `__fbuf__` | 64字节 | 容量上限为2KB。 |

架构基线不等于所有原型的最终约束。例如NDDMA的目的UB地址允许1字节对齐，Reg加载和存储的部分元素或掩码模式可按数据类型、8字节或16字节对齐。地址不满足普通接口要求时，应选择对应的非对齐接口，不能通过向前取整地址或扩大访问范围读写未分配空间。

## 参数单位

数据搬运参数常见以下单位：

- `size`通常以字节为单位，但`asc_copy_ub2ub`、`asc_copy_ub2l1`、`asc_copy_l12bt`和`asc_copy_l12fb`等接口会按32字节粒度处理，不对齐部分可能被向下截断。
- `burst_len`、`len_burst`可能以字节、32字节DataBlock、64字节或512字节分形为单位。
- `src_stride`、`dst_stride`可能表示相邻数据块**起始地址之间**的距离；`src_gap`、`dst_gap`通常表示前一数据块**结束地址到下一数据块起始地址**的间隔。
- L1到L0A/L0B二维搬运中的行列起始位置和步长分别可能以16个元素、32字节或512字节分形为单位。
- NDDMA各维循环大小、源步长、目的步长和Padding数量以元素个数为单位。
- Reg接口的偏移可能以元素、字节或DataBlock为单位。

同名参数在不同接口中的单位不一定相同。计算源、目的地址跨度时，应先按接口文档把所有参数换算为字节，再检查对齐和边界。

## 搬运量与存储边界

[NPU架构版本3510硬件规格](../../../../../guide/programming_guide/advanced_programming/hardware_implementation/architecture_spec/npu_arch_3510.md)中的主要片上存储容量如下：

| 存储单元 | 容量上限 |
| --- | --- |
| UB | 256KB；实际可用空间还会扣除SIMD VF栈、预留空间以及SIMD+SIMT混编时的Data Cache。 |
| L1 Buffer | 512KB |
| L0A/L0B Buffer | 各64KB |
| L0C Buffer | 256KB |
| L0A_MX/L0B_MX Buffer | 各4KB |
| BiasTable Buffer | 4KB |
| Fixpipe Buffer | 量化参数空间4KB，ReLU参数空间2KB |

源、目的起始地址加上实际访问跨度均不得超过已分配范围。高维切分模式需要把最后一个数据块的起始偏移与块长相加；GM与UB之间的非32字节有效长度需要计入UB一侧的32字节补齐访问，即使使用普通接口也不例外；左右Padding也需要计入实际跨度；随路格式或类型转换需要按目的数据类型和目的布局重新计算空间；矩阵搬运还需要计入边界分形中的填充元素。

## 执行范围

- 矩阵数据搬运接口通常仅在AIC生效，矢量数据搬运接口通常仅在AIV生效；在不支持的执行核调用时，部分接口会直接返回而不执行搬运。
- Reg加载、存储接口仅在AIV的Vector Function（`__simd_vf__`标记的函数）内使用。
- L1到UB接口在AIC发射，并通过`sub_blockid`选择目的AIV；AIC与AIV之间共享数据时还需要满足相应核间同步要求。
- 通过宏区分AIC和AIV代码路径时，应保证接口只在其支持的分支执行。

## 多指令同步

搬运指令由Scalar发射后在对应硬件流水异步执行，不能仅根据源码书写顺序推断不同流水之间的完成顺序。同步分为以下两类：

- **同一流水同步：** 多条PIPE_MTE2或PIPE_MTE3指令的目的地址存在重叠时，使用[asc_sync_pipe](../../sync/intra_core_sync/asc_sync_pipe.md)等待前序搬运完成，再发射会覆盖同一区域的后序搬运。
- **不同流水同步：** 生产者和消费者位于不同流水时，使用[asc_sync_notify](../../sync/intra_core_sync/asc_sync_notify.md)和[asc_sync_wait](../../sync/intra_core_sync/asc_sync_wait.md)建立事件依赖，或按接口文档选择对应的易用同步接口。

图1左侧表示两次UB到GM搬运的目的GM范围重叠，图1右侧表示两次GM到UB搬运的目的UB范围重叠。两种场景都需要在同一搬运流水内等待前一条指令完成。

**图1** 搬运目的地址重叠

![搬运目的地址重叠](../../figures/datacopy_address_overlap_sync_diagram.png "搬运目的地址重叠")

以下代码展示PIPE_MTE2上两次GM到UB搬运的目的区域重叠时的同步方式：

```c
asc_copy_gm2ub(ub0, gm0, size0);
asc_sync_pipe(PIPE_MTE2);
asc_copy_gm2ub(ub1, gm1, size1);
```

如果GM到UB搬运的结果随后由PIPE_V读取，则需要建立PIPE_MTE2到PIPE_V的依赖：

```c
asc_copy_gm2ub(ub0, gm0, size0);
asc_sync_notify(PIPE_MTE2, PIPE_V, EVENT_ID0);
asc_sync_wait(PIPE_MTE2, PIPE_V, EVENT_ID0);
// 在此调用读取ub0的PIPE_V接口。
```

事件ID由同一对源、目的流水共享，`asc_sync_notify`和`asc_sync_wait`的流水与事件参数必须完全一致。可用流水组合和事件资源请参见[核内同步](../../sync/intra_core_sync/intra_core_sync_overview.md)。

## 地址重叠

源、目的有效搬运区域是否允许重叠由具体接口决定。NPU架构版本3510的UB内部复制要求实际参与搬运的源、目的DataBlock互不重叠，即使两段地址的整体跨度可以交叠，也不能把`asc_copy_ub2ub`当作支持重叠区域的`memmove`。Reg指令访问重叠UB区域并存在读写冲突时，按对应接口要求使用[asc_mem_bar](../../reg_compute/reg_sync/asc_mem_bar.md)。

## 初始化与配置状态

- Kernel入口建议先调用[asc_init](../../utils/sys_init/asc_init.md)，清理可能由前序任务保留的控制状态。
- Padding、循环、Nz格式转换、量化、激活和矩阵三维搬运等配置接口会影响后续搬运指令。复用配置前，应确认所有相关字段均已按当前任务更新。
- 多核可能读取同一块GM且该数据可能被其他核更新时，应在NDDMA前按[asc_ndim_copy_dci](../../vector_datamove/asc_ndim_copy_dci.md)要求刷新NDDMA DataCache。

存储空间和C API通用规则请参见[通用说明和约束](../../general_description_and_constraints.md)。
