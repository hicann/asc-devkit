# GroupBarrier Example

## Overview

This example implements correct synchronization between two groups of AIVs with dependencies. After Group A AIVs complete computation, Group B AIVs depend on the computation results of Group A AIVs for subsequent computation. Group A is called the Arrive group, and Group B is called the Wait group.

> **Notice:** This example is only applicable to the TPipe and TQue programming model.

## Supported Products and CANN Versions

| Product | CANN Version |
|---------|-------------|
| Ascend 950PR/Ascend 950DT | >= CANN 9.1.0 |
| Atlas A3 Training Series Products/Atlas A3 Inference Series Products | >= CANN 9.0.0 |
| Atlas A2 Training Series Products/Atlas A2 Inference Series Products | >= CANN 9.0.0 |

## Directory Structure

```
├── group_barrier
│   ├── CMakeLists.txt          // Build project file
│   ├── group_barrier.asc       // Ascend C example implementation & invocation example
│   └── README.md               // Example documentation
```

## Example Description

- Example function:
  The GroupBarrier example implements correct synchronization between two groups of AIVs with dependencies. After Group A AIVs complete computation, Group B AIVs depend on the computation results of Group A AIVs for subsequent computation. Group A is called the Arrive group, and Group B is called the Wait group. This example does not perform input/output computation. Instead, after the Arrive group writes specified values, the Wait group reads those values and prints the correct values via printf to complete verification.

- Example specifications:
  <table>
  <tr><td rowspan="2" align="center">Example Input</td><td align="center">name</td><td align="center">shape</td><td align="center">data type</td><td align="center">format</td></tr>
  <tr><td align="center">barGm</td><td align="center">[3072]</td><td align="center">uint8_t</td><td align="center">ND</td></tr>
  <tr><td rowspan="1" align="center">Example Output</td><td align="center">z</td><td align="center">[8]</td><td align="center">int32_t</td><td align="center">ND</td></tr>
  <tr><td rowspan="1" align="center">Kernel Function Name</td><td colspan="4" align="center">kernel_group_barrier</td></tr>
  </table>

- Example implementation:

  The GroupBarrier example enables 8 AIV cores. 2 AIV cores serve as the Arrive group, enabling atomic accumulation, writing specified values to Global Memory, and calling the Arrive instruction. The remaining 6 AIV cores first call the Wait instruction to wait for the Arrive group to complete writing, then read from Global Memory and print the results via printf.

## Build and Run

Run the following steps in the root directory of this example to build and run the example.

- Configure environment variables
  Configure environment variables according to the [installation method](../../../../../docs/en/quick_start.md#prepare&install) of the CANN development kit in the current environment.
  ```bash
  source ${install_path}/cann/set_env.sh
  ```

  > **Note:** `${install_path}` is the CANN package installation directory. When no installation directory is specified, the default installation path is `/usr/local/Ascend`.

- Run the example

  Run the following commands in the example directory.
  ```bash
  mkdir -p build && cd build;      # Create and enter the build directory
  cmake .. -DCMAKE_ASC_ARCHITECTURES=dav-2201;make -j;    # Build the project
  ./demo                           # Run the compiled executable to execute the example
  ```

  To use NPU simulation mode, add the `-DCMAKE_ASC_RUN_MODE=sim` parameter.

  Example:

  ```bash
  cmake .. -DCMAKE_ASC_RUN_MODE=sim -DCMAKE_ASC_ARCHITECTURES=dav-2201;make -j; # NPU simulation mode
  ```

  > **Notice:** Clear the cmake cache before switching build modes. Run `rm CMakeCache.txt` in the build directory and then re-run cmake.

- Build option description

  | Option | Values | Description |
  |--------|--------|-------------|
  | `CMAKE_ASC_RUN_MODE` | `npu` (default), `sim` | Run mode: NPU execution, NPU simulation |
  | `CMAKE_ASC_ARCHITECTURES` | `dav-2201` (default), `dav-3510` |  NPU architecture: dav-2201 corresponds to Atlas A2 Training Series Products/Atlas A2 Inference Series Products and Atlas A3 Training Series Products/Atlas A3 Inference Series Products, dav-3510 corresponds to Ascend 950PR/Ascend 950DT |

- Execution result

  The following execution result indicates that the example executed successfully.

  ```bash
  [Block (0/6)]: OUTPUT = 24
  [Block (1/6)]: OUTPUT = 24
  [Block (2/6)]: OUTPUT = 24
  [Block (3/6)]: OUTPUT = 24
  [Block (4/6)]: OUTPUT = 24
  [Block (5/6)]: OUTPUT = 24
  ```
