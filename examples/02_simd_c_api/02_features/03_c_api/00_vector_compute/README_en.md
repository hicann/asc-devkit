# C-API Sample Introduction

## Overview

This sample demonstrates the compilation process for building Vector operator samples using C_API, supporting implementation of the main function and kernel function in the same cpp file.

## Sample List

| Directory Name | Description |
| -------------- | ----------- |
| [00_sync_add](./00_sync_add) | This sample demonstrates the compilation process for building Add operator samples using C_API based on synchronous data movement and computation interfaces, supporting implementation of the main function and kernel function in the same cpp file |
| [01_async_add](./01_async_add) | This sample demonstrates the compilation process for building Add operator samples using C_API based on asynchronous data movement, computation interfaces, and unified synchronization interfaces, supporting implementation of the main function and kernel function in the same cpp file |
| [02_c_api_delicacy_async_add](./02_c_api_delicacy_async_add) | This sample demonstrates the Add operator sample written using C_API interfaces, implemented based on asynchronous data movement, computation interfaces, and fine-grained synchronization interfaces |
| [03_add_double_buffer](./03_add_double_buffer) | An Add performance sample that compares a single-buffer baseline with Ping-Pong double buffering under identical computation specifications to demonstrate MTE2, Vector, and MTE3 pipeline overlap | 
