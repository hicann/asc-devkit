# 矩阵搬入总体说明

矩阵计算的搬入是Ascend C编程框架中用于数据搬运的一类核心接口，主要实现Global Memory到L0A Buffer/L0B Buffer之间的数据高效传输。该类接口提供了多种数据搬运模式，能够满足矩阵计算中左右矩阵等数据的搬运需求，通常与Mmad接口配合使用。

矩阵计算的搬入的接口支持多种数据搬运场景，包括但不限于**二维格式数据搬运、三维格式转换、压缩数据解压、矩阵转置搬运以及稀疏化数据处理**等。通过灵活配置不同的参数结构体，开发者可以精确控制数据的搬运路径、起始位置、搬运长度等关键信息，充分利用硬件的数据搬运能力，最大化算子的整体执行性能。

矩阵计算的搬入接口支持多种数据通路，每种通路对应特定的存储层级和访问特性。

**表1** **数据通路和存储层级**

| 源位置 | 源地址对齐要求 | 目的位置 | 目的地址对齐要求 | 典型应用场景 |
| --- | --- | --- | --- | --- |
| Global Memory | 1字节 | L1 Buffer（`__cbuf__`，A矩阵） | 32字节 | 从全局内存加载左矩阵数据到L1 Buffer。 |
| Global Memory | 1字节 | L1 Buffer（`__cbuf__`，B矩阵）| 32字节 | 从全局内存加载右矩阵数据到L1 Buffer。 |
| Global Memory | 1字节 | L0A Buffer（`__ca__`） | 512字节 | 从全局内存加载左矩阵数据到L0A Buffer。 |
| Global Memory | 1字节 | L0B Buffer（`__cb__`） | 512字节 | 从全局内存加载右矩阵数据到L0B Buffer。 |
| L1 Buffer（`__cbuf__`，A矩阵） | 32字节 | L0A Buffer（`__ca__`） | 512字节 | 从L1 Buffer加载左矩阵数据到L0A Buffer。 |
| L1 Buffer（`__cbuf__`，B矩阵） | 32字节 | L0B Buffer（`__cb__`） | 512字节 | 从L1 Buffer加载右矩阵数据到L0B Buffer。 |
| L1 Buffer | 32字节 | BiasTable Buffer | 64字节 | 从L1 Buffer加载存储矩阵计算所需的Bias（偏置）数据到BiasTable Buffer。 |
| L1 Buffer | 32字节 | Fixpipe Buffer | 128字节 | 从L1 Buffer加载Fixpipe搬运过程中所需的量化参数等数据到Fixpipe Buffer。 |

<!-- npu="950" id1 -->
> [!NOTE]说明
> Ascend 950PR&950DT系列产品因硬件变更，删除GM到L0A Buffer、L0B Buffer的数据通路。产生的影响是原GM到L0A Buffer和L0B Buffer的数据搬运需要拆分为两步，即从GM到L1 Buffer的数据搬运和从L1 Buffer到L0A Buffer、L0B Buffer的数据搬运。
<!-- end id1 -->

## GM->L1 Buffer的接口分类与使用场景

**表2** GM->L1 Buffer接口

| 接口类别 | 主要功能 | 支持通路 | 典型应用场景 |
| --- | --- | --- | --- |
| [asc_copy_gm2l1](asc_copy_gm2l1/asc_copy_gm2l1.md)，连续/高维切分重载 | 拷贝已排布数据，保持内容和格式 | GM-\>L1 Buffer | 完整矩阵、非连续矩阵分片 |
| [asc_copy_gm2l1_nd2nz](asc_copy_gm2l1_nd2nz/asc_copy_gm2l1_nd2nz.md) | 搬运并将ND转为Nz | GM-\>L1 Buffer | 线性矩阵输入 |
| [asc_copy_gm2l1](asc_copy_gm2l1/asc_copy_gm2l1.md)，2D分形重载 | 按分形搬运 | GM-\>L1 Buffer | 离线分形数据 |

<!-- npu="950" id2 -->
针对Ascend 950PR&950DT系列产品，新增GM->L1 Buffer如下接口，请开发者参考表3。

**表3** GM->L1 Buffer接口

| 接口类别 | 主要功能 | 支持通路 | 典型应用场景 |
| --- | --- | --- | --- |
| [asc_copy_gm2l1_dn2nz](asc_copy_gm2l1_dn2nz.md) | DN-\>Nz | GM-\>L1 Buffer | 列主序矩阵输入 |
| [asc_copy_gm2l1_align](asc_copy_gm2l1_align.md) | 非对齐搬运及左右Padding | GM-\>L1 Buffer | 字节长度、边界不满足普通块拷贝要求 |
| [asc_copy_gm2l1（2D）](asc_copy_gm2l1/asc_copy_gm2l1_2d_arch_3510.md) | 2DV2分形搬运，可配L2 Cache策略 | GM-\>L1 Buffer | 多方向分形块输入 |
| [asc_copy_gm2l1（高维切分）](asc_copy_gm2l1/asc_copy_gm2l1_highdim_split_arch_3510.md) | 支持`asc_channel_pad_mode`通道填充/压缩 | GM-\>L1 Buffer | 按32B通道扩展有效区或去除高位字节 |
<!-- end id2 -->

<!-- npu="950" id5 -->
## UB->L1 Buffer的接口分类与使用场景

**表4** UB->L1 Buffer接口

