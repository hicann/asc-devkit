# F32 Channel Split<a name="ZH-CN_TOPIC_0000002569070989"></a>

## 特性说明

矩阵计算的搬出过程中对于目标类型为f32，如果开启了Channel Split（通道拆分），硬件就能够将16×16的分形矩阵转换为16×8的小z分形矩阵，此时每个16×16的分形矩阵将被拆分为2个独立的16×8的小z分形矩阵。

对于DataCopy接口，可以通过配置**DataCopyCO12DstParams结构体参数**定义中channelSplit为true开启。

对于Fixpipe接口，可以通过配置[Fixpipe搬运参数结构体说明](../矩阵计算的搬出/Fixpipe（L0C到GM数据搬运）.md)中channelSplit为true开启。

## 特性约束

<!-- npu="A3,910b" id1 -->
- 针对如下产品型号：

    <!-- npu="A3" id2 -->
    Atlas A3 训练系列产品/Atlas A3 推理系列产品
    <!-- end id2 -->
    
    <!-- npu="910b" id3 -->
    Atlas A2 训练系列产品/Atlas A2 推理系列产品
    <!-- end id3 -->
    
    当Fixpipe指令的输入和输出数据类型都为float，NZ输出到GM，并且不开启NZ2ND与unitFlag时，才能够开启Channel Split功能。
<!-- end id1 -->

<!-- npu="950" id4 -->
- 针对Ascend 950PR/Ascend 950DT，当Fixpipe指令的输入和输出数据类型都为float,NZ输出到GM，并且不开启NZ2ND、NZ2DN与unitFlag时，才能够开启Channel Split功能。当前搬出到L1 Buffer和UB不支持该功能。
<!-- end id4 -->

## 使用示例

完整示例请参考[fixpipe_l0c2gm样例](../../../../../../../examples/01_simd_cpp_api/03_basic_api/03_matrix_compute/fixpipe_l0c2gm)场景7。

如下图所示，当开启Channel Split功能后，shape为\[64,32\]的源操作数将会被拆分为16个独立的16x8分形矩阵。

**图1** F32 Channel Split示意图（无无效数据）<a name="zh-cn_topic_0000002515660950_fig222295694112"></a>  

![](../../../../figures/Fixpipe_Channel_Split.png)

当开启Channel Split功能后，nSize（源NZ矩阵在N方向上的大小）必须为8的倍数。如下图所示，shape为\[64,24\]的源操作数将会被拆分为3列16x8分形矩阵。

**图2** F32 Channel Split示意图（有无效数据）<a name="zh-cn_topic_0000002515660950_fig5809145844116"></a>  

![](../../../../figures/Fixpipe_Channel_Split_with_dirty.png)
