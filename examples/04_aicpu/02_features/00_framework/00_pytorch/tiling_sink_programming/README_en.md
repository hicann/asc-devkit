# Custom Operator Tiling Sink Graph Mode Invocation Example

## Overview

This example, based on a sample custom operator project, introduces how to call custom operators in PyTorch graph mode and optimize scheduling performance by enabling Tiling sink to the device side.

## Supported Products and CANN Software Versions

| Product | CANN Software Version |
|------|-------------|
| Ascend 950PR/Ascend 950DT | >= CANN 9.1.0 |
| Atlas A3 Training Series Products/Atlas A3 Inference Series Products | >= CANN 9.0.0 |
| Atlas A2 Training Series Products/Atlas A2 Inference Series Products | >= CANN 9.0.0 |

## Directory Structure

```
├── tiling_sink_programming
│   ├── test_add_custom_tiling_sink.py   // Tiling sink invocation test script in PyTorch graph mode
│   └── README.md                        // Example documentation
```

## Code Implementation Introduction

The example script `test_add_custom_tiling_sink.py` contains three key parts:
1. Register the custom operator in PyTorch and provide placeholder implementations for Meta/CPU/PrivateUse1 to ensure graph compatibility.
2. Register an FX-to-GE converter to map `add_custom_tiling_sink` to the GE-side custom operator `AddCustomTilingSink`.
3. Enable graph mode execution through `torch.compile` and turn on the `tiling_schedule_optimize` configuration.

## Build and Run

- Install PyTorch and Ascend Extension for PyTorch plugin

  Refer to the [pytorch: Ascend Extension for PyTorch](https://gitcode.com/Ascend/pytorch) open source repository or the [Ascend Extension for PyTorch Ascend Community](https://hiascend.com/document/redirect/Pytorch-index) installation instructions. Select a compatible `Python` version distribution and complete the installation of `torch` and `torch-npu`.

- Build, package, and deploy the custom operator project

  Before running this example, first enter the [custom operator project example](../../../../../01_simd_cpp_api/02_features/99_acl_based/00_acl_compilation/custom_op) directory to complete building, packaging, and deployment.

- Install Prerequisites

  ```bash
  pip3 install expecttest
  ```

- Configure Environment Variables

  Select the corresponding command to configure environment variables based on the [installation method](../../../../../../docs/en/quick_start.md#prepare&install) of the CANN development kit in the current environment.
  - Default path, root user installs the CANN package
    ```bash
    source /usr/local/Ascend/cann/set_env.sh
    ```
  - Default path, non-root user installs the CANN package
    ```bash
    source $HOME/Ascend/cann/set_env.sh
    ```
  - Specified path install_path, install the CANN package
    ```bash
    source ${install_path}/cann/set_env.sh
    ```

  > **Note:** `${install_path}` is the CANN package installation directory. When no installation directory is specified, the default installation path is `/usr/local/Ascend`.

- Run the Example

  Run the following steps in the root directory of this example to run the example.

  ```bash
  python3 test_add_custom_tiling_sink.py
  ```

  The following output indicates that the accuracy verification is successful.

  ```bash
  Ran 1 test in **s
  OK
  ```
