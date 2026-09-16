# 普通矩阵计算流程

C API针对矩阵计算编程模型提供了数据搬入、矩阵计算和结果搬出接口，分别承载Cube核中各个通路的搬运能力和计算能力，如下图所示：

**图1** 普通矩阵基础计算流程图  
![](../../../../figures/matrix_computation_workflow_c_api.png "普通矩阵基础计算流程图")

1. 通过[asc_copy_gm2l1](../../cube_datamove/asc_copy_gm2l1/asc_copy_gm2l1.md)搬入已排布好的A、B矩阵；对于ND输入，使用[asc_copy_gm2l1_nd2nz](../../cube_datamove/asc_copy_gm2l1_nd2nz/asc_copy_gm2l1_nd2nz.md)完成分形转换。Bias和随路量化系数也先搬入L1 Buffer。

2. 通过[asc_copy_l12l0a](../../cube_datamove/asc_copy_l12l0a/asc_copy_l12l0a.md)、[asc_copy_l12l0b](../../cube_datamove/asc_copy_l12l0b/asc_copy_l12l0b.md)将A、B分别搬入L0A Buffer、L0B Buffer；根据输入布局选择转置变体。Bias和量化系数分别通过[asc_copy_l12bt](../../cube_datamove/asc_copy_l12bt/asc_copy_l12bt.md)、[asc_copy_l12fb](../../cube_datamove/asc_copy_l12fb/asc_copy_l12fb.md)搬入对应Buffer。

3. 通过[asc_mmad](../asc_mmad.md)计算，输出至L0C Buffer。参数顺序为`m, k, n`，初次从零计算时设置`c_matrix_init_val=true`；后续K块累加时设置`c_matrix_source=false`、`c_matrix_init_val=false`。Bias初始化的参数说明见[asc_mmad](../asc_mmad.md)。

4. 通过[asc_copy_l0c2gm](../../cube_datamove/asc_copy_l0c2gm/asc_copy_l0c2gm.md)搬出结果，配置随路量化、激活和输出格式。也可搬到L1 Buffer，见[搬出总体说明](../../cube_datamove/cube_compute_store_overall_description.md)。

<!-- npu="950" id1 -->
# Mx矩阵计算流程

Mx矩阵计算仅在Ascend 950PR/Ascend 950DT型号支持。

MX矩阵乘法（Microscaling矩阵乘加）为带有量化系数的矩阵乘法，即左矩阵和右矩阵均有对应的量化系数矩阵，左量化系数矩阵scaleA和右量化系数矩阵scaleB。MX矩阵乘法场景中，左量化系数矩阵与左矩阵乘积，右量化系数矩阵与右矩阵乘积，对两个乘积的结果做矩阵乘法。C API通过asc_mmad_mx及独立的数据/系数搬运接口支持MX矩阵计算，如下图所示：

**图 2** Mx矩阵基础计算流程图  
![](../../../../figures/mx_matrix_computation_workflow_c_api.png "Mx矩阵基础计算流程图")

1. 将A、B和[scaleA/scaleB系数矩阵](../cube_compute_fractal_intro/aux_cube_fractal_format_details.md)从GM搬到L1 Buffer。A/B数据分形与系数分形不同，分别配置各自的布局、大小和步长。

2. 使用[asc_copy_l12l0a](../../cube_datamove/asc_copy_l12l0a/asc_copy_l12l0a_2d_arch_3510.md)、[asc_copy_l12l0b](../../cube_datamove/asc_copy_l12l0b/asc_copy_l12l0b_2d_arch_3510.md)搬入A/B；另外调用[asc_copy_l12l0a_mx](../../cube_datamove/asc_copy_l12l0a_mx.md)、[asc_copy_l12l0b_mx](../../cube_datamove/asc_copy_l12l0b_mx.md)搬入scaleA/scaleB。

3. 数据、系数及可选Bias就绪后，通过[asc_mmad_mx](../asc_mmad_mx.md)计算并输出至L0C Buffer。

4. 使用[asc_copy_l0c2gm](../../cube_datamove/asc_copy_l0c2gm/asc_copy_l0c2gm_arch_3510.md)处理并搬出结果。
<!-- end id1 -->

<!-- npu="910b,A3" id2 -->
# 4选2稀疏矩阵计算流程

