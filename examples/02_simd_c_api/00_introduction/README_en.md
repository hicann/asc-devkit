# Introduction Sample Overview

## Summary

Simple examples based on Ascend C, demonstrating custom operator implementations through Ascend C programming language with corresponding C_API implementations.

## Sample List

|  Directory Name                                                   |  Description                                              |
| ------------------------------------------------------------ | ---------------------------------------------------- |
| [00_quickstart](./00_quickstart) | This sample demonstrates the kernel function direct call method for the HelloWorld operator based on Ascend C, verifying the operator kernel function from the NPU side, showing the overall flow from invocation to execution |
| [01_add](./01_add) | This sample demonstrates the kernel function direct call method for the Add custom Vector operator based on Ascend C, implementing element-wise addition of two input tensors, supporting main function and kernel function implementation in the same cpp file |
| [02_matrix](./02_matrix) | Basic matrix multiplication examples implemented with the SIMD C API. |
| [04_reg_base_add_compute](./04_reg_base_add_compute) | This sample demonstrates the kernel function direct call method for the Add operator based on Ascend C (RegBase scenario), implementing element-wise addition of two input tensors through C_API, showing the vector computation flow at on-chip storage and register levels. |
