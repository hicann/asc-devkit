# F32 Channel Split

## 特性说明

矩阵计算的搬出过程中对于目标类型为f32，如果开启了Channel Split（通道拆分），硬件就能够将16×16的分形矩阵转换为16×8的小z分形矩阵，此时每个16×16的分形矩阵将被拆分为2个独立的16×8的小z分形矩阵。

## 特性约束

<!-- npu="A3,910b" id3 -->
- 针对如下产品型号：

    <!-- npu="A3" id4 -->
    Atlas A3 训练系列产品/Atlas A3 推理系列产品
    <!-- end id4 -->

    <!-- npu="910b" id5 -->
    Atlas A2 训练系列产品/Atlas A2 推理系列产品
    <!-- end id5 -->

    当Fixpipe指令的输入和输出数据类型都为float，Nz输出到GM，并且不开启Nz2ND与unitFlag时，才能够开启Channel Split功能。
<!-- end id3 -->

<!-- npu="950" id6 -->
- 针对Ascend 950PR/Ascend 950DT，当Fixpipe指令的输入和输出数据类型都为float,Nz输出到GM，并且不开启Nz2ND、Nz2DN与unitFlag时，才能够开启Channel Split功能。当前搬出到L1 Buffer和Unified Buffer（UB）不支持该功能。
<!-- end id6 -->

## 使用示例

如下图所示，当开启Channel Split功能后，shape为\[64,32\]的源操作数将会被拆分为16个独立的16x8分形矩阵。

**图1** F32 Channel Split示意图（无无效数据）

![](../../../../figures/Fixpipe_Channel_Split.png)

当开启Channel Split功能后，n_size（源Nz矩阵在N方向上的大小）必须为8的倍数。如下图所示，shape为\[64,24\]的源操作数将会被拆分为3列16x8分形矩阵。

**图2** F32 Channel Split示意图（有无效数据）

![](../../../../figures/Fixpipe_Channel_Split_with_dirty.png)
