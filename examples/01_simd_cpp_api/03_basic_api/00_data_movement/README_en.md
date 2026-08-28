# Data Movement API Sample Introduction

## Overview

This directory contains samples for multiple APIs related to data movement. Each sample uses Ascend C's `<<<>>>` direct invocation method, supporting implementation of main function and kernel function in the same .asc file.

## Sample List

| Directory Name | Function Description | Supported Products |
| ----------------------------------------------------------| -----------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------| --- |
| [broadcast_ub2l0c](./broadcast_ub2l0c) |  This sample implements data broadcast movement based on BroadCastVecToMM, broadcasting data located on UB (Unified Buffer) and moving it to L0C Buffer | Atlas Inference Series Products AI Core |
| [copy_ub2ub](./copy_ub2ub) |  This sample implements data movement based on Copy, applicable for data movement between VECIN and VECOUT, supporting mask continuous mode and counter mode | Ascend 950PR/Ascend 950DT<br>Atlas A3 Training Series Products/Atlas A3 Inference Series Products<br>Atlas A2 Training Series Products/Atlas A2 Inference Series Products |
| [data_copy_gm2l1](./data_copy_gm2l1) |  This sample introduces how to use DataCopy to move input data from GM (Global Memory) to L1 (L1 Buffer) | Ascend 950PR/Ascend 950DT<br>Atlas A3 Training Series Products/Atlas A3 Inference Series Products<br>Atlas A2 Training Series Products/Atlas A2 Inference Series Products |
| [data_copy_gm2ub_nddma](./data_copy_gm2ub_nddma) |  This sample introduces how to use multi-dimensional data movement interface to implement data movement from GM (Global Memory) to UB (Unified Buffer) pathway. By freely configuring dimension information and corresponding Stride, it can be used for Padding, Transpose, BroadCast, Slice and other data transformation operations | Ascend 950PR/Ascend 950DT |
| [data_copy_gm2ub_slice](./data_copy_gm2ub_slice) |  This sample implements data slice movement based on DataCopy, extracting subsets of multi-dimensional Tensor data for movement between GM (Global Memory) and UB (Unified Buffer) pathways | Ascend 950PR/Ascend 950DT<br>Atlas A3 Training Series Products/Atlas A3 Inference Series Products<br>Atlas A2 Training Series Products/Atlas A2 Inference Series Products |
| [data_copy_l0c2gm](./data_copy_l0c2gm) |  This sample implements data inline quantization activation movement based on DataCopy in convolution scenarios | Ascend 950PR/Ascend 950DT<br>Atlas A3 Training Series Products/Atlas A3 Inference Series Products<br>Atlas A2 Training Series Products/Atlas A2 Inference Series Products |
| [data_copy_pad_gm2ub_ub2gm](./data_copy_pad_gm2ub_ub2gm) |  This sample implements non-32-byte aligned data movement based on DataCopyPad, with data padding | Ascend 950PR/Ascend 950DT<br>Atlas A3 Training Series Products/Atlas A3 Inference Series Products<br>Atlas A2 Training Series Products/Atlas A2 Inference Series Products |
| [data_copy_ub2l1](./data_copy_ub2l1) |  This sample implements data movement from UB (Unified Buffer) to L1 (L1 Buffer) based on DataCopy in Mmad matrix multiplication scenarios | Ascend 950PR/Ascend 950DT |
| [reg_auxscalar](./reg_auxscalar) |  This sample demonstrates using AuxScalar method to read multiple scalar data from UB for computation | Ascend 950PR/Ascend 950DT |
| [reg_load_gather](./reg_load_gather) |  This sample demonstrates using Gather interface to implement discrete data load, including high-dimensional Gather (source is LocalTensor) and Reg::GatherB (collect by DataBlock) scenarios | Ascend 950PR/Ascend 950DT |
| [reg_load_store_align](./reg_load_store_align) |  This sample implements aligned data movement operations (continuous and non-continuous) from UB (Unified Buffer) to RegTensor (Reg vector computation basic unit) using Reg programming interface | Ascend 950PR/Ascend 950DT |
| [reg_load_store_mask](./reg_load_store_mask) |  This sample implements UB (Unified Buffer) load/store operations to MaskReg (mask register) using Reg programming interface, and mask-based masked store operations | Ascend 950PR/Ascend 950DT |
| [reg_load_store_unalign](./reg_load_store_unalign) |  This sample implements unaligned data movement operations from UB (Unified Buffer) to RegTensor (Reg vector computation basic unit) using Reg programming interface | Ascend 950PR/Ascend 950DT |
| [reg_move](./reg_move) |  This sample demonstrates using the MaskGenWithRegTensor interface to move mask data from RegTensor to MaskReg | Ascend 950PR/Ascend 950DT |
| [reg_store_scatter](./reg_store_scatter) |  This sample demonstrates using Reg::Scatter interface to implement discrete data store (scatter elements to UB) | Ascend 950PR/Ascend 950DT |
