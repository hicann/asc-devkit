# 工具类API样例介绍

## 概述

本路径下包含了与工具相关API的样例。样例基于Ascend C的<<<>>>直调方法，支持main函数和kernel函数在同一个文件中实现。

## 样例列表

| 目录名称 | 功能描述 | 支持的产品 |
| ------- | -------- | --- |
| [ctrl_spr](./ctrl_spr) |  本样例基于SetCtrlSpr、GetCtrlSpr、ResetCtrlSpr接口实现对CTRL寄存器（控制寄存器）的特定比特位的设置、读取和重置，并验证非饱和模式是否正确。 | Ascend 950PR/Ascend 950DT |
| [get_ub_size](./get_ub_size) |  本样例展示GetUBSizeInBytes和GetRuntimeUBSize接口使用方法，用于获取用户最大可使用的UB大小。 | Ascend 950PR/Ascend 950DT |
| [gm_by_dcache](./gm_by_dcache) |  本样例介绍经过DCache（Data Cache）从GM地址读取和写入数据。 | Ascend 950PR/Ascend 950DT<br>Atlas A3 训练系列产品/Atlas A3 推理系列产品<br>Atlas A2 训练系列产品/Atlas A2 推理系列产品 |
| [gm_by_pass_dcache](./gm_by_pass_dcache) |  本样例基于ReadGmBypassDCache和WriteGmBypassDCache接口，实现不经过DCache从GM读取和写入数据。 | Ascend 950PR/Ascend 950DT<br>Atlas A3 训练系列产品/Atlas A3 推理系列产品<br>Atlas A2 训练系列产品/Atlas A2 推理系列产品 |
