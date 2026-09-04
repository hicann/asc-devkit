# DataSyncBarrier Example

## Overview

This example introduces the invocation of DataSyncBarrier. This interface blocks the execution of subsequent instructions until all preceding memory access instructions (the memory locations to wait for can be controlled through parameters) have completed.

## Supported Products and CANN Versions

| Product | CANN Version |
|---------|-------------|
| Ascend 950PR/Ascend 950DT | >= CANN 9.1.0 |
| Atlas A3 Training Series Products/Atlas A3 Inference Series Products | >= CANN 9.0.0 |
| Atlas A2 Training Series Products/Atlas A2 Inference Series Products | >= CANN 9.0.0 |

## Directory Structure

```txt
├── data_sync_barrier
│   ├── scripts
│   │   ├── gen_data.py            // Script for generating input data and ground truth data
│   │   └── verify_result.py       // Script for verifying whether output data matches ground truth data
│   ├── CMakeLists.txt             // Build project file
│   ├── data_utils.h               // Data read/write functions
│   ├── data_sync_barrier.asc      // Ascend C example implementation & invocation example
│   └── README.md                  // Example documentation
```

## Example Specifications

<table border="2">
<caption>Table 1: Example Specifications</caption>
<tr><th>Type</th><th>Name</th><th>Shape</th><th>Data Type</th><th>Format</th></tr>
<tr><td>Input</td><td>srcGm</td><td>[1, 8]</td><td>int32_t</td><td>ND</td></tr>
<tr><td>Output</td><td>dstGm</td><td>[1, 8]</td><td>int32_t</td><td>ND</td></tr>
<tr><td>Kernel Function Name</td><td colspan="4" style="text-align:center;">kernel_data_sync_barrier</td></tr>
</table>

## Example Description

The following describes the usage scenario of `DataSyncBarrier` in this example step by step:

1. The system has two AIV cores, denoted as Core 0 and Core 1. Two variables `x` and `y` in GM both have an initial value of 1.
2. Core 0 first writes `x=7` to `srcGm[1]` through the scalar pipeline interface `WriteGmBypassDCache`.
3. Core 0 then inserts `DataSyncBarrier<AscendC::MemDsbT::DDR>()` to wait for the previous GM write operation to complete.
4. Core 0 subsequently writes `y=6` to `srcGm[0]`.
5. Core 1 continuously polls `srcGm[0]` until it reads `y=6`, then reads `srcGm[1]` and writes `2 * x` to the output.

Expected behavior:

- When Core 1 reads `y=6`, `x=7` must have already been written back to GM.
- Therefore, the `x` value read by Core 1 should be 7, and the final output should be 14.

Without synchronization, the scalar pipeline does not guarantee the order of the two GM writes. It is possible that `y` has been updated while `x` has not yet completed writing. In this case, even though Core 1 has already observed `y=6`, it may read an incorrect `x` value.

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
  python3 ../scripts/gen_data.py   # Generate test input data
  ./demo                           # Run the compiled executable to execute the example
  python3 ../scripts/verify_result.py output/output.bin output/golden.bin  # Verify whether the output result is correct
  ```

  To use CPU debug mode, add the `-DCMAKE_ASC_RUN_MODE=cpu` parameter.

  Example:

  ```bash
  cmake .. -DCMAKE_ASC_RUN_MODE=cpu -DCMAKE_ASC_ARCHITECTURES=dav-2201;make -j; # CPU debug mode
  ```

  > **Notice:** Clear the cmake cache before switching build modes. Run `rm CMakeCache.txt` in the build directory and then re-run cmake.

- Build option description

  | Option | Values | Description |
  |--------|--------|-------------|
  | `CMAKE_ASC_RUN_MODE` | `npu` (default), `cpu` | Run mode: NPU execution, CPU debug |
  | `CMAKE_ASC_ARCHITECTURES` | `dav-2201` (default), `dav-3510` | NPU architecture: dav-2201 corresponds to Atlas A2 Training Series Products/Atlas A2 Inference Series Products and Atlas A3 Training Series Products/Atlas A3 Inference Series Products, dav-3510 corresponds to Ascend 950PR/Ascend 950DT |

- Execution result

  The following execution result indicates that the accuracy comparison is successful.

  ```bash
  test pass!
  ```
