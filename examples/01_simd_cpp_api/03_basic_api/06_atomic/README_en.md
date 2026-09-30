# Atomic Operations API Sample Introduction

## Overview

This directory contains API samples related to atomic operations. The samples are based on the Ascend C `<<<>>>` direct invocation method, supporting the implementation of both the main function and kernel function in the same file.

## Sample List

| Directory Name | Description | Supported Products |
|--------------------------------------| ------------------------------------------------- | --- |
| [data_movement_with_atomic_operations](./data_movement_with_atomic_operations) |  This sample demonstrates the implementation flow of atomic accumulation and atomic maximum comparison when moving data from VECOUT to GM, based on the SetAtomicAdd and SetAtomicMax atomic operation interfaces. It supports scenarios where multiple cores read the same or different inputs. | Ascend 950PR/Ascend 950DT<br>Atlas A3 Training Series Products/Atlas A3 Inference Series Products<br>Atlas A2 Training Series Products/Atlas A2 Inference Series Products |
| [scalar_atomic_operations](./scalar_atomic_operations) |  This sample demonstrates the implementation flow of scalar atomic addition and atomic compare-and-swap on GM addresses, based on the AtomicAdd and AtomicCas interfaces. It supports parallel updates of a single memory address by multiple cores. | Ascend 950PR/Ascend 950DT |
| [set_atomic_deterministic_computation](./set_atomic_deterministic_computation) |  This sample first introduces the necessity and specific implementation approach for deterministic computation in scenarios involving data movement with accompanying atomic operations. It then describes how to apply this approach in single-AIV core, multi-AIV core, and multi-AIC core scenarios. | Ascend 950PR/Ascend 950DT<br>Atlas A3 Training Series Products/Atlas A3 Inference Series Products<br>Atlas A2 Training Series Products/Atlas A2 Inference Series Products |
