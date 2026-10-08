# Resource Management API Examples Introduction

## Overview

This directory contains examples for multiple APIs related to resource management. Each example is based on the Ascend C <<<>>> direct call method, supporting implementation of both the main function and kernel function in the same .asc file.

## Example List

| Directory Name | Description | Supported Products |
| ----------------------------------------------------------- | --------------------------------------------------- | --- |
| [tbufpool_management](./tbufpool_management) |  This example implements TBufPool memory resource management based on TPipe::InitBufPool and TBufPool::InitBufPool interfaces, demonstrating usage patterns such as TBufPool resource allocation, memory partitioning, memory reuse, and custom TBufPool. | Ascend 950PR&950DT products<br>Atlas A3 products<br>Atlas A2 products |
| [list_tensor_desc_input](./list_tensor_desc_input) |  This example implements an AddN example based on the static Tensor programming model, using the ListTensorDesc structure to handle dynamic input parameters, combined with static memory allocation and event synchronization mechanisms to achieve coordinated scheduling of data transfer and computation tasks. | Ascend 950PR&950DT products<br>Atlas A3 products<br>Atlas A2 products |
| [tensor_allocation](./tensor_allocation) |  This Sample introduces the allocation and usage methods of GlobalTensor、LocalTensor and RegTensor. | Ascend 950PR&950DT products<br>Atlas A3 products<br>Atlas A2 products |
| [tmp_buffer](./tmp_buffer) |  This example initializes TBuf memory space based on the TPipe::InitBuffer interface and uses TBuf temporary buffer for data conversion during computation, implementing a vector addition (Add) example with bfloat16_t data type. | Ascend 950PR&950DT products<br>Atlas A3 products<br>Atlas A2 products |
