# type_traits Type Traits Direct Invocation Example
## Overview
This example implements type_traits type traits use cases using the Ascend C programming language. The type traits APIs include is_void/remove_const/add_const and others. The <<<>>> kernel invocation operator is used to complete the basic workflow of running and verifying the operator kernel function on the NPU side, providing a corresponding end-to-end implementation.

## Supported Products and CANN Software Versions

| Product | CANN Software Version |
|------|-------------|
| Ascend 950PR/Ascend 950DT | >= CANN 9.1.0 |
| Atlas A3 Training Series Products/Atlas A3 Inference Series Products | >= CANN 9.0.0 |
| Atlas A2 Training Series Products/Atlas A2 Inference Series Products | >= CANN 9.0.0 |

## Directory Structure
```
├── type_traits
│   ├── CMakeLists.txt          // Build project file
│   ├── data_utils.h            // Data read/write functions
│   ├── type_traits.asc         // Ascend C operator implementation & invocation example
│   └── README.md               // Example documentation
```

## Function Description
- type_traits type traits APIs, including is_void/is_integral/is_floating_point/is_array/is_pointer/is_reference/is_const/remove_const/remove_volatile/remove_cv/remove_reference/remove_pointer/add_const/add_volatile/add_cv/add_pointer/add_lvalue_reference/add_rvalue_reference, can be used for type checking, adding, removing, and converting during program compilation.

- Operator implementation:  
  - Kernel implementation  
    Ascend C calls each API, prints the API return values, and performs ascendc_assert checks.   

  - Invocation implementation  
    Use the kernel invocation operator <<<>>> to call the kernel function.

## Build and Run

Run the following steps in the root directory of this example to build and run the operator.

- Configure environment variables
  Configure environment variables based on the [installation method](../../../../../docs/en/quick_start.md#prepare&install) of the CANN development kit in the current environment.
  ```bash
  source ${install_path}/cann/set_env.sh
  ```

  > **Note:** `${install_path}` is the CANN package installation directory. When no installation directory is specified, the default installation path is `/usr/local/Ascend`.

- Run the example

  Run the following commands in the example directory.
  ```bash
  mkdir -p build && cd build;
  cmake .. -DCMAKE_ASC_ARCHITECTURES=dav-2201;make -j; # NPU mode by default
  ./demo
  ```

  To use CPU debugging or NPU simulation mode, add the `-DCMAKE_ASC_RUN_MODE=cpu` or `-DCMAKE_ASC_RUN_MODE=sim` parameter.

  For example:
  ```bash
  cmake -DCMAKE_ASC_RUN_MODE=cpu -DCMAKE_ASC_ARCHITECTURES=dav-2201 ..;make -j; # CPU debugging mode
  cmake -DCMAKE_ASC_RUN_MODE=sim -DCMAKE_ASC_ARCHITECTURES=dav-2201 ..;make -j; # NPU simulation mode
  ```

  > **Notice:** Before switching the build mode, clear the cmake cache by running `rm CMakeCache.txt` in the build directory and then re-run cmake.

- Build option description

  | Option | Values | Description |
  |------|--------|------|
  | `CMAKE_ASC_RUN_MODE` | `npu` (default), `cpu`, `sim` | Run mode: NPU run, CPU debug, NPU simulation |
  | `CMAKE_ASC_ARCHITECTURES` | `dav-2201` (default), `dav-3510` | NPU architecture: dav-2201 corresponds to Atlas A2 Training Series Products/Atlas A2 Inference Series Products and Atlas A3 Training Series Products/Atlas A3 Inference Series Products; dav-3510 corresponds to Ascend 950PR/Ascend 950DT |

  The following execution result indicates that execution is successful.
  ```bash
  test pass!
  ```
