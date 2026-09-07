# Cache Control API Samples

## Overview

This directory contains samples related to cache control APIs, demonstrating how to improve cache hit rate and reduce memory access latency by preloading data and adjusting data access order. The samples are based on Ascend C's `<<<>>>` direct invocation method, supporting implementation of both the main function and kernel function in the same file.

## Sample List

| Directory Name | Description | Supported Products |
| ------- | -------- | --- |
| [data_cache_preload](./data_cache_preload) |  This sample demonstrates how to use the DataCachePreload interface in an indirect index (random-hopping) access scenario, preloading upcoming Cache Lines into DCache while the Scalar unit reads scattered data from GM to reduce Scalar memory access wait time. | Ascend 950PR/Ascend 950DT<br>Atlas A3 Training Series Products/Atlas A3 Inference Series Products<br>Atlas A2 Training Series Products/Atlas A2 Inference Series Products |
| [add_l2_cache_split](./add_l2_cache_split) |  This sample compares no-split and L2-batch traversal orders for a fixed in-place Add workload, demonstrating how to improve the L2 Cache hit rate for repeated data access across iterations. | Ascend 950PR/Ascend 950DT<br>Atlas A3 Training Series Products/Atlas A3 Inference Series Products<br>Atlas A2 Training Series Products/Atlas A2 Inference Series Products |
