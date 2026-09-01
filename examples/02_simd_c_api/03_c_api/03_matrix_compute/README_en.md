# Matrix Compute API Sample Introduction

## Overview

This sample set introduces typical usage of different features of matrix computation APIs, providing corresponding end-to-end implementations. Samples in the directory are named according to "API_name_pathway", as follows:
1. **load_data**: Samples starting with "load_data" introduce related APIs for L1 Buffer -> L0 Buffer;
2. **mmad**: Samples starting with "mmad" introduce the Mmad and MmadMx matrix multiplication APIs;
3. **fixpipe**: Samples starting with "fixpipe" introduce related APIs for L0C Buffer → Global Memory/L1 Buffer/Unified Buffer;

## Sample List

| Directory Name | Function Description | Supported Products |
|--------------------------------------------------------------------------------------------| ---------------------------------------------------- | --- |
| [load_data_l12l0_950](./load_data_l12l0_950) |  This example demonstrates how to call `asc_copy_l12l0a` / `asc_copy_l12l0b` and their `_transpose` variants to transfer A / B matrices from L1 to L0A / L0B Buffer | Ascend 950PR/Ascend 950DT |
| [mmad](./mmad) |  This example uses int8_t and bfloat16 as two input data types to demonstrate how to implement matrix multiplication (C = A x B + Bias) through the C_API | Ascend 950PR/Ascend 950DT |
| [mmad_mx](./mmad_mx) | This example demonstrates MX matrix multiplication with quantization through the C API. Matrices A and B use FP4 or FP8 data types, while ScaleA and ScaleB use fp8_e8m0_t. It covers three scenarios: initializing C to zero, initializing C from the BiasTable Buffer, and accumulating from L0C. It also describes K-axis alignment and FP8 tail clearing | Ascend 950PR/Ascend 950DT |
