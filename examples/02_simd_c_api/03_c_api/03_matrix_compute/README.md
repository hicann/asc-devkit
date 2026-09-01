# Matrix Compute API样例介绍

## 概述

本样例集介绍了矩阵计算API不同特性的典型用法，给出了对应的端到端实现。目录下的样例命名按照"API名称_通路"，如下：
1. **load_data**：以"load_data"开头的样例介绍L1 Buffer -> L0 Buffer的相关API；
2. **mmad**：以"mmad"开头的样例介绍矩阵乘法Mmad、MmadMx API；
3. **fixpipe**：以"fixpipe"开头的样例介绍L0C Buffer → Global Memory/L1 Buffer/Unified Buffer的相关API；

## 样例列表

| 目录名称 | 功能描述 | 支持的产品 |
|--------------------------------------------------------------------------------------------| ---------------------------------------------------- | --- |
| [load_data_l12l0_950](./load_data_l12l0_950) |  本样例展示如何调用`asc_copy_l12l0a`/`asc_copy_l12l0b`及其`_transpose`接口，将A/B矩阵从L1搬运到L0A/L0B Buffer。 | Ascend 950PR/Ascend 950DT |
| [mmad](./mmad) |  本样例以 int8_t 和 bfloat16 两种输入数据类型为例，演示如何通过C API实现矩阵乘法（C = A × B + Bias） | Ascend 950PR/Ascend 950DT |
| [mmad_mx](./mmad_mx) | 本样例演示如何通过C API实现带量化功能的MX矩阵乘法。A、B矩阵使用FP4或FP8数据类型，ScaleA、ScaleB使用fp8_e8m0_t数据类型；包含C矩阵初始化为0、从BiasTable Buffer初始化以及从L0C累加三种场景，并介绍K轴对齐和FP8尾部清零 | Ascend 950PR/Ascend 950DT |
