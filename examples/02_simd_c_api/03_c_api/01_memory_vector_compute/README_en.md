# Memory Vector Compute C API Examples

## Overview

This directory contains multiple C API examples for Memory vector compute. Each example uses Ascend C `<<<>>>` direct invocation and supports implementing the `main` function and `kernel` function in the same `.asc` file.

## Operator Development Examples

| Directory Name | Description |
| --- | --- |
| [cast](./cast) | This example implements data type conversion from half to int4b_t or int32_t based on C API programming interfaces, primarily calling `asc_half2int4` and `asc_half2int32` |
| [compare](./compare) | This example implements data comparison based on C API programming interfaces, primarily calling `asc_lt`, `asc_gt_scalar`, and `asc_get_cmp_mask` |
| [fused_compute](./fused_compute) | This example implements the Leaky ReLU fused operation based on C API programming interfaces, primarily calling `asc_leakyrelu` |
| [reduce](./reduce) | This example implements vector sum reduction based on C API programming interfaces, primarily calling `asc_repeat_reduce_sum` and `asc_datablock_reduce_sum` |