| 接口类别 | 主要功能 | 支持通路 | 典型应用场景 |
| --- | --- | --- | --- |
| [asc_copy_ub2l1](../vector_datamove/asc_copy_ub2l1.md)连续重载 | 连续数据搬运 | UB-\>L1 Buffer | 矢量结果送入矩阵计算 |
| 同接口高维切分重载 | 按32B块配置长度和间隔 | UB-\>L1 Buffer | 非连续矢量结果搬运 |
<!-- end id5 -->

## L1 Buffer->L0A Buffer/L0B Buffer的接口分类与使用场景

**表5** L1 Buffer->L0A/B Buffer接口

| 接口类别 | 主要功能 | 支持通路 | 典型应用场景 |
| --- | --- | --- | --- |
| [asc_copy_l12l0a](asc_copy_l12l0a/asc_copy_l12l0a.md)、[asc_copy_l12l0b](asc_copy_l12l0b/asc_copy_l12l0b.md)的2D重载 | 按512B分形搬运 | L1 Buffer-\>L0A Buffer/L0B Buffer | 普通矩阵输入 |
| 同接口的3D重载 | NC1HWC0输入在线img2col | L1 Buffer-\>L0A Buffer/L0B Buffer | 卷积展开 |
| [asc_copy_l12l0a_trans](asc_copy_l12l0a_trans.md)、[asc_copy_l12l0b_trans](asc_copy_l12l0b_trans/asc_copy_l12l0b_trans.md) | 拼接分形后转置搬运 | L1 Buffer-\>L0A Buffer/L0B Buffer | 特定位宽的块转置 |
| [asc_copy_l12bt](asc_copy_l12bt/asc_copy_l12bt.md) | Bias搬入 | L1 Buffer-\>BiasTable | 累加初值 |
| [asc_copy_l12fb](asc_copy_l12fb/asc_copy_l12fb.md) | 随路系数搬入 | L1 Buffer-\>Fixpipe Buffer | 量化/激活 |

<!-- npu="910b,A3" id8 -->
针对Atlas A2训练系列产品/Atlas A2推理系列产品和Atlas A3训练系列产品/Atlas A3推理系列产品，支持L1 Buffer->L0B Buffer如下接口，请开发者参考表6。

**表6** 稀疏矩阵搬运

| 接口类别 | 主要功能 | 支持通路 | 典型应用场景 |
| --- | --- | --- | --- |
| [asc_copy_l12l0b_sparse](asc_copy_l12l0b_sparse.md) | 搬运4选2结构化稀疏矩阵计算所需的稠密化权重矩阵和索引矩阵。 | L1 Buffer->L0B Buffer | 稀疏矩阵乘法计算数据搬入。 |
<!-- end id8 -->

<!-- npu="950" id9 -->
针对Ascend 950PR&950DT系列产品，新增如下接口，请开发者参考表7。

**表7** 3510二维和MX扩展

| 接口 | 功能 | 使用方式 |
| --- | --- | --- |
| `asc_copy_l12l0a` / `asc_copy_l12l0b`的2D重载 | 2D二维分形搬运，支持FP4/FP8/HiFloat8及整数位宽 | 设置`m_step`、`k_step`和源/目标步长 |
| `asc_copy_l12l0a_transpose` / `asc_copy_l12l0b_transpose` | 同一2DV2指令的转置变体 | 与普通2D重载参数布局对应；不是`_trans`指令 |
| [asc_copy_l12l0a_mx](asc_copy_l12l0a_mx.md) / [asc_copy_l12l0b_mx](asc_copy_l12l0b_mx.md) | 只搬运32B分形的E8M0系数 | 与A/B数据搬运分别调用，匹配映射地址 |
<!-- end id9 -->

## 辅助配置接口说明

**表8** 辅助配置接口

| 接口 | 功能 | 架构/注意事项 |
| --- | --- | --- |
| [asc_fill_l1](asc_fill_l1.md)、[asc_fill_l0a](asc_fill_l0a.md)、[asc_fill_l0b](asc_fill_l0b.md) | 初始化Buffer | 支持L1 Buffer、L0A Buffer、L0B Buffer |
| [asc_set_l13d_fmatrix](asc_set_l13d_fmatrix.md)、[asc_set_l13d_fmatrix_b](asc_set_l13d_fmatrix_b.md) | FeatureMap尺寸和Padding边界 | 根据A/B侧与`f_matrix_ctrl`选择 |
| [asc_set_l12l0a_3d_padding](asc_set_l12l0a_3d_padding.md)、[asc_set_l12l0b_3d_padding](asc_set_l12l0b_3d_padding.md) | Padding配置 | 根据A/B侧与`f_matrix_ctrl`选择 |
| [asc_set_l13d_size](asc_set_l13d_size.md) | L1 Buffer边界大小 | 按对应3D接口要求 |
| [asc_set_l13d_rpt](asc_set_l13d_rpt.md) | 3D重复和步长 | 按对应重载配置 |
| [asc_load_image_to_cbuf](asc_load_image_to_cbuf.md) | 图像搬入L1 Buffer | 按该接口支持范围配置 |
| [asc_copy_l12l0c](asc_copy_l12l0c.md) | L1 Buffer-\>L0C Buffer初值搬运 | 初始化路径的补充 |
| [asc_set_l12l0a_3d_padding](asc_set_l12l0a_3d_padding.md)、[asc_set_l12l0b_3d_padding](asc_set_l12l0b_3d_padding.md) | 带类型的Padding值配置 | 注意两侧寄存器及共享配置 |
