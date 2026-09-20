# Instruction Optimizations Sample Introduction

## Overview

Instruction optimization samples, implemented through direct `<<<>>>` invocation, introduce instruction tuning approaches based on SIMT programming. Currently, two kinds of cases are provided: atomic operation optimization and in-place add instruction optimization, demonstrating optimization methods such as hierarchical reduction and instruction cost control to improve instruction execution efficiency.

## Sample List

| Directory Name                                              | Description                                                                                        |
| ------------------------------------------------------ | ----------------------------------------------------------------------------------------------- |
| [atomic_histogram](./atomic_histogram)   | Using histogram counting as an example, this sample compares GM global atomic accumulation with UB block-local atomic accumulation followed by merge, demonstrating tuning methods and performance gains for atomic operation instructions. |
| [inplace_add_atomic](./inplace_add_atomic)   | Using in-place add (three-operand multiply-add and two-operand add) as an example, this sample compares in-kernel read-modify-write with atomic add, demonstrating the tuning method and performance gain of using atomic operations to offload accumulation to the memory side and eliminate one main-memory read. |