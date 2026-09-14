# Dump Samples

## Overview

This sample shows how to use the Ascend C asc_dump interface series and the simd_vf asc_dump interfaces.

## Sample List

| Directory | Description | Supported Products |
| --- | --- | --- |
| [simple_dump](./simple_dump) | Uses static Tensor programming mode to implement matrix multiplication, showing basic usage of the `asc_dump_gm`, `asc_dump_l1buf`, `asc_dump_cbuf`, and `asc_dump_ubuf` interfaces | Ascend 950PR/Ascend 950DT<br>Atlas A3 Training Series/Atlas A3 Inference Series<br>Atlas A2 Training Series/Atlas A2 Inference Series |
| [simd_vf_dump](./simd_vf_dump) | Uses vector programming mode, showing basic usage of the simd_vf asc_dump_ubuf/asc_dump/asc_dump_reg interfaces | Ascend 950PR/Ascend 950DT |
| [cube_buffer_dump](./cube_buffer_dump) | Uses HiFloat8 quantized MatMul as the context and DumpTensor to print the HiFloat8 input matrix, bias, and per-channel scale from L1 Buffer, Bias Table Buffer, and Fixpipe Buffer | Ascend 950PR/Ascend 950DT |
