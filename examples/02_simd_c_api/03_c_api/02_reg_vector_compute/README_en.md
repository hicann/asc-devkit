# Vector Compute API Examples

## Overview

This directory contains examples for multiple APIs related to vector computation. Each example is based on Ascend C's `<<<>>>` direct invocation method, supporting both main function and kernel function implementation in the same asc file.

## Operator Development Examples

| Directory Name | Description |
| -------------- | ----------- |
| [abs](./abs) | This example implements the Abs operation based on the C API programming interface, primarily calling the asc_abs API. The asc_relu/asc_exp/asc_sqrt/asc_ln/asc_neg APIs can all refer to this example |
| [arange](./arange) | This example implements the Arange operation based on the C API programming interface, primarily calling the asc_arange API, generating increasing/decreasing index sequences starting from a scalar value |
| [cast](./cast) | This example implements the Cast operation based on the C API programming interface, primarily calling the asc_half2int32_rd/asc_float2int16_rna_sat API for data type conversion (widening/narrowing) |
| [compare](./compare) | This example implements data comparison functionality across multiple scenarios using the asc_gt and asc_gt_scalar APIs based on the C API programming interface |
| [data_relayout](./data_relayout) | This example implements data relayout functionality based on the C API programming interface, supporting Interleave (asc_intlv) and Pack (asc_pack_to_low) scenarios |
| [div](./div) | This example implements the Div operation based on the C API programming interface, primarily calling the asc_div interface |
| [duplicate](./duplicate) | This example implements the Duplicate operation (scalar fill mode) based on the C API programming interface, primarily calling the asc_duplicate_scalar API, copying a scalar value multiple times and filling it into a vector |
| [gather](./gather) | This example implements the functionality of collecting elements from a vector data register by index based on the C API programming interface, primarily calling the asc_gather API |
| [histogram](./histogram) | This example implements histogram statistics functionality based on the C API programming interface, using the asc_frequency_histogram_bin0/asc_frequency_histogram_bin1 API |
| [mergemode](./mergemode) | This example demonstrates data movement in MERGING mode based on the C API programming interface, using the asc_copy API to move src into active mask bits while preserving the original dst value at inactive bits |
| [mul](./mul) | This example implements element-wise multiplication based on the C API programming interface, primarily calling the asc_mul API |
| [muls](./muls) | This example implements the Muls operation based on the C API programming interface, primarily calling the asc_mul_scalar API. The asc_add_scalar/asc_max_scalar/asc_min_scalar APIs can all refer to this example |
| [reduce](./reduce) | This example implements the Reduce operation based on the C API programming interface, primarily calling the asc_reduce_sum API (SUM mode), supporting SUM/MAX/MIN reduction modes |
| [reduce_block](./reduce_block) | This example implements the ReduceDataBlock operation based on the C API programming interface, primarily calling the asc_reduce_sum_datablock API (SUM mode), performing reduction within each DataBlock (32B) |
| [reduce_pair](./reduce_pair) | This example implements the PairReduceElem operation based on the C API programming interface, primarily calling the asc_pair_reduce_sum API (SUM mode), performing reduction sum on adjacent odd-even element pairs |
| [select](./select) | This example implements the Select operation based on the C API programming interface, primarily calling the asc_select API, selecting elements between two vectors based on mask |
| [squeeze](./squeeze) | This example implements the Squeeze operation based on the C API programming interface, primarily calling the asc_squeeze API, compressing a vector to a scalar |
| [truncate](./truncate) | This example implements the Truncate operation based on the C API programming interface, truncating floating-point numbers in a vector data register to integer values. It primarily calls the asc_floor API |
