# HelloWorld Operator Direct Call Sample

## Overview

This sample demonstrates the basic process of verifying operator kernel function execution on the NPU side using the <<<>>> kernel call operator. The kernel function prints output results through printf.

## Supported Products

- Ascend 950PR/Ascend 950DT
- Atlas A3 Training Series Products/Atlas A3 Inference Series Products
- Atlas A2 Training Series Products/Atlas A2 Inference Series Products

## Directory Structure

```
├── hello_world_npu
│   ├── CMakeLists.txt      // Build project file
│   └── hello_world.asc     // Ascend C operator implementation and call sample
```

## Building and Running

Execute the following steps in the root directory of this sample to compile and run the operator.
- Configure environment variables  
  Please select the corresponding command to configure environment variables according to the [installation method](../../../../../docs/en/quick_start.md#prepare&install) of the CANN development kit package on the current environment.
  - Default path, root user installs CANN software package
    ```bash
    source /usr/local/Ascend/cann/set_env.sh
    ```

  - Default path, non-root user installs CANN software package
    ```bash
    source $HOME/Ascend/cann/set_env.sh
    ```

  - Specified path install_path, install CANN software package
    ```bash
    source ${install_path}/cann/set_env.sh
    ```

- Sample execution
  ```bash
  mkdir -p build && cd build;   # Create and enter build directory
  cmake ..;make -j;             # Build project
  ./demo                        # Execute sample
  ```
  The execution result is as follows, indicating successful execution.
  ```bash
  [Block (0/8)]: Hello World!!!
  [Block (1/8)]: Hello World!!!
  [Block (2/8)]: Hello World!!!
  [Block (3/8)]: Hello World!!!
  [Block (4/8)]: Hello World!!!
  [Block (5/8)]: Hello World!!!
  [Block (6/8)]: Hello World!!!
  [Block (7/8)]: Hello World!!!
  ```