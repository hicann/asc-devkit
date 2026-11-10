---
okftype: reference.concept
description: "说明卷积场景下Fixpipe的格式转换：L0C Buffer上的NC1HWC0数据经Nz2ND转为NHWC输出，Ascend 950PR&950DT系列产品还可经Nz2DN转为NCHW输出，转换参数在结果搬出入口按目标布局设置。"
---
# 卷积格式转换

在卷积运算中，L0C Buffer上存放的数据格式为NC1HWC0格式，Fixpipe指令通过[Nz2ND](NZ2ND.md)能力，支持将L0C Buffer上数据转为NHWC格式输出。转换示意图如下所示：

**图1** NC1HWC0转换为NHWC格式

![](../../../../figures/Fixpipe_NC1HWC0_NHWC.png)

<!-- npu="950" id1 -->
特别的，针对Ascend 950PR&950DT系列产品，还支持Fixpipe指令通过[Nz2DN](NZ2DN.md)能力，将L0C Buffer上数据转为NCHW格式输出。转换示意图如下所示：

**图2** NC1HWC0转换为NCHW格式

![](../../../../figures/Fixpipe_NC1HWC0_NCHW.png)
<!-- end id1 -->

卷积输入的im2col搬入、矩阵计算与结果搬出之间的关系见[矩阵计算流程](../../cube_compute/overview.md#conv2d前向计算流程)中的卷积场景。C API结果搬出入口为[asc_copy_l0c2gm](../cube_compute_store/asc_copy_l0c2gm.md)；选择当前架构版本后，再按目标布局设置格式转换参数。
