# Basic API Sample Introduction

## Overview

This sample introduces the usage of Ascend C Basic API, including data movement, memory vector computation, Reg vector computation, matrix computation, resource management, synchronization control, atomic operations, TPipe/TQue, and utility APIs.

## Sample List

| Directory Name  | Description |
| --------- | --------- |
| [00_data_movement](./00_data_movement) | This directory contains samples for multiple APIs related to data movement, demonstrating the usage of data movement interfaces.|
| [01_memory_vector_compute](./01_memory_vector_compute) | This sample directory uses operators such as reduce, sort, and transpose as examples to demonstrate the usage of memory vector computation interfaces.|
| [02_reg_vector_compute](./02_reg_vector_compute) | This directory contains samples for multiple APIs related to vector computation, implemented based on Reg programming interfaces.|
| [03_matrix_compute](./03_matrix_compute) | This sample directory uses operators such as batch_matmul to demonstrate the usage of matrix computation interfaces.|
| [04_memory_management](./04_memory_management) | This directory contains samples for multiple APIs related to resource management. Each sample is based on the Ascend C `<<<>>>` direct invocation method, supporting the implementation of both the main function and kernel function in the same cpp file.         |
| [05_sync_control](./05_sync_control) | This directory contains samples for multiple APIs related to synchronization control. Each sample is based on the Ascend C `<<<>>>` direct invocation method, supporting the implementation of both the main function and kernel function in the same cpp file.         |
| [06_atomic](./06_atomic) | This directory contains samples for multiple APIs related to atomic operations. Each sample is based on the Ascend C `<<<>>>` direct invocation method, supporting the implementation of both the main function and kernel function in the same cpp file.         |
| [07_tpipe_tque](./07_tpipe_tque) | This directory contains samples for multiple APIs related to TPipe/TQue resource management. Each sample is based on the Ascend C `<<<>>>` direct invocation method, supporting the implementation of both the main function and kernel function in the same cpp file.         |
| [09_utils](./09_utils) | This directory contains API samples related to utilities. Each sample is based on the Ascend C `<<<>>>` direct invocation method, supporting the implementation of both the main function and kernel function in the same cpp file.         |
| [10_add_l2_cache_split](./10_add_l2_cache_split) | This sample compares no-split and 96MB L2-batch traversal orders for a fixed 384MB in-place Add workload, demonstrating how to improve the L2 Cache hit rate for repeated data access.|
