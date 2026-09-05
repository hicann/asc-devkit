# Tensor API Sample Introduction

## Overview

This directory contains samples that directly include `tensor_api/tensor.h` and use the Tensor API programming model. The directory is organized around Tensor construction, layout processing, matrix computation, and experimental APIs. Each category directory maintains an index of its specific samples.

## Directory Plan

| Directory Name | Description |
| --- | --- |
| [tensor](./tensor) | Contains samples for fundamental capabilities such as Tensor construction, access, and slicing. |
| [layout](./layout) | Contains samples related to layout construction, transformation, and layout patterns. |
| [matrix_compute](./matrix_compute) | Contains matrix computation samples covering matrix multiplication, batch matrix multiplication, convolution, data copy-in and copy-out, and high-performance MxFP4 matrix multiplication. |
| [experimental/reg_vector_compute](./experimental/reg_vector_compute) | Contains experimental Tensor API samples, currently including vector computation samples based on the register Tensor interfaces. |
