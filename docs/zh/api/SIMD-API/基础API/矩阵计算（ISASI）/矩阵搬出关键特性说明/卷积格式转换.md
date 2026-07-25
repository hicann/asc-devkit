# 卷积格式转换

在卷积运算中，L0C Buffer上存放的数据格式为NC1HWC0格式，Fixpipe指令通过[NZ2ND](NZ2ND.md)能力，支持将L0C Buffer上数据转为NHWC格式输出。转换示意图如下所示：

**图1** NC1HWC0转换为NHWC格式<a name="zh-cn_topic_0000002515830464_fig63620361414"></a>  

![](../../../../figures/Fixpipe_NC1HWC0_NHWC.png)

<!-- npu="950" id1 -->
特别的，针对Ascend 950PR/Ascend 950DT产品，还支持Fixpipe指令通过[NZ2DN](NZ2DN.md)能力，将L0C Buffer上数据转为NCHW格式输出。转换示意图如下所示：

**图2**  NC1HWC0转换为NCHW格式<a name="fig188671630134414"></a>  

![](../../../../figures/Fixpipe_NC1HWC0_NCHW.png)
<!-- end id1 -->
