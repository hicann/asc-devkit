# Data Filter Operator Example Introduction

## Overview

This example set introduces typical usage of different features of data filter operators and provides corresponding end-to-end implementations.

## Example List

| Directory Name | Description | Supported Products |
| ------------------------------------------------------------ | ---------------------------------------------------- | --- |
| [dropout](./dropout) |  This example demonstrates how to call the DropOut high-level API to implement the dropout operator, providing the functionality to filter SrcTensor (source operand, input Tensor) based on MaskTensor to obtain DstTensor (destination operand, output Tensor) | Ascend 950PR&950DT products<br>Atlas A3 products<br>Atlas A2 products |
| [select](./select) |  This example is based on the Kernel direct call operator project and demonstrates how to call the Select high-level API to implement the SelectCustom single operator, mainly demonstrating the usage of the Select high-level API in the Kernel direct call project | Ascend 950PR&950DT products<br>Atlas A3 products<br>Atlas A2 products |