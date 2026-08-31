# Data Movement C API Example Introduction

## Overview

This directory contains Ascend C C API examples for multiple data-movement interfaces. Each example uses Ascend C C API's `<<<>>>` direct invocation method, with the main function and kernel function implemented in the same `.asc` file.

## Example List

| Directory Name | Function Description | Supported Products |
| --- | --- | --- |
| [data_copy_gm2l1](./data_copy_gm2l1) | This example demonstrates moving input matrix data from GM (Global Memory) to L1 (L1 Buffer), including format conversion, matrix multiplication, and Fixpipe quantized-result copy-out. | Ascend 950PR/Ascend 950DT |
| [data_copy_l0c2gm](./data_copy_l0c2gm) | This example demonstrates using `asc_copy_l0c2gm` to move matrix multiplication results from L0C (L0C Buffer) to GM (Global Memory), while performing scalar and Vector quantization, ReLU activation, and Nz-to-ND conversion during the transfer. | Ascend 950PR/Ascend 950DT |
| [data_copy_l0c2l1](./data_copy_l0c2l1) | This example demonstrates using `asc_copy_l0c2l1` to move matrix multiplication results from L0C (L0C Buffer) to L1 (L1 Buffer), while performing scalar and Vector quantization, ReLU activation, and Nz-to-ND conversion during the transfer. | Ascend 950PR/Ascend 950DT |
| [data_copy_l0c2ub](./data_copy_l0c2ub) | This example demonstrates using `asc_copy_l0c2ub` to move matrix multiplication results from L0C (L0C Buffer) to UB (Unified Buffer), while performing scalar and Vector quantization, ReLU activation, and Nz-to-ND conversion during the transfer. | Ascend 950PR/Ascend 950DT |
| [reg_load_gather](./reg_load_gather) | This example demonstrates non-contiguous data loads, including element-indexed `asc_gather` and DataBlock-indexed `asc_gather_datablock` scenarios. | Ascend 950PR/Ascend 950DT |
| [reg_load_store_align](./reg_load_store_align) | This example demonstrates contiguous and non-contiguous aligned data movement between UB (Unified Buffer) and vector registers with Reg vector computation interfaces. | Ascend 950PR/Ascend 950DT |
| [reg_load_store_mask](./reg_load_store_mask) | This example demonstrates mask-register load and store between UB (Unified Buffer) and mask registers with Reg vector computation interfaces, as well as mask-register-based data selection. | Ascend 950PR/Ascend 950DT |
