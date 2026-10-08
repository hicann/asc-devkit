# Memory Vector Compute Operator Examples

## Overview

This example set introduces typical usage of different features of Memory vector compute operators, providing end-to-end implementations.

## Example List

| Directory Name | Description | Supported Products |
|----------------------------------------------------------------------------------------------------------------------|-----------------------------------------------------------------------------| --- |
| [brcb](./brcb) |  This example implements data padding using Brcb, which can be used to fill 8 numbers from the input tensor into 8 datablocks of the result tensor each time | Ascend 950PR&950DT products<br>Atlas A3 products<br>Atlas A2 products |
| [cast](./cast) |  This example implements data type and precision conversion between source and destination tensors using Cast | Ascend 950PR&950DT products<br>Atlas A3 products<br>Atlas A2 products |
| [compare](./compare) |  This example implements data comparison functionality in multiple scenarios using Compare and Compares interfaces, performing element-wise comparison. | Ascend 950PR&950DT products<br>Atlas A3 products<br>Atlas A2 products |
| [create_vec_index](./create_vec_index) |  This example demonstrates the method of creating a vector index with a specified starting value using CreateVecIndex | Ascend 950PR&950DT products<br>Atlas A3 products<br>Atlas A2 products |
| [duplicate](./duplicate) |  This example implements data padding using Duplicate, which can be used to copy a variable or immediate value multiple times and fill it into a vector | Ascend 950PR&950DT products<br>Atlas A3 products<br>Atlas A2 products |
| [element_wise_arithmetic](./element_wise_arithmetic) |  This example demonstrates the usage of basic arithmetic interfaces based on LeakyRelu | Ascend 950PR&950DT products<br>Atlas A3 products<br>Atlas A2 products |
| [element_wise_compound_compute](./element_wise_compound_compute) |  This example introduces the usage of Ascend C vector compute compound interfaces | Ascend 950PR&950DT products<br>Atlas A3 products<br>Atlas A2 products |
| [element_wise_logic](./element_wise_logic) |  This example implements bitwise logic operations using And, Ors, ShiftLeft, ShiftRight interfaces | Ascend 950PR&950DT products<br>Atlas A3 products<br>Atlas A2 products |
| [gather](./gather) |  This example implements data selection functionality in multiple scenarios using GatherMask, Gather, Gatherb and other interfaces, selecting elements from source operands and writing them to destination operands. | Ascend 950PR&950DT products<br>Atlas A3 products<br>Atlas A2 products |
| [interleave_pair](./interleave_pair) |  This example implements element interleaving and de-interleaving functionality using Interleave and DeInterleave interfaces | Ascend 950PR&950DT products |
| [mrg_sort](./mrg_sort) |  This example implements merging up to 4 pre-sorted queues into 1 queue using Sort32 and MrgSort basic APIs, with results sorted by score field in descending order | Ascend 950PR&950DT products<br>Atlas A3 products<br>Atlas A2 products |
| [reduce](./reduce) |  This example demonstrates how to call the `ReduceRepeat<SUM>` and `ReduceDataBlock<SUM>` basic APIs to implement the reduce operator, achieving cumulative sum of data elements on continuous memory and returning the cumulative result | Ascend 950PR&950DT products<br>Atlas A3 products<br>Atlas A2 products |
| [reduce_data_block](./reduce_data_block) |  This example implements reduction computation using `ReduceDataBlock` | Ascend 950PR&950DT products<br>Atlas A3 products<br>Atlas A2 products |
| [reduce_repeat](./reduce_repeat) |  This example introduces the usage of reduction interfaces `ReduceRepeat` in multiple scenarios. | Ascend 950PR&950DT products<br>Atlas A3 products<br>Atlas A2 products |
| [reduce_pair_elem](./reduce_pair_elem) |  This example implements sum reduction for adjacent odd-even element pairs using `ReducePairElem<SUM>` | Ascend 950PR&950DT products<br>Atlas A3 products<br>Atlas A2 products |
| [reduce_computation](./reduce_computation) |  This example implements reduction computation using `ReduceMax`/`ReduceMin`/`ReduceSum` interfaces | Ascend 950PR&950DT products<br>Atlas A3 products<br>Atlas A2 products |
| [region_proposal_sort](./region_proposal_sort) |  This example introduces the usage of Region Proposal related sorting interfaces | Atlas Inference Series Products AI Core |
| [select](./select) |  This example implements data selection functionality in multiple scenarios using the Select interface, selecting elements from two vectors or between a vector and a scalar based on a mask and writing them to a destination vector | Ascend 950PR&950DT products<br>Atlas A3 products<br>Atlas A2 products |
| [transpose](./transpose) |  This example implements data transpose functionality using Transpose and TransDataTo5HD interfaces, including 16*16 2D matrix block transpose, conversion between [N,C,H,W] and [N,H,W,C] 4D matrices, and NCHW format to NC1HWC0 format conversion. | Ascend 950PR&950DT products<br>Atlas A3 products<br>Atlas A2 products |
