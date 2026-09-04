# Utilities API Samples

## Overview

This directory contains API samples related to utilities. The samples are based on Ascend C's `<<<>>>` direct invocation method, supporting implementation of both main function and kernel function in the same file.

## Sample List

| Directory Name | Description | Supported Products |
| ------- | -------- | --- |
| [ctrl_spr](./ctrl_spr) |  This sample demonstrates setting, reading, and resetting specific bit fields of the CTRL register (control register) using SetCtrlSpr, GetCtrlSpr, and ResetCtrlSpr interfaces, and verifies whether non-saturation mode is working correctly. | Ascend 950PR/Ascend 950DT |
| [get_ub_size](./get_ub_size) |  This sample demonstrates the usage of GetUBSizeInBytes and GetRuntimeUBSize interfaces to get the maximum user-available UB size. | Ascend 950PR/Ascend 950DT |
| [gm_by_dcache](./gm_by_dcache) |  This sample demonstrates reading data from and writing data to GM with going through DCache. | Ascend 950PR/Ascend 950DT<br>Atlas A3 Training Series Products/Atlas A3 Inference Series Products<br>Atlas A2 Training Series Products/Atlas A2 Inference Series Products |
| [gm_by_pass_dcache](./gm_by_pass_dcache) |  This sample demonstrates reading data from and writing data to GM without going through DCache, based on the ReadGmBypassDCache and WriteGmBypassDCache interfaces. | Ascend 950PR/Ascend 950DT<br>Atlas A3 Training Series Products/Atlas A3 Inference Series Products<br>Atlas A2 Training Series Products/Atlas A2 Inference Series Products |
