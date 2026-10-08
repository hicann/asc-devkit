# AOT Compilation Example

## Overview

A performance practice example based on AOT (Ahead-of-Time) compilation optimization. By converting runtime Tiling parameters into compile-time constants, the compiler can perform more aggressive optimizations such as loop unrolling, constant propagation, and dead code elimination, thereby automatically matching pre-compiled specialized versions at runtime to achieve performance improvements.

## Example List

| Directory Name | Description | Supported Products |
| --- | --- | --- |
| [matmul_aot_compilation](./matmul_aot_compilation) | Introduces AOT compilation optimization on top of the Matmul high-level API, demonstrating the complete workflow of Tiling constant definition, AOT Registry registration, and runtime dispatch (AOTDispatcher). At runtime, it automatically matches AOT-specialized versions or falls back to the generic version based on Tiling parameters. | Ascend 950PR&950DT products<br>Atlas A3 products<br>Atlas A2 products |
