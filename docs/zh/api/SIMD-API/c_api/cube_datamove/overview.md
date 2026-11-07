# 概述

## API列表

### 矩阵计算的搬入

**表1** 矩阵计算的搬入接口

| 接口名称 | 接口描述 | 最细粒度公共头文件 |
| --- | --- | --- |
| [asc_copy_gm2l1](cube_compute_load/asc_copy_gm2l1.md) | 将GM中的数据搬运到L1中。 | `c_api/cube_datamove/cube_datamove.h` |
| [asc_copy_gm2l1（NPU架构版本2201）](cube_compute_load/asc_copy_gm2l1_arch_2201.md) | 将矩阵数据从Global Memory搬运到L1 Buffer中，并支持不同类型的pad模式。 | `c_api/cube_datamove/cube_datamove.h` |
| [asc_copy_gm2l1（NPU架构版本3510，高维切分数据搬运）](cube_compute_load/asc_copy_gm2l1_highdim_split_arch_3510.md) | 将GM数据按高维切分方式搬运到L1 Buffer，支持按通道插入或移除Padding。 | `c_api/cube_datamove/cube_datamove.h` |
| [asc_copy_gm2l1（NPU架构版本3510，2D矩阵搬运模式）](cube_compute_load/asc_copy_gm2l1_2d_arch_3510.md) | 以512字节分形为单位，将GM中的2D矩阵数据切分搬运到L1 Buffer，搬运过程中不进行分形格式转换。 | `c_api/cube_datamove/cube_datamove.h` |
| [asc_copy_gm2l1_align](cube_compute_load/asc_copy_gm2l1_align.md) | 将GM中的数据padding后搬运到L1中。 | `c_api/cube_datamove/cube_datamove.h` |
| [asc_copy_gm2l1_dn2nz](cube_compute_load/asc_copy_gm2l1_dn2nz.md) | 将GM中的数据搬运到L1中，在此过程中执行DN->NZ/NCHW->NC1HWC0/NCHW->C1HWNC0操作。 | `c_api/cube_datamove/cube_datamove.h` |
| [asc_copy_gm2l1_nd2nz](cube_compute_load/asc_copy_gm2l1_nd2nz.md) | 将GM数据搬运到L1 Buffer，并将ND格式转换为Nz分形格式。 | `c_api/cube_datamove/cube_datamove.h` |
| [asc_copy_gm2l1_nd2nz（NPU架构版本2201）](cube_compute_load/asc_copy_gm2l1_nd2nz_arch_2201.md) | 将GM数据搬运到L1 Buffer，支持ND格式到NZ格式的转换。 | `c_api/cube_datamove/cube_datamove.h` |
| [asc_copy_gm2l1_nd2nz（NPU架构版本3510）](cube_compute_load/asc_copy_gm2l1_nd2nz_arch_3510.md) | 将GM中的ND格式数据搬运到L1 Buffer，并转换为Nz分形排布。 | `c_api/cube_datamove/cube_datamove.h` |
| [asc_copy_gm2l0a](cube_compute_load/asc_copy_gm2l0a.md) | 将GM中的数据搬运到L0A中。 | `c_api/cube_datamove/cube_datamove.h` |
| [asc_copy_gm2l0b](cube_compute_load/asc_copy_gm2l0b.md) | 将GM中的数据搬运到L0B中。 | `c_api/cube_datamove/cube_datamove.h` |
| [asc_copy_l12l0a](cube_compute_load/asc_copy_l12l0a.md) | 用于搬运存放在L1 Buffer里的512B大小的矩阵到L0A Buffer里。 | `c_api/cube_datamove/cube_datamove.h` |
| [asc_copy_l12l0a（NPU架构版本2201）](cube_compute_load/asc_copy_l12l0a_arch_2201.md) | 用于搬运存放在L1 Buffer里的512B大小的矩阵到L0A Buffer里。 | `c_api/cube_datamove/cube_datamove.h` |
| [asc_copy_l12l0a（NPU架构版本3510，2D矩阵搬运）](cube_compute_load/asc_copy_l12l0a_2d_arch_3510.md) | 以512字节分形为单位，将L1 Buffer中的2D矩阵搬运到L0A Buffer，支持非转置和转置搬运。 | `c_api/cube_datamove/cube_datamove.h` |
| [asc_copy_l12l0a（NPU架构版本3510，3D矩阵搬运）](cube_compute_load/asc_copy_l12l0a_3d_arch_3510.md) | 将L1 Buffer中的Feature Map执行img2col展开后搬运到L0A Buffer，也支持2D矩阵数据搬运。 | `c_api/cube_datamove/cube_datamove.h` |
| [asc_copy_l12l0b](cube_compute_load/asc_copy_l12l0b.md) | 用于搬运存放在L1 Buffer里的512B大小的矩阵到L0B Buffer里。 | `c_api/cube_datamove/cube_datamove.h` |
| [asc_copy_l12l0b（NPU架构版本2201）](cube_compute_load/asc_copy_l12l0b_arch_2201.md) | 用于搬运存放在L1 Buffer里的512B大小的矩阵到L0B Buffer里。 | `c_api/cube_datamove/cube_datamove.h` |
| [asc_copy_l12l0b（NPU架构版本3510，2D矩阵搬运）](cube_compute_load/asc_copy_l12l0b_2d_arch_3510.md) | 以512字节分形为单位，将L1 Buffer中的2D矩阵搬运到L0B Buffer，支持非转置和转置搬运。 | `c_api/cube_datamove/cube_datamove.h` |
| [asc_copy_l12l0b（NPU架构版本3510，3D矩阵搬运）](cube_compute_load/asc_copy_l12l0b_3d_arch_3510.md) | 将L1 Buffer中的Feature Map执行img2col展开后搬运到L0B Buffer，也支持2D矩阵数据搬运。 | `c_api/cube_datamove/cube_datamove.h` |
| [asc_copy_l12l0a_mx](cube_compute_load/asc_copy_l12l0a_mx.md) | 将Mx scale矩阵从L1 Buffer搬运到L0A Buffer。 | `c_api/cube_datamove/cube_datamove.h` |
| [asc_copy_l12l0b_mx](cube_compute_load/asc_copy_l12l0b_mx.md) | 将Mx scale矩阵从L1 Buffer搬运到L0B Buffer。 | `c_api/cube_datamove/cube_datamove.h` |
| [asc_copy_l12l0a_trans](cube_compute_load/asc_copy_l12l0a_trans.md) | 该接口实现带转置的2D格式数据从L1 Buffer到L0A Buffer的加载。 | `c_api/cube_datamove/cube_datamove.h` |
| [asc_copy_l12l0b_trans](cube_compute_load/asc_copy_l12l0b_trans.md) | 该接口实现带转置的2D格式数据从L1 Buffer到L0B Buffer的加载。 | `c_api/cube_datamove/cube_datamove.h` |
| [asc_copy_l12l0b_trans（NPU架构版本2201）](cube_compute_load/asc_copy_l12l0b_trans_arch_2201.md) | 该接口实现带转置的2D格式数据从L1 Buffer到L0B Buffer的加载。 | `c_api/cube_datamove/cube_datamove.h` |
| [asc_copy_l12l0b_trans（NPU架构版本3510）](cube_compute_load/asc_copy_l12l0b_trans_arch_3510.md) | 将数据从L1 Buffer搬运到L0B Buffer，搬运过程中以大小为512字节的数据分形为单位进行二维分形矩阵转置。 | `c_api/cube_datamove/cube_datamove.h` |
| [asc_copy_l12l0b_sparse](cube_compute_load/asc_copy_l12l0b_sparse.md) | 用于搬运存放在L1 Buffer里的512B大小的稠密权重矩阵到L0B Buffer里，同时读取128B大小的索引矩阵用于稠密矩阵的稀疏化。 | `c_api/cube_datamove/cube_datamove.h` |
| [asc_copy_l12l0c](cube_compute_load/asc_copy_l12l0c.md) | 将矩阵由L1 Buffer搬运到L0C Buffer中。 | `c_api/cube_datamove/cube_datamove.h` |
| [asc_copy_l12bt](cube_compute_load/asc_copy_l12bt.md) | 将MMAD指令的Bias数据从L1 Buffer搬运到BiasTable Buffer中。 | `c_api/cube_datamove/cube_datamove.h` |
| [asc_copy_l12bt（NPU架构版本2201）](cube_compute_load/asc_copy_l12bt_arch_2201.md) | 将数据从L1 Buffer搬运到BiasTable Buffer中，BiasTable Buffer用于存放矩阵计算中的Bias。 | `c_api/cube_datamove/cube_datamove.h` |
| [asc_copy_l12bt（NPU架构版本3510）](cube_compute_load/asc_copy_l12bt_arch_3510.md) | 将L1 Buffer中的偏置数据搬运到BiasTable Buffer，支持连续和高维切分搬运，并按数据类型和配置进行转换或位宽扩展。 | `c_api/cube_datamove/cube_datamove.h` |
| [asc_copy_l12fb](cube_compute_load/asc_copy_l12fb.md) | 将数据从L1 Buffer搬运到Fixpipe Buffer中，Fixpipe Buffer用于存放量化参数。 | `c_api/cube_datamove/cube_datamove.h` |
| [asc_copy_l12fb（NPU架构版本2201）](cube_compute_load/asc_copy_l12fb_arch_2201.md) | 将数据从L1 Buffer搬运到Fixpipe Buffer中，Fixpipe Buffer用于存放量化参数。 | `c_api/cube_datamove/cube_datamove.h` |
| [asc_copy_l12fb（NPU架构版本3510）](cube_compute_load/asc_copy_l12fb_arch_3510.md) | 将L1 Buffer中的量化参数和ReLU参数搬运到Fixpipe Buffer，支持连续和高维切分搬运。 | `c_api/cube_datamove/cube_datamove.h` |

