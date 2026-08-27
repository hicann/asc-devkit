# Memory Access Sample Introduction

## Overview

This document presents memory access samples based on Ascend C SIMT, covering scenarios such as Global Memory discrete access, cross-core data synchronization, and UB subbank access in SIMT programming.

## Sample List

| Directory Name | Description |
| --- | --- |
| [insert_hash_table](./insert_hash_table) | This example implements the InsertHashTable operator based on Ascend C SIMT programming, demonstrating the capability of SIMT operators to handle complex business logic. |
| [bank_conflict](./bank_conflict) | This example constructs multiple Warp-level UB subbank access patterns in Ascend C SIMT, demonstrating same-address merge, cross-bank distribution, and subbank conflicts. |
