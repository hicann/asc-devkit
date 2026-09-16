# L0C Buffer到UB双目标模式

## 特性说明

本特性仅适用于3510。同一AI Core内有一个Cube Core和两个Vector Core，当启用双目标模式控制时，L0C Buffer中的M×N矩阵将被分成两半，并同时分别写入两个Vector Core各自的Unified Buffer（UB）中，其中前半部分写入SUB BLOCK0，后半部分写入SUB BLOCK1。

- 双目标模式（M维度）：按M维度拆分成形状为M / 2 \* N的两个矩阵，分别写入两个UB。
- 双目标模式（N维度）：按N维度拆分成形状为M \* N / 2的两个矩阵，分别写入两个UB。

## 特性约束

- 双目标模式仅支持在L0C Buffer到UB通路，普通搬运模式（Nz2Nz）或Nz2ND搬运场景下使用，不支持随路功能场景。
- 按M维度拆分时M必须为2的倍数。
- 按N维度拆分时N须为32的倍数。

## 使用示例

在普通搬运模式下启用双目标模式如下图所示，分为按M维度拆分和按N维度拆分，按M维度拆分M必须为2的倍数，按N维度拆分N必须为32的倍数：

N方向切分：

- n_size = 32，表示源Nz矩阵中待搬运矩阵在N方向上的大小为32个元素。
- m_size = 48，表示源Nz矩阵中待搬运矩阵在M方向上的大小48个元素。
- src_stride = 64，表示源Nz矩阵中待搬运矩阵相邻Z排布的起始地址偏移，即下图中第一个块Z排布矩阵的起始地址与第二个Z排布矩阵的起始地址之间的间隔为64 \* C0\_Size。
- dst_stride = 64 \* C0，表示目的Nz矩阵中相邻Z排布的起始地址偏移，该步长分别应用于每个UB内部，不是两个UB地址空间之间的距离。

M方向切分：

- n_size = 32，表示源Nz矩阵中待搬运矩阵在N方向上的大小为32个元素。
- m_size = 48，表示完整源矩阵M为48；拆分后每个UB收到24行，不能把拆分后行数作为源m_size。
- src_stride = 64，表示源Nz矩阵中待搬运矩阵相邻Z排布的起始地址偏移，即下图中第一个块Z排布矩阵的起始地址与第二个Z排布矩阵的起始地址之间的间隔为64 \* C0\_Size。
- dst_stride = 40 \* C0，表示目的Nz矩阵中相邻Z排布的起始地址偏移，即下图中UB0（或UB1）中第一个Z排布的起始地址与第二个Z排布的起始地址之间的间隔为40 \* 16个元素。

**图1** Nz2Nz双目标搬运模式设置示意图

![](../../../../figures/nz2nz_l0c2ub_dual_dst_c_api.png)

在Nz2ND模式下启用双目标模式如下图所示，分为按M维度拆分和按N维度拆分，按M维度拆分M必须为2的倍数，按N维度拆分N必须为32的倍数：

- nd_num = 2，表示源Nz矩阵的数目为2。图中红框区域为矩阵1，蓝框区域为矩阵2。
- n_size = 32，表示源Nz矩阵（图中红框区域或蓝框区域）在N方向上的大小为32个元素。
- m_size = 48，表示源Nz矩阵在M方向上的大小为48个元素。
- src_stride = 64，表示源Nz矩阵中相邻Z排布的起始地址偏移，即下图红框区域中左侧浅色Z排布矩阵的起始地址与右侧深色Z排布矩阵的起始地址之间的间隔为64 \* C0\_Size。
- dst_stride = 64，表示目的ND矩阵每一行中的元素个数为64。
- src_nd_stride = 240 ,  表示不同Nz矩阵起始地址之间的间隔为240 \* C0\_Size。
- dst_nd_stride  = 4096，表示目的相邻ND矩阵起始地址之间的偏移为4096个元素。

**图2** Nz2ND双目标搬出模式设置示意图

![](../../../../figures/nz2nd_l0c2ub_dual_dst_c_api.png)

图中`nd_num`、`src_nd_stride`、`dst_nd_stride`分别对应[asc_set_l0c_copy_nz_para](../asc_set_l0c_copy_nz_para.md)的`matrix_num`、`src_nz_matrix_stride`、`dst_matrix_stride`，本例取2、240、4096。`asc_copy_l0c2ub`通过`asc_dual_dst_mode::DUAL_DST_SPLIT_M`或`DUAL_DST_SPLIT_N`选择切分方向。Nz输出示例中的C0为16，`dst_stride`按目标元素计数。