### 矩阵搬入辅助配置接口

**表2** 矩阵搬入辅助配置接口

| 接口名称 | 接口描述 | 最细粒度公共头文件 |
| --- | --- | --- |
| [asc_fill_l0a](cube_load_aux_config/asc_fill_l0a.md) | 将L0A Buffer的Local Memory初始化为某一具体数值。 | `c_api/cube_datamove/cube_datamove.h` |
| [asc_fill_l0b](cube_load_aux_config/asc_fill_l0b.md) | 将L0B Buffer的Local Memory初始化为某一具体数值。 | `c_api/cube_datamove/cube_datamove.h` |
| [asc_fill_l1](cube_load_aux_config/asc_fill_l1.md) | 将L1 Buffer的Local Memory初始化为某一具体数值。 | `c_api/cube_datamove/cube_datamove.h` |
| [asc_load_image_to_cbuf](cube_load_aux_config/asc_load_image_to_cbuf.md) | 将图像数据从Global Memory搬运到L1 Buffer。 | `c_api/cube_datamove/cube_datamove.h` |
| [asc_set_gm2l1_loop1_stride](cube_load_aux_config/asc_set_gm2l1_loop1_stride.md) | 将数据从GM搬运到L1 Buffer时，设置内层循环中相邻迭代数据块间的间隔。 | `c_api/cube_datamove/cube_datamove.h` |
| [asc_set_gm2l1_loop2_stride](cube_load_aux_config/asc_set_gm2l1_loop2_stride.md) | 将数据从GM搬运到L1 Buffer时，设置外层循环中相邻迭代数据块间的间隔。 | `c_api/cube_datamove/cube_datamove.h` |
| [asc_set_gm2l1_loop_size](cube_load_aux_config/asc_set_gm2l1_loop_size.md) | 将数据从GM搬运到L1 Buffer时，设置数据搬运流程中的循环次数。 | `c_api/cube_datamove/cube_datamove.h` |
| [asc_set_gm2l1_nz_para](cube_load_aux_config/asc_set_gm2l1_nz_para.md) | 设置MTE2_NZ_PARA寄存器的值。 | `c_api/cube_datamove/cube_datamove.h` |
| [asc_set_gm2l1_pad](cube_load_aux_config/asc_set_gm2l1_pad.md) | 设置[asc_copy_gm2l1_align](cube_compute_load/asc_copy_gm2l1_align.md)接口的常量填充值。 | `c_api/cube_datamove/cube_datamove.h` |
| [asc_set_gm2l1_padding](cube_load_aux_config/asc_set_gm2l1_padding.md) | 设置[asc_copy_gm2l1（NPU架构版本3510，高维切分数据搬运）](cube_compute_load/asc_copy_gm2l1_highdim_split_arch_3510.md)的填充值。 | `c_api/cube_datamove/cube_datamove.h` |
| [asc_set_l12l0a_3d_padding](cube_load_aux_config/asc_set_l12l0a_3d_padding.md) | 设置3D左矩阵搬运填充值。 | `c_api/cube_datamove/cube_datamove.h` |
| [asc_set_l12l0b_3d_padding](cube_load_aux_config/asc_set_l12l0b_3d_padding.md) | 设置3D右矩阵搬运填充值。 | `c_api/cube_datamove/cube_datamove.h` |
| [asc_set_l13d_fmatrix](cube_load_aux_config/asc_set_l13d_fmatrix.md) | 配置3D img2col搬运模式下左矩阵的Feature Map属性描述参数。 | `c_api/cube_datamove/cube_datamove.h` |
| [asc_set_l13d_fmatrix_b](cube_load_aux_config/asc_set_l13d_fmatrix_b.md) | 配置3D img2col搬运模式下右矩阵的Feature Map属性描述参数。 | `c_api/cube_datamove/cube_datamove.h` |
| [asc_set_l13d_rpt](cube_load_aux_config/asc_set_l13d_rpt.md) | 配置3D img2col搬运模式下使用`repeat`模式搬运左矩阵所需的参数。 | `c_api/cube_datamove/cube_datamove.h` |
| [asc_set_l13d_rpt_b](cube_load_aux_config/asc_set_l13d_rpt_b.md) | 配置3D img2col搬运模式下使用`repeat`模式搬运右矩阵所需的参数。 | `c_api/cube_datamove/cube_datamove.h` |
| [asc_set_l13d_size](cube_load_aux_config/asc_set_l13d_size.md) | 设置[asc_copy_l12l0a](cube_compute_load/asc_copy_l12l0a_arch_2201.md)/[asc_copy_l12l0b](cube_compute_load/asc_copy_l12l0b_arch_2201.md)的3D格式搬运接口在L1 Buffer的边界值。 | `c_api/cube_datamove/cube_datamove.h` |
| [asc_set_l3d_rpt_b](cube_load_aux_config/asc_set_l3d_rpt_b.md) | 通过64位配置值设置3D img2col搬运模式下使用`repeat`模式搬运右矩阵所需的参数。 | `c_api/cube_datamove/cube_datamove.h` |

