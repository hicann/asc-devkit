# 概述

## 矩阵计算单元

矩阵计算单元的详细描述请参考[LINK](../../basic_api/cube_compute_ISASI/overview/cube_compute_unit.md)。

## 矩阵计算流程

Tensor API针对矩阵计算编程模型提供了两类接口，分别承载Cube核中各个通路的搬运能力和计算能力，如下图所示：

**图1**  普通矩阵基础计算流程图<a name="zh-cn_topic_0000002535567224_fig135639216483"></a>

![普通矩阵基础计算流程图](../../../figures/matrix_computation_workflow_tensor_api.png)

1. 通过`copy`接口将A、B原始矩阵的Global Memory数据搬运到L1 Buffer中（如果存在Bias/随路量化，则同样通过`copy`搬运到L1 Buffer中）。详细内容请参考[矩阵计算的搬入](cube_compute_load/cube_compute_load.md)。

2. 通过`copy`接口将A、B矩阵分别加载到L0A Buffer和L0B Buffer上准备计算（如果存在Bias/随路量化，则通过`copy`将L1 Buffer中的Bias数据/量化系数数据搬运到BiasTable Buffer/Fixpipe Buffer上）。详细内容请参考[矩阵计算的搬入](cube_compute_load/cube_compute_load.md)。

3. 通过`mmad`接口对L0A Buffer、L0B Buffer、BiasTable Buffer上面的数据进行矩阵计算，并输出结果到L0C Buffer上。详细内容请参考[mmad计算](mmad_compute/mmad_compute.md)。

4. 通过`copy`接口将L0C Buffer的数据进行处理并搬出到Global Memory，`copy`接口可以利用Fixpipe Buffer数据进行如随路量化、Relu等操作。详细内容请参考[矩阵计算的搬出](cube_compute_store/cube_compute_store.md)。
