# 概述<a name="ZH-CN_TOPIC_0000001983023284"></a>

本章节内容介绍AI框架调用自定义算子的方法。如下图所示，Pytorch支持单算子和图模式两种，TensorFlow仅支持图模式。

AI框架调用时，除了需要提供CANN框架调用时需要的代码实现文件，还需要进行插件适配开发。

![](../../../figures/kernelLaunch.png)