### 矩阵计算的搬出

**表3** 矩阵计算的搬出接口

| 接口名称 | 接口描述 | 最细粒度公共头文件 |
| --- | --- | --- |
| [asc_copy_l0c2gm](cube_compute_store/asc_copy_l0c2gm.md) | 将L0C中的数据搬运到GM中。 | `c_api/cube_datamove/cube_datamove.h` |
| [asc_copy_l0c2gm（NPU架构版本2201）](cube_compute_store/asc_copy_l0c2gm_arch_2201.md) | 将L0C Buffer中的矩阵计算结果搬运到GM，支持随路量化处理。 | `c_api/cube_datamove/cube_datamove.h` |
| [asc_copy_l0c2gm（NPU架构版本3510）](cube_compute_store/asc_copy_l0c2gm_arch_3510.md) | 将矩阵计算结果从L0C Buffer搬运至Global Memory，搬运过程中可同步支持随路量化、随路激活、随路格式转换（Nz2ND/Nz2DN）等能力组合。 | `c_api/cube_datamove/cube_datamove.h` |
| [asc_copy_l0c2l1](cube_compute_store/asc_copy_l0c2l1.md) | 矩阵计算完成后，对结果进行量化处理，之后将处理结果搬运到L1中。 | `c_api/cube_datamove/cube_datamove.h` |
| [asc_copy_l0c2l1（NPU架构版本2201）](cube_compute_store/asc_copy_l0c2l1_arch_2201.md) | 将L0C Buffer中的矩阵计算结果搬运到L1 Buffer，支持随路量化处理。 | `c_api/cube_datamove/cube_datamove.h` |
| [asc_copy_l0c2l1（NPU架构版本3510）](cube_compute_store/asc_copy_l0c2l1_arch_3510.md) | 将矩阵计算结果从L0C Buffer搬运到L1 Buffer，支持随路量化、ReLU、Nz2ND格式转换和通道拆分。 | `c_api/cube_datamove/cube_datamove.h` |
| [asc_copy_l0c2ub](cube_compute_store/asc_copy_l0c2ub.md) | 将L0C中的数据搬运到UB中。 | `c_api/cube_datamove/cube_datamove.h` |
| [asc_copy_l12gm](cube_compute_store/asc_copy_l12gm.md) | 将数据从L1搬运到GM。 | `c_api/cube_datamove/cube_datamove.h` |
| [asc_copy_l12ub](cube_compute_store/asc_copy_l12ub.md) | 将数据从L1 Buffer搬运到UB中。 | `c_api/cube_datamove/cube_datamove.h` |

