# 概述

Region Proposal（简称RP）是目标检测算法中用于定位潜在目标区域的候选框。Region Proposal采用特定的数据结构进行存储和管理，以支持高效的排序和筛选操作。在业务场景下，为了从大量Region Proposals中筛选出最有可能包含目标的区域，需要根据每个RP的置信度score进行排序。

<!-- npu="A3,910b" id1 -->
以Atlas A3 训练系列产品/Atlas A3 推理系列产品和Atlas A2 训练系列产品/Atlas A2 推理系列产品为例：

Region Proposal支持两种数据类型：half/float。

下图展示了n个Region Proposal的结构示意图，每组\[x1， x2， y1，y2，score，label\]代表一个完整的Region Proposal。其中，x1、x2、y1、y2表示候选框的坐标，score为置信度，label为目标的类别标签。

**图1**  Region Proposal结构示意图

![](../../../../figures/region_proposal.png)<a id="图1-Region Proposal结构示意图"></a>

排序组合接口用于实现（score, index）对的排序操作，其中index用于索引完整的RP数据，数据结构请参考[Sort32排序示意图](Sort32.md#图1-Sort32排序示意图)。其中，[Sort32接口](Sort32.md)实现一次迭代内对32个（score, index）对的降序排列。[MrgSort接口](MrgSort.md)将多个（最多4个）已经排好序的子队列，归并成一个全局有序的队列。
<!-- end id1 -->
