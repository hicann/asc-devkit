# Atomic Operation Sample Introduction

## Overview

This document presents basic samples based on Ascend C programming, demonstrating the usage of SIMT atomic operation interfaces.

## Sample List

|  Directory Name                                                   |  Description                                              |
| ------------------------------------------------------------ | ---------------------------------------------------- |
 | [atomic_add_perf](./atomic_add_perf) | Taking [`asc_atomic_add()`](../../../../../docs/zh/api/SIMT-API/atomic_operations/asc_atomic_add.md) as an example, uses five groups of controlled experiments to show how the memory tier, atomic-operation organization, return-value usage, and data type affect atomic-add performance, with optimization guidance.|
| [atomic_intrinsics](./atomic_intrinsics) | Demonstrates the usage of Ascend C SIMT atomic operation interfaces (add, sub, exchange, compare-and-swap, increment, decrement, bitwise AND/OR/XOR, etc.).|