### 矩阵搬出辅助配置接口

**表4** 矩阵搬出辅助配置接口

| 接口名称 | 接口描述 | 最细粒度公共头文件 |
| --- | --- | --- |
| [asc_get_l0c2gm_prequant](cube_store_aux_config/asc_get_l0c2gm_prequant.md) | 数据搬运过程中进行随路量化时，通过调用该接口获取量化操作前矢量的起始地址。 | `c_api/cube_datamove/cube_datamove.h` |
| [asc_get_l0c2gm_relu](cube_store_aux_config/asc_get_l0c2gm_relu.md) | 数据搬运过程中进行随路量化时，通过调用该接口获取ReLU操作前矢量的起始地址。 | `c_api/cube_datamove/cube_datamove.h` |
| [asc_get_l0c2gm_unitflag](cube_store_aux_config/asc_get_l0c2gm_unitflag.md) | 数据搬运过程中进行随路量化时，通过调用该接口获取`unit_flag`设置。 | `c_api/cube_datamove/cube_datamove.h` |
| [asc_set_l0c_copy_channel_para](cube_store_aux_config/asc_set_l0c_copy_channel_para.md) | 配置Nz2DN格式转换时源Nz矩阵中相邻行的地址偏移。 | `c_api/cube_datamove/cube_datamove.h` |
| [asc_set_l0c_copy_config](cube_store_aux_config/asc_set_l0c_copy_config.md) | 设置L0C Buffer搬出场景下随路tensor量化、随路tensor激活和UnitFlag功能的相关配置。 | `c_api/cube_datamove/cube_datamove.h` |
| [asc_set_l0c_copy_lrelu_alpha](cube_store_aux_config/asc_set_l0c_copy_lrelu_alpha.md) | 在L0C Buffer搬出过程中进行随路Scalar激活时，配置激活计算所需的缩放系数。 | `c_api/cube_datamove/cube_datamove.h` |
| [asc_set_l0c_copy_nz_para](cube_store_aux_config/asc_set_l0c_copy_nz_para.md) | 数据搬运过程中进行随路格式转换（NZ格式转换为ND格式）时，设置格式转换的相关配置。 | `c_api/cube_datamove/cube_datamove.h` |
| [asc_set_l0c_copy_params](cube_store_aux_config/asc_set_l0c_copy_params.md) | 将L0C Buffer数据搬运到Global Memory或L1 Buffer并进行随路格式转换（NZ格式转换为ND格式）时，通过该接口设置格式转换的相关配置。 | `c_api/cube_datamove/cube_datamove.h` |
| [asc_set_l0c_copy_prequant](cube_store_aux_config/asc_set_l0c_copy_prequant.md) | 数据搬运过程中进行随路量化时，通过调用该接口设置量化流程中的标量量化参数。 | `c_api/cube_datamove/cube_datamove.h` |
| [asc_set_l0c_copy_relu_alpha](cube_store_aux_config/asc_set_l0c_copy_relu_alpha.md) | 在L0C Buffer搬出过程中进行随路Scalar激活时，配置激活计算所需的缩放系数。 | `c_api/cube_datamove/cube_datamove.h` |
