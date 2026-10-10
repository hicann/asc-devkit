# 数据搬运概念

数据搬运接口覆盖从连续复制到多维寻址、格式转换和量化激活等能力。选择接口时，先确定源、目的存储单元和执行核，再判断数据是否连续、是否对齐以及是否需要随路处理。不同接口中的同名参数可能采用不同单位，调用前应核对对应接口文档。

## 搬运功能分类总览

| 功能类别 | 主要用途 | 代表接口 |
| --- | --- | --- |
| 连续数据搬运 | 搬运一段连续数据，格式和内容保持不变 | [asc_copy_gm2ub](../../vector_datamove/asc_copy_gm2ub/asc_copy_gm2ub_arch_3510.md)、[asc_copy_ub2gm](../../vector_datamove/asc_copy_ub2gm/asc_copy_ub2gm_arch_3510.md) |
| 高维切分数据搬运 | 按固定块数、块长和步长搬运非连续数据 | [asc_copy_gm2l1](../../cube_datamove/cube_compute_load/asc_copy_gm2l1_highdim_split_arch_3510.md)、[asc_copy_ub2ub](../../vector_datamove/asc_copy_ub2ub.md) |
| 多维切片及重排 | 按1至5维循环、步长和Padding搬运数据 | [asc_ndim_copy_gm2ub](../../vector_datamove/asc_ndim_copy_gm2ub.md) |
| 随路格式转换 | 搬运时完成ND、DN、Nz等格式转换 | [asc_copy_gm2l1_nd2nz](../../cube_datamove/cube_compute_load/asc_copy_gm2l1_nd2nz_arch_3510.md)、[asc_copy_l0c2gm](../../cube_datamove/cube_compute_store/asc_copy_l0c2gm_arch_3510.md) |
| 随路量化激活 | 搬出L0C结果时完成类型转换、量化或激活 | [asc_copy_l0c2gm](../../cube_datamove/cube_compute_store/asc_copy_l0c2gm_arch_3510.md)、[asc_copy_l0c2ub](../../cube_datamove/cube_compute_store/asc_copy_l0c2ub.md) |
| 非对齐数据搬运 | 显式控制非32字节块长的片上补齐布局或Padding | [asc_copy_gm2ub_align](../../vector_datamove/asc_copy_gm2ub_align/asc_copy_gm2ub_align_arch_3510.md)、[asc_copy_gm2l1_align](../../cube_datamove/cube_compute_load/asc_copy_gm2l1_align.md) |
| UB内部复制 | 在UB内连续或按块复制数据 | [asc_copy_ub2ub](../../vector_datamove/asc_copy_ub2ub.md) |
| 矩阵分形搬运 | 在L1与L0A/L0B之间搬运二维、三维或MX矩阵 | [asc_copy_l12l0a](../../cube_datamove/cube_compute_load/asc_copy_l12l0a.md)、[asc_copy_l12l0b](../../cube_datamove/cube_compute_load/asc_copy_l12l0b.md) |
| Reg与UB数据交换 | 在Vector Function中加载或存储矢量、掩码数据 | [Reg数据搬入](../../reg_compute/load/reg_load_overview.md)、[Reg数据搬出](../../reg_compute/store/reg_store_overview.md) |

## 连续数据搬运

连续搬运从源起始地址读取一段连续数据，并按原有顺序写入目的起始地址，传输过程中不改变数据格式和内容。多数连续接口通过`size`指定总字节数；L1到L0A/L0B等矩阵搬运接口则以完整分形为最小粒度。

连续搬运适用于源、目的数据均连续存放且地址满足接口对齐要求的场景，例如通过`asc_copy_gm2ub`加载矢量输入、通过`asc_copy_ub2gm`回写矢量结果，或通过`asc_copy_l12bt`加载连续bias数据。

**图1** 连续数据搬运示意图

![连续数据搬运示意图](../../figures/continuous_data_copy_diagram.png "连续数据搬运示意图")

## 高维切分数据搬运

源或目的数据由多个等长数据块组成，且相邻数据块之间存在固定间隔时，可使用高维切分模式。此类接口通常包含以下参数：

