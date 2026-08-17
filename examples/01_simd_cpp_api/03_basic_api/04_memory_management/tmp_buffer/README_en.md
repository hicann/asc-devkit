# VectorAdd Example Using TmpBuf

## Overview

This example initializes TBuf memory space based on the TPipe::InitBuffer interface and uses the TBuf temporary buffer for data conversion during computation, implementing a vector addition (Add) example with bfloat16_t data type.

> **Notice:** This example is only applicable to the TPipe and TQue programming model.

## Supported Products and CANN Versions

| Product | CANN Version |
|---------|-------------|
| Ascend 950PR/Ascend 950DT | >= CANN 9.1.0 |
| Atlas A3 Training Series Products/Atlas A3 Inference Series Products | >= CANN 9.0.0 |
| Atlas A2 Training Series Products/Atlas A2 Inference Series Products | >= CANN 9.0.0 |

## Directory Structure

```
├── tmp_buffer
│   ├── scripts
│   │   ├── gen_data.py         // Script for generating input data and ground truth data
│   │   └── verify_result.py    // Script for verifying whether output data matches ground truth data
│   ├── CMakeLists.txt          // Build project file
│   ├── data_utils.h            // Data read/write functions
│   ├── tmp_buffer.asc          // Ascend C example implementation & invocation example
│   └── README.md               // Example documentation
```

## Example Description

- Example function

  This example calls the Cast interface to convert bfloat16_t input data to float type and stores it in the TBuf temporary buffer. After completing the Add computation, it calls the Cast interface again to convert back to bfloat16_t type.

- Example specifications

  <table>
    <tr><td rowspan="1" align="center">Example Type (OpType)</td><td colspan="4" align="center">Add</td></tr>
    <tr><td rowspan="3" align="center">Example Input</td><td align="center">name</td><td align="center">shape</td><td align="center">data type</td><td align="center">format</td></tr>
    <tr><td align="center">x</td><td align="center">[1, 2048]</td><td align="center">bfloat16_t</td><td align="center">ND</td></tr>
    <tr><td align="center">y</td><td align="center">[1, 2048]</td><td align="center">bfloat16_t</td><td align="center">ND</td></tr>
    <tr><td rowspan="1" align="center">Example Output</td><td align="center">z</td><td align="center">[1, 2048]</td><td align="center">bfloat16_t</td><td align="center">ND</td></tr>
    <tr><td rowspan="1" align="center">Kernel Function Name</td><td colspan="4" align="center">tmp_buffer_custom</td></tr>
  </table>

- Example implementation

  - Kernel implementation

    - Call the TPipe::InitBuffer interface to allocate memory space for TQue and TBuf, where TBuf is used to store temporary data.

    - Call the DataCopy basic API to transfer data from GM (Global Memory) to UB (Unified Buffer).

    - Call the Cast interface to convert bfloat16_t data to float type and store it in the TBuf temporary buffer.

    - Call the Add interface to perform addition on two input tensors.

    - Call the Cast interface to convert float computation results to bfloat16_t type and store them in the UB (Unified Buffer) space allocated by TQue.

    - Call the DataCopy basic API to transfer computation results from UB (Unified Buffer) to GM (Global Memory).

  - Invocation implementation

    Use the kernel invocation operator <<<>>> to call the kernel function.

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
  cmake -DCMAKE_ASC_ARCHITECTURES=dav-2201 ..;make -j;    # Build the project, default npu mode
  python3 ../scripts/gen_data.py   # Generate test input data
  ./demo                           # Run the compiled executable to execute the example
  python3 ../scripts/verify_result.py output/output.bin output/golden.bin   # Verify whether the output result is correct
  ```

  To use CPU debug or NPU simulation mode, add the `-DCMAKE_ASC_RUN_MODE=cpu` or `-DCMAKE_ASC_RUN_MODE=sim` parameter.
  
  Examples:
  ```bash
  cmake -DCMAKE_ASC_RUN_MODE=cpu -DCMAKE_ASC_ARCHITECTURES=dav-2201 ..;make -j; # CPU debug mode
  cmake -DCMAKE_ASC_RUN_MODE=sim -DCMAKE_ASC_ARCHITECTURES=dav-2201 ..;make -j; # NPU simulation mode
  ```

  > **Notice:** Clear the cmake cache before switching build modes. Run `rm CMakeCache.txt` in the build directory and then re-run cmake.

- Build option description

  | Option | Values | Description |
  |--------|--------|-------------|
  | `CMAKE_ASC_RUN_MODE` | `npu` (default), `cpu`, `sim` | Run mode: NPU execution, CPU debug, NPU simulation |
  | `CMAKE_ASC_ARCHITECTURES` | `dav-2201` (default), `dav-3510` | NPU architecture: dav-2201 corresponds to Atlas A2 Training Series Products/Atlas A2 Inference Series Products and Atlas A3 Training Series Products/Atlas A3 Inference Series Products, dav-3510 corresponds to Ascend 950PR/Ascend 950DT |

- Execution result

  The following execution result indicates that the accuracy comparison is successful:
  ```bash
  test pass!
  ```
