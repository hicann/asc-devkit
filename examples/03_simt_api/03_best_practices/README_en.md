# BestPractices Example Introduction

## Overview

Operator development practices based on Ascend C SIMT, demonstrating the application of SIMT features in actual operator development.

## Example List

| Directory Name                                              | Description                                                                                        |
| ------------------------------------------------------ | ----------------------------------------------------------------------------------------------- |
| [00_memory_optimizations](./00_memory_optimizations)   | Introduce memory access optimization methods based on SIMT programming, including global memory access coalescing, on-chip cache reuse, bank conflict avoidance, and short vector tuning. |
| [01_execution_conf_optimizations](./01_execution_conf_optimizations)   | Introduces optimization methods for maximum thread block configuration settings within execution configuration under SIMT programming model. |
| [02_control_flow](./02_control_flow)   | Introduces the impact of Warp Divergence on performance and optimization methods in SIMT programming. |
| [03_instruction_optimizations](./03_instruction_optimizations) | A histogram counting case study demonstrating atomic operation performance optimization techniques. |