- `burst_count`或`n_burst`：搬运的数据块个数。
- `burst_len`或`len_burst`：每个数据块的有效长度。
- `src_stride`、`dst_stride`：相邻数据块起始地址之间的距离。
- `src_gap`、`dst_gap`：前一数据块结束地址与后一数据块起始地址之间的间隔。

参数单位随通路变化。例如NPU架构版本3510的`asc_copy_gm2ub`以字节描述块长和起始地址间距，`asc_copy_ub2ub`以32字节DataBlock描述块长和块间间隔，`asc_copy_gm2l1`高维切分模式则以32字节描述块长和起始地址间距。不能仅根据参数名推断单位。

当源、目的数据块首尾相接时，搬运结果等价于连续复制，如图2所示；配置非零间隔后，可跳过源数据或在目的端预留区域，如图3所示。

**图2** 高维切分连续搬运示意图

![高维切分连续搬运示意图](../../figures/continuous_data_copy_diagram.png "高维切分连续搬运示意图")

**图3** 高维切分非连续搬运示意图

![高维切分非连续搬运示意图](../../figures/discontinuous_data_copy_diagram.png "高维切分非连续搬运示意图")

## 多维切片数据搬运

[asc_ndim_copy_gm2ub](../../vector_datamove/asc_ndim_copy_gm2ub.md)通过最多五层循环描述GM到UB的数据搬运。每一维分别配置处理元素数、源步长、目的步长以及左右Padding元素数，因此可以从多维Tensor中提取子区域，也可以实现Transpose、Broadcast和Padding。

多维切片适用于从特征图提取ROI、选取Tiling子块或跳过边界无效区域。各维源、目的步长通过[asc_set_ndim_loop_stride](../../vector_datamove/asc_set_ndim_loop_stride.md)设置，1至4维的Padding数量通过[asc_set_ndim_pad_count](../../vector_datamove/asc_set_ndim_pad_count.md)设置。

## 随路格式转换搬运

矩阵计算输入通常需要采用Nz分形排布，输出也可能需要恢复为ND或DN排布。随路格式转换可在数据搬运的同时完成排布变化，减少额外的重排操作：

- [asc_copy_gm2l1_nd2nz](../../cube_datamove/cube_compute_load/asc_copy_gm2l1_nd2nz_arch_3510.md)将GM中的ND数据搬到L1并转换为Nz；调用前通过[asc_set_gm2l1_nz_para](../../cube_datamove/cube_load_aux_config/asc_set_gm2l1_nz_para.md)配置目的Nz矩阵步长和矩阵个数。
- [asc_copy_gm2l1_dn2nz](../../cube_datamove/cube_compute_load/asc_copy_gm2l1_dn2nz.md)将GM中的DN数据搬到L1并转换为Nz。
- [asc_copy_l0c2gm](../../cube_datamove/cube_compute_store/asc_copy_l0c2gm_arch_3510.md)、[asc_copy_l0c2l1](../../cube_datamove/cube_compute_store/asc_copy_l0c2l1_arch_3510.md)和[asc_copy_l0c2ub](../../cube_datamove/cube_compute_store/asc_copy_l0c2ub.md)可在搬出L0C结果时执行Nz2ND或Nz2DN转换。

随路转换会改变目的端的数据排布和实际占用空间，目的步长和边界应按转换后的格式计算。

**图4** ND2Nz格式转换示意图

![ND2Nz格式转换示意图](../../figures/nd2nz_conversion_half.png "ND2Nz格式转换示意图")

## 随路量化激活搬运

矩阵计算结果位于L0C Buffer，源数据类型通常为`float`或`int32_t`。L0C搬出接口可在写入GM、L1或UB时组合类型转换、scalar/tensor量化、ReLU或Leaky ReLU等处理，并可与Nz2ND、Nz2DN等格式转换组合使用。

启用随路处理前，需要通过对应的配置接口设置量化参数、激活参数和格式转换参数。例如tensor量化需要配置量化参数所在地址，Nz2DN需要配置通道参数。源、目的数据类型必须与`quant_pre_mode`匹配，目的空间应按转换后的数据类型和布局预留。

## 非对齐数据搬运

