# 样例列表
|  目录名称                                                   |  功能描述                                              |
| ------------------------------------- | ------------------------------------- |
| [00_introduction](./00_introduction) | 基于Ascend C SIMT编程的简单的示例，通过Ascend C编程语言实现了自定义算子，分别给出对应的<<<>>>直调实现，适合初学者。 |
| [01_utilities](./01_utilities) | 基于Ascend C SIMT的调试调优样例，通过printf()、assert()、clock()等API介绍上板打印、打点、CPU孪生调试等系统工具使用方法，适用于调试阶段。 |
| [02_features](./02_features) | 展示Ascend C SIMT编程的重点特性的样例，包含基础编程、内存资源管理、线程同步、原子操作等特性。 |
| [03_best_practices](./03_best_practices) | 展示Ascend C SIMT编程的最佳实践。 |
| [05_troubleshooting](./05_troubleshooting) | 展示Ascend C SIMT编程的故障分析与问题定位样例。 |

## npu-arch编译选项说明

开发者需根据实际的执行环境，修改具体样例目录下CMakeLists.txt文件中的`--npu-arch`编译选项，参考下表中的对应关系，修改为环境对应的npu-arch参数值。
| 产品型号 |  npu-arch参数 |
| ---- | ---- |
| Ascend 950PR/Ascend 950DT | --npu-arch=dav-3510 |
