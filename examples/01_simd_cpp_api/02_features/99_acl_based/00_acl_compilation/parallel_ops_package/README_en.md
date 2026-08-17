# Custom Operator Project Multi-Vendor Parallel Build, Packaging, and Deployment Example

## Overview

This example demonstrates how to use `ExternalProject_Add` in a top-level CMake project to build two independent custom operator projects in parallel:
- `add_custom` (AddCustom)
- `leaky_relu_custom` (LeakyReluCustom)

The `add_custom` project uses a flat directory layout (host/kernel/tiling source files at the same level), while `leaky_relu_custom` maintains a hierarchical directory structure.

Each sub-project independently completes custom operator compilation, packaging, and generates a standalone `custom_opp_*.run` installation package.

## Supported Products and CANN Versions

| Product | CANN Version |
|---------|-------------|
| Ascend 950PR/Ascend 950DT | >= CANN 9.1.0 |
| Atlas A3 Training Series Products/Atlas A3 Inference Series Products | >= CANN 9.0.0 |
| Atlas A2 Training Series Products/Atlas A2 Inference Series Products | >= CANN 9.0.0 |
| Atlas 200I/500 A2 Inference Products | >= CANN 9.0.0 |
| Atlas Inference Series Products | >= CANN 9.0.0 |

> Notice: This example involves multiple operator examples. Refer to the actual product models supported by each operator example.

## Directory Structure

```
├── parallel_ops_package
│   ├── CMakeLists.txt              // Top-level build project file (parallel build of multiple operator packages)
│   ├── add_custom                  // Add operator project directory
│   ├── leaky_relu_custom           // LeakyRelu operator project directory
│   │   ├── CMakeLists.txt          // LeakyRelu operator build project file
│   │   ├── framework               // Framework adaptation plugin directory
│   │   ├── op_host                 // Host-side operator implementation directory
│   │   └── op_kernel               // Kernel-side operator implementation directory
│   └── README.md                   // Example documentation
```

## Example Description

`parallel_ops_package` uses the same Add/LeakyRelu example description as `custom_op`. Refer to:

- [custom_op/README_en.md "Example Description" section](../custom_op/README_en.md#example-description)

## Example Specification Description

`parallel_ops_package` uses the same Add/LeakyRelu specification description as `custom_op`. Refer to:

- [custom_op/README_en.md "Example Specification Description" section](../custom_op/README_en.md#example-specification-description)

## Code Implementation

The Add/LeakyRelu code implementation for `parallel_ops_package` can be found in the `custom_op` documentation.

- [custom_op/README_en.md "Code Implementation" section](../custom_op/README_en.md#code-implementation)

## Build and Run

Run the following steps in the root directory of this example to build, package, and deploy the custom operator packages.

- Configure environment variables

  Configure environment variables based on the [installation method](../../../../../../docs/en/quick_start.md#prepare&install) of the CANN development kit in the current environment.
  ```bash
  source ${install_path}/cann/set_env.sh
  ```

  > **Note:** `${install_path}` is the CANN package installation directory. When no installation directory is specified, the default installation path is `/usr/local/Ascend`.

- Build, package, and deploy both operator packages

  ```bash
  cmake -S . -B build
  cmake --build build -j
  # add_custom package
  ./build/add_custom/custom_opp_*.run

  # leaky_relu_custom package
  ./build/leaky_relu_custom/custom_opp_*.run
  ```

  The following output indicates successful execution.

  ```text
  SUCCESS
  ```

## Build Result Description

The top-level project generates two subdirectories under `build/`:
- `build/add_custom/`: AddCustom intermediate artifacts and installation package
- `build/leaky_relu_custom/`: LeakyReluCustom intermediate artifacts and installation package

These two directories are independent, facilitating parallel development and release in multi-vendor scenarios.
