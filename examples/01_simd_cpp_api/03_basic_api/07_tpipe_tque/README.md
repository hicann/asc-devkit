# TPipe/TQue资源管理类API样例介绍

## 概述

本路径下包含了与TPipe/TQue资源管理相关的多个API样例。每个样例均基于Ascend C的<<<>>>直调方法，展示TPipe、TQue相关接口的使用方式。

## 样例列表

| 目录名称 | 功能描述 | 支持的产品 |
| ----------------------------------------------------------- | --------------------------------------------------- | --- |
| [get_tpipe_ptr](./get_tpipe_ptr) |  样例基于GetTPipePtr获取全局TPipe指针，核函数无需显式传入TPipe指针，即可进行TPipe相关的操作。 | Ascend 950PR&950DT系列产品<br>Atlas A3系列产品<br>Atlas A2系列产品 |
| [tpipe_reuse](./tpipe_reuse) |  本样例基于TPipe::Init和TPipe::Destroy，实现TPipe重复申请与使用。 | Ascend 950PR&950DT系列产品<br>Atlas A3系列产品<br>Atlas A2系列产品 |
