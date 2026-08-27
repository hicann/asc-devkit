# Cooperative Groups Sample Introduction

## Overview

This document presents basic samples based on Ascend C programming, demonstrating the basic usage of SIMT [cooperative groups](../../../../../docs/zh/api/SIMT-API/cooperative_groups/cooperative_groups_intro.md).

## Sample List

|  Directory Name                                                   |  Description                                              |
| ------------------------------------------------------------ | ---------------------------------------------------- |
| [reduce_sum](./reduce_sum) | Taking the reduction-sum operator as an example, implements the same reduction logic with cooperative groups at grid, thread block, and warp granularity, showing the advantages of cooperative groups in code abstraction and reuse, and comparing the performance of the three granularities.|
