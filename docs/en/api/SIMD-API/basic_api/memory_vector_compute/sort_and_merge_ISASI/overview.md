# Overview

<!-- md-trans-meta sourceCommit=754fb95859dead826727a591e674261c6a829a08 translatedAt=2026-10-26T10:48:45.528Z pushedAt=2026-10-28T03:14:39.584Z nl=2 -->

Region Proposal (RP for short) is a candidate box used in object detection algorithms to locate potential target regions. Region Proposal is stored and managed using a specific data structure to support efficient sorting and filtering operations. In business scenarios, to filter out the regions most likely to contain targets from a large number of Region Proposals, sorting is required based on the confidence score of each RP.

<!-- npu="A3,910b" id1 -->
For Atlas A3 products and Atlas A2 products:

Region Proposal supports two data types: half/float.

The following figure shows the structure of n Region Proposals. Each group \[x1, x2, y1, y2, score, label\] represents a complete Region Proposal. Here, x1, x2, y1, and y2 represent the coordinates of the candidate box, score is the confidence, and label is the category label of the target.

**Figure 1**  Region Proposal structure diagram

![](../../../../figures/region_proposal.png)<a id="Figure 1-Region Proposal structure diagram"></a>

The sorting combination API implements the sorting operation on (score, index) pairs, where index is used to index the complete RP data. For the data structure, see [Sort32 sorting diagram](Sort32.md#figure-1-sort32-sorting-diagram). The [Sort32 API](Sort32.md) implements descending sorting of 32 (score, index) pairs within one iteration. The [MrgSort API](MrgSort.md) merges multiple (up to 4) already-sorted sub-queues into a globally ordered queue.
<!-- end id1 -->
