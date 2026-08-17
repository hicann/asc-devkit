# 概述<a name="ZH-CN_TOPIC_0000002500468230"></a>

本节将以Add算子为例，带您快速构建Ascend C矢量算子程序，并学习矢量算子开发的典型场景以及处理方式。涉及的场景包括：

-   [基础矢量算子](basic_vector_operator.md)：开发一个简单的Add矢量算子。
-   [TBuf的使用](tbuf_usage.md)：在算子计算过程中使用临时空间存储运算的中间结果。
-   [多核Tiling切分](multi_core_tiling/overview.md)：当算子需要多核并行计算或支持动态shape时，对输入数据进行切分、分块计算，并将不同数据块分配到多个AI Core上处理。
    -   [主块均分](multi_core_tiling/main_block_even_split.md)：算子在AI处理器的多个核上运行，所有核的计算数据量相等且32字节对齐。
    -   [尾块均分](multi_core_tiling/tail_block_even_split.md)：算子在AI处理器的多个核上运行，所有核的计算数据量相等，每个核上除最后一个数据块（尾块）外，其余数据块的数据量相等，每个核都需要处理尾块数据的计算。
    -   [尾核切分](multi_core_tiling/tail_core_split.md)：算子在AI处理器的多个核上运行，数据无法平均分配到每个核。将所有核分为多个整核和多个尾核，整核的计算数据量相等，尾核的计算数据量相等。
    -   [尾核尾块切分](multi_core_tiling/tail_core_tail_block_split.md)：算子在AI处理器的多个核上运行，数据无法平均分配到每个核，同时每个核内的数据无法均分，除最后一个数据块（尾块）外，其余数据块的数据量相等，每个核都需要单独处理尾块数据的计算。
-   [DoubleBuffer场景](double_buffer_scenario.md)：开启double buffer，算子中的多条流水并行执行。
-   [Broadcast场景](broadcast_scenario.md)：算子中两个输入的shape（形状）不相等，需要将一个输入的shape进行Broadcast（广播）后，再执行计算。
-   [非对齐场景](unaligned_scenario.md)：更多数据非32字节对齐场景的处理方案。

>[!NOTE]注意 
>进行数据搬运和Vector计算时，对于搬运的数据长度和操作数的起始地址有如下的对齐要求：
>- 使用DataCopy接口进行数据搬运，搬运的数据长度和操作数的起始地址（Unified Buffer上）必须保证32字节对齐。
>- 通常情况下，进行Vector计算时，操作数的起始地址必须保证32字节对齐，执行计算的基本单位为32字节。
