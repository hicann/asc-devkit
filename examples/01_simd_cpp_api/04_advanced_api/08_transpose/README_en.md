# Tensor Transformation Operator Examples

## Overview

This example set introduces typical usage of different features for tensor transformation operators and provides corresponding end-to-end implementations.

## Example List

| Directory Name | Description | Supported Products |
| ------------------------------------------------------------ | ---------------------------------------------------- | --- |
| [add_broadcast](./add_broadcast) |  This example introduces the kernel function direct call method for the Add operator, adding input Broadcast in multi-core and tiling scenarios | Ascend 950PR&950DT products<br>Atlas A3 products<br>Atlas A2 products |
| [broadcast](./broadcast) |  This example is based on the Kernel direct call operator project, demonstrating how to call the BroadCast high-level API to implement the broadcast single operator, mainly demonstrating the BroadCast high-level API call in the Kernel direct call project | Ascend 950PR&950DT products<br>Atlas A3 products<br>Atlas A2 products |
| [fill](./fill) |  This example introduces how to call Fill to initialize data on Global Memory to 0 | Ascend 950PR&950DT products<br>Atlas A3 products<br>Atlas A2 products |
| [pad](./pad) |  This example is based on the Kernel direct call operator project, demonstrating how to call the Pad high-level API to implement the pad single operator, padding a height * width 2D Tensor to 32B alignment in the width direction. If the Tensor width is already 32B aligned and all data is valid, calling this interface for alignment is not supported | Ascend 950PR&950DT products<br>Atlas A3 products<br>Atlas A2 products |
| [transdata](./transdata) |  This example demonstrates an operator implementation based on the TransData high-level API. The example converts the input data layout format to the target layout format | Ascend 950PR&950DT products<br>Atlas A3 products<br>Atlas A2 products |
| [transpose](./transpose) |  This example introduces how to call the Transpose high-level API to implement the Transpose operator, and provides corresponding end-to-end implementations using the kernel function direct call method | Ascend 950PR&950DT products<br>Atlas A3 products<br>Atlas A2 products |
| [unpad](./unpad) |  This example is based on the Kernel direct call operator project, demonstrating how to call the UnPad high-level API to implement the unpad single operator, unpadding a height * width 2D Tensor to 32B alignment in the width direction. If the Tensor width is already 32B aligned and all data is valid, calling this interface for alignment is not supported | Ascend 950PR&950DT products<br>Atlas A3 products<br>Atlas A2 products |