搬运接口通常要求UB或L1起始地址满足32字节对齐，但有效长度约束因接口而异。`asc_copy_gm2ub`和`asc_copy_ub2gm`以字节表示有效长度，长度只需满足数据类型字节对齐，可以不是32字节的整数倍。普通接口的GM到UB仍会在UB端补齐写入，UB到GM也会补齐读取UB，容量计算不能只使用有效长度。需要显式控制非32字节数据的片上补齐布局、左右Padding、L2 Cache策略或更大的高维切分参数范围时，使用专用非对齐接口：

- GM到UB使用[asc_copy_gm2ub_align](../../vector_datamove/asc_copy_gm2ub_align/asc_copy_gm2ub_align_arch_3510.md)。高维切分原型的目的端可选择Compact或Normal模式，并支持常量或首元素填充。
- UB到GM使用[asc_copy_ub2gm_align](../../vector_datamove/asc_copy_ub2gm_align/asc_copy_ub2gm_align_arch_3510.md)。硬件读取UB时补充dummy数据，写入GM时丢弃补充部分。
- GM到L1使用[asc_copy_gm2l1_align](../../cube_datamove/cube_compute_load/asc_copy_gm2l1_align.md)，支持Compact、Normal、左右Padding和循环填充模式。

非对齐接口可能访问对齐补齐后的片上空间。即使只有少量有效数据，也必须为补齐后的范围预留足够容量，不能通过普通接口扩大搬运范围访问未分配地址。

## UB内部数据搬运

[asc_copy_ub2ub](../../vector_datamove/asc_copy_ub2ub.md)在UB内部执行连续或高维切分复制，运行在PIPE_V。连续模式通过`size`指定字节数；高维切分模式通过数据块个数、块长和源/目的间隔描述地址排布，其中块长和间隔以32字节DataBlock为单位。

源、目的实际参与搬运的DataBlock不能重叠。若需要在Vector Function中按掩码选择元素，或在Reg中完成数据处理，应使用Reg加载、计算和存储接口，而不是扩大UB复制的能力边界。

## 矩阵分形搬运

L1到L0A/L0B的接口以矩阵分形为主要搬运粒度，为Cube矩阵计算准备左、右矩阵。NPU架构版本3510支持以下模式：

- 二维分形搬运：按行列起始位置、分形步长和源/目的矩阵间距装载数据，可选择伴随转置的原型。
- 三维img2col搬运：按Feature Map、Filter、Stride和Padding配置提取卷积窗口，并支持Repeat模式。
- 二维分形转置：通过[asc_copy_l12l0b_trans](../../cube_datamove/cube_compute_load/asc_copy_l12l0b_trans_arch_3510.md)按重复次数和分形间隔完成转置。
- MX量化系数搬运：通过`asc_copy_l12l0a_mx`或`asc_copy_l12l0b_mx`将量化系数搬到对应MX Buffer，并与矩阵数据地址建立映射。

二维搬运的完整分形通常为512字节，L0A/L0B目的地址通常需要512字节对齐。具体分形形状随数据位宽和A/B矩阵方向变化。

## 多维数据搬运NDDMA

NDDMA以1至5维循环描述GM到UB的数据访问，每维均可独立配置元素数、源步长、目的步长以及左右Padding。通过步长组合可实现多维Padding、Transpose、Broadcast和Slice，比固定数据块模式更适合不规则多维排布。

各维步长的单位为元素个数。Padding支持常数填充和最近值填充；常量值通过[asc_set_ndim_pad_value](../../vector_datamove/asc_set_ndim_pad_value.md)设置。多核可能读取同一块GM且该数据可能被其他核更新时，应在搬运前调用[asc_ndim_copy_dci](../../vector_datamove/asc_ndim_copy_dci.md)刷新NDDMA DataCache。NDDMA仅覆盖GM到UB方向。

## Reg与UB数据交换

Vector Function通过加载接口把UB数据读入矢量数据寄存器、掩码寄存器或非对齐寄存器，通过存储接口将结果写回UB。对齐访问通常以32字节为基线；尾块或任意起始地址可使用非对齐加载、存储接口，并按规定复用`vector_load_unalign`或`vector_store_unalign`状态。

Reg接口的有效元素数由数据类型和掩码共同决定。多条Reg指令访问重叠UB区域并存在读写冲突时，应按接口约束使用[asc_mem_bar](../../reg_compute/reg_sync/asc_mem_bar.md)建立顺序。
