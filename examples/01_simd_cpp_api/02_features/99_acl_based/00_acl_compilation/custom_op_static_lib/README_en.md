# Custom Operator Static Library

## Overview

This example uses `AddCustom` to demonstrate how to compile, package, and link a custom operator static library, and execute the operator via aclnn.

## Supported Products and CANN Versions

| Product | CANN Version |
|---------|-------------|
| Ascend 950PR/Ascend 950DT | >= CANN 9.1.0 |
| Atlas A3 Training Series Products/Atlas A3 Inference Series Products | >= CANN 9.0.0 |
| Atlas A2 Training Series Products/Atlas A2 Inference Series Products | >= CANN 9.0.0 |

## Directory Structure

```
├── custom_op_static_lib
│   ├── CMakeLists.txt            // Top-level build project file
│   ├── app                       // Links the static library and executes the operator
│   ├── op                        // Compiles and packages the custom operator static library
│   └── README.md                 // Example documentation
```

## Code Implementation

In this example, the `op` directory is responsible for compiling and packaging the custom operator static library. The generated static library path is `./build/customize-install/lib/lib${package_name}.a`. The `app` directory imports the static library via `find_package(${package_name})`, compiles `main.cpp` to generate `execute_add_op`, calls `aclnnAddCustom`, and verifies the results.

For the AddCustom operator description and related information, refer to [Operator Description](../custom_op/README_en.md).

## Build and Run

- Configure environment variables

  Configure environment variables based on the [installation method](../../../../../../docs/en/quick_start.md#prepare&install) of the CANN development kit in the current environment. Ensure that `ASCEND_HOME_PATH` points to the CANN installation root path so that header file and library paths take effect.
  ```bash
  source ${install_path}/cann/set_env.sh
  ```

  > **Note:** `${install_path}` is the CANN package installation directory. When no installation directory is specified, the default installation path is `/usr/local/Ascend`.

- Run the example

  Run the following steps in the root directory of this example.

  ```bash
  mkdir -p build && cd build
  python3 ../app/scripts/gen_data.py
  cmake .. && make -j
  ./execute_add_op
  ```

  The following output indicates successful execution.

  ```text
  test pass
  ```
