# Memory Access Practices Example Introduction

## Overview

This section contains optimization examples based on data transfer APIs. Through the `<<<>>>` direct call implementation, it introduces methods such as reducing invalid data transfers and reducing the number of transfer instructions.

## Example List

| Directory Name | Description | Supported Products |
| ------------------------------------------------------------ | ---------------------------------------------------- | --- |
| [bank_conflict_nd2nz](./bank_conflict_nd2nz) |  This example introduces how to avoid bank conflict by adjusting the UB write stride during ND matrix to NZ layout conversion on Atlas A2/A3 series products and Ascend 950PR&950DT products, and provides a kernel direct invocation method. | Ascend 950PR&950DT products<br>Atlas A3 products<br>Atlas A2 products |
| [bank_conflict_ub](./bank_conflict_ub) |  This example is based on the Add instruction. By configuring different LocalTensor addresses, it verifies and analyzes the impact of bank conflicts and address overlaps in UB (Unified Buffer) on performance. The example uses the compile-time parameter SCENARIO_NUM to select different address configuration scenarios, covering typical cases such as no conflict, read-read conflict, read-write conflict, and complete address overlap. | Atlas A3 products<br>Atlas A2 products |
| [data_copy](./data_copy) |  This example introduces data transfer practices from Global Memory to UB and from Global Memory to L1, comparing the impact of tile granularity, unaligned data transfer, L2Cache reuse, and same-address access conflict avoidance on DataCopy/DataCopyPad performance. | Ascend 950PR&950DT products<br>Atlas A3 products<br>Atlas A2 products |
