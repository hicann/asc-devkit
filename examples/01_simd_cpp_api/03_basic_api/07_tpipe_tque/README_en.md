# TPipe/TQue Resource Management API Sample Introduction

## Overview

This directory contains examples for multiple APIs related to TPipe/TQue resource management. Each example is based on the Ascend C <<<>>> direct call method and demonstrates TPipe/TQue-related interfaces.

## Example List

| Directory Name | Description | Supported Products |
| ----------------------------------------------------------- | --------------------------------------------------- | --- |
| [get_tpipe_ptr](./get_tpipe_ptr) |  This example obtains the global TPipe pointer based on GetTPipePtr, allowing the kernel function to perform TPipe-related operations without explicitly passing the TPipe pointer. | Ascend 950PR&950DT products<br>Atlas A3 products<br>Atlas A2 products |
| [tpipe_reuse](./tpipe_reuse) |  This example implements TPipe repeated allocation and usage based on TPipe::Init and TPipe::Destroy. | Ascend 950PR&950DT products<br>Atlas A3 products<br>Atlas A2 products |
