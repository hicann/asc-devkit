# Compatibility Guide Example Introduction

## Overview

This section provides several examples for features that have compatibility differences among Atlas A2 Training Series Products/Atlas A2 Inference Series Products, Atlas A3 Training Series Products/Atlas A3 Inference Series Products, and Ascend 950PR/Ascend 950DT. Users can migrate and adapt according to the examples.
This section uses the `<<<>>>` kernel call operator to complete the basic flow of running operator kernel functions on the NPU side, providing corresponding end-to-end implementations.

## Example List

| Directory Name | Description | Supported Products |
| -------------------------------------------------- | ---------------------------------------------------- | --- |
| [data_copy_l1togm](./data_copy_l1togm) |  This example demonstrates the end-to-end flow of transferring L1 data to GM. | Ascend 950PR/Ascend 950DT<br>Atlas A3 Training Series Products/Atlas A3 Inference Series Products<br>Atlas A2 Training Series Products/Atlas A2 Inference Series Products |
| [fill](./fill) |  This example shows how to use the Fill interface to initialize L0A Buffer and L0B Buffer. | Ascend 950PR/Ascend 950DT<br>Atlas A3 Training Series Products/Atlas A3 Inference Series Products<br>Atlas A2 Training Series Products/Atlas A2 Inference Series Products |
| [fixpipe_params_switch](./fixpipe_params_switch) |  This example demonstrates the parameter structure switch for the Fixpipe interface across 2201/3510 architectures. | Ascend 950PR/Ascend 950DT<br>Atlas A3 Training Series Products/Atlas A3 Inference Series Products<br>Atlas A2 Training Series Products/Atlas A2 Inference Series Products |
| [matmul_s4](./matmul_s4) |  This example demonstrates the end-to-end implementation of int4 matrix multiplication computation. | Ascend 950PR/Ascend 950DT<br>Atlas A3 Training Series Products/Atlas A3 Inference Series Products<br>Atlas A2 Training Series Products/Atlas A2 Inference Series Products |
| [pattern_transformation](./pattern_transformation) |  This example demonstrates the fractal transformation logic for L1 Buffer -> L0A Buffer pathway based on the basic mmad example. | Ascend 950PR/Ascend 950DT<br>Atlas A3 Training Series Products/Atlas A3 Inference Series Products<br>Atlas A2 Training Series Products/Atlas A2 Inference Series Products |
| [scatter](./scatter) |  This example demonstrates the scatter function, which can scatter the input tensor to the result tensor based on the input tensor and destination address offset tensor. | Ascend 950PR/Ascend 950DT<br>Atlas A3 Training Series Products/Atlas A3 Inference Series Products<br>Atlas A2 Training Series Products/Atlas A2 Inference Series Products |
| [set_loaddata_boundary](./set_loaddata_boundary) |  This example implements the setting of L1 Buffer boundary values. | Ascend 950PR/Ascend 950DT<br>Atlas A3 Training Series Products/Atlas A3 Inference Series Products<br>Atlas A2 Training Series Products/Atlas A2 Inference Series Products |
| [subnormal](./subnormal) |  This example uses the Ln interface to demonstrate the Subnormal computation mode compatibility adaptation of vector compute interfaces. | Ascend 950PR/Ascend 950DT<br>Atlas A3 Training Series Products/Atlas A3 Inference Series Products<br>Atlas A2 Training Series Products/Atlas A2 Inference Series Products |