稀疏矩阵是一种特殊类型的矩阵，即矩阵中包含较多的零元素。4选2结构化稀疏矩阵计算，要求一个连续的4个权重或激活值的组（通常是张量中的一行或一列）中，最多只有2个值为非零，其余2个强制为零。针对Atlas A2训练系列产品/Atlas A2推理系列产品和Atlas A3训练系列产品/Atlas A3推理系列产品，Ascend C针对4选2稀疏矩阵计算编程模型提供了对应sparse类接口，如下图所示：

**图 3** 4选2稀疏矩阵基础计算流程图  
![](../../../../figures/sparse_matrix_computation_workflow_c_api.png "4选2稀疏矩阵基础计算流程图")

1. 首先将原始矩阵B按照固定粒度划分连续4个元素为一组，按照4选2结构化稀疏压缩规则，离线生成对应的压缩矩阵B与[索引矩阵](../cube_compute_fractal_intro/aux_cube_fractal_format_details.md#4选2稀疏索引矩阵)作为输入。

2. 通过asc_copy_gm2l1接口将原始矩阵A、压缩矩阵B、索引矩阵的GM数据搬运到L1 Buffer中（如果存在随路量化，则同样通过asc_copy_gm2l1搬到L1 Buffer中）。

3. 通过asc_copy_l12l0a接口将原始矩阵A加载到L0A Buffer，通过[asc_copy_l12l0b_sparse接口](../../cube_datamove/asc_copy_l12l0b_sparse.md)将压缩矩阵B与索引矩阵分别加载到L0B Buffer与内置IDX Buffer上准备计算（如果存在随路量化，则通过asc_copy_l12fb将L1 Buffer中的量化系数数据搬运到Fixpipe Buffer上）。

4. 通过asc_mmad_sparse接口对L0A Buffer、L0B Buffer、内置IDX Buffer上面的数据进行4选2稀疏矩阵计算，并输出结果到L0C Buffer上。详细内容请参考[asc_mmad_sparse计算](../asc_mmad_sparse.md)。

5. 通过asc_copy_l0c2gm接口将L0C Buffer的数据进行处理并搬出到GM，asc_copy_l0c2gm接口可以利用Fixpipe Buffer数据进行如随路量化、Relu等操作。详细内容请参考[asc_copy_l0c2gm（L0C Buffer到GM数据搬运）](../../cube_datamove/asc_copy_l0c2gm/asc_copy_l0c2gm.md)。
<!-- end id2 -->

# Conv2D前向计算流程

Conv2D前向计算的本质是将卷积运算转换为矩阵乘：先通过img2col（Image to Column）将FeatureMap按卷积核滑动窗口展开为左矩阵A，将Weight作为右矩阵B，再做矩阵乘累加得到输出。与[普通矩阵计算流程](#普通矩阵计算流程)的区别在于：A矩阵从GM直接搬入后，由[asc_copy_l12l0a的3D重载](../../cube_datamove/asc_copy_l12l0a/asc_copy_l12l0a.md)在L1 Buffer到L0A Buffer的搬运过程中，根据卷积核尺寸、stride、dilation、padding在线完成img2col空间展开生成，如下图所示：

**图 4** Conv2D前向计算流程图  
![](../../../../figures/conv2d_forward_c_api.png "Conv2D前向计算流程图")

1. 将FeatureMap和Weight从GM搬入L1 Buffer。FeatureMap按[NC1HWC0格式](../cube_compute_fractal_intro/key_fractal_format_details.md#nc1hwc0格式)准备；Weight按后续L0B Buffer搬运所需布局准备。

2. 使用`asc_copy_l12l0a`的3D重载执行img2col，并通过`asc_copy_l12l0b`搬入Weight。调用前配置FeatureMap属性、重复步长、边界与Padding，具体接口见[搬入总体说明](../../cube_datamove/cube_compute_load_overall_description.md)。

3. 使用[asc_mmad](../asc_mmad.md)完成矩阵乘加，结果存放在L0C Buffer。

4. 使用`asc_copy_l0c2gm`的[Nz2ND](../../cube_datamove/cube_store_key_features/NZ2ND.md)能力，将卷积结果输出为NHWC。维度映射见[卷积格式转换](../../cube_datamove/cube_store_key_features/convolution_format_conversion.md)。<!-- npu="950" id3 -->特别地，针对Ascend 950PR/Ascend 950DT产品，还可通过[Nz2DN](../../cube_datamove/cube_store_key_features/NZ2DN.md)将卷积结果输出为NCHW格式。<!-- end id3 -->
