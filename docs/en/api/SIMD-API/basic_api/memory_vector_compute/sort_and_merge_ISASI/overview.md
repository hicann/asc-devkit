# Overview

<!-- md-trans-meta sourceCommit=e3c8c70b3f5d27f3770063ae4b535bc75d904e3e translatedAt=2026-09-09T15:09:49.182Z -->

Region Proposal (RP) is a candidate box used in object detection algorithms to locate potential target regions. Region Proposal uses a specific data structure for storage and management to support efficient sorting and filtering operations. In business scenarios, to filter out the regions most likely to contain targets from a large number of Region Proposals, the proposals need to be sorted by the confidence score of each RP.

<!-- npu="A3,910b" id1 -->
Take Atlas A3 training products/Atlas A3 inference products and Atlas A2 training products/Atlas A2 inference products as an example:

Region Proposal supports two data types: half/float.

The following figure shows the structure of n Region Proposals. Each group of \[x1, x2, y1, y2, score, label\] represents a complete Region Proposal. Among them, x1, x2, y1, and y2 represent the coordinates of the candidate box, score is the confidence, and label is the category label of the target.

**Figure 1** Region Proposal structure

![](../../../../figures/region_proposal.png)<a id="fig1-region-proposal-structure"></a>

The sort combination API is used to implement the sorting of (score, index) pairs, where index is used to index the complete RP data. For the data structure, see [Sort32 sorting diagram](Sort32.md#figure-1-sort32-sorting-diagram). The [Sort32 API](Sort32.md) implements the descending sorting of 32 (score, index) pairs in one iteration. The [MrgSort API](MrgSort.md) merges multiple (up to 4) sorted subqueues into a globally ordered queue.
<!-- end id1 -->
