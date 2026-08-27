# Execution Configuration Optimizations Sample Introduction

## Overview

Operator execution configuration optimization introduces `gridDim`/`blockDim` related configurations in the SIMT programming model through direct invocation using `<<<>>>`. Currently, optimization cases for setting the maximum thread count (`sincos_compute`) are provided to fully utilize registers and optimize operator execution performance; as well as thread block count configuration strategy samples (`grid_dim_config`), demonstrating thread block configuration optimization methods under different data size scenarios.

## Sample List

| Directory Name                                        | Description                                                                                                                                      |
| ----------------------------------------------------- | ------------------------------------------------------------------------------------------------------------------------------------------------ |
| [sincos_compute](./max_thread_config/sincos_compute) | This example uses sincos computation as an example to compare performance differences between using default values and 512 for \_\_launch_bounds__, demonstrating the impact of register spilling on performance in SIMT programming and methods for configuring maximum thread count. |
| [grid_dim_config](./grid_dim_config)   | Using the Gather operator as an example, demonstrate the impact of different thread block configuration strategies on operator performance under different data size scenarios and provide corresponding optimization guidance. |
