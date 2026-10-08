# Normalization API Sample Introduction

## Overview

This sample set introduces typical usage of different normalization operation characteristics, providing corresponding end-to-end implementations.

## Sample List

| Directory Name | Function Description | Supported Products |
| ------------------------------------------------------------ | ---------------------------------------------------- | --- |
| [deepnorm](./deepnorm) |  This sample is based on Kernel direct call sample project, introducing calling DeepNorm high-level API to implement deepnorm single sample. During deep neural network training, when executing layer LayerNorm normalization, can use DeepNorm as replacement, improving Transformer stability through enlarged residual connection | Ascend 950PR&950DT products<br>Atlas A3 products<br>Atlas A2 products |
| [layernorm](./layernorm) |  This sample is based on Kernel direct call sample project, introducing continuously calling LayerNorm, LayerNormGrad, LayerNormGradBeta three high-level APIs in one kernel function, implementing LayerNorm complete forward and backward propagation | Ascend 950PR&950DT products<br>Atlas A3 products<br>Atlas A2 products |
| [layernorm_v2](./layernorm_v2) |  This sample is based on Kernel direct call sample project, introducing continuously calling LayerNorm and Normalize high-level APIs in one kernel function, both used together to implement complete normalization computation | Ascend 950PR&950DT products<br>Atlas A3 products<br>Atlas A2 products |
| [rmsnorm](./rmsnorm) |  This sample is based on Kernel direct call sample project, introducing calling RmsNorm high-level API to implement rmsnorm single sample, implementing RmsNorm normalization on input data with shape size [B, S, H] | Ascend 950PR&950DT products<br>Atlas A3 products<br>Atlas A2 products |
| [welford](./welford) |  This sample is based on Kernel direct call sample project, introducing continuously calling WelfordUpdate and WelfordFinalize high-level APIs in one kernel function, implementing complete Welford online algorithm, used for online computation of mean and variance | Ascend 950PR&950DT products<br>Atlas A3 products<br>Atlas A2 products |