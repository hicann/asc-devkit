# Reg Vector Compute Practices Example Introduction

## Overview

VF-based performance optimization examples using the `<<<>>>` kernel direct invocation operator, introducing VF fusion optimization, loop splitting optimization, and loop unrolling optimization methods.

## Example List
| Directory Name | Description | Supported Products |
| -------------------------------------------------- | ---------------------------------------------------- | --- |
| [gelu_eltwise_high_performance](./gelu_eltwise_high_performance) |  This example uses Gelu+Element-wise computation to introduce RegBase vector performance tuning methods, demonstrating performance gains after parallelism adjustment, loop splitting, and loop unrolling. | Ascend 950PR/Ascend 950DT |
| [gelu_high_performance](./gelu_high_performance) |  This example uses Gelu computation to introduce RegBase vector performance tuning methods, demonstrating performance gains after enabling VF fusion and loop unrolling. | Ascend 950PR/Ascend 950DT |
| [softmax_high_performance](./softmax_high_performance) |  This example uses single-core Softmax as a case study to demonstrate a complete performance tuning path from MemBase to RegBase, and from basic loops to optimizations such as loop fusion and loop unrolling. | Ascend 950PR/Ascend 950DT |
