# Mutex Intra-Core Pipeline Synchronization Example

## Overview

This example demonstrates the usage of the Mutex::Lock, Mutex::Unlock, AllocMutexID, and ReleaseMutexID intra-core pipeline synchronization interfaces. The example first obtains a MutexID from the framework through AllocMutexID, then uses Mutex::Lock and Mutex::Unlock to lock and release specified pipelines to achieve synchronization dependencies between PIPE_MTE2, PIPE_V, and PIPE_MTE3 asynchronous pipelines. The example implements data transfer-in, addition computation, and data transfer-out tasks, using double buffering and Mutex lock mechanisms for pipeline synchronization control, and finally uses ReleaseMutexID to release the MutexID.

## Supported Products and CANN Versions

| Product | CANN Version |
|---------|-------------|
| Ascend 950PR/Ascend 950DT | >= CANN 9.1.0 |

## Directory Structure

```
├── mutex
│   ├── scripts
│   │   ├── gen_data.py         // Script for generating input data and ground truth data
│   │   └── verify_result.py    // Script for verifying whether output data matches ground truth data
│   ├── CMakeLists.txt          // Build project file
│   ├── data_utils.h            // Data read/write functions
│   ├── mutex.asc               // Ascend C example implementation & invocation example
│   └── README.md               // Example documentation
```

## Example Description

- Example function:
  This example demonstrates the complete usage flow of Mutex-related interfaces:
  1. Use `AllocMutexID()` to obtain two MutexIDs (mutexId0 and mutexId1) from the framework
  2. The input data volume is 1024 * 1024, which cannot fit into UB at once, so it is processed in tiles
  3. In the loop, a double buffering mechanism is used, alternating between two buffers (mutexId0 and mutexId1)
  4. Each tile sequentially locks and unlocks the three tasks: transfer-in, computation, and transfer-out
  5. After use, call `ReleaseMutexID()` to release the two MutexIDs

  The Mutex lock mechanism achieves synchronization between asynchronous pipelines within a core, ensuring the correct execution order of the three tasks: data transfer-in, computation, and transfer-out.

- Example specifications:
  <table border="2">
  <caption>Table 1: Example Specifications</caption>
  <tr><td rowspan="1" align="center">Example Type (OpType)</td><td colspan="4" align="center">Mutex</td></tr>
  <tr><td rowspan="3" align="center">Example Input</td><td align="center">name</td><td align="center">shape</td><td align="center">data type</td><td align="center">format</td></tr>
  <tr><td align="center">x</td><td align="center">[1024, 1024]</td><td align="center">float</td><td align="center">ND</td></tr>
  <tr><td align="center">y</td><td align="center">[1024, 1024]</td><td align="center">float</td><td align="center">ND</td></tr>
  <tr><td rowspan="1" align="center">Example Output</td><td align="center">z</td><td align="center">[1024, 1024]</td><td align="center">float</td><td align="center">ND</td></tr>
  <tr><td rowspan="1" align="center">Kernel Function Name</td><td colspan="4" align="center">mutex_custom</td></tr>
  </table>

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
  cmake -DCMAKE_ASC_ARCHITECTURES=dav-3510 ..;make -j;    # Build the project
  python3 ../scripts/gen_data.py   # Generate test input data
  ./demo                           # Run the compiled executable to execute the example
  python3 ../scripts/verify_result.py output/output.bin output/golden.bin  # Verify whether the output result is correct
  ```

  To use CPU debug or NPU simulation mode, add the `-DCMAKE_ASC_RUN_MODE=cpu` or `-DCMAKE_ASC_RUN_MODE=sim` parameter.

  Examples:

  ```bash
  cmake -DCMAKE_ASC_RUN_MODE=cpu -DCMAKE_ASC_ARCHITECTURES=dav-3510 ..;make -j; # CPU debug mode
  cmake -DCMAKE_ASC_RUN_MODE=sim -DCMAKE_ASC_ARCHITECTURES=dav-3510 ..;make -j; # NPU simulation mode
  ```

  > **Notice:** Clear the cmake cache before switching build modes. Run `rm CMakeCache.txt` in the build directory and then re-run cmake.

- Build option description

  | Option | Values | Description |
  |--------|--------|-------------|
  | `CMAKE_ASC_RUN_MODE` | `npu` (default), `cpu`, `sim` | Run mode: NPU execution, CPU debug, NPU simulation |
  | `CMAKE_ASC_ARCHITECTURES` | `dav-3510` (default) | NPU architecture: corresponds to Ascend 950PR/Ascend 950DT |

- Execution result

  The following execution result indicates that the accuracy comparison is successful.

  ```bash
  test pass!
  ```
