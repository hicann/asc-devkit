# 数据通路

## AI Core硬件架构与存储单元

AI Core采用分层存储架构。计算单元包括Cube、Vector和Scalar；存储单元包括Global Memory（GM）、L1 Buffer、L0A/L0B/L0C Buffer、Unified Buffer（UB）、BiasTable Buffer、Fixpipe Buffer和SIMD Register File；MTE1、MTE2、MTE3和Fixpipe负责在不同存储单元之间传输数据。

图1给出[NPU架构版本2201](../../../../../guide/programming_guide/language_extension/simd_builtin_keywords.md#npu-arch)的存储和搬运通路，便于理解AI Core各存储层级的基本关系。

**图1** NPU架构版本2201下的AI Core硬件架构

![NPU架构版本2201下的AI Core硬件架构](../../figures/atlas_a2_a3_architecture.png "NPU架构版本2201下的AI Core硬件架构")

[NPU架构版本3510](../../../../../guide/programming_guide/language_extension/simd_builtin_keywords.md#npu-arch)的内部结构如图2所示。与图1相比，数据通路主要有以下变化：

- 新增L0C Buffer到UB的单向数据通路。
- 新增UB到L1 Buffer的数据通路。
- 不再提供GM到L0A/L0B的直达通路，矩阵输入需要先搬到L1 Buffer。
- 不再提供L1 Buffer到GM的直达通路。
- AIV增加SIMD Register File，Vector Function通过Reg加载、存储接口与UB交换数据。

**图2** NPU架构版本3510下的AI Core硬件架构与同步关系

![NPU架构版本3510下的AI Core硬件架构与同步关系](../../figures/npu_3510_hw_arch_sync.png "NPU架构版本3510下的AI Core硬件架构与同步关系")

本导览后续接口选择均以Ascend 950PR&950DT系列产品（NPU架构版本3510）为准。其他产品的数据通路和接口支持情况应以对应接口文档的产品支持表为准。

### 存储单元用途

| 存储单元 | C API地址空间或对象 | 主要用途 |
| --- | --- | --- |
| GM | `__gm__` | 保存Kernel输入、输出以及跨核共享数据。 |
| L1 Buffer | `__cbuf__` | 缓存Cube输入，也是GM与L0A/L0B之间的中转存储。 |
| L0A Buffer | `__ca__` | 保存矩阵计算的左矩阵输入。 |
| L0B Buffer | `__cb__` | 保存矩阵计算的右矩阵输入。 |
| L0C Buffer | `__cc__` | 保存矩阵计算累加结果。 |
| UB | `__ubuf__` | 保存Vector输入、输出和中间结果，并作为AIC与AIV交换数据的片上存储。 |
| BiasTable Buffer | 以`uint64_t`目的地址传入 | 保存Mmad使用的bias。 |
| Fixpipe Buffer | `__fbuf__` | 保存L0C搬出使用的随路Vector量化和随路ReLU参数。 |
| SIMD Register File | `vector_*`、`vector_bool`等[寄存器类型](../../defs/type/reg_data_types.md) | 保存Vector Function使用的矢量数据、掩码和地址状态。 |

## 数据通路与搬运流水

下表按源、目的存储单元列出NPU架构版本3510可用的数据搬运功能。每行对应一种可直接选择的C接口形态；同一接口具有多个重载时，按功能分别列出。

**表1** NPU架构版本3510数据搬运功能总览

| 源（SRC） | 目的（DST） | 流水 | 功能 | C API |
| --- | --- | --- | --- | --- |
| GM | L1 Buffer | PIPE_MTE2 | 连续搬运；配置一个数据块，源、目的步长设为0 | [asc_copy_gm2l1](../../cube_datamove/asc_copy_gm2l1/asc_copy_gm2l1_highdim_split_arch_3510.md) |
| GM | L1 Buffer | PIPE_MTE2 | 高维切分搬运；通过数据块个数、块长和源/目的步长描述排布 | [asc_copy_gm2l1](../../cube_datamove/asc_copy_gm2l1/asc_copy_gm2l1_highdim_split_arch_3510.md) |
| GM | L1 Buffer | PIPE_MTE2 | 以512字节分形为单位执行二维矩阵搬运 | [asc_copy_gm2l1](../../cube_datamove/asc_copy_gm2l1/asc_copy_gm2l1_2d_arch_3510.md) |
| GM | L1 Buffer | PIPE_MTE2 | 搬运时将ND数据转换为Nz排布 | [asc_copy_gm2l1_nd2nz](../../cube_datamove/asc_copy_gm2l1_nd2nz/asc_copy_gm2l1_nd2nz_arch_3510.md) |
| GM | L1 Buffer | PIPE_MTE2 | 搬运时将DN数据转换为Nz排布 | [asc_copy_gm2l1_dn2nz](../../cube_datamove/asc_copy_gm2l1_dn2nz.md) |
| GM | L1 Buffer | PIPE_MTE2 | 非对齐搬运，支持Compact、Normal、左右Padding和循环填充 | [asc_copy_gm2l1_align](../../cube_datamove/asc_copy_gm2l1_align.md) |
| L0C Buffer | GM | PIPE_FIX | 搬出矩阵结果，可组合量化、激活、Nz2ND/Nz2DN和通道处理 | [asc_copy_l0c2gm](../../cube_datamove/asc_copy_l0c2gm/asc_copy_l0c2gm_arch_3510.md) |
| GM | UB | PIPE_MTE2 | 连续搬运 | [asc_copy_gm2ub](../../vector_datamove/asc_copy_gm2ub/asc_copy_gm2ub_arch_3510.md) |
| GM | UB | PIPE_MTE2 | 高维切分搬运 | [asc_copy_gm2ub](../../vector_datamove/asc_copy_gm2ub/asc_copy_gm2ub_arch_3510.md) |
| GM | UB | PIPE_MTE2 | 非对齐连续搬运，目的端补齐到32字节边界 | [asc_copy_gm2ub_align](../../vector_datamove/asc_copy_gm2ub_align/asc_copy_gm2ub_align_arch_3510.md) |
| GM | UB | PIPE_MTE2 | 非对齐高维切分搬运，支持Compact、Normal和左右Padding | [asc_copy_gm2ub_align](../../vector_datamove/asc_copy_gm2ub_align/asc_copy_gm2ub_align_arch_3510.md) |
| GM | UB | PIPE_MTE2 | 最多五维的Padding或多维数据搬运 | [asc_ndim_copy_gm2ub](../../vector_datamove/asc_ndim_copy_gm2ub.md) |
| GM | UB | PIPE_MTE2 | 通过多维步长组合实现Transpose、Broadcast或Slice | [asc_ndim_copy_gm2ub](../../vector_datamove/asc_ndim_copy_gm2ub.md) |
| UB | GM | PIPE_MTE3 | 连续搬运 | [asc_copy_ub2gm](../../vector_datamove/asc_copy_ub2gm/asc_copy_ub2gm_arch_3510.md) |
| UB | GM | PIPE_MTE3 | 高维切分搬运 | [asc_copy_ub2gm](../../vector_datamove/asc_copy_ub2gm/asc_copy_ub2gm_arch_3510.md) |
| UB | GM | PIPE_MTE3 | 非对齐连续搬运，写入GM时丢弃补齐数据 | [asc_copy_ub2gm_align](../../vector_datamove/asc_copy_ub2gm_align/asc_copy_ub2gm_align_arch_3510.md) |
| UB | GM | PIPE_MTE3 | 非对齐高维切分搬运，支持Compact和Normal源排布 | [asc_copy_ub2gm_align](../../vector_datamove/asc_copy_ub2gm_align/asc_copy_ub2gm_align_arch_3510.md) |
| UB | L1 Buffer | PIPE_MTE3 | 连续搬运 | [asc_copy_ub2l1](../../vector_datamove/asc_copy_ub2l1.md) |
| UB | L1 Buffer | PIPE_MTE3 | 高维切分搬运 | [asc_copy_ub2l1](../../vector_datamove/asc_copy_ub2l1.md) |
| L1 Buffer | UB | PIPE_MTE1 | 连续搬运，可指定目的AIV | [asc_copy_l12ub](../../cube_datamove/asc_copy_l12ub.md) |
| L1 Buffer | UB | PIPE_MTE1 | 高维切分搬运，可指定目的AIV | [asc_copy_l12ub](../../cube_datamove/asc_copy_l12ub.md) |
| L1 Buffer | L0A Buffer | PIPE_MTE1 | 二维分形矩阵搬运 | [asc_copy_l12l0a](../../cube_datamove/asc_copy_l12l0a/asc_copy_l12l0a_2d_arch_3510.md) |
| L1 Buffer | L0A Buffer | PIPE_MTE1 | 二维分形矩阵伴转置搬运 | [asc_copy_l12l0a_transpose](../../cube_datamove/asc_copy_l12l0a/asc_copy_l12l0a_2d_arch_3510.md) |
| L1 Buffer | L0A Buffer | PIPE_MTE1 | 三维img2col搬运，支持Feature Map、Filter、Padding和Repeat配置 | [asc_copy_l12l0a](../../cube_datamove/asc_copy_l12l0a/asc_copy_l12l0a_3d_arch_3510.md) |
| L1 Buffer | L0A_MX Buffer | PIPE_MTE1 | 搬运MX矩阵计算使用的左矩阵量化系数 | [asc_copy_l12l0a_mx](../../cube_datamove/asc_copy_l12l0a_mx.md) |
| L1 Buffer | L0B Buffer | PIPE_MTE1 | 二维分形矩阵搬运 | [asc_copy_l12l0b](../../cube_datamove/asc_copy_l12l0b/asc_copy_l12l0b_2d_arch_3510.md) |
| L1 Buffer | L0B Buffer | PIPE_MTE1 | 二维分形矩阵伴转置搬运 | [asc_copy_l12l0b_transpose](../../cube_datamove/asc_copy_l12l0b/asc_copy_l12l0b_2d_arch_3510.md) |
| L1 Buffer | L0B Buffer | PIPE_MTE1 | 三维img2col搬运，搬运时自动转置 | [asc_copy_l12l0b](../../cube_datamove/asc_copy_l12l0b/asc_copy_l12l0b_3d_arch_3510.md) |
| L1 Buffer | L0B Buffer | PIPE_MTE1 | 按Repeat和分形间隔执行二维分形转置 | [asc_copy_l12l0b_trans](../../cube_datamove/asc_copy_l12l0b_trans/asc_copy_l12l0b_trans_arch_3510.md) |
| L1 Buffer | L0B_MX Buffer | PIPE_MTE1 | 搬运MX矩阵计算使用的右矩阵量化系数 | [asc_copy_l12l0b_mx](../../cube_datamove/asc_copy_l12l0b_mx.md) |
| L0C Buffer | L1 Buffer | PIPE_FIX | 搬出矩阵结果，可组合量化、激活和格式转换 | [asc_copy_l0c2l1](../../cube_datamove/asc_copy_l0c2l1/asc_copy_l0c2l1_arch_3510.md) |
| L0C Buffer | UB | PIPE_FIX | 搬出矩阵结果，可组合量化、激活、格式转换和双目标搬运 | [asc_copy_l0c2ub](../../cube_datamove/asc_copy_l0c2ub.md) |
| L1 Buffer | BiasTable Buffer | PIPE_MTE1 | 连续bias搬运，可执行`half`/`bfloat16_t`到32位数据的处理 | [asc_copy_l12bt](../../cube_datamove/asc_copy_l12bt/asc_copy_l12bt_arch_3510.md) |
| L1 Buffer | BiasTable Buffer | PIPE_MTE1 | 高维切分bias搬运 | [asc_copy_l12bt](../../cube_datamove/asc_copy_l12bt/asc_copy_l12bt_arch_3510.md) |
| L1 Buffer | Fixpipe Buffer | PIPE_FIX | 连续搬运量化或ReLU参数 | [asc_copy_l12fb](../../cube_datamove/asc_copy_l12fb/asc_copy_l12fb_arch_3510.md) |
| L1 Buffer | Fixpipe Buffer | PIPE_FIX | 高维切分搬运量化或ReLU参数 | [asc_copy_l12fb](../../cube_datamove/asc_copy_l12fb/asc_copy_l12fb_arch_3510.md) |
| UB | UB | PIPE_V | 连续复制 | [asc_copy_ub2ub](../../vector_datamove/asc_copy_ub2ub.md) |
| UB | UB | PIPE_V | 高维切分复制 | [asc_copy_ub2ub](../../vector_datamove/asc_copy_ub2ub.md) |
| UB | SIMD Register File | Vector Function | 对齐、非对齐、广播或掩码加载 | [Reg数据搬入](../../reg_compute/load/reg_load_overview.md) |
| SIMD Register File | UB | Vector Function | 对齐、非对齐或掩码存储 | [Reg数据搬出](../../reg_compute/store/reg_store_overview.md) |

NPU架构版本3510不支持GM到L0A/L0B直达搬运，数据需要经过GM到L1 Buffer、L1 Buffer到L0两段通路；也不支持L1 Buffer到GM或UB到L0C的通用直达搬运。接口名称在多个NPU架构间相同时，也不能据此推断NPU架构版本3510支持该接口，应以接口文档中的产品支持情况为准。

各通路的模式、参数单位和关键约束请继续阅读[数据搬运导览](../data_move_guide.md)中的对应章节。硬件存储结构的完整说明请参见[NPU架构版本3510硬件规格](../../../../../guide/programming_guide/advanced_programming/hardware_implementation/architecture_spec/npu_arch_3510.md)。
