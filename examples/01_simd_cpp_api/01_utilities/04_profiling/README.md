# Profiling样例介绍
## 概述
本样例展示了如何通过Profiling工具采集性能数据。

## 样例列表
| 目录名称 | 功能描述 | 支持的产品 |
| ------------------------------------------------------------ | ---------------------------------------------------- | --- |
| [msprof](./msprof) |  本样例以Add计算为载体，展示如何通过`msopprof`采集上板性能数据。 | Ascend 950PR&950DT系列产品<br>Atlas A3系列产品<br>Atlas A2系列产品 |
| [torch_library_report_tensor](./torch_library_report_tensor) |  本样例展示在`torch.library`自定义算子调用方式下，如何集成Profiling并在msprof结果中展示输入输出Tensor的Shape、数据类型和Format信息。 | Ascend 950PR&950DT系列产品<br>Atlas A3系列产品<br>Atlas A2系列产品 |
