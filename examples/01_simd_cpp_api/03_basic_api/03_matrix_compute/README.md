# Matrix Compute API样例介绍

## 概述

本样例集介绍了矩阵计算API不同特性的典型用法，给出了对应的端到端实现。目录下的样例命名按照"API名称_通路"，如下：
1. **load_data**：以"load_data"开头的样例介绍L1 Buffer -> L0 Buffer的相关API；
2. **mmad**：以"mmad"开头的样例介绍矩阵乘法Mmad API；
3. **fixpipe**：以"fixpipe"开头的样例介绍L0C Buffer → Global Memory/L1 Buffer/Unified Buffer的相关API；

## 样例列表
| 目录名称 | 功能描述 | 支持的产品 |
|--------------------------------------------------------------------------------------------| ---------------------------------------------------- | --- |
| [batch_matmul](./batch_matmul) |  本样例介绍在输入为float数据类型并且左、右矩阵均不转置的场景下，带batch的矩阵乘法，其中GM -> L1通路采用DataCopy（Nd2NzParams）批量搬运，L0C -> GM、L0C -> L1通路采用Fixpipe批量搬运，L1 -> L0A / L0B搬运和Mmad矩阵乘循环执行batch次 | Ascend 950PR/Ascend 950DT<br>Atlas A3 训练系列产品/Atlas A3 推理系列产品<br>Atlas A2 训练系列产品/Atlas A2 推理系列产品 |
| [fixpipe_l0c2gm](./fixpipe_l0c2gm) |  本样例介绍如何使用Fixpipe将矩阵乘的结果从L0C Buffer搬出到GM（Global Memory） | Ascend 950PR/Ascend 950DT<br>Atlas A3 训练系列产品/Atlas A3 推理系列产品<br>Atlas A2 训练系列产品/Atlas A2 推理系列产品 |
| [fixpipe_l0c2l1](./fixpipe_l0c2l1) |  本样例介绍如何使用Fixpipe将矩阵乘的结果从L0C Buffer搬出到L1（L1 Buffer） | Ascend 950PR/Ascend 950DT<br>Atlas A3 训练系列产品/Atlas A3 推理系列产品<br>Atlas A2 训练系列产品/Atlas A2 推理系列产品 |
| [fixpipe_l0c2ub](./fixpipe_l0c2ub) |  本样例介绍如何使用Fixpipe将矩阵乘的结果从L0C Buffer搬出到UB（Unified Buffer） | Ascend 950PR/Ascend 950DT |
| [load_data_2dmx_l12l0](./load_data_2dmx_l12l0) |  本样例展示MX量化矩阵乘法中，如何使用基础API `LoadData` 完成L1到L0的数据搬运 | Ascend 950PR/Ascend 950DT |
| [load_data_2dv2_l12l0](./load_data_2dv2_l12l0) |  本样例展示如何调用`LoadData`接口并配置`LoadData2DParamsV2`结构体参数（本样例中简称：`Load2Dv2`接口），将A / B矩阵从L1搬运到L0A / L0B | Ascend 950PR/Ascend 950DT |
| [load_data_l12l0](./load_data_l12l0) |  本样例展示B4 / B8 / B16 / B32输入数据类型下，A / B矩阵从GM到L1、L1到L0、矩阵乘和结果搬出的完整流程 | Atlas A3 训练系列产品/Atlas A3 推理系列产品<br>Atlas A2 训练系列产品/Atlas A2 推理系列产品 |
| [load_data_with_stride](./load_data_with_stride) |  本样例介绍LoadDataWithStride指令在矩阵乘法中的使用场景和方法，配合SetLoadDataRepeatWithStride可以将二维的A、B矩阵从L1搬运到L0A/L0B | Ascend 950PR/Ascend 950DT |
| [mmad](./mmad) |  本样例介绍了输入为ND格式，B4 / B8 / B16 / B32输入数据类型（具体以int4_t / int8_t / bfloat16 / float为例），四种输入数据类型下的矩阵乘法，说明如何通过Mmad指令实现矩阵乘法计算（C = A x B + Bias） | Ascend 950PR/Ascend 950DT<br>Atlas A3 训练系列产品/Atlas A3 推理系列产品<br>Atlas A2 训练系列产品/Atlas A2 推理系列产品 |
| [mmad_gemv](./mmad_gemv) |  本样例介绍了GEMV（M=1）模式下的矩阵乘法 | Ascend 950PR/Ascend 950DT<br>Atlas A3 训练系列产品/Atlas A3 推理系列产品<br>Atlas A2 训练系列产品/Atlas A2 推理系列产品 |
| [mmad_load3dv2](./mmad_load3dv2) |  本样例介绍LoadData（卷积数据搬运）v2指令在矩阵乘法中的使用场景和方法。LoadData（卷积数据搬运）v2可以将二维的A、B矩阵从L1搬运到L0A/L0B，其中 A 和 B 分别表示矩阵乘法的左右输入矩阵 | Ascend 950PR/Ascend 950DT<br>Atlas A3 训练系列产品/Atlas A3 推理系列产品<br>Atlas A2 训练系列产品/Atlas A2 推理系列产品 |
| [mmad_mx](./mmad_mx)  |  本样例介绍了输入为ND格式，A、B矩阵为FP4（fp4x2_e1m2_t、fp4x2_e2m1_t）和FP8（fp8_e4m3fn_t、fp8_e5m2_t）数据类型，量化系数矩阵scaleA、scaleB为fp8_e8m0_t数据类型，左矩阵和左量化系数矩阵不转置、右矩阵和右量化系数矩阵转置共 4 种带有量化功能的矩阵乘（简称：MX矩阵乘法） | Ascend 950PR/Ascend 950DT |
| [mmad_unitflag](./mmad_unitflag) |  本样例介绍了如何在调用Mmad指令时，使用unitFlag功能。 | Ascend 950PR/Ascend 950DT<br>Atlas A3 训练系列产品/Atlas A3 推理系列产品<br>Atlas A2 训练系列产品/Atlas A2 推理系列产品 |
| [mmad_with_sparse](./mmad_with_sparse) |  本样例介绍基础API MmadWithSparse调用样例 | Atlas A3 训练系列产品/Atlas A3 推理系列产品<br>Atlas A2 训练系列产品/Atlas A2 推理系列产品 |
