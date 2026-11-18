# Deterministic Computation Sample Introduction

## Overview

This sample is based on Ascend C programming and demonstrates deterministic computation in SIMT.

## Sample List

|  Directory Name                                                   |  Description                                              |
| ------------------------------------------------------------ | ---------------------------------------------------- |
| [deterministic_reduce](./deterministic_reduce) | A comparison sample for reduction sum implemented using SIMT programming. It demonstrates the non-determinism of floating-point atomic accumulation and the determinism of two fixed-order reductions: the industry-standard halving reduction and the [`asc_shfl_down`](../../../../../docs/zh/api/SIMT-API/Warp_functions/Warp_shfl_functions/asc_shfl_down.md) butterfly merge tree (measured to be faster). It also performs bit-by-bit verification by recomputing the golden result on the Host in the same order.